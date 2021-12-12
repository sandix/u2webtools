BEGIN { print "/*************************************************/"
        print "/* Datei mit den globalen Variablendeklarationen */"
        print "/* generiert mit makevar.awk                     */"
        print "/*************************************************/"
        sflag = 0;
      }

/Anfang globale Variablen/,/Ende globale Variablen/{
  if( $0 !~  "^ *[{]" )
  { if( index($0, ";") > 0 )
    { gsub(" *; *", ";#")
      split($0, a, "#")
      if( index(a[1], "=") > 0 )
        gsub(" *=.*;", ";", a[1])
      printf("extern %-30s%s\n", a[1], a[2])
    }
    else if( index($0, "=") > 0 )
    { gsub(" *= *", ";#")
      split($0, a, "#")
      if( a[2] ~ "^/" )
        printf("extern %-30s%s\n", a[1], a[2])
      else
        printf("extern %-30s\n", a[1])
      while( getline && index($0, ";") == 0 );
      if( index($0, ";") > 0 )
      { gsub(".*;", "")
        print $0
      }
    }
    else
      print $0
  }
}

