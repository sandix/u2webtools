/******************************************************/
/* u2w_prgpars.h                                      */
/* Headerdatei fuer unix2webd mit Programmparametern  */
/* timestamp: 2017-02-12 14:40:54                     */
/******************************************************/


#define OPT_HELP                '?'
#define OPT_ACCESS_FILE         'a'  /* keine .access Datei, kein Zugriff auf Verzeich.*/
#define OPT_BINPATH             'b'  /* -b <binpath>, pfad in PATH Env aufnehmen       */
#define OPT_BIN_NOFLUSH          'b' /* -bb Daten von Programmen werden gepuffert      */
/***************************************************************************************/
/* Noch nicht implementiert                                                            */
#define OPT_CONFIGFILE          'c'  /* -c <configfile>: Read config from file         */
/***************************************************************************************/
#define OPT_DIRLIST             'd'  /* Directory-Listings sind erlaubt                */
#define OPT_ERRORPAGE           'e'  /* -e <page> Errorpage setzen                     */
#define OPT_ERROR_REDIRECT       'r' /* -er <page> Redirect, wenn Seite nicht lesbar   */
#define OPT_UPLOAD_POST         'f'  /* Uploads mit Post erlauben                      */
#define OPT_UPLOAD_POST_P        'P' /* -fP Uploadpfad mit Rechteprüfung im Pfad       */
#define OPT_UPLOAD_POST_PATH     'p' /* -fp pfad legt Uploadpfad fest                  */
                                     /* Es reicht Leseberechtigung auf POST-Datei      */
#define OPT_UPLOAD_CLIENTFILENAME 'c' /* -fc Dateiname vom Client übernehmen           */
#define OPT_UPLOAD_POST_NODEL     'D' /* -fD Post-Dateien nicht automatisch löschen    */
#define OPT_UPLOAD_POST_MAXSIZE   'S' /* -fS maximale Groesse der Post Datei in Byte   */
#define OPT_UPLOAD_PUT          'g'  /* Upload erlauben -gg ohne Anmeldung             */
#define OPT_UPLOAD_PUT_PATH      'p' /* -gp pfad für Uploads mit PUT                   */
#define OPT_UPLOAD_EDIT          'n' /* -gn, datei?&edit erlauben                      */
#define OPT_HOME                'h'  /* Homeverzeichnis                                */
#define OPT_HOME_HOST            'H' /* -hH Homepfad um host aus HTTP-header erweitern */
#define OPT_HOST_UNKNOWN         'U' /* -hU name: name, wenn host fehlt oder ungültig  */
#define OPT_INCLUDE             'i'  /* Verzeichnis für *.i2w Dateien                  */
#define OPT_NO_U2W              'j'  /* u2w-Dateien werden nicht ausgewertet           */
#define OPT_KEEPALIVE           'k'  /* -k Keepalive erlaubt                           */
#define OPT_NORMLOG             'l'  /* Logging einschalten                            */
#define OPT_LOGCONNECTION        'c' /* -lc connections loggen                         */
#define OPT_LOGCONNECTIONLONG    'C' /* -lC <path> connections loggen                  */
#define OPT_LOGCONNECTIONFORMAT  'F' /* -lF 'formatstring' für -lC                     */
#define OPT_LOGCMDERRORS         'e' /* -le Fehler bei #...# und `...` ins Logfile     */
#define OPT_NOCMDERRORS          'E' /* -lE Fehler bei #...# und `...` nicht ausgeben  */
#define OPT_LOGLEVEL             'l' /* -ll <level> Loglevel ändern                    */
#define OPT_NORMLOG_PATH         'p' /* -lp pfad legt Pfad für logging fest            */
#ifdef MAYDBCLIENT
#define OPT_MYSQL               'm'  /* Mysql Parameter                                */
#define OPT_MYSQL_ERRORLOG       'e' /* -me MySQL Errors loggen (stderr oder Logdatei) */
#define OPT_MYSQL_ERROROUT       'E' /* -mE MySQL Errors ausgeben (HTML oder stdout)   */
#define OPT_MYSQL_PORT           'P' /* -mP <mysqlport>                                */
#define OPT_MYSQL_RECONNECT      'r' /* -mr <secs> Anzahl Sekunden, die versucht wird, */
                                     /* eine neue Verbindung mit der DB aufzubauen     */
/***************************************************************************************/
/* Noch nicht implementiert                                                            */
#define OPT_MYSQL_USER           'u' /* -mu <mysqluser>                                */
#define OPT_MYSQL_PWD            'p' /* -mp <mysqlpwd>                                 */
#define OPT_MYSQL_DB             'd' /* -md <mysqldb>                                  */
#define OPT_MYSQL_SERVER         'h' /* -mh <mysqlhost>                                */
/***************************************************************************************/
#endif
#ifdef SQLITE3
#define OPT_SQL3                'n'  /* Sqlite3 Parameter                              */
#define OPT_SQL3_ERRORLOG        'e' /* -ne Sqlite3 Errors loggen (stderr oder Logfile)*/
#define OPT_SQL3_ERROROUT        'E' /* -nE Sqlite3 Errors ausgeben (HTML oder stdout) */
#define OPT_SQL3_DBPATH          'd' /* -nd <database file>                            */
#endif
#define OPT_HOSTS_FILE          'o'  /* .hosts-Datei für Zugriff beachten              */
#define OPT_HOSTS_FILE_FORCE     'f' /* -of keine .hosts Datei, kein Zugriff auf Verz. */
#define OPT_HOSTS_FILE_PATH      'p' /* -op hostsfile, Pfad der .hosts-Datei           */
#define OPT_HOSTS_GLOB_PATH      'g' /* -og hostsfile, .hosts für Vz ohne .hosts       */
//#define OPT_HOSTS_GLOB_X_PATH    'G' /* -oG hostsfile, zusätzliche .hosts für alle Vz  */
#define OPT_HOSTS_RESTRICT       'r' /* -or "ip1 ip2 ..." erlaubte Hosts               */
#define OPT_PORT                'p'  /* Port                                           */
#ifdef POSTGRESQLCLIENT
#define OPT_PGSQL               'q'  /* PostgreSQL Parameter                           */
#define OPT_PGSQL_ERRORLOG       'e' /* -qe PSQL Errors loggen (stderr oder Logdatei)  */
#define OPT_PGSQL_ERROROUT       'E' /* -mE PSQL Errors ausgeben (HTML oder stdout)    */
#define OPT_PGSQL_PORT           'P' /* -qP <psqlport>                                 */
/***************************************************************************************/
/* Noch nicht implementiert                                                            */
#define OPT_PGSQL_USER           'u' /* -qu <psqluser>                                 */
#define OPT_PGSQL_PWD            'p' /* -qp <psqlpwd>                                  */
#define OPT_PGSQL_DB             'd' /* -qd <psqldb>                                   */
#define OPT_PGSQL_SERVER         'h' /* -qh <psqlhost>                                 */
/***************************************************************************************/
#endif
#define OPT_REALM               'r'  /* -r <realmtext>, Text fürs Anmeldefenster       */
#define OPT_DIGEST               'd' /* -rd <realmtext>, Text + Digest ein             */
#define OPT_PASSWD_FILE         's'  /* .passwd Datei, regelt Zugriff auf Verzeichnis  */
#define OPT_PASSWD_FILE_FORCE    'f' /* -sf, keine .pwd Dat, kein Zugriff auf Verz.    */
#define OPT_PASSWD_FILE_PATH     'p' /* -sp pwdfile, Pfad der .passwd-Datei für alle Vz*/
#define OPT_PASSWD_FILE_PATH_STR "sp" /* -sp als String                                */
#define OPT_PASSWD_HOME          'h' /* -sh pwdfile, .pwd Dat und Homes aus .pwd Dat   */
#define OPT_PASSWD_GLOB_PATH     'g' /* -sg pwdfile, .passwd für Vz ohne .passwd       */
#define OPT_PASSWD_FIX_USER      'U' /* -sU user, User festlegen                       */
#define OPT_PASSWD_FIX_PWD       'P' /* -sP pwd, Passwort festlegen                    */
#define OPT_PASSWD_FIX_B64       'B' /* -sB user:pwd im Base 64 Format                 */
#define OPT_TIMEOUT             't'  /* -t <timeout_sekunden> Sekunden bis Timeout     */
#define OPT_TIMEOUT_U            'u' /* -tu mysec Mikrosekunden bis Timeout            */
#define OPT_USER                'u'  /* User bei Zugriff ohne Anmeldung                */
#define OPT_USER_START           's' /* -us <user> Set User vor daemonisieren          */
#define OPT_SET_USER_U2W         'u' /* -uu <defuser> User erst nach öffnen            */
                                     /*               der *.u2w wechseln               */
#define OPT_USER_GROUP           'g' /* -ug <group> set group                          */
#define OPT_USER_ALL_GROUPS      'G' /* -uG alle Gruppen setzen                        */
#define OPT_VERSION             'v'  /* Version anzeigen                               */
#define OPT_UPLOAD_X2W          'w'  /* Upload von .?2w Dateien erlauben               */
#define OPT_PRINT_LINES         'x'  /* Ausgeführte Zeilen anzeigen                    */
#define OPT_POST_CONTENT_SIZE   'y'  /* Post-Cotent Size ändern                        */
                                     /*   def ist MAX_POST_CONTENT_SIZE 204800         */
#define OPT_LOGALL              'A'  /****** Nur im DEBUG-Mode   ***********************/
#define OPT_LOGALL_TIME          't' /****** Nur DEBUG: -At mit Sekunden               */
#define OPT_NO_BROWSERVARS      'B'  /* -B Browser/CMD-Vars können nur mit %bpar       */
                                     /*    gelesen werden.                             */
#define OPT_CHARSET             'C'  /* -C <u2w-charset> Characterset setzen           */
#define OPT_DAEMON              'D'  /* als Daemon starten                             */
#define OPT_DAEMON_PID           'p' /* -Dp <path> write Daemon pid to <path>          */
#define OPT_SKIP_EMPTY          'E'  /* Leere Parameter nicht an Programme übergeben   */
#define OPT_FLUSH_MODE          'F'  /* Flush-Mode einstellen                          */
#define OPT_FLUSH_MODE_HTTP      'h' /* -Fh flush nach http- aber vor html-Header      */
                                     /* -F flush nach Beginn html-Header               */
#define OPT_BACKLOG             'G'  /* Num Backlog in listen                          */
#define OPT_HEADER              'H'  /* keinen HTTP-Header senden                      */
#define OPT_HEADER_PRGVERS       'p' /* Programm und Version nicht im Header senden    */
#define OPT_TEXTDOMAIN          'I'  /* -I <domain>, Internationalisierung: Textdomain */
#define OPT_TEXTDOMAINDIR        'p' /* -Ip <pfad>, std: /usr/share/locale             */
#define OPT_LOGFILE             'J'  /****** Nur im DEBUG-Mode   ***********************/
                                     /* -Jt + time siehe -At                           */
#define OPT_LISTEN_IP           'L'  /* -L <IP-Adresse>, die Listen-IP-Adresse         */
#define OPT_LONGLOG             'M'  /****** Nur im DEBUG-Mode   ***********************/
#define OPT_NO_OPTIONS          'N'  /* Methode OPTIONS nicht erlaubt                  */
#define OPT_LOGSENDDATA         'O'  /****** Nur im DEBUG-Mode   ***********************/
#define OPT_SEC                 'P'  /* immer Anmeldung anfordern                      */
#define OPT_GET_STATUS          'Q'  /* enable Status-Page /?&status                   */
                                     /* -Q[cCqQsSrReEtTAf]                             */
#define OPT_GET_STATUS_CONNECT   'c' /* -Qc: count connections (default)               */
#define OPT_GET_STATDIFF_CONNECT 'C' /* -QC: count connections (default)               */
#define OPT_GET_STATUS_REQUEST   'q' /* -Qq: count requests                            */
#define OPT_GET_STATDIFF_REQUEST 'Q' /* -QQ: count requests                            */
#define OPT_GET_STATUS_SEND      's' /* -Qs: count Bytes send                          */
#define OPT_GET_STATDIFF_SEND    'S' /* -QS: count Bytes send                          */
#define OPT_GET_STATUS_RECEIVE   'r' /* -Qr: count Bytes received                      */
#define OPT_GET_STATDIFF_RECEIVE 'R' /* -QR: count Bytes received                      */
#define OPT_GET_STATUS_ERROR     'e' /* -Qe: count errors                              */
#define OPT_GET_STATDIFF_ERROR   'E' /* -QE: count errors                              */
#define OPT_GET_STATUS_TIMESTAMP 't' /* -Qt: current timestamp                         */
#define OPT_GET_STATUS_UPTIME    'U' /* -QU: seconds uptime [after flush]              */
#define OPT_GET_STATDIFF_PS      'p' /* -Qp: stat diff with ratio per second           */
#define OPT_GET_STATDIFF_ONLYPS  'P' /* -QP: stat diff only ratio per second           */
#define OPT_GET_STATUS_ALL       'A' /* -QA: same as -QcCqQsSrReEt                     */
#define OPT_GET_STATENABLE_FLUSH 'f' /* -Qf: enable /?&status_flush                    */
#define OPT_LOGREADDATA         'R'  /****** Nur im DEBUG-Mode   ***********************/
#define OPT_SSL                 'S'  /* SSL einschalten                                */
#define OPT_SSL_KEY_FILE         'k' /* SSL-Key-File                                   */
#define OPT_SSL_CERT_FILE        'c' /* SSL-Cert-File                                  */
#define OPT_SSL_CA_FILE          'a' /* SSL-CA-File                                    */
#define OPT_SSL_CHAIN_FILE       'A' /* SSL-Chain-File                                 */
#define OPT_SSL_CLIENTCERT_FORCE 'f' /* Force Client Certificate                       */
#define OPT_SSL_CLIENTCERT_ASK   'F' /* ask for Client Certificate                     */
#define OPT_SSL_DH_PARAMS        'D' /* Read DH-Params from file                       */
#define OPT_NO_TRACE            'T'  /* Methode TRACE nicht erlaubt                    */
#define OPT_VERZEICHNIS         'V'  /* automatisch ins Verzeichnis des Users wechseln */
#define OPT_VERZEICHNIS_HOME     'h' /* -Vh, ins HOME Verz. des Users wechseln         */
#define OPT_VERZEICHNIS_EVERYB   'e' /* -Ve pfad, Verzeichnis für -V ohne Anmeldung    */
                                     /* Std. ist everybody                             */
#define OPT_EXTRA               'X'
#define OPT_EXTRA_DEF            'd'  /* -Xd <varname>=<value> Systempars setzen       */
#define OPT_EXTRA_INIFILE        'i' /* -Xi <path.i2w> zum setzen globaler Systemvars  */
                                     /* mit %set('<varname>', '<value>')               */
#define OPT_EXTRA_EXECUTE_PATH   'p' /* -Xp "<relpath>:<*.??w-path>"                   */
                                     /* execute <*.??w-path> for <relpath>             */
#define OPT_EXTRA_EXECUTE_START  's' /* -Xs <path>: execute <path> on Service start    */
#define OPT_EXTRA_EXECUTE_STOP   'S' /* -XS <path>: execute <path> on Service stop     */
#define OPT_EXTRA_EXECUTE_ALL    'x' /* -Xx <path>: execute *.??w Script for all       */
                                     /*   requests. Set %method to HTTP-Method         */
