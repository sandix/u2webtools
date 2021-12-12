#!/bin/bash
#
# installedincludes.sh
#
# nur installierte includes zusammensetzen
#
##############################################################
#
if [[ $1 = "-I" ]]
 then
  IDIRS="$2"
  shift 2
else
  IDIRS=""
fi
#
if [[ $1 = "-L" ]]
 then
  LDIRS="$2"
  shift 2
else
  LDIRS=""
fi
#
if [[ $1 = "-static" ]]
 then
  STATIC="-static"
  shift
else
  STATIC=""
fi
#
H="$1"
shift
#
while [[ $# -gt 0 ]]
 do
  if [[ $(awk '/#ifdef '${1#-D}'/,/^#e/ {if( $1 == "#include" ) print $2}' "$H" | sed 's/^<//;s/>$//' | while read i
         do
          if [[ ! -f /usr/include/$i ]]
           then
            if echo "#include <$i>" | gcc -E -x c - 2>/dev/null | grep -q $i
             then
              :
            elif [[ -z "$IDIRS" ]]
             then
              echo 1
            else
              if [[ `for ip in $IDIRS
                      do
                       if [[ -f $ip/$i ]]
                        then
                         echo 2
                       fi
                    done` = "" ]]
               then
                echo 1
              fi
            fi
          fi
        done) = "" ]]
   then
    if ./installedlibs.sh -L "$LDIRS" $STATIC -q `./def2lib.sh "$1"`
     then
      echo "$1"
    fi
  fi
  shift
done | sort -u
