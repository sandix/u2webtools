/*************************************/
/* File: u2w_head.c                  */
/* Funktion erzeugt den HTTP Header  */
/* timestamp: 2016-03-13 00:07:15    */
/*************************************/


#include "u2w.h"

#ifndef COMPILER
#ifdef WEBSERVER

#include "mime_types.h"

/***************************************************************************************/
/* char *get_ct(char *file)                                                            */
/*              char *file: Dateiname zur Bestimmung des Content-Types                 */
/*              return    : Liefert Zeiger auf den Content-Type der Datei file         */
/***************************************************************************************/
char *get_ct(char *file)
{ int i;
  char *p;
  static char ct[128];

  i = 0;
  if( NULL != (p = strrchr(file, '.')) )
  { if( *p )
      p++;
    while( *content_types[i][0] && strcmp(content_types[i][0], p) )
      i++;
    if( *content_types[i][0] )
      return content_types[i][1];
    else
    { strcpy(ct, "application/x-");
      strcat(ct, p);
      return ct;
    }
  }
  else
    return "text/plain";
}


/***************************************************************************************/
/* int send_http_header(char *ct, char *ts, int expires_flag, unsigned LONGWERT cl,    */
/*                 char *setcharset)                                                   */
/*                 char *ct        : Content-Type oder leer bei cgi-Aufruf             */
/*                 char *ts        : Modifikationsdatum der Datei                      */
/*                 int expires_flag: true, aktuelles Datum als Expires senden          */
/*                 off_t cl        : Content-Length, 0 wenn nicht anzugeben            */
/*                 char *setcharset   : Characterset                                   */
/*                 return          : true bei Fehler                                   */
/*     send_http_header sendet den HTTP/1.1 ... Header mit Date, Server, ...           */
/***************************************************************************************/
int send_http_header(char *ct, char *ts, int expires_flag, unsigned LONGWERT cl,
                     char *setcharset)
{ char tss[128];
  time_t tt;
  extern char *versionstring;
  int i;

  LOG(20, "send_http_header, cl: " ULONGFORMAT ", no_header_flag: %d, http_v: %d.\n",
      cl, no_header_flag, http_v);

  if( no_header_flag == NO_HEADER || !http_v )
  { http_head_flag = 7;
    return false;
  }

  if( httpnum || httpdesc )
    dosendf("HTTP/1.1 %d %s" CRLF, httpnum > 0 ? httpnum : 200,
            httpdesc ? httpdesc : "OK");
  else
    if( dosend("HTTP/1.1 200 OK" CRLF) )
      return true;

  strcpyn(tss, ts, 128);
  time(&tt);

  if( no_header_flag & NO_HEADER_PRGVERS )
  { if( dosendf("Date: %s" CRLF, httptime(gmtime(&tt))) )
      return true;
  }
  else
  { if( dosendf("Date: %s" CRLF "Server: unix2web/%s" CRLF, httptime(gmtime(&tt)), versionstring) )
      return true;
  }

  if( expires_flag )
    if( dosendf("Expires: %s" CRLF, httptime(gmtime(&tt))) )
      return true;

  if( dosendf("Last-Modified: %s" CRLF, tss[0] ? tss : httptime(gmtime(&tt))) )
    return true;

  if( cl > 0 )
  { if( dosendf("Content-Length: " ULONGFORMAT CRLF, cl) )
      return true;
  }

  if( *ct || *mime )
  { if( *charset )
    { if( dosendf("Content-Type: %s; charset=%s" CRLF, *mime ? mime : ct, charset) )
        return true;
    }
    else if( setcharset && *setcharset )
    { if( dosendf("Content-Type: %s; charset=%s" CRLF, *mime ? mime : ct, setcharset) )
        return true;
    }
    else
    { if( dosendf("Content-Type: %s; charset=UTF-8" CRLF, *mime ? mime : ct) )
        return true;
    }
  }

  if( *savename )
    if( dosendf("Content-Disposition: filename=\"%s\"" CRLF, savename) )
      return true;

  for( i = 0 ; i < anz_httpheadlines; i++ )
  { if( dosendf("%s" CRLF, httpheadlines[i]) )
      return true;
  }

  if( cl > 0 )
  { if( keepalive_flag )
    { if( dosend("Connection: Keep-Alive" CRLF CRLF) )
        return true;
    }
    else
    { if( dosend("Connection: close" CRLF CRLF) )
        return true;
    }
    http_head_flag = 2;
  }
  else
  { http_head_flag = 1;
  }

  return false;
}
#endif  /* #ifdef WEBSERVER */


/***************************************************************************************/
/* int send_body_start(void)                                                           */
/*     send_body_start erzeugt html-Body ggf. mit Hintergrundbild                      */
/***************************************************************************************/
int send_body_start(void)
{ LOG(11, "send_body_start\n");
  if( css_defs[CSS_BODY] )
  { if( *background )                                    /* Hintergrundbild angegeben? */
    { if( dosendf(PAGE_BODY_START_CLASS_BG, css_defs[CSS_BODY], background) )
                                                         /* Body mit Bild              */
        return true;
    }
    else if( *bodypars )
    { if( dosendf(PAGE_BODY_START_CLASS_PAR, css_defs[CSS_BODY], bodypars) )
                                                         /* Body mit Parameter         */
        return true;
    }
    else
    { if( dosendf(PAGE_BODY_START_CLASS, css_defs[CSS_BODY]) )  /* Body ohne alles     */
      return true;
    }
  }
  else
  { if( *background )                                    /* Hintergrundbild angegeben? */
    { if( dosendf(PAGE_BODY_START_BG, background) )      /* Body mit Bild              */
        return true;
    }
    else if( *bodypars )
    { if( dosendf(PAGE_BODY_START_PAR, bodypars) )       /* Body mit Parameter         */
        return true;
    }
    else
    { if( dosend(PAGE_BODY_START) )                      /* Body ohne alles            */
      return true;
    }
  }
  return false;
}


/***************************************************************************************/
/* int send_headline(char *zeile)                                                      */
/*                   char *zeile: gelesene Zeile                                       */
/*     send_headline erzeugt die Ueberschriftzeile einer Seite                         */
/***************************************************************************************/
int send_headline(char *zeile)
{
  LOG(11, "send_headline\n");
  if( *zeile )
  { if( dosend(HEADLINE) )
      return true;
    do_scan(zeile, NOMENU);
    return dosend(HEADLINE_ENDE);
  }
  return false;
}
#endif  /* #ifndef COMPILER */
