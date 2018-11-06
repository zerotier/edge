#!/bin/bash

cd /opt/edge/mgr

export NODE_ENV=production

while /bin/true; do
	node edge.js >>/dev/shm/mgr.out 2>&1
	sleep 1
done

exit 0
