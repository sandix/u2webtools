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
  fi
done
