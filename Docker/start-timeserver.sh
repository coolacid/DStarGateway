#!/bin/sh

chown dstar:dstar /var/log/dstargateway/
/usr/local/bin/dgwtimeserver /usr/local/etc/dstargateway.d/dgwtimeserver.cfg
