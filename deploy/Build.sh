#!/usr/bin/env bash

#  Build.sh
#  Free-Copter
#
#  Created by Master on 6/26/15.
#
scp -r -P 22 -i ~/.ssh/id_rsa ./* root@ocean.local:/root/free-copter/
ssh root@ocean.local -p 22 -i ~/.ssh/id_rsa 'cd /root/free-copter/; make'