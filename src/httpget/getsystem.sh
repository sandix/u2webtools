#!/bin/bash
#
# getsystem.sh
#
# BS-Kennung bestimmen
######################
#
UO=`uname -s`
#
if [[ "$UO" = "Linux" ]]
 then
  echo "-DLINUX"
elif [[ "$UO" = "SunOS" ]]
 then
  echo "-DSOLARIS"
elif echo "$UO" | grep -qi "Cygwin"
 then
  echo "-DCYGWIN"
fi
