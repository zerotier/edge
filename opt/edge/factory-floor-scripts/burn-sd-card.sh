#!/bin/bash

cd /root

SDCARD=/dev/sdb

umount -f /edge-sd >>/dev/null 2>&1
umount -f /mnt >>/dev/null 2>&1

# to create the partitions programatically (rather than manually)
# we're going to simulate the manual input to fdisk
# The sed script strips off all the comments so that we can 
# document what we're doing in-line with the actual commands
# Note that a blank line (commented as "defualt" will send a empty
# line terminated with a newline to take the fdisk default.
sed -e 's/\s*\([\+0-9a-zA-Z]*\).*/\1/' << EOF | fdisk ${SDCARD}
  o # clear the in memory partition table
  n # new partition
  p # primary partition
  1 # partition number 1
    # default - start at beginning of disk 
    # defauit - entire disk
  a # make a partition bootable
  p # print the in-memory partition table
  w # write the partition table
  q # and we're done
EOF

mkfs.ext4 -F /dev/sdb1
mount /dev/sdb1 /edge-sd -t ext4 -o noatime,discard,rw
cd /edge-sd
if [ ! -d lost+found ]; then
	echo 'Disk init failed!'
	exit 0
fi

# Older uboot firmwares had issues with boot not being the
# first directory created. May no longer matter but do it
# anyway.
sync
cp -av /root/edge-root/boot .
sync

rsync -a -S --progress /root/edge-root/* /root/edge-root/.git* .

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
#cp var/lib/zerotier-one/identity.public /root/edge-identities/$ztaddr.public
echo -n $ztaddr >etc/hostname

cd etc
cp group.dist group
cp passwd.dist passwd
cp resolv.conf.dist resolv.conf
cp shadow.dist shadow

cd /root

ls -al /edge-sd
sync
umount /edge-sd
