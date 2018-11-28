#!/bin/bash

for d in tmp dev run proc sys mnt root home opt; do
        mkdir -p $d
done
chmod 0777 tmp
mkdir -p usr/local

mkdir -p var/lib/zerotier-one
cd var/lib/zerotier-one
zerotier-idtool generate identity.secret identity.public
chmod 0600 identity.secret
cd ../../..
mkdir -p etc/edge/factory-identity
cp var/lib/zerotier-one/identity.* etc/edge/factory-identity/
ztaddr=`cat var/lib/zerotier-one/identity.public | cut -d : -f 1`
echo -n $ztaddr >etc/hostname

cd etc
cp group.dist group
cp passwd.dist passwd
cp resolv.conf.dist resolv.conf
cp shadow.dist shadow
cd ..

exit 0
