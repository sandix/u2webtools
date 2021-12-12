#!/bin/bash
#
# update_timestamp.h
#
# Zeile mit timestamp: Datum in den Kommentaren der *.c und *.h
# Dateien auf das aktuelle Datum der Datei setzen.
# Datei das Modifikationsdatum der Datei unverändert lassen.
#
find . -name '*.[ch]' -type f -printf "%p %Td %Tm %Ty %TH %TM %TS %TY\n" | while read n d mon y h min s yy
 do
  if ! grep -q "/\* timestamp: $yy-$mon-$d $h:$min:${s%.*}" $n
   then
    if grep -q '/\* timestamp: [0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] *\*/' $n
     then
      ed -s $n <<EOF
/\/\* timestamp: [0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] *\*\//s/timestamp: [0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9]/timestamp: $yy-$mon-$d $h:$min:${s%.*}/
wq
EOF
      touch -t $y$mon$d$h$min.${s%.*} $n
    fi
  fi
done
