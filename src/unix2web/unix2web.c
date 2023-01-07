/********************************************************************/
/* File: unix2webd.c                                                */
/* Ein einfacher HTTP-Server, der es erlaubt, die Ausgaben diverser */
/* Skripte als WEB-Seiten zu generieren.                            */
/* timestamp: 2017-02-12 14:49:25                                   */
/********************************************************************/

#include "u2w.h"
#include "unix2web_pars.h"

/* Anfang globale Variablen */

FILE *akt_ptr;                       /* aktueller ptr                                  */
short include_counter = 0;

char *clientgetpath;                 /* angeforderter Pfad                             */
char *clientgetfile;                 /* Angeforderte Datei ohne Pfad                   */
short headflag;                      /* auf 1, wenn Methode HEAD                       */
char *resultpage = "";               /* Ergebnisseite für Parameterseiten              */
char *parameter_ext = "";            /* ggf. extension für <form>                      */
char *rechtspage = "";               /* Ergebnisseite Rechts für Parameterseiten       */
char *obenpage = "";                 /* Ergebnisseite Oben für Parameterseiten         */
char *errorpage = NULL;              /* ggf. Seite für Fehlermeldungen                 */
short error_redirect_flag = false;   /* Wenn 1, dann Redirect auf Fehlerseite          */

short kzaehler;                      /* zaehlt oeffnende (++) und                      */
                                     /* schliessende (--) < >       (u3w)              */

short last_char_sended;              /* wird auf letztes Zeichen bei direkt sendenden  */
                                     /* Prozessen gesetzt (%mysqlread und #)           */
                                     /* '\1' entspricht '</td>'                        */

FILE *file_ptrs[MAX_ANZ_OPEN_FILES];
                                     /* Filehandles für File-Operationen               */

char i2w_include_path[MAX_PATH_LEN] = { '\0' };  /* Include-Pfad für *.i2w Dateien,    */
                                     /* kann mit -i <pfad> gesetzt werden              */

menutype menu = NOMENU;              /* Position auf Menueseite                        */
char *headframe = NULL;              /* ggf. Headframe                                 */
short noframeborders = 0;            /* OR - 1: No-Head, 2: NO-Left                    */
char headsize[MAX_SIZELENGTH] = {'\0'};  /* Hoehe HeadFrame                            */

#ifdef WEBSERVER

char *methodstr = NULL;              /* HTTP-Methode bei -Xx allscript.??w             */
char *putdata = NULL;                /* Pfad zu PUT-Daten bei -Xx allscript.??w        */
short term_flag = 0;                 /* Auf true, wenn SIGTERM erhalten                */
char *servicestartjob = NULL;        /* Job to execute on service start                */
char *servicestopjob = NULL;         /* Job to execute on service stop                 */
char *executeall = NULL;             /* *.??w script for all requests                  */
char *executepath[MAX_ANZ_EXECUTE_PATHS];  /* relativer Pfad                           */
char *executex2w[MAX_ANZ_EXECUTE_PATHS];   /* *.??w script                             */
short num_execute_paths = 0;         /* Anzahl in executepath / executex2w             */
char *inifile = NULL;                /* ini-file nach -Xi <file>                       */
char *http_accept_language;          /* http_accept_language des Browsers              */

unsigned long max_post_content_size = MAX_POST_CONTENT_SIZE;
                                     /* Maximale Größe des Content Blocks bei POST     */
unsigned long long max_post_filesize = 0; /* max Groesse der Post Dateien, 0: unlimited*/
char upload_post_path[MAX_PATH_LEN] = {'u','p', 'l', 'o', 'a', 'd', '\0'};
                                     /* Upload Path Std-Einstellung                    */
char auto_delete_files[MAX_ANZ_AUTO_DELETE_FILES][MAX_PATH_LEN]; /* Pfade der durch    */
                                     /* POST oder %delonexit() markierten Dateien      */
short anz_auto_delete_files = 0;     /* Anzahl der durch POST gespeicherten Dateien    */
short auto_delete_post_files = true; /* Dateien, die durch POST gespeichert wurden     */
                                     /* werden automatisch wieder gelöscht             */
char *everybody_path = EVERYBODY_PATH;  /* Pfad bei -V ohne Anmeldung                  */
char *restricted_ips = NULL;         /* Wenn angegeben, dann sind nur diese IPs erlaubt*/
char *fix_user = "";                 /* fester Username                                */
char *fix_pwd  = "";                 /* festes Passwort                                */
char *mime = "";                     /* Mime-Type von %mime                            */
char *httpheadlines[MAX_ANZ_HTTPHEADLINES];  /* zusätzliche Zeilen im HTTP-Header      */
short anz_httpheadlines = 0;         /* Anzahl in httpheadlines                        */
char *charset = "";                  /* character-set von %charset                     */
short httpnum = 0;                   /* HTTP-Response-Code                             */
char *httpdesc = NULL;               /* HTTP-Response-Description                      */
char *savename = "";                 /* Datei-Name für Content-Disposition             */
char *content_type = "";             /* Content-Type                                   */
char *content_path = "";             /* DateiPfad Content                              */
char *http_host = NULL;              /* Name des Servers - Http-Content - Host:        */
short home_host_flag = 0;            /* 1: Home wird um host aus HTTP-Header erweitert */
char *http_host_unknown = HTTP_HOST_UNKNOWN;  /* -hH und kein http_host                */
char hostname[MAXHOSTNAMELEN];       /* Hostname, wenn *server nicht gesetzt           */
char myport[6];                      /* Port des Servers                               */
char *listenip = NULL;               /* IP-Adresse für den bind                        */
#ifdef WITH_IPV6
char clientip6[MAX_IP_LEN];          /* IP-Adresse des Clients                         */
char *clientip;
struct in6_addr *clientip6struct;    /* IPV6 Adresse network order                     */
#else
char clientip[MAX_IP_LEN];           /* IP-Adresse des Clients                         */
#endif
struct in_addr *clientipstruct;      /* IPV4 Adresse network order                     */
char *auth_basic = "";               /* Platz fuer den Auth Basic String               */
char *realm=STD_REALM;               /* String im Anmeldefenster                       */
int digestnum = 0;                   /* Digest num                                     */
char servernonce[MAX_LEN_DIGEST_NONCE];  /* Digest nonce                               */
char clientnonce[MAX_LEN_DIGEST_NONCE];  /* Nonce from Client                          */
char response[MD5_DIGEST_LENGTH*2+1];  /* Response vom Browser bei Digest              */
char *uri = "";                      /* bei Digest uri                                 */
char *u2w_charset = NULL;            /* Characterset für http-Header                   */
short http_v;                        /* 1, Anfrage HTTP/1.0, 2, HTTP/1.1, 0 sonst      */
short http_head_flag;                /* 0: noch nichts gesendet                        */
                                     /* 1: HTTP-Header begonnen (Sendepuffer)          */
                                     /* 2: HTTP-Header begonnen mit Content-Length     */
                                     /* 3: HTTP-Header beendet und Chunkflag           */
                                     /* 4: HTTP-Header beendet, kein Chunkflag         */
short buffer_programdata = false;    /* Programmdaten puffern, nicht sofort senden     */
short access_file_flag = false;      /* true, keine .access Datei, kein Zugriff        */

unsigned int timeout_secs = TIMEOUT_SECS;  /* Timeout, Sekunden Wartezeit auf Client   */
unsigned long timeout_usecs = 0;     /* Timeout, Mikrosekunden Wartezeit auf Client    */

#ifndef CYGWIN
char *defuser = NULL;                /* Default User, wenn keiner angegeben            */
#endif
char user[MAX_LEN_AUTHORIZE];        /* Aktueller User                                 */
char *passwd = "";                   /* Aktuelles Passwort                             */

access_file_type hosts_file_mode = NOACCESSFILE;
                                     /* NOACCESSFILE, .hosts nicht beachten            */
                                     /* NORMAL, .hosts beachten, wenn vorhanden        */
                                     /* FORCE, keine .hosts Datei, kein Zugriff        */
                                     /* PATH, .hosts Datei fest für alle Verz.         */
                                     /* GLOBAL, keine .hosts, dann globale Dat.        */
                                     /* GLOBAL_X, zusätzliche .hosts für alle Verz.    */
char hosts_path[MAX_LEN_FILENAME];   /* Pfad der hosts-Datei bei -op <hostspfad>       */

access_file_type password_file_mode = NOACCESSFILE;
                                     /* NOACCESSFILE, .passwd nicht beachten           */
                                     /* NORMAL, USER+PWD aus .password, wenn vorhanden */
                                     /* FORCE, keine .password Datei, kein Zugriff     */
                                     /* PATH, .passwd Datei fest für alle Verz.        */
                                     /* GLOBAL, keine .passwd, dann globale Dat.       */
                                     /* GLOBAL_X, zusätzliche .passwd für alle Verz.   */
                                     /* FIX, fester User und Passwort                  */
                                     /* FIX64, fester User und Passwort Base64         */
char passwd_path[MAX_LEN_FILENAME];  /* Pfad der passwd-Datei bei -sp <pwdpfad>        */

short no_options_flag = false;       /* true, OPTIONS ist nicht erlaubt                */
short no_trace_flag = false;         /* true, TRACE nicht erlaubt                      */


auth_type auth_mode;                 /* NONE: user+pwd nicht angegeben,                */
                                     /* AUTH: user+pwd angegeben, BAD: user+pwd falsch */
short sec_flag = false;              /* true, user+pwd müssen angegeben sein           */

set_user_type set_user_mode = NOSETUSER;  /* NOSETUSER, Prozess wechselt User nicht    */
                                     /* STARTSETUSER, User sofort wechseln             */
                                     /* USERSETUSER, User nach Anmeldung wechseln      */
                                     /* U2WSETUSER, User erst nach öffnen der *.u2w    */
                                     /* setzen und ggf. %setuser auswerten             */
set_group_type set_user_group_flag = USERORIGGROUP;  /* USERORIGGROUP: eigene Gruppe   */
                                                     /* USERGROUPSET: set_user_group   */
                                                     /* USERALLGROUPS: alle Gruppen    */
gid_t set_user_group;                /* groupid bei set_user_group_flag == USERGROUPSET*/

short no_header_flag = 0;            /* Elemente des Headers nicht ausgeben            */
short keepalive_flag = false;        /* true, Keep-Alive vom Browser gewünscht/erlaubt */
short upload_flag = false;           /* true, es werden Uploads mit PUT erlaubt        */
short upload_x2w_flag = false;       /* true, Uploads mit PUT von .?2w Dateien erlaubt */
short upload_noauth_flag = false;    /* true, Uploads ohne Passwort erlaubt            */
posttype upload_post_mode = NO_POST; /* true, es werden Upload mit POST erlaubt        */
short upload_post_noauth_flag = false;/* true, es werden Uploads mit Post ohne PWD erl.*/
short store_client_filename_flag = false;/* true, Dateiname vom Client übernehmen      */
short edit_flag = 0;                 /* 1: ?&edit erl., 2: u2w-Dateien nicht auswerten */
short dirlist_flag = false;          /* true, es ist ein Directory-Listing erlaubt     */
short verzeichnis_mode = VZ_NO_CHANGE;/* VZ_NO_CHANGE: root des Servers wechselt nicht */
                                     /* VZ_HOME: root des Servers ist HOME des users   */
                                     /* VZ_USER: root des Servers ist .../user         */
                                     /* VZ_PWD:  root ist .../<pfad_aus_.passwd>       */
short flushmode = 0;                 /* 0: flush erst, wenn Puffer voll                */
                                     /* 1: flush, direkt vor html-Header               */
                                     /* 2: nach html-Header                            */
char get_home[HOME_LENGTH];          /* Root des Servers für != PUT                    */
char put_home[HOME_LENGTH];          /* Root des Servers für PUT                       */

#ifdef WITH_CGI
char *query_string;                  /* zum Aufbauen des QUERY_STRING fuer cgis        */
#endif

long long unsigned connectioncounter = 0;
#ifdef WITH_MMAP
counter_type *status_counter;        /* Globales Arry mit Statuswerten (mmap)          */
short unsigned status_mode = 0;      /* Umfang der Counter                             */
int parentpid;
#endif

#ifdef HAS_DAEMON
short daemonflag = NOLOG;            /* true, wenn daemonisiert werden soll            */
char *pidfile = NULL;                /* if set, write Process-PID to "pidfile"         */
#endif

#ifdef WITH_HTTPS
int fcntlmode;
#endif

int sh;                              /* Sockethandle auf Datenstrom vom Browser        */

char longlogpath[MAX_PATH_LEN] ={'\0'};  /* Dateiname für logging, "" -> stderr        */
char *longlogformat = NULL;          /* Format der Log-Zeilen bei -lC                  */

#endif  /* #ifdef WEBSERVER */

char *css_defs[NUM_CSS_DEF_ELEMENTS];
char *css_elementtypes[NUM_CSS_DEF_ELEMENTS] = {
  [CSS_LINK]             = "link",
  [CSS_TABLE]            = "table",
  [CSS_TABLEHEAD]        = "tablehead",
  [CSS_TABLEBODY]        = "tablebody",
  [CSS_HTML]             = "html",
  [CSS_BODY]             = "body",
  [CSS_FORM]             = "form",
  [CSS_H1]               = "h1",
  [CSS_H2]               = "h2",
  [CSS_H3]               = "h3",
  [CSS_H4]               = "h4",
  [CSS_H5]               = "h5",
  [CSS_H6]               = "h6",
  [CSS_TEXT]             = "text",
  [CSS_TEXTAREA]         = "textarea",
  [CSS_PASSWORD]         = "password",
  [CSS_CHECKBOX]         = "checkbox",
  [CSS_OPTION]           = "option",
  [CSS_RADIO]            = "radio",
  [CSS_SELECT]           = "select",
  [CSS_FILE]             = "file",
  [CSS_LINKLIST]         = "linklist",
  [CSS_PCOLOR]           = "pcolor",
  [CSS_PDATE]            = "pdate",
  [CSS_PDATETIME]        = "pdatetime",
  [CSS_PEMAIL]           = "pemail",
  [CSS_PMONTH]           = "pmonth",
  [CSS_PNUMBER]          = "pnumber",
  [CSS_PRANGE]           = "prange",
  [CSS_PSEARCH]          = "psearch",
  [CSS_PTEL]             = "ptel",
  [CSS_PTIME]            = "ptime",
  [CSS_PURL]             = "purl",
  [CSS_PWEEK]            = "pweek",
  [CSS_BUTTON]           = "button",
  [CSS_TD]               = "td",
  [CSS_TH]               = "th",
  [CSS_TR]               = "tr",
  [CSS_RADIOBUTTONTABLE] = "radiobuttontable", 
  [CSS_CHECKBOXTABLE]    = "checkboxtable",
  [CSS_LINKLISTTABLE]    = "linklisttable"
};

short print_lines_flag = false;      /* Ausgeführte Zeilen anzeigen                    */

#ifdef WITH_GETTEXT
char *u2wtextdomain = NULL;          /* wenn gesetzt, dann wird textdomain und         */
                                     /* bindtextdomain aufgerufen                      */
char *u2wtextdomaindir = U2WTEXTDOMAINPATH;
                                     /* für bindtextdomain                             */
char *dezimalpunkt = ".";            /* Dezimalpunkt                                   */
short utf8flag = true;               /* bei "-C != UTF-8" auf false                    */
#endif

u2w_mode_type u2w_mode;              /* Auswertungsmodus der Zeile                     */
short file_flag = true;              /* false, ptr aus %input, true sonst              */
                                     /*  false: '`' und '#' nicht auswerten.           */
long logflag = 0;                    /* bei != 0 werden einige Infos ausgegeben        */
                                     /* dazu darf PRG nicht daemonisiert sein          */
int tablecols = 0;                   /* Spalten einer Tabelle                          */
int akt_tablecols = 0;               /* aktuelle Spalten einer Tabelle                 */
short table_autofill_cols = 0;       /* bei Tabellen leere Zellen anfügen, wenn        */
                                     /* vorherige Zeilen mehr Spalten enthielten       */
short newlineflag = true;            /* Formularfelder stehen auf neuen Zeilen         */
short par_old_format = false;        /* true: altes Parameterformat für Formulare      */

char logpath[MAX_PATH_LEN] ={'\0'};  /* Dateiname für logging, "" -> stderr            */

short skip_empty_flag_def = false;   /* true: Leere Parameter nicht an Prg.            */
short skip_empty_flag = false;       /* true: Leere Parameter werden nicht an Programme*/
                                     /* übergeben.                                     */

short no_browservars_flag = false;   /* true: Vars vom Browser und CMD nur über %bpar()*/

hparstype *phash[HASH_TABLE_SIZE];   /* Hash Array auf Variablennamen                  */
hiparstype *inihash[HASH_TABLE_SIZE];  /* Hash Array auf Ini-Variablennamen            */

hu2wtype *u2whash[U2W_HASH_TABLE_SIZE];  /* Hash Array auf u2w-Kommandos               */
hu2wtype u2wcmds[U2W_COMMAND_HASH_SIZE];  /* Liste der u2w-cmds für Hash-Zeiger        */
int anz_u2wcmds = 0;

char globalquote[MAX_LEN_QUOTE] = {'\0'};
                                     /* Zeichen, die mit erstem Zeichen gequotet werden*/

#ifdef WITH_HTTPS
short ssl_mode = SSL_MODE_OFF;       /* std. SSL aus                                   */
char *ssl_key_file = SSL_KEY_FILE;   /* SSL-Key-File                                   */
char *ssl_cert_file = SSL_CERT_FILE; /* SSL-Cert-File                                  */
char *ssl_ca_file = NULL;            /* SSL-CA-File                                    */
char *ssl_chain_file = NULL;         /* SSL-Chain-File                                 */
char *ssl_dh_params_file = NULL;     /* File with DH-Params                            */
SSL     *ssl;                        /* Handle zum Lesen/Senden über SSL               */
char *ssl_client_subject = "";       /* Client Subject                                 */
char *ssl_client_issuer = "";        /* Client Issuer                                  */
#endif  /* WITH_HTTPS */

char zeile[MAX_ZEILENLAENGE];        /* temp. stringbuffer                             */

char *qf_strings[] = {
    "",          /* QF_NONE      */
    "\\'",       /* QF_SHELL     */
    "",          /* QF_NOPARWERT */
    "",          /* QF_OUTFORMAT */
    "",          /* QF_HTML      */
    "",          /* QF_NOHTML    */
    "\\'",       /* QF_MYSQL     */
    "\\'",       /* QF_PGSQL     */
    "''",        /* QF_SQLITE3   */
    ""           /* QF_STRING    */
  };

#ifdef DEBUG
char logfiles[MAX_LEN_LOGFILES] = {'\0'};
                                     /* durch ' ' getrennte Liste mit den Logfilenamen */
short all_log_flag = false;          /* true, alle Logs werden ausgegeben              */
short loglevel = 1;                  /* Loglevel                                       */
int linenumber;
int line_number_stack[MAX_ANZ_INCLUDE];
char *curfile_stack[MAX_ANZ_INCLUDE];
#endif  /* #ifdef DEBUG */

time_t starttime;

/* Ende globale Variablen */

#ifndef U2W_PASSWD
/***************************************************************************************/
/* void usage(char *prg, char *msg, char *msgpar)                                      */
/*            char *prg: Name, mit dem Programm aufgerufen wurde                       */
/*            char *msg: zusätzliche Meldung                                           */
/*            char *msgpar: Parameter der zusätzlichen Meldung                         */
/*      usage zeigt Aufrufparameter                                                    */
/***************************************************************************************/
void usage(char *prg, char *msg, char *msgpar)
#ifdef INTERPRETER
{ char *p;
  if( (p = strrchr(prg, '/')) )
    p++;
  else
    p = prg;
  if( msg )
    printf(msg, msgpar);
  printf("usage: %15s [-i path] [-l] [-lE]"
#ifdef DEBUG
                                 " [-ll l]"
#endif
                                         " [-lp path]"
#ifdef MAYDBCLIENT
                  " [-me] [-mp port]"
#ifdef MAYDBRECONNECT
                                   " [-mr secs]"
#endif
#endif
#ifdef SQLITE3
                  " [-ne] [-nd path]"
#endif
#ifdef POSTGRESQLCLIENT
                  " [-qe] [-qp port]"
#endif
                                                    " [-v] [-x] [-B]\n"
                  "                       [-E] [-Xd def] [-Xi path]"
#ifdef WITH_GETTEXT
                                        " [-I td] [-Ip path]"
#endif
                                       " [-?]"
#ifdef DEBUG
                "\n                       [-A[t]] [-T] [-J dat.c] [-M]"
#endif
          " path [par=value ...]\n", p);

  fputs(" -i path  ", stdout);
          fputs(_("include-path for *.i2w files"), stdout);
fputs("\n -l       ", stdout);
          fputs(_("switch logging on"), stdout);
/*************************** noch nicht implementiert
fputs("\n -le      ", stdout);
          fputs(_("write cmd errors into logfile"), stdout);
****************************/
fputs("\n -lE      ", stdout);
          fputs(_("don't show cmd errors in output"), stdout);
#ifdef DEBUG
fputs("\n -ll l    ", stdout);
          fputs(_("set loglevel to l"), stdout);
#endif
fputs("\n -lp path ", stdout);
          fputs(_("write logging into path"), stdout);
#ifdef MARIADBCLIENT
fputs("\n -me      ", stdout);
          fputs(_("write MARIADB-Errors into logfile/stderr"), stdout);
fputs("\n -mp port ", stdout);
          fputs(_("change MARIADB-Port"), stdout);
#endif
#ifdef MYSQLCLIENT
fputs("\n -me      ", stdout);
          fputs(_("write MySQL errors into logfile/stderr"), stdout);
fputs("\n -mp port ", stdout);
          fputs(_("change MySQL port"), stdout);
#endif
#ifdef SQLITE3
fputs("\n -ne      ", stdout);
          fputs(_("write SQLite3 errors into logfile/stderr"), stdout);
fputs("\n -nd path ", stdout);
          fputs(_("set SQLite3 database file"), stdout);
#endif
#ifdef POSTGRESQLCLIENT
fputs("\n -qe      ", stdout);
          fputs(_("write PostgreSQL-Errors into logfile/stderr"), stdout);
fputs("\n -qp port ", stdout);
          fputs(_("change PostgreSQL-Port"), stdout);
#endif
fputs("\n -v       ", stdout);
          fputs(_("show version"), stdout);
fputs("\n -x       ", stdout);
          fputs(_("print executed lines"), stdout);
fputs("\n -B       ", stdout);
          fputs(_("browser/cmd variables only readable by %bpar/%cpar"), stdout);
fputs("\n -E       ", stdout);
          fputs(_("ignore empty pars on calling programs"), stdout);
#ifdef WITH_GETTEXT
fputs("\n -I td    ", stdout);
          fputs(_("set textdomain to td"), stdout);
fputs("\n -Ip path ", stdout);
          fputs(_("set textdomaindir to path"), stdout);
#endif
fputs("\n -Xd def  ", stdout);
          fputs(_("define systemvars (-Xd <varname>=<value>)"), stdout);
fputs("\n -Xi path ", stdout);
          fputs(_("read systemvars from file on startup"), stdout);
fputs("\n -?       ", stdout);
          fputs(_("show this help"), stdout);
#ifdef DEBUG
fputs("\n -A       ", stdout);
          fputs(_("logging of all functions"), stdout);
fputs("\n -At      ", stdout);
          fputs(_("logging of all functions with seconds"), stdout);
fputs("\n -J dat.c ", stdout);
          fputs(_("logging of functions from sourcecode file dat.c"), stdout);
fputs("\n -M       ", stdout);
          fputs(_("verbose logging"), stdout);
fputs("\n -O       ", stdout);
          fputs(_("logging of sended data"), stdout);
fputs("\n -R       ", stdout);
          fputs(_("logging of readed data from commandline"), stdout);
#endif
      fputs("\n", stdout);
}
#else  /* #ifdef INTERPRETER */
{ char *p;
  if( (p = strrchr(prg, '/')) )
    p++;
  else
    p = prg;
  if( msg )
    printf(msg, msgpar);
  printf("usage: %9s [-a] [-b path] [-bb] [-d] [-e path|-er path]\n", p);
  fputs(    "                 [-f] [-fc] [-fD] [-fp path] [-fP] [-fS size] [-g|-gg [-gf path]] [-gn]\n"
            "                 [-h home] [-hH] [-hU name] [-i path] [j] [-k] [-l|-lp path] [-lc] [-lC path] [-lF form] [-lE]\n"
            "                 "
#ifdef MAYDBCLIENT
                              "[-me] [-mp port] "
#ifdef MAYDBRECONNECT
                                              "[-mr secs] "
#endif
#endif
#ifdef SQLITE3
                              "[-ne] [-nd path] "
#endif
#ifdef POSTGRESQLCLIENT
                              "[-qe] "
#endif
                                    "[-o|-op path|-og path] [-of] [-or ips] [-p port] [-r]\n"
            "                 [-s|-sp path|-sg path|-sH path|[[-sU user] [-sP pwd]|[-sB b64]]\n"
            "                 [-sf] [-t sek] "
#ifndef CYGWIN
                                            "[-u[s|u] user] [-ug grp|-uG] "
#endif
                                                      "[-v] [-w] [-x] [-y size] [-B] [-C charset]\n"
#ifdef HAS_DAEMON
          "                 [-D] [-Dp path]\n"
#endif
          "                 [-E] [-F|-Fh] [-G bcklg] [-H] [-I td] [-Ip path] [-L ip] [-N] [-P]\n"
#ifdef WITH_MMAP
          "                 [-Q[cCqQsSrReEtUpPAf]]\n"
#endif
#ifdef WITH_SSL
            "                 [-S] [-Sa path|-SA path] [-Sc path] [-SD path] [-Sf|-SF] [-Sk path]\n"
#endif
            "                 [-V [-Ve path]|-Vh] [-Xd def] [-Xi path] [-Xp par]\n"
            "                 [-Xs path] [-XS path] [-Xx path] [-?]\n"
        " -a       ", stdout);
printf(_("no access if file \"%s\" is missing"), ACCESS_FILE);
fputs("\n -b path  ", stdout);
          fputs(_("set path for binaries"), stdout);
fputs("\n -bb      ", stdout);
          fputs(_("buffer data from binaries"), stdout);
fputs("\n -d       ", stdout);
          fputs(_("show directory contents"), stdout);
fputs("\n -e path  ", stdout);
          fputs(_("path of errorpage"), stdout);
fputs("\n -er path ", stdout);
          fputs(_("path of redirectpage on error"), stdout);
fputs("\n -f       ", stdout);
          fputs(_("allow uploads with POST"), stdout);
fputs("\n -fc      ", stdout);
          fputs(_("on uploads with POST get filename from client"), stdout);
fputs("\n -fD      ", stdout);
          fputs(_("don't delete temporary files after uploads with POST"), stdout);
fputs("\n -fp path ", stdout);
          fputs(_("path for uploads with POST"), stdout);
fputs("\n -fP      ", stdout);
          fputs(_("uploads with POST only to directories with write permission"), stdout);
fputs("\n -fS size ", stdout);
          fputs(_("maximum size of post files in bytes"), stdout);
fputs("\n -g       ", stdout);
          fputs(_("allow uploads with PUT"), stdout);
fputs("\n -gf path ", stdout);
          fputs(_("server root for PUT"), stdout);
fputs("\n -gg      ", stdout);
          fputs(_("allow uploads with PUT without authorization"), stdout);
fputs("\n -gn      ", stdout);
          fputs(_("allow ?&edit"), stdout);
fputs("\n -h home  ", stdout);
          fputs(_("set path home as server root"), stdout);
fputs("\n -hH      ", stdout);
          fputs(_("expand home by host from HTTP header"), stdout);
fputs("\n -hU name ", stdout);
          fputs(_("expand home by name on missing host in HTTP header"), stdout);
fputs("\n -i path  ", stdout);
          fputs(_("Include-path for *.i2w files"), stdout);
fputs("\n -j       ", stdout);
          fputs(_("don't interpret u2w-files"), stdout);
fputs("\n -k       ", stdout);
          fputs(_("allow Keep-Alive"), stdout);
fputs("\n -l       ", stdout);
          fputs(_("switch logging on"), stdout);
fputs("\n -lc      ", stdout);
          fputs(_("log connections"), stdout);
fputs("\n -lC path ", stdout);
          fputs(_("log detailed connectioninfos to file path"), stdout);
fputs("\n -lF form ", stdout);
          fputs(_("format for detailed connectioninfos"), stdout);
/*************************** noch nicht implementiert
fputs("\n -le      ", stdout);
          fputs(_("write cmd errors into logfile"), stdout);
****************************/
fputs("\n -lE      ", stdout);
          fputs(_("don't show cmd errors in output"), stdout);
#ifdef DEBUG
fputs("\n -ll l    ", stdout);
          fputs(_("set loglevel to l"), stdout);
#endif
fputs("\n -lp path ", stdout);
          fputs(_("write logging into path"), stdout);
#ifdef MARIADBCLIENT
fputs("\n -me      ", stdout);
          fputs(_("write MariaDB errors into logfile/stderr"), stdout);
fputs("\n -mp port ", stdout);
          fputs(_("set MariaDB connection port"), stdout);
#ifdef MAYDBRECONNECT
fputs("\n -mr secs ", stdout);
          fputs(_("set MariaDB reconnection timeout"), stdout);
#endif
#endif
#ifdef MYSQLCLIENT
fputs("\n -me      ", stdout);
          fputs(_("write MySQL errors into logfile/stderr"), stdout);
fputs("\n -mp port ", stdout);
          fputs(_("set MySQL connection port"), stdout);
#ifdef MAYDBRECONNECT
fputs("\n -mr secs ", stdout);
          fputs(_("set MySQL reconnection timeout"), stdout);
#endif
#ifdef SQLITE3
fputs("\n -ne      ", stdout);
          fputs(_("write Sqlite3 errors into logfile/stderr"), stdout);
fputs("\n -np path ", stdout);
          fputs(_("set Sqlite3 database file"), stdout);
                              "[-ne] [-nd path] "
#endif
#endif
fputs("\n -o       ", stdout);
          printf(_("accesscontrol with file \"%s\""), HOSTS_FILE);
fputs("\n -of      ", stdout);
printf(_("no access if hosts file \"%s\" is missing"), HOSTS_FILE);
fputs("\n -og path ", stdout);
printf(_("path is hosts file for all directories without \"%s\" file"),
        HOSTS_FILE);
fputs("\n -op path ", stdout);
          fputs(_("path is global hosts file for all directories"), stdout);
fputs("\n -or ips  ", stdout);
          fputs(_("allow only this IP-addresses"), stdout);
fputs("\n -p port  ", stdout);
          fputs(_("set listen port"), stdout);
#ifdef POSTGRESQLCLIENT
fputs("\n -qe      ", stdout);
          fputs(_("write PostgreSQL errors into logfile/stderr"), stdout);
#endif
  fputs(" -r name  ", stdout);
          fputs(_("show this name as realm"), stdout);
  fputs("\n -rd name ", stdout);
          fputs(_("set realm and use digest access authentication"), stdout);
fputs("\n -s       ", stdout);
printf(_("get user and passwords from file \"%s\""), PASSWORD_FILE);
fputs("\n -sf      ", stdout);
printf(_("no access if password file \"%s\" is missing"), PASSWORD_FILE);
fputs("\n -sg path ", stdout);
printf(_("path is global password file for all directories without \"%s\" file"),
        PASSWORD_FILE);
fputs("\n -sh path ", stdout);
          fputs(_("path is global password file including homedirectories"), stdout);
fputs("\n -sp path ", stdout);
          fputs(_("path is global password file for all directories"), stdout);
fputs("\n -sB b64  ", stdout);
          fputs(_("user and password in base64"), stdout);
fputs("\n -sP pwd  ", stdout);
          fputs(_("allow only this password"), stdout);
fputs("\n -sU user ", stdout);
          fputs(_("allow only this user"), stdout);
fputs("\n -t sek   ", stdout);
          fputs(_("set timeout in seconds"), stdout);
fputs("\n -tu msek ", stdout);
          fputs(_("set timeout in microseconds"), stdout);
#ifndef CYGWIN
fputs("\n -u user  ", stdout);
          fputs(_("defaultuser for webserver"), stdout);
fputs("\n -us user ", stdout);
          fputs(_("user for webserver"), stdout);
fputs("\n -uu user ", stdout);
          fputs(_("defaultuser, change user after opening *.u2w file"), stdout);
fputs("\n -ug grp  ", stdout);
          fputs(_("group for webserver"), stdout);
fputs("\n -uG      ", stdout);
          fputs(_("set all groups for webserver"), stdout);
#endif
fputs("\n -v       ", stdout);
          fputs(_("show version"), stdout);
fputs("\n -w       ", stdout);
          fputs(_("allow uploads of .?2w und .?3w files"), stdout);
fputs("\n -x       ", stdout);
          fputs(_("print executed lines"), stdout);
fputs("\n -y size  ", stdout);
          fputs(_("set maximum size of content data on POST requests"), stdout);
fputs("\n -B       ", stdout);
          fputs(_("browser/cmd variables only readable by %bpar/%cpar"), stdout);
fputs("\n -C chset ", stdout);
          fputs(_("set characterset in HTTP-header to chset"), stdout);
#ifdef HAS_DAEMON
fputs("\n -D       ", stdout);
          fputs(_("make daemon"), stdout);
fputs("\n -Dp path ", stdout);
          fputs(_("make daemon and wirte PID of process to path"), stdout);
#endif
fputs("\n -E       ", stdout);
          fputs(_("ignore empty pars on calling programs"), stdout);
fputs("\n -F       ", stdout);
          fputs(_("flush after html-header"), stdout);
fputs("\n -Fh      ", stdout);
          fputs(_("flush after http-header"), stdout);
fputs("\n -G bcklg ", stdout);
          fputs(_("set backlog in listen to bcklg"), stdout);
fputs("\n -H       ", stdout);
          fputs(_("send no HTTP-header"), stdout);
fputs("\n -Hp      ", stdout);
          fputs(_("do'nt send program and version in HTTP-header"), stdout);
#ifdef WITH_GETTEXT
fputs("\n -I td    ", stdout);
          fputs(_("set textdomain to td"), stdout);
fputs("\n -Ip path ", stdout);
          fputs(_("set textdomaindir to path"), stdout);
#endif
fputs("\n -L ip    ", stdout);
          fputs(_("bind only on this IP-address"), stdout);
fputs("\n -N       ", stdout);
          fputs(_("deny methode OPTIONS"), stdout);
fputs("\n -P       ", stdout);
          fputs(_("force authorization"), stdout);
#ifdef WITH_MMAP
fputs("\n -Q       -Q[cCqQsSrReEtTAf], ", stdout);
          fputs(_("allow /?&status[_flush] and select counter"), stdout);
#endif
#ifdef WITH_HTTPS
fputs("\n -S       ", stdout);
          fputs(_("use HTTPS"), stdout);
fputs("\n -Sa path ", stdout);
          fputs(_("file with SSL-CA"), stdout);
fputs("\n -SA path ", stdout);
          fputs(_("file with SSL-Chain"), stdout);
fputs("\n -Sc path ", stdout);
          fputs(_("file with SSL-certificate"), stdout);
fputs("\n -SD path ", stdout);
          fputs(_("file with DH-parameters"), stdout);
fputs("\n -Sf      ", stdout);
          fputs(_("force client certificate"), stdout);
fputs("\n -SF      ", stdout);
          fputs(_("ask for client certificate"), stdout);
fputs("\n -Sk path ", stdout);
          fputs(_("file with SSL-key"), stdout);
#endif  /* WITH_HTTPS */
fputs("\n -V       ", stdout);
          fputs(_("change directory after authorization"), stdout);
fputs("\n -Ve path ", stdout);
          fputs(_("home-directory for anonymous"), stdout);
fputs("\n -Vh      ", stdout);
          fputs(_("change into home-directory of user (forces -P)"), stdout);
fputs("\n -Xd def  ", stdout);
          fputs(_("define systemvars (-Xd <varname>=<value>)"), stdout);
fputs("\n -Xi path ", stdout);
          fputs(_("read systemvars from file on startup"), stdout);
fputs("\n -Xp par  ", stdout);
          fputs(_("paths with execute scripts (-Xp <path>:<script>)"), stdout);
fputs("\n -Xs prg  ", stdout);
          fputs(_("execute prg on programstart"), stdout);
fputs("\n -XS prg  ", stdout);
          fputs(_("execute prg on programstop"), stdout);
fputs("\n -Xx scr  ", stdout);
          fputs(_("execute script scr for all requests"), stdout);
fputs("\n -?       ", stdout);
          fputs(_("show this help"), stdout);
#ifdef DEBUG
fputs("\n -A       ", stdout);
          fputs(_("logging of all functions"), stdout);
fputs("\n -J dat.c ", stdout);
          fputs(_("logging of functions from sourcecode file dat.c"), stdout);
fputs("\n -M       ", stdout);
          fputs(_("verbose logging"), stdout);
fputs("\n -O       ", stdout);
          fputs(_("logging of sended data"), stdout);
fputs("\n -R       ", stdout);
          fputs(_("logging of readed data from browser"), stdout);
#endif
  fputs("\n", stdout);
}
#endif  /* #else #ifdef INTERPRETER */
#endif  /* #ifdef U2W_PASSWD */

/***************************************************************************************/
/* int main(int argc, char **argv)                                                     */
/*     main wertet die Aufrufparameter aus                                             */
/***************************************************************************************/
int main(int argc, char **argv)
{ int options = 0;                             /* zaehlt die Aufrufparameter           */
#ifdef WEBSERVER
  char *set_put_home = NULL;                   /* geänderter Root des Server für PUT   */
  char *set_get_home = STD_GET_HOME;           /* geänderter Root des Servers          */
  int port = PORT;                             /* IP-Port fuer den Server              */
  int backlog = BACKLOG;
  char binpath[HOME_LENGTH];                   /* zusätzlicher Pfad für PATH im Env.   */
  static char path[MAX_PATH_LEN];              /* PATH im Environment aufbauen         */
#ifdef WITH_GETTEXT
  struct lconv *l;

  setlocale(LC_ALL, "");
  textdomain(U2WTEXTDOMAIN);
  bindtextdomain(U2WTEXTDOMAIN, U2WTEXTDOMAINPATH);
  if( NULL != (l = localeconv()) && l->decimal_point[0] )
    dezimalpunkt = l->decimal_point;
#endif

  binpath[0] = '\0';

  if( NULL == getcwd(get_home, HOME_LENGTH) )  /* aktuelles Working Directory          */
  { fprintf(stderr, _("can't get current directory!\n"));
    return 2;
  }
#else   /* #ifdef WEBSERVER */

#ifdef WITH_GETTEXT
  struct lconv *l;

  setlocale(LC_ALL, "");
  textdomain(U2WTEXTDOMAIN);
  bindtextdomain(U2WTEXTDOMAIN, U2WTEXTDOMAINPATH);
  if( NULL != (l = localeconv()) && l->decimal_point[0] )
    dezimalpunkt = l->decimal_point;
#endif

#endif  /* #ifdef WEBSERVER */


  while( argc > options+1
         && argv[options+1][0] == '-' && argv[options+1][1] != '\0' )
  { options++;

    switch( argv[options][1] )
    {
#ifdef WEBSERVER
#ifdef HAS_DAEMON
      case OPT_DAEMON:   daemonflag = true;    /* -D, das Programm daemonisieren       */
                         if( argv[options][2] == OPT_DAEMON_PID )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           pidfile = argv[options];
                         }
                         break;
#endif
#ifdef WITH_MMAP
      case OPT_GET_STATUS:
                         if( strchr(argv[options]+2, OPT_GET_STATUS_CONNECT) )
                           status_mode |= STAT_CONNECT;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_REQUEST) )
                           status_mode |= STAT_REQUEST;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_RECEIVE) )
                           status_mode |= STAT_RECEIVED;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_SEND) )
                           status_mode |= STAT_SEND;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_ERROR) )
                           status_mode |= STAT_ERROR;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_TIMESTAMP) )
                           status_mode |= STAT_TIMESTAMP;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_UPTIME) )
                           status_mode |= STAT_UPTIME;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_CONNECT) )
                           status_mode |= STATDIFF_CONNECT;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_REQUEST) )
                           status_mode |= STATDIFF_REQUEST;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_RECEIVE) )
                           status_mode |= STATDIFF_RECEIVED;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_SEND) )
                           status_mode |= STATDIFF_SEND;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_ERROR) )
                           status_mode |= STATDIFF_ERROR;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_PS) )
                           status_mode |= STATDIFF_PS;
                         if( strchr(argv[options]+2, OPT_GET_STATDIFF_ONLYPS) )
                           status_mode |= STATDIFF_ONLYPS;
                         if( strchr(argv[options]+2, OPT_GET_STATUS_ALL) )
                           status_mode |= STAT_CONNECT|STAT_REQUEST|STAT_SEND
                             |STAT_RECEIVED|STAT_ERROR|STAT_TIMESTAMP|STAT_UPTIME
                             |STATDIFF_PS|STATDIFF_CONNECT|STATDIFF_REQUEST
                             |STATDIFF_SEND|STATDIFF_RECEIVED|STATDIFF_ERROR;
                         if( strchr(argv[options]+2, OPT_GET_STATENABLE_FLUSH) )
                           status_mode |= STAT_ENABLE_FLUSH;
                         if( !status_mode )
                           status_mode = STAT_CONNECT;
                         break;
#endif
      case OPT_ACCESS_FILE:
                         access_file_flag = true; /* -a keine .access Datei, kein      */
                         break;                /* Zugriff auf Verzeichnis              */
      case OPT_ERRORPAGE:
                         if( argv[options][2] == OPT_ERROR_REDIRECT )
                           error_redirect_flag = true;
                         else
                           error_redirect_flag = false;
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         errorpage = argv[options];
                         break;
      case OPT_NO_U2W:   edit_flag = 2;        /* -j, u2w nicht auswerten              */
                         break;
      case OPT_DIRLIST:  dirlist_flag = true;  /* -d, Directory-Listeings sind erlaubt */
                         break;
      case OPT_KEEPALIVE: keepalive_flag = true;
                        break;
      case OPT_HOSTS_FILE:
                         if( argv[options][2] == OPT_HOSTS_RESTRICT  )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           restricted_ips = argv[options];
                           break;
                         }
                         if( argv[options][2] == OPT_HOSTS_FILE_PATH 
                             || argv[options][2] == OPT_HOSTS_GLOB_PATH )
                         { hosts_file_mode = argv[options][2] == OPT_HOSTS_FILE_PATH 
                                             ? PATH : GLOBAL;
                           if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           abs_path(hosts_path, get_home, argv[options],
                                    MAX_LEN_FILENAME);
                         }
                         else if( argv[options][2] == OPT_HOSTS_FILE_FORCE )
                           hosts_file_mode = FORCE;
                         else
                           hosts_file_mode = NORMAL;
                         break;
      case OPT_PASSWD_FILE:
                         if( argv[options][2] == OPT_PASSWD_FILE_PATH
                             || argv[options][2] == OPT_PASSWD_GLOB_PATH
                             || argv[options][2] == OPT_PASSWD_HOME )
                         { password_file_mode = argv[options][2] == OPT_PASSWD_FILE_PATH
                                                ? PATH : GLOBAL;
                           if( argv[options][2] == OPT_PASSWD_HOME )
                             verzeichnis_mode = VZ_PWD;
                           if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           abs_path(passwd_path, get_home, argv[options],
                                    MAX_LEN_FILENAME);
                         }
                         else if( argv[options][2] == OPT_PASSWD_FILE_FORCE )
                           password_file_mode = FORCE;
                         else if( argv[options][2] == OPT_PASSWD_FIX_USER )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           fix_user = argv[options];
                           password_file_mode = FIX;
                         }
                         else if( argv[options][2] == OPT_PASSWD_FIX_PWD )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           fix_pwd = argv[options];
                           password_file_mode = FIX;
                         }
                         else if( argv[options][2] == OPT_PASSWD_FIX_B64 )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           fix_user = argv[options];
                           password_file_mode = FIX64;
                         }
                         else
                           password_file_mode = NORMAL;
                         break;
      case OPT_UPLOAD_PUT:
                         upload_flag = true;   /* -g, Uploads mit PUT erlauben         */
                         if( argv[options][2] == OPT_UPLOAD_PUT )
                           upload_noauth_flag = true; /* -gg, Uploads ohne USER/PWD    */
                         else if( argv[options][2] == OPT_UPLOAD_PUT_PATH )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           set_put_home = argv[options];
                         }
                         else if( argv[options][2] == OPT_UPLOAD_EDIT )
                           edit_flag = 1;        /* -gn, ?&edit erlaubt                   */
                         break;
      case OPT_UPLOAD_X2W:
                         upload_x2w_flag = true;
                         break;
      case OPT_UPLOAD_POST:
                         upload_post_mode = POST_READ; /* -f, Uploads mit POST erlauben*/
                         if( argv[options][2] == OPT_UPLOAD_POST_P )
                           upload_post_mode = POST_WRITE;
                         else if( argv[options][2] == OPT_UPLOAD_POST_PATH )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           if( argv[options][0] == '/' )
                             strcpyn(upload_post_path, argv[options], MAX_PATH_LEN);
                           else
                           { strcpyn(path, get_home, MAX_PATH_LEN);
                             strcatn(path, "/", MAX_PATH_LEN);
                             strcatn(path, argv[options], MAX_PATH_LEN);
                             strcpyn(upload_post_path, path, MAX_PATH_LEN);
                           }
                         }
                         else if( argv[options][2] == OPT_UPLOAD_CLIENTFILENAME )
                           store_client_filename_flag = true;
                         else if( argv[options][2] == OPT_UPLOAD_POST_NODEL )
                           auto_delete_post_files = false;
                         else if( argv[options][2] == OPT_UPLOAD_POST_MAXSIZE )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           max_post_filesize = atoll(argv[options]);
                         }
                         break;
      case OPT_POST_CONTENT_SIZE:
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         max_post_content_size = atoll(argv[options]);
                         break;
      case OPT_NO_OPTIONS:
                         no_options_flag = true;    /* -o, OPTIONS ist nicht erlaubt   */
                         break;
      case OPT_NO_TRACE: no_trace_flag = true;      /* Methode TRACE nicht erlaubt     */
                         break;
      case OPT_CHARSET:  if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         u2w_charset = argv[options];
                         break;
      case OPT_PORT:     if( argc < ++options+1 )   /* -p <port>, auf <port> hoeren    */
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         sscanf(argv[options], "%i", &port);
                         break;
      case OPT_BACKLOG:  if( argc < ++options+1 )   /* -G <numbacklog>                 */
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         sscanf(argv[options], "%i", &backlog);
                         if( backlog <= 0 )
                           backlog = BACKLOG;
                         break;
      case OPT_REALM:    if( argv[options][2] == OPT_DIGEST )
                         { digestnum = 1;
                           keepalive_flag = true;
                         }
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         realm = argv[options];
                         break;
      case OPT_TIMEOUT:                         /* -t <timeout_sek> Sekunden Timeout   */
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         if( argv[options-1][2] == OPT_TIMEOUT_U )
                         { sscanf(argv[options], "%lu", &timeout_usecs);
                           timeout_secs = 0;
                         }
                         else
                           sscanf(argv[options], "%u", &timeout_secs);
                         break;
      case OPT_SEC:      sec_flag = true;      /* -P, bei jeder Seite PWD und PASS     */
		         break;
      case OPT_HEADER:   if( argv[options][2] == OPT_HEADER_PRGVERS )
                           no_header_flag |= NO_HEADER_PRGVERS;
                         else
                           no_header_flag = NO_HEADER;  /* keinen HTTP-Header senden     */
                         break;
      case OPT_BINPATH:  if( argv[options][2] == OPT_BIN_NOFLUSH )
                           buffer_programdata = true;
                         else
                         { if( argc < ++options+1 )    /* -b <path>, <path> erweitern  */
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           if( strlen(argv[options]) > HOME_LENGTH-1 )
                           { fprintf(stderr, _("%s: bin-path too long.\n"), argv[0]);
                             return 4;
                           }
                           strcpyn(binpath, argv[options], HOME_LENGTH);
                         }
                         break;
      case OPT_VERZEICHNIS:
                         if( argv[options][2] == OPT_VERZEICHNIS_HOME )
                         { verzeichnis_mode = VZ_HOME;
                           sec_flag = true;
                         }
                         else
                           verzeichnis_mode = VZ_USER;
                         if( argv[options][2] == OPT_VERZEICHNIS_EVERYB )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           everybody_path = argv[options];
                         }
                         break;
      case OPT_LISTEN_IP:
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         listenip = argv[options];
                         break;
#ifndef CYGWIN
      case OPT_USER:                           /* -u <user>, Server laeuft als <user>  */
                         if( argv[options][2] == OPT_USER_ALL_GROUPS )
                         { set_user_group_flag = USERALLGROUPS;
                           break;
                         }
                         else if( argv[options][2] == OPT_USER_GROUP )
                         { struct group *sgrp;
                           if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           if( (sgrp = getgrnam(argv[options])) )
                             set_user_group = sgrp->gr_gid;
                           else
                           { usage(argv[0], _("Error: option -ug, wrong group name.\n"), argv[options]);
                             return 3;
                           }
                           set_user_group_flag = USERGROUPSET;
                           break;
                         }
                         else if( argv[options][2] == OPT_USER_START )
                           set_user_mode = STARTSETUSER;
                         else if( argv[options][2] == OPT_SET_USER_U2W )
                           set_user_mode = U2WSETUSER;
                         else
                            set_user_mode = USERSETUSER;
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         defuser = argv[options];
                         if( test_user(defuser) )
                         { fprintf(stderr, _("%s: unable to change to user %s.\n"),
                                   argv[0], defuser);
                           return 6;
                         }
                         break;
#endif
      case OPT_HOME:     if( argv[options][2] == OPT_HOME_HOST )  /* -hH               */
                           home_host_flag = 1;
                                               /* -h <home>, <home>: Server root Verz. */
                         else if( argv[options][2] == OPT_HOST_UNKNOWN ) /* -hU        */
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           http_host_unknown = argv[options];
                         }
                         else 
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           if( strlen(argv[options]) > HOME_LENGTH-1 )
                           { usage(argv[0], _("Error: option %s, home-path too long.\n"), argv[options-1]);
                             return 4;
                           }
                           set_get_home = argv[options];
                         }
                         break;
      case OPT_FLUSH_MODE: flushmode = argv[options][2] == OPT_FLUSH_MODE_HTTP ? 1 : 2;
                         break;
#ifdef WITH_HTTPS
      case OPT_SSL:      ssl_mode = ssl_mode | SSL_MODE_ON;
                         if( argv[options][2] == OPT_SSL_CERT_FILE )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           ssl_cert_file = argv[options];
                         }
                         else if( argv[options][2] == OPT_SSL_KEY_FILE )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           ssl_key_file = argv[options];
                         }
                         else if( argv[options][2] == OPT_SSL_CA_FILE )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           ssl_ca_file = argv[options];
                         }
                         else if( argv[options][2] == OPT_SSL_CHAIN_FILE )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           ssl_chain_file = argv[options];
                         }
                         else if( argv[options][2] == OPT_SSL_DH_PARAMS )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           ssl_dh_params_file = argv[options];
                         }
                         else if( argv[options][2] == OPT_SSL_CLIENTCERT_FORCE )
                         { ssl_mode = ssl_mode | SSL_MODE_ASK_CLIENT | SSL_MODE_FORCE_CLIENT;
                         }
                         else if( argv[options][2] == OPT_SSL_CLIENTCERT_ASK )
                         { ssl_mode = ssl_mode | SSL_MODE_ASK_CLIENT;
                         }
                         break;
#endif  /* #ifdef WITH_HTTPS */
#endif  /* #ifdef WEBSERVER */
      case OPT_PRINT_LINES:
                         print_lines_flag = true;
                         break;
#ifdef MAYDBCLIENT
      case OPT_MYSQL:    switch( argv[options][2] )
                         { case OPT_MYSQL_ERRORLOG: mysql_error_log_flag = true;
                                break;
                           case OPT_MYSQL_ERROROUT: mysql_error_out_flag = true;
                                break;
                           case OPT_MYSQL_PORT: mysqlport = atoi(argv[++options]);
                                break;
#ifdef MAYDBRECONNECT
                           case OPT_MYSQL_RECONNECT: if( atoi(argv[++options]) >= 0 )
                                  mysql_reconnect_secs = atoi(argv[options]);
                                break;
#endif
                         }
                         break;
#endif  /* #ifdef MAYDBCLIENT */
#ifdef SQLITE3
      case OPT_SQL3:     switch( argv[options][2] )
                         { case OPT_SQL3_ERRORLOG: sqlite3_error_log_flag = true;
                                break;
                           case OPT_SQL3_ERROROUT: sqlite3_error_out_flag = true;
                                break;
                           case OPT_SQL3_DBPATH: sql3_open(argv[++options], true);
                                break;
                         }
                         break;
#endif  /* #ifdef MAYDBCLIENT */
#ifdef POSTGRESQLCLIENT
      case OPT_PGSQL:    switch( argv[options][2] )
                         { case OPT_PGSQL_ERRORLOG: pgsql_error_log_flag = true;
                                break;
                           case OPT_PGSQL_ERROROUT: pgsql_error_out_flag = true;
                                break;
                           case OPT_PGSQL_PORT: pgsqlport = atoi(argv[++options]);
                                break;
                         }
                         break;
#endif  /* #ifdef POSTGRESQLCLIENT */
      case OPT_INCLUDE:  if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         strcpyn(i2w_include_path, argv[options], MAX_PATH_LEN);
#ifdef DOS
                         if( i2w_include_path[strlen(i2w_include_path)-1] != '\\' )
                           strcatn(i2w_include_path, "\\", MAX_PATH_LEN);
#else
                         if( i2w_include_path[strlen(i2w_include_path)-1] != '/' )
                           strcatn(i2w_include_path, "/", MAX_PATH_LEN);
#endif
                         break;
      case OPT_SKIP_EMPTY:                     /* -E, leere Parameter nicht an Prgs.   */
                         skip_empty_flag = skip_empty_flag_def = true;
                         break;
#ifdef WITH_GETTEXT
      case OPT_TEXTDOMAIN:
                         if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         if( argv[options-1][2] == OPT_TEXTDOMAINDIR )
                           u2wtextdomaindir = argv[options];
                         else
                           u2wtextdomain = argv[options];
                         break;
#endif
      case OPT_NORMLOG:  if( argv[options][2] == OPT_NORMLOG_PATH )
                         { logflag |= NORMLOG;
                           if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
#ifdef WEBSERVER
                           if( argv[options][0] == '/' )
                             strcpyn(logpath, argv[options], MAX_PATH_LEN);
                           else
                           { strcpyn(logpath, get_home, MAX_PATH_LEN);
                             strcatn(logpath, "/", MAX_PATH_LEN);
                             strcatn(logpath, argv[options], MAX_PATH_LEN);
                           }
#else
                           strcpyn(logpath, argv[options], MAX_PATH_LEN);
#endif
                         }
#ifdef WEBSERVER
                         else if( argv[options][2] == OPT_LOGCONNECTIONFORMAT )
                         { if( argc < ++options+1 || !*argv[options] )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           logflag |= LOGCONNECTIONLONG;
                           longlogformat = argv[options];
                           break;
                         }
                         else if( argv[options][2] == OPT_LOGCONNECTIONLONG )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           logflag |= LOGCONNECTIONLONG;
                           if( argv[options][0] == '/' )
                             strcpyn(longlogpath, argv[options], MAX_PATH_LEN);
                           else
                           { strcpyn(longlogpath, get_home, MAX_PATH_LEN);
                             strcatn(longlogpath, "/", MAX_PATH_LEN);
                             strcatn(longlogpath, argv[options], MAX_PATH_LEN);
                           }
                           break;
                         }
#endif
                         else if( argv[options][2] == OPT_LOGCONNECTION )
                           logflag |= LOGCONNECTION;
                         else if( argv[options][2] == OPT_LOGCMDERRORS )
                           logflag |= LOGCMDERRORS;
                         else if( argv[options][2] == OPT_NOCMDERRORS )
                           logflag |= LOGNOCMDERRORS;
#ifdef DEBUG
                         else if( argv[options][2] == OPT_LOGLEVEL )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           sscanf(argv[options], "%hd", &loglevel);
                           break;
                         }
#endif
#ifdef WEBSERVER
                         logflag |= NORMLOG;
#endif
                         break;
#ifdef DEBUG
      case OPT_LONGLOG:
                         logflag |= LONGLOG;
                         break;
      case OPT_LOGREADDATA:
                         logflag |= LOGREADDATA;
                         break;
      case OPT_LOGSENDDATA:
                         logflag |= LOGSENDDATA;
                         break;
      case OPT_LOGALL:   all_log_flag = true;
                         if( argv[options][2] == OPT_LOGALL_TIME )
                           logflag |= LOGALLTIME;
                         break;
      case OPT_LOGFILE:  if( argc < ++options+1 )
                         { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                           return 3;
                         }
                         strcatn(logfiles, " ", MAX_LEN_LOGFILES);
                         strcatn(logfiles, argv[options], MAX_LEN_LOGFILES);
                         break;
#endif
      case OPT_EXTRA:    if( argv[options][2] == OPT_EXTRA_DEF )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           set_initpar(argv[options]);
                         }
                         else if( argv[options][2] == OPT_EXTRA_INIFILE )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
#ifdef WEBSERVER
                           inifile = argv[options];
                           read_inifile();
#else
                           read_inifile(argv[options]);
#endif
                         }
#ifdef WEBSERVER
                         else if( argv[options][2] == OPT_EXTRA_EXECUTE_START )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           servicestartjob = argv[options];
                         }
                         else if( argv[options][2] == OPT_EXTRA_EXECUTE_STOP )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           servicestopjob = argv[options];
                         }
                         else if( argv[options][2] == OPT_EXTRA_EXECUTE_ALL )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           executeall = argv[options];
                         }
                         else if( argv[options][2] == OPT_EXTRA_EXECUTE_PATH )
                         { if( argc < ++options+1 )
                           { usage(argv[0], _("Error: option %s, missing value.\n"), argv[options-1]);
                             return 3;
                           }
                           if( num_execute_paths < MAX_ANZ_EXECUTE_PATHS )
                           { executex2w[num_execute_paths]
                               = executepath[num_execute_paths] = argv[options];
                             while( *executex2w[num_execute_paths]
                                    && *executex2w[num_execute_paths] != ':' )
                               executex2w[num_execute_paths]++;
                             if( *executex2w[num_execute_paths] == ':' )
                               *executex2w[num_execute_paths++]++ = '\0';
                             else
                             { usage(argv[0], _("Error: option -Xp, missing ':' in '%s'.\n"), argv[options]);
                               return 3;
                             }
                           }
                           else
                           { fprintf(stderr, _("Error: too many -Xp par\n"));
                             return 1;
                           }
                         }
#endif
                         break;
      case OPT_NO_BROWSERVARS:
                         no_browservars_flag = true;
                         break;
      case OPT_VERSION:  show_version(argv[0], true);
                         return 0;
      case OPT_HELP:     usage(argv[0], NULL, NULL);
                         return 0;
      default:           usage(argv[0], _("Error: Unknown option: %s.\n"), argv[options]);
                         return 1;
    }
  }

#ifdef WEBSERVER
#ifdef HAS_DAEMON
  if( daemonflag && !logpath[0] )
    logflag &= LOGCONNECTIONLONG;
#endif
#endif

  time(&starttime);

  LOG(90, "main, nach getopts, logflag: %lx\n", logflag);


#ifdef WITH_GETTEXT
  if( u2wtextdomain && *u2wtextdomain )
  { LOG(2, "main, u2wtextdomain: %s, u2wtextdomaindir: %s.\n",
        u2wtextdomain, u2wtextdomaindir);
    bindtextdomain(u2wtextdomain, u2wtextdomaindir);
  }
#endif

#ifdef WEBSERVER
  if( digestnum && password_file_mode != PATH )
  { fputs(_("Error: Option -rd without -sp.\n"), stderr);
    return 1;
  }

#ifdef WITH_GETTEXT
  LOG(40, "main, u2w_charset: %s.\n", u2w_charset?u2w_charset:"(NULL)");
  if( u2w_charset )
  { utf8flag = !strcmp(u2w_charset, "UTF-8");
  }
  else
  { char *p;

    p = getenv("LC_CTYPE");
    LOG(40, "main, LC_CTYPE: %s.\n", p?p:"(NULL)");
    if( !p || !*p )
      p = getenv("LANG");
    if( p && *p )
    { if( strstr(p, ".UTF-8") )
      { u2w_charset = "UTF-8";
        utf8flag = true;
      }
      else
        u2w_charset = "UTF-8";
    }
  }
#endif

  LOG(200, "main, vor gethostname\n");
  LOG(100, "main, flushmode: %d.\n", flushmode);

  gethostname(hostname, MAXHOSTNAMELEN);         /* Servername, wenn kein Host:        */
  snprintf(myport, 6, "%d", port);

  LOG(200, "main, vor verzeichnis_mode\n");

  if( verzeichnis_mode == VZ_HOME
      && (password_file_mode != NOACCESSFILE || set_user_mode != USERSETUSER) )
  { fprintf(stderr, _("-Vh only without -s or -us/-uu!\n"));
    exit(1);
  }

  if( set_put_home != NULL )
    strcpyn(put_home, get_home, HOME_LENGTH);

  if( set_get_home[0] == '/' )          /* neues Home absoluter Pfad, dann uebernehmen */
    strcpy(get_home, set_get_home);
  else
  { strcatn(get_home, "/", HOME_LENGTH);  /* sonst an akutelles Verzeichnis anhaengen  */
    strcatn(get_home, set_get_home, HOME_LENGTH);
  }

  if( set_put_home == NULL )
    strcpyn(put_home, get_home, HOME_LENGTH);
  else if( *set_put_home == '/' )       /* neues Home absoluter Pfad, dann uebernehmen */
    strcpy(put_home, set_put_home);
  else
  { strcatn(put_home, "/", HOME_LENGTH);  /* sonst an akutelles Verzeichnis anhaengen  */
    strcatn(put_home, set_put_home, HOME_LENGTH);
  }

  LOG(200, "main, vor status_mode\n");

#ifdef WITH_MMAP
  if( status_mode > STAT_CONNECT )
  { status_counter = (counter_type *)mmap(NULL, sizeof(counter_type),
                                          PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,
                                          -1, 0);
    if( status_counter == MAP_FAILED )
    { status_mode &= STAT_CONNECT;
      fputs(_("Warning: mmap failed, no status counter available.\n"), stderr);
      status_counter = NULL;
    }
    else
    { memset(status_counter, '\0', sizeof(counter_type));
      if( status_mode >= STATDIFF_CONNECT )
        time(&status_counter->time_last_read);
    }
  }
#endif

  LOG(200, "main, vor path\n");

  strcpy(path, "PATH=.:");
  if( binpath[0] )
  { strcatn(path, binpath, MAX_PATH_LEN);
    strcatn(path, ":", MAX_PATH_LEN);
  }
  strcatn(path, getenv("PATH"), MAX_PATH_LEN);
  putenv(path);

  LOG(200, "main, vor init_u2w_hash\n");

  init_u2w_hash();
  LOG(200, "main, nach init_u2w_hash, logflag: %ld, all_log_flag: %d\n", logflag, all_log_flag);

  unix2web(port, backlog);

  LOG(200, "main, nach unix2web\n");

#ifdef WITH_MMAP
  if( status_mode & (STAT_REQUEST|STAT_SEND|STAT_RECEIVED|STAT_ERROR) )
    munmap(status_counter, sizeof(counter_type));
#endif

  return 0;

#else
  if( argc <= options+1 )
  { usage(argv[0], NULL, NULL);
    return 1;
  }
  else
  { init_u2w_hash();
    return unix2web(options, argc, argv);
  }

#endif  /* #ifdef WEBSERVER */
}
