BEGIN {
  print "'\\\" t -*- coding: UTF-8 -*-"
  print ".\\\" Process this file with"
  print ".\\\" groff -man -Tascii " PRG "." NUM
  print ".\\\""
  print ".ad l"
  FS = " *:: *"
  IGNORECASE = 0
  if( DATE ~ / / )
    DATE = "\"" DATE "\""
  if( !SOUECE )
    SOURCE = "Linux"
  else if( SOURCE ~ / / )
    SOURCE = "\"" SOURCE "\""
  if( MANUAL ~ / / )
    MANUAL = "\"" MANUAL "\""
  print ".TH " PRG " " NUM " " DATE " " SOURCE " " MANUAL
}

function form(text)
{ if( text ~ "%.%..*%%" )
  { t = gensub(/^%F%(.*)%%/, ".I \\1\n", "1", text)
    t = gensub(/ %F%(.*)%%/, "\n.I \\1\n", "g", t)
    t = gensub(/^%O%(.*)%%/, ".B \\1\n", "1", t)
    t = gensub(/ %O%(.*)%%/, "\n.B \\1\n", "g", t)
    print t
  }
  else
    print text
}

/^NAME$/ {
  m = "N"
  print ".SH " $0
  nlflag = 0
  next
}

/^SYNOPSIS$|^SYNTAX$/ {
  m = "S"
  print ".SH " $0
  nlflag = 1
  next
}

/^SAMPLES$|^BEISPIELE$/ {
  m = "B"
  print ".SH " $0
  nlflag = 0
  next
}

/^OPTIONS$|^OPTIONEN$/ {
  m = "O"
  print ".SH " $0
  nlflag = 0
  mark = ""
  next
}

/^EXIT STATUS$/ {
  m = "X"
  print ".SH \"" $0 "\""
  nlflag = 0
  next
}

/^RÜCKGABEWERT$/ {
  m = "X"
  print ".SH " $0
  nlflag = 0
  next
}

/^FILES$|^DATEIEN$/ {
  m = "F"
  print ".SH " $0
  nlflag = 0
  next
}

/^ENVIRONMENT$|^UMGEBUNG$/ {
  m = "E"
  print ".SH " $0
  nlflag = 0
  next
}

/^SEE ALSO$|^SIEHE AUCH$/ {
  m = "A"
  print ".SH \"" $0 "\""
  nlflag = 0
  next
}

/^[[:upper:]][[:upper:]]*$/ {
  m = "D"
  print ".SH " $0
  nlflag = 0
  next
}

{ if( $0 == "" )
  { nlflag = 1
  }
  else
  { if( m == "S" )
    { gsub("^[[:blank:]][[:blank:]]*", "")
      gsub("%% %%", " %% %%")
      s = $0
      k = f = 0
      l = ""
      while( match(s, "^ %% %%|^[[:alnum:]_-][[:alnum:]_-]*|^[[:blank:]][[:blank:]]*|^[^[:alnum:]_[:blank:]-][^[:alnum:]_[:blank:]-]*") )
      { b = substr(s, RSTART, RLENGTH)
        s = substr(s, RSTART+RLENGTH)
        if( b == " %% %%" )
          printf("%s", "\\ ")
        else if( nlflag )
        { print ".HP \\w'\\fB" b "\\fR\\ 'u"
          nlflag = 0
          gsub("\\\\", "\\[rs]", b)
          gsub("-", "\\-", b)
          print ".B " b
          gsub("^[[:blank:]][[:blank:]]*", "", s)
        }
        else
        { f = 1
          if( b ~ "^-" )
          { gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            printf("%s", "\\fB" b "\\fP")
            l = "M"
          }
          else if( l == "m" )
          { gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            if( b  ~ "[[:alnum:]_]" )
              printf("%s", "\\fB" b "\\fP")
            else
              printf("%s", "\\|" b "\\|")
            if( b == "]" )
              l = "S"
          }
          else if( l == "a" )
          { gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            if( b  ~ "[[:alnum:]_]" )
              printf("%s", "\\fI" b "\\fP")
            else
              printf("%s", "\\|" b "\\|")
            if( b == "]" )
              l = "S"
          }
          else if( b ~ "[[:alnum:]_]" )
          { gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            printf("%s", "\\fI" b "\\fP")
            l = "A"
          }
          else if( b ~ "[[:blank:]]" )
          { gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            if( l ~ "[Mm]" && k )
              printf("%s", "\\ ")
            else
              printf("%s", b)
            l = "B"
          }
          else if( b == "[" && l ~ "[AM]" )
          { gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            printf("%s", "\\|" b "\\|")
            l = tolower(l)
          }
          else
          { if( b == "[" )
              k++
            else if( b == "]" )
              k--
            gsub("\\\\", "\\[rs]", b)
            gsub("-", "\\-", b)
            printf("%s", "\\|" b "\\|")
            l = "S"
          }
        }
      }
      if( f )
        printf("\n")
    }
    else
    { gsub("\\\\", "\\[rs]")
      gsub("-", "\\-")
      gsub("%% %%", "\\ ")
      if( m == "O" )
      { if( $2 )
        { print ".TP"
          if( $1 ~ " " )
          { split($1, oa, " ")
            mark = oa[2]
            gsub(" ", " \\ ", $1)
            print ".BI " $1
            gsub(" " mark "$", "\n.I " mark, $2)
            gsub(" " mark " ", "\n.I " mark "\n", $2)
            gsub(" " mark "s ", "\n.I " mark "s\n", $2)
            gsub("^" mark " ", ".I " mark "\n", $2)
          }
          else
          { mark = ""
            print ".B " $1
          }
          form($2)
        }
        else if( $0 )
        { if( mark )
          { gsub(" " mark "$", "\n.I " mark, $0)
            gsub(" " mark " ", "\n.I " mark "\n", $0)
            gsub("^" mark " ", ".I " mark "\n", $0)
          }
          form($0)
        }
      }
      else if( m == "F" )
      { if( $2 )
        { print ".TP"
          print ".I " $1
          print $2
        }
        else if( $0 )
          print $0
      }
      else if( m == "N" )
        print $0
      else if( m != "" )
      { if( nlflag )
        { print ""
          nlflag = 0
        }
        if( PRG )
        { gsub(" " PRG " ", "\n.B " PRG "\n", $0)
          sub("^" PRG " ", ".B " PRG "\n", $0)
        }
        print $0
      }
    }
  }
}
