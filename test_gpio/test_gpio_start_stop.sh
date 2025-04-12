#!/bin/sh

case "$1" in
    start)
        echo "Starting test_gpio"
        /usr/bin/test_gpio &
        ;;
    stop)
        echo "Stopping test_gpio"
        pkill -f /usr/bin/test_gpio
        ;;
    *)
        echo "Usage: $0 {start|stop}"
        exit 1
        ;;
esac

exit 0
