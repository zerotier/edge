#!/bin/sh
# @(#) $Id: bihourly.sh,v 1.2 2006/07/28 18:19:32 leres Exp $ (LBL)
#
#  bihourly arpwatch job
#
#

. /etc/conf.d/arpwatch

ARPWATCH_REPORT_EMAIL=

cd /usr/share/arpwatch

list="`cat list`"
cname="`cat cname`"
temp1=$(mktemp)
temp2=$(mktemp)
d=/tmp/errs

# imperfect hack
l=/tmp/bihourly.lock
if [ -f ${l} ]; then
	echo "skipping arpwatch bihourly job, lock file exists"
	exit
fi

trap 'rm -f ${l}; exit 1' 1 2 3 15
touch ${l}

alist=""
for r in ${list}; do \
	arpfetch ${r} ${cname} > ${r} 2> ${temp1}
	if [ -s ${temp1} ]; then
		echo "arpfetch ${r} errors:"
		xr=${d}/${r}.$$
		echo "(file ${r} saved in ${xr})"
		mkdir -p ${d}
		cp ${r} ${xr}
		sed -e 's/^/    /' ${temp1}
		continue
	fi
	alist="${alist} ${r}"
done

arpsnmp -d ${alist} > ${temp1} 2>&1

sed -e '/arpsnmp: suppressed DECnet flip flop/d' ${temp1} > ${temp2}
if [ -s ${temp2} ]; then
	mail -s "`hostname` arpwatch report" \
	    $ARPWATCH_REPORT_EMAIL < ${temp2}
fi
rm -f ${temp1} ${temp2}

d=/www/NETINFO/arpwatch

f=arp.dat
cp ${f} ${d}

f=ethercodes.dat
diff ethercodes.dat ${d} >>/dev/null 2>&1
if [ $? -ne 0 ]; then
	echo Updating ${d}/${f}
	cp ${f} ${d}
fi

rm ${l}

exit 0
