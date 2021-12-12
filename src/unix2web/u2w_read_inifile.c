/**********************************/
/* File: u2w_inifile.c            */
/* inifile einlesen               */
/* timestamp: 2013-10-27 11:27:30 */
/**********************************/

#include "u2w.h"


/***************************************************************************************/
/* void read_inifile(void)                                                             */
/*                   char *inifile: Datei mit Initialparametern                        */
/*     Datei mit Initalparameter (%spar) einlesen                                      */
/***************************************************************************************/
#ifdef WEBSERVER
void read_inifile(void)
#else
void read_inifile(char *inifile)
#endif
{ FILE *ptr;                                         /* einzulesende Datei             */
  char varname[MAX_PAR_NAME_LEN];
  char value[PARLEN];                                /* Wert eines Parameters          */
  char *z, *v, c;

  LOG(1, "read_inifile, inifile: %s.\n", inifile ? inifile : "(NULL)");

  if( inifile && NULL != (ptr = fopen(inifile, "r")) )
  { while( NULL != fgets(zeile, MAX_ZEILENLAENGE, ptr) )
    { z = zeile;
      LOG(11, "read_inifile, zeile: %s.\n", zeile);
      while( isblank(*z) )
        z++;
      if( isalpha(*z) )
      { v = varname;
        while( isalnum(*z) )
          *v++ = *z++;
        *v = '\0';
        LOG(21, "read_inifile, varname: %s.\n", varname);
        while( isblank(*z) )
          z++;
        if( *z == '=' )
        { z++;
          while( isblank(*z) )
            z++;
          v = value;
          if( *z == '\'' || *z == '"' )
          { c = *z++;
            while( *z != c )
            { if( !*z )
              { if( NULL != fgets(zeile, MAX_ZEILENLAENGE, ptr) )
                  z = zeile;
                else
                  break;
              }
              else if( *z == '\\' )
              { z++;
                if( *z )
                  *v++ = *z++;
              }
              else
                *v++ = *z++;
            }
            *v = '\0';
            store_inipar(varname, value, true);            /* Parameter speichern      */
          }
          else
          { while( !isspace(*z) && v - value < PARLEN )
              *v++ = *z++;
            *v = '\0';
            store_inipar(varname, value, false);           /* Parameter speichern      */
          }
        }
      }
    }
    fclose(ptr);
  }
}
