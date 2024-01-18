#!/bin/bash
#
# update_mime_types.sh
#
################################
#
if [[ -f /etc/mime.types ]]; then
  newmt=/tmp/new_mime_types.h
  echo 'char *content_types[][2] = {' >$newmt
  { cat /etc/mime.types
    sed -n 's/[^"]*"\([a-z0-9][^"]*\)", "\([a-z][^"]*\)".*/\2 \1/p' mime_types.h
  } | awk '
/^[a-z]/ { for( i = 2; i <= NF; i++ )
           { if( !a[$i] )
               a[$i] = $1
           }
         }
END { for( i in a )
        printf("{ \"%s\", \"%s\" },\n", i, a[i])
    }
' | sort >>$newmt
  echo '{ "", "application/x-"}
};' >>$newmt
  if [[ $(wc -l <$newmt) -gt $(wc -l <mime_types.h) ]]; then
    cp $newmt mime_types.h
  fi
else
  echo "/etc/mime.types nicht gefunden"
fi
