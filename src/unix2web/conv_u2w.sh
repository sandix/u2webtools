#!/bin/bash
#
# conv_u2w.sh
#
# Dateien werden vom u2w V7 ins V8er Format konvertiert
#
BASE=`dirname $0`
for n in "$@"
 do
  if ! grep -q '^%title' $n
   then
    if grep -q '^ *[^ %]' $n
     then
      echo $n 
(      awk -f $BASE/get_headlines.awk $n | while read l t z
       do
        if [ $t = "T" ]
         then
          echo "${l}s/^/%title /"
        else
          echo "${l}s/^/%h2 /"
          echo "${l}s/$/%\/h/"
        fi
      done
      echo "wq"
) | ed -s $n
      echo
    fi
  fi
done
