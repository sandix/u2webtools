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

/^LONGWERT [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^wert [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^void [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^int [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^unsigned int [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^long [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^char [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^unsigned char [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^const char [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^float [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^double [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^time_t [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^struct [a-zA-Z0-9_]* [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^enum [a-zA-Z0-9_]* [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^[a-zA-Z0-9_]*type [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ || /^parart [*]?[a-zA-Z0-9_]*[(].*[,)][ \t]*$/ {
      if( $0 ~ "[)][ \t]*$" )
      { print $0 ";"
        print FILENAME " " $0 >"proto.lst"
      }
      else
        line=$0
}

/#if / || /#ifdef/ || /#ifndef/ || /#endif/ || /#else/ {print}

