BEGIN { cfile = ""
        line = ""
        print "/*******************************/"
        print "/* Funktionsprototypen         */"
        print "/* Generiert mit makeproto.awk */"
        print "/*******************************/"
      }

{ if( FILENAME != cfile )
  { printf("\n\n/* Funktionen aus %-12s */\n\n", FILENAME);
    cfile = FILENAME
  }
}

line != "" { if( $0 ~ "[;{}(]" )
               line = ""
             else
             { gsub("[ \t]+", " ")
               if( $0 ~ "[)][ \t]*$" )
               { print line $0 ";"
                 print FILENAME " " line $0 >"proto.lst"
                 line = ""
               }
               else
                 line = line $0
             }
           }

/^s?size_t [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^LONGWERT [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^wert [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^void [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^int [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^unsigned int [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^long [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^char [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^unsigned char [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^const char [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^float [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^double [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^time_t [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^struct [a-z0-9_]* [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^enum [a-z0-9_]* [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^[a-z0-9_]*type [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ || /^parart [*]?[a-z0-9_]*[(].*[,)][ \t]*$/ {
      if( $0 ~ "[)][ \t]*$" )
      { print $0 ";"
        print FILENAME " " $0 >"proto.lst"
      }
      else
        line=$0
}

/#if / || /#ifdef/ || /#ifndef/ || /#endif/ || /#else/ {print}

