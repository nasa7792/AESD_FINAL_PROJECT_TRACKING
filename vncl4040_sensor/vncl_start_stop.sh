#!/bin/sh
# /etc/init.d/S99vncl

case "$1" in
  start)
    echo "Starting VNCL daemon"
    /usr/bin/vncl4040_sensor &
    ;;
  stop)
    echo "Stopping VNCL daemon"
    killall vncl4040_sensor
    ;;
  restart)
    $0 stop
    $0 start
    ;;
  *)
    echo "Usage: $0 {start|stop|restart}"
    exit 1
esac

exit 0
