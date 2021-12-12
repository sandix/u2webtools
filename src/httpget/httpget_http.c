/*******************************************************/
/* File: httpget_http.c                                */
/* main function and variables                         */
/* timestamp: see COMPILEDATE                          */
/*******************************************************/

#include "httpget.h"


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
/* int test_http_header(char *b)                                                       */
/*                      char *b: vom Browser gelesene Zeichen                          */
/*                      return : true, wenn HTTP-Header komplett gelesen               */
/***************************************************************************************/
int test_http_header(char *b)
{ char *p;

  LOG(1, "test_http_header: %s.\n", b);

  if( NULL != strstr(b, "\n\n") || NULL != strstr(b, "\r\n\r\n")
      || NULL != strstr(b, "\r\r") )
    return true;
  p = b;
  while( *p && *p != ' ' && *p != '\t' )
    p++;
  if( !*p )
    return false;
  while( *p && (*p == ' ' || *p == '\t') )
    p++;
  if( !*p )
    return false;
  while( *p && *p != ' ' && *p != '\n' && *p != '\r' )
    p++;
  if( *p == '\n' || *p == '\r' )
    return true;
  return false;
}


/***************************************************************************************/
/* char *content_start(char *b, long n)                                                */
/*                     char *b: gelesene Zeichen vom Browser                           */
/*                     long n : Zeichen in b                                           */
/*                     return : Pointer auf Anfang des Content-Blockes                 */
/*       content_start sucht in b den Anfang des Content-Blockes, dabei wird nach      */
/*                     einer Leerzeile gesucht                                         */
/***************************************************************************************/
char *content_start(char *b, long n)
{ long i;

  if( n < 6 || b[0] != 'H' || b[1] != 'T' || b[2] != 'T'
      || b[3] != 'P' || b[4] != '/' || b[5] != '1' )
    return b;

  i = 0;
  while( i+1 < n )                           /* Es muss noch Platz für 2 Zeichen sein  */
  { if( *b == '\xd' )
    { if( *(b+1) == '\xd' )
        return b+2;
      else if( *(b+1) == '\xa' )
      { if( i+3 < n )
        { if( *(b+2) == '\xd' && *(b+3) == '\xa' )
            return b+4;
	}
        else
          return NULL;
      }
    }
    else if( *b == '\xa' )
    { if( *(b+1) == '\xa' )
	return b+2;
    }
    b++;
    i++;
  }
  return NULL;
}

/***************************************************************************************/
/* ssize_t send_http(char *b, size_t len)                                              */
/*                char *b        : Zeiger auf zu sendende Zeichen                      */
/*                size_t len     : Anzahl zu sendender Zeichen                         */
/*      send_http liefert Anzahl gesendeter Zeichen                                    */
/***************************************************************************************/
ssize_t send_http(char *b, size_t len)
{
#ifdef DEBUG
  if( log_send_flag )
    fprintf(stderr, "send_http, b: %s.\n", b);
#endif
#ifdef WITH_HTTPS
  if( ssl_mode )
    return SSL_write(ssl, b, len);
  else
#endif  /* WITH_HTTPS */
    return send(sh, b, len, 0);
}

int test_cr(char *s, int n)
{ while( n > 0 )
    if( s[n--] == CR )
      return true;
  return false;
}

/***************************************************************************************/
/* long read_http(char **b, int read_mode)                                             */
/*                char **b       : Zeiger auf gelesene Zeichen setzen                  */
/*                int read_mode: 1, Header zurueck, 0: Daten, 2: Daten bis Timeout     */
/*      read_http liefert Anzahl gelesener Zeichen, -1 bei Fehler                      */
/***************************************************************************************/
long read_http(char **b, int read_mode)
{ static char buffer[MAXDATASIZE+1];         /* Platz fuer empfangene Zeichen          */
  static char *bp = buffer;                  /* aktuelle Leseposition                  */
  static ULONG cl;                           /* Content-length                         */
  static int cl_flag;                        /* Content-length gefunden?               */
  static ULONG cr;                           /* Content received                       */
  static long nb;                            /* Anzahl Zeichen im buffer               */
  static int chunk_flag;                     /* true, Chunks lesen                     */
  long nnb;                                  /* Anzahl gelesene Zeichen                */
  long chunk_len;                            /* Länge des Chunks                       */
  char *p;                                   /* zum Suchen                             */
  unsigned long ncr;                         /* content received für Rückgabe          */

  LOG(1, "read_http, read_mode: %d, chunk_flag: %d.\n", read_mode, chunk_flag);
  LOG(3, "read_http, cl: " ULONGMUSTER ", cr: " ULONGMUSTER ", nb: %ld.\n", cl, cr, nb);

  if( read_mode == 1 )                       /* soll Header gelesen werden?            */
  { if( 0 > (nb = read_data(buffer, MAXDATASIZE, timeout_secs)) )
      return nb;
    buffer[nb] = '\0';
    LOG(9, "read_http, nb: %ld, buffer: %s.\n", nb, buffer);
    bp = buffer;
    nnb = 1;
    while( nnb > 0 && (nb < 1 || buffer[0] == 'H') && (nb < 2 || buffer[1] == 'T')
          && (nb < 3 || buffer[2] == 'T') && (nb < 4 || buffer[3] == 'P')
          && (nb < 5 || buffer[4] == '/') && (nb < 6 || buffer[5] == '1')
          && !test_http_header(buffer) )
    { nnb = read_data(buffer + nb, MAXDATASIZE - nb, timeout_secs);
      if( nnb < 0 )
        return nnb;
      else
      { nb += nnb;
        buffer[nb] = '\0';
      }
    }
    cr = 0;
    LOG(3, "read_http, nb: %ld.\n", nb);
    *b = buffer;
    if( nb < 6 || buffer[0] != 'H' || buffer[1] != 'T' || buffer[2] != 'T'
        || buffer[3] != 'P' || buffer[4] != '/' || buffer[5] != '1' )
      return 0;
    return nb;                               /* nicht Content-Block, dann ganzen puffer*/
  }
  else
  { if( cr == 0 )
    { if( (chunk_flag = strcasestr(buffer, "Transfer-Encoding: chunked") != NULL) )
      { cr = cl = 0;
      }
      if( NULL == (p = strcasestr(buffer, "Content-length:")) )
        cl_flag = false;                     /* Content-length nicht gefunden          */
      else
      { cl_flag = true;
        sscanf(p+strlen("Content-length:"), ULONGMUSTER, &cl);  /* Content Laenge bestimmen  */
      }
      if( NULL != (p = content_start(buffer, nb)) )  /* Content Anfang bestimmen       */
      { bp = p;
        nb = nb - (p-buffer);                /* Anzahl bis jetzt gelesener Zeichen     */
      }
    }
    else if( !chunk_flag )
    { bp = buffer;
      nb = 0;
    }
    else if( nb == 0 )
      bp = buffer;

    if( chunk_flag && cr == cl )             /* Es muss ein neuer Chunk-Header gelesen */
    { if( nb == 0 )                          /* werden                                 */
      { bp = buffer;
        if( 0 >= (nb = read_data(bp, MAXDATASIZE, timeout_secs)) )
          return nb;
      }

      if( *bp == CR )
      { bp++;
        nb--;
      }
      if( nb == 0 )
      { bp = buffer;
        if( 0 >= (nb = read_data(bp, MAXDATASIZE, timeout_secs)) )
          return nb;
      }
      if( *bp == LF )
      { bp++;
        nb--;
      }
      if( nb == 0 )
      { bp = buffer;
        if( 0 >= (nb = read_data(bp, MAXDATASIZE, timeout_secs)) )
          return nb;
      }
      if( nb < 5 && !test_cr(bp, nb) )
      { int i;
        if( bp != buffer )
        { for(i = 0; i < nb; i++)
            buffer[i] = *bp++;
          bp = buffer;
        }
        if( 0 >= (nnb = read_data(bp+nb, MAXDATASIZE-nb, timeout_secs)) )
          return nnb;
        nb += nnb;
      }
      sscanf(bp, "%lx", &chunk_len);
      LOG(3, "read_http, chunk_len: %ld.\n", chunk_len);
      if( chunk_len == 0 )
        return 0;
      cl += chunk_len;
      while( *bp != CR && *bp != LF )
      { bp++;
        nb--;
        if( nb == 0 )
        { bp = buffer;
          if( 0 >= (nb = read_data(bp, MAXDATASIZE, timeout_secs)) )
            return nb;
        }
      }
      if( *bp == CR )
      { bp++;
        nb--;
      }
      if( nb == 0 )
      { bp = buffer;
        if( 0 >= (nb = read_data(bp, MAXDATASIZE, timeout_secs)) )
          return nb;
      }
      if( *bp == LF )
      { bp++;
        nb--;
      }
      cl_flag = true;
    }
    while( (!cl_flag || cr+nb < cl)         /* C-B komplett gelesen?                   */
           && nb < MAXDATASIZE-1-(bp-buffer) ) /* noch Platz im Puffer?                */
    { nnb = MAXDATASIZE-(bp-buffer)-nb;     /* Platz im Puffer                         */
      if( cl_flag && cl - cr - nb < nnb )   /* soviel muss noch gelesen werden         */
        nnb = cl - cr - nb;
      LOG(3, "read_http, nnb1: %ld.\n", nnb);
      if( nnb > 0 )
      { if( 0 >= (nnb = read_data(bp+nb, nnb,
                                  (read_mode == 2 && ! cl_flag)
                                  ? 5
                                  : timeout_secs)) )
        { if( nb > 0 )                      /* noch was im Puffer?                     */
          { nnb = nb;                       /* Puffergröße merken                      */
            nb = 0;                         /* für nächsten Aufruf merken              */
            *b = bp;
            bp += nnb;
            return nnb;                     /* Anzahl Zeichen zurück                   */
          }
          else
            return nnb;                     /* nichts im Puffer, dann Feher zurück     */
        }
      }
      nb += nnb;                            /* Gesamtzahl Zeichen im Puffer            */
    }

    LOG(3, "read_http NW, cr: " ULONGMUSTER ", cl: " ULONGMUSTER ", nb: %ld.\n",
        cr, cl, nb);
    if( cl_flag && nb > cl - cr )           /* größer als Content Länge?               */
    { ncr = cl - cr;
      cr = cl;
    }
    else
    { ncr = nb;
      cr += nb;
    }
    nb -= ncr;

    LOG(2, "/read_http, ncr: %ld.\n", ncr);
    *b = bp;
    bp += ncr;
    return ncr;                              /* Anzahl Bytes zurueck                   */
  }
}


/***************************************************************************************/
/* int gethcont(char *wert, char *buffer, char *g, long n)                             */
/*              char   *wert: Wert zum Attribut                                        */
/*              char *buffer: vom Client empfangene Anforderung                        */
/*              char *g     : String, der gesucht werden soll                          */
/*              long n      : Anzahl Zeichen Platz in wert                             */
/*              return      : true, wenn Zeile gefunden wurde                          */
/*     gethcont extrahiert aus dem vom Client empfangenen String den Eintrage zu g     */
/*              z. B. mit *g = "Authorization" den User und Passwort                   */
/***************************************************************************************/
int gethcont(char *wert, char *buffer, char *g, long n)
{ char *p;                                    /* zeigt auf *g im Eingangspuffer        */
  char *w;                                    /* zum Aufbauen des angeforderten Files  */
  long i;

  w = wert;
  if( NULL != (p = strcasestr(buffer, g)) )   /* *g suchen                             */
  { p += strlen(g);                           /* p ans Ende von *g                     */
    while( *p == ' ' )                        /* Leerzeichen ueberspringen             */
      p++;
    i = 1;
    while( *p >= ' ' && i++ < n )             /* Alle druckbaren Zeichen nach wert     */
      *w++ = *p++;
    *w = '\0';
    return true;
  }
  return false;
}


void codeurl(char *url, char *name)
{ unsigned char *u, *n;
  unsigned char a[4];

  u = (unsigned char *)url;
  n = (unsigned char *)name;

  while( *n )
  { if( *n == '\\' )
    { if( *(n+1) == '\\' )
        n++;
      else if( *(n+1) == '%' )
      { n++;
        *u++ = *n++;
        continue;
      }
      else if( *(n+1) == '&' )
      { n += 2;
        *u++ = '%';
        *u++ = '2';
        *u++ = '6';
        continue;
      }
    }
    if( *n != '&' && *n != '.' && (*n < '/' || *n > '~') )
    { *u++ = '%';
      snprintf((char *)a, 3, "%02x", *n);
      *u++ = a[0];
      *u++ = a[1];
      n++;
    }
    else
      *u++ = *n++;
  }
  *u = '\0';
}
