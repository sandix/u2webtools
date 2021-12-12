#!/bin/bash
#
# defs2libs.sh
#
# Defs zu Libraries
#
##############################################################
#
if uname -s | grep -qi cygwin
 then
  M=cygwin
else
  M=`uname -s`
fi
if [[ -f defs_$M.lst ]]
 then
  DEFS=defs_$M.lst
else
  DEFS=defs.lst
fi
#
awk -v Z="ALL $*" '
$1 !~ " *#" { a[$1] = $2
              if( $3 )
                b[$1] = $3
            }
END { split(Z, t)
      for( u in t )
      { if( a[t[u]] )
          print a[t[u]]
        if( b[t[u]] )
          b[t[u]] = ""
      }
      for( u in b )
      { if( b[u] )
          print b[u]
      }
    }' $DEFS
