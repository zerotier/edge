module.exports = {
	// Wired interfaces
	wired: [ 'phy0','phy1','phy2' ],

	// WiFi devices
	wireless: [ 'wlan0' ],

	// The ad-hoc network all Edge devices join to allow users to easily set them up and to get updates, etc.
	serviceNetwork: 'ff001601bb000000',

	// Host on service network that serves Edge updates, can provide remote assistance, etc.
	serviceHost: 'fc44:16:19e:b1b5:daf::1',

	// Temporary directory for runtime-only files (no need to persist) with trailing /
	home: '/var/lib/zerotier-edge/'
};
