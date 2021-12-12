#!/bin/bash
#
# installedincludepaths.sh
#
# nur installierte includes zusammensetzen
#
##############################################################
#
H="$1"
shift
#
while [[ $# -gt 0 ]]
 do
  awk '/#ifdef '${1#-D}'/,/^#e/ {if( $1 == "#include" ) print $2}' "$H" | sed 's/^<//;s/>$//' | while read i
   do
    if [[ ! -f /usr/include/$i ]]
     then
      for ip in `find /usr/local -name 'include' -type d -print 2>/dev/null`
       do
        if [[ -f $ip/$i ]]
         then
          echo $ip
        fi
      done
    fi
  done
  shift
done | sort -u
