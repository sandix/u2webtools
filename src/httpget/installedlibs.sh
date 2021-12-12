#!/bin/bash
#
# installedlibs.sh
#
# nur installierte Libraries zusammensetzen
#
##############################################################
#
exec 2>/tmp/installedlibs.sh
set -x
#
if [[ "$1" = "-L" ]]
 then
  LDIRS="$2"
  shift 2
else
  LDIRS=""
fi
#
R=0
#
if [[ "$1" = "-static" ]]
 then
  STATIC=1
  shift
else
  STATIC=0
fi
#
if [[ "$1" = "-q" ]]
 then
  Q=1
  shift
else
  Q=0
fi
#
while [[ $# -gt 0 ]]
 do
  S=0
  for L in ${1//,/ }
   do
    if [[ $STATIC = 1 ]]
     then
      if [[ $(find `./get_dirs.sh /etc/ld.so.conf` /usr/lib /usr/lib64 $LDIRS -name "lib${L#-l}.a*" 2>/dev/null | wc -l) -gt 0 ]]
       then
        [[ $Q = 0 ]] && echo "$L"
        S=1
      fi
    elif uname -s | grep -qi cygwin
     then
      if [[ $(find `./get_dirs.sh /etc/ld.so.conf` /usr/lib /usr/lib64 $LDIRS -name "lib${L#-l}.dll*" 2>/dev/null | wc -l) -gt 0 ]]
       then
        [[ $Q = 0 ]] && echo "$L"
        S=1
      fi
    else
      if [[ $(find `./get_dirs.sh /etc/ld.so.conf` /usr/lib /usr/lib64 $LDIRS -name "lib${L#-l}.so*" 2>/dev/null | wc -l) -gt 0 ]]
       then
        [[ $Q = 0 ]] && echo "$L"
        S=1
      fi
    fi
  done
  if [[ $S = 0 ]]
   then
    R=1
  fi
  shift
done
exit $R
