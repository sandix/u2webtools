D=$(pwd -P)
DP=${D%/*/*}
CD=${DP%/*}
DIR=${DP##*/}
ARCHIV=$HOME/archiv/U2W-Archiv/u2w_${DIR#*-}_`date +%F`.txz
cd $CD
tar --create --verbose --xz --file=$ARCHIV $DIR/SRC/*/*.c $DIR/SRC/*/*.h $DIR/SRC/*/*.lst $DIR/SRC/*/*.sed $DIR/SRC/*/*.awk $DIR/SRC/*/Makefile* $DIR/SRC/*/*.sh $DIR/SRC/*/locale $DIR/PREP $DIR/etc $DIR/usr
