#!/bin/bash
#
# gencpath.sh
#
##############################
#
P=$1
shift
#
if [[ -n "$1" ]]
 then
  echo "-$P$*" | sed 's/ /:/g'
fi
