#***************************************************
#! /bin/sh
# /etc/init.d/autoconnectnet

### BEGIN INIT INFO
# Provides:          noip
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Simple script to start a program at boot
# Description:       A simple script from .
### END INIT INFO


case "$1" in
  start)
    sleep 10
    echo "connecting via sakis3g"
    # run application you want to start
    /opt/sakis3g/sakis3g --sudo  "connect"
    ;;
  stop)
<autoconnectnet.sh" [no hay fin de línea] 35L, 852C
