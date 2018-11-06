// (c)2018 ZeroTier, Inc.

const cmd = require('node-cmd');

const DO_NOTHING = false;
const VERBOSE_MODE = false;

module.exports = {
	get: (cmdline,callback) => {
		if (DO_NOTHING) {
			console.log('> '+cmdline);
			return callback(null,null,null);
		} else {
			cmd.get(cmdline,(err,stdout,stderr) => {
				if (VERBOSE_MODE) {
					console.log('> '+cmdline);
					if (stdout) {
						console.log('< '+stdout);
					}
					if (stderr) {
						console.log('2< '+stderr);
					}
				}
				setTimeout(() => {
					callback(err,stdout,stderr);
				},10);
			});
		}
	}
};
