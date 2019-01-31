#!/bin/bash

echo ">> musrfit post uninstall script started ..."

# test if nemu-x86_64.conf does not exist
if [ -f "/etc/ld.so.conf.d/nemu-x86_64.conf" ]
then
  rm -f /etc/ld.so.conf.d/nemu-x86_64.conf  
fi
/sbin/ldconfig

echo ">> musrfit post uninstall script done."

