/********************************************/
/* File: u2w_get.c                          */
/* Funktionen zum Auswerten der Anforderung */
/* timestamp: 2014-11-01 23:37:49           */
/********************************************/


#include "u2w.h"

#ifndef COMPILER

#ifdef WEBSERVER

/***************************************************************************************/
/* void conv_get_path(char *s)                                                         */
/*               char *s: String, der konvertiert werden soll                          */
/*      conv_get_path wertet Sonderzeichen im Pfad aus                                 */
/***************************************************************************************/
void conv_get_path(char *s)
{ char *in, *out;
  int i;

  in = out = s;
  while( *in )
  { if( *in == '%' && 1 == sscanf(in, "%%%2x", &i) ) /* EscapeZeichen                  */
    {
#ifdef UNIX
        if( i != 13 )                        /* Unter Unix CR nicht auswerten          */
#endif
          *out++ = (char)i;
      in += 3;
    }
    else
      *out++ = *in++;
  }
  *out = '\0';
}


/***************************************************************************************/
/* int getcontdisp(char *wert, char *buffer, char *g, long n)                          */
/*                 char   *wert: Wert zum Attribut                                     */
/*                 char *buffer: vom Client empfangene Anforderung                     */
/*                 char *g     : String, der gesucht werden soll                       */
/*                 long n      : Anzahl Zeichen Platz in wert                          */
/*                 return      : true, wenn g gefunden wurde                           */
/*     getcontdisp extrahiert aus dem vom Client empfangenen String den Eintrage zu g  */
/*                 z. B. *g = "name=" den Namen des multipart/content Feldes           */
/***************************************************************************************/
int getcontdisp(char *wert, char *buffer, char *g, long n)
{ char *p;                                    /* zeigt auf *g im Eingangspuffer        */

  LOG(1, "getcontdisp, buffer: %s, g: %s.\n", buffer, g);

  if( NULL != (p = strcasestr(buffer, g)) )   /* *g suchen                          */
  { p += strlen(g);                           /* p ans Ende von *g                     */
    strcpyn_s(wert, &p, n);
    return true;
  }
  return false;
}
#endif  /* #ifdef WEBSERVER */


/***************************************************************************************/
/* void code_pars(char *s)                                                             */
/*                char *s: String, der konvertiert werden soll                         */
/*      code_pars ersetzt ? und & durch ! und $                                        */
/***************************************************************************************/
void code_pars(char *s)
{
  while( *s )
  { switch( *s )
    { case '?': *s++ = '!';
                break;
      case '&': *s++ = '$';
                break;
      default:  s++;
    }
  }
}


/***************************************************************************************/
/* void decode_pars(char *out, char *in)                                               */
/*                  char *out: hier wird der Dateiname abgelegt                        */
/*                  char  *in: der zu konvertierende Dateiname                         */
/*      decode_pars ersetzt ! und $ durch ? und &                                      */
/***************************************************************************************/
void decode_pars(char *out, char *in)
{
  while( *in )
  { switch( *in )
    { case '!': *out++ = '?';
                in++;
                break;
      case '$': *out++ = '&';
                in++;
                break;
      default:  *out++ = *in++;
    }
  }
  *out = '\0';
}
#endif  /* #ifndef COMPILER */
