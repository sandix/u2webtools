/****************************************/
/* File: u2w_version.c                  */
/* Programm zum anzeigen der Version    */
/* timestamp: 2015-12-26 14:42:52       */
/****************************************/

#include "unix2web_version.h"
#include "u2w.h"
#include <stdio.h>

char *versionstring = VERSION;

/***************************************************************************************/
/* void show_version(char * prg, int buildflag)                                        */
/*      show_version druckt Versionsnummer des Programms                               */
/***************************************************************************************/
void show_version(char *prg, int buildflag)
{ char *p;

  if( (p = strrchr(prg, '/')) )
    p++;
  else
    p = prg;

  printf("%s Version %s, %s", p, versionstring, COMPILEDATE);
  if( buildflag )
    printf(", build %d.\n", BUILD);
  else
    puts("");
#if defined(WITH_HTTPS) || defined(MAYDBCLIENT) || defined(POSTGRESQLCLIENT) || defined(HAS_PAM)
  fputs("Compiled with"
#ifdef HAS_PAM
  " pam"
#endif
#ifdef WITH_HTTPS
  " https"
#endif
#ifdef MARIADBCLIENT
  " mariadbclient"
#endif
#ifdef MYSQLCLIENT
  " mysqlclient"
#endif
#ifdef POSTGRESQLCLIENT
  " postgresqlclient"
#endif
#ifdef SQLITE3
  " sqlite3"
#endif
  ".\n", stdout);
#endif 
  fputs("Copyright: " COPYRIGHT, stdout);
}
