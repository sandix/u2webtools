#!/bin/bash
#
# ./installedlibpaths.sh
#
##############################
#
for i in $*
 do
  if [[ -d "${i%include}lib" ]]
   then
    echo "${i%include}lib"
  elif [[ $i =~ mysql ]]
   then
    if ! ls /usr/lib*/libmysql* >/dev/null 2>&1
     then
      ls -d /usr/lib*/mysql
    fi
  fi
done
