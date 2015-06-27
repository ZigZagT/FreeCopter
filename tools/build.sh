#!/usr/bin/env bash

#  build.sh
#  Free-Copter
#
#  Created by Master on 6/26/15.
#
source ./tools/defines.sh
export LogFile="$LogPath/build.log"

function log() {
    for arg in "$@"
    do
        echo -e "\t$arg" >> $LogFile
    done

    if [ "$#" == 0 ]; then
        echo >> $LogFile
    fi
}

if [ ! -d $LogPath ]; then
    mkdir $LogPath
fi
if [ ! -d ./dest ]; then
    mkdir ./dest
fi

echo -e "\n`date` :" >> $LogFile
log "Name:      $0"
log "WorkDir:   `pwd`"
log "Args:      $*"
log "PID:       $$"
log "PATH:      $PATH"

ssh root@ocean.local -p 22 -i ~/.ssh/id_rsa "if [ ! -d $RemoteDir ]; then mkdir $RemoteDir; fi"
scp -r -P 22 -i ~/.ssh/id_rsa ./* root@ocean.local:$RemoteDir
ssh root@ocean.local -p 22 -i ~/.ssh/id_rsa "cd $RemoteDir; make"