/*******************************************************/
/* File: httpget.c                                     */
/* main function and variables                         */
/* timestamp: see COMPILEDATE                          */
/*******************************************************/

#include "httpget.h"
#include "httpget_version.h"
#include "signal.h"

/* start global variables (don't change this line) */

SOCKET sh;
int timeout_usecs = 1;
int logflag = false;
int code_url_flag = true;
int timeout_secs = TIMEOUT;
int connect_timeout_secs;

#ifdef WITH_HTTPS
int opt_ssl_mode = SSL_MODE_OFF;    /* Option SSL ist ausgewählt                       */
int ssl_mode = SSL_MODE_OFF;        /* wie opt_ssl_mode, wird nach öffnen Proxy gesetzt*/
char *ssl_key_file = NULL;
char *ssl_cert_file = NULL;
char *ssl_ca_file = NULL;
SSL  *ssl;                           /* Handle zum Lesen/Senden über SSL               */
int ssl_print_subject = 0;           /* 1: subject ausgeben, 2: subj. mit Header ausg. */
int ssl_print_issuer = 0;            /* 1: issuer ausgeben, 2: issuer mit Header ausg. */
#endif

#ifdef DEBUG
int log_send_flag = false;
#endif

char chunkbuffer[MAX_LEN_CHUNKS];
char *chunkpointer = chunkbuffer;
int chunk_header_flag;
int header_send_flag;
int http_v = 2;
int keepalive_flag = false;

int chunksend(char *s, size_t nb);

char *headeradd[MAX_ANZ_HEADERADD];
int anz_headeradd = 0;

/* end global variables (don't change this line) */

short gotsigterm = 0;


/***************************************************************************************/
/* void sig_handler(int signum)                                                        */
/*                      int signum: ausloesendes Signal                                */
/*      sig_handler wartet auf SIGUSR1|SIGTERM, gibt alle konsolidierten Zeilen aus    */
/*                  und beendet lfa4uls bei SIGTERM                                    */
/***************************************************************************************/
void sig_handler(int signum)
{
  if( signum == SIGTERM )
  { gotsigterm = 1;
  }
}


/***************************************************************************************/
/* int strcpyn_str(char *out, char **in, char *ez, long n)                             */
/*                  char *out: Ausgabezeile                                            */
/*                  char **in: Eingabezeile                                            */
/*                  char *ez : Zeichen, bei denen nicht weiterkopiert wird             */
/*                  long n   : Maximal n Zeichen                                       */
/*                  return   : 1, bei n erreicht                                       */
/*      strcpyn_str kopiert in nach out bis Zeichen aus ez gefunden wird               */
/***************************************************************************************/
int strcpyn_str(char *out, char **in, char *ez, long n)
{
  while( **in && NULL == strchr(ez, **in) && --n > 0 )
    *out++ = *(*in)++;
  *out = '\0';
  return n <= 0;
}


/***************************************************************************************/
/* void usage(char *prg)                                                               */
/*            char *prg: Name, mit dem Programm aufgerufen wurde                       */
/*      usage zeigt Aufrufparameter                                                    */
/***************************************************************************************/
void usage(char *prg)
{ char *p;

  if( (p = strrchr(prg, '/')) )
    p++;
  else
    p = prg;

  printf("usage: %7s [-a auth] [-b len] [-B path] [-c] [-C ct] [-d path] [-D path]\n"
         "               [-e] [-f file] [-h|-hh] [-H text] [-i] [-m] [-mf file]\n"
         "               [-mm mime] [-mn name] [-m-] [-o path] [-O] [-p pwd|-pf pwdfile] [-P proxy]\n"
         "               [-q] [-r range] [-R] [-s]"
#ifdef WITH_HTTPS
                       " [-S] [-Sa path] [-Sc path] [-Sf|-SF]\n"
         "               [-Sk path] [-SC]"
#endif  /* WITH_HTTPS */
                       " [-t sec] [-tc sec] [-T] [-u user]\n"
         "               [-v] [-?] host:port file1 [...]\n",
          p);
fputs(" -a auth  ", stdout);
         puts(_("set user:password in base64"));
fputs(" -b len   ", stdout);
         puts(_("set size of inputbuffer for reading from stdin"));
fputs(" -B path  ", stdout);
         puts(_("write inputbuffer to file on errors"));
fputs(" -c       ", stdout);
         puts(_("URL is coded"));
fputs(" -C ct    ", stdout);
         puts(_("set content-type of post data"));
fputs(" -d path  ", stdout);
         puts(_("set default path on server"));
fputs(" -D path  ", stdout);
         puts(_("path of post data"));
fputs(" -e       ", stdout);
         puts(_("on error write page to file/stdout"));
fputs(" -f file  ", stdout);
         puts(_("path to outputfile"));
fputs(" -h       ", stdout);
         puts(_("write received http-header to file/stdout"));
fputs(" -hh      ", stdout);
         puts(_("set method to HEAD"));
fputs(" -H text  ", stdout);
         puts(_("add text to http-header"));
fputs(" -i       ", stdout);
         puts(_("read URLs from stdin"));
fputs(" -m       ", stdout);
         puts(_("set mthod to post with multipart-content block"));
fputs(" -mf file ", stdout);
         puts(_("send file in multipart-content block"));
fputs(" -mm mime ", stdout);
         puts(_("set mime-type"));
fputs(" -mn name ", stdout);
         puts(_("set name for multipart-content block"));
fputs(" -m-      ", stdout);
         puts(_("read file for multipart-content from stdin"));
fputs(" -o path  ", stdout);
         puts(_("write output to path"));
fputs(" -O       ", stdout);
         puts(_("set method to POST"));
fputs(" -p pwd   ", stdout);
         puts(_("set password"));
fputs(" -pf file ", stdout);
         puts(_("set password from file"));
fputs(" -P proxy ", stdout);
         puts(_("set proxy-server"));
fputs(" -q       ", stdout);
         puts(_("quiet"));
fputs(" -r range ", stdout);
         puts(_("set byterange"));
fputs(" -R       ", stdout);
         puts(_("don't send HTTP-header"));
fputs(" -s       ", stdout);
         puts(_("write file to stdout"));
#ifdef WITH_HTTPS
fputs(" -S       ", stdout);
         puts(_("use https"));
fputs(" -Sa path ", stdout);
         puts(_("path to ssl-ca"));
fputs(" -Sc path ", stdout);
         puts(_("path to ssl-cert"));
fputs(" -Sf      ", stdout);
         puts(_("force server cert"));
fputs(" -SF      ", stdout);
         puts(_("force server cert and signed hostname"));
fputs(" -Sk path ", stdout);
         puts(_("path to ssl-key"));
fputs(" -SC      ", stdout);
         puts(_("get ssl-certificate"));
#endif  /* WITH_HTTPS */
fputs(" -T       ", stdout);
         puts(_("send method TRACE"));
fputs(" -t sec   ", stdout);
         puts(_("set timeout in seconds"));
fputs(" -tc sec  ", stdout);
         puts(_("set connect-timeout in seconds"));
fputs(" -u user  ", stdout);
         puts(_("set username"));
fputs(" -v       ", stdout);
         puts(_("show version"));
fputs(" -?       ", stdout);
         puts(_("show usage"));
}

#ifdef MINGW
ssize_t readpwd(char *zeile, size_t len)
{ scanf("%s", zeile);
}
#else
ssize_t readpwd(char *zeile, size_t len)
{ struct termios tsave, tset;
  char c, *z;

  z = zeile;
  if( tcgetattr(0, &tsave) )
    return -1;

  tset = tsave;
  tset.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
  if( tcsetattr(0, TCSAFLUSH, &tset) )
    return -1;

  while( (c = getchar()) != '\n' && (z-zeile) < len - 1 )
  { if( c == 127 )
    { if( z > zeile )
      { z--;
        printf("\b \b");
      }
    }
    else
    { *z++ = c;
      printf("*");
    }
  }
  printf("\n");
  *z = '\0';
  tcsetattr(0, TCSAFLUSH, &tsave);
  return z-zeile;
}
#endif

int main(int argc, char **argv)
{ char outputfile[MAX_LEN_PATHNAME], getfile[MAX_URL_LEN], host[MAX_LEN_HOST];
  char stdin_url[MAX_URL_LEN];
  char outpath[MAX_LEN_PATHNAME];
  char proxy[MAX_LEN_PROXY];
  char user[MAX_LEN_USERNAME], pwd[MAX_LEN_PASSWORD];
  char auth[MAX_LEN_AUTH];
  int options = 0, i;
  char urlpath[MAX_LEN_URLPATH];
  char *errorbufferfile = NULL;
  char *post_data_file = NULL;
  char *content_type = NULL;
  int stdout_flag = false;
  int stdin_url_flag = false;
  int quiet_flag = false;
  int http_flag = true;
  int error_out_flag = false;
  int http_method = HTTP_NONE;
  int ret = 0;
  int stdin_urls_flag = false;
  char *multipart_file;
  char *multipart_mimetype;
  char *multipart_name;
  int multipart_flag = false;
  int post_flag = false;
  int multipart_stdin_flag = false;
  unsigned long iobuffersize = 0;
  char *range;
  char *iobuffer;
  char zeile[STRING_BUFFER_LEN];
#ifdef MINGW
  WSADATA wsaData;
#endif

#ifdef WITH_GETTEXT
  setlocale(LC_ALL, "");
  textdomain(TEXTDOMAIN);
  bindtextdomain(TEXTDOMAIN, TEXTDOMAINPATH);
#endif

  strcpy(urlpath, "/");

  multipart_name = multipart_file = multipart_mimetype = "";

  outpath[0] = outputfile[0] = getfile[0] = auth[0] =
    user[0] = pwd[0] = proxy[0] = '\0';
  range = NULL;

  while( argc > options+1
         && argv[options+1][0] == '-' && argv[options+1][1] != '\0' )
  { options++;
    switch( argv[options][1] )
    { case 'b': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                iobuffersize = atol(argv[options]);
                break;
      case 'B': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                errorbufferfile = argv[options];
                break;
      case 'c': code_url_flag = false;
                break;
      case 'C': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                content_type = argv[options];
                break;
      case 'H': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
	        if( anz_headeradd < MAX_ANZ_HEADERADD )
		  headeradd[anz_headeradd++] = argv[options];
		break;
      case 'd': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                if( argv[options][0] == '/' )
                  strncpy(urlpath, argv[options], MAX_LEN_URLPATH-1);
                else
                  strncpy(urlpath+1, argv[options], MAX_LEN_URLPATH-2);
                if( urlpath[strlen(urlpath)] != '/' )
                  strcat(urlpath, "/");
                break;
      case 'D': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                post_data_file = argv[options];
                break;
      case 'e': error_out_flag = true;
                break;
      case 'f': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                strncpy(outputfile, argv[options], MAX_LEN_PATHNAME-1);
                break;
      case 'i': stdin_urls_flag = true;
                break;
      case 'u': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                strncpy(user, argv[options], MAX_LEN_USERNAME-1);
                break;
      case 'p': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                if( argv[options-1][2] == 'f' )
                { FILE *ptr;
                  if( NULL != (ptr = fopen(argv[options], "r")) )
                  { int c;
                    char *p;
                    p = pwd;
                    for(;;)
                    { c = getc(ptr);
                      if( !c || c == '\xd' || c == '\xa' || c == EOF || p-pwd > MAX_LEN_PASSWORD-1 )
                        break;
                      *p++ = c;
                    }
                    *p = '\0';
                    fclose(ptr);
                  }
                  else
                    fprintf(stderr, _("Error on opening file %s.\n"), argv[options]);
                }
                else
                { strncpy(pwd, argv[options], MAX_LEN_PASSWORD-1);
                  { char *p;
                    p = argv[options];
                    while( *p )
                      *p++ = 'x';
                  }
                }
                break;
      case 'a': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                strncpy(auth, argv[options], MAX_LEN_AUTH-1);
                { char *p;
                  p = argv[options];
                  while( *p )
                    *p++ = 'x';
                }
                break;
      case 'o': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                strncpy(outpath, argv[options], MAX_LEN_PATHNAME-1);
                if( outpath[strlen(outpath)] != '/' )
                  strcat(outpath, "/");
                break;
      case 'P': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                strncpy(proxy, argv[options], MAX_LEN_PROXY-1);
                break;
      case 'm': multipart_flag = true;
                if( argv[options][2] == 'f' )
                { if( argc < ++options+1 )
                  { usage(argv[0]);
                    return ERROR_CALLINGPARS;
                  }
                  multipart_file = argv[options];
                }
                else if( argv[options][2] == 'm' )
                { if( argc < ++options+1 )
                  { usage(argv[0]);
                    return ERROR_CALLINGPARS;
                  }
                  multipart_mimetype = argv[options];
                }
                else if( argv[options][2] == 'n' )
                { if( argc < ++options+1 )
                  { usage(argv[0]);
                    return ERROR_CALLINGPARS;
                  }
                  multipart_name = argv[options];
                }
                else if( argv[options][2] == '-' )
                  multipart_stdin_flag = true;
                break;
      case 'O': post_flag = true;
                break;
      case 'q': quiet_flag = true;
                break;
      case 'r': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                range = argv[options];
                break;
      case 's': stdout_flag = true;
                break;
#ifdef WITH_HTTPS
      case 'S': opt_ssl_mode = opt_ssl_mode | SSL_MODE_ON;
                if( argv[options][2] == 'c' )
                { if( argc < ++options+1 )
                  { usage(argv[0]);
                    return ERROR_CALLINGPARS;
                  }
                  ssl_cert_file = argv[options];
                }
                else if( argv[options][2] == 'k' )
                { if( argc < ++options+1 )
                  { usage(argv[0]);
                    return ERROR_CALLINGPARS;
                  }
                  ssl_key_file = argv[options];
                }
                else if( argv[options][2] == 'a' )
                { if( argc < ++options+1 )
                  { usage(argv[0]);
                    return ERROR_CALLINGPARS;
                  }
                  ssl_ca_file = argv[options];
                }
                else if( argv[options][2] == 'f' )
                  opt_ssl_mode = opt_ssl_mode | SSL_MODE_FORCE_CERT;
                else if( argv[options][2] == 'F' )
                  opt_ssl_mode = opt_ssl_mode | SSL_MODE_FORCE_CERT | SSL_MODE_FORCE_CN;
                else if( argv[options][2] == 's' )
                  ssl_print_issuer = 1;
                else if( argv[options][2] == 'i' )
                  ssl_print_subject = 1;
                else if( argv[options][2] == 'S' )
                  ssl_print_issuer = 2;
                else if( argv[options][2] == 'I' )
                  ssl_print_subject = 2;
                else if( argv[options][2] == 'C' )
                  http_method |= HTTPS_GET_SSL_CERT;
                LOG(20, "httpget, op_ssl_mode: %d\n", ssl_mode);
                break;
#endif  /* WITH_HTTPS */
     case 'T':  http_method |= HTTP_TRACE;
                break;
#ifdef DEBUG
      case 'Q': log_send_flag = true;
                break;
#endif
      case 't': if( argc < ++options+1 )
                { usage(argv[0]);
                  return ERROR_CALLINGPARS;
                }
                if( argv[options-1][2] == 'c' )
                  connect_timeout_secs = atoi(argv[options]);
                else
                  timeout_secs = atoi(argv[options]);
                break;
      case 'U': stdin_url_flag = true;
                break;
      case 'h': if( argv[options][2] == 'h' )
                  http_method |= HTTP_HEAD;
                http_method |= HTTP_HEAD_OUT;
                break;
      case 'R': http_flag = false;
                break;
      case 'v': printf("httpget version %s, %s, build %d.\n%s\n",
                       VERSION, COMPILEDATE, BUILD, COPYRIGHT);
                exit(0);
      default : usage(argv[0]);
                exit(1);
    }
  }

  if( !auth[0] && user[0] )
  { if( !pwd[0] )
    { printf(_("password for user %s: "), user);
      readpwd(pwd, MAX_LEN_PASSWORD);
    }
    strcat(user, ":");
    strcat(user, pwd);
    encode_base64((unsigned char *)auth, (unsigned char *)user);
  }

  if( argc <= 1+options )                  /* Hostname angegeben?                      */
  { fputs(_("hostname: "), stdout);        /* nein, dann erfragen                      */
    scanf("%s", host);
  }
  else
  { char *h, *p, *q;

    if( !strncmp(argv[1+options], "http://", 7) )
      h = argv[1+options]+7;
    else if( !strncmp(argv[1+options], "https://", 7) )
    { h = argv[1+options]+8;
#ifdef WITH_HTTPS
      opt_ssl_mode = opt_ssl_mode | SSL_MODE_ON;
#endif
    }
    else
      h = argv[1+options];
    p = strchr(h, '/');
    q = strchr(h, '@');
    if( q && (!p || q < p) )
    { if( strcpyn_str(user, &h, "@:", MAX_LEN_USERNAME) )
        return 1;
      if( *h == ':' )
      { if( strcpyn_str(pwd, &h, "@", MAX_LEN_PASSWORD) )
          return 1;
      }
      if( *h++ != '@' )
        return 1;
	}
    if( strcpyn_str(host, &h, "/", MAX_LEN_HOST) )
      return 1;
    if( *h == '/' )
      return httpget(urlpath, outputfile, h, host, auth, quiet_flag, http_flag,
                     proxy, http_method, error_out_flag, false,
                     multipart_flag, multipart_name, multipart_file, multipart_mimetype,
                     multipart_stdin_flag, range, post_flag, NULL, 0, post_data_file,
                     content_type);
  }

#ifdef MINGW
  if( WSAStartup(MAKEWORD(2, 2), &wsaData) )
    return ERROR_WSA_STARTUP;
#endif

  if( http_method & HTTPS_GET_SSL_CERT )
    return httpget(urlpath, outputfile, getfile, host, auth, quiet_flag, http_flag,
                   proxy, http_method, error_out_flag, false,
                   multipart_flag, multipart_name, multipart_file, multipart_mimetype,
                   multipart_stdin_flag, range, post_flag, NULL, 0, post_data_file,
                   content_type);

  if( argc <= 2+options )                 /* ist Dateiname angegeben?                  */
  { if( isatty(fileno(stdin)) )           /* nein, stdin mit tty verbunden?            */
    { fputs(_("filename: "), stdout);     /* Ja, dann Datei erfragen                   */
      scanf("%s", getfile);
      return httpget(urlpath, outputfile, getfile, host, auth, quiet_flag, http_flag,
                     proxy, http_method, error_out_flag, false,
                     multipart_flag, multipart_name, multipart_file, multipart_mimetype,
                     multipart_stdin_flag, range, post_flag, NULL, 0, post_data_file,
                     content_type);
    }
    else if( !stdin_url_flag && !stdin_urls_flag )
      puts(_("specify file name."));
  }

                                     /* zu empfangende Dateien als Parameter angegeben */
  for( i = 2+options; i < argc; i++ )   /* alle Dateien                                */
  {
#ifdef WINDOWSQUOTING
    if( (argv[i][0] == '"' && argv[i][strlen(argv[i])-1] == '"')
        || (argv[i][0] == '\'' && argv[i][strlen(argv[i])-1] == '\'') )
    { strcpy(getfile, argv[i]+1);
      getfile[strlen(getfile)-1] = '\0';
    }
    else
#endif
    strcpy(getfile, argv[i]);           /* Dateiname aus Parameterliste                */
    if( !stdout_flag && !*outputfile )  /* nicht stdout und -f nicht angegeben         */
    { if( *outpath )
      { char *p;
		strcpy(outputfile, outpath);
        if( NULL != (p = strrchr(getfile, '/')) )
          strcat(outputfile, p+1);
        else
          strcat(outputfile, getfile);
      }
      else
        strcpy(outputfile, getfile);    /* Orginaldateiname übernehmen                 */
    }
    if( iobuffersize )
    { size_t nb, b;
      size_t ms;

      if( errorbufferfile )
        signal(SIGTERM, sig_handler);

      LOG(1, "main, iobuffersize: %lu.\n", iobuffersize);

      if( iobuffersize < STRING_BUFFER_LEN )
        ms = iobuffersize;
      else
        ms = STRING_BUFFER_LEN;

      if( NULL == (iobuffer = malloc(iobuffersize)) )
      { fprintf(stderr, _("Error on allocating buffer.\n"));
        exit(1);
      }
      b = 0;
      ret = 0;
      while( 0 < (nb = read(STDIN_FILENO, zeile, ms)) )
      { if( gotsigterm && errorbufferfile )
        { ret = 1;
          break;
        }
        LOG(7, "main, nb: %lu.\n", (unsigned long)nb);
        if( b + nb > iobuffersize )               /* noch Platz im Puffer?             */
        {                                         /* nein, bisherigen Puffer senden    */
          if( (ret = httpget(urlpath, outputfile, getfile, host, auth, quiet_flag,
                        http_flag, proxy, http_method, error_out_flag, false,
                        multipart_flag, multipart_name, multipart_file,
                        multipart_mimetype, multipart_stdin_flag, range, post_flag,
                        iobuffer, b, post_data_file, content_type)) )
            break;
          b = 0;                                  /* Puffer leer                       */
        }
        memcpy(iobuffer+b, zeile, nb);            /* neue Daten anhängen               */
        b += nb;
      }
      LOG(3, "main, nach while-read stdin, b: %lu.\n", (unsigned long)b);
      if( b && !ret )
        ret = httpget(urlpath, outputfile, getfile, host, auth, quiet_flag,
                      http_flag, proxy, http_method, error_out_flag, false,
                      multipart_flag, multipart_name, multipart_file,
                      multipart_mimetype, multipart_stdin_flag, range, post_flag,
                      iobuffer, b, post_data_file, content_type);
      LOG(3, "main, nach while-read stdin, ret: %d, nb: %lu.\n", ret,
          (unsigned long)nb);
      if( ret && errorbufferfile )
      { int oh;

#ifdef _LARGEFILE64_SOURCE
        if( 0 <= (oh = open64(errorbufferfile, O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR)) )
#else
#ifdef MINGW
        if( 0 > (oh = open(errorbufferfile, O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR | O_BINARY)) )
#else
        if( 0 > (oh = open(errorbufferfile, O_WRONLY | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR )) )
#endif
#endif
        { if( b )
            write(oh, iobuffer, b);
          while( nb > 0 )
          { write(oh, zeile, nb);
            LOG(33, "vor read...\n");
            nb = read(STDIN_FILENO, zeile, ms);
            LOG(33, "nach read, nb: %lu\n", (unsigned long)nb);
          }
          close(oh);
        }
        return ERROR_EBUFFER;
      }
      return 0;
    }
    else
      ret = httpget(urlpath, outputfile, getfile, host, auth, quiet_flag, http_flag,
                    proxy, http_method, error_out_flag, false,
                    multipart_flag, multipart_name, multipart_file,
                    multipart_mimetype, multipart_stdin_flag, range, post_flag,
                    NULL, 0, post_data_file, content_type);
    *outputfile = '\0';                 /* -f <dateiname> nur einmal Verwenden         */
  }

  if( stdin_url_flag || stdin_urls_flag )
  { if( !stdout_flag && !*outputfile )  /* nicht stdout und -f nicht angegeben         */
    { if( *outpath )
      { strcpy(outputfile, outpath);
        strcat(outputfile, "stdin");
      }
      else
        strcpy(outputfile, "stdin");    /* Orginaldateiname übernehmen                 */
    }
    if( stdin_url_flag )
      fread(stdin_url, 1, MAX_URL_LEN, stdin);
    ret = httpget(urlpath, outputfile, stdin_url, host, auth, quiet_flag, http_flag,
                  proxy, http_method, error_out_flag, stdin_urls_flag,
                  multipart_flag, multipart_name, multipart_file,
                  multipart_mimetype, multipart_stdin_flag, range, post_flag,
                        NULL, 0, post_data_file, content_type);
  }

#ifdef MINGW
  WSACleanup();
#endif

  return ret;
}
