export DIR="/Users/master/GitHub/Free-Copter"
cd $DIR
source $DIR/tools/defines.sh
export LogFile=$LogPath/run.log

function log() {
    for arg in "$@"
    do
    echo -e "\t$arg" >> $LogFile
    done
    
    if [ "$#" == 0 ]; then
    echo >> $LogFile
    fi
}

echo -e "\n`date` :" >> $LogFile
log "Name:      $0"
log "WorkDir:   `pwd`"
log "Args:      $*"
log "PID:       $$"
log "PATH:      $PATH"

open -a /Applications/Utilities/Terminal.app $DIR/tools/run.sh