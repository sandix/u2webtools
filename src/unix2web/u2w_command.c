
/*************************************************/
/* File: u2w_command.c                           */
/* Funktionen zum Auswerten von %funktionen      */
/* timestamp: 2017-02-12 21:10:52                */
/*************************************************/


#include "u2w.h"


#define P0      0
#define P12     P1|P2
#define P1_3    P1|P2|P3
#define P1_4    P1|P2|P3|P4
#define P1_5    P1|P2|P3|P4|P5
#define P1_6    P1|P2|P3|P4|P5|P6
#define P1_9    P1|P2|P3|P4|P5|P6|P7|P8|P9
#define P13     P1|P3
#define P14     P1|P4

// Formularfelder
#define P189A      P1|P8|P9|PA
#define P3589A     P3|P5|P8|P9|PA
#define P123489A   P1|P2|P3|P4|P8|P9|PA
#define P13459     P1|P3|P4|P5|P9
#define P13589A    P1|P3|P5|P8|P9|PA
#define P12389A    P1|P2|P3|P8|P9|PA
#define P123789A   P1|P2|P3|P7|P8|P9|PA
#define P1234789A  P1|P2|P3|P4|P7|P8|P9|PA
#define P1234579A  P1|P2|P3|P4|P5|P7|P9|PA
#define P12345789A P1|P2|P3|P4|P5|P7|P8|P9|PA
#define P12356789A P1|P2|P3|P5|P6|P7|P8|P9|PA
#define P123589A   P1|P2|P3|P5|P8|P9|PA
#define P89A       P8|P9|PA


#define D60(c, f, p, p1, p2, t) { c, f, p, p1, p2, {QF_NONE, QF_NONE, QF_NONE}, t}
#define D63(c, f, p, p1, p2, q1, q2, q3, t) { c, f, p, p1, p2, {q1, q2, q3}, t}
#define D50(c, f, p, p1, p2) { c, f, p, p1, p2, {QF_NONE, QF_NONE, QF_NONE}}
#define D53(c, f, p, p1, p2, q1, q2, q3) { c, f, p, p1, p2, {q1, q2, q3}}
#define D40(c, f, p1, p2) { c, f, p1, p2, {QF_NONE, QF_NONE, QF_NONE}}
#define D43(c, f, p1, p2, q1, q2, q3) { c, f, p1, p2, {q1, q2, q3}}

/***************************************************************************************/
/* Parameterzuordnung                                                                  */
/*              %p:<n>:<m>:<ro|d>:<title>:<class> <name> <options>, <def>; <text>      */
/*                  5   6     7     8        9      1        4        2      3         */
/***************************************************************************************/


/* Anfang globale Variablen */

/* command_list_progs sind alle Funktionen, die einen Wahrheitswert liefern können
   und listen beinhaletn */
/* T_COMMAND_LIST_PROGS */
command_list_prog_type command_list_progs[] =
{
  D50(LIST        , u2w_list    , get_listpars, P12, P12),
  D50(LISTA       , u2w_lista   , get_listpars, P12, P12),
  D50(LISTI       , u2w_listi   , get_listpars, P12, P12),
  D50(LISTAI      , u2w_listai  , get_listpars, P12, P12),
  D50(LISTF       , u2w_listf   , get_listpars, P12, P12),
  D50(LISTAF      , u2w_listaf  , get_listpars, P12, P12),
  D50(LISTS       , u2w_lists   , get_listpars, P12, P12),
  D50(LISTAS      , u2w_listas  , get_listpars, P12, P12),
#ifdef WITH_MALLOC
  D50(LLIST       , u2w_llist   , get_listpars, P12, P12),
  D50(LLISTA      , u2w_llista  , get_listpars, P12, P12),
  D50(LLISTI      , u2w_llisti  , get_listpars, P12, P12),
  D50(LLISTAI     , u2w_llistai , get_listpars, P12, P12),
  D50(LLISTF      , u2w_llistf  , get_listpars, P12, P12),
  D50(LLISTAF     , u2w_llistaf , get_listpars, P12, P12),
  D50(LLISTS      , u2w_llists  , get_listpars, P12, P12),
  D50(LLISTAS     , u2w_llistas , get_listpars, P12, P12),
#endif
  D50(""          , NULL        , NULL        , 0  , 0),
};

/* command_progs sind alle Funktionen, die einen Wahrheitswert liefern können */
/* nach der Funktion wird nicht weiter ausgewertet!                           */
/* T_COMMAND_PROGS */
command_prog_type command_progs[] =
{
  D50(LET         , u2w_let      , get_letpars , P12, P12),
  D50(LET_PLUS    , u2w_let_plus , get_letpars , P12, P12),
  D50(LETI        , u2w_leti     , get_letpars , P12, P12),
  D50(LETF        , u2w_letf     , get_letpars , P12, P12),
  D50(LETS        , u2w_lets     , get_letpars , P12, P12),
  D50(LET_PLUSI   , u2w_let_plusi, get_letpars , P12, P12),
  D50(LET_PLUSF   , u2w_let_plusf, get_letpars , P12, P12),
  D50(LET_PLUSS   , u2w_let_pluss, get_letpars , P12, P12),
  D50(LLET        , u2w_llet     , get_letpars , P12, P12),
  D50(LLET_PLUS   , u2w_llet_plus, get_letpars , P12, P12),
  D50(LLETI       , u2w_lleti    , get_letpars , P12, P12),
  D50(LLETF       , u2w_lletf    , get_letpars , P12, P12),
  D50(LLETS       , u2w_llets    , get_letpars , P12, P12),
  D50(LLET_PLUSI  , u2w_llet_plusi,get_letpars , P12, P12),
  D50(LLET_PLUSF  , u2w_llet_plusf,get_letpars , P12, P12),
  D50(LLET_PLUSS  , u2w_llet_pluss,get_letpars , P12, P12),
  D50(EVAL        , u2w_eval     , get_evalpars, P1,  P1),
  D50(ISSET       , u2w_isset    , get_pars    , P1,  P1),
#ifdef WITH_REGEX
  D50(ISREGEXP    , u2w_isregexp , get_pars    , P1,  P1),
#endif
  D50(SETQUOTE    , u2w_setquote , get_pars    , P1,  P12),
  D53(STORE       , u2w_store    , get_pars    , P12, P12,
      QF_NOPARWERT,QF_NONE,QF_NONE),
  D53(BSTORE      , u2w_bstore   , get_pars    , P12, P12,
      QF_NOPARWERT,QF_NONE,QF_NONE),
  D50(DELETEFILE  , u2w_delete   , get_pars    , P1,  P1),
  D50(PRINT       , u2w_print    , get_pars    , P1,  P12),
  D50(WRITE       , u2w_write    , get_pars    , P1,  P12),
  D50(CLOSE       , u2w_close    , get_pars    , P1,  P1),
  D50(RENAME      , u2w_rename   , get_pars    , P12, P12),
  D50(ERROROUT    , u2w_errorout , get_pars    , P1,  P1),
  D50(FILE_EOF    , u2w_eof      , get_pars    , P1,  P1),
  D50(SETENV      , u2w_setenv   , get_pars    , P1,  P12),
  D50(SET         , u2w_set      , get_pars    , P1,  P12),
  D50(SLEEP       , u2w_sleep    , get_pars    , P1,  P1),
#ifndef OLDUNIX
  D50(USLEEP      , u2w_usleep   , get_pars    , P1,  P1),
#endif
  D50(PARTRIM     , u2w_partrim  , get_pars    , P1,  P12),
  D50(PARLTRIM    , u2w_parltrim , get_pars    , P1,  P12),
  D50(PARRTRIM    , u2w_parrtrim , get_pars    , P1,  P12),
  D50(GPARTRIM    , u2w_gpartrim , get_pars    , P1,  P12),
  D50(GPARLTRIM   , u2w_gparltrim, get_pars    , P1,  P12),
  D50(GPARRTRIM   , u2w_gparrtrim, get_pars    , P1,  P12),
  D50(BPARTRIM    , u2w_bpartrim,  get_pars    , P1,  P12),
  D50(BPARLTRIM   , u2w_bparltrim, get_pars    , P1,  P12),
  D50(BPARRTRIM   , u2w_bparrtrim, get_pars    , P1,  P12),
  D50(CPARTRIM    , u2w_cpartrim,  get_pars    , P1,  P12),
  D50(CPARLTRIM   , u2w_cparltrim, get_pars    , P1,  P12),
  D50(CPARRTRIM   , u2w_cparrtrim, get_pars    , P1,  P12),
  D50(BGLOBAL     , u2w_bglobal,   get_pars    , P1,  P1_3),
  D50(BPARDEF     , u2w_bpardef,   get_pars    , P12, P1_4),
#ifdef WITH_GETTEXT
  D50(SETLANG     , u2w_setlang  , get_pars    , P1,  P1),
  D50(BINDTEXTDOMAIN, u2w_bindtextdomain, get_pars, P1, P12),
#endif
  D50(LOGGING     , u2w_logging  , get_pars    , P1,  P1_9),
#ifdef WEBSERVER
  D50(CHECK_CRED  , u2w_check_cred, get_pars   , P12, P12),
  D50(TIMEOUT     , u2w_timeout  , get_pars    , P1,  P1),
  D50(CSS         , u2w_css      , get_pars    , P1, P12),
  D50(DELONEXIT   , u2w_delonexit, get_pars    , P1,  P1),
#endif
#ifdef MAYDBCLIENT
  D50(MYSQLCONNECT, u2w_mysql_connect , get_pars    , P1_4,  P1_5),
  D53(MYSQLQUERY  , u2w_mysql_query   , get_pars    , P1,    P12, QF_MYSQL, QF_NONE, QF_NONE),
  D53(MYSQLWRITE  , u2w_mysql_write   , get_pars    , P1,    P1,  QF_MYSQL, QF_NONE, QF_NONE),
  D53(MYSQLTEST   , u2w_mysql_test    , get_pars    , P1,    P1,  QF_MYSQL, QF_NONE, QF_NONE),
  D50(MYSQLGETLINE, u2w_mysql_get_line, get_pars    , 0,     P1),
  D50(MYSQLPORT   , u2w_mysql_port    , get_pars    , P1,    P1),
  D50(MYSQLISVALUE, u2w_mysql_isvalue , get_pars    , 0,     0),
  D50(MYSQLSTORE  , u2w_mysql_store   , get_pars    , P1_4,  P1_5),
  D50(MYSQLSTOREV , u2w_mysql_storev  , get_pars    , P12,   P12),
  D50(MYSQLOUT    , u2w_mysql_out     , get_pars    , P12,   P1_4),
#endif
#ifdef POSTGRESQLCLIENT
  D50(PGSQLCONNECT, u2w_pgsql_connect , get_pars    , P1_4,  P1_4),
  D53(PGSQLQUERY  , u2w_pgsql_query   , get_pars    , P1,    P12, QF_PGSQL, QF_NONE, QF_NONE),
  D53(PGSQLWRITE  , u2w_pgsql_write   , get_pars    , P1,    P1,  QF_PGSQL, QF_NONE, QF_NONE),
  D53(PGSQLTEST   , u2w_pgsql_test    , get_pars    , P1,    P1,  QF_PGSQL, QF_NONE, QF_NONE),
  D50(PGSQLGETLINE, u2w_pgsql_get_line, get_pars    , 0,     P1),
  D50(PGSQLPORT   , u2w_pgsql_port    , get_pars    , P1,    P1),
  D50(PGSQLISVALUE, u2w_pgsql_isvalue , get_pars    , 0,     0),
//  D50(PGSQLSTORE  , u2w_pgsql_store   , get_pars    , P1_4, P1_5),
  D50(PGSQLOUT    , u2w_pgsql_out     , get_pars    , P12,   P1_4),
#endif
#ifdef SQLITE3
  D50(SQL3OPEN   , u2w_sql3_open    , get_pars    , P1,    P1),
  D53(SQL3QUERY  , u2w_sql3_query   , get_pars    , P1,    P12, QF_SQLITE3, QF_NONE, QF_NONE),
  D53(SQL3WRITE  , u2w_sql3_write   , get_pars    , P1,    P1,  QF_SQLITE3, QF_NONE, QF_NONE),
  D53(SQL3TEST   , u2w_sql3_test    , get_pars    , P1,    P1,  QF_SQLITE3, QF_NONE, QF_NONE),
  D50(SQL3GETLINE, u2w_sql3_get_line, get_pars    , 0,     P1),
  D50(SQL3ISVALUE, u2w_sql3_isvalue , get_pars    , 0,     0),
  D50(SQL3STORE  , u2w_sql3_store   , get_pars    , P1_4,  P1_5),
  D50(SQL3STOREV , u2w_sql3_storev  , get_pars    , P12,   P12),
  D50(SQL3OUT    , u2w_sql3_out     , get_pars    , P12,   P1_4),
#endif
  D50(""          , NULL         , NULL   , 0,     0),
};


#ifdef WEBSERVER
/* u2w_table_progs, Tabellenfunktionen, die nur bei u2w erlaubt sind */
/* T_U2W_TABLE_PROGS */
command_prog_type u2w_table_progs[] =
{
  D50(COLSPAN     , u2w_colspan, get_pars    , P12, P1_4),
  D50(ROWSPAN     , u2w_rowspan, get_pars    , P12, P1_4),
  D50(""          , NULL       , NULL        ,   0,    0),
};
#endif


#ifdef WEBSERVER
/* u2w_command_progs, Funktionen, die nur bei u2w erlaubt sind */
/* T_U2W_COMMAND_PROGS */
command_prog_token_type u2w_command_progs[] =
{
  D60(VAR         , u2w_par_var  , get_letpars,  P12, P12,    T_VAR),
  D60(VARIF       , u2w_par_varif, get_letpars,  P12, P12,    T_VARIF),
  D60(HIDDENVARS  , u2w_hiddenvars,get_pars, P1, P1_3, T_HIDDENVARS),
  D60(DATALIST        , u2w_par, get_formpars, P14, P14,      T_DATALIST),
  // Ab hier Parameter, bei denen Leerzeichen vor dem Befehl nicht ignoriert werden,
  // wenn eine '(' folgt. Bei weiteren Elementen vor PAR_TEXT muss in "u2w_token.h"
  // #define T_U2W_COMMAND_PROGS_IGNORE_BLANKS <n> geändert werden
  D60(PAR_TEXT        , u2w_par, get_formpars, P1,  P12356789A,T_PAR_TEXT),
  D63(PAR_TEXTAREA    , u2w_par, get_formpars, P1,  P12356789A,
      QF_NONE,QF_NOHTML, QF_NONE, T_PAR_TEXTAREA),
  D60(PAR_PWD         , u2w_par, get_formpars, P1,  P12356789A,T_PAR_PWD),
  D60(PAR_CHECKBOX    , u2w_par, get_formpars, P1,  P123789A,  T_PAR_CHECKBOX),
  D60(PAR_CHECK       , u2w_par, get_formpars, P14, P1234579A, T_PAR_CHECK),
  D60(PAR_CHECKID     , u2w_par, get_formpars, P14, P1234789A, T_PAR_CHECKID),
  D60(PAR_OPTION      , u2w_par, get_formpars, P14, P12345789A, T_PAR_OPTION),
  D60(PAR_SELECT      , u2w_par, get_formpars, P14, P12345789A, T_PAR_SELECT),
  D60(PAR_RADIO_BUTTON, u2w_par, get_formpars, P14, P1234789A, T_PAR_RADIO_BUTTON),
  D60(PAR_RADIO       , u2w_par, get_formpars, P14, P1234579A, T_PAR_RADIO),
  D60(PAR_FILE        , u2w_par, get_formpars, P1,  P123589A,  T_PAR_FILE),
  D60(PAR_LINKLIST    , u2w_par, get_formpars, P14, P13459,   T_PAR_LINKLIST),
  D60(PAR_PARBUTTON   , u2w_par, get_formpars, P1,  P13589A,   T_PAR_PARBUTTON),
  D60(PAR_PBUTTON     , u2w_par, get_formpars, P13, P12389A,   T_PAR_PBUTTON),
  D60(PAR_OKBUTTON    , u2w_par, get_formpars, 0,   P189A,     T_PAR_OKBUTTON),
  D60(PAR_BACKBUTTON  , u2w_par, get_formpars, 0,   P3589A,    T_PAR_BACKBUTTON),
  D60(PAR_CLOSEBUTTON , u2w_par, get_formpars, 0,   P89A,      T_PAR_CLOSEBUTTON),
  D60(PAR_HAUPTMENUE  , u2w_par, get_formpars, P1,  P189A,     T_PAR_MAINMENU),
  D60(PAR_MAINMENU    , u2w_par, get_formpars, P1,  P189A,     T_PAR_MAINMENU),
  D60(PAR_COLOR       , u2w_par, get_formpars, P1,  P123489A,  T_PAR_COLOR),
  D60(PAR_DATE        , u2w_par, get_formpars, P1,  P123489A,  T_PAR_DATE),
  D60(PAR_DATETIMETZ  , u2w_par, get_formpars, P1,  P123489A,  T_PAR_DATETIMETZ),
  D60(PAR_DATETIME    , u2w_par, get_formpars, P1,  P123489A,  T_PAR_DATETIME),
  D60(PAR_EMAIL       , u2w_par, get_formpars, P1,  P123489A,  T_PAR_EMAIL),
  D60(PAR_MONTH       , u2w_par, get_formpars, P1,  P123489A,  T_PAR_MONTH),
  D60(PAR_NUMBER      , u2w_par, get_formpars, P1,  P123489A,  T_PAR_NUMBER),
  D60(PAR_RANGE       , u2w_par, get_formpars, P1,  P123489A,  T_PAR_RANGE),
  D60(PAR_SEARCH      , u2w_par, get_formpars, P1,  P123489A,  T_PAR_SEARCH),
  D60(PAR_TEL         , u2w_par, get_formpars, P1,  P123489A,  T_PAR_TEL),
  D60(PAR_TIME        , u2w_par, get_formpars, P1,  P123489A,  T_PAR_TIME),
  D60(PAR_URL         , u2w_par, get_formpars, P1,  P123489A,  T_PAR_URL),
  D60(PAR_WEEK        , u2w_par, get_formpars, P1,  P123489A,  T_PAR_WEEK),
  D60(PAR_BUTTON  , u2w_button , get_formpars, P1,  P13589A,   T_PAR_BUTTON),
  D60(""              , NULL   , NULL         , 0,  0,        '\0'),
};
#endif

/* scan_progs, Funktionen, die eine Ausgabe als Liste liefern */
/* T_SCAN_LIST_PROGS */
scan_list_prog_type scan_list_progs[] =
{ D40(SPLIT       , do_list_split      , P12, P1_4),
  D40(GETPAR      , do_list_getpar     , P1,  P12),
  D40(GETNAME     , do_list_getname    , P1,  P1_3),
#ifdef MAYDBCLIENT
  D43(MYSQLREAD   , do_mysql_list_read,    P1, P1, QF_MYSQL, QF_NONE, QF_NONE),
  D40(MYSQLGETLINE, do_mysql_list_get_line, 0, P1),
#endif
#ifdef POSTGRESQLCLIENT
  D43(PGSQLREAD   , do_pgsql_list_read,    P1, P1,
      QF_PGSQL, QF_NONE, QF_NONE),
  D40(PGSQLGETLINE, do_pgsql_list_get_line, 0, P1),
#endif
#ifdef SQLITE3
  D43(SQL3READ   , do_sql3_list_read,    P1, P1, QF_SQLITE3, QF_NONE, QF_NONE),
  D40(SQL3GETLINE, do_sql3_list_get_line, 0, P1),
#endif
  D40(""          , NULL               , 0,    0),
};


/* scan_progs, Funktionen, die eine Ausgabe (als String) liefern */
/* T_SCAN_PROGS */
scan_prog_type scan_progs[] =
{
  D40(GETENV        , do_getenv          , P1,   P1),
  D40(GETPID        , do_getpid          , 0,    0),
  D40(GETPPID       , do_getppid         , 0,    0),
  D40(MYPORT        , do_myport          , 0,    0),
  D40(GETHTTPS      , do_https           , 0,    0),
  D40(GETHTTP       , do_https           , 0,    0),
  D40(PRINTUSER     , do_printuser       , 0,    0),
  D40(PRINTPWD      , do_printpwd        , 0,    0),
  D40(CLIENTIP      , do_clientip        , 0,    0),
#ifdef WITH_IPV6
  D40(CLIENTIP6     , do_clientip6       , 0,    0),
#endif
  D40(VAR_OK        , do_var_ok          , 0,    0),
  D40(THISFILE      , do_thisfile        , 0,    0),
  D40(MYHOST        , do_myhost          , 0,    0),
  D40(MYPAGE        , do_mypage          , 0,    0),
  D40(METHOD        , do_method          , 0,    0),
  D40(PUTDATA       , do_putdata         , 0,    0),
  D40(SUBNAME       , do_subname         , 0,    0),
  D40(ALLVARS       , do_allvars         , 0,    0),
  D40(ALLPARS       , do_allpars         , 0,    P1),
  D40(PAR           , do_par             , P1,   P1_3),
  D40(GPAR          , do_gpar            , P1,   P1_3),
  D40(BPAR          , do_bpar            , P1,   P1_3),
  D40(BFPAR         , do_bfpar           , P1,   P1_3),
  D40(CPAR          , do_cpar            , P1,   P1_3),
  D40(SPAR          , do_spar            , P1,   P1),
  D40(IPAR          , do_ipar            , P1,   P1),
  D40(HPAR          , do_hpar            , P1,   P1),
  D40(COUNTPAR      , do_countpar        , P1,   P1),
  D40(GCOUNTPAR     , do_gcountpar       , P1,   P1),
  D40(BCOUNTPAR     , do_bccountpar      , P1,   P1),
  D40(CCOUNTPAR     , do_bccountpar      , P1,   P1),
  D40(GET_CHECKS    , do_par             , P1,   P12),
  D40(SPLIT         , do_split           , P12,  P1_5),
  D40(SUBSTR        , do_substr          , P12,  P1_3),
  D40(STRPOS        , do_strpos          , P12,  P1_3),
  D40(STRCPOS       , do_strcpos         , P12,  P1_3),
  D40(ISODATE       , do_isodate         , P1,   P12),
  D40(DATE          , do_date            , 0,    P12),
  D40(TRIM          , do_trim            , P1,   P12),
  D40(LTRIM         , do_ltrim           , P1,   P12),
  D40(RTRIM         , do_rtrim           , P1,   P12),
  D40(REPLACE       , do_replace         , P1_3, P1_3),
  D40(COUNTCHAR     , do_countchar       , P12,  P12),
  D40(RANDOM        , do_random          , 0,    P12),
  D40(READLINE      , do_readline        , 0,    P1),
  D40(FILEOPEN      , do_fileopen        , P1,   P1),
  D40(FILEAPPEND    , do_fileappend      , P1,   P1),
  D40(FILECREATE    , do_filecreate      , P1,   P1),
  D40(FILESIZE      , do_filesize        , P1,   P1),
  D43(HLINK         , u2w_hlink          , P12,  P1_6,  QF_NONE, QF_HTML, QF_NONE),
  D43(IFRAME        , u2w_iframe         , P1,   P1_5,  QF_NONE, QF_HTML, QF_NONE),
  D40(URL           , u2w_url            , P1,   P12),
  D40(GETPAR        , do_getpar          , P12,  P1),
  D40(QUERYVARS     , u2w_queryvars      , P1,   P1_4),
  D40(CALCSTR       , do_calcstr         , P1,   P1),
#ifdef WITH_GETTEXT
  D40(DECIMALPOINT  , do_decimalpoint    , 0,    0),
#endif
#ifdef WEBSERVER
  D40(VAR_CONTENTTYPE, do_var_contenttype, 0,    0),
  D40(VAR_CONTENT   , do_var_content     , 0,    0),
  D40(REFERER       , do_referer         , 0,    0),
  D40(HTTP_ACCEPT_LANGUAGE,do_http_accept_language,0,0),
  D40(PREFLANG      , do_preflang        , 0,    P1),
  D40(USERAGENT     , do_useragent       , 0,    P1),
  D40(USERAGENTTYPE , do_uatype          , 0,    P1),
#endif
#ifdef WITH_HTTPS
  D40(PRINT_SSLSUBJECT, do_ssl_subject  , 0,   0),
  D40(PRINT_SSLISSUER, do_ssl_issuer    , 0,   0),
#endif  /* WITH_HTTPS */
#ifdef MAYDBCLIENT
  D40(MYSQLREADWRITELINE, do_mysql_readwriteline, 0, P12),
  D40(MYSQLNUMFIELDS, do_mysql_num_fields, 0, 0),
  D40(MYSQLID       , do_mysql_id        , 0,    0),
  D40(MYSQLNUMROWS  , do_mysql_num_rows  , 0,    P1),
  D43(MYSQLINS      , do_mysql_ins       , P12,  P12,  QF_NONE, QF_MYSQL, QF_NONE),
  D40(MYSQLSTOREINS , do_mysql_store_ins , P1_3, P1_3),
  D40(MYSQLENUMS    , do_mysql_enums     , P12,  P12),
  D40(MYSQLSETS     , do_mysql_enums     , P12,  P12),
#endif
#ifdef POSTGRESQLCLIENT
  D40(PGSQLREADWRITELINE, do_pgsql_readwriteline, 0, P12),
  D40(PGSQLNUMFIELDS, do_pgsql_num_fields, 0, 0),
  D40(PGSQLID       , do_pgsql_id        , 0,    0),
  D40(PGSQLNUMROWS  , do_pgsql_num_rows  , 0,    P1),
  D43(PGSQLINS      , do_pgsql_ins       , P12,  P12,  QF_NONE, QF_PGSQL, QF_NONE),
#endif
#ifdef SQLITE3
  D40(SQL3READWRITELINE, do_sql3_readwriteline, 0, P12),
  D40(SQL3NUMFIELDS, do_sql3_num_fields, 0, 0),
  D40(SQL3ID       , do_sql3_id        , 0,    0),
  D40(SQL3CHANGES  , do_sql3_changes   , 0,    P1),
  D43(SQL3INS      , do_sql3_ins       , P12,  P12,  QF_NONE, QF_SQLITE3, QF_NONE),
  D40(SQL3STOREINS , do_sql3_store_ins , P1_3, P1_3),
#endif
  D40(""            , NULL               , 0,    0),
};


/* scan_send_progs, Funktionen, die eine Ausgabe (als String oder direkt) liefern */
/* T_SCAN_SEND_PROGS */
scan_prog_type scan_send_progs[] =
{
  D40(READDATEI         , do_readfile           , P1,   P12),
#ifdef WEBSERVER
  D40(IMAGE             , do_image              , P1,   P1_5),
  D40(OBJECT            , do_object             , P1,   P1_4),
  D40(SOBJECT           , do_sobject            , P1,   P1_4),
  D40(EOBJECT           , do_eobject            , 0,    0),
  D40(HTML_FKT          , do_send_html          , P1,   P1),
#endif
#ifdef MAYDBCLIENT
  D40(MYSQLWRITELINE    , do_mysql_writeline    , 0,    P12),
  D40(MYSQLVALUE        , do_mysql_value        , P1,   P12),
  D40(MYSQLRAWVALUE     , do_mysql_rawvalue     , P1,   P12),
  D40(MYSQLREADVALUE    , do_mysql_read_value   , 0,    P1),
  D40(MYSQLWERT         , do_mysql_wert         , P1_3, P1_3),
  D40(MYSQLIDVALUE      , do_mysql_wert         , P1_3, P1_3),
#endif
#ifdef POSTGRESQLCLIENT
  D40(PGSQLWRITELINE    , do_pgsql_writeline    , 0,    P12),
  D40(PGSQLVALUE        , do_pgsql_value        , P1,   P12),
  D40(PGSQLREADVALUE    , do_pgsql_read_value   , 0,    P1),
  D40(PGSQLIDVALUE      , do_pgsql_idvalue      , P1_3, P1_3),
#endif
#ifdef SQLITE3
  D40(SQL3WRITELINE    , do_sql3_writeline    , 0,    P12),
  D40(SQL3VALUE        , do_sql3_value        , P1,   P12),
  D40(SQL3RAWVALUE     , do_sql3_rawvalue     , P1,   P12),
  D40(SQL3READVALUE    , do_sql3_read_value   , 0,    P1),
  D40(SQL3IDVALUE      , do_sql3_idvalue      , P1_3, P1_3),
#endif
  D40(""                , NULL                  , 0,    0),
};


/* scan_send_progs, Funktionen, die eine Ausgabe nur direkt liefern                    */
/* unter Beachtung von tablecols                                                       */
/* T_SCAN_SEND_PROGS_TC                                                                */
scan_prog_send_type scan_only_send_progs[] =
{
#ifdef MAYDBCLIENT
  D43(MYSQLREAD   , do_mysql_read_send    , P1, P1_3, QF_MYSQL, QF_OUTFORMAT, QF_OUTFORMAT),
  D43(MYSQLREADRAW, do_mysql_read_raw_send, P1, P1_3, QF_MYSQL, QF_OUTFORMAT, QF_OUTFORMAT),
#endif
#ifdef POSTGRESQLCLIENT
  D43(PGSQLREAD   , do_pgsql_read_send    , P1,   P1_3, QF_PGSQL, QF_OUTFORMAT, QF_OUTFORMAT),
#endif
#ifdef SQLITE3
  D43(SQL3READ   , do_sql3_read_send    , P1, P1_3, QF_SQLITE3, QF_OUTFORMAT, QF_OUTFORMAT),
  D43(SQL3READRAW, do_sql3_read_raw_send, P1, P1_3, QF_SQLITE3, QF_OUTFORMAT, QF_OUTFORMAT),
#endif
  D40(""          , NULL                  , 0,    0),
};


/* scan_send_progs, Funktionen, die eine Ausgabe (als String oder direkt) liefern      */
/* unter Nichtbeachtung von tablecols                                                  */
/* T_SCAN_SEND_PROGS_NTC                                                               */
scan_prog_type scan_send_progs_out[] =
{
#ifdef MAYDBCLIENT
  D43(MYSQLREAD   , do_mysql_read    , P1, P1_4, QF_MYSQL, QF_OUTFORMAT, QF_OUTFORMAT),
  D43(MYSQLREADRAW, do_mysql_read_raw, P1, P1_3, QF_MYSQL, QF_OUTFORMAT, QF_OUTFORMAT),
#endif
#ifdef POSTGRESQLCLIENT
  D43(PGSQLREAD   , do_pgsql_read    , P1, P1_3, QF_PGSQL, QF_OUTFORMAT, QF_OUTFORMAT),
#endif
#ifdef SQLITE3
  D43(SQL3READ   , do_sql3_read    , P1, P1_4, QF_SQLITE3, QF_OUTFORMAT, QF_OUTFORMAT),
  D43(SQL3READRAW, do_sql3_read_raw, P1, P1_3, QF_SQLITE3, QF_OUTFORMAT, QF_OUTFORMAT),
#endif
  D40(""          , NULL             , 0,    0),
};


/* Kommandos aus u2wput vor Senden des HTTP-Headers */
/* T_U2W_PUT_PRE_HEADER */
u2w_put_command_type u2w_put_pre_http_commands[] =
{
#ifdef WEBSERVER
  { MIME          , do_mime               , 0       },
  { SETCOOKIE     , do_setcookie          , 0       },
  { CHARSET       , do_charset            , 0       },
  { ERRORNUM      , do_httpnum            , 0       },
  { HTTPNUM       , do_httpnum            , 0       },
  { ERRORDESC     , do_httpdesc           , 0       },
  { HTTPDESC      , do_httpdesc           , 0       },
  { SAVENAME      , do_savename           , 0       },
  { REDIRECT      , do_redirect           , 0       },
#ifndef CYGWIN
  { SETUSER       , do_set_user           , 0       },
#endif
  { AUTHORIZE     , do_authorize          , 0       },
  { PAGE_NOT_FOUND, do_page_not_found     , 0       },
  { LOGIN         , do_login              , 0       },
#endif  /* WEBSERVER */
  { INCLUDE       , do_include            , PCFILEFLAG},
  { INPUT         , do_input              , PCFILEFLAG},
  { SYSTEMCMD     , do_systemcmd          , PCFILEFLAG},
  { CONTINUE      , do_continue           , PCFILEFLAG},
  { FOR           , do_for                , PCFILEFLAG},
  { FOREACH       , do_foreach            , PCFILEFLAG},
  { END_FOR       , do_end_for            , PCFILEFLAG},
  { WHILE         , do_while              , PCFILEFLAG},
  { END_WHILE     , do_end_while          , PCFILEFLAG},
  { BREAK         , do_break              , PCFILEFLAG},
  { SWITCH        , do_switch             , PCFILEFLAG},
  { CASE          , do_case_default       , PCFILEFLAG},
  { DEFAULT       , do_case_default       , PCFILEFLAG},
  { END_SWITCH    , NULL                  , PCFILEFLAG},
  { IF            , do_if                 , PCFILEFLAG},
  { ELSE          , do_else_elseif        , PCFILEFLAG},
  { ELSEIF        , do_else_elseif        , PCFILEFLAG},
  { FI            , NULL                  , PCFILEFLAG},
  { SKIP_EMPTY    , do_skip_empty_flag    , PCFILEFLAG},
  { NOT_SKIP_EMPTY, do_not_skip_empty_flag, PCFILEFLAG},
  { EXIT          , do_exit               , 0       },
  { RETURN        , do_return             , 0       },
  { TITLE         , do_title              , PCU2WFLAG},
  { BACKGROUND    , do_background         , PCU2WFLAG},
  { BODYPARS      , do_bodypars           , PCU2WFLAG},
  { ENDBLOCK      , do_endblock           , PCU2WFLAG},
  { ENDPAR        , do_endpar             , PCU2WFLAG},
  { RESULTPAGE    , do_resultpage         , PCU2WFLAG},
  { PARAMETER_EXT , do_parameter_ext      , PCU2WFLAG},
  { NO_NEWLINE    , do_no_newline         , PCU2WFLAG},
  { NOT_NO_NEWLINE, do_not_no_newline     , PCU2WFLAG},
  { TABLE_AUTOCOL , do_table_autocol_on   , PCU2WFLAG},
  { TABLE_AUTOCOL_OFF,do_table_autocol_off,PCU2WFLAG},
  { ""            , NULL                  , 0       },
};

/* Kommandos aus u2wput nach Senden des HTTP-Headers - werden bei *.s3w ignoriert */
/* T_U2W_PUT_HEADER */
u2w_put_command_type u2w_put_header_commands[] =
{ { HTML_HEAD_ON       , do_html_head_on       , PCU2WFLAG },
  { HTML_HEAD_OFF      , do_html_head_off      , PCU2WFLAG },
  { HTML_HEADS_ON      , do_html_heads_on      , PCU2WFLAG },
  { HTML_HEADS_OFF     , do_html_head_off      , PCU2WFLAG },
  /* ACHTUNG: Die Position der ersten 4 Eintraege darf nicht veraendert werden         */
  /* ansonsten muss "T_U2W_PUT_HEADER_S_ELEMENTS" geaendert werden                     */
  { PARAMETER_GET      , do_parameter_get      , PCU2WFLAG },
  { PARAMETER_MULTIPART, do_parameter_multipart, PCU2WFLAG },
  { PARAMETER          , do_parameter          , PCU2WFLAG },
  { REFRESH            , do_refresh            , 0         },
  { FRAME              , do_frame              , PCU2WFLAG },
  { FRAME_END          , do_frame_end          , PCU2WFLAG },
  { MENUHEAD           , do_menuhead           , PCU2WFLAG },
  { MENU               , do_menu               , PCU2WFLAG },
  { SETOBEN            , do_settop             , PCU2WFLAG },
  { SETTOP             , do_settop             , PCU2WFLAG },
  { SETLINKS           , do_setleft            , PCU2WFLAG },
  { SETLEFT            , do_setleft            , PCU2WFLAG },
  { SETRECHTS          , do_setright           , PCU2WFLAG },
  { SETRIGHT           , do_setright           , PCU2WFLAG },
  { FSETTOP            , do_fsettop            , PCU2WFLAG },
  { FSETBOTTOM         , do_fsetbottom         , PCU2WFLAG },
  { FSETLEFT           , do_fsetleft           , PCU2WFLAG },
  { FSETRIGHT          , do_fsetright          , PCU2WFLAG },
  { SETFRAME           , do_setframe           , PCU2WFLAG },
  { CSSLINK            , do_csslink            , PCU2WFLAG },
  { ""                 , NULL                  , 0 },
};

/* Kommandos aus u2wput nach Beginn des HTTP-Headers */
/* T_U2W_PUT_HEADER */
u2w_put_command_type u2w_put_http_header_commands[] =
{ { HTTP_HEAD_ON       , do_http_head_on       , PCU2WFLAG },
  { HTTP_HEAD_OFF      , do_http_head_off      , PCU2WFLAG },
  { HTTP_HEADS_ON      , do_http_heads_on      , PCU2WFLAG },
  { HTTP_HEADS_OFF     , do_http_head_off      , PCU2WFLAG },
  { ""                 , NULL                  , 0 },
};

/* Kommandos aus u2wput nach Senden des HTML-Headers */
/* T_U2W_PUT */
u2w_put_command_type u2w_put_commands[] =
{ { PRE_OFF       , do_pre_off       , PCU2WFLAG },
  /* PRE_OFF muss an erster Stelle stehen                         */
  { PRE_ON        , do_pre_on        , PCU2WFLAG },
#ifdef WEBSERVER
  { FLUSH         , do_flush         , PCU2WFLAG },
#endif
  { SHELLMENU     , do_shellmenu     , PCU2WFLAG },
  { SUBFILE       , do_subfile       , PCU2WFLAG },
  { EXITMENU      , do_exitmenu      , PCU2WFLAG },
  { MENULINK      , do_menulink      , PCU2WFLAG },
  { NEWWIND       , do_newwind       , PCU2WFLAG },
  { NEWMENU       , do_newmenu       , PCU2WFLAG },
  { SUBMENU       , do_submenu       , PCU2WFLAG },
  { TABLE_START   , do_table_start   , PCU2WFLAG },
  { TABLE_HEAD    , do_table_head    , PCU2WFLAG },
  { TABLE_END     , do_table_end     , PCU2WFLAG },
  { CENTER        , do_center        , PCU2WFLAG },
  { LEFT          , do_left          , PCU2WFLAG },
  { ""            , NULL             , 0 },
};
 

/* T_FORMAT */
format_command_type format_commands[] =
{ { TT,            "<code>"                           , ""},
  { AUS TT,        "</code>"                          , ""},
  { KBD,           "<kbd>"                            , ""},
  { AUS KBD,       "</kbd>"                           , ""},
  { SAMP,          "<samp>"                           , ""},
  { AUS SAMP,      "</samp>"                          , ""},
  { CODE,          "<code>"                           , ""},
  { AUS CODE,      "</code>"                          , ""},
  { PRESAMP,       "<pre><samp>"                      , ""},
  { AUS PRESAMP,   "</samp></pre>"                    , ""},
  { PRECODE,       "<pre><code>"                      , ""},
  { AUS PRECODE,   "</code></pre>"                    , ""},
  { UNDERLINE,     "<u>"                              , ""},
  { AUS UNDERLINE, "</u>"                             , ""},
  { BF,            "<b>"                              , ""},
  { AUS BF,        "</b>"                             , ""},
  { RED,           "<span style=\"color: red\">"      , ""},
  { GREEN,         "<span style=\"color: green\">"    , ""},
  { BLUE,          "<span style=\"color: blue\">"     , ""},
  { YELLOW,        "<span style=\"color: yellow\">"   , ""},
  { MAGENTA,       "<span style=\"color: magenta\">"  , ""},
  { AQUA,          "<span style=\"color: aqua\">"     , ""},
  { BLACK,         "<span style=\"color: black\">"    , ""},
  { WHITE,         "<span style=\"color: white\">"    , ""},
  { AUS RED,       "</span>"                          , ""},
  { AUS GREEN,     "</span>"                          , ""},
  { AUS BLUE,      "</span>"                          , ""},
  { AUS YELLOW,    "</span>"                          , ""},
  { AUS MAGENTA,   "</span>"                          , ""},
  { AUS AQUA  ,    "</span>"                          , ""},
  { AUS BLACK,     "</span>"                          , ""},
  { AUS WHITE,     "</span>"                          , ""},
  { NL,            LINEEND                            , "\n"},
  { NL_CRLF,       CRLF                               , CRLF},
  { "",            ""                                 , ""},
};

/* Ende globale Variablen */


/***************************************************************************************/
/* short is_x2w_command_z(char **line, command_prog_type progs[])                      */
/*                    char **line: Zeile von der Eingabe                               */
/*                    return: true, es folgt u2w-Funktion                              */
/*     is_x2w_command testet, ob eine u2w-Funktion folgt                               */
/***************************************************************************************/
short is_x2w_command_z(char **line, command_prog_type progs[])
{ int i;

  for(i = 0; *progs[i].command; i++)
  { if( is_function_z(line, progs[i].command) )
      return i;
  }
  return -1;
}

/***************************************************************************************/
/* short is_x2w_list_command_z(char **line, command_prog_type progs[])                 */
/*                    char **line: Zeile von der Eingabe                               */
/*                    return: true, es folgt u2w-Funktion                              */
/*     is_x2w_list_command testet, ob eine Listen ?2w-Funktion folgt                   */
/***************************************************************************************/
short is_x2w_list_command_z(char **line, command_list_prog_type progs[])
{ int i;

  for(i = 0; *progs[i].command; i++)
  { if( is_function_z(line, progs[i].command) )
      return i;
  }
  return -1;
}


/***************************************************************************************/
/* short do_x2w_command(char **s, command_prog_type prog)                              */
/*                     char **s : gelesene Eingabezeile                                */
/*                     command_prog_type Aufzurufendes Programm                        */
/*     do_x2w_command  Bestimmt Parameter und führt Funktion aus                       */
/***************************************************************************************/
short do_x2w_command(char **s, command_prog_type prog)
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "do_x2w_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = (*(prog.get_pars_fkt))(s, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return (*(prog.do_fkt))(pars, prg_pars);
}


/***************************************************************************************/
/* short skip_x2w_command(char **s, command_prog_type prog)                            */
/*                        char **s : gelesene Eingabezeile                             */
/*                        command_prog_type Aufzurufendes Programm                     */
/*     skip_x2w_command  Parameter überlesen                                           */
/***************************************************************************************/
short skip_x2w_command(char **s, command_prog_type prog)
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "skip_x2w_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = (*(prog.get_pars_fkt))(s, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return false;
}


/***************************************************************************************/
/* short do_x2w_token_command(char **s, command_prog_token_type prog)                  */
/*                     char **s : gelesene Eingabezeile                                */
/*                     command_prog_type Aufzurufendes Programm                        */
/*     do_x2w_token_command  Bestimmt Parameter und führt Funktion mit token aus       */
/***************************************************************************************/
short do_x2w_token_command(char **s, command_prog_token_type prog)
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "do_x2w_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = (*(prog.get_pars_fkt))(s, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return (*(prog.do_fkt))(pars, prg_pars, prog.token);
}


/***************************************************************************************/
/* short do_x2w_list_command(char **s, command_list_prog_type prog)                    */
/*                     char **s : gelesene Eingabezeile                                */
/*                     command_list_prog_type Aufzurufendes Programm                   */
/*     do_x2w_list_command  Bestimmt Parameter und führt Funktion aus                  */
/***************************************************************************************/
short do_x2w_list_command(char **s, command_list_prog_type prog)
{ int lens[2];
  char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN];
  char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS];
  int pars;

  LOG(1, "do_x2w_list_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = (*(prog.get_list_pars_fkt))(s, prog.maxpars, lens, parnames, list_pars,
                                     prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  LOG(7, "do_x2w_list_command, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "do_x2w_list_command, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  return lens[1] < 0 || (*(prog.do_list_fkt))(pars, lens, parnames, list_pars);
}


/***************************************************************************************/
/* short skip_x2w_list_command(char **s, command_list_prog_type prog)                  */
/*                     char **s : gelesene Eingabezeile                                */
/*                     command_list_prog_type Aufzurufendes Programm                   */
/*     skip_x2w_list_command überliest Parameter                                       */
/***************************************************************************************/
short skip_x2w_list_command(char **s, command_list_prog_type prog)
{ int lens[2];
  char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN];
  char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS];
  int pars;

  LOG(1, "skip_x2w_list_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = (*(prog.get_list_pars_fkt))(s, prog.maxpars, lens, parnames, list_pars,
                                     prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return false;
}


/***************************************************************************************/
/* short is_scan_command_z(char **line, scan_prog_type progs[])                        */
/*                    char **line: Zeile von der Eingabe                               */
/*                    scan_prog_type Programmliste                                     */
/*                    return: nf, es folgt u2w-scan-Funktion                           */
/*     is_scan_command testet, ob eine u2w-scan-Funktion folgt                         */
/***************************************************************************************/
short is_scan_command_z(char **line, scan_prog_type progs[])
{ int i;

  LOG(3, "is_scan_command_z, line: %s.\n", *line);

  for(i = 0; *progs[i].command; i++)
  { if( is_function_z(line, progs[i].command) )
      return i;
  }
  LOG(3, "/is_scan_command_z, nicht gefunden.\n");
  return -1;
}


/***************************************************************************************/
/* short is_scan_list_command_z(char **line, scan_list_prog_type progs[])              */
/*                    char **line: Zeile von der Eingabe                               */
/*                    scan_prog_type Programmliste                                     */
/*                    return: nf, es folgt u2w-scan-Funktion                           */
/*     is_scan_list_command testet, ob eine u2w-list-scan-Funktion folgt               */
/***************************************************************************************/
short is_scan_list_command_z(char **line, scan_list_prog_type progs[])
{ int i;

  LOG(3, "is_list_command_z, line: %s.\n", *line);

  for(i = 0; *progs[i].command; i++)
  { if( is_function_z(line, progs[i].command) )
      return i;
  }
  LOG(3, "/is_scan_list_command_z, nicht gefunden.\n");
  return -1;
}


/***************************************************************************************/
/* short do_scan_command(char **o, char **s, long n, scan_prog_type prog,              */
/*                     int quote)                                                      */
/*                     char **out: Ziel des Ergebnisses                                */
/*                     char **in : gelesene Eingabezeile                               */
/*                     long n    : Platz in out                                        */
/*                     scan_prog_type prog: Programm                                   */
/*                     int quote: Quotierung der Ausgabe für z. B. MySQL oder Shells   */
/*     do_scan_command Bestimmt Parameter und führt Funktion aus                       */
/***************************************************************************************/
short do_scan_command(char **out, char **s, long n, scan_prog_type prog,
                    int quote )
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "do_scan_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = get_pars(s, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return (*(prog.do_fkt))(pars, out, n, prg_pars, quote);
}


/***************************************************************************************/
/* short skip_scan_command(char **s, scan_prog_type prog)                              */
/*                     char **in : gelesene Eingabezeile                               */
/*                     scan_prog_type prog: Programm                                   */
/*     skip_scan_command überliest Parameter                                           */
/***************************************************************************************/
short skip_scan_command(char **s, scan_prog_type prog)
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "skip_scan_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = get_pars(s, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return false;
}


/***************************************************************************************/
/* short do_scan_prog_send_command(char **o, char **s, long n,scan_prog_send_type prog,*/
/*                     int quote)                                                      */
/*                     char **out: Ziel des Ergebnisses                                */
/*                     char **in : gelesene Eingabezeile                               */
/*                     long n    : Platz in out                                        */
/*                     scan_prog_type prog: Programm                                   */
/*                     int quote: Quotierung der Ausgabe für z. B. MySQL oder Shells   */
/*     do_scan_command Bestimmt Parameter und führt Funktion aus                       */
/***************************************************************************************/
short do_scan_prog_send_command(char **out, char **s, long n, scan_prog_send_type prog,
                    int quote )
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "do_scan_command, prg: %s, s: %s.\n", prog.command, *s);

  pars = get_pars(s, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return (*(prog.do_fkt))(pars, prg_pars, quote);
}


/***************************************************************************************/
/* short do_scan_list_command(int *listlen,                                            */
/*                          char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],           */
/*                          char **in, scan_list_prog_type prog)                       */
/*                     int *listlen: Anzahl der Listenelemente                         */
/*                     char list_pars: Ergebnisse                                      */
/*                     char **in : gelesene Eingabezeile                               */
/*                     scan_list_prog_type prog: Programm                              */
/*     do_scan_list_command Bestimmt Parameter und führt Funktion aus                  */
/***************************************************************************************/
short do_scan_list_command(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                         char **in, scan_list_prog_type prog)
{ char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS];
  int pars;

  LOG(1, "do_scan_list_command, prg: %s, s: %s.\n", prog.command, *in);

  pars = get_pars(in, prog.maxpars, prg_pars, prog.quoteflags);
  if( (prog.minpars & pars) != prog.minpars )
    return true;

  return (*(prog.do_fkt))(listlen, list_pars, pars, prg_pars);
}


/***************************************************************************************/
/* short get_pars(char **in, int maxpars,                                              */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],                     */
/*              char quotes[MAX_ANZ_QUOTE_PARS])                                       */
/*              char **in: Zeiger auf Eingabezeile                                     */
/*              int maxpars: Maximale Parameterpositionen                              */
/*              char prg_pars: Platz für die gefundenen Parameter                      */
/*              char quotes: ggf. Quotinganweisung für Parameterersetzung              */
/*              return: Bitflag mit den gefundener Parametern                          */
/*     get_pars Parameter einer Funktion bestimmen                                     */
/*              "..." werden beachtet und Strings werden mit '\0' beendet.             */
/*              *in Zeigt anschliessend auf Zeichen nach ')'                           */
/***************************************************************************************/
short get_pars(char **in, int maxpars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
             char quotes[MAX_ANZ_QUOTE_PARS])
{ char *p;
  int i, pars;

  LOG(1, "get_pars, s: %s, maxpars: %d.\n", *in, maxpars);

  if( **in != '(' )
  { if( **in == ' ' )
      (*in)++;
    return P0;
  }
  (*in)++;

  skip_blanks(*in);
  pars = 0;
  for( i = 0; (1<<i) <= maxpars; i++ )
  { if( (1<<i) & maxpars )
    { if( **in != ',' && **in != ')' )
        pars = pars | (1 << i);
      if( maxpars & (1 << i) )
      { p = prg_pars[i];

        LOG(21, "get_pars, i: %d, quotes: %d.\n", i, i < MAX_ANZ_QUOTE_PARS ? quotes[i] : 0);
        if( scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, "),", '\0',
                      i < MAX_ANZ_QUOTE_PARS
                      ? ( quotes[i] == QF_OUTFORMAT
                          ? ( u2w_mode == U2W_MODE ? QF_HTML : QF_STRING )
                          : quotes[i] )
                      : QF_NONE )
            && i < MAX_ANZ_QUOTE_PARS
#if defined(QF_MYSQL) || defined(QF_PGSQL)
 && (
#endif
#ifdef QF_MYSQL
 quotes[i] == QF_MYSQL
#endif
#if defined(QF_MYSQL) && defined(QF_PGSQL)
 ||
#endif
#ifdef QF_PGSQL
 quotes[i] == QF_PGSQL
#endif
#if defined(QF_MYSQL) || defined(QF_PGSQL)
)
#endif
 )
        { pars = P0;
          break;
        }
        if( **in == ' ' )
        { while( **in != ')' && **in != ',' )
            (*in)++;
        }
        if( **in == ')' )
          break;
        else if( **in )
          (*in)++;
        skip_blanks(*in);
      }
    }
  }
  while( **in && **in != ')' )
    (*in)++;
  if( **in )
    (*in)++;
  LOG(2, "/get_pars, pars: %d.\n", pars);
  LOG(7, "/get_pars, in: %s.\n", *in);
  return pars;
}


/***************************************************************************************/
/* short get_letpars(char **in, int maxpars,                                           */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],                     */
/*              char quotes[MAX_ANZ_QUOTE_PARS])                                       */
/*              char **in: Zeiger auf Eingabezeile                                     */
/*              int maxpars: Maximale Parameterpositionen                              */
/*              char prg_pars: Platz für die gefundenen Parameter                      */
/*              char quotes: ggf. Quotinganweisung für Parameterersetzung              */
/*              return: Anzahl gefundener Parameter                                    */
/*     get_letpars Parameter einer Funktion bestimmen                                  */
/*              "..." werden beachtet und Strings werden mit '\0' beendet.             */
/*              *in Zeigt anschliessend auf Zeichen nach ')'                           */
/*     return: Anzahl Parameter oder P13, wenn fehlerhafte Berechnung                  */
/***************************************************************************************/
short get_letpars(char **in, int maxpars,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                char quotes[MAX_ANZ_QUOTE_PARS])
{ char *p;

  LOG(1, "get_letpars, s: %s, maxpars: %d.\n", *in, maxpars);

  if( **in == '(' )
    return get_pars(in, maxpars, prg_pars, quotes);

  skip_blanks(*in);
  p = prg_pars[0];
  scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, "= \t,", '\0', QF_NONE);

  while( **in == ' ' || **in == '\t' )
    (*in)++;

  if( **in == LET_WERT_CHAR || **in == DEFAULT_WERT_CHAR )
  { if( **in == DEFAULT_WERT_CHAR )
    { (*in)++;
      while( **in == ' ' || **in == '\t' )
        (*in)++;
    }
    else
      (*in)++;

    if( scan_to(prg_pars[1], in, MAX_LEN_PRG_PARS, 0, "", '\0') )
    { LOG(2, "/get_letpars, i: P1|P2ERROR.\n");
      return P12|P2ERROR;
    }
    else
    { LOG(2, "/get_letpars, i: P12.\n");
      return P12;
    }
  }
  LOG(2, "/get_letpars, i: P1.\n");
  return P1;
}


/***************************************************************************************/
/* short get_listpars(char **in, int maxpars,                                          */
/*              char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],                         */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],                       */
/*              char quotes[MAX_ANZ_QUOTE_PARS])                                       */
/*              char **in: Zeiger auf Eingabezeile                                     */
/*              int maxpars: Maximale Parameterpositionen                              */
/*              char prg_pars: Platz für die gefundenen Parameter                      */
/*              char quotes: ggf. Quotinganweisung für Parameterersetzung              */
/*              return: Anzahl gefundener Parameter                                    */
/*     get_listpars Parameter einer Funktion bestimmen                                 */
/*              "..." werden beachtet und Strings werden mit '\0' beendet.             */
/*              *in Zeigt anschliessend auf Zeichen nach ')'                           */
/***************************************************************************************/
short get_listpars(char **in, int maxpars, int len[2],
                 char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
                 char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                 char quotes[2])
{ char *p;
  int l;

  LOG(1, "get_listpars, s: %s, maxpars: %d.\n", *in, maxpars);

  skip_blanks(*in);
  l = 0;
  p = parnames[l];
  do
  { scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, "= \t,", '\0', QF_NONE);
    while( **in == ' ' || **in == '\t' || **in == ',' )
    { if( **in == ',' )
      { *p = '\0';
        p = parnames[++l];
      }
      (*in)++;
    }
  } while( **in && **in != '=' );
  *p = '\0';
  len[0] = l+1;

  if( **in == LET_WERT_CHAR )
  { (*in)++;

    len[1] = 0;
    list_scan_to(&len[1], list_pars, in);
    LOG(7, "get_listpars, list_pars[0]: %s.\n", list_pars[0]);
    LOG(17, "get_listpars, list_pars[1]: %s.\n", list_pars[1]);
    LOG(2, "/get_listpars, i: 2.\n");
    return P12;
  }
  LOG(2, "/get_listpars, i: 1.\n");
  return P1;
}


/***************************************************************************************/
/* short get_evalpars(char **in, int n,                                                */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],                 */
/*                  char quotes[MAX_ANZ_QUOTE_PARS])                                   */
/*              char **in: Zeiger auf Eingabezeile                                     */
/*              int n  : Anzahl der Parameter                                          */
/*              char prg_pars: Platz für die gefundenen Parameter                      */
/*              char quotes: ggf. Quotinganweisung für Parameterersetzung              */
/*              return: Anzahl gefundener Parameter                                    */
/*     get_evalpars Parameter einer Funktion bestimmen                                 */
/*              "..." werden beachtet und Strings werden mit '\0' beendet.             */
/*              *in Zeigt anschliessend auf Zeichen nach ')'                           */
/***************************************************************************************/
short get_evalpars(char **in, int n, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                 char quotes[MAX_ANZ_QUOTE_PARS])
{ char *p;

  skip_blanks(*in);
  p = prg_pars[0];
  scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, "", '\0', QF_NONE);
  LOG(2, "/get_evalpars.\n");
  return P1;
}


/***************************************************************************************/
/* short get_formpars(char **in, int maxpars,                                          */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],                 */
/*                  char quotes[MAX_ANZ_QUOTE_PARS])                                   */
/*              char **in: Zeiger auf Eingabezeile                                     */
/*              int n  : Anzahl der Parameter                                          */
/*              char prg_pars: Platz für die gefundenen Parameter                      */
/*              char quotes: ggf. Quotinganweisung für Parameterersetzung              */
/*              return: Anzahl gefundener Parameter                                    */
/*     get_formpars Parameter einer Funktion bestimmen                                 */
/*              "..." werden beachtet und Strings werden mit '\0' beendet.             */
/*              *in Zeigt anschliessend auf Zeichen nach ')'                           */
/*              Positionsnummern der Parameter:                                        */
/*              %p:<n>:<m>:<ro|d>:<title>:<class>:<add> <name> <options>, <def>; <text>*/
/*                  5   6     7     8        9      A     1        4        2      3   */
/***************************************************************************************/
short get_formpars(char **in, int maxpars,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],
                 char quotes[MAX_ANZ_QUOTE_PARS])
{ char *p;
  int pars;

  LOG(1, "get_formpars, s: %s, maxpars: %d.\n", *in, maxpars);

  if( **in == '(' )
  { par_old_format = false;
    return get_pars(in, maxpars, prg_pars, quotes);
  }
  else if( **in == ' ' )
    (*in)++;

  par_old_format = true;
  pars = 0;

  if( **in == PAR_PAR_CHAR )
  { (*in)++;
    if( **in != ' ' && **in != '\t' && **in != PAR_PAR_CHAR )
    { scan_to(prg_pars[4], in, MAX_LEN_PRG_PARS, 0, ": \t", '\0');
      pars = pars | P5;
    }
    if( **in == PAR_PAR_CHAR )
    { (*in)++;
      if( **in != ' ' && **in != '\t' && **in != PAR_PAR_CHAR )
      { scan_to(prg_pars[5], in, MAX_LEN_PRG_PARS, 0, ": \t", '\0');
        pars = pars | P6;
      }
      if( **in == PAR_PAR_CHAR )
      { (*in)++;
        if( **in != ' ' && **in != '\t' && **in != PAR_PAR_CHAR )
        { scan_to(prg_pars[6], in, MAX_LEN_PRG_PARS, 0, ": \t", '\0');
          pars = pars | P7;
        }
        if( **in == PAR_PAR_CHAR )
        { (*in)++;
          if( **in != ' ' && **in != '\t' && **in != PAR_PAR_CHAR )
          { scan_to(prg_pars[7], in, MAX_LEN_PRG_PARS, 0, ": \t", '\0');
            pars = pars | P8;
          }
          if( **in == PAR_PAR_CHAR )
          { (*in)++;
            if( **in != ' ' && **in != '\t' && **in != PAR_PAR_CHAR )
            { scan_to(prg_pars[8], in, MAX_LEN_PRG_PARS, 0, ": \t", '\0');
              pars = pars | P9;
            }
            if( **in == PAR_PAR_CHAR )
            { (*in)++;
              if( **in != ' ' && **in != '\t' && **in != PAR_PAR_CHAR )
              { scan_to(prg_pars[9], in, MAX_LEN_PRG_PARS, 0, ": \t", '\0');
                pars = pars | PA;
              }
            }
          }
        }
      }
    }
  }
  if( maxpars & P1 )
  { skip_blanks(*in);
    scan_to(prg_pars[0], in, MAX_LEN_PRG_PARS, 0, " \t,;", '\0');
    pars = pars | P1;
  }

  skip_blanks(*in);
  if( **in && **in != ',' && **in != ';' )
  { p = prg_pars[3];
    while( **in && **in != ',' && **in != ';' )
    { if( **in == '"' )
      { *p++ = *(*in)++;
        scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, "\"", '\0', QF_NONE);
        if( **in == '"' )
        { *p++ = *(*in)++;
          *p = '\0';
        }
      }
      else if( **in == '\'' )
      { *p++ = *(*in)++;
        scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, "'", '\0', QF_NONE);
        if( **in == '\'' )
        { *p++ = *(*in)++;
          *p = '\0';
        }
      }
      else
        scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, ",;", '\0', QF_NONE);
    }
    pars = pars | P4;
  }
  else
    prg_pars[3][0] = '\0';

  if( **in == ',' )
  { (*in)++;
    skip_blanks(*in);
    p = prg_pars[1];
    scan_to_z(&p, in, MAX_LEN_PRG_PARS, 0, ";", '\0', quotes[1]);
    pars = pars | P2;
  }
  else
    prg_pars[1][0] = '\0';

  if( **in == ';' )
  { (*in)++;
    skip_blanks(*in);
    scan_to(prg_pars[2], in, MAX_ZEILENLAENGE, 0, "", '\0');
    pars = pars | P3;
  }

  LOG(2, "/get_formpars, pars: %d.\n", pars);
  return pars;
}
