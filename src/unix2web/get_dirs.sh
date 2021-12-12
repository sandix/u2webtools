#!/bin/bash
#
# get_dirs.sh
#
# Aus config-Datei mit include
# Verzeichnisse bestimmen
#######################################
#
for i in "$@"
 do
  [[ -f $i ]] && grep -v '^ *#' $i | while read a b
   do
    if [[ "$a" = "include" ]]
     then
      $0 $b
    elif [[ -n "$a" ]]
     then
      echo $a
   fi
  done
done
