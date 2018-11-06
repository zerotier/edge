#!/bin/bash

if [ -n "$1" ]; then
	cat `find /sys/devices -type f -name 'address' | grep -F "/$1/"`
fi
