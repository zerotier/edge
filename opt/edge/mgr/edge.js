// (c)2018 ZeroTier, Inc.

const toml = require('toml');
const fs = require('fs');
const deepequal = require('deepequal');
const async = require('async');
const ipaddr = require('ipaddr.js');
const spawn = require('child_process').spawn;

const ztDaemonQuery = require('./ztdaemon.js')
const RouterListener = require('./router-listener.js');
const VirtualUplink = require('./virtual-uplink.js');
const cmd = require('./cmd.js');

const config = require('/etc/edge/mgr-config.js');

const REFRESH_INTERVAL = 5000;

var exiting = false;        // set to true if an exit signal has been received

var physical = {};          // physical port configurations by [physical port]
var virtual = {};           // virtual networks by [network ID]

var uplinks = {};           // virtual uplinks by [physical port] -> [router] -> uplink info
var routerListeners = {};   // router listeners by [physical port]

var physMac = {};           // MACs of physical interfaces by [interface name]
var wlanConfig = {};        // wireless ports and their most recent configs
var wlanDaemons = {};       // wireless sub-processes by interface name

function deepEqualWithout(x,y,without)
{
	let a = {};
	let b = {};
	if (x) {
		for(let k in x) {
			if (!without[k])
				a[k] = x[k];
		}
	} else {
		a = x;
	}
	if (y) {
		for(let k in y) {
			if (!without[k])
				b[k] = y[k];
		}
	} else {
		b = y;
	}
	return deepequal(a,b);
}

function sanitizeField(s)
{
	let r = '';
	if ((s)&&(typeof s === 'string')) {
		for(let i=0;i<s.length;i++) {
			let c = s.charAt(i);
			if ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.:/0123456789_-%".indexOf(c) >= 0) {
				r += c;
			}
		}
	}
	return r;
}

function updateBridgeTables(done)
{
	cmd.get('ebtables -F',() => {
		async.eachSeries(Object.keys(physical),(physDev,nextPhysDev) => {
			let mac = physMac[physDev];

			let prohibit = physical[physDev].prohibit;
			if (!Array.isArray(prohibit))
				prohibit = ['dhcp4','dhcp6','ipv6-ra'];
			let ptmp = {};
			for(let i=0;i<prohibit.length;i++) {
				ptmp[prohibit[i]] = true;
			}
			let parr = prohibit;
			prohibit = ptmp;

			console.log('Updating bridge tables for '+physDev+' ('+mac+') to prohibit: '+JSON.stringify(parr));

			if (!mac) {
				console.log('WARNING: MAC address not known for '+physDev);
				mac = '02:ff:ff:ff:ff:ff';
			}

			async.series([
				// These block general bridge forwarding of DHCP, DHCP6, or SLAAC if it is not desired.
				// Sometimes you don't want local devices getting DHCP from off-site, but then again
				// sometimes you do.
				(next) => {
					if (prohibit['dhcp4'])
						cmd.get('ebtables -A FORWARD --logical-in '+physDev+' --protocol ipv4 --ip-protocol udp --ip-destination-port 67:68 -j DROP',() => { next(); });
					else return next(null);
				},(next) => {
					if (prohibit['dhcp4'])
						cmd.get('ebtables -A FORWARD --logical-in '+physDev+' --protocol ipv4 --ip-protocol udp --ip-source-port 67:68 -j DROP',() => { next(); });
					else return next(null);
				},(next) => {
					if (prohibit['dhcp6'])
						cmd.get('ebtables -A FORWARD --logical-in '+physDev+' --protocol ipv6 --ip6-protocol udp --ip6-destination-port 546:547 -j DROP',() => { next(); });
					else return next(null);
				},(next) => {
					if (prohibit['dhcp6'])
						cmd.get('ebtables -A FORWARD --logical-in '+physDev+' --protocol ipv6 --ip6-protocol udp --ip6-source-port 546:547 -j DROP',() => { next(); });
					else return next(null);
				},(next) => {
					if (prohibit['ipv6-ra'])
						cmd.get('ebtables -A FORWARD --logical-in '+physDev+' --protocol ipv6 --ip6-protocol ipv6-icmp --ip6-icmp-type router-advertisement -j DROP',() => { next(); });
					else return next(null);
				},

				// These block UDP traffic that originates (or is destined to) the Edge itself
				// from being forwarded over the bridge or out any of the non-physical bridge
				// ports. This prevents ZeroTier, DHCP, or anything else from looping or leaking
				// packets that have no importance outside the local network.
				(next) => {
					cmd.get('ebtables -A INPUT -i Pzt+ -d '+mac+' --protocol ipv4 --ip-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A OUTPUT -o Pzt+ -s '+mac+' --protocol ipv4 --ip-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A FORWARD -s '+mac+' --protocol ipv4 --ip-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A FORWARD -d '+mac+' --protocol ipv4 --ip-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A INPUT -i Pzt+ -d '+mac+' --protocol ipv6 --ip6-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A OUTPUT -o Pzt+ -s '+mac+' --protocol ipv6 --ip6-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A FORWARD -s '+mac+' --protocol ipv6 --ip6-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A FORWARD -d '+mac+' --protocol ipv6 --ip6-protocol udp -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A INPUT -i Pzt+ -d '+mac+' --protocol ipv6 --ip6-protocol ipv6-icmp --ip6-icmp-type router-advertisement -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A OUTPUT -o Pzt+ -s '+mac+' --protocol ipv6 --ip6-protocol ipv6-icmp --ip6-icmp-type router-advertisement -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A FORWARD -s '+mac+' --protocol ipv6 --ip6-protocol ipv6-icmp --ip6-icmp-type router-advertisement -j DROP',() => { next(); });
				},(next) => {
					cmd.get('ebtables -A FORWARD -d '+mac+' --protocol ipv6 --ip6-protocol ipv6-icmp --ip6-icmp-type router-advertisement -j DROP',() => { next(); });
				}
			],nextPhysDev);
		},() => {
			if (done)
				done();
		});
	});
}

function leaveVirtual(nwid,done)
{
	console.log('Leaving virtual network '+nwid);
	ztDaemonQuery('/network/'+nwid,null,(err,result) => {
		if ((!err)&&(result)) {
			let virtualPort = sanitizeField(result.portDeviceName);
			if (virtualPort) {
				let v = virtual[nwid];
				async.eachSeries(Object.keys(v.physical),(physDev,nextPhysDev) => {
					cmd.get('ip link del Z'+virtualPort+physDev,() => {
						return nextPhysDev();
					});
				},() => {
					cmd.get('ip link del B'+virtualPort,() => {
						ztDaemonQuery('/network/'+nwid,false,() => {
							if (done)
								return done(null);
						});
					});
				});
			} else {
				ztDaemonQuery('/network/'+nwid,false,() => {
					if (done)
						return done(null);
				});
			}
		} else {
			if (done)
				return done(null);
		}
	});
}

function assignZtAddresses(ifname,config,done)
{
	async.eachSeries(config.assignedAddresses||[],(addr,nextAddr) => {
		if (addr) {
			if (addr.indexOf(':') < 0) {
				cmd.get('ip -4 addr add '+addr+' dev '+ifname,() => { nextAddr(); });
			} else {
				cmd.get('ip -6 addr add '+addr+' dev '+ifname,() => { nextAddr(); });
			}
		} else return nextAddr(null);
	},(err) => {
		if (err) {
			if (done)
				return done(err);
			else return;
		}
		async.eachSeries(config.routes||[],(route,nextRoute) => {
			return nextRoute(null);
		},(err) => {
			if (done)
				return done(err);
		});
	});
}

function refreshVirtual(done)
{
	if (!done)
		done = function(){};

	ztDaemonQuery('/network/'+config.serviceNetwork,{
		allowManaged: true,
		allowGlobal: false,
		allowDefault: false
	},() => {
		ztDaemonQuery('/network',null,(err,joined) => {
			if (Array.isArray(joined)) {
				for(let i=0;i<joined.length;i++) {
					if ((joined[i])&&(joined[i].id)&&(joined[i].id != config.serviceNetwork)) {
						let v = virtual[joined[i].id];
						if (!v) {
							leaveVirtual(joined[i].id);
						}
					}
				}
			}

			async.eachSeries(Object.keys(virtual),(nwid,nextNwid) => {
				let v = virtual[nwid];
				ztDaemonQuery('/network/'+nwid,{
					allowManaged: false,
					allowGlobal: false,
					allowDefault: false
				},(_err,config) => {
					if ((config)&&(config.id == nwid)) {
						let virtualPort = config.portDeviceName;
						let mtu = config.mtu||2800;

						async.eachSeries(Object.keys(v.physical),(physDev,nextPhysDev) => {
							let p = v.physical[physDev];
							let pairName = virtualPort+physDev.charAt(0)+physDev.charAt(physDev.length - 1);
							let physBr = physDev;
							if (wlanConfig[physDev])
								physBr = physDev+'br'; // wlans in AP mode get a bridge, while phy# ports are always bridges

							if (p.layer == 3) {
								cmd.get('ip link del Z'+pairName,() => {
									cmd.get('ip link del B'+virtualPort,() => {
										assignZtAddresses(virtualPort,config,nextPhysDev);
									});
								});
							} else {
								async.series([
									(next) => { cmd.get('brctl addbr B'+virtualPort,() => { next(); }); },
									(next) => { cmd.get('brctl stp B'+virtualPort+' off',() => { next(); }); },
									(next) => { cmd.get('ip link add Z'+pairName+' type veth peer P'+pairName,() => { next(); }); },
									(next) => { cmd.get('sysctl -w net.ipv6.conf.Z'+pairName+'.disable_ipv6=1',() => { next(); }); },
									(next) => { cmd.get('sysctl -w net.ipv6.conf.P'+pairName+'.disable_ipv6=1',() => { next(); }); },
									(next) => { cmd.get('ip link set Z'+pairName+' mtu '+mtu+' up',() => { next(); }); },
									(next) => { cmd.get('ip link set P'+pairName+' mtu '+mtu+' up',() => { next(); }); },
									(next) => { cmd.get('ip link set B'+virtualPort+' address '+config.mac+' mtu '+mtu+' up',() => { next(); }); },
									(next) => { cmd.get('brctl addif B'+virtualPort+' '+virtualPort,() => { next(); }); },
									(next) => { cmd.get('brctl addif B'+virtualPort+' Z'+pairName,() => { next(); }); },
									(next) => { cmd.get('brctl addif '+physBr+' P'+pairName,() => { next(); }); },
									(next) => { assignZtAddresses('B'+virtualPort,config,() => { next(); }); }
								],nextPhysDev);
							}
						},nextNwid);
					} else {
						v.config = config;
						return nextNwid(null);
					}
				});
			},done);
		});
	});
}

function updateUplinks(done)
{
	for (let physDev in physical) {
		let p = physical[physDev];
		if (p.name) {
			let rl = routerListeners[p.name];
			let ap = (wlanConfig[physDev])&&(p.mode == 'ap');
			if (rl) {
				if (Array.isArray(p.autoconf)) {
					rl.setDhcp4Enabled((p.autoconf.indexOf('dhcp4') >= 0)&&(!ap));
					rl.setDhcp6Enabled((p.autoconf.indexOf('dhcp6') >= 0)&&(!ap));
					rl.setSlaacEnabled((p.autoconf.indexOf('slaac') >= 0)&&(!ap));
				}
			}
		}
	}
	if (done)
		return done(null);
}

function updateWirelessPorts(done)
{
	for (let physDev in wlanConfig) {
		let p = physical[physDev];
		let essid = sanitizeField((p.essid||'ZeroTierEdge').trim());
		let channel = 40;
		let mode = (p.mode == 'ap') ? 'ap' : 'client';

		if (!deepequal(wlanConfig[physDev],p)) {
			if (wlanDaemons[physDev]) {
				wlanDaemons[physDev].kill('SIGTERM');
				wlanDaemons[physDev] = null;
			}

			if (mode == 'ap') {
				// Source of channel+6 magic:
				// http://blog.fraggod.net/2017/04/27/wifi-hostapd-configuration-for-80211ac-networks.html

				console.log('Configurating wireless port '+physDev+' for AP mode');
				fs.writeFileSync('/dev/shm/hostap-psk-'+physDev,'00:00:00:00:00:00 '+p.psk||'');
				fs.writeFileSync('/dev/shm/hostap-'+physDev+'.conf',
`interface=`+physDev+`
logger_syslog=0
logger_syslog_level=4
logger_stdout=-1
logger_stdout_level=2
driver=nl80211
ctrl_interface=/var/run/hostapd-`+physDev+`
ssid2="`+essid+`"
hw_mode=a
channel=`+channel+`
beacon_int=250
dtim_period=2
max_num_sta=2007
rts_threshold=-1
fragm_threshold=-1
macaddr_acl=0
auth_algs=3
ap_isolate=0
eap_server=0

wmm_enabled=1
wmm_ac_bk_cwmin=4
wmm_ac_bk_cwmax=10
wmm_ac_bk_aifs=7
wmm_ac_bk_txop_limit=0
wmm_ac_bk_acm=0
wmm_ac_be_aifs=3
wmm_ac_be_cwmin=4
wmm_ac_be_cwmax=10
wmm_ac_be_txop_limit=0
wmm_ac_be_acm=0
wmm_ac_vi_aifs=2
wmm_ac_vi_cwmin=3
wmm_ac_vi_cwmax=4
wmm_ac_vi_txop_limit=94
wmm_ac_vi_acm=0
wmm_ac_vo_aifs=2
wmm_ac_vo_cwmin=2
wmm_ac_vo_cwmax=3
wmm_ac_vo_txop_limit=47
wmm_ac_vo_acm=0

ieee80211n=1
ieee80211ac=1
vht_oper_chwidth=0

wpa=2
wpa_psk_file=/dev/shm/hostap-psk-`+physDev+`
wpa_key_mgmt=WPA-PSK WPA-PSK-SHA256
wpa_pairwise=TKIP CCMP
rsn_pairwise=CCMP
`);

				cmd.get('ip link set '+physDev+' down',() => {
					cmd.get('ip link set '+physDev+' up',() => {
						wlanDaemons[physDev] = spawn('/usr/sbin/hostapd',['/dev/shm/hostap-'+physDev+'.conf'],{
							stdio: 'inherit'
						});

						/* For AP mode we have to create a bridge that will be used to connect virtual interfaces */
						async.series([
							(next) => {
								cmd.get('sysctl -w net.ipv6.conf.'+physDev+'.disable_ipv6=1',() => { next(); });
							},
							(next) => {
								cmd.get('brctl addbr '+physDev+'br',() => { next(); });
							},
							(next) => {
								cmd.get('brctl stp '+physDev+'br off',() => { next(); });
							},
							(next) => {
								cmd.get('brctl addif '+physDev+'br '+physDev,() => { next(); });
							},
							(next) => {
								cmd.get('ip link set '+physDev+'br address '+physMac[physDev]+' up',() => { next(); });
							}
						],() => {
						});
					});
				});

			} else { // mode == client

				console.log('Configuring wireless port '+physDev+' for client mode');
				let psk = '';
				if (typeof p.psk === 'string') {
					for(let i=0;i<p.psk.length;i++) {
						let c = p.psk.charAt(i);
						if ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]{}_0123456789.-+=!@#$%^&*()~".indexOf(c) >= 0)
							psk += c;
					}
				}
				fs.writeFileSync('/dev/shm/wpa_supplicant-'+physDev+'.conf',
`ap_scan=1
device_name=ZeroTier Edge
manufacturer=ZeroTier, Inc.
model_name=Edge
model_number=0
serial_number=0
device_type=6-0050F204-1
p2p_disabled=1
network={
	ssid="`+essid+`"
	scan_ssid=1
	key_mgmt=WPA-PSK
	group=CCMP TKIP WEP104 WEP40
	psk="`+psk+`"
}
`);

				/* For client mode we delete any bridges and reset port config to normal. */
				cmd.get('ip link del '+physDev+'br',() => {
					cmd.get('ip link set '+physDev+' down',() => {
						cmd.get('sysctl -w net.ipv6.conf.'+physDev+'.disable_ipv6=0',() => {
							cmd.get('ip link set '+physDev+' up',() => {
								wlanDaemons[physDev] = spawn('/sbin/wpa_supplicant',['-i'+physDev,'-Dnl80211','-c/dev/shm/wpa_supplicant-'+physDev+'.conf'],{
									stdio: 'inherit'
								});
							});
						});
					});
				});

			}

			wlanConfig[physDev] = p;
		}
	}
	return setTimeout(done,250);
}

var previousConfigText = ''; // used to store config text to detect changes
function refreshConfig()
{
	if (exiting) {
		return;
	}
	try {
		let tmp = fs.readFileSync(config.home + 'config.toml').toString();
		if ((tmp)&&(previousConfigText !== tmp)) {
			previousConfigText = tmp;
			let ec = toml.parse(tmp)||null;
			if (ec) {
				let physicalList = ec.physical;

				let newPhysical = {};
				let newVirtual = {};

				if (Array.isArray(physicalList)) {
					for(let ii=0;ii<physicalList.length;++ii) {
						let p = physicalList[ii];
						if ((p)&&(p.name)) {
							newPhysical[p.name] = p;
							if ((typeof p.virtual === 'string')&&(p.virtual.length === 16)&&(p.mode != 'ap')) {
								let v = virtual[p.virtual];
								if (!v) {
									v = {
										physical: {},
										config: null
									};
								}
								v.physical[p.name] = p;
								newVirtual[p.virtual] = v;
							}
						}
					}
				}

				for(let nwid in newVirtual) {
					let v = newVirtual[nwid];
					for(let physDev in v.physical) {
						if ((!newPhysical[physDev])||(newPhysical[physDev].virtual != nwid))
							delete v.physical[physDev];
					}
					if (Object.keys(v.physical).length === 0) {
						delete newVirtual[nwid];
					}
				}

				if (!deepequal(physical,newPhysical)) {
					physical = newPhysical;
					virtual = newVirtual;

					async.series([updateBridgeTables,updateWirelessPorts,updateUplinks],(err) => {
						if (err) {
							console.log('ERROR: unexpected top-level error applying configuration: '+err);
						}
						setTimeout(refreshVirtual,250);
					});
				} else {
					refreshVirtual();
				}
			} else {
				refreshVirtual();
			}
		} else {
			refreshVirtual();
		}
	} catch (e) {
		console.log('WARNING: malformed TOML or other config error: '+e.toString());
	}
}

function initializeInterface(ifname)
{
	cmd.get(__dirname+'/utils/get-if-mac.sh '+ifname,(err,stdout,stderr) => {
		if (stdout) {
			addr = stdout.toString().trim();
			if (!addr) {
				console.log('ERROR: unable to get '+ifname+' address: not found!');
			} else {
				physMac[ifname] = addr;
				cmd.get('ip link set '+ifname+' address '+addr,() => {}); /* stop bridges from rotating MACs */
			}
		} else {
			console.log('ERROR: unable to get '+ifname+' address: not found!');
		}
	});

	routerListeners[ifname] = new RouterListener(ifname,(_routerListener,routers,_update) => {
		if (exiting)
			return;
		for(let physDev in routers) {
			let portUplinks = uplinks[physDev];
			let routersByDev = routers[physDev];
			if (!portUplinks) {
				portUplinks = {};
				uplinks[physDev] = portUplinks;
			}

			if ((routerListeners[physDev])&&(routersByDev)) {
				for (let routerIp in routersByDev) {
					let router = routersByDev[routerIp];
					if ((router)&&(router.router)&&(ipaddr.isValid(routerIp))&&(ipaddr.isValid(router.ip))) {
						let uplink = portUplinks[routerIp];
						if ((!uplink)||(!deepEqualWithout(uplink.router,router,{timestamp:true,lease:true}))) {
							if ((uplink)&&(uplink.vport))
								uplink.vport.shutdown();
							portUplinks[routerIp] = {
								isStatic: false,
								router: router,
								vport: new VirtualUplink(
									(router.router.indexOf(':') > 0) ? 6 : 4,
									sanitizeField(physDev),
									sanitizeField(router.ip),
									sanitizeField(router.subnet),
									null,
									sanitizeField(router.router))
							};
						}
					}
				}

				for(let routerIp in portUplinks) {
					if (!routersByDev[routerIp]) {
						let uplink = portUplinks[routerIp];
						if ((uplink)&&(uplink.vport))
							uplink.vport.shutdown();
						delete portUplinks[routerIp];
					}
				}
			}
		}
	},false,false,false); // initially start with all modes disabled, enable/disable in syncUplinks() from config
}

function initialize()
{
	let wired = config.wired;
	if (wired) {
		for(let i=0;i<wired.length;i++) {
			console.log('Initializing wired port: '+wired[i]);
			initializeInterface(wired[i]);
		}
	}
	let wireless = config.wireless;
	if (wireless) {
		for(let i=0;i<wireless.length;i++) {
			console.log('Initializing wireless port: '+wireless[i]);
			initializeInterface(wireless[i]);
			wlanConfig[wireless[i]] = null;
			wlanDaemons[wireless[i]] = null;
		}
	}
}

function onExit()
{
	if (exiting)
		return;
	exiting = true;

	console.log('Shutting down...');

	for(let physDev in wlanDaemons) {
		if (wlanDaemons[physDev])
			wlanDaemons[physDev].kill('SIGTERM');
		cmd.get('ip link del '+physDev+'br',() => {}); // destroy any AP mode bridge devices
	}
	for(let physDev in physical) {
		let p = physical[physDev];
		if ((p.virtual)&&(p.virtual.length === 16)) {
			leaveVirtual(p.virtual);
		}
	}

	setTimeout(() => {
		async.eachSeries(Object.keys(routerListeners),(rl,nextrl) => {
			console.log('shutting down rl: '+rl);
			if (routerListeners[rl])
				routerListeners[rl].shutdown();
			return nextrl();
		},() => {
			setTimeout(() => {
				async.eachSeries(Object.keys(uplinks),(physDev,nextUplink) => {
					async.eachSeries(Object.keys(uplinks[physDev]),(routerIp,nextPortUplink) => {
						let uplink = uplinks[physDev][routerIp];
						if ((uplink)&&(uplink.vport))
							uplink.vport.shutdown(nextPortUplink);
						else return nextPortUplink(null);
					},nextUplink);
				},() => {
					setTimeout(() => {
						process.exit(0);
					},1000);
				});
			},500);
		});
	},500);
}

process.on('exit',onExit);
process.on('SIGTERM',onExit);
process.on('SIGQUIT',onExit);
process.on('SIGINT',onExit);
process.on('SIGQUIT',onExit);

process.on('uncaughtException',(err) => { console.error('WARNING: uncaught exception: '+err.toString()); });

initialize();
setInterval(refreshConfig,REFRESH_INTERVAL);
