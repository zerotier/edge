#!/bin/bash

# This adds a .empty file in empty directories on the system so git will retain them.

cd /
for d in `find bin usr etc sbin var -type d -empty`; do
	touch $d/.gitkeep
done
