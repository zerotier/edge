#!/bin/bash

chown 0 /usr/bin/sudo
chgrp 0 /usr/bin/sudo
chmod u+s /usr/bin/sudo

/bin/bash -c '/bin/sleep 5; /usr/bin/killall node' &

exit 0
