#!/bin/bash

cd /opt/edge/webui

while /bin/true; do
	source /opt/edge/webui-env.sh
	node index.js >>/dev/shm/webui.out 2>&1
	sleep 1
done

exit 0
