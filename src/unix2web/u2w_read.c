/*************************************************/
/* File: u2w_read.c                              */
/* Funktionen zum Lesen der Daten vom Browser    */
/* timestamp: 2015-10-10 10:22:05                */
/*************************************************/


#include "u2w.h"


/* Anfang globale Variablen */
#ifdef WEBSERVER
char httpheader[MAX_HTTPHEADER_SIZE];        /* Platz fuer HTTP-Header                 */
unsigned LONGWERT content_length;            /* Content-length                         */
#endif
/* Ende globale Variablen */


#ifdef WEBSERVER

char *content_buffer;
char *content_malloc_buffer = NULL;
unsigned long content_buffer_size;
char *read_header_pos;                       /* aktuelle Leseposition                  */
static unsigned LONGWERT content_received;   /* Content received (pro Chunk)           */
long bytes_in_content_buffer;                /* Anzahl Zeichen im buffer               */
int read_chunk_flag;                         /* true, Chunks lesen                     */
int cl_flag;                                 /* Content-length gefunden?               */


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
  extern char chunkbuffer[MAX_LEN_CHUNKS];        /* Sendepuffer zum Lesen verwenden   */
  extern char *chunkpointer;                      /* Position im chunkbuffer           */

  LOG(1, "read_data, angeforderte Bytes: %ld, timeout: %d.\n", n, timeout);

  if( n <= 0 )                                    /* kein Platz, aber kein Fehler      */
    return 0;

#ifdef WITH_HTTPS
  if( ssl_mode )
  { int ssl_errno, ust, selret;

    if( (ret = SSL_read(ssl, buffer, n)) > 0 )
    { LOG(4, "read_data, ssl-read bytes: %ld, first: <%x>.\n",
          ret, (unsigned char)*buffer);

      LOGLOG(LOGREADDATA, "read_data, buffer: %.*s.\n", (int)ret, buffer);
      return ret;
    } 
    ssl_errno = SSL_get_error(ssl, ret);
    if( ssl_errno == SSL_ERROR_NONE )
      return 0;
    ust = 0;
    while( ssl_errno == SSL_ERROR_WANT_READ && ++ust < 10 )
    { do
      { FD_ZERO(&fds);                            /* Alle fds auf Null                 */
        FD_SET(sh, &fds);                         /* Bit fuer sh setzen                */
        tv.tv_sec = timeout;                      /* Timeout                           */
        tv.tv_usec = timeout_usecs;

        selret = select(sh+1, &fds, NULL, NULL, &tv);    /* sind Daten vorhanden?      */
      }while( selret < 0 && errno == EINTR && !term_flag );
      if( (term_flag && chunkpointer == chunkbuffer) || term_flag > 1 )
                                                /* erst beenden, wenn Sendepuffer leer */
        return -1;
      if( (ret = SSL_read(ssl, buffer, n)) > 0 )
      { LOG(4, "read_data, ssl-read bytes: %ld, first: <%x>.\n",
            ret, (unsigned char)*buffer);

        LOGLOG(LOGREADDATA, "read_data, buffer: %.*s.\n", (int)ret, buffer);
        return ret;
      }
      ssl_errno = SSL_get_error(ssl, ret);
      if( selret <= 0 )                         /* Timeout oder Fehler               */
        break;
#ifdef OLDUNIX
      sleep(ust/100);
#else
      usleep(ust*ust*1000);
#endif
    }
    if( ssl_errno == SSL_ERROR_NONE || ssl_errno == SSL_ERROR_WANT_READ )
      return 0;
    else
      return -1;
  }
#endif  /* WITH_HTTPS */


  do
  { FD_ZERO(&fds);                              /* Alle fds auf Null                 */
    FD_SET(sh, &fds);                           /* Bit fuer sh setzen                */
    tv.tv_sec = timeout;                        /* Timeout                           */
    tv.tv_usec = timeout_usecs;

    ret = select(sh+1, &fds, NULL, NULL, &tv);  /* sind Daten vorhanden?             */
  }while( ret < 0 && errno == EINTR && !term_flag );  /* oder nur Fehler EINTR       */

  LOG(13, "read_data, ret: %d, term_flag: %d.\n", ret, term_flag);

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
        perror("recv");
      return -1;
    }
    else if( ret == 0 )                              /* keine Daten, Socket wurde      */
      return -1;                                     /* vom Browser geschlossen        */
  }
  else if( (term_flag && chunkpointer == chunkbuffer) || term_flag > 1 )
                                                /* erst beenden, wenn Sendepuffer leer */
    return -1;

  LOGLOG(LOGREADDATA, "read_data, buffer: %.*s.\n", (int)ret, buffer);
#ifdef WITH_MMAP
  if( status_mode & (STAT_RECEIVED|STATDIFF_RECEIVED) )
    status_counter->bytesreceived += ret;
#endif
  LOG(19, "/read_data, ret: %ld.\n", ret);
  return ret;
}


/***************************************************************************************/
/* int test_line(char *b, long n)                                                      */
/*               char *b: gelesene Zeichen vom Browser                                 */
/*               long n : Zeichen in b                                                 */
/*               return : true, wenn vollstaendige Zeile                               */
/*      test_line: testen, ob Zeilenende im Puffer                                     */
/***************************************************************************************/
inline int test_line(char *b, long n)
{
  while( n > 0 )
  { if( *b == '\r' || *b == '\n' )
      return true;
    n--;
    b++;
  }
  return false;
}


/***************************************************************************************/
/* int read_http_header(methodtype *method, char **par, char **auth_digest,            */
/*                      time_t *test_modified_time, char **range,                      */
/*                      int *gzip_flag, int *keepalive_found)                          */
/*         methodtype *method         : GET|POST|HEAD|OPTIONS|PUT|TRACE                */
/*         char **par                 : Nach '?', wenn vorhanden, sonst NULL           */
/*         char **auth_digest         : "Authorization: Digest"                        */
/*         time_t *test_modified_time : "If-Modified-Since:" als Unix-Timestamp        */
/*         char **range               : "Range:"                                       */
/*         int *gzip_flag             : 1, wenn gzip im "Accept-Encoding", sonst 0     */
/*         int *keepalive_found       : 1, "connection: keep-alive" gefunden, sonst 0  */
/*         return: -1 bei Fehler oder TRACE, 0 bei kein Content, sonst 1               */
/*      read_http_header HTTP-Header einlesen und auswerten. Content-Block bestimmen   */
/***************************************************************************************/
int read_http_header(methodtype *method, char **par,
                     char **auth_digest, time_t *test_modified_time, char **range,
                     int *gzip_flag, int *keepalive_found)
{ long nnb;                                  /* Anzahl gelesene Zeichen                */
  int nto;                                   /* number of Timeouts                     */
  int parflag;                               /* 1: hinter '?'                          */
  char *http_name, *http_value, *o;

  LOG(3, "read_http_header, keepalive_found: %d.\n", *keepalive_found);

  nto = 5;
                                                           /* Header lesen             */
  do
  { if( 0 > (bytes_in_content_buffer
             = read_data(httpheader, MAX_HTTPHEADER_SIZE, timeout_secs)) )
      return -1;
    if( bytes_in_content_buffer == 0 && --nto <= 0 )
      return -1;
  } while( bytes_in_content_buffer < 2 );

  LOG(9, "read_http_header, bytes_in_content_buffer: %ld, httpheader: %.30s.\n",
      bytes_in_content_buffer, httpheader);
  LOG(19, "read_http_header, httpheader: %s.500s.\n", httpheader);
  read_header_pos = httpheader;
  cl_flag = read_chunk_flag = false;
  content_buffer = http_host = NULL;
  content_length = 0;

  while( !test_line(read_header_pos, bytes_in_content_buffer) )
  { nnb = read_data(httpheader + bytes_in_content_buffer,
                    MAX_HTTPHEADER_SIZE - bytes_in_content_buffer,
                    timeout_secs);
    bytes_in_content_buffer += nnb;
    LOG(19, "read_http_header, nto: %d, bytes_in_content_buffer: %ld.\n",
        nto, bytes_in_content_buffer);
    if( nnb < 0 )
      return -1;
    else if( nnb == 0 && --nto <= 0 )    /* Timeout                        */
      return -1;
  }

  LOG(19, "read_http_header, nach test_line, read_header_pos: %.10s\n",
      read_header_pos);
  methodstr = read_header_pos;
  if( *read_header_pos == 'G' && *(read_header_pos + 1) == 'E'
      && *(read_header_pos + 2) == 'T' && *(read_header_pos + 3) == ' ' )
  { *(read_header_pos + 3) = '\0';
    read_header_pos += 4;
    *method = GET;
  }
  else if( *read_header_pos == 'P' && *(read_header_pos + 1) == 'O'
      && *(read_header_pos + 2) == 'S' && *(read_header_pos + 3) == 'T'
      && *(read_header_pos + 4) == ' ' )
  { *(read_header_pos + 4) = '\0';
    read_header_pos += 5;
    *method = POST;
  }
  else if( *read_header_pos == 'P' && *(read_header_pos + 1) == 'U'
      && *(read_header_pos + 2) == 'T' && *(read_header_pos + 3) == ' ' )
  { *(read_header_pos + 3) = '\0';
    read_header_pos += 4;
    *method = PUT;
  }
  else if( *read_header_pos == 'H' && *(read_header_pos + 1) == 'E'
      && *(read_header_pos + 2) == 'A' && *(read_header_pos + 3) == 'D'
      && *(read_header_pos + 4) == ' ' )
  { *(read_header_pos + 4) = '\0';
    read_header_pos += 5;
    *method = HEAD;
  }
  else if( *read_header_pos == 'O' && *(read_header_pos + 1) == 'P'
      && *(read_header_pos + 2) == 'T' && *(read_header_pos + 3) == 'I'
      && *(read_header_pos + 4) == 'O' && *(read_header_pos + 5) == 'N'
      && *(read_header_pos + 6) == 'S' && *(read_header_pos + 7) == ' ' )
  { *(read_header_pos + 7) = '\0';
    read_header_pos += 8;
    *method = OPTIONS;
  }
  else if( !no_trace_flag && *read_header_pos == 'T' && *(read_header_pos + 1) == 'R'
      && *(read_header_pos + 2) == 'A' && *(read_header_pos + 3) == 'C'
      && *(read_header_pos + 4) == 'E' && *(read_header_pos + 5) == ' ' )
  { for(;;)
    { if( read_header_pos+3 - httpheader < bytes_in_content_buffer )
      { if( *read_header_pos == '\r' && *(read_header_pos+1) == '\n'
            && *(read_header_pos+2) == '\r' && *(read_header_pos+3) == '\n' )
        { *read_header_pos = '\0';
#ifdef WITH_HTTPS
          if( ssl_mode )
            fcntl(sh, F_SETFL, fcntlmode);
#endif
          send_trace(httpheader, read_header_pos - httpheader);
          if( http_head_flag & 1 )
            send_last_chunk();
          return -1;
        }
        read_header_pos++;
      }
      else
      { nnb = read_data(httpheader + bytes_in_content_buffer,
                        MAX_HTTPHEADER_SIZE - bytes_in_content_buffer,
                        timeout_secs);
        bytes_in_content_buffer += nnb;
        LOG(19, "read_http_header, nto: %d, bytes_in_content_buffer: %ld.\n",
            nto, bytes_in_content_buffer);
        if( nnb < 0 )
          return -1;
        else if( nnb == 0 && --nto <= 0 )  /* Timeout                      */
          return -1;
      }
    }
  }
  else 
  { LOG(50, "read_http_header, else, read_header_pos: %s.\n", read_header_pos);
    if( executeall && *executeall )
    { while( *read_header_pos && *read_header_pos != ' ' )
        read_header_pos++;
      if( *read_header_pos == ' ' )
        *read_header_pos++ = '\0';
    }
    else
    {
#ifdef WITH_HTTPS
      if( ssl_mode )
        fcntl(sh, F_SETFL, fcntlmode);
#endif       
      send_method_not_allowed();
      return -1;
    }
  }

  LOG(50, "read_http_header, pfad einlesen, read_header_pos: %s.\n", read_header_pos);
  /* Path einlesen */
  o = clientgetpath = read_header_pos;
  parflag = 0;                                    /* ab '?' auf 1, damit Sonderzeichen */
                                                  /* nicht mehr ausgewertet werden     */
  for(;;)
  { int i;

    if( *read_header_pos == '+' )
    { *o++ = ' ';
      read_header_pos++;
    }
    else if( !parflag && *read_header_pos == '%' && 1 == sscanf(read_header_pos+1, "%2x", &i) )
    {
#ifdef UNIX
      if( i != 13 )                          /* Unter Unix CR nicht auswerten          */
#endif
        *o++ = (char)i;
      read_header_pos += 3;
    }
    else if( read_header_pos - httpheader < bytes_in_content_buffer )
    { if( !parflag && *read_header_pos == '?' )
      { *o++ = '\0';
        *par = o;
        parflag = 1;
        read_header_pos++;
      }
      else if( *read_header_pos == ' '
               ||  *read_header_pos == '\r' || *read_header_pos == '\n' )
      { *o++ = '\0';
        read_header_pos++;
        break;
      }
      else
        *o++ = *read_header_pos++;
    }
    else
    { nnb = read_data(httpheader + bytes_in_content_buffer,
                      MAX_HTTPHEADER_SIZE - bytes_in_content_buffer, timeout_secs);
      bytes_in_content_buffer += nnb;
      LOG(19, "read_http_header, nto: %d, bytes_in_content_buffer: %ld.\n",
          nto, bytes_in_content_buffer);
      if( nnb < 0 )
        return -1;
      else if( nnb == 0 && --nto <= 0 )              /* Timeout                        */
      { *read_header_pos = '\0';
        return 0;
      }
    }
  }

  if( NULL != strstr(clientgetpath, "/../") || clientgetpath == strstr(clientgetpath, "../") )
  { clientgetpath[0] = '/';
    clientgetpath[1] = '.';
    clientgetpath[2] = '.';
    clientgetpath[3] = '.';
    clientgetpath[4] = '\0';
  }

  LOG(50, "read_http_header, HTTP lesen, read_header_pos: %s.\n", read_header_pos);
  LOG(50, "read_http_header, read_header_pos+7-httpheader: %ld, bytes_in_content_buffer: %ld.\n",
      (long)(read_header_pos+7 - httpheader), (long)bytes_in_content_buffer);
  if( *read_header_pos == 'H' )
  { while( read_header_pos+7 - httpheader >= bytes_in_content_buffer )
    { nnb = read_data(httpheader + bytes_in_content_buffer,
                      MAX_HTTPHEADER_SIZE - bytes_in_content_buffer, timeout_secs);
      bytes_in_content_buffer += nnb;
      LOG(19, "read_http_header, nto: %d, bytes_in_content_buffer: %ld.\n",
          nto, bytes_in_content_buffer);
      if( nnb < 0 )
        return -1;
      else if( nnb == 0 && --nto <= 0 )                /* Timeout                        */
      { http_v = 0;
        return 0;
      }
    }
    if( *read_header_pos++ == 'H' && *read_header_pos++ == 'T' && *read_header_pos++ == 'T'
        && *read_header_pos++ == 'P' && *read_header_pos++ == '/'
        && *read_header_pos++ == '1' && *read_header_pos++ == '.' )
      http_v = *read_header_pos - '0'+1;
  }
  else
    http_v = 0;
  for(;;)
  { if( *read_header_pos == '\r' )
    { if( *++read_header_pos == '\n' )
        read_header_pos++;
      break;
    }
    if( *read_header_pos == '\n' )
    { if( *++read_header_pos == '\r' )
        read_header_pos++;
      break;
    }
    read_header_pos++;
  }

  LOG(50, "read_http_header, restlichen HTTP-Header lesen, read_header_pos: %s.\n",
      read_header_pos);
  /* Restlichen HTTP-Header lesen                                                      */
  for(;;)
  { while( !test_line(read_header_pos,
                      bytes_in_content_buffer - (read_header_pos - httpheader)) )
    { nnb = read_data(httpheader + bytes_in_content_buffer,
                      MAX_HTTPHEADER_SIZE - bytes_in_content_buffer, timeout_secs);
      bytes_in_content_buffer += nnb;
      LOG(19, "read_http_header, nto: %d, bytes_in_content_buffer: %ld.\n",
          nto, bytes_in_content_buffer);
      if( nnb < 0 )
        return -1;
      else if( nnb == 0 && --nto <= 0 )              /* Timeout                        */
      { *read_header_pos = '\0';
        return 0;
      }
    }
    if( *read_header_pos == '\r' )                /* Leerzeile?                        */
    { if( *++read_header_pos == '\n' )
        read_header_pos++;
      break;                                      /* Header vollständig gelesen        */
    }
    else if( *read_header_pos == '\n' )           /* Leerzeile?                        */
    { if( *++read_header_pos == '\r' )
        read_header_pos++;
      break;                                      /* Header vollständig gelesen        */
    }
    /* HTTP-Header Elemente einlesen                                                   */
    http_value = NULL;
    if( tolower(*read_header_pos) == 'a'
        && str_lcasestarts_z(&read_header_pos, "accept-language:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      http_accept_language = read_header_pos;
      store_parstring("accept-language", read_header_pos, HP_HTTP_HEADER_LEVEL);
    }
    else if( tolower(*read_header_pos) == 'a'
             && str_lcasestarts_z(&read_header_pos, "accept-encoding:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      while( !*read_header_pos && *read_header_pos != '\r'
             && *read_header_pos != '\n' )
      { if( str_lcasecmp_z(&read_header_pos, "gzip") )
        { *gzip_flag = true;
          break;
        }
        while( isalnum(*read_header_pos) )
          read_header_pos++;
        while( *read_header_pos == ',' || *read_header_pos == ' ' )
          read_header_pos++;
      }
    }
    else if( tolower(*read_header_pos) == 'a'
             && str_lcasestarts_z(&read_header_pos, "authorization:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      if( str_lcasestarts_z(&read_header_pos, "basic ") )
      { while( *read_header_pos == ' ' )
          read_header_pos++;
        auth_basic = read_header_pos;
      }
      else if( str_lcasestarts_z(&read_header_pos, "digest ") )
      { while( *read_header_pos == ' ' )
          read_header_pos++;
        *auth_digest = read_header_pos;
      }
    }
    else if( tolower(*read_header_pos) == 'c'
             && str_lcasestarts_z(&read_header_pos, "connection:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      LOG(30, "read_http_header, read_header_pos: %s.\n", read_header_pos);
      *keepalive_found = str_lcasecmp_z(&read_header_pos, "keep-alive");
      LOG(30, "read_http_header, keepalive_found: %d.\n", *keepalive_found);
    }
    else if( tolower(*read_header_pos) == 'c'
             && str_lcasestarts_z(&read_header_pos, "content-type:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      content_type = read_header_pos;
      store_parstring("content-type", read_header_pos, HP_HTTP_HEADER_LEVEL);
    }
    else if( tolower(*read_header_pos) == 'c'
             && str_lcasestarts_z(&read_header_pos, "content-length:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      sscanf(read_header_pos, ULONGFORMAT, &content_length);
      cl_flag = true;
    }
    else if( tolower(*read_header_pos) == 'h'
             && str_lcasestarts_z(&read_header_pos, "host:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      store_parstring("host", read_header_pos, HP_HTTP_HEADER_LEVEL);
      http_host = read_header_pos;
    }
    else if( tolower(*read_header_pos) == 'i'
             && str_lcasestarts_z(&read_header_pos, "if-modified-since:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      store_parstring("if-modified-since", read_header_pos, HP_HTTP_HEADER_LEVEL);
      *test_modified_time = datestr2time_t(read_header_pos);
    }
    else if( tolower(*read_header_pos) == 'r'
             && str_lcasestarts_z(&read_header_pos, "range:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      store_parstring("range", read_header_pos, HP_HTTP_HEADER_LEVEL);
      *range = read_header_pos;
    }
    else if( tolower(*read_header_pos) == 't'
             && str_lcasestarts_z(&read_header_pos, "transfer-encoding:") )
    { while( *read_header_pos == ' ' )
        read_header_pos++;
      while( *read_header_pos && *read_header_pos != '\r'
             && *read_header_pos != '\n' )
      { if( str_lcasecmp_z(&read_header_pos, "chunked") )
        { read_chunk_flag = true;
          break;
        }
        while( isalnum(*read_header_pos) )
          read_header_pos++;
        while( *read_header_pos == ',' || *read_header_pos == ' ' )
          read_header_pos++;
      }
    }
    else
    { http_name = read_header_pos;
      while( *read_header_pos && *read_header_pos != ':' )
        read_header_pos++;
      if( *read_header_pos == ':' )
      { *read_header_pos++ = '\0';
        while( *read_header_pos == ' ' )
          read_header_pos++;
        http_value = read_header_pos;
      }
    }
                                             /* bis zum Zeilenende alles überlesen     */
    while( *read_header_pos && *read_header_pos != '\r' && *read_header_pos != '\n' )
      read_header_pos++;
    if( *read_header_pos == '\r' )
    { *read_header_pos++ = '\0';             /* Stringende                             */
      if( *read_header_pos == '\n' )
        read_header_pos++;
    }
    else if( *read_header_pos )
    { *read_header_pos++ = '\0';             /* Stringende                             */
      if( *read_header_pos == '\r' )
        read_header_pos++;
    }
    if( http_value )
    { if( strcasecmp(http_name, "cookie") )
      { char *p;
        for( p = http_name; *p; p++ )
          if( isupper(*p) )
            *p =  tolower((unsigned char)*p);
        store_parstring(http_name, http_value, HP_HTTP_HEADER_LEVEL);
      }
      else
        store_cookies(http_value);
    }
  }
  content_received = 0;
  content_buffer_size = MAX_HTTPHEADER_SIZE - (read_header_pos - httpheader);
  return 1;
}


/***************************************************************************************/
/* long read_http(char **b, int read_mode)                                             */
/*                char **b       : Zeiger auf gelesene Zeichen setzen                  */
/*                int read_mode: 0 - ganzen Content Block zusammenhängend POST !Multip.*/
/*                               1 - Daten, die vorliegen, (Multipart Content)         */
/*                               2 - Daten bis Timeout     (Multipart Content)         */
/*           return: Anzahl gelesener Zeichen, -1 bei Fehler, -2 bei Content zu groß   */
/*       read_http liest den content-Block                                             */
/***************************************************************************************/
long read_http(char **b, int read_mode)
{ static char *bp;                           /* aktuelle Leseposition                  */
  long chunk_len;                            /* Länge des Chunks                       */
  unsigned long ncr;                         /* content received für Rückgabe          */
  int nto;                                   /* number of Timeouts                     */
  long nnb;                                  /* Anzahl gelesene Zeichen                */

  LOG(3, "read_http, content_length: " ULONGFORMAT ", content_received: "
         LONGFORMAT ", bytes_in_content_buffer: %ld.\n",
      content_length, content_received, bytes_in_content_buffer);
  LOG(9, "read_http, read_chunk_flag: %d.\n", read_chunk_flag);

  if( content_received == 0 )
  { bytes_in_content_buffer -= read_header_pos - httpheader;
    if( !read_mode && content_length > content_buffer_size )
    {                                          /* content passt nicht in Headerpuffer  */
      if( content_length <= max_post_content_size )
      { if( NULL == (content_buffer = content_malloc_buffer = malloc(2+content_length)) )
        {         /* Puffer kann nicht bereitgestellt werden, restliche Daten einlesen */
          while( read_data(read_header_pos, content_buffer_size, timeout_secs) > 0);
          return -2;
        }
        LOG(19, "read_http, nach malloc, bytes_in_content_buffer: %ld.\n",
            bytes_in_content_buffer);
        bp = content_buffer;
        content_buffer_size = 2+content_length;
        while( bp - content_buffer < bytes_in_content_buffer )
          *bp++ = *read_header_pos++;
      }
      else
      {                                   /* Content zu groß, restliche Daten einlesen */
        while( read_data(read_header_pos, content_buffer_size, timeout_secs) > 0);
        LOG(9, "/read_http, content_length zu gross - Return -2\n");
        return -2;
      }
    }
    else
    { if( read_chunk_flag )
        content_length = 0;
      content_buffer = read_header_pos;
    }
    bp = content_buffer;
  }
//  else if( !read_chunk_flag )             /* vermutlich kann das nicht auftreten     */
//  { bp = content_buffer;
//    bytes_in_content_buffer = 0;
//  }
  else if( bytes_in_content_buffer == 0 )
    bp = content_buffer;

  if( read_chunk_flag && content_received == content_length )   /* Es muss ein neuer   */
  { if( bytes_in_content_buffer == 0 )                   /* Chunk-Header gelesen werden*/
    { bp = content_buffer;
      nto = 3;
      while( (bytes_in_content_buffer = read_data(bp, content_buffer_size, timeout_secs))
             == 0 )
      { if( --nto <= 0 )
          break;
      }
      if( bytes_in_content_buffer <= 0 )
        return -1;
    }
  
    if( *bp == CR )
    { bp++;
      bytes_in_content_buffer--;
    }
    if( bytes_in_content_buffer == 0 )
    { bp = content_buffer;
      nto = 3;
      while( (bytes_in_content_buffer = read_data(bp, content_buffer_size, timeout_secs))
             == 0 )
      { if( --nto <= 0 )
          break;
      }
      if( bytes_in_content_buffer <= 0 )
        return -1;
    }
    if( *bp == LF )
    { bp++;
      bytes_in_content_buffer--;
    }
    if( bytes_in_content_buffer == 0 )
    { bp = content_buffer;
      nto = 3;
      while( (bytes_in_content_buffer = read_data(bp, content_buffer_size, timeout_secs))
             == 0 )
      { if( --nto <= 0 )
          break;
      }
      if( bytes_in_content_buffer <= 0 )
        return -1;
    }
    sscanf(bp, "%lx", &chunk_len);
    LOG(3, "read_http, chunk_len: %ld.\n", chunk_len);
    if( chunk_len == 0 )
      return -1;
    content_length += chunk_len;
    while( *bp != CR && *bp != LF )
    { bp++;
      bytes_in_content_buffer--;
      if( bytes_in_content_buffer == 0 )
      { bp = content_buffer;
        nto = 3;
        while( (bytes_in_content_buffer = read_data(bp, content_buffer_size, timeout_secs))
               == 0 )
        { if( --nto <= 0 )
            break;
        }
        if( bytes_in_content_buffer <= 0 )
          return -1;
      }
    }
    if( *bp == CR )
    { bp++;
      bytes_in_content_buffer--;
    }
    if( bytes_in_content_buffer == 0 )
    { bp = content_buffer;
      nto = 3;
      while( (bytes_in_content_buffer = read_data(bp, content_buffer_size, timeout_secs))
             == 0 )
      { if( --nto <= 0 )
          break;
      }
      if( bytes_in_content_buffer <= 0 )
        return -1;
    }
    if( *bp == LF )
    { bp++;
      bytes_in_content_buffer--;
    }
    cl_flag = true;
  }

  LOG(19, "read_http, cl_flag: %d, bytes_in_content_buffer: %ld.\n",
      cl_flag, bytes_in_content_buffer);
  LOG(19, "read_http, content_received+bytes_in_content_buffer: %ld, content_length: %ld.\n",
      (long)(content_received+bytes_in_content_buffer), content_length);
  LOG(19, "read_http, bytes_in_content_buffer: %ld, content_buffer_size-1-(bp-content_buffer): %ld.\n",
     bytes_in_content_buffer, (long)(content_buffer_size-1-(bp-content_buffer)));
  nto = 2;                                       /* noch kein Tiemout                  */
  while( (!cl_flag || content_received+bytes_in_content_buffer < content_length)
                                                 /* C-B komplett gelesen?              */
         && bytes_in_content_buffer < content_buffer_size-1-(bp-content_buffer)
                                                 /* noch Platz im Puffer?              */
         && nto > 0 )                            /* kein Tiemout                       */
  { nnb = content_buffer_size-(bp-content_buffer)-bytes_in_content_buffer;
                                                 /* Platz im Puffer                    */
    LOG(19, "read_http, nnb1: %ld.\n", nnb);
    if( cl_flag && content_length - content_received - bytes_in_content_buffer < nnb )
                                                 /* soviel muss noch gelesen werden    */
      nnb = content_length - content_received - bytes_in_content_buffer;
    LOG(3, "read_http, while - min(<platz_im_puffer>,<rest_zu_lesen>): %ld.\n", nnb);
    LOG(30, "read_http, while - read_mode: %d, cl_flag: %d, timeout_secs: %d\n",
        read_mode, cl_flag, timeout_secs);
    if( nnb > 0 )
    { if( 0 == (nnb = read_data(bp+bytes_in_content_buffer, nnb,
                                read_mode == 2 && ! cl_flag ? 1 : timeout_secs)) )
                                                 /* neu gelesene Zeichen               */
        nto--;                                   /* keine Zeichen, dann Tiemout        */
      LOG(4, "read_http, while-gelesen: %ld.\n", nnb);
      if( nnb < 0 )                              /* Fehler?                            */
      { if( bytes_in_content_buffer > 0 )        /* noch was im Puffer?                */
        { nnb = bytes_in_content_buffer;         /* Puffergröße merken                 */
          bytes_in_content_buffer = 0;           /* für nächsten Aufruf merken         */
          *b = bp;
          bp += nnb;
          return nnb;                            /* Anzahl Zeichen zurück              */
        }
        else
          return -1;                             /* nichts im Puffer, dann Feher zurück*/
      }
    }
    if( nnb > 0 )
    { bytes_in_content_buffer += nnb;            /* Gesamtzahl Zeichen im Puffer       */
      if( !cl_flag )
        break;
    }
  }
  
  LOG(3, "read_http, content_received: " ULONGFORMAT ", content_length: " ULONGFORMAT ", bytes_in_content_buffer: %ld.\n",
      content_received, content_length, bytes_in_content_buffer);
  if( cl_flag && bytes_in_content_buffer > content_length - content_received )
                                                 /* größer als Content Länge?          */
  { ncr = content_length - content_received;
    content_received = content_length;
  }
  else
  { ncr = bytes_in_content_buffer;
    content_received += bytes_in_content_buffer;
  }
  bytes_in_content_buffer -= ncr;
  
  LOG(2, "/read_http, ncr: %ld.\n", ncr);
  *b = bp;
  bp += ncr;
  return ncr;                                    /* Anzahl Bytes zurueck               */
  
}
#endif  /* #ifdef WEBSERVER */
