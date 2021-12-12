/*******************************************************/
/* File: httpget.h                                     */
/* main function and variables                         */
/* timestamp: see COMPILEDATE                          */
/*******************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef MINGW
#include <winsock2.h>
#define S_IRGRP 0
#define S_IROTH 0
#else
#include <termios.h>
#define SOCKET int
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define NORMLOG     1

#ifdef CYGWIN
#undef _LARGEFILE64_SOURCE
#endif

#ifdef WITH_GETTEXT
#include <libintl.h>
#include <locale.h>
#define TEXTDOMAIN "httpget"
#define TEXTDOMAINPATH "/usr/share/locale"
#define _(txt) gettext(txt)
#else
#define _(txt) (txt)
#endif

#ifdef WITH_HTTPS
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define SSL_MODE_OFF 0                /* SSL ist ausgeschaltet                         */
#define SSL_MODE_ON  1                /* SSL ist eingeschaltet                         */
#define SSL_MODE_FORCE_CERT 2         /* SSL ist eingeschaltet Cert testen             */
#endif /* WITH_HTTPS */

#define HAS_VSNPRINTF 1

#ifdef SOLARIS
#include <netdb.h>
#endif

#ifdef _LARGEFILE64_SOURCE
#define ULONG unsigned long long
#define ULONGMUSTER "%llu"
#else
#define ULONG unsigned long
#define ULONGMUSTER "%lu"
#endif

#define PORT 80                       /* the port client will be connecting to http  */
#define PORT_S "80"
#define PORTHTTPS 443                 /* the port client will be connecting to https */
#define PORTHTTPS_S "443"

#define PROXY_PORT 3128               /* the default port für Proxy-Server      */
#define PROXY_PORT_S "3128"           /* the default port für Proxy-Server      */

#define MAXDATASIZE 204800            /* max number of bytes we can get at once */
#define MAX_ZEILENLAENGE 204800
#define MAX_URL_LEN MAXDATASIZE       /* max Länge URL               */
#define MAXHEADERSIZE 1024+MAXDATASIZE
#define MAX_ANZ_HEADERADD 100
#define MAX_LEN_HOST 3000
#define MAX_LEN_PATHNAME 3000
#define MAX_LEN_PROXY 300
#define MAX_LEN_URLPATH 10240
#define MAX_LEN_USERNAME 256
#define MAX_LEN_PASSWORD 128
#define MAX_LEN_AUTH MAX_LEN_USERNAME+MAX_LEN_PASSWORD

#define MAX_LEN_CHUNKS 204800

#define STRING_BUFFER_LEN 10240

#define TIMEOUT 600


#ifdef DEBUG
#define LOG(l, ...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(l, ...) ;
#endif

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#define CR '\xd'
#define LF '\xa'
#define NL "\xd\xa"

#define BOUNDARY "X---------------------------45879652368"

#define HTTP_NONE          0
#define HTTP_HEAD_OUT      1
#define HTTP_HEAD          2
#define HTTP_TRACE         4
#define HTTPS_GET_SSL_CERT 8

/***************************************************************************************/
/* Error-Codes                                                                         */
#define ERROR_OPEN_OUTPUT    1
#define ERROR_WRITE          2
#define ERROR_TIMEOUT        3
#define ERROR_NOT_OK         4
#define ERROR_EBUFFER        5
#define ERROR_RECEIVE        6
#define ERROR_CALLINGPARS    9

#define ERROR_HOSTNAME      10
#define ERROR_GETHOSTBYNAME 11
#define ERROR_SOCKET        12
#define ERROR_CONNECT_TIMEO 13
#define ERROR_SEND          14
#define ERROR_GETADDRINFO   15
#define ERROR_CONNECT       16

#define ERROR_SSL_NEW       21
#define ERROR_SSL_SET_FD    22
#define ERROR_SSL_CONNECT   23
#define ERROR_SSL_GET_PEER  23
#define ERROR_SSL_VERIFY    24

#define ERROR_CTX_NEW       31
#define ERROR_CTX_USE_C     32
#define ERROR_CTX_USE_P     33
#define ERROR_CTX_CHECK     34
#define ERROR_CTX_LOCATION  35
#define ERROR_CTX_SET_CIPHER 36

#define ERROR_WSA_STARTUP   41

#define ERROR_FCNTL_GET     51
#define ERROR_FCNTL_SET     52

#include "httpget_dec.h"
#include "httpget_var.h"
