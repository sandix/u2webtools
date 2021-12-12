BEGIN { print "/*************************************************/"
        print "/* Datei mit den globalen Variablendeklarationen */"
        print "/* generiert mit makevar.awk                     */"
        print "/*************************************************/"
        sflag = 0;
      }

/start global variables/,/end global variables/{
  if( $0 !~  "^ *[{/]" )
  { if( index($0, ";") > 0 )
    { if( sflag )
      { sflag = 0
        gsub(".*;", "")
        print $0
      }
      else
      { gsub(" *; *", ";#")
        split($0, a, "#")
        if( index(a[1], "=") > 0 )
          gsub(" *=.*;", ";", a[1])
        printf("extern %-30s%s\n", a[1], a[2])
      }
    }
    else if( index($0, "=") > 0 )
    { sflag = 1
      gsub(" *=[^/]*", ";#")
      split($0, a, "#")
      printf("extern %-30s%s\n", a[1], a[2])
    }
    else if( !sflag )
      print $0
  }
}

