/*************************************************/
/* File: u2w_u2w.c                               */
/* Hauptprogramm zum Starten der Subprozesse     */
/* timestamp: 2016-08-29 08:34:30                */
/*************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* void init_global_vars(void)                                                         */
/*      init_global_vars initialisiert alle globalen Variablen                         */
/***************************************************************************************/
void init_global_vars(void)
{ int i;

  extern char chunkbuffer[MAX_LEN_CHUNKS];
  extern char *chunkpointer;

  init_global_buffer();

  chunkpointer = chunkbuffer;

  resultpage = "";                   /* Ergebnisseite für Parameterseiten              */
  parameter_ext = "";                /* ggf. extension für <form>                      */
  rechtspage = "";                   /* Ergebnisseite Rechts für Parameterseiten       */
  obenpage = "";                     /* Ergebnisseite Oben für Parameterseiten         */
  mime = "";                         /* Mime-Type von %mime                            */
  anz_httpheadlines = 0;             /* Anzahl zusätzlicher HTTP-Headerzeilen          */
  charset = "";                      /* Characterset von %charset                      */
  savename = "";                     /* Datei-Name für Content-Disposition             */
  content_type = "";
  content_path = "";
  user[0] = '\0';
  passwd = "";
  auth_basic = "";
  http_head_flag = 0;                /* true, Header ist komplett gesendet             */
  file_flag = true;                  /* false, ptr aus %input, true sonst              */
                                     /*  false: '`' und '#' nicht auswerten.           */
  tablecols = 0;                     /* Spalten einer Tabelle                          */
  akt_tablecols = 0;                 /* aktuelle Spalten einer Tabelle                 */
  newlineflag = true;                /* Formularfelder stehen auf neuen Zeilen         */


  header_flag = true;                /* true, HTTP-Header wird gesendet                */

  menu = NOMENU;                     /* Position auf Menueseite                        */

  skip_empty_flag = skip_empty_flag_def;
                                     /* true: Leere Parameter werden nicht an Programme*/
                                     /* übergeben.                                     */

  globalquote[0] = '\0';
                                     /* Zeichen, die mit erstem Zeichen gequotet werden*/


#ifdef HAS_DAEMON
  for( i = daemonflag ? 0 : 3; i < MAX_ANZ_OPEN_FILES; i++ )
#else
  for( i = 3; i < MAX_ANZ_OPEN_FILES; i++ )
#endif
  { if( file_ptrs[i] )
    { fclose(file_ptrs[i]);
      file_ptrs[i] = NULL;
    }
  }

  for( i = 0; i < anz_auto_delete_files; i++ )         /* POST-Dateien löschen         */
    unlink(auto_delete_files[i]);
  anz_auto_delete_files = 0;

  headframe = NULL;
  headsize[0] = '\0';

#ifdef WEBSERVER
  for( i = 0; i < NUM_CSS_DEF_ELEMENTS; css_defs[i++] = NULL);
#endif
}


#ifdef WEBSERVER
/***************************************************************************************/
/* void sig_handler(int signum)                                                        */
/*                      int signum: ausloesendes Signal                                */
/*      sig_handler wartet auf SIGUSR1|SIGTERM, gibt alle konsolidierten Zeilen aus    */
/*                  und beendet lfa4uls bei SIGTERM                                    */
/***************************************************************************************/
void sig_handler(int signum)
{
  LOG(15, "sig_handler, signum: %d, term_flag: %d.\n",
      signum, term_flag);

  if( signum == SIGQUIT )
    term_flag += 2;
  else if( signum == SIGHUP )
    read_inifile();
#ifdef CYGWIN
  else if( signum == SIGTERM )
  { if( servicestopjob )
      system(servicestopjob);
    exit(0);
  }
#endif
  else if( signum == SIGUSR1 )
  {
#ifdef WITH_MMAP
    if( (status_mode & STAT_ENABLE_FLUSH)
        && status_counter && status_counter->flushconnectioncounter )
    { connectioncounter -= status_counter->flushconnectioncounter;
      status_counter->lastconnectioncounter = status_counter->flushconnectioncounter = 0;
    }
    else
    { connectioncounter = 0;
      if( status_counter )
      { if( status_mode >= STATDIFF_CONNECT )
        { status_counter->lastconnectioncounter = status_counter->lastrequests
          = status_counter->lastbytessend = status_counter->lastbytesreceived
          = status_counter->lasterrors
          = status_counter->requests = status_counter->bytessend
          = status_counter->bytesreceived = status_counter->errors = 0;
          time(&status_counter->time_last_read);
        }
        else if( status_mode > STAT_CONNECT )
        { status_counter->requests = status_counter->bytessend
          = status_counter->bytesreceived = status_counter->errors = 0;
        }
      }
    }
#else
    connectioncounter = 0;
#endif
    time(&starttime);
  }
  else if( signum == SIGUSR2 )
    term_flag = 1;
  LOG(15, "/sig_handler, term_flag: %d.\n", term_flag);
}


/***************************************************************************************/
/* void sigchld_handler(int signum)                                                    */
/*                      int signum: ausloesendes Signal                                */
/*      sigchld_handler wartet auf SIGCHLD und fragt dann Status des Child-Przesses ab */
/*                      damit Child aus der Prozessliste verschwindet                  */
/***************************************************************************************/
void sigchld_handler(int signum)
{ while( waitpid(-1, NULL, WNOHANG) > 0 )
    ;
}
#endif  /* #ifdef WEBSERVER */


#ifdef ENABLE_DH_CURVE
DH *get_dh2048(void)
{ static unsigned char dh2048_p[] = {
    0xDB, 0x10, 0x57, 0x24, 0xC2, 0xF1, 0x7E, 0xD2, 0x63, 0xDD, 
    0xF1, 0x20, 0x2C, 0x3F, 0x24, 0x0F, 0x7D, 0x65, 0x22, 0x80, 
    0xEB, 0x75, 0x66, 0xA3, 0xA0, 0x11, 0xFB, 0x9E, 0xF2, 0xF0, 
    0xA6, 0xCB, 0x4E, 0x0E, 0xF4, 0x09, 0x0A, 0x13, 0xEE, 0x4B, 
    0xE2, 0x61, 0x80, 0xF2, 0x62, 0x36, 0x19, 0xA1, 0x08, 0x2F, 
    0xDE, 0x53, 0x2F, 0x10, 0x6E, 0xD0, 0xBC, 0x7A, 0x1E, 0x2E, 
    0xBC, 0xC0, 0x87, 0x3E, 0x78, 0x9E, 0xEF, 0xD4, 0x9A, 0x9A, 
    0x0E, 0x7B, 0x06, 0x1B, 0x46, 0x78, 0x58, 0x7C, 0x82, 0xA4, 
    0x75, 0xB5, 0xA2, 0xEE, 0xC6, 0x3D, 0xE7, 0xA4, 0xAF, 0x75, 
    0x54, 0xD9, 0x5C, 0x3F, 0x9F, 0x53, 0x93, 0x59, 0xAD, 0x48, 
    0x77, 0xD7, 0x1D, 0x7C, 0x02, 0xBC, 0x14, 0xA0, 0x3A, 0x70, 
    0x0F, 0xB7, 0x89, 0x90, 0x3E, 0x8D, 0xA3, 0x9A, 0x2E, 0x89, 
    0x02, 0xE1, 0xDF, 0x14, 0x39, 0xD4, 0xFC, 0xB2, 0xF2, 0xAF, 
    0xDB, 0x04, 0xB1, 0x8F, 0x1F, 0x38, 0x3F, 0xEA, 0x14, 0x9F, 
    0x62, 0xEC, 0xC9, 0x7C, 0x92, 0x36, 0x41, 0x36, 0xE3, 0x53, 
    0xF6, 0x81, 0xB1, 0x87, 0x95, 0x74, 0x6D, 0x30, 0x6E, 0x36, 
    0x9B, 0xB8, 0xC2, 0x3B, 0x24, 0xBE, 0x08, 0x65, 0x2A, 0xFB, 
    0x5B, 0xD2, 0xB3, 0x49, 0x65, 0xEC, 0x6F, 0x89, 0xAC, 0xF3, 
    0x40, 0xE3, 0x87, 0x0B, 0x72, 0x14, 0x61, 0x7E, 0x7C, 0x38, 
    0x2E, 0xB3, 0x30, 0x7F, 0x1A, 0x5B, 0xB4, 0x27, 0x10, 0x78, 
    0xE0, 0x18, 0x33, 0x7A, 0x02, 0xF1, 0x12, 0x09, 0x63, 0x4E, 
    0xE8, 0x72, 0xAF, 0x9B, 0x7D, 0x3F, 0x51, 0x8D, 0x46, 0x51, 
    0xE7, 0x89, 0xDB, 0x87, 0x1F, 0xF5, 0x1E, 0x36, 0x67, 0x53, 
    0x54, 0x68, 0x39, 0xBB, 0xB4, 0x47, 0xF1, 0x26, 0xBA, 0xF6, 
    0xF8, 0x2F, 0xF5, 0x50, 0x40, 0x75, 0x52, 0x0E, 0xCC, 0x36, 
    0xFB, 0x0E, 0xFE, 0xD9, 0x23, 0xC3 };
  static unsigned char dh2048_g[]={ 0x02 };
  DH *dh;
  BIGNUM *dh_bn_p, *dh_bn_g;

  if( !(dh = DH_new()) )
    return NULL;

  if( !(dh_bn_p = BN_bin2bn(dh2048_p, sizeof (dh2048_p), NULL)) )
  { DH_free(dh);
    return NULL;
  }
  if( !(dh_bn_g = BN_bin2bn(dh2048_g, sizeof (dh2048_g), NULL)) )
  { DH_free(dh);
    BN_free(dh_bn_p);
    return NULL;
  }
#ifdef DH_F_DH_METH_DUP
  if( !DH_set0_pqg(dh, dh_bn_p, NULL, dh_bn_g) )
  { DH_free(dh);
    BN_free(dh_bn_p);
    BN_free(dh_bn_g);
    return NULL;
  }
#else
  dh->p = dh_bn_p;
  dh->g = dh_bn_g;
#endif
  return dh;
}
#endif


#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
// verhindert: warning: ‘meth’ may be used uninitialized in this function [-Wdiscarded-qualifiers]
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
// verhindert: warning: ‘ctx’ may be used uninitialized in this function [-Wmaybe-uninitialized]
#pragma GCC diagnostic ignored "-Wsequence-point"
// verhindert: warning: operation on ‘ust’ may be undefined [-Wsequence-point]
#endif
#endif
/***************************************************************************************/
/* void unix2web(int port, int backlog)                                                */
/*               int       port: Port, auf den gehoert wird                            */
/*               int      backlog: Num backlog in listen                               */
/*      unix2web Hauptprogramm wartet auf Verbindungen und startet dann                */
/*               Subprozesse, die die Anforderungen auswerten                          */
/***************************************************************************************/
void unix2web(int port, int backlog)
{ int sockfd;                              /* Socket Handle fuer warten auf Verbindund */
  int new_fd;                              /* Socket Handle der neuen Verbindung       */
#ifdef WITH_IPV6
  struct sockaddr_in6 my_addr6;            /* meine Adresse                            */
  struct sockaddr_in6 their_addr6;         /* Adresse des Clients                      */
  short ipv4flag;                          /* wird gesetzt, wenn Listen-ip IPV4 Adresse*/
#endif
  struct sockaddr_in my_addr;              /* meine Adresse                            */
  struct sockaddr_in their_addr;           /* Adresse des Clients                      */
#ifdef OLDUNIX
  size_t sin_size;
#else
  socklen_t sin_size;
#endif
  int i, select_error_counter;
#ifdef HAS_DAEMON
  FILE *ptr_pidfile;
#endif
#ifdef WITH_HTTPS
  SSL_CTX *ctx;
  X509    *client_cert;
  SSL_METHOD *meth;
  static int session_id_context = 1;
#ifdef ENABLE_DH_CURVE
  EC_KEY *ecdh;
  DH *dh;
#endif

  if( ssl_mode )
  { SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
#ifdef SSL_OP_NO_COMPRESSION
#ifdef ONLY_SECURE_SSL
    meth = TLSv1_2_server_method();
#define SSL_OPTS SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3|SSL_OP_NO_TLSv1|SSL_OP_NO_COMPRESSION
#else
    meth = SSLv23_server_method();
#define SSL_OPTS SSL_OP_ALL|SSL_OP_NO_COMPRESSION
#endif
#else
    meth = SSLv23_server_method();
#ifdef ONLY_SECURE_SSL
#define SSL_OPTS SSL_OP_ALL|SSL_OP_NO_TLSv1
#else
#define SSL_OPTS SSL_OP_ALL
#endif
#endif
    ctx = SSL_CTX_new(meth);
    if( !ctx )
    { ERR_print_errors_fp(stderr);
      exit(2);
    }

//    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
    SSL_CTX_set_options(ctx, SSL_OPTS);
    SSL_CTX_sess_set_cache_size(ctx, 128);
    if( SSL_CTX_use_certificate_file(ctx, ssl_cert_file, SSL_FILETYPE_PEM) <= 0 )
    { ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(3);
    }
    if( SSL_CTX_use_PrivateKey_file(ctx, ssl_key_file, SSL_FILETYPE_PEM) <= 0 )
    { ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(4);
    }

    if( !SSL_CTX_check_private_key(ctx) )
    { fputs(_("Private key does not match the certificate public key\n"), stderr);
      SSL_CTX_free(ctx);
      exit(5);
    }

    if( ssl_chain_file != NULL && !SSL_CTX_use_certificate_chain_file(ctx, ssl_chain_file) )
    { ERR_print_errors_fp(stderr);
      exit(6);
    }
    else if( ssl_ca_file != NULL && !SSL_CTX_load_verify_locations(ctx, ssl_ca_file, NULL) )
    { ERR_print_errors_fp(stderr);
      exit(6);
    }

#ifdef ENABLE_DH_CURVE
    if( NULL == (ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)) )
    { fputs(_("Failed to get EC curve.\n"), stderr);
      SSL_CTX_free(ctx);
      exit(5);
    }
    SSL_CTX_set_tmp_ecdh(ctx, ecdh);
    EC_KEY_free(ecdh);

    if( ssl_dh_params_file && *ssl_dh_params_file )
    { FILE *ptr;
      if( (ptr = fopen(ssl_dh_params_file, "r")) )
      { if( !(dh = PEM_read_DHparams(ptr, NULL, NULL, NULL)) )
        { fclose(ptr);
          fputs(_("Failed to get DH params, wrong DH param file.\n"), stderr);
          SSL_CTX_free(ctx);
          exit(5);
        }
        fclose(ptr);
      }
      else
      { fputs(_("Failed opening DH params file.\n"), stderr);
        SSL_CTX_free(ctx);
        exit(5);
      }
    }
    else if( NULL == (dh = get_dh2048()) )
    { fputs(_("Failed to get DH params.\n"), stderr);
      SSL_CTX_free(ctx);
      exit(5);
    }
    if( 1 != SSL_CTX_set_tmp_dh(ctx, dh) )
    { fputs(_("Failed to set DH.\n"), stderr);
      SSL_CTX_free(ctx);
      exit(5);
    }
    DH_free(dh);

    if( !SSL_CTX_set_cipher_list(ctx, "EECDH+ECDSA+AESGCM:EECDH+aRSA+AESGCM:EECDH+ECDSA+SHA384:EECDH+ECDSA+SHA256:EECDH+aRSA+SHA384:EECDH+aRSA+SHA256:EECDH+aRSA+RC4:EECDH:EDH+aRSA:RC4:!aNULL:!eNULL:!LOW:!3DES:!MD5:!EXP:!PSK:!SRP:!DSS") )
    { ERR_print_errors_fp(stderr);
      SSL_CTX_free(ctx);
      exit(5);
    }
#endif

    if( ssl_mode & SSL_MODE_FORCE_CLIENT )
      SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT|SSL_VERIFY_CLIENT_ONCE, NULL);
    else if( ssl_mode & SSL_MODE_ASK_CLIENT )
      SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_CLIENT_ONCE, NULL);
    else
      SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

    SSL_CTX_set_session_id_context(ctx,(void*)&session_id_context, sizeof session_id_context);
  }
#endif  /* WITH_HTTPS */

#ifdef HAS_DAEMON
  if( daemonflag )                         /* wenn daemonflag, dann daemonisierten     */
  {
    for( i = 0; i < 20; i++ )              /* alle I/O-Kanaele schliessen              */
      close(i);
    if( (i = open("/dev/tty", O_RDWR)) >= 0 )    /* Terminalverbindung trennen         */
    { if( ioctl(i, TIOCNOTTY, NULL) < 0 )
        logging("Can't disassociate from tty\n");
      close(i);
    }
    for( i = 0; i < MAX_ANZ_OPEN_FILES; file_ptrs[i++] = NULL);
  }
  else
  { file_ptrs[0] = stdin;
    file_ptrs[1] = stdout;
    file_ptrs[2] = stderr;
    for( i = 3; i < MAX_ANZ_OPEN_FILES; file_ptrs[i++] = NULL);
  }
#else
  file_ptrs[0] = stdin;
  file_ptrs[1] = stdout;
  file_ptrs[2] = stderr;
  for( i = 3; i < MAX_ANZ_OPEN_FILES; file_ptrs[i++] = NULL);
#endif

#ifdef HAS_DAEMON
  if( !daemonflag || !fork() )                   /* Subprozess starten                 */
#endif
  {
#ifdef WITH_IPV6
    if( !listenip || !strchr(listenip, '.') )
    { sin_size = sizeof(their_addr6);
      if( (sockfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1 ) /* Socket oeffnen          */
      { if( !daemonflag )
          perror("socket");
        exit(1);
      }
      ipv4flag = 0;
    }
    else
    { ipv4flag = 1;
#endif
      sin_size = sizeof(their_addr);
      if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )  /* Socket oeffnen          */
      { if( !daemonflag )
          perror("socket");
        exit(1);
      }
#ifdef WITH_IPV6
    }
#endif
    i = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

#ifdef WITH_IPV6
    if( !ipv4flag )
    { memset(&my_addr6, 0, sizeof(my_addr6));
      my_addr6.sin6_family = AF_INET6;
      my_addr6.sin6_port   = htons(port);
      if( listenip )
      { struct in6_addr adr6;
        if( !inet_pton(AF_INET6, listenip, &adr6) )
        { if( !daemonflag )
            perror("inet_pton");
          exit(1);
        }
        my_addr6.sin6_addr = adr6;
      }
      else
        my_addr6.sin6_addr = in6addr_any;
      if( bind(sockfd, (struct sockaddr *)&my_addr6, sizeof(my_addr6)) == -1 ) 
      { if( !daemonflag )
          perror("bind");
        exit(1);
      }
    }
    else
    {
#endif
      my_addr.sin_family = AF_INET;                /* host byte order                    */
      my_addr.sin_port = htons(port);              /* short, network byte order          */
      if( listenip )
        my_addr.sin_addr.s_addr = inet_addr(listenip);  /* listen IP                     */
      else
        my_addr.sin_addr.s_addr = INADDR_ANY;      /* alle IP                            */
      memset(&(my_addr.sin_zero), '\0', 8);        /* Rest der struct auf 0              */
      if( bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1 ) 
      { if( !daemonflag )
          perror("bind");
        exit(1);
      }
#ifdef WITH_IPV6
    }
#endif

    if( listen(sockfd, backlog) == -1 )          /* auf Verbindungen warten            */
    { if( !daemonflag )
        perror("listen");
      exit(1);
    }

#ifdef HAS_DAEMON
    if( pidfile && *pidfile )
    { if( NULL == (ptr_pidfile = fopen(pidfile, "w")) )
      { logging(_("Error on opening Pidfile %s.\n"), pidfile);
        exit(2);
      }
      fprintf(ptr_pidfile, "%d\n", getpid());
      fclose(ptr_pidfile);
    }

#endif

#ifdef WITH_MMAP
    parentpid = getpid();
#endif

    signal(SIGCHLD, sigchld_handler);
    signal(SIGHUP, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGUSR2, sig_handler);
#ifdef CYGWIN
    signal(SIGTERM, sig_handler);
#endif

#ifndef CYGWIN
    if( set_user_mode == STARTSETUSER && defuser )
    { struct passwd *pass;

      if( (pass = getpwnam(defuser)) != NULL )
      { if( pidfile && *pidfile )
          chown(pidfile, pass->pw_uid, pass->pw_gid);
        if( logpath[0] )
          chown(logpath, pass->pw_uid, pass->pw_gid);
        if( change_user(pass, true) )          /* User auf defuser wechseln            */
        { logging(_("Error on change user to %s.\n"), defuser);
          exit(3);
        }
      }
      else
      { logging(_("Error on change user to %s.\n"), defuser);
        exit(3);
      }
    }
#endif

    select_error_counter = 0;

    if( servicestartjob )
    { if( ! fork() )
      { sleep(5);
        system(servicestartjob);
      }
    }

    for(;;)
    { fd_set fds;

      FD_ZERO(&fds);
      FD_SET(sockfd, &fds);

      LOG(11, "unix2web - for(;;).\n");
      if( (i = select(sockfd+1, &fds, NULL, NULL, NULL)) <= 0 )
      { if( i < 0 && errno != EINTR )
        { if( !daemonflag )
            perror("select");
          if( ++select_error_counter >= 1000 )
            exit(1);
          continue;
        }
        select_error_counter = 0;
        if( term_flag == 1 )
        { if( servicestopjob )
          { if( !fork() )
            { system(servicestopjob);
            }
            servicestopjob = NULL;
          }
        }
        else if( term_flag )
        { LOG(11, "unix2web - term_flag: %d.\n", term_flag);
          if( servicestopjob )
            system(servicestopjob);
          while( wait(NULL) >= 0 )
            if( term_flag > 2 )
              break;
          LOG(13, "unix2web - nach wait for children.\n");
          break;
        }
        continue;
      }
#ifdef WITH_IPV6
      if( !ipv4flag )
      { if( (new_fd = accept(sockfd, (struct sockaddr *)&their_addr6, &sin_size)) == -1 )
        { if( errno != EINTR && !daemonflag )
            perror("accept");
          continue;
        }
      }
      else
#endif
      if( (new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1 )
      { if( errno != EINTR && !daemonflag )
          perror("accept");
        continue;
      }
      connectioncounter++;
      LOG(31, "unix2web - connectioncounter: %d.\n", connectioncounter);
      if( !fork() ) 
      {                                           /* Kindprozess                       */
        close(sockfd);                            /* socket handle des Elternprozesses */

#ifdef WITH_IPV6
        if( !ipv4flag )
        { inet_ntop(AF_INET6, &their_addr6.sin6_addr, clientip6, MAX_IP_LEN);
          clientip = clientip6;
          if( !str_starts_z(&clientip, "::ffff:") )
            clientip = clientip6;
        }
        else
        { clientip = clientip6;
#endif
          strcpyn(clientip, inet_ntoa(their_addr.sin_addr), MAX_IP_LEN);
#ifdef WITH_IPV6
        }
#endif

        if( logflag & LOGCONNECTION )
          logging("server: got connection from %s\n", clientip);

        if( restricted_ips != NULL )
        { if( !host_in_list(clientip, restricted_ips) )
          { close(new_fd);
            exit(1);
          }
        }

#ifdef WITH_HTTPS
        if( ssl_mode )
        { int ssl_errno, ust, selret;
          struct timeval tv;                      /* Timeval Struktur                  */
          time_t end_time;

          fcntlmode = fcntl(new_fd, F_GETFL);               /* Non Blocking IO         */
          fcntl(new_fd, F_SETFL, fcntlmode | O_NONBLOCK);

          tv.tv_sec = timeout_secs;               /* Timeout                           */
          tv.tv_usec = timeout_usecs;
          setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
  
          if( NULL == (ssl = SSL_new(ctx)) )
          { close(new_fd);
            exit(7);
          }
          SSL_set_fd(ssl, new_fd);
          ust = 0;
          time(&end_time);
          if( timeout_secs )
            end_time += timeout_secs*3;
          else
            end_time += 10;
          while( (i = SSL_accept(ssl)) < 0 )
          { ssl_errno = SSL_get_error(ssl, i);
            if( (ssl_errno != SSL_ERROR_WANT_READ && ssl_errno != SSL_ERROR_WANT_WRITE)
                || time(NULL) > end_time )
            { close(new_fd);
#ifdef DEBUG
if( logpath[0] )
{ FILE *eptr;
  if( NULL != (eptr = fopen(logpath, "a")) )
  { ERR_print_errors_fp(eptr);
    fclose(eptr);
  }
}
#endif
              exit(8);
            }
#ifdef OLDUNIX
            sleep(++ust/100);
#else
            usleep(++ust*ust*1000);
#endif
            do
            { FD_ZERO(&fds);                    /* Alle fds auf Null                 */
              FD_SET(new_fd, &fds);             /* Bit fuer new_fd setzen            */
              tv.tv_sec = timeout_secs;         /* Timeout                           */
              tv.tv_usec = timeout_usecs;
            
              selret = select(new_fd+1, &fds, NULL, NULL, &tv);  /* Daten vorhanden? */
            } while( selret < 0 && errno == EINTR && !term_flag );
            if( term_flag )
            { close(new_fd);
              exit(0);
            }
          }
          if( i != 1 )
          { close(new_fd);
            exit(0);
          }
          if( ssl_mode & (SSL_MODE_FORCE_CLIENT | SSL_MODE_ASK_CLIENT) )
          { if( NULL != (client_cert = SSL_get_peer_certificate(ssl)) )
            { char *cstr;
  
              if( NULL != (cstr = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0)) )
              { LOG(7, "unix2web, ssl-client-cert, subject: %s.\n", cstr);
                ssl_client_subject = store_str(cstr);
                OPENSSL_free(cstr);
              }
              if( NULL != (cstr = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0)) )
              { LOG(7, "unix2web, ssl-client-cert, issuer: %s.\n", cstr);
                ssl_client_issuer = store_str(cstr);
                OPENSSL_free(cstr);
              }
              X509_free(client_cert);
              if( ssl_mode & SSL_MODE_FORCE_CLIENT )
                if( SSL_get_verify_result(ssl) != X509_V_OK )
                  exit(9);
            }
            else
            {
#ifdef DEBUG
              LOG(7, "unix2web, NO client-cert.\n");
#endif
              if( ssl_mode & SSL_MODE_FORCE_CLIENT )
                exit(10);
            }
          }
        }
#endif  /* WITH_HTTPS */
        sh = new_fd;

        /* Randomgenerator initialisieren */
#ifdef OLDUNIX
        srand(getpid());
#else
        srandom(getpid());
#endif

        while( !do_httpd() && !term_flag) /* Anforderung bearbeiten  */
        { init_global_vars();
#ifdef MAYDBCLIENT
          mysql_free_res();                       /* Speicher freigeben                */
#endif
        }

        for( i = 0; i < anz_auto_delete_files; i++ )   /* POST-Dateien löschen         */
          unlink(auto_delete_files[i]);

#ifdef MAYDBCLIENT
        if( mysql_connect_flag )
          mysql_close(&mh);
#endif

        sleep(1);
        close(new_fd);
#ifdef WITH_HTTPS
        if( ssl_mode )
          SSL_free(ssl);
#endif  /* WITH_HTTPS */
        LOG(1, "unix2web, nach close(new_fd).\n");
#ifdef WITH_MMAP
        if( status_mode > STAT_CONNECT )
          munmap(status_counter, sizeof(counter_type));
#endif
        exit(0);                                  /* Kindprozess beenden               */
      }
      else
        close(new_fd);                            /* Elternprozess                     */
    }
    if( pidfile && *pidfile )
      unlink(pidfile);
  }
#ifdef WITH_HTTPS
  if( ssl_mode )
    SSL_CTX_free(ctx);
#endif  /* WITH_HTTPS */
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif

#endif  /* #ifdef WEBSERVER */
#ifdef INTERPRETER

/***************************************************************************************/
/* int unix2web(int options, int argc, char **argv)                                    */
/*               int options: ausgewertete Optionen                                    */
/*               int argc: argc von main()                                             */
/*               char **argv: argv von main()                                          */
/*      unix2web Hauptprogramm wertet Parameterliste aus und startet Prg               */
/***************************************************************************************/
int unix2web(int options, int argc, char **argv)
{ int ret, i;
#ifdef DEBUG
#ifdef WITH_MALLOC
  extern char *strbuff;
#else
  extern char strbuff[STRINGBUFFERSIZE];
#endif
  extern char *sb;
#endif

  file_ptrs[0] = stdin;
  file_ptrs[1] = stdout;
  file_ptrs[2] = stderr;
  for( i = 3; i < MAX_ANZ_OPEN_FILES; file_ptrs[i++] = NULL);

#ifdef MAYDBCLIENT
  mysql_init_flags();
#endif
#ifdef POSTGRESQLCLIENT
  pgsql_init_flags();
#endif
#ifdef SQLITE3
  sql3_init_flags();
#endif
#ifdef MAXDBCLIENT
  maxdb_init_flags();
#endif

  /* Randomgenerator initialisieren */
#ifdef OLDUNIX
  srand(getpid());
#else
  srandom(getpid());
#endif

  ret = do_httpd(options, argc, argv);            /* PRG bearbeiten                    */

#ifdef MAYDBCLIENT
   mysql_free_res();                              /* Speicher freigeben                */
   if( mysql_connect_flag )
     mysql_close(&mh);
#endif

  LOG(2, "/unix2web, STRINGBUFFER: %d, STRINGBUFFERSIZE - (sb-strbuff): %ld.\n",
      STRINGBUFFERSIZE, STRINGBUFFERSIZE -(sb-strbuff));

  return ret;
}
#endif  /* #ifdef INTERPRETER */
