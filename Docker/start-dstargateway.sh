#!/bin/sh

wget http://www.pistar.uk/downloads/DExtra_Hosts.txt -nv -O /usr/local/share/dstargateway.d/DExtra_Hosts.txt
wget http://www.pistar.uk/downloads/DCS_Hosts.txt -nv -O /usr/local/share/dstargateway.d/DCS_Hosts.txt
wget http://www.pistar.uk/downloads/DPlus_Hosts.txt -nv -O /usr/local/share/dstargateway.d/DPlus_Hosts.txt

/usr/local/bin/dstargateway /usr/local/etc/dstargateway.d/dstargateway.cfg
