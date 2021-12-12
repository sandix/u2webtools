#
if [ $# -ne 1 ]
 then
  echo "usage: $0 <po-file>"
  exit 1
fi
#
msgfmt -o ${1%/*}/unix2web.mo $1
