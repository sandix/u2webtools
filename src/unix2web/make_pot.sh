#
D=`date '+%Y-%m-%d'`
#
if [ $# -ne 1 ]
 then
  echo "usage: $0 <old.po>"
  exit 1
fi
P=`dirname $1`
#
xgettext -o $P/u2w-$D.pot -C -k_ *.c
#
msgmerge -o $P/u2w-$D.po "$1" $P/u2w-$D.pot
