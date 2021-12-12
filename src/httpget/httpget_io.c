/*******************************************************/
/* File: httpget_io.c                                  */
/* main function and variables                         */
/* timestamp: see COMPILEDATE                          */
/*******************************************************/

#include "httpget.h"


/***************************************************************************************/
/* int binsend(char *s, size_t nb)                                                     */
/*             char *s  : string                                                       */
/*             size_t nb: Anzahl Zeichen, die gesendet werden sollen                   */
/*             return   : true bei fehler, sonst false                                 */
/*     binsend sendet die Daten aus s an den socket sh und testet auf Fehler           */
/***************************************************************************************/
int binsend(char *s, size_t nb)
{ size_t b, sb;

  LOG(1, "binsend, s: %.20s, nb: %ld\n", s, (long)nb);

  if( nb > 0 )
  { for( b = 0; b < nb; )
    {
      LOG(3, "binsend, b: %ld\n", (long)b);
#ifdef WITH_HTTPS
      if( ssl_mode )
        sb = SSL_write(ssl, s+b, nb-b);
      else
#endif  /* WITH_HTTPS */
        sb = send(sh, s+b, nb-b, 0);
      if( -1 == sb )
      { if( logflag & NORMLOG )
          perror("send");
        return true;
      }
      b += sb;
    }
  }

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
/* int send_chunk(void)                                                                */
/*     send_chunk sendet einen Chunk                                                   */
/***************************************************************************************/
int send_chunk(void)
{ char chunkheader[10];

  LOG(1, "send_chunk.\n");

  if( chunkpointer > chunkbuffer )
  { if( !header_send_flag )
    { header_send_flag = true;
      if( http_v >= 2 )
      { if( keepalive_flag )
        { if( bsend("Connection: Keep-Alive" NL) )
            return true;
        }
        else
        { if( bsend("Connection: close" NL) )
            return true;
        }
        if( bsend("Transfer-Encoding: chunked" NL NL) )
          return true;
        chunk_header_flag = true;
      }
      else
      { if( bsend("Connection: close" NL NL) )
          return true;
        keepalive_flag = false;
        chunk_header_flag = false;
      }
    }

    if( chunk_header_flag )
    { snprintf(chunkheader, 10, "%x" NL, (unsigned int)(chunkpointer-chunkbuffer));
      if( bsend(chunkheader)
          || binsend(chunkbuffer, chunkpointer-chunkbuffer)
          || binsend(NL, 2) )
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
/* int dosend(char *s)                                                                 */
/*             char *s: string soll gesendet werden                                    */
/*             return : true bei fehler, sonst false                                   */
/***************************************************************************************/
int dosend(char *s)
{ LOG(1, "dosend.\n");

  return chunksend(s, strlen(s));
}


/***************************************************************************************/
/* int bsendf(char *f, ...)                                                           */
/*             char *f: Format string                                                  */
/*             return : true bei fehler, sonst false                                   */
/*     bsendf sendet den string s an den socket sh und testet auf Fehler              */
/***************************************************************************************/
int bsendf(char *f, ...)
{ char outzeile[MAX_ZEILENLAENGE];
  va_list ap;

  LOG(1, "bsendf, f: %s.\n", f);

  va_start(ap, f);
#ifdef HAS_VSNPRINTF
  vsnprintf(outzeile, MAX_ZEILENLAENGE, f, ap);
#else
  vsprintf(outzeile, f, ap);
#endif
  va_end(ap);

  return bsend(outzeile);
}


/***************************************************************************************/
/* int send_last_chunk(void)                                                           */
/*     send_last_chunk sendet letzten Chunk                                            */
/***************************************************************************************/
int send_last_chunk(void)
{ LOG(1, "send_last_chunk.\n");

  if( !header_send_flag )
  { if( bsendf("Connection: %s" NL "Content-Length: %ld" NL NL,
                keepalive_flag ? "Keep-Alive" : "close",
                chunkpointer - chunkbuffer) )
      return true;
    header_send_flag = true;
    if( binsend(chunkbuffer, chunkpointer - chunkbuffer) )
      return true;
    chunkpointer = chunkbuffer;
  }
  else
  { if( chunk_header_flag )
      return send_chunk() || binsend("0" NL NL, 5);
    else
      return send_chunk();
  }
  return false;
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

  LOG(1, "chunksend, s: %.50s.\n", s);

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


/***************************************************************************************/
/* long read_data(char *buffer, long n, int timeout)                                   */
/*                char *buffer : Platz fuer die Daten                                  */
/*                long n       : n Byte Platz in buffer                                */
/*                int timeout  : Timeout in Sekunden, die gewartet wird                */
/*                return       : -1 bei Fehler, sonst Anzahl gelesener Zeichen         */
/*      read_data liest aus rh in den Puffer buffer                                    */
/***************************************************************************************/
long read_data(char *buffer, long n, int timeout)
{ long ret;                                       /* Anzahl empfangener Zeichen        */
  fd_set fds;                                     /* Set von Filedescriptoren          */
  struct timeval tv;                              /* Timeval Struktur                  */
#ifdef WITH_HTTPS
  time_t end_time;
#endif

  LOG(1, "read_data, angeforderte Bytes: %ld, timeout: %d.\n", n, timeout);

  if( n <= 0 )                                    /* kein Platz, aber kein Fehler      */
    return 0;

#ifdef WITH_HTTPS
  if( ssl_mode )
  { int ssl_errno, ust, selret;

    if( (ret = SSL_read(ssl, buffer, n)) > 0 )
    { LOG(4, "read_data, ssl-read bytes: %ld, first: <%x>.\n",
          ret, (unsigned char)*buffer);

      return ret;
    } 
    ssl_errno = SSL_get_error(ssl, ret);
    LOG(90, "read_data, ssl_read - ret: %d, err: %d.\n", ret, ssl_errno);
    if( ssl_errno == SSL_ERROR_NONE )
      return 0;
    ust = 0;
    end_time = time(NULL) + timeout*3;
    while( ssl_errno == SSL_ERROR_WANT_READ && time(NULL) < end_time )
    { do
      { FD_ZERO(&fds);                          /* Alle fds auf Null                 */
        FD_SET(sh, &fds);                       /* Bit fuer sh setzen                */
        tv.tv_sec = timeout;                    /* Timeout                           */
        tv.tv_usec = timeout_usecs;

        selret = select(sh+1, &fds, NULL, NULL, &tv);  /* sind Daten vorhanden?      */
      }while( selret < 0 && errno == EINTR );
      if( (ret = SSL_read(ssl, buffer, n)) > 0 )
      { LOG(4, "read_data, ssl-read bytes: %ld, first: <%x>.\n",
            ret, (unsigned char)*buffer);

        return ret;
      }
      if( ret == 0 )                         /* Fehler oder close               */
        return -ERROR_RECEIVE;
      ssl_errno = SSL_get_error(ssl, ret);
      LOG(90, "read_data, ssl_read - ret: %d, errno: %d.\n", ret, ssl_errno);
      ust++;
      usleep(ust*ust*1000);
    }
    return -ERROR_TIMEOUT;
  }
#endif  /* WITH_HTTPS */


  do
  { FD_ZERO(&fds);                              /* Alle fds auf Null                 */
    FD_SET(sh, &fds);                           /* Bit fuer sh setzen                */
    tv.tv_sec = timeout;                        /* Timeout                           */
    tv.tv_usec = timeout_usecs;

    ret = select(sh+1, &fds, NULL, NULL, &tv);  /* sind Daten vorhanden?             */
  }while( ret < 0 && errno == EINTR );  /* oder nur Fehler EINTR       */

  if( ret < 0 && logflag & NORMLOG )
    perror("read_data");

  if( ret > 0 )                                    /* Daten zum Lesen vorhanden?       */
  {
    { do
      { ret = read(sh, buffer, n);                 /* Einlesen                         */
      } while( ret == -1 && errno == EINTR );
      LOG(4, "read_data, ret: %ld.\n", ret);
    }

    if( ret < 0 )                                  /* Fehler?                          */
    { if( logflag & NORMLOG )
        perror("read");
      return -ERROR_RECEIVE;
    }
    else if( ret == 0 )                             /* keine Daten, Socket wurde      */
      return -ERROR_RECEIVE;                         /* vom Browser geschlossen        */
    return ret;
  }
  return -ERROR_TIMEOUT;
}
