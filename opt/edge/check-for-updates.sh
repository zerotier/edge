#!/bin/bash

if ! fping -q edge-update-server; then
	echo
	echo '*** Unable to reach update server!'
	echo
	exit
fi

echo
echo '*** Getting latest revision history from update server...'
echo
if [ -z "`git remote show | grep -F edge-update-server`" ]; then
	git remote rm edge-update-server >>/dev/null 2>&1
	git remote add edge-update-server http://edge-update-server/edge-root.git >>/dev/null 2>&1
fi
git fetch edge-update-server master
echo

if [ -n "`git log HEAD..edge-update-server/master`" ]; then
	echo '*** Updates since current revision:'
	echo
	git log HEAD..edge-update-server/master
	echo
else
	echo '*** Your Edge is up to date!'
	echo
fi

if [ -n "`git status -s`" ]; then
	echo '*** WARNING: these untracked local changes would be lost on update:'
	echo
	git status -s
	echo
fi

exit 0
