#!/bin/sh

case "$1" in
  start)
    echo "Starting motor"
    /usr/bin/motor &
    ;;
  stop)
    echo "Stopping motor "
    killall motor
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