/#define COMPILEDATE/ { print "#define COMPILEDATE \"" D "\""
                 next
               }
/#define COPYRIGHT/ { print "#define COPYRIGHT \"H. Sander, Flintbek " Y "\\n\""
                      next
		    }
{ print }
