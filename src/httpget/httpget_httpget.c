/*******************************************************/
/* File: httpget_httpget.c                             */
/* main function and variables                         */
/* timestamp: see COMPILEDATE                          */
/*******************************************************/

#include "httpget.h"


void send_parameter(char *parameterliste)
{ int i;
  char *p;
  char *q;
  char zeile[10240], *z;

  p = parameterliste;
  while( *p )
  { if( NULL == (q = strchr(p, '=')) )
      break;
    *q++ = '\0';
    snprintf(zeile, 10240,
             "--" BOUNDARY NL "Content-Disposition: form-data; name=\"%s\"" NL NL,
             p);
    chunksend(zeile, strlen(zeile));

    if( NULL != (p = strchr(q, '&')) )
      *p++ = '\0';
    else
      p = "";

    z = zeile;
    while( *q )
    { if( *q == '%' )
      { if( 0 < sscanf(q, "%%%2x", &i) )
          *z++ = (char)i;
        q += 3;
      }
      else if( *q == '+' )
      { *z++ = ' ';
        q++;
      }
    else
      *z++ = *q++;
    }

    chunksend(zeile, z-zeile);
    chunksend(NL, strlen(NL));
  }
}


void filesend(char *path)
{ int hd_in;
  size_t nb;
  char zeile[STRING_BUFFER_LEN];
  int stdin_flag;

  if( (stdin_flag = path[0] == '-' && path[1] == '\0') )
  {
#ifdef MINGW
    setmode(fileno(stdin), O_BINARY);
#endif
    hd_in = STDIN_FILENO;
  }
  else
#ifdef MINGW
    hd_in = open(path, O_RDONLY|O_BINARY);
#else
    hd_in = open(path, O_RDONLY);
#endif

  if( 0 <= hd_in )
  { while( 0 < (nb = read(hd_in, zeile, STRING_BUFFER_LEN)) )   /* lesen             */
    { if( chunksend(zeile, nb) )   /* und absenden            */
      { perror("send");
          return;
      }
    }
  }
  if( !stdin_flag )
    close(hd_in);
}


void send_multipart_file(char *multipart_name,
                         char *multipart_file, char *multipart_mimetype,
                         int multipart_stdin_flag,
                         char *multipartbuffer, unsigned long multipartbufferlen)
{ char *mime;
  int hd_in;
  size_t nb;                                          /* Anzahl gelesener Zeichen      */
  char zeile[STRING_BUFFER_LEN];

  LOG(1, "send_multipart_file, multipartbuffer: %.10s.\n", multipartbuffer);

  if( multipart_mimetype[0] )
    mime = multipart_mimetype;
  else
    mime = get_ct(multipart_file);

  snprintf(zeile, STRING_BUFFER_LEN,
    "--" BOUNDARY NL "Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"" NL
           "Content-Type: %s" NL NL,
           multipart_name, multipart_file, mime);
  chunksend(zeile, strlen(zeile));

  if( multipartbufferlen )
  { if( chunksend(multipartbuffer, multipartbufferlen) )
    { perror("send");
      return;
    }
  }
  else
  { if( multipart_stdin_flag )
    {
#ifdef MINGW
      setmode(fileno(stdin), O_BINARY);
#endif
      hd_in = STDIN_FILENO;
    }
    else
#ifdef MINGW
      hd_in = open(multipart_file, O_RDONLY|O_BINARY);
#else
      hd_in = open(multipart_file, O_RDONLY);
#endif

    if( 0 <= hd_in )
    { while( 0 < (nb = read(hd_in, zeile, STRING_BUFFER_LEN)) )   /* lesen             */
      { if( chunksend(zeile, nb) )   /* und absenden            */
        { perror("send");
          return;
        }
      }
    }
    if( ! multipart_stdin_flag )
      close(hd_in);
  }
  chunksend(NL, strlen(NL));
}


int httpget(char *path, char *outputfile, char *gfile, char *host,
            char *auth, int quiet_flag, int http_flag, char *proxy, int http_method,
            int error_out_flag, int stdin_urls_flag,
            int multipart_flag, char *multipart_name, char *multipart_file,
            char *multipart_mimetype, int multipart_stdin_flag, char *range,
            int post_flag, char *multipartbuffer, unsigned long multipartbufferlen,
            char *post_data_file, char *content_type)
{ long numbytes;
  char buf[2*MAXDATASIZE+400];
  char *buffer;
#ifdef WITH_IPV6
  struct addrinfo connectaddr, *connectres, *res;
  char *hoststr, *portstr;
#else
  struct hostent *he;
  struct sockaddr_in their_addr; /* connector's address information */
  int port;
#endif
  int i;
  char *p, h[256];
  char http[512];
  int hd_out;
  char *proxy_close = "";
  int ret = 0;
  long header_len;
  char *method;
  char codedgetfile[2*MAXDATASIZE];
  char getfile[MAXDATASIZE];
  char *parameter_start;
#ifdef WITH_HTTPS
  SSL_CTX    *ctx;
  X509       *server_cert;
  SSL_METHOD *meth;
  int fcntlmode;
#endif  /* WITH_HTTPS */

  strcpy(getfile, gfile);

#ifdef WITH_HTTPS
  if( opt_ssl_mode )
  { SSLeay_add_ssl_algorithms();
    meth = SSLv23_client_method();
    SSL_load_error_strings();
    if( NULL == (ctx = SSL_CTX_new(meth)) )
    { return ERROR_CTX_NEW;
    }
    if( ssl_cert_file != NULL && ssl_key_file != NULL )
    { if( SSL_CTX_use_certificate_file(ctx, ssl_cert_file, SSL_FILETYPE_PEM) <= 0 )
      { ERR_print_errors_fp(stderr);
        return ERROR_CTX_USE_C;
      }
      if( SSL_CTX_use_PrivateKey_file(ctx, ssl_key_file, SSL_FILETYPE_PEM) <= 0 )
      { ERR_print_errors_fp(stderr);
        return ERROR_CTX_USE_P;
      }

      if( !SSL_CTX_check_private_key(ctx) )
      { fprintf(stderr, "Private key does not match the certificate public key\n");
        return ERROR_CTX_CHECK;
      }
    }
    if( ssl_ca_file != NULL && !SSL_CTX_load_verify_locations(ctx, ssl_ca_file, NULL) )
    { ERR_print_errors_fp(stderr);
      return ERROR_CTX_LOCATION;
    }

    if( !SSL_CTX_set_cipher_list(ctx, "EECDH+ECDSA+AESGCM:EECDH+aRSA+AESGCM:EECDH+ECDSA+SHA384:EECDH+ECDSA+SHA256:EECDH+aRSA+SHA384:EECDH+aRSA+SHA256:EECDH+aRSA+RC4:EECDH:EDH+aRSA:RC4:!aNULL:!eNULL:!LOW:!3DES:!MD5:!EXP:!PSK:!SRP:!DSS") )
    { ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      return ERROR_CTX_SET_CIPHER;
    }

    if( ssl_mode & (SSL_MODE_FORCE_CERT) )
      SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
  }
#endif  /* WITH_HTTPS */

  LOG(1, "httpget, path: %s, timeout_secs: %d, connect_timeout_secs: %d.\n", path, timeout_secs, connect_timeout_secs);

  if( http_method & HTTP_HEAD )
    method = "HEAD";
  else if( http_method & HTTP_TRACE )
    method = "TRACE";
  else if( multipart_flag || post_flag || post_data_file )
  { method = "POST";
    if( NULL != (parameter_start = strchr(getfile, '?')) )
      *parameter_start++ = '\0';
  }
  else
    method = "GET";

  if( *proxy )
  { strcpy(h, proxy);
#ifdef WITH_HTTPS
    if( opt_ssl_mode )
      http[0] = '\0';
    else
#endif
    { sprintf(http, "http://%s", host);
      proxy_close = "Proxy-Connection: Close" NL;
    }
#ifdef WITH_IPV6
    portstr = PROXY_PORT_S; 
#else
    port = PROXY_PORT;
#endif
  }
  else
  { strcpy(h, host);
    http[0] = '\0';
#ifndef WITH_IPV6
    if( opt_ssl_mode )
      port = PORTHTTPS;
    else
      port = PORT;
#endif
  }

#ifdef WITH_IPV6
  if( strchr(h, '.') )                            /* IP-V 4?                           */
  { hoststr = h;
    if( (p = strchr(h, ':')) )
    { *p = '\0';
      portstr = p+1;
    }
#ifdef WITH_HTTPS
    else if( opt_ssl_mode )
      portstr = PORTHTTPS_S;
#endif
    else
      portstr = PORT_S;
  }
  else                                            /* IP-V 6                            */
  { if( *h == '[' )
    { hoststr = h+1;
      if( (p = strchr(h, ']')) )
      { *p++ = '\0';
        if( *p == ':' )
          portstr = p+1;
#ifdef WITH_HTTPS
        else if( opt_ssl_mode )
          portstr = PORTHTTPS_S;
#endif
        else
          portstr = PORT_S;
      }
      else
      { fprintf(stderr, _("Error in hostname, missing ']'.\n"));
        return ERROR_HOSTNAME;
      }
    }
    else if( (p = strchr(h, ':')) )
    { hoststr = h;
      *p++ = '\0';
      portstr = p;
    }
    else
    { hoststr = h;
#ifdef WITH_HTTPS
      if( opt_ssl_mode )
        portstr = PORTHTTPS_S;
      else
#endif
        portstr = PORT_S;
    }
  }

  LOG(7, "httpget, hoststr: %s, portstr: %s.\n", hoststr, portstr);

  memset(&connectaddr, '\0', sizeof(connectaddr));
  connectaddr.ai_flags    = AI_PASSIVE;
  connectaddr.ai_family   = AF_UNSPEC;
  connectaddr.ai_socktype = SOCK_STREAM;
  if( getaddrinfo(hoststr, portstr, &connectaddr, &connectres) < 0 )
  { perror("getaddrinfo");
    return ERROR_GETADDRINFO;
  }
  sh = -1;
  res = connectres;

  while( res )
  { LOG(21, "httpget - while(res)...\n");
    if( 0 <= (sh = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) )
    { if( setsockopt(sh, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0 )
      { perror("setsockopt");
        return ERROR_SOCKET;
      }
      if( connect(sh, res->ai_addr, res->ai_addrlen) >= 0 )
        break;
      close(sh);
      sh = -1;
    }
    res = res->ai_next;
  }
  freeaddrinfo(connectres);
  if( sh < 0 )
  { fprintf(stderr, "socket: can't open socket.\n");
    return ERROR_SOCKET;
  }
  LOG(3, "/httpget, sh: %d.\n", sh);

#else  /* #ifdef WITH_IPV6 */

  if( NULL != (p = strchr(h, ':')) )
  { *p = '\0';
    sscanf(p+1, "%d", &port);
  }

  if( (he=gethostbyname(h)) == NULL ) 
  { fprintf(stderr, "connect: %s: Name or service not known.\n", h);
    return ERROR_GETHOSTBYNAME;
  }

  if( NULL != p )
    *p = ':';

  if( (sh = socket(PF_INET, SOCK_STREAM, 0)) == -1 ) 
  { perror("socket");
    return ERROR_SOCKET;
  }

  their_addr.sin_family = AF_INET;                /* host byte order                   */
  their_addr.sin_port = htons(port);              /* short, network byte order         */
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(their_addr.sin_zero), '\0', 8);        /* zero the rest of the struct       */

#ifndef MINGW
  if( connect_timeout_secs )
  { int fsave;
    fd_set fds;                                   /* Set von Filedescriptoren          */
    struct timeval tv;                            /* Timeval Struktur                  */

    LOG(11, "httpget, connect mit timeout: %d.\n", connect_timeout_secs);
    if( (fsave = fcntl(sh, F_GETFL, NULL)) < 0 )
      return ERROR_FCNTL_GET;
    if( fcntl(sh, F_SETFL, fsave | O_NONBLOCK) < 0 )
      return ERROR_FCNTL_SET;
    if( connect(sh, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) < 0 ) 
    { if( errno == EINPROGRESS )
      { int err;
        socklen_t len = sizeof err;

        FD_ZERO(&fds);                            /* Alle fds auf Null                 */
        FD_SET(sh, &fds);                         /* Bit fuer sh setzen                */
        tv.tv_sec = connect_timeout_secs;         /* Timeout                           */
        tv.tv_usec = 0;

        if( !select(sh+1, NULL, &fds, NULL, &tv) )
        { fprintf(stderr, "connect: %s.\nconnect to: %s.\n", strerror(ETIMEDOUT), h);
          return ERROR_CONNECT_TIMEO;
        }
        getsockopt(sh, SOL_SOCKET, SO_ERROR, &err, &len);
        if( err )
        { fprintf(stderr, "connect: %s.\nconnect to: %s.\n", strerror(err), h);
          return ERROR_CONNECT;
        }
      }
    }
    if( fcntl(sh, F_SETFL, fsave) < 0 )
      return ERROR_FCNTL_SET;
  }
  else
#endif  /* MINGW */
  { if( connect(sh, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1 ) 
    { perror("connect");
      fprintf(stderr, "connect to: %s.\n", h);
      return ERROR_CONNECT_TIMEO;
    }
  }
#endif  /* #else #ifdef WITH_IPV6 */

#ifdef WITH_HTTPS
  if( opt_ssl_mode )
  { int ssl_errno, ust, selret;
    fd_set fds;                                           /* Set von Filedescriptoren  */
    struct timeval tv;                                    /* Timeval Struktur          */
    time_t end_time;

    if( *proxy )
    { snprintf(buf, 2*MAXDATASIZE+400, "CONNECT %s HTTP/1.0" NL
                                       "Host: %s" NL
                                       "Proxy-Connection: Keep-Alive" NL
                                       "Content-Length: 0" NL
                                       "Proxy-Connection: Keep-Alive" NL NL,
              host, host);
      send(sh, buf, strlen(buf), 0);
      read_data(buf, 1024, 10);
    }
    ssl_mode = opt_ssl_mode;
    if( NULL == (ssl = SSL_new(ctx)) )
      return ERROR_SSL_NEW;
    ust = 0;
    end_time = time(NULL) + (connect_timeout_secs ? connect_timeout_secs<<1 : 60);
    LOG(50, "httpget, SSL_connect - now: %ld, end_time: %ld.\n", time(NULL), end_time);
    if( 1 != SSL_set_fd(ssl, sh) )
    { ERR_print_errors_fp(stderr);
      return ERROR_SSL_SET_FD;
    }
    fcntlmode = fcntl(sh, F_GETFL);                         /* Non Blocking IO         */
    fcntl(sh, F_SETFL, fcntlmode | O_NONBLOCK);             /* Non Blocking IO         */
    while( (i = SSL_connect(ssl)) < 0 )
    { ssl_errno = SSL_get_error(ssl, i);
      LOG(50, "httpget, SSL_connect - i: %d, ssl_errno: %d, now: %ld.\n",
          i, ssl_errno, time(NULL));
      if( ssl_errno != SSL_ERROR_WANT_READ || time(NULL) > end_time )
        return ERROR_SSL_CONNECT;
      LOG(50, "httpget, SSL_connect - i: %d, ssl_errno: %d, ust: %d, timout: %d.\n",
          i, ssl_errno, ust, connect_timeout_secs ? connect_timeout_secs : 60);
      do
      { FD_ZERO(&fds);                    /* Alle fds auf Null                 */
        FD_SET(sh, &fds);                 /* Bit fuer sh setzen                */
        tv.tv_sec = connect_timeout_secs ? connect_timeout_secs : 60; /* Timeout       */
        tv.tv_usec = 0;
      
        selret = select(sh+1, &fds, NULL, NULL, &tv);  /* Daten vorhanden? */
        LOG(50, "httpget, selret: %d, errno: %d.\n", selret, errno);
      } while( selret < 0 && errno == EINTR );
      ust++;
      usleep(ust*ust*1000);
    }
    if( NULL == (server_cert = SSL_get_peer_certificate(ssl)) )
      return ERROR_SSL_GET_PEER;
    if( NULL != (p = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0)) )
    { LOG(5, "httpget, ssl subject name: %s.\n", p);
      if( ssl_print_subject )
        printf("%s%s\n", ssl_print_subject == 2 ? "Subject: " : "", p);
      OPENSSL_free(p);
    }
    if( NULL != (p = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0)) )
    { LOG(5, "httpget, ssl issuer name: %s.\n", p);
      if( ssl_print_issuer )
        printf("%s%s\n", ssl_print_issuer == 2 ? "Issuer: " : "", p);
      OPENSSL_free(p);
    }
    if( ssl_mode & SSL_MODE_FORCE_CERT )
      if( SSL_get_verify_result(ssl) != X509_V_OK )
        return ERROR_SSL_VERIFY;
    if( http_method & HTTPS_GET_SSL_CERT )
    { if( *outputfile )
      { FILE *ptr;

        if( (ptr = fopen(outputfile, "w")) )
        { PEM_write_X509(ptr, server_cert);
          fclose(ptr);
          return 0;
        }
        else
          return ERROR_OPEN_OUTPUT;
      }
      PEM_write_X509(stdout, server_cert);
      return 0;
    }
    X509_free(server_cert);
    fcntl(sh, F_SETFL, fcntlmode);                              /* Blocking IO         */
  }

#endif  /* WITH_HTTPS */

  if( *outputfile )
  {
#ifdef _LARGEFILE64_SOURCE
    if( 0 > (hd_out = open64(outputfile, O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) )
#else
#ifdef MINGW
    if( 0 > (hd_out = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | O_BINARY)) )
#else
    if( 0 > (hd_out = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) )
#endif
#endif
    { fprintf(stderr, _("Error on opening outputfile %s.\n"), outputfile);
      return ERROR_OPEN_OUTPUT;
    }
  }
  else
    hd_out = fileno(stdout);

  do
  { if( stdin_urls_flag )
    { if( NULL == fgets(getfile, MAX_URL_LEN, stdin) )
        break;

      while( getfile[0] &&
             (getfile[strlen(getfile)-1] == CR || getfile[strlen(getfile)-1] == LF) )
        getfile[strlen(getfile)-1] = '\0';
    }

    LOG(3, "httpget, getfile: %s.\n", getfile);

    if( code_url_flag )
    { if( *getfile == '/' )
        codeurl(codedgetfile, getfile+1);
      else
        codeurl(codedgetfile, getfile);
    }
    else
    { if( *getfile == '/' )
        strncpy(codedgetfile, getfile+1, 2*MAXDATASIZE);
      else
        strncpy(codedgetfile, getfile, 2*MAXDATASIZE);
    }

    if( NULL != (p = strchr(codedgetfile, '\xd')) )
      *p = '\0';
    if( NULL != (p = strchr(codedgetfile, '\xa')) )
      *p = '\0';

    if( http_flag )
    { if( stdin_urls_flag )
        keepalive_flag = true;
      else
        keepalive_flag = false;
      snprintf(buf, 2*MAXDATASIZE+400, "%s %s%s%s HTTP/1.1" NL "Host: %s" NL "%s",
               method, http, path, codedgetfile, host, proxy_close);
    }
    else
      snprintf(buf, 2*MAXDATASIZE+400, "%s %s%s%s" NL, method, http, path, codedgetfile);

    if( *auth )
      snprintf(buf+strlen(buf), 2*MAXDATASIZE+400-strlen(buf),
               "Authorization: Basic %s" NL, auth);
    if( multipart_flag )
      strcatn(buf, "Content-Type: multipart/form-data; boundary=" BOUNDARY NL, 2*MAXDATASIZE+400);
    else if( content_type )
    { strcatn(buf, "Content-Type: ", 2*MAXDATASIZE+400);
      strcatn(buf, content_type, 2*MAXDATASIZE+400);
      strcatn(buf, NL, 2*MAXDATASIZE+400);
    }
    if( range )
    { strcatn(buf, "Range: bytes=", 2*MAXDATASIZE+400);
      strcatn(buf, range, 2*MAXDATASIZE+400);
      strcatn(buf, NL, 2*MAXDATASIZE+400);
    }
    for( i = 0; i < anz_headeradd; i++ )
    { strcatn(buf, headeradd[i], 2*MAXDATASIZE+400);
      strcatn(buf, NL, 2*MAXDATASIZE+400);
    }

    if( send_http(buf, strlen(buf)) == -1 )
    { perror("send");
      return ERROR_SEND;
    }

    header_send_flag = false;

    if( multipart_flag )
    { if( parameter_start )
        send_parameter(parameter_start);
      if( multipart_file )
        send_multipart_file(multipart_name, multipart_file, multipart_mimetype,
                            multipart_stdin_flag, multipartbuffer, multipartbufferlen);
      chunksend("--" BOUNDARY "--" NL, strlen("--" BOUNDARY "--" NL));
    }
    else if( post_data_file )
      filesend(post_data_file);
    else if( post_flag && parameter_start )
      chunksend(parameter_start, strlen(parameter_start));

    send_last_chunk();

    header_len = 0;

#ifdef WITH_HTTPS
    if( opt_ssl_mode )
      fcntl(sh, F_SETFL, fcntlmode | O_NONBLOCK);           /* Non Blocking IO         */
#endif

    if( 0 <= (header_len = read_http(&buffer, true)) )
    { if( header_len )
      { if( http_method & HTTP_HEAD_OUT
            && header_len != write(hd_out, buffer, header_len) )
          return ERROR_WRITE;
        if( NULL == strstr(buffer, "200")
            && (!range || NULL == strstr(buffer, "206 Partital Content")) )
          ret = ERROR_NOT_OK;

        if( !ret || error_out_flag )
        { while( 0 < (numbytes = read_http(&buffer, false)) )
          { if( numbytes != write(hd_out, buffer, numbytes) )
              return ERROR_WRITE;
          }
          LOG(33, "httpget - numbytes: %ld.\n", numbytes);
          ret = -numbytes;
        }
      }
      else
      { while( 0 < (numbytes = read_http(&buffer, false)) )
        { if( numbytes != write(hd_out, buffer, numbytes) )
            return ERROR_WRITE;
        }
        ret = -numbytes;
      }
    }
    else
      ret = -header_len;
  } while( stdin_urls_flag );

  if( *outputfile )
  { close(hd_out);
    if( !quiet_flag )
      printf(_("File %s written.\n"), outputfile);
  }

  close(sh);

  return ret;
}
