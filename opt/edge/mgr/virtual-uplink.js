// (c)2018 ZeroTier, Inc.

const ipaddr = require('ipaddr.js');
const crypto = require('crypto');

const cmd = require('./cmd.js');

const IF_CHARS = 'abcdefghijklmnopqrstuvwxyz0123456789';

// These are used to assign unique route table and metric numbers.
var routeTableCounter = 10;
var metricCounter = 10;

/**
 * VirtualUplink is an IPVLAN virtual uplink port to a physical router/gateway or other network link.
 * 
 * This allows more than one router to be connected via a hub/switch to one port of the Edge and to
 * be independently DHCP'd and addressed. It works in tandem with RouterListener to achieve this.
 */
class VirtualUplink
{
	/**
	 * @param {number} ip4or6 4 or 6
	 * @param {string} ifname Physical interface e.g. phy0
	 * @param {string} ip IP address to assign to THIS device on uplink to physical interface
	 * @param {string} subnet Subnet as CIDR form or number of bits
	 * @param {string} target Route target or NULL for default route (0.0.0.0 or ::0)
	 * @param {string} router Route gateway or NULL/empty for none (local LAN)
	 */
	constructor(ip4or6,ifname,ip,subnet,target,router)
	{
		// Compute a constant canonical name for the IPVlan interface that will uplink to this router
		let sha = crypto.createHash('sha512');
		sha.update(ifname + (router||'~') + ip);
		let ifSha = sha.digest();
		let ifHash = '';
		for (let i=0;i<4;++i) {
			ifHash += IF_CHARS.charAt(ifSha.readUInt8(i) % IF_CHARS.length);
		}

		this.ip4or6 = ip4or6;
		this.ifname = ifname;
		this.ip = ip;
		this.subnet = subnet;
		this.target = target;
		this.router = router;
		this.ipVlanIfName = 'V'+ifname+'R'+ifHash;
		this.routeTable = (routeTableCounter++).toString();

		let viaRouter = '';
		if (router) {
			let pctIdx = router.indexOf('%');
			if (pctIdx > 0)
				router = router.substr(0,pctIdx);
			viaRouter = ' via '+router+((router.substr(0,4) === 'fe80') ? ' scope link' : '');
		}

		cmd.get('ip link del '+this.ipVlanIfName,() => { cmd.get('ip -a rule del table '+this.routeTable,() => { cmd.get('ip -a -6 rule del table '+this.routeTable,() => {
			cmd.get('ip link add '+this.ipVlanIfName+' link '+ifname+' type ipvlan mode l2',(err,stdout,stderr) => { setTimeout(() => {

				if ((err)||(stderr)) {
					// TODO: smarter error handling
					console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip link add, link == '+ifname+') '+this.ipVlanIfName+': '+stderr);
				} else {
					if ((ip4or6 === 4)&&(ip)&&(subnet)) {
						// IPv4 ------------------------------------------------------------------------------

						cmd.get('sysctl -w net.ipv6.conf.'+this.ipVlanIfName+'.disable_ipv6=1',() => { cmd.get('ip -4 addr add '+ip+'/'+subnet+' dev '+this.ipVlanIfName,(err,stdout,stderr) => {
							if ((err)||(stderr)) {
								console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip addr add) '+this.ipVlanIfName+': '+stderr);
								return
							}

							cmd.get('ip link set '+this.ipVlanIfName+' up',(err,stdout,stderr) => {
								if ((err)||(stderr)) {
									console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip link set up) '+this.ipVlanIfName+': '+stderr);
									return
								}

								cmd.get('ip -4 route add '+((target) ? target : '0.0.0.0/0')+viaRouter+' dev '+this.ipVlanIfName+' table '+this.routeTable,(err,stdout,stderr) => {
									if ((err)||(stderr)) {
										console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip route add) '+this.ipVlanIfName+': '+stderr);
										return;
									}

									let ipParsed = null;
									let subnetParsed = null;
									try {
										ipParsed = ipaddr.parse(ip).octets;
										if ((typeof subnet === 'string')&&(subnet.indexOf('.') > 0)) {
											subnetParsed = ipaddr.parse(subnet).octets;
										} else {
											subnetParsed = [ 0,0,0,0 ];
											let sn = parseInt(subnet);
											let snMask = 0xffffffff >>> (32 - sn);
											for(let i=0;i<4;i++) {
												subnetParsed[i] = (snMask & 0xff);
												snMask >>>= 8;
											}
										}
									} catch (e) {
										console.log('ERROR: invalid IP or subnet from router on '+ifname+': '+subnet||'(null)');
										subnetParsed = null;
									}

									if ((Array.isArray(subnetParsed))&&(subnetParsed.length === 4)&&(Array.isArray(ipParsed))&&(ipParsed.length === 4)) {
										let network = (ipParsed[0] & subnetParsed[0])+'.'+(ipParsed[1] & subnetParsed[1])+'.'+(ipParsed[2] & subnetParsed[2])+'.'+(ipParsed[3] & subnetParsed[3]);
										cmd.get('ip -4 route add '+network+'/'+subnet+' dev '+this.ipVlanIfName+' table '+this.routeTable+' priority 100',() => {
											cmd.get('ip -4 rule add from '+ip+'/32 table '+this.routeTable+' priority 100',() => {
												cmd.get('ip -4 rule add to '+ip+'/32 table '+this.routeTable+' priority 100',() => {
													cmd.get('ip -4 route flush cache',() => {
														if ((!target)&&(viaRouter)) {
															cmd.get('ip route add 0.0.0.0/0'+viaRouter+' metric '+(metricCounter++)+' dev '+this.ipVlanIfName,() => {});
														}
													});
												});
											});
										});
									}
								});
							});
						}); });

						// -----------------------------------------------------------------------------------
					} else if ((ip4or6 === 6)&&(ip)&&(subnet)) {
						// IPv6 ------------------------------------------------------------------------------

						let subnetBits = subnet.split('/');
						try {
							if (subnetBits.length > 1)
								subnetBits = parseInt(subnetBits[1]);
							else subnetBits = 64; // 64 is the RFC default if unspecified
						} catch (e) {
							subnetBits = 64; // this can happen if subnet bits is something invalid or empty, in which case we default to RFC default /64
						}

						if (typeof ip === 'string') {
							if (ip.indexOf('/') > 0)
								ip = ip.substr(0,ip.indexOf('/'));
						}

						// Enable IPv6 but disable autoconf and RA since we assion addresses manually.
						cmd.get('sysctl -w net.ipv6.conf.'+this.ipVlanIfName+'.disable_ipv6=0',() => { cmd.get('sysctl -w net.ipv6.conf.'+this.ipVlanIfName+'.autoconf=0',() => { cmd.get('sysctl -w net.ipv6.conf.'+this.ipVlanIfName+'.accept_ra=0',() => {
							cmd.get('ip -6 addr add '+ip+'/'+subnetBits+' dev '+this.ipVlanIfName,(err,stdout,stderr) => {
								if ((err)||(stderr)) {
									console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip addr add) '+this.ipVlanIfName+': '+stderr);
								} else {
									cmd.get('ip link set '+this.ipVlanIfName+' up',(err,stdout,stderr) => {
										if ((err)||(stderr)) {
											console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip link set up) '+this.ipVlanIfName+': '+stderr);
										} else {
											cmd.get('ip -6 route add '+((target) ? target : '::0/0')+viaRouter+' dev '+this.ipVlanIfName+' table '+this.routeTable,(err,stdout,stderr) => {
												if ((err)||(stderr)) {
													console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip route add) '+this.ipVlanIfName+': '+stderr);
												} else {
													cmd.get('ip -6 rule add from '+ip+'/128 table '+this.routeTable+' priority 100',(err,stdout,stderr) => {
														if ((err)||(stderr)) {
															console.log('ERROR: unable to configure ipvlan WAN uplink interface (ip route add) '+this.ipVlanIfName+': '+stderr);
														} else {
															cmd.get('ip -6 route flush cache',() => {});
														}
													});
												}
											});
										}
									});
								}
							});
						}); }); });

						// -----------------------------------------------------------------------------------
					}
				}
			},250); });
		}); }); });
	}

	_shutdown(done)
	{
		console.log('delete '+this.ipVlanIfName);
		cmd.get('ip link del '+this.ipVlanIfName,() => {
			cmd.get('ip -a rule del table '+this.routeTable,() => {
				cmd.get('ip -a rule del table '+this.routeTable,() => {
					cmd.get('ip -a -6 rule del table '+this.routeTable,() => {
						cmd.get('ip -a -6 rule del table '+this.routeTable,() => {
							cmd.get('ip route flush cache',() => {
								cmd.get('ip -6 route flush cache',() => {
									if (done)
										process.nextTick(done);
								});
							});
						});
					});
				});
			});
		});
	}

	shutdown(done)
	{
		this._shutdown(() => {
			this._shutdown(done);
		});
	}
};

module.exports = VirtualUplink;
