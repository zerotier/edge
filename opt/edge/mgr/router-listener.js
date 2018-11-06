// (c)2018 ZeroTier, Inc.

const spawn = require('child_process').spawn;
const ipaddr = require('ipaddr.js');
const crypto = require('crypto');

const cmd = require('./cmd.js');

const DHCP_VALID_FIELDS = [ 'interface','ip','subnet','router','dns','hostname','domain','ipttl','mtu','broadcast','lease','serverid' ];
const DHCP_REFRESH_FREQUENCY = 10;

/**
 * RouterListener listens for DHCP, DHCP6, and SLAAC (IPv6 RA) announcements and calls a callback when received.
 */
class RouterListener
{
	constructor(name,onUpdateCallback,dhcp4,dhcp6,slaac)
	{
		this._name = name; // device name on OS e.g. wan0
		this._onUpdateCallback = onUpdateCallback;
		this._run = true;
		this._ra = {};
		this._rdisc6Timer = null;
		this._udhcpc = null;
		this._udhcpc6 = null;
		this._rdisc6Timer = null;

		if (dhcp4)
			this._listenForDhcp4();
		if (dhcp6)
			this._listenForDhcp6();
		if (slaac)
			this._listenForIpv6Slaac_rdisc6();

		this._refreshTimer = setInterval(() => {
			let now = Date.now();
			for(let ifname in this._ra) {
				let ifr = this._ra[ifname];
				for(let router in ifr) {
					let m = ifr[router];
					if ((!m)||((now - m.timestamp) > 300000))
						delete ifr[router];
				}
				if (Object.keys(ifr).length === 0)
					delete this._ra[ifname];
			}
		},10000);
	}

	_stripType(type)
	{
		for(let ifname in this._ra) {
			let ifr = this._ra[ifname];
			for(let router in ifr) {
				let m = ifr[router];
				if ((!m)||(m.type == type)) {
					delete ifr[router];
				}
			}
			if (Object.keys(ifr).length === 0)
				delete this._ra[ifname];
		}
	}

	setDhcp4Enabled(enabled)
	{
		if ((enabled)&&(!this._udhcpc)) {
			this._listenForDhcp4();
		} else if ((!enabled)&&(this._udhcpc)) {
			this._udhcpc.kill('SIGKILL');
			this._udhcpc = null;
			this._stripType('dhcp4');
			if (this._onUpdateCallback)
				this._onUpdateCallback(this,this._ra,m);
		}
	}

	setDhcp6Enabled(enabled)
	{
		if ((enabled)&&(!this._udhcpc6)) {
			this._listenForDhcp6();
		} else if ((!enabled)&&(this._udhcpc6)) {
			this._udhcpc6.kill('SIGKILL');
			this._udhcpc6 = null;
			this._stripType('dhcp6');
			if (this._onUpdateCallback)
				this._onUpdateCallback(this,this._ra,m);
		}
	}

	setSlaacEnabled(enabled)
	{
		if ((enabled)&&(!this._rdisc6Timer)) {
			if (this._rdisc6Timer !== false) {
				this._listenForIpv6Slaac_rdisc6();
			}
		} else if ((!enabled)&&(this._rdisc6Timer !== null)) {
			if ((this._rdisc6Timer !== null)&&(this._rdisc6Timer !== false))
				clearTimeout(this._rdisc6Timer);
			this._rdisc6Timer = null;
			this._stripType('slaac');
			if (this._onUpdateCallback)
				this._onUpdateCallback(this,this._ra,m);
		}
	}

	shutdown()
	{
		if (this._run) {
			this._run = false;
			clearInterval(this._refreshTimer);
			if (this._udhcpc)
				this._udhcpc.kill('SIGKILL');
			if (this._udhcpc6)
				this._udhcpc6.kill('SIGKILL');
			if ((this._rdisc6Timer !== null)&&(this._rdisc6Timer !== false))
				clearTimeout(this._rdisc6Timer);
			this._rdisc6Timer = null;
		}
	}

	_update(m)
	{
		if (this._run) {
			let ifr = this._ra[m.interface];
			if (!ifr)
				this._ra[m.interface] = ifr = {};
			ifr[m.router] = m;
			if (this._onUpdateCallback)
				this._onUpdateCallback(this,this._ra,m);
		}
	}

	_listenForDhcp4()
	{
		this._udhcpc = spawn('udhcpc',[
			'-q',
			'-n',
			'-s',
			process.cwd() + '/utils/dhcp-dummy-handler',
			'-t',
			'4',
			'-T',
			Math.round(DHCP_REFRESH_FREQUENCY / 4).toString(),
			'-A',
			DHCP_REFRESH_FREQUENCY.toString(),
			'-i',
			this._name
		],{
			stdio: ['ignore','ipc','ignore']
		});

		var startTime = Date.now();
		this._udhcpc.on('exit',() => {
			let elapsed = Date.now() - startTime;
			//console.log('dhcp4 exit: '+this._name+' '+elapsed);
			let wtime = (elapsed < (DHCP_REFRESH_FREQUENCY * 1000)) ? ((DHCP_REFRESH_FREQUENCY * 1000) - elapsed) : 100;
			if (this._run) {
				setTimeout(() => {
					if (this._run)
						this._listenForDhcp4();
				},wtime);
			}
		});

		this._udhcpc.on('message',(msg) => {
			if (!msg)
				return;
			let dhcpEvent = msg['~'];
			if (dhcpEvent === 'bound') {
				let m = {};
				let cnt = 0;
				for(let i=0;i<DHCP_VALID_FIELDS.length;++i) {
					let f = msg[DHCP_VALID_FIELDS[i]];
					if (f) {
						m[DHCP_VALID_FIELDS[i]] = f;
						++cnt;
					}
				}
				if ((cnt > 0)&&(m.router)&&(m.interface)) {
					m.type = 'dhcp4';
					m.timestamp = Date.now();
					this._update(m);
				}
			}
		});
	}

	_listenForDhcp6()
	{
		this._udhcpc6 = spawn('udhcpc6',[
			'-q',
			'-n',
			'-s',
			process.cwd() + '/utils/dhcp-dummy-handler',
			'-t',
			'4',
			'-T',
			Math.round(DHCP_REFRESH_FREQUENCY / 4).toString(),
			'-A',
			DHCP_REFRESH_FREQUENCY.toString(),
			'-i',
			this._name
		],{
			stdio: ['ignore','ipc','ignore']
		});

		var startTime = Date.now();
		this._udhcpc6.on('exit',() => {
			let elapsed = Date.now() - startTime;
			//console.log('dhcp6 exit: '+this._name+' '+elapsed);
			let wtime = (elapsed < (DHCP_REFRESH_FREQUENCY * 1000)) ? ((DHCP_REFRESH_FREQUENCY * 1000) - elapsed) : 100;
			if (this._run) {
				setTimeout(() => {
					if (this._run)
						this._listenForDhcp6();
				},wtime);
			}
		});

		this._udhcpc6.on('message',(msg) => {
			if (!msg)
				return;
			let dhcpEvent = msg['~'];
			if (dhcpEvent === 'bound') {
				let m = {};
				let cnt = 0;
				for(let i=0;i<DHCP_VALID_FIELDS.length;++i) {
					let f = msg[DHCP_VALID_FIELDS[i]];
					if (f) {
						m[DHCP_VALID_FIELDS[i]] = f;
						++cnt;
					}
				}
				if ((cnt > 0)&&(m.router)) {
					m.type = 'dhcp6';
					m.timestamp = Date.now();
					this._update(m);
				}
			}
		});
	}

	_listenForIpv6Slaac_rdisc6_do(done)
	{
		cmd.get(process.cwd() + '/utils/rdisc6 -n -m -w 5000 '+this._name,(err,stdout,stderr) => {
			if (!this._run) {
				return
			}
			if (stdout) {
				let lines = stdout.split('\n');
				let fromRouter = null;
				let prefix = null;
				let ipv6Dns = [];
				for(let li=0;li<lines.length;++li) {
					if (lines[li]) {
						let l = lines[li].trim().toLowerCase();
						if (l.startsWith('prefix ')) {
							let cidx = l.indexOf(':');
							if ((cidx > 0)&&(cidx < (l.length-1))) {
								prefix = l.substr(cidx+1).trim();
							}
						} else if (l.startsWith('from ')) {
							fromRouter = l.substr(5).trim();
						} else if (l.startsWith('recursive dns server')) {
							let cidx = l.indexOf(':');
							if ((cidx > 0)&&(cidx < (l.length-1))) {
								let dns = l.substr(cidx+1).trim();
								if ((dns)&&(dns.indexOf(':') > 0)) {
									ipv6Dns.push(dns);
								}
							}
						}
					}
				}
				if ((fromRouter)&&(prefix)) {
					let ps = prefix.split('/');
					if (ps.length >= 2) {
						try {
							let ipv6PrefixBits = parseInt(ps[1]);
							let prefixAddr = ipaddr.parse(ps[0]).toByteArray();
							let sha = crypto.createHash('sha512');
							sha.update(this._name);
							let bits = sha.digest();
							for(let i=(ipv6PrefixBits / 8);i<16;++i)
								prefixAddr[i] = bits.readUInt8(i);
							if (prefixAddr[15] === 0)
								prefixAddr[15] = 1;
							let slaacIp = ipaddr.fromByteArray(prefixAddr).toString();
							this._update({
								router: fromRouter+'%'+this._name,
								timestamp: Date.now(),
								type: 'slaac',
								dns: (ipv6Dns.length > 0) ? ipv6Dns : null,
								interface: this._name,
								subnet: prefix,
								ip: slaacIp
							});
						} catch (e) {}
					}
				}
			}
			if (done)
				return done();
		});
	}

	_listenForIpv6Slaac_rdisc6()
	{
		if (this._run) {
			this._rdisc6Timer = false;
			this._listenForIpv6Slaac_rdisc6_do(() => {
				if ((!this._run)||(this._rdisc6Timer === null)) {
					return;
				}
				this._rdisc6Timer = setTimeout(() => { this._listenForIpv6Slaac_rdisc6(); },10000);
			});
		}
	}
}

module.exports = RouterListener;
