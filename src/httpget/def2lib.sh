#!/bin/bash
#
# def2lib.sh
#
# Def zu Library
#
##############################################################
#
if uname -o | grep -qi cygwin
 then
  M=cygwin
else
  M=`uname -s`
fi
if [[ -f defs_$M.lst ]]
 then
  DEFS=defs_$M.lst
else
  DEFS=defs.lst
fi
#
awk '
$1 == "'$1'" { print $2
             }' $DEFS
