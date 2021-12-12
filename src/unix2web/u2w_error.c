/**************************************************/
/* File: u2w_error.c                              */
/* Funktionen zum Senden von Fehlermeldungsseiten */
/* timestamp: 2015-10-10 10:20:53                 */
/**************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* int send_error_page(char *num, char *desc, char *http, char *text)                  */
/*                     char *num : Fehlernummer                                        */
/*                     char *desc: Text zur Fehlernummer                               */
/*                     char *http: Zusatzzeilen im HTTP/1.1 Header                     */
/*                     char *text: Infotext fuer den Body                              */
/*                     return    : true bei Fehler                                     */
/*     send_error_page sendet eine Fehlermeldungsseite an den Browser                  */
/***************************************************************************************/
int send_error_page(char *num, char *desc, char *http, char *text)
{ extern char *versionstring;

  LOG(1, "send_error_page, num: %s, desc: %s, http: %s, text: %s.\n",
      num, desc, http, text);

  if( headflag )
    zeile[0] = '\0';
  else
    snprintf(zeile, MAX_ZEILENLAENGE, PAGE_ERROR_HEADER "%s" PAGE_END,
             num, desc, desc, text);

  LOG(3, "send_error_page, zeile: %s.\n", zeile);

#ifdef WITH_MMAP
  if( status_mode & (STAT_ERROR|STATDIFF_ERROR) )
    status_counter->errors++;
#endif

  if( http_v )
    return dosendf(ERROR_HTTP_HEADER, num, desc, versionstring,
                   keepalive_flag ? "Keep-Alive" : "close",
                   strlen(zeile), http, zeile);
  else
    return dosend(zeile);
}


/***************************************************************************************/
/* int send_moved_permanently(char *server, char *path, char *file)                    */
/*                            char *server: server                                     */
/*                            char *path  : Pfad der URL                               */
/*                            char *file  : File, das gemoved ist                      */
/*     send_moved_permanently sendet neue URL                                          */
/***************************************************************************************/
int send_moved_permanently(char *server, char *path, char *file)
{ char z1[MAX_ZEILENLAENGE];
  char z2[MAX_ZEILENLAENGE];

  LOG(30, "send_moved_permanently, server: %s, path: %s, file: %s.\n",
      server, path, file);

#ifdef WITH_HTTPS
  if( ssl_mode )
  { snprintf(z1, MAX_ZEILENLAENGE, PAGE_MOVED_PERMANENTLY_HTTPS, server, path, file);
    snprintf(z2, MAX_ZEILENLAENGE, PAGE_MOVED_PERMANENTLY_HTTPS_TEXT, server, path,
             file);
  }
  else
#endif  /* WITH_HTTPS */
  { snprintf(z1, MAX_ZEILENLAENGE, PAGE_MOVED_PERMANENTLY_HTTP, server, path, file);
    snprintf(z2, MAX_ZEILENLAENGE, PAGE_MOVED_PERMANENTLY_HTTP_TEXT, server, path, file);
  }
  return send_error_page(PAGE_MOVED_PERMANENTLY_NUM, PAGE_MOVED_PERMANENTLY_DESC, z1, z2);
}


/***************************************************************************************/
/* int send_method_not_allowed(void)                                                   */
/*     send_method_not_allowed sendet Method Not Allowed                               */
/***************************************************************************************/
int send_method_not_allowed(void)
{
  return send_error_page(METHOD_NOT_ALLOWED_NUM, METHOD_NOT_ALLOWED_DESC, "",
                         METHOD_NOT_ALLOWED_TEXT);
}


/***************************************************************************************/
/* int send_not_modified(void)                                                         */
/*     send_not_modified sendet die HTTP/1.1 Meldung, dass Seite nicht veraendert wurde*/
/***************************************************************************************/
int send_not_modified(void)
{ time_t tt;                                                /* Aktuelle Zeit/Datum     */

  time(&tt);                                                /* Jetzt als String        */
  return dosendf(NOT_MODIFIED, httptime(gmtime(&tt)),
                 keepalive_flag ? "Keep-Alive" : "close");  /* senden                  */
}
#endif  /* #ifdef WEBSERVER */
