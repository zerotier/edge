#!/bin/bash

while /bin/true; do
	sleep 30
	if ! fping -q edge-update-server; then
		if ! fping -q 1.1.1.1; then
			if ! fping -q 8.8.8.8; then
				if ! fping -q 2606:4700:4700::1111; then
					if ! fping -q 2001:4860:4860::8888; then
						/opt/edge/led-control sos
					fi
				fi
			fi
		fi
	fi
done
