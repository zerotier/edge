// (c)2018 ZeroTier, Inc.

const request = require('request');
const fs = require('fs');

function ztDaemonQuery(uri,data,callback)
{
	var authToken = fs.readFileSync("/var/lib/zerotier-one/authtoken.secret");
	if (!authToken) {
		return callback(new Error("authtoken.secret not readable"),null);
	}
	authToken = authToken.toString();
	var port = fs.readFileSync("/var/lib/zerotier-one/zerotier-one.port");
	if (port)
		port = parseInt(port.toString());
	else port = 9993;
	request({
		url: 'http://127.0.0.1:'+port+uri,
		method: (data === false) ? "DELETE" : ((data) ? "POST" : "GET"),
		body: (data === false) ? null : data,
		json: true,
		headers: {
			'X-ZT1-Auth': authToken
		}
	},(err,response,body) => {
		if ((!response)||(!response.statusCode))
			return callback(new Error('zerotier service not responding'));
		if (response.statusCode != 200)
			return callback(new Error('status '+response.statusCode),null);
		if ((body)&&(typeof body === 'string')) {
			try {
				body = JSON.parse(body);
			} catch (e) {
				return callback(new Error('invalid JSON: '+e+' (body: '+body+')'),null);
			}
		}
		callback(err,body);
	});
}

module.exports = ztDaemonQuery
