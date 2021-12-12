/**************************************************/
/* File: u2w_send.c                               */
/* Funktionen zum Senden von Daten an den Browser */
/* timestamp: 2015-10-10 10:22:15                 */
/**************************************************/


#include "u2w.h"

#ifdef WEBSERVER

char chunkbuffer[MAX_LEN_CHUNKS];
char *chunkpointer = chunkbuffer;
int chunk_header_flag;

/***************************************************************************************/
/* int send_chunk(void)                                                                */
/*     send_chunk sendet einen Chunk                                                   */
/***************************************************************************************/
int send_chunk(void)
{ char chunkheader[10];

  LOG(10, "send_chunk, http_head_flag: %d, chunk_header_flag: %d, http_v: %d\n",
      http_head_flag, chunk_header_flag, http_v);

  if( chunkpointer > chunkbuffer )
  { if( http_head_flag < 4 )
    { http_head_flag = 7;
      if( http_v >= 2 )
      { if( keepalive_flag )
        { if( bsend("Connection: Keep-Alive" CRLF) )
            return true;
        }
        else
        { if( bsend("Connection: close" CRLF) )
            return true;
        }
        if( bsend("Transfer-Encoding: chunked" CRLF CRLF) )
          return true;
        chunk_header_flag = true;
      }
      else if( http_v )
      { if( bsend("Connection: close" CRLF CRLF) )
          return true;
        keepalive_flag = false;
        chunk_header_flag = false;
      }
    }

    if( chunk_header_flag )
    { snprintf(chunkheader, 10, "%x" CRLF, (int)(chunkpointer-chunkbuffer));
      if( bsend(chunkheader)
          || binsend(chunkbuffer, chunkpointer-chunkbuffer)
          || binsend(CRLF, 2) )
        return true;
    }
    else
    { if( binsend(chunkbuffer, chunkpointer-chunkbuffer) )
        return true;
    }
    chunkpointer = chunkbuffer;
  }
  return false;
}


/***************************************************************************************/
/* int send_last_chunk(void)                                                           */
/*     send_lst_chunk sendet letzten Chunk                                             */
/***************************************************************************************/
int send_last_chunk(void)
{
  LOG(20, "send_last_chunk, http_head_flag: %d.\n", http_head_flag);

  if( http_head_flag < 4 )
  { http_head_flag = 8;
    if( dosendf("Connection: %s" CRLF "Content-Length: %ld" CRLF CRLF,
                keepalive_flag ? "Keep-Alive" : "close",
                chunkpointer - chunkbuffer) )
      return true;

    if( binsend(chunkbuffer, chunkpointer - chunkbuffer) )
      return true;
    chunkpointer = chunkbuffer;
    return false;
  }
  else
  { if( chunk_header_flag )
      return send_chunk() || binsend("0" CRLF CRLF, 5);
    else
      return send_chunk();
  }
}


/***************************************************************************************/
/* int chunksend(char *s, size_t nb)                                                   */
/*             char *s  : string                                                       */
/*             size_t nb: Anzahl Zeichen, die gesendet werden sollen                   */
/*             return   : true bei fehler, sonst false                                 */
/*     chunksend sendet die Daten aus s an den socket sh und testet auf Fehler         */
/***************************************************************************************/
int chunksend(char *s, size_t nb)
{ char *ss;

  LOGLOG(LOGSENDDATA, "chunksend: %s.\n", s);
  if( nb > 0 )
  { for( ss = s; ss-s < nb; )
    { if( chunkpointer - chunkbuffer >= MAX_LEN_CHUNKS )
      { if( send_chunk() )
          return true;
      }
      *chunkpointer++ = *ss++;
    }
  }
  return false;
}
#endif  /* #ifdef WEBSERVER */

/***************************************************************************************/
/* int binsend(char *s, size_t nb)                                                     */
/*             char *s  : string                                                       */
/*             size_t nb: Anzahl Zeichen, die gesendet werden sollen                   */
/*             return   : true bei fehler, sonst false                                 */
/*     binsend sendet die Daten aus s an den socket sh und testet auf Fehler           */
/***************************************************************************************/
int binsend(char *s, size_t nb)
{ size_t b, sb;

  LOG(1, "binsend, nb: %d\n", nb);
  LOGLOG(LOGSENDDATA, "binsend: %s.\n", s);

  if( nb > 0 )
  { for( b = 0; b < nb; )
    {
      LOG(3, "binsend, b: %d\n", b);
#ifdef WEBSERVER
#ifdef WITH_HTTPS
      if( ssl_mode )
        sb = SSL_write(ssl, s+b, nb-b);
      else
#endif  /* WITH_HTTPS */
#ifdef MSG_NOSIGNAL
        sb = send(sh, s+b, nb-b, MSG_NOSIGNAL);
#else
        sb = send(sh, s+b, nb-b, 0);
#endif
#else
      sb = write(OH, s+b, nb-b);
#endif
      if( -1 == sb )
      { if( logflag & NORMLOG )
          perror("send");
        return true;
      }
      b += sb;
    }
#ifdef WITH_MMAP
  if( status_mode & (STAT_SEND|STATDIFF_SEND) )
    status_counter->bytessend += nb;
#endif
  }
  LOG(2, "/binsend\n");
  return false;
}


/***************************************************************************************/
/* int bsend(char *s)                                                                 */
/*             char *s: string soll gesendet werden                                    */
/*             return : true bei fehler, sonst false                                   */
/***************************************************************************************/
int bsend(char *s)
{ LOG(1, "bsend, s: %s\n", s);
  return binsend(s, strlen(s));
}


/***************************************************************************************/
/* int dosend(char *s)                                                                 */
/*             char *s: string soll gesendet werden                                    */
/*             return : true bei fehler, sonst false                                   */
/***************************************************************************************/
int dosend(char *s)
{
#ifdef WEBSERVER
  LOG(1, "dosend, http_head_flag: %d, strlen: %d.\n", http_head_flag, strlen(s));
  if( http_head_flag & 2 )
    return chunksend(s, strlen(s));
  else
#endif
    return binsend(s, strlen(s));
}


/***************************************************************************************/
/* int dosendh(char *s)                                                                */
/*             char *s: string soll gesendet werden                                    */
/*             return : true bei fehler, sonst false                                   */
/***************************************************************************************/
int dosendh(char *s)
{
  if( u2w_mode == U2W_MODE )
  { char z[2*MAX_ZEILENLAENGE], *zp;

    zp = z;
    while( *s && zp-z < 2*MAX_ZEILENLAENGE )
    { if( *s == '<' )
      { *zp++ = '&';
        *zp++ = 'l';
        *zp++ = 't';
        *zp++ = ';';
        s++;
      }
      else if( *s == '>' )
      { *zp++ = '&';
        *zp++ = 'g';
        *zp++ = 't';
        *zp++ = ';';
        s++;
      }
      else if( *s == '&' )
      { *zp++ = '&';
        *zp++ = 'a';
        *zp++ = 'm';
        *zp++ = 'p';
        *zp++ = ';';
        s++;
      }
      else
        *zp++ = *s++;
    }
#ifdef WEBSERVER
    LOG(1, "dosendh, http_head_flag: %d, strlen: %d.\n", http_head_flag, zp-z);
    if( http_head_flag > 1 )
      return chunksend(z, zp-z);
    else
#endif
      return binsend(z, zp-z);
 }
 else
 {
#ifdef WEBSERVER
    LOG(1, "dosendh, http_head_flag: %d, strlen: %d.\n", http_head_flag, strlen(s));
    if( http_head_flag > 1 )
      return chunksend(s, strlen(s));
    else
#endif
      return binsend(s, strlen(s));
 }
}


/***************************************************************************************/
/* int dobinsend(char *s, size_t nb)                                                   */
/*             char *s: string soll gesendet werden                                    */
/*             size_t nb: Anzahl Zeichen                                               */
/*             return : true bei fehler, sonst false                                   */
/***************************************************************************************/
int dobinsend(char *s, size_t nb)
{
  if( nb > 0 )
  {
#ifdef WEBSERVER
    if( http_head_flag > 1 )
      return chunksend(s, nb);
    else
#endif
      return binsend(s, nb);
  }
  return false;
}


/***************************************************************************************/
/* int dobinsendh(char *s, size_t nb)                                                  */
/*             char *s: string soll gesendet werden                                    */
/*             size_t nb: Anzahl Zeichen                                               */
/*             return : true bei fehler, sonst false                                   */
/***************************************************************************************/
int dobinsendh(const char *s, size_t nb)
{
  if( nb > 0 )
  { if( u2w_mode == U2W_MODE )
    { char z[2*MAX_ZEILENLAENGE], *zp, *sp;

      zp = z;
      sp = s;
      while( sp - s < nb && zp-z < 2*MAX_ZEILENLAENGE )
      { if( *sp == '<' )
        { *zp++ = '&';
          *zp++ = 'l';
          *zp++ = 't';
          *zp++ = ';';
          sp++;
        }
        else if( *sp == '>' )
        { *zp++ = '&';
          *zp++ = 'g';
          *zp++ = 't';
          *zp++ = ';';
          sp++;
        }
        else if( *sp == '&' )
        { *zp++ = '&';
          *zp++ = 'a';
          *zp++ = 'm';
          *zp++ = 'p';
          *zp++ = ';';
          sp++;
        }
        else
          *zp++ = *sp++;
      }
#ifdef WEBSERVER
      if( http_head_flag > 1 )
        return chunksend(z, zp-z);
      else
#endif
        return binsend(z, zp-z);
    }
    else
    {
#ifdef WEBSERVER
      if( http_head_flag > 1 )
        return chunksend(s, nb);
      else
#endif
        return binsend(s, nb);
    }
  }
  return false;
}


/***************************************************************************************/
/* int dosendf(char *f, ...)                                                           */
/*             char *f: Format string                                                  */
/*             return : true bei fehler, sonst false                                   */
/*     dosendf sendet den string s an den socket sh und testet auf Fehler              */
/***************************************************************************************/
int dosendf(char *f, ...)
{ char outzeile[MAX_ZEILENLAENGE];
  va_list ap;

  LOG(1, "dosendf, f: %s.\n", f);

  va_start(ap, f);
#ifdef HAS_VSNPRINTF
  vsnprintf(outzeile, MAX_ZEILENLAENGE, f, ap);
#else
  vsprintf(outzeile, f, ap);
#endif
  va_end(ap);

  return dosend(outzeile);
}


/***************************************************************************************/
/* int dosendhf(char *f, ...)                                                          */
/*             char *f: Format string                                                  */
/*             return : true bei fehler, sonst false                                   */
/*     dosendhf sendet den string f an den socket sh und testet auf Fehler             */
/***************************************************************************************/
int dosendhf(char *f, ...)
{ char outzeile[MAX_ZEILENLAENGE];
  char *s, *o, *p;
  int i;
  va_list ap;

  LOG(1, "dosendh, f: %s.\n", f);

  va_start(ap, f);

  s = f;
  o = outzeile;
  while( *s && o-outzeile < MAX_ZEILENLAENGE )
  { if( *s == '%' )
    { s++;
      switch(*s)
      { case 's': p = va_arg(ap, char *);
                  while( *p )
                    *o++ = *p++;
                  break;
        case 'd': i = va_arg(ap, int);
                  snprintf(o, MAX_ZEILENLAENGE-(o-outzeile), "%d", i);
                  o += strlen(o);
                  break;
        case 'H': p = va_arg(ap, char *);
                  while( *p )
                  { switch( *p )
                    { case '<': p++;
                                strcpyn_z(&o, "&lt;", MAX_ZEILENLAENGE-(o-outzeile));
                                break;
                      case '>': p++;
                                strcpyn_z(&o, "&gt;", MAX_ZEILENLAENGE-(o-outzeile));
                                break;
                      case '&': p++;
                                strcpyn_z(&o, "&amp;", MAX_ZEILENLAENGE-(o-outzeile));
                                break;
                      default:  *o++ = *p++;
                    }
                  }
                  break;
        default : *o++ = *s;
      }
      s++;
    }
    else
      *o++ = *s++;
  }
  *o = '\0';

  va_end(ap);

  LOG(2, "/dosendh, outzeile: %s.\n", outzeile);

  return dosend(outzeile);
}


#ifdef WEBSERVER
/***************************************************************************************/
/* int send_options(int getflag, int postflag, int putflag)                            */
/*                  int getflag: true, HET ist erlaubt                                 */
/*                  int postflag: true, POST ist erlaubt                               */
/*                  int putflag : true, PUT ist erlaubt                                */
/*     send_options sendet die HTTP/1.1 Antowrt auf OPTIONS                            */
/***************************************************************************************/
int send_options(int getflag, int postflag, int putflag)
{ time_t tt;                                                /* Aktuelle Zeit/Datum     */

  time(&tt);                                                /* Jetzt                   */
  return dosendf(OPTIONS_RETURN, httptime(gmtime(&tt)),
           keepalive_flag ? "Keep-Alive" : "close",
           getflag ? "GET, HEAD, " : "",
           putflag ? ", PUT" : "",
           postflag ? ", POST" : "",
           no_trace_flag ? "" : ", TRACE");                 /* als String einfuegen    */
}
#endif  /* #ifdef WEBSERVER */
