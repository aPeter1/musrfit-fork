#!/bin/bash

echo "  >> musrfit post install script started ..."

# test if nemu-x86_64.conf does not exist
if [ ! -f "/etc/ld.so.conf.d/nemu-x86_64.conf" ]
then
  echo "/usr/local/lib" >> /etc/ld.so.conf.d/nemu-x86_64.conf 
  echo "/usr/local/lib64" >> /etc/ld.so.conf.d/nemu-x86_64.conf   
fi
/sbin/ldconfig

