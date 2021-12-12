#!/bin/bash
#
# objdirname.sh
#
# Object Dirname aus Architektur und Libraries zusammensetzen
#
##############################################################
#
ARCH=`uname -m`
#
echo -n $ARCH
#
if [[ `uname -s` = "SunOS" ]]
 then
  while [[ $# -gt 0 ]]
   do
    echo "${1#-l}"
    shift
  done
else  
  if uname -o | grep -qi cygwin
   then
    echo "Cygwin"
    exit
  fi
  #
  LIBDIRS="/usr/lib /usr/lib64 `./get_dirs.sh /etc/ld.so.conf`"
  if [[ -x /bin/rpm ]]
   then
    while [[ $# -gt 0 ]]
     do
      for L in ${1//,/ }
       do
        rpm -qf $(find $LIBDIRS -name "lib${L#-l}.so*" 2>/dev/null | sort | head -1) | sort | head -1
      done
      shift
    done
  elif which dpkg >/dev/null 2>&1
   then
    while [[ $# -gt 0 ]]
     do
      dpkg -S $(find $LIBDIRS -name "lib${1#-l}.so*" 2>/dev/null | sort | head -1) | head -1 | awk -F ':' '{print $1}'
      shift
    done
  elif which pacman >/dev/null 2>&1
   then
    while [[ $# -gt 0 ]]
     do
      pacman -Qo $(find $LIBDIRS -name "lib${1#-l}.so*" 2>/dev/null | sort | head -1) | head -1 | sed 's/.*is owned by //;s/ /-/g'
      shift
    done
  fi
fi | sort -u | sed 's/\(.*\)-[^-]*$/\1/;s/-devel-/-/' | awk '{printf("_%s", $1)};END{printf("\n")}'
