#!/bin/bash
#
# update_mime_types.sh
#
# mime_types.h aus /etc/mime.types updaten
###########################################
#
TMPMT=/dev/shm/mime_types.$$.h
#
WDIR=$(dirname $0)
#
unset FORCE
if [[ "$1" == "-f" ]]
 then
  FORCE=1
fi
#
awk '$1 !~ /^#/ && $2 != "" { printf("%-20s %s\n", $2, $1) }' /etc/mime.types | sort -u | awk '
BEGIN { print "char *content_types[][2] = {" }

      { printf("{ \"%s\", \"%s\"},\n", $1, $2) }

END   { print "{ \"\", \"\"}\n};" }' >"$TMPMT"

if [[ -n "$FORCE" || $(wc -l <"$TMPMT") -gt $(wc -l <"$WDIR/unix2web/mime_types.h") ]]
 then
  cp "$TMPMT" "$WDIR/unix2web/mime_types.h"
  mv "$TMPMT" "$WDIR/httpget/mime_types.h"
else
  rm "$TMPMT"
fi
