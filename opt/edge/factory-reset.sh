#!/bin/bash

killall -9 zerotier-one

cd /
git clean -f -d
git reset --hard

cd /var/lib/zerotier-one
rm -rf networks.d peers.d controllers.d planet *.pid *.port identity.*
cp /etc/edge/factory-identity/identity.* *
chmod 0600 identity.secret

rm -rf /var/lib/zerotier-edge

cd /etc
rm -f *- passwd shadow group
cp passwd.dist passwd
cp shadow.dist shadow
cp group.dist group

rm -rf /home/*
