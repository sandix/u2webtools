/********************************************************/
/* Headerdatei fuer unix2webd mit globalen Definitionen */
/* timestamp: 2017-02-12 14:40:54                       */
/********************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/termios.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include <dirent.h>
#include <limits.h>

#ifdef WITH_SSL

#define WITH_HTTPS 1
#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/md5.h>

#ifdef ENABLE_DH_CURVE
#include <openssl/dh.h>
#endif

#else

#undef ENABLE_DH_CURVE
#define MD5_DIGEST_LENGTH 16

typedef struct {
  unsigned int  state[4];
  unsigned int  bits[2];
  unsigned char in[64];
} MD5_CTX;

#endif /* WITH_SSL */

#ifdef WITH_GETTEXT
#include <libintl.h>
#include <locale.h>
#define U2WTEXTDOMAIN "unix2web"
#define U2WTEXTDOMAINPATH "/usr/share/locale"
#define _(txt) gettext(txt)
#else
#define _(txt) (txt)
#endif

#ifdef WITH_MMAP
#include <sys/mman.h>
#endif

#ifdef INTERPRETER
#define OH STDOUT_FILENO
#else
#ifndef U2W_PASSWD
#define WEBSERVER 1
#endif
#endif

#ifndef HAS_PAM
#define HAS_SHADOW 1
#endif
#define HAS_DAEMON 1
#define HAS_VSNPRINTF 1

#ifdef LINUX
#include <sys/ioctl.h>
#include <time.h>
#define SCANDIR 1
#define LONGLONG 1
#define MAX_INT_VALUE LLONG_MAX
#define NUM_DIGITS_TEST_INT 19
#else
#define atoll(x) atol(x)
#define MAX_INT_VALUE LONG_MAX
#define NUM_DIGITS_TEST_INT 10
#endif

#ifdef HPUX
#include <sys/ioctl.h>
#define TIOCNOTTY _IO('t', 113)
#undef HAS_SHADOW
/* #define HAS_TRUSTED 1 */
#define LONGLONG 1
#endif

#ifdef OLDUNIX
#undef HAS_VSNPRINTF
#undef HAS_SHADOW
int snprintf(char *, size_t, char *, ...);
#define strcasestr(s1, s2) strstr(s1, s2)
#define isblank(s) (s == ' ' || s == '\t')
#else
#define WITH_REGEX 1
#include <stdint.h>
#endif

#define min(a,b) ((a) < (b) ? (a) : (b))

#ifdef CYGWIN
#include <windows.h>
#include <sys/cygwin.h>
#include <sys/ioctl.h>
#define TIOCNOTTY _IO('t', 113)
#undef HAS_SHADOW
#define SCANDIR 1
#define LONGLONG 1
#endif

#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#ifdef MYSQLCLIENT
#include <mysql/mysql.h>
#define STD_MYSQLPORT  3306
//#define DBCLIENT 1
#define MAYDBCLIENT 1
#endif

#ifdef MARIADBCLIENT
#include <mariadb/mysql.h>
#define STD_MYSQLPORT  3306
//#define DBCLIENT 1
#define MAYDBCLIENT 1
#endif

#ifdef POSTGRESQLCLIENT
#include <pgsql/libpq-fe.h>
#define STD_PGSQLPORT  5432
#ifndef DBCLIENT
//#define DBCLIENT 1
#endif
#endif

#ifdef MAXDBCLIENT
#include <SQLDBC.h>
#define STD_MAXDBPORT  7210
#ifndef DBCLIENT
//#define DBCLIENT 1
#endif
#endif

#ifdef SQLITE3
#include <sqlite3.h>
#endif

#define false 0
#define true !false

#define DEFUSER             "nobody"

#define EVERYBODY_PATH      "everybody"

#define PORT 3232                    /* Standard Port fuer die Verbindung              */

#define TIMEOUT_SECS 10              /* 30 Sekunden auf Daten vom Client warten        */
#define MAX_TIMEOUT_SECS 36000       /* Maximal 36000 Sekunden auf Daten warten        */



#define NOLOG              0
#define NORMLOG            1
#define LOGCONNECTION      2
#define LOGCMDERRORS       4
#define LOGNOCMDERRORS     8
#ifdef DEBUG
#define LONGLOG          0x10
#define LOGSENDDATA   0x40000
#define LOGREADDATA   0x80000
#define LOGALLTIME   0x100000
#endif


/***************************************************************************************/
/* Pfade und Standardseiten                                                            */
/***************************************************************************************/
#define STD_GET_HOME "./html"        /* Standardpfad fuer das Rootverzeichnis          */
#define STD_EXT  ".u2w"              /* Extension fuer Auszuwertende Dateien           */
#define U4W_EXT  ".u4w"              /* Extension fuer Auszuwertende Dateien           */
#define U5W_EXT  ".u5w"
#define STD_INC_EXT  ".i2w"          /* Extension fuer Auszuwertende Include Dateien   */
#define HTML_EXT ".u3w"              /* Extension fuer HTML-Format mit Programmaufrufen*/
#define SHELL_EXT ".s2w"             /* Extension für Text-Format mit Programmaufrufen */
#define BIN_SHELL_EXT ".s3w"         /* Extension für Binär-Format mit Programmaufrufen*/
#define S4W_EXT ".s4w"               /* Extension für Text-Format mit Programmaufrufen */
#define S5W_EXT ".s5w"               /* Extension für Binär-Format mit Programmaufrufen*/
#define ACC_EXT   ".uaw"             /* Extension für Accessdateien unter DOS/Windows  */
#define EDIT_PAR "&edit"             /* file.u2w?&edit sendet u2w ohne Umwandlung      */
#define STD_PAGE1 "index" STD_EXT    /* Standartdatei, wenn keine angegeben            */
#define STD_PAGE2 "index.html"       /* Standartdatei, wenn STD_PAGE1 nicht gefunden   */
#define STD_PAGE3 "index.htm"        /* Seite, wenn STD_PAGE2 nicht gefunden           */
#define DIR_SUCH_PATTERN "such"      /* Dir/?such=pattern                              */
#define HTTP_HOST_UNKNOWN "unknown"  /* Wenn -hH gesetzt und http_host ungültig        */


typedef enum { NO_MODE = 0, U3W_MODE = 1, U2W_HTML_MODE = 2, U2W_MODE = 4,
               U4W_MODE = 8, U5W_MODE = 16,
               S2W_MODE = 32, S2W_HTTPHEAD_MODE = 64, S3W_MODE = 128, S4W_MODE = 256,
               S5W_MODE = 512, RAW_MODE = 1024, RAW_HTTPHEAD_MODE = 2048 }
  u2w_mode_type;


/***************************************************************************************/
/* Zugriffssteuerung ueber .access File                                                */
/* Aufbau .access File:                                                                */
/* username1                                                                           */
/* username2                                                                           */
/* ...                                                                                 */
/* Ist eine .access Datei in einem Verzeichnis angelegt, dann dürfen nur Benutzer,     */
/* die eingetragen sind, auf die Dateien des Verzeichnisses zugreifen.                 */
/***************************************************************************************/
#ifdef DOS
#define ACCESS_FILE "access" ACC_EXT /* Datei, in der ggf. Zugriffsrechte stehen       */
#else
#define ACCESS_FILE ".access"        /* Datei, in der ggf. Zugriffsrechte stehen       */
#endif

/***************************************************************************************/
/* Zugriffssteuerung ueber .passwd File                                                */
/* Aufbau .passwd File:                                                                */
/* [a][r][w] user1:passwort1 [[unixuser]|. [pfad]]                                     */
/* [a][r][w] user2:passwort2 [[unixuser]|. [pfad]]                                     */
/* ...                                                                                 */
/* Dabei stehen user:passwort mit Base64 verschluesselt in der Datei                   */
/* hat ein user das zeichen a stehen, dann darf er ueber die Datei .passwd bearbeiten  */
/* Die URL dazu lautet: /pfad/.passwd?&access                                          */
/* Der Eintrag für keine Angabe von User und PWD ist NO_USER_PASS                      */
/* *************************************************************************************/
#ifdef DOS
#define PASSWORD_FILE "passwd" ACC_EXT /* Datei mit Usernamen und Passworten stehen    */
#define PWDNAME_X_POS 5              /* 5. Zeichen von Hinten wird durch X erstetzt,   */
                                     /* um Kopie fürs Ändern des Passwortes zu erzeigen*/
#else
#define PASSWORD_FILE ".passwd"      /* Datei, inder ggf. Username und Passworte stehen*/
#define PWDNAME_X_POS 1              /* 1. Zeichen von Hinten wird durch X erstetzt,   */
                                     /* um Kopie fürs Ändern des Passwortes zu erzeigen*/

#endif
#define ADMIN_ACCESS_CHAR 'a'        /* a in PASSWORD_FILE: Adminrecht a. PASSWORD_FILE*/
#define READ_ACCESS_CHAR  'r'        /* r in PASSWORD_FILE: Leserecht im Verzeichnis   */
#define WRITE_ACCESS_CHAR 'w'        /* w in PASSWORD_FILE: Schreibrecht im Verzeichnis*/
#define PWDEDITMODE     "pwdedit"    /* Parameter pwdedit auf PWDSAVE beim Sichern     */
#define PWDSAVE         "save763946" /* Magic Number für PWDEDITMODE                   */
#define PWDGLOBSAVE     "save768446" /* Magic Number für PWDEDITMODE Global            */
#define PWDUSERSAVE     "save674856" /* Magic Number für PWDCHANGEMODE                 */
#define PWDUSERGLOBSAVE "save674765" /* Magic Number für PWDCHANGEMODE Global          */
#define NO_USER_AUTH      "-"        /* "-" als User: nicht angemeldeter User          */
#define NO_USER_AUTH_CHAR '-'        /* '-' als User: nicht angemeldeter User          */
#define NO_USER_AUTH_B64  "LTo="     /* "-:" Base64 codiert                            */
#define GLOB_USER_AUTH    "+"        /* "+": in globaler Datei weitersuchen            */
#define GLOB_USER_AUTH_CHAR '+'      /* '+': in globaler Datei weitersuchen            */
#define GLOB_USER_AUTH_B64 "Kzo="    /* "+:" Base64 codiert                            */
#define ALL_USER_AUTH     "*"        /* "*" alle User sind erlaubt                     */
#define ALL_USER_AUTH_CHAR '*'       /* '*' alle User sind erlaubt                     */
#define ALL_USER_AUTH_B64 "Kjo="     /* "*:" Base64 codiert                            */
#define MAX_AUTH_LEN      1024       /* Maximale Länge einer Zeile der .passwd         */
#define MAX_ACC_LEN       4          /* Maximale Länge des arw Strings                 */
#define ACCESS_PAR        "&access"  /* Verändern der Zugriffsrechte .passwd, .hosts   */
#define ACCESS_GLOB_PAR   "&global"  /* Verändern der globalen .passwd, .hosts         */
#define PWD_CHANGE_GLOB_PAR "&change"  /* Ändern des eigenen globalen Passwortes       */
#define PWD_CHANGE_PAR "&pwdchange"  /* Ändern des eigenen Passwortes                  */
#define SAVE_GLOBAL_ACC   ".global"  /* Name fürs Sichern der globalen .passwd, .hosts */
#define CHANGE_GLOBAL_PWD ".globchng"/* Name fürs Ändern des globalen Passwortes       */
#define PWDALT_PAR_NAME   "palt"     /* Name des Feldes mit altem Passwort             */
#define PWDNEU_PAR_NAME   "pneu"     /* Name des Feldes mit neuem Passwort             */
#define PWDVER_PAR_NAME   "pver"     /* Name des Feldes mit Passwort Wiederholung      */
#define LOGIN_PAR         "&login"   /* Name des Parameters für neue Anmeldung         */
#define STATUS_PAR        "status"   /* "&status" Status-Page /?&status                */
#define STATUS_FLUSH_PAR  "status_flush" /* "&status" Status-Page /?&status_flush      */
#define NEW_LOGIN         LOGIN_PAR "="  /* Neue Anmeldung gewünscht                   */
#define UPLOAD            "upload.uxw"  /* interner Uploadpfad                         */


/***************************************************************************************/
/* Zugriffssteuerung ueber .hosts File                                                 */
/* Aufbau .hosts File:                                                                 */
/* r|w|rw ip-adresse1                                                                  */
/* r|w|re ip-adresse2                                                                  */
/* ...                                                                                 */
/* Existiert eine .hosts Datei, dann dürfen nur Rechner, die in der Datei verzeichent  */
/* sind auf dieses Verzeichnis zugreifen. r bedeutet nur Lesen, w nur Schreiben, rw    */
/* Lesen und Schreiben ist erlaubt. Die ip-adresse in der Form: a.b.c.d ohne führende  */
/* Nullen z. B.: 10.3.18.30                                                            */
/***************************************************************************************/
#ifdef DOS
#define HOSTS_FILE "hosts" ACC_EXT   /* Datei mit zugriffsberechtigten hosts           */
#else
#define HOSTS_FILE ".hosts"          /* Datei mit zugriffsberechtigten hosts           */
#endif
#define NO_IP_AUTH      "-"          /* "-" als IP: nicht aufgeführte                  */
#define GLOB_IP_AUTH    "+"          /* "+" als IP: in globaler Datei weitersuchen     */
#define HOSTSEDITMODE   "hostsedit"  /* Parameter hostsedit auf HOSTSSAVE beim Sichern */
#define HOSTSSAVE       "save874532" /* Magic Number für HOSTSEDITMODE                 */
#define HOSTSGLOBSAVE   "save873432" /* Magic Number für HOSTSEDITMODE Global          */


/***************************************************************************************/
/* SSL-Default-Parameter                                                               */
/***************************************************************************************/
#define SSL_CERT_FILE    "cert.pem"    /* STD-Cert-File                                */
#define SSL_KEY_FILE     "key.pem"     /* STD-Key-File                                 */
#define SSL_MODE_OFF 0                 /* SSL ist ausgeschaltet                        */
#define SSL_MODE_ON  1                 /* SSL ist eingeschaltet                        */
#define SSL_MODE_FORCE_CLIENT 2        /* SSL ist ein und Client Cert notwendig        */
#define SSL_MODE_ASK_CLIENT   4        /* SSL ist ein und Client Cert anfordern        */
#define PRINT_SSLSUBJECT "sslsubject"  /* SSL-Subject                                  */
#define PRINT_SSLISSUER  "sslissuer"   /* SSL-Issuer                                   */


/***************************************************************************************/
/* Stringlaengen                                                                       */
/***************************************************************************************/
#define BACKLOG               2000   /* Zahl wartender connections, die gehalten werden*/
#define MAX_HTTPHEADER_SIZE  60000   /* Maximale Headergröße                           */
#define MAX_POST_CONTENT_SIZE 204800 /* Maximallaenge Content bei POST ohne Multipart  */
#define MAX_LEN_CHUNKS        8192   /* Maximallaenge eines Chunks                     */
#define MAX_LEN_FILENAME      4096   /* Maximallaenge eines Pfades                     */
#define MAX_LEN_CLASSNAME     1024   /* Maximallaenge CSS-class                        */
#define HOME_LENGTH           1024   /* Maximallaenge von Rootpfad                     */
#define MAX_PATH_LEN          4096   /* Maximallaenge des PATH im Environment          */
#define MAX_MIME_LEN           100   /* Maximallaenge des Mime-Types                   */
#define MAX_ZEILENLAENGE     65000   /* Maximallaenge einer Zeile in der Eingabedatei  */
#define MAX_LEN_GLOBALPWDLINE 4000   /* Maximallaenge einer Zeile in globaler .passwd  */
#define MAX_LEN_PRG_PARS     65000   /* Maximallaenge eines Parameters                 */
#define MAX_LEN_LIST_PARS     6500   /* Maximallaenge eines List-Parameters            */
#define MAX_LIST_LEN            30   /* Maximale Anzahl Elemente bei %list             */
#define MAX_ANZ_PRG_PARS        10   /* Eine Funktion kann maximal 10 Parameter haben  */
#define MAX_ANZ_QUOTE_PARS       3   /* Die ersten 3 Parameter können gequotet werden  */
#define MAX_MULTIPART_BLOCK  32000   /* Blockgröße beim Lesen von Multipart/Form-Data  */
#define MAX_ANZ_AUTO_DELETE_FILES 300 /* Es können maximal 300 Dateien automatisch     */
                                     /* gelöscht werden                                */
#define MAX_BOUNDARY_LEN       128   /* Maximale Länge des Boundary Strings zum Suchen */
#define MAX_REFLAENGE         4096   /* Maximallaenge eines Hyperlinkpfades            */
#define MAX_LINKLAENGE        1024   /* Maximallaenge einer Hyperlinkbeschreibung      */
#define MAX_DIR_LEN           2048   /* Maximallaenge eines Direcotrypfades            */
#define MAX_NAME_LEN           256   /* Maximallaenge eines Dateinamens                */
#define MAX_PAR_NAME_LEN       256   /* Maximallaenge eines Parameternamens            */
#define MAX_ANZ_HPARS        10000   /* Maximale Anzahl Parameter (ohne Malloc)        */
#define MAX_ANZ_HIPARS         100   /* Maximale Anzahl Init-Parameter (ohne Malloc)   */
#define MAX_ANZ_HTTPHEADLINES   30   /* Maximale Anzahl zusätzlicher HTTP-Headerzeilen */
#define ANZ_HPARS_ALLOC       1000   /* 1000 Parameter allokieren                      */
#define MAX_ANZ_HINITPARS      100   /* Maximale Anzahl INIT-Parameter (ohne Malloc)   */
#define ANZ_HINITPARS_ALLOC     10   /* 10 INIT Parameter allokieren                   */
#define MAX_ANZ_WERTE        50000   /* Maximale Anzahl zurueckgelieferte Werte (o. Ma)*/
#define ANZ_WERTE_ALLOC       1000   /* 1000 Werte allokieren                          */
#define MAX_LEN_QUOTE          128   /* Maximale Laenge Quote                          */
#define MAX_LEN_AUTHORIZE      256   /* Maximale Laenge des Authorize Strings          */
#define MAX_LEN_DATESTRING      64   /* Maximale Laenge eines Datumstrings             */
#define MAX_LEN_CONTENT_TYPE   256   /* Maximale Laenge des Content-Type-Strings       */
#define PARLEN               65000   /* Maximallaenge der Parameter                    */
#define STRINGBUFFERSIZE   1024000   /* Groesse des Stringpuffers                      */
#ifdef MALLOC
#define STRINGINITBUFFERSIZE  1024   /* Groesse des Stingpuffer fuer INIT-Variablen    */
#else
#define STRINGINITBUFFERSIZE 10240   /* Groesse des Stingpuffer fuer INIT-Variablen    */
#endif
#define USER_LENGTH            256   /* Maximale Laenge eines Usernamens               */
#define PWD_LENGTH             256   /* Maximale Laenge eines Passwortes               */
#ifdef WITH_IPV6
#define MAX_IP_LEN INET6_ADDRSTRLEN  /* Maximale Länge einer IP-Adresse                */
#else
#define MAX_IP_LEN INET_ADDRSTRLEN   /* Maximale Länge einer IP-Adresse                */
#endif
#define FOR_STACK_SIZE          10   /* Maximal 10 geschachtelte For-Schleifen         */
#define WHILE_STACK_SIZE        10   /* Maximal 10 geschachtelte While-Schleifen       */
#define TABLEROWSSTACKSIZE       4   /* 4 verschachtelte Tabellen                      */
#define MAX_ANZ_OPEN_FILES      10   /* 10 Dateien können gleichzeitig offen sein      */
#define ENVIRONMENTBUFFERSIZE 1024   /* Environmentbuffer, wenn kein MALLOC            */
#define HASH_TABLE_SIZE       1000   /* Groesse der Hash Primaertabelle                */
#define HASH_SHIFT               2   /* Linksshift um 2                                */
#define HASH_INIT             2264   /* Initialwert def Hashberechnung                 */
#define U2W_COMMAND_HASH_SIZE  400   /* Hashtabelle für U2W-Kommandos                  */
#define U2W_HASH_TABLE_SIZE    300   /* Hash Primaertabelle für U2W-Kommandos          */
#define U2W_HASH_SHIFT           2   /* Hash Linksshift um 2                           */
#define U2W_HASH_INIT         7428   /* Hash Init für U2W                              */
#define MAX_LEN_DEBUG_HEXSTRING 1024 /* Maximallaenge hexstring Return                 */
#define MAX_SIZELENGTH          20   /* Maximallaenge Groessen ggf. in Prozent         */
#define MAX_ANZ_EXECUTE_PATHS   10   /* Maximal 10 Pfade mit -Xp                       */
#define ALLOC_SIZE_HIDDENVARS 4096   /* Hidden-Vars-Speicher pro "realloc"             */
#define MAX_SIZE_HIDDENVARS  10240   /* Hiddenvars-Speicher bei Tabellen (kein MALLOC) */


#define eqbpar(a,b) (firstwert(a, HP_BROWSER_LEVEL) && (strcmp(parwert(a, HP_BROWSER_LEVEL), b) == 0))
#define skip_blanks(a) while(*a == ' ' || *a == '\t') (a)++
#define skip_to_blank(a) while(*a && *a != ' ' && *a != '\t') (a)++
#define skip_next_blanks(a) {skip_to_blank(a); skip_blanks(a);}
#define skip_attrib_char(a) if( *a == ATTRIB_CHAR ) (a)++

#define del_cr_lf(a) {if(*(a+strlen(a)-1)=='\n')*(a+strlen(a)-1)='\0';if(*(a+strlen(a)-1)=='\r')*(a+strlen(a)-1)='\0';}

/************************************************/
/* Definitionen aus u2w_test.c                  */
/************************************************/

#define MAX_OP_LEN MAX_ZEILENLAENGE
#define MAXANZWERTE 300
#define MAXANZOPS 90


#ifdef LONGLONG
#define LONGWERT long long
#define LONGFORMAT "%lld"
#define ULONGFORMAT "%llu"
#define ABS(a) llabs(a)
#else
#define LONGWERT long
#define LONGFORMAT "%ld"
#define ULONGFORMAT "%lu"
#define ABS(a) labs(a)
#endif


typedef enum { NONEW, EMPTYW, STRINGW, DOUBLEW, LONGW, BINARYW
#ifdef MAYDBCLIENT
, DB_MYSQL, RES_MYSQL
#endif
#ifdef POSTGRESQLCLIENT
, DB_PGSQL, RES_PGSQL
#endif
 } wtype;

typedef union { double d; LONGWERT l;} wertunion;

typedef struct { wertunion w;
                 char *s;
                 wtype type;
                 unsigned int len;
                 unsigned int binlen;
               } wert;

typedef struct { char *o;
                 char c;
                 int p;
               } operatortype;

typedef struct { char o;
                 char c;
                 int p;
               } unaryoperatortype;


/***************************************************************************************/
/* Speichern der Parameter von POST oder GET                                           */
/***************************************************************************************/
struct werte_struct { wert wert;                   /* Einzelner Wert                   */
                      struct werte_struct *next;   /* Zeiger auf naechsten oder NULL   */
                      };
typedef struct werte_struct wertetype;

struct hpars_struct { char name[MAX_PAR_NAME_LEN];  /* Name des Parameters             */
                      wertetype *werte;             /* Zeiger auf das erste Element    */
                      char *quote;                  /* ggf. Zeiger auf Quoting         */
                      struct hpars_struct *next;    /* nächster Wert mit gleichem Hash */
                      short level;                  /* -1: vom Browser, 0: global      */
                                                    /* >0 lokal pro %include Datei     */
                                                    /* -2 HTTP-Header Elemente         */
                                                    /* -3 automatisch generiert        */
                                                    /*    oder Systemvars mit %set     */
                    };
typedef struct hpars_struct hparstype;

#define HP_SYSTEM_LEVEL      -5
#define HP_HTTP_HEADER_LEVEL -4
#define HP_COOKIE_LEVEL      -3
#define HP_BROWSER_SYS_LEVEL -2
#define HP_BROWSER_LEVEL     -1
#define HP_GLOBAL_LEVEL       0


/***************************************************************************************/
/* Speichern der INIT-Parameter (-Xi -Xd)                                              */
/***************************************************************************************/

struct hipars_struct { char name[MAX_PAR_NAME_LEN]; /* Name des Parameters             */
                       wert wert;                   /* Wert                            */
                       struct hipars_struct *next;  /* nächster Wert mit gleichem Hash */
                     };
typedef struct hipars_struct hiparstype;


/***************************************************************************************/
/* Hashtabelle für U2W-Interpreter                                                     */
/***************************************************************************************/
struct hu2w_struct { char *command;                 /* Kommandostring                  */
                     unsigned short tblid;          /* Tabellentoken                   */
                     unsigned short id;             /* Funktion                        */
                     struct hu2w_struct *next;      /* nächster Wert mit gleichem Hash */
                   };
typedef struct hu2w_struct hu2wtype;

typedef struct { char *command;                     /* Kommandostring                  */
                 int (*do_fkt)(void);               /* Aufzurufende Funktion           */
                 char flags;                        /* Flags                           */
               } u2w_put_command_type;
#define PCFILEFLAG  1  /* u2w_put_command_type.flags | 1: cmd nur bei Dateien erlaubt  */
#define PCU2WFLAG   2  /*                              2: nur bei U2W erlaubt          */

/***************************************************************************************/
typedef enum { NO_POST, POST_READ, POST_WRITE } posttype;
                                      /* NO_POST: kein POST                            */
                                      /* POST_READ: Read auf u2w-Datei reicht          */
                                      /* POST_WRITE: .passwd und .hosts im Upload-Pfad */

typedef enum { NO_METHOD, GET, POST, HEAD, OPTIONS, PUT, TRACE } methodtype;

typedef enum { NO_SEC=false, SYSTEM_SEC, FILE_SEC } sectype;

typedef enum { NOMENU, INMENU, INMENUHEAD, MENURIGHT, SUBITEM, SUBSHELL } menutype;

typedef enum { NOACCESSFILE, NORMAL, FORCE, PATH, GLOBAL,
               GLOBAL_X, FIX, FIX64 } access_file_type;

typedef enum { NONE, BAD, BAD_NONCE, AUTH_ALL_USER, AUTH, AUTH_MD5 } auth_type;

typedef enum { NOSETUSER, STARTSETUSER, USERSETUSER, U2WSETUSER } set_user_type;

typedef enum { USERORIGGROUP, USERGROUPSET, USERALLGROUPS } set_group_type;

#define BUTTONS        END_OK " " END_CLEAR

typedef enum { VZ_NO_CHANGE, VZ_HOME, VZ_USER, VZ_PWD } verzeichnistype;


typedef enum { CSS_LINK, CSS_TABLE, CSS_TABLEHEAD, CSS_TABLEBODY, CSS_HTML, CSS_BODY,
               CSS_FORM, CSS_H1, CSS_H2, CSS_H3, CSS_H4, CSS_H5, CSS_H6,
               CSS_TEXT, CSS_TEXTAREA, CSS_PASSWORD, CSS_CHECKBOX,
               CSS_OPTION, CSS_RADIO, CSS_SELECT, CSS_FILE, CSS_LINKLIST,
               CSS_PCOLOR, CSS_PDATE, CSS_PDATETIME, CSS_PEMAIL, CSS_PMONTH,
               CSS_PNUMBER, CSS_PRANGE, CSS_PSEARCH, CSS_PTEL, CSS_PTIME,
               CSS_PURL, CSS_PWEEK, CSS_BUTTON, CSS_TD, CSS_TH, CSS_TR,
               CSS_RADIOBUTTONTABLE, CSS_CHECKBOXTABLE, CSS_LINKLISTTABLE,
               NUM_CSS_DEF_ELEMENTS                                   /* last element! */
             } cssdefelementtype;

/***************************************************************************************/
/* Definitionen zum Logging                                                            */
/***************************************************************************************/
#ifdef DEBUG
#define MAX_LEN_LOGFILES 1024
#define LOG(l, ...) if( l <= loglevel && ((all_log_flag&1) || is_elem(logfiles, __FILE__)) ) logging(__VA_ARGS__)
#define LOGLOG(a, ...) if( logflag & a ) logging(__VA_ARGS__)
#define binout(a, b, c) {int h;if(0<=(h=open(c, O_WRONLY|O_CREAT|O_TRUNC,00644))){write(h,a,b);close(h);}}
#else  /* #ifdef DEBUG */
#ifdef SDEBUG
#define LOG(l, ...) fprintf(stderr, __VA_ARGS__)
#else  /* #ifdef SDEBUG */
#define LOG(l, ...) ;
#define LOGLOG(a, ...) ;
#define binout(a, b) ;
#endif  /* #else #ifdef U2W_PASSWD */
#endif  /* #else #ifdef DEBUG */

typedef struct { char *command;
                 int (*do_fkt)(int pa,
                               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS]);
                 int (*get_pars_fkt)(char **s, int n,
                                     char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                                     char quoteflags[MAX_ANZ_QUOTE_PARS]);
                 int minpars;
                 int maxpars;
                 char quoteflags[MAX_ANZ_QUOTE_PARS];
               } command_prog_type;

typedef struct { char *command;
                 int (*do_fkt)(int pa,
                               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                               char token);
                 int (*get_pars_fkt)(char **s, int n,
                                     char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                                     char quoteflags[MAX_ANZ_QUOTE_PARS]);
                 int minpars;
                 int maxpars;
                 char quoteflags[MAX_ANZ_QUOTE_PARS];
                 char token;
               } command_prog_token_type;

typedef struct { char *command;
                 int (*do_list_fkt)(int pa, int lens[2],
                                    char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
                                    char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]);
                 int (*get_list_pars_fkt)(char **s, int n, int lens[2],
                                    char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
                                    char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                                    char quoteflags[2]);
                 int minpars;
                 int maxpars;
                 char quoteflags[MAX_ANZ_QUOTE_PARS];
               } command_list_prog_type;

typedef struct { char *command;
                 int (*do_fkt)(int optpars, char **out, long n,
                               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                               int quote);
                 int minpars;
                 int maxpars;
                 char quoteflags[MAX_ANZ_QUOTE_PARS];
               } scan_prog_type;

typedef struct { char *command;
                 int (*do_fkt)(int optpars,
                               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                               int quote);
                 int minpars;
                 int maxpars;
                 char quoteflags[MAX_ANZ_QUOTE_PARS];
               } scan_prog_send_type;

typedef struct { char *command;
                 int (*do_fkt)(int *listlen,
                               char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                               int optpars,
                               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS]);
                 int minpars;
                 int maxpars;
                 char quoteflags[MAX_ANZ_QUOTE_PARS];
               } scan_list_prog_type;

typedef struct { char *command;
                 char *html;
                 char *tty;
                 char *numhtml;
               } format_command_type;

#ifdef WITH_MMAP
typedef struct { time_t time_last_read;
                 long long unsigned flushconnectioncounter, lastconnectioncounter;
                 long long unsigned requests, lastrequests;
                 long long unsigned bytessend, lastbytessend;
                 long long unsigned bytesreceived, lastbytesreceived;
                 long long unsigned errors, lasterrors;
	             } counter_type;

#define STAT_CONNECT         1
#define STAT_REQUEST         2
#define STAT_SEND            4
#define STAT_RECEIVED        8
#define STAT_ERROR          16
#define STAT_TIMESTAMP      32
#define STAT_UPTIME         64
#define STAT_ENABLE_FLUSH  128
#define STATDIFF_PS        256
#define STATDIFF_ONLYPS    512
#define STATDIFF_CONNECT  1024
#define STATDIFF_REQUEST  2048
#define STATDIFF_SEND     4096
#define STATDIFF_RECEIVED 8192
#define STATDIFF_ERROR   16384 
#endif

/* Achtung: Nummern müssen mit dem Array qf_strings[] übereinstimmen - unix2web.c      */
#define QF_NONE      '\0'
#define QF_SHELL     '\1'          /* Zeichen "'" u. "\" quoten -> "\'" u. "\\"        */
#define QF_NOPARWERT '\2'          /* Sonderbedeutung von '$' aufheben                 */
#define QF_OUTFORMAT '\3'          /* Quoten nach Ausgabeformat                        */
#define QF_HTML      '\4'          /* < -> &lt;  > -> &gt;  & -> &amp;                 */
#define QF_NOHTML    '\5'          /* HTML Ausschalten (nur TXT)                       */
#ifdef MAYDBCLIENT
#define QF_MYSQL     '\6'
#endif
#ifdef POSTGRESQLCLIENT
#define QF_PGSQL     '\7'
#endif
#ifdef SQLITE3
#define QF_SQLITE3   '\10'
#endif
#define QF_STRING    '\11'         /* für Strings, entspricht QF_NONE                  */

#define P1   1
#define P2   2
#define P3   4
#define P4   8
#define P5  16
#define P6  32
#define P7  64
#define P8 128
#define P9 256
#define PA 512

/***************************************************************************************/
#ifdef HAS_PAM

#include <security/pam_appl.h>

typedef struct {char *name; char *pwd;} pamuserstruct;
#endif  /* HAS_PAM */
/***************************************************************************************/

#include "u2w_token.h"
#include "u2w_http.h"
#include "u2w_u2w.h"

#include "unix2web_dec.h"
#include "unix2web_var.h"
