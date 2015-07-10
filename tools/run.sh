#!/usr/bin/env bash

#  run.sh
#  Free-Copter
#
#  Created by Master on 6/26/15.
#
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/defines.sh

ssh root@ocean.local -p 22 -i ~/.ssh/id_rsa "cd $RemoteDir/dest; if [ -f ./fc ]; then ./fc; fi"