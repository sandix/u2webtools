#!/bin/bash
#
# next_build.sh
################
#
if [[ -f "$1" ]]; then
  NB=$(awk '$2 == "BUILD" {print 1+$3}' "$1")
  sed -i "s/#define BUILD .*/#define BUILD $NB/" "$1"
fi
