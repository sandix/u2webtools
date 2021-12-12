/****************************************************************/
/* File: u2w_acc.c                                              */
/* Funktionen zum Bestimmen der Berechtigungen eines Directorys */
/* ueber die Datei .access                                      */
/* timestamp: 2013-10-27 11:23:47                               */
/****************************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* int test_access(char *pfad, char *user)                                             */
/*                 char *pfad: Pfad der angeforderten Seite                            */
/*                 char *user: User, der die Seite anfordert                           */
/*                 return: true, der User hat Zugriff, false sonst                     */
/*     test_access bestimmt anhand der Datei .access die Zugriffsrechte                */
/***************************************************************************************/
int test_access(char *pfad, char *user)
{ char *p;
  char accdat[MAX_DIR_LEN];                      /* Aufbau des ACCESS-Pfades+Datei     */
  char name[MAX_NAME_LEN];                       /* Dateiname der Anforderung          */
  FILE *ptr;                                     /* ptr auf ACCESS-Datei               */
  char zeile[USER_LENGTH], *z;                   /* eingelesen Zeilen aus ACCESS-Dat.  */
  struct stat stat_buf;                          /* Zum Test, ob ACCESS-Datei vorhanden*/
  char u[USER_LENGTH];                           /* username aus ACCESS-Datei          */

  LOG(1, "test_acc, pfad: %s, user: %s.\n", pfad, user);
  strcpyn(accdat, pfad, MAX_DIR_LEN);            /* Pfad, der angefordert wird         */
  if( NULL != (p = strrchr(accdat, '/')) )       /* letzten '/'                        */
  { LOG(11, "test_acc, p: %s.\n", p);
    strcpyn(name, p+1, MAX_NAME_LEN);            /* Dateiname extrahieren              */
    strcpyn(p+1, ACCESS_FILE, MAX_DIR_LEN+accdat-p-1); /* Pfad+Name der ACCESS-Datei   */
    LOG(13, "test_acc, accdat: %s.\n", accdat);
    if( stat(accdat, &stat_buf) || (S_IFREG & stat_buf.st_mode) != S_IFREG )
      return !access_file_flag;                  /* keine ACCESS-Datei vorhanden       */
    LOG(13, "test_acc, nach stat.\n");
    if( NULL != (ptr = fopen(accdat, "r")) )     /* ACCESS-Datei oeffnen               */
    { while( !feof(ptr) )
      { fgets(zeile, USER_LENGTH, ptr);          /* Zeile einlesen                     */
        z = zeile;
        strcpyn_l(u, &z, USER_LENGTH);           /* Username einlesen                  */
        if( !strcmp(u, user) )                   /* mit Anmeldung vergleichen          */
	{ fclose(ptr);                           /* gefunden, ACCESS-Datei schliessen  */
          return true;                           /* Zugriff gewaehrt zurueck           */
	}
      }
      fclose(ptr);
      return false;                              /* kein Zugriff zurück                */
    }
    return false;                                /* kein Zugriff zurueck               */
  }
  else                                           /* Pfad kann nicht aufgeloest werden, */
    return false;                                /* dann auch kein Zugriff             */
}
#endif  /* #ifdef WEBSERVER */
