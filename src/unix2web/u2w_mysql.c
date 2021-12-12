/**********************************************/
/* File: u2w_mysql.c                          */
/* Funktionen zum Ansprechen von MySQL        */
/* timestamp: 2017-02-25 18:50:20             */
/**********************************************/

#include "u2w.h"


/* Anfang globale Variablen */
#ifdef MAYDBCLIENT
int mysql_error_log_flag = false;    /* true: MySQL-Error ins Logfile/stderr           */
int mysql_error_out_flag = false;    /* true: MySQL-Error an stdout / HTML             */
int mysql_connect_flag = false;
#ifdef MAYDBRECONNECT
int mysql_reconnect_secs = 0;
#else
#define MYSQL_QUERY(mh, q) mysql_query(mh, q)
#endif
MYSQL mh;
int mysqlport = STD_MYSQLPORT;
#endif
/* Ende globale Variablen */


#ifdef MAYDBCLIENT

#ifndef _LARGEFILE64_SOURCE
#define open64(...) open(__VA_ARGS__)
#define stat64(a,b) stat(a,b)
#endif

int mysql_query_flag = false;
int mysql_id_flag = false;

#define MAX_P_QUERIES 5
MYSQL_RES *mres[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
int mysql_res_flag[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
int mysql_line_flag[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
int mysql_next_value[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
MYSQL_ROW mrow[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
unsigned int mnf[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
unsigned long *mlengths[MAX_ANZ_INCLUDE+MAX_P_QUERIES];


/***************************************************************************************/
/* nur bei DBCLIENT (noch nicht fertig)                                                */
#ifdef DBCLIENT
/***************************************************************************************/
/* MYSQL *mysql_connect(char *server, char *user, char *pwd, char *db, int port,       */
/*                      char *cs)                                                      */
/*                      char *server: Servername                                       */
/*                      char *user  : Username                                         */
/*                      char *pwd   : Kennwort                                         */
/*                      char *db    : Datenbank                                        */
/*                      char *cs    : Characterset, wenn gesetzt                       */
/*                      int port    : Port                                             */
/*        mysql_connect öffnet Verbindung mit MySQL-Datenbank, wird von sql_connect    */
/*                      aufgerufen.                                                    */
/*                      Return: MySQL-Handle bei Erfolg, sonst NULL                    */
/***************************************************************************************/
MYSQL *mysql_connect(char *server, char *user, char *pwd, char *db, int port, char *cs)
{ MYSQL *mh;

  mysql_query_flag = false;

  if( NULL != (mh = calloc(1, sizeof(MYSQL))) )
  { mysql_init(mh);
    if( NULL == mysql_real_connect(mh, prg_pars[1], prg_pars[2], prg_pars[3],
                                   prg_pars[4], mysqlport, NULL, 0) )
    { if( mysql_error_log_flag )
        logging("Failed to connect to database: Error: %s.\n", mysql_error(&mh));
      free(mh);
    }
    else
    { if( cs && *cs )
      { LOG(11, "mysql_connect, mysql_set_character_set: %s.\n", cs);
         mysql_set_character_set(&mh, cs);
      }
#ifdef WEBSERVER
      else if( u2w_charset && strstr(u2w_charset, "UTF-8") )
      { LOG(11, "mysql_connect, mysql_set_character_set: utf8.\n");
        mysql_set_character_set(&mh, "utf8");
      }
#endif
      return mh;
    }
  }
  return NULL;
}


/***************************************************************************************/
/* MYSQL_RES *sql_mysql_query(MYSQL *mh, char *query)                                  */
/*                            MYSQL *mh: Handle zur DB                                 */
/*                            char *query: SQL-Query                                   */
/*            sql_mysql_query: Query ausführen, wird von u2w_sql_query aufgerufen      */
/***************************************************************************************/
MYSQL_RES *sql_mysql_query(MYSQL *mh, char *query)
{ MYSQL_RES *res;

  LOG(1, "sql_mysql_query.\n");

  LOG(3, "sql_mysql_query, query: %s.\n", query);

  if( mysql_query(mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s\nquery: %s.\n", mysql_error(mh), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { mysql_query_flag = true;
    return mysql_store_result(mh);
  }
}
#endif  /* #ifdef DBCLIENT */
/***************************************************************************************/


/***************************************************************************************/
/* void mysql_init_flags(void)                                                         */
/*      mysql_init_flags, MySQL-Flags initialisieren                                   */
/***************************************************************************************/
void mysql_init_flags(void)
{ int i;

  LOG(11, "mysql_init_flags.\n");

  for( i = 0; i < MAX_ANZ_INCLUDE+MAX_P_QUERIES; i++ )
  { mysql_res_flag[i] = 0;
    mnf[i] = 0;
  }
}


/***************************************************************************************/
/* void mysql_free_one_res(int i)                                                      */
/*      mysql_free_one_res Speicher der mres freigeben                                 */
/***************************************************************************************/
void mysql_free_one_res(int i)
{
  LOG(1, "mysql_free_one_res, i: %d.\n", i);

  if( mysql_res_flag[i] )
  { mysql_free_result(mres[i]);
    mysql_res_flag[i] = 0;
    mnf[i] = 0;
  }
  LOG(2, "/mysql_free_one_res.\n");
}


/***************************************************************************************/
/* void mysql_free_res(void)                                                           */
/*      mysql_free_res Speicher der mres freigeben                                     */
/***************************************************************************************/
void mysql_free_res(void)
{ int i;

  LOG(1, "mysql_free_res.\n");

  for( i = MAX_ANZ_INCLUDE; i < MAX_ANZ_INCLUDE+MAX_P_QUERIES; i++ )
    mysql_free_one_res(i);
}


/***************************************************************************************/
/* int u2w_mysql_port(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_port Port für MySQL-Server ändern                                     */
/***************************************************************************************/
int u2w_mysql_port(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( isdigit(prg_pars[0][0]) )
  { mysqlport = atoi(prg_pars[0]);
    return false;
  }
  return true;
}


/***************************************************************************************/
/* int u2w_mysql_connect(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_connect öffnet eine Verbindung zum MySQL-Server                       */
/***************************************************************************************/
int u2w_mysql_connect(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ static char cursqlserver[128], cursqluser[128], cursqlpwd[128], cursqldb[128];

  LOG(1, "u2w_mysql_connect.\n");

  LOG(3, "u2w_mysql_connect, server: %s, user: %s, pwd, %s, db: %s.\n", prg_pars[0],
      prg_pars[1], prg_pars[2], prg_pars[3]);

  LOG(5, "u2w_mysql_conncet, port: %d.\n", mysqlport);

  if( mysql_connect_flag )
  { if( !strcmp(prg_pars[0], cursqlserver) && !strcmp(prg_pars[1], cursqluser)
        && !strcmp(prg_pars[2], cursqlpwd) && !strcmp(prg_pars[3], cursqldb) )
      return false;
    else
    { mysql_free_res();
      mysql_close(&mh);
      mysql_query_flag = false;
    }
  }

  mysql_init(&mh);

  if( NULL == mysql_real_connect(&mh, prg_pars[0], prg_pars[1], prg_pars[2],
                                 prg_pars[3], mysqlport, NULL, 0) )
  { if( mysql_error_log_flag )
      logging("Failed to connect to database: Error: %s.\n", mysql_error(&mh));
    mysql_connect_flag = false;
    return true;
  }

  if( pa & P5 )
  { LOG(11, "u2w_mysql_connect, mysql_set_character_set: %s.\n", prg_pars[4]);
    mysql_set_character_set(&mh, prg_pars[4]);
  }
#ifdef WEBSERVER
  else if( u2w_charset && !strcmp(u2w_charset, "UTF-8") )
  { LOG(11, "u2w_mysql_connect, mysql_set_character_set, u2w_charset: %s.\n",
        u2w_charset);
    mysql_set_character_set(&mh, "utf8");
  }
#endif

  strcpyn(cursqlserver, prg_pars[0], 128);
  strcpyn(cursqluser, prg_pars[1], 128);
  strcpyn(cursqlpwd, prg_pars[2], 128);
  strcpyn(cursqldb, prg_pars[3], 128);
  mysql_connect_flag = true;
  return false;
}


/***************************************************************************************/
/* int u2w_mysql_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_query Stellt Anfrage an Mysql-Datenbank, Ergebnisse können dann mit   */
/*                 u2w_mysql_next_line gelesen und mit mysql_get_value gelesen werden  */
/***************************************************************************************/
int u2w_mysql_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_mysql_query.\n");

  if( !mysql_connect_flag )
    return true;

  if( pa & P2 )
  { if( !isdigit(prg_pars[1][0]) )
      return true;
    cn = atoi(prg_pars[1]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( mysql_res_flag[cn] )
  { mysql_res_flag[cn] = mysql_line_flag[cn] = false;
    mysql_free_result(mres[cn]);
    mnf[cn] = 0;
  }

  LOG(3, "u2w_mysql_query, query: %s.\n", prg_pars[0]);

  if( MYSQL_QUERY(&mh, prg_pars[0]) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s\nquery: %s.\n", mysql_error(&mh), prg_pars[0]);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { mysql_query_flag = true;
    if( (mres[cn] = mysql_store_result(&mh)) )
    { mysql_res_flag[cn] = true;
      mnf[cn] = mysql_num_fields(mres[cn]);
      LOG(50, "u2w_mysql_query, mnf[%d] = %d\n", cn, mnf[cn]);
      return false;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* int u2w_mysql_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_write Stellt Anfrage an Mysql-Datenbank, für insert und update        */
/***************************************************************************************/
int u2w_mysql_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(1, "u2w_mysql_write, query: %s.\n", prg_pars[0]);

  if( !mysql_connect_flag )
    return true;

  if( MYSQL_QUERY(&mh, prg_pars[0]) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s\nquery: %s.\n", mysql_error(&mh), prg_pars[0]);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { mysql_query_flag = true;
    mysql_id_flag = true;
    return false;
  }
}


/***************************************************************************************/
/* int u2w_mysql_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_test Stellt Anfrage an Mysql-Datenbank, und liefert nur Wahrheitswert */
/***************************************************************************************/
int u2w_mysql_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ MYSQL_RES *lmres;

  LOG(1, "u2w_mysql_test, query: %s.\n", prg_pars[0]);

  if( !mysql_connect_flag )
    return true;

  if( MYSQL_QUERY(&mh, prg_pars[0]) )
    return true;
  else
  { mysql_query_flag = true;
    if( (lmres = mysql_store_result(&mh)) )
      mysql_free_result(lmres);
    mysql_id_flag = true;
    return false;
  }
}


#define MYSQL_STORE_MODE_APPEND 1
#define MYSQL_PUFFERSIZE 10000000

/***************************************************************************************/
/* int u2w_mysql_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_store Datei als blob in Datenbank speichern - Update                  */
/***************************************************************************************/
int u2w_mysql_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *puffer, *p;
  long nb, i;
  int hd_in, ret;
  int mode = 0;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#endif

  LOG(1, "u2w_mysql_store, Datei: %s, Tabelle: %s, Feld: %s, ID: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3]);

  if( !mysql_connect_flag )
    return true;

  if( pa & P5 )
  { switch( prg_pars[4][0] )
    { case 'a': mode = MYSQL_STORE_MODE_APPEND | mode ;
                break;
    }
  }

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_mysql_store, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { int concatflag;

        LOG(5, "u2w_mysql_store, grosse Datei.\n");
        if( NULL == (puffer = malloc(MYSQL_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = puffer;
        if( mode & MYSQL_STORE_MODE_APPEND )
        { concatflag = true;
          p += snprintf(p, MYSQL_PUFFERSIZE, "update %s set %s=concat(%s,'",
                        prg_pars[1], prg_pars[2], prg_pars[2]);
        }
        else
        { concatflag = false;
          p += snprintf(p, MYSQL_PUFFERSIZE, "update %s set %s='",
                        prg_pars[1], prg_pars[2]);
        }
        LOG(9, "u2w_mysql_store, grosse Datei vor while.\n");
        while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
        { for( i = 0; i < nb; i++ )
          { switch( zeile[i] )
            { case '\0': *p++ = '\\';
                         *p++ = '0';
                         break;
              case '\\':
              case '\'': *p++ = '\\';
              default:   *p++ = zeile[i];
            }
            if( p-puffer >= MYSQL_PUFFERSIZE-100 )
            { LOG(9, "u2w_mysql_store, grosse Date p-puffer >= 9999900 ...\n");
              if( concatflag )
                snprintf(p, MYSQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                         prg_pars[3]);
              else
              { snprintf(p, MYSQL_PUFFERSIZE - (p-puffer), "' where ID='%s'",
                         prg_pars[3]);
                concatflag = true;
              }
              LOG(9, "u2w_mysql_store, grosse Datei vor mysql_query.\n");
              if( mysql_query(&mh, puffer) )
              { if( mysql_error_log_flag )
                  logging("mysql_query error: %s.\n", mysql_error(&mh));
                close(hd_in);
                free(puffer);
#ifdef WEBSERVER
                keepalive_flag = false;
#endif
                return true;
              }
              LOG(9, "u2w_mysql_store, grosse Datei nach mysql_query.\n");
              p = puffer + snprintf(puffer, MYSQL_PUFFERSIZE, "update %s set %s=concat(%s,'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
            }
          }
        }
        LOG(7, "u2w_mysql_store, grosse Datei, concatflag: %d.\n", concatflag);
        if( concatflag )
          snprintf(p, MYSQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        else
          snprintf(p, MYSQL_PUFFERSIZE - (p-puffer), "' where ID='%s'",
                   prg_pars[3]);
        if( mysql_query(&mh, puffer) )
        { if( mysql_error_log_flag )
            logging("mysql_query error: %s.\n", mysql_error(&mh));
          free(puffer);
          close(hd_in);
#ifdef WEBSERVER
          keepalive_flag = false;
#endif
          return true;
        }
        free(puffer);
        close(hd_in);
        return false;
      }
      else
      { LOG(5, "u2w_mysql_store, kleine Datei.\n");
        if( NULL != (puffer = malloc(100+2*stat_buf.st_size)) )
        { LOG(11, "u2w_mysql_store, kleine Datei, Puffer allokiert.\n");
          if( mode & MYSQL_STORE_MODE_APPEND )
            p = puffer + sprintf(puffer, "update %s set %s=concat(%s,'",
                                 prg_pars[1], prg_pars[2], prg_pars[2]);
          else
            p = puffer + sprintf(puffer, "update %s set %s='", prg_pars[1], prg_pars[2]);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { LOG(50, "u2w_mysql_store, kleine Datei, nb: %d.\n", nb);
            for( i = 0; i < nb; i++ )
            { switch( zeile[i] )
              { case '\0': *p++ = '\\';
                           *p++ = '0';
                           break;
                case '\\':
                case '\'': *p++ = '\\';
                default:   *p++ = zeile[i];
              }
            }
          }
          if( mode & MYSQL_STORE_MODE_APPEND )
            sprintf(p, "') where ID='%s'", prg_pars[3]);
          else
            sprintf(p, "' where ID='%s'", prg_pars[3]);
          LOG(100, "mysql_store, puffer: %s.\n", puffer);
          if( mysql_query(&mh, puffer) )
          { if( mysql_error_log_flag )
              logging("mysql_query error: %s.\n", mysql_error(&mh));
#ifdef WEBSERVER
            keepalive_flag = false;
#endif
            ret = true;
          }
          else
            ret = false;
          free(puffer);
        }
        else if( logflag & NORMLOG )
        { fprintf(stderr, _("Unable to allocate memory.\n"));
          ret = true;
        }
        else
          ret = false;
      }
      close(hd_in);
      return ret;
    }
  }
  return true;
}


/***************************************************************************************/
/* int u2w_mysql_storev(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_storev Datei als blob in @-Variable speichern                         */
/***************************************************************************************/
int u2w_mysql_storev(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *puffer, *p;
  long nb, i;
  int hd_in, ret;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#endif

  LOG(1, "u2w_mysql_storev, Datei: %s, Var: %s.\n",
      prg_pars[0], prg_pars[1]);

  if( !mysql_connect_flag )
    return true;

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_mysql_store, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { int concatflag;

        LOG(5, "u2w_mysql_storev, grosse Datei.\n");
        if( NULL == (puffer = malloc(MYSQL_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = puffer;
        concatflag = false;
        p += snprintf(p, MYSQL_PUFFERSIZE, "set %s='", prg_pars[1]);
        LOG(9, "u2w_mysql_storev, grosse Datei vor while.\n");
        while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
        { for( i = 0; i < nb; i++ )
          { switch( zeile[i] )
            { case '\0': *p++ = '\\';
                         *p++ = '0';
                         break;
              case '\\':
              case '\'': *p++ = '\\';
              default:   *p++ = zeile[i];
            }
            if( p-puffer >= MYSQL_PUFFERSIZE-100 )
            { LOG(9, "u2w_mysql_storev, grosse Date p-puffer >= 9999900 ...\n");
              if( concatflag )
                strcpyn(p, "')", MYSQL_PUFFERSIZE - (p-puffer));
              else
              { strcpyn(p, "'", MYSQL_PUFFERSIZE - (p-puffer));
                concatflag = true;
              }
              LOG(9, "u2w_mysql_storev, grosse Datei vor mysql_query.\n");
              if( mysql_query(&mh, puffer) )
              { if( mysql_error_log_flag )
                  logging("mysql_query error: %s.\n", mysql_error(&mh));
                close(hd_in);
                free(puffer);
#ifdef WEBSERVER
                keepalive_flag = false;
#endif
                return true;
              }
              LOG(9, "u2w_mysql_storev, grosse Datei nach mysql_query.\n");
              p = puffer + snprintf(puffer, MYSQL_PUFFERSIZE, "set %s=concat(%s,'",
                                    prg_pars[1], prg_pars[1]);
            }
          }
        }
        LOG(7, "u2w_mysql_storev, grosse Datei, concatflag: %d.\n", concatflag);
        if( concatflag )
          strcpyn(p, "')", MYSQL_PUFFERSIZE - (p-puffer));
        else
        { strcpyn(p, "'", MYSQL_PUFFERSIZE - (p-puffer));
          concatflag = true;
        }
        if( mysql_query(&mh, puffer) )
        { if( mysql_error_log_flag )
            logging("mysql_query error: %s.\n", mysql_error(&mh));
          free(puffer);
          close(hd_in);
#ifdef WEBSERVER
          keepalive_flag = false;
#endif
          return true;
        }
        free(puffer);
        close(hd_in);
        return false;
      }
      else
      { LOG(5, "u2w_mysql_store, kleine Datei.\n");
        if( NULL != (puffer = malloc(100+2*stat_buf.st_size)) )
        { LOG(11, "u2w_mysql_store, kleine Datei, Puffer allokiert.\n");
          p = puffer + sprintf(puffer, "set %s='", prg_pars[1]);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { LOG(50, "u2w_mysql_store, kleine Datei, nb: %d.\n", nb);
            for( i = 0; i < nb; i++ )
            { switch( zeile[i] )
              { case '\0': *p++ = '\\';
                           *p++ = '0';
                           break;
                case '\\':
                case '\'': *p++ = '\\';
                default:   *p++ = zeile[i];
              }
            }
          }
          *p++ = '\'';
          *p = '\0';
          LOG(100, "mysql_storev, puffer: %s.\n", puffer);
          if( mysql_query(&mh, puffer) )
          { if( mysql_error_log_flag )
              logging("mysql_query error: %s.\n", mysql_error(&mh));
#ifdef WEBSERVER
            keepalive_flag = false;
#endif
            ret = true;
          }
          else
            ret = false;
          free(puffer);
        }
        else if( logflag & NORMLOG )
        { fprintf(stderr, _("Unable to allocate memory.\n"));
          ret = true;
        }
        else
          ret = false;
      }
      close(hd_in);
      return ret;
    }
  }
  return true;
}


/***************************************************************************************/
/* int u2w_mysql_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_out Query ausführen und Ergebnis in Datei schreiben                   */
/***************************************************************************************/
int u2w_mysql_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(3, "u2w_mysql_out, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  return mysql2dat(prg_pars[0], prg_pars[1], pa & P3 ? prg_pars[2] : NULL,
                   pa & P4 ? prg_pars[3] : NULL);
}


/***************************************************************************************/
/* int u2w_mysql_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_get_line bereitet eine Zeile zum lesen vor                            */
/***************************************************************************************/
int u2w_mysql_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_mysql_get_line.\n");

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( mysql_res_flag[cn] && mres[cn] )
  { if( (mrow[cn] = mysql_fetch_row(mres[cn])) )
    { mlengths[cn] = mysql_fetch_lengths(mres[cn]);
      mysql_line_flag[cn] = true;
      mysql_next_value[cn] = 0;
      return false;
    }
    else
    { mysql_line_flag[cn] = false;
      mnf[cn] = 0;
      return true;
    }
  }
  mysql_line_flag[cn] = false;
  mnf[cn] = 0;
  return true;
}


/***************************************************************************************/
/* int do_mysql_list_get_line(int *listlen,                                            */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_list_get_line Eine Zeile in Liste                                      */
/***************************************************************************************/
int do_mysql_list_get_line(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn, i;

  LOG(1, "do_mysql_list_get_line, listlen: %d.\n", *listlen);

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( mysql_res_flag[cn] && mres[cn] )
  { if( (mrow[cn] = mysql_fetch_row(mres[cn])) )
    { mlengths[cn] = mysql_fetch_lengths(mres[cn]);
      mysql_line_flag[cn] = true;
      mysql_next_value[cn] = 0;
      for( i = 0; i < mnf[cn] && *listlen < MAX_LIST_LEN; i++ )
      { if( mlengths[cn][i] && mrow[cn][i] )
          strcpynm(list_pars[(*listlen)++], mrow[cn][i], MAX_LEN_LIST_PARS,
                   mlengths[cn][i]);
        else
          list_pars[(*listlen)++][0] = '\0';
      }
      LOG(17, "/do_mysql_list_get_line, list_pars[0]: %s.\n", list_pars[0]);
      return false;
    }
    else
    { mysql_line_flag[cn] = false;
      mnf[cn] = 0;
      *listlen = -1;
      return true;
    }
  }
  mysql_line_flag[cn] = false;
  mnf[cn] = 0;
  *listlen = -1;
  return true;
}


/***************************************************************************************/
/* int u2w_mysql_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_isvalue testet, ob noch ein Wert für mysqlreadvalue vorliegt          */
/***************************************************************************************/
int u2w_mysql_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_mysql_isvalue.\n");

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  return !mysql_line_flag[cn] || mysql_next_value[cn] >= mnf[cn];
}


/***************************************************************************************/
/* int do_mysql_writeline(int pa, char **out, long n,                                  */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_mysql_writeline fügt Zeile des letzten mysql_getline ein                    */
/***************************************************************************************/
int do_mysql_writeline(int pa, char **out, long n,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  char *oo;
  int i;

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( !(pa & P2) )
  { prg_pars[1][0] = ' ';
    prg_pars[1][1] = '\0';
  }

  if( mysql_line_flag[cn] )
  { if( out )
    { oo = *out;
      for( i = 0; i < mnf[cn]; i++ )
      { if( i )
          strqcpyn_z(out, prg_pars[1], n - (*out-oo), qf_strings[quote]);
        if( mrow[cn][i] )
          strqcpynm_z(out, mrow[cn][i], n -(*out-oo), mlengths[cn][i],
                      qf_strings[quote]);
      }
    }
    else
    { for( i = 0; i < mnf[cn]; i++ )
      { if( i )
          dosendh(prg_pars[1]);
        if( mrow[cn][i] )
        { dobinsendh(mrow[cn][i], mlengths[cn][i]);
          last_char_sended = last_non_ws_char(mrow[cn][i], mlengths[cn][i]);
        }
      }
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_mysql_readwriteline(int pa, char **out, long n,                              */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_readwriteline fügt eine Zeile der letzten mysql_query ein              */
/***************************************************************************************/
int do_mysql_readwriteline(int pa, char **out, long n,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !u2w_mysql_get_line(pa, prg_pars) )
    return do_mysql_writeline(pa, out, n, prg_pars, quote);
  else
    return true;
}


/***************************************************************************************/
/* int do_mysql_num_fields(int pa, char **out, long n,                                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_num_fields ergibt Anzahl Spalten der Mysql-Abfrage                     */
/***************************************************************************************/
int do_mysql_num_fields(int pa, char **out, long n,
                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

 if( !mysql_res_flag[cn] )
    return true;
  *out += snprintf(*out, n, "%d", mnf[cn]);
  return false;
}


/***************************************************************************************/
/* int do_mysql_value(int pa, char **out, long n,                                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_value fügt einen Wert einer Zeile ein                                  */
/***************************************************************************************/
int do_mysql_value(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_mysql_value, qoute: %d\n", quote);

  i = 0;

  if( pa & P2 )
  { if( !isdigit(prg_pars[1][0]) )
      return true;
    cn = atoi(prg_pars[1]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  LOG(3, "do_mysql_value, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( mysql_line_flag[cn] && isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_mysql_value, i: %d, mnf[%d]: %d.\n", i, cn, mnf[cn]);
    if( i < mnf[cn] )
    { LOG(50, "do_mysql_value, mrow[%d][%d]: %s\n", cn, i, mrow[cn][i]);
      if( mrow[cn][i] )
      { if( out )
        { if( quote == QF_HTML )
            code_html_nm(out, mrow[cn][i], n, mlengths[cn][i]);
          else
            strqcpynm_z(out, mrow[cn][i], n, mlengths[cn][i], qf_strings[quote]);
        }
        else
        { dobinsendh(mrow[cn][i], mlengths[cn][i]);
          last_char_sended = last_non_ws_char(mrow[cn][i], mlengths[cn][i]);
        }
      }
    }
    LOG(1, "/do_mysql_value\n");
    return false;
  }
  LOG(1, "/do_mysql_value - ERROR\n");
  return true;
}


/***************************************************************************************/
/* int do_mysql_rawvalue(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_rawvalue fügt einen Wert einer Zeile ein ohne HTML Konvertierung       */
/***************************************************************************************/
int do_mysql_rawvalue(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_mysql_rawvalue\n");

  i = 0;

  if( pa & P2 )
  { if( !isdigit(prg_pars[1][0]) )
      return true;
    cn = atoi(prg_pars[1]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  LOG(3, "do_mysql_rawvalue, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_rawmysql_value, i: %d, mnf[%d]: %d.\n", i, cn, mnf[cn]);
    if( i < mnf[cn] )
    { if( mrow[cn][i] )
      { if( out )
          strqcpynm_z(out, mrow[cn][i], n, mlengths[cn][i], qf_strings[quote]); 
        else
        { dobinsend(mrow[cn][i], mlengths[cn][i]);
          last_char_sended = last_non_ws_char(mrow[cn][i], mlengths[cn][i]);
        }
      }
    }
    LOG(1, "/do_mysql_rawvalue\n");
    return false;
  }
  LOG(1, "/do_mysql_rawvalue - ERROR\n");
  return true;
}


/***************************************************************************************/
/* int do_mysql_read_value(int pa, char **out, long n,                                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_read_value fügt näcshten Wert einer Zeile ein                          */
/***************************************************************************************/
int do_mysql_read_value(int pa, char **out, long n,
                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_P_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( !mysql_line_flag[cn] || mysql_next_value[cn] >= mnf[cn] )
    return true;

  if( mrow[cn][mysql_next_value[cn]] )
  { if( out )
      strqcpynm_z(out, mrow[cn][mysql_next_value[cn]], n,
                  mlengths[cn][mysql_next_value[cn]], qf_strings[quote]); 
    else
    { dobinsendh(mrow[cn][mysql_next_value[cn]], mlengths[cn][mysql_next_value[cn]]);
      last_char_sended = last_non_ws_char(mrow[cn][mysql_next_value[cn]],
                                          mlengths[cn][mysql_next_value[cn]]);
    }
  }

  mysql_next_value[cn]++;
  return false;
}


/***************************************************************************************/
/* int do_mysql_id(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql fügt letzte ID (nach insert) ein                                       */
/***************************************************************************************/
int do_mysql_id(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( !mysql_id_flag )
    return true;
  *out += snprintf(*out, n, "%llu", mysql_insert_id(&mh));
  return false;
}


/***************************************************************************************/
/* int do_mysql_read_send(int pa,                                                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_read_tc Query ausführen und Ergebnis einfügen tablecols beachten       */
/***************************************************************************************/
int do_mysql_read_send(int pa,
                     char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_mysql_read_send, pa: %d, %s, %s, quote: %d, last_char_sended: %d.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "", quote, (int)last_char_sended);

  if( (pa & (P2|P3)) || !tablecols || u2w_mode != U2W_MODE )
    return mysql2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, true);
  else
    return mysql2net_tbl(prg_pars[0], true, true);
}


/***************************************************************************************/
/* int do_mysql_read(int pa, char **out, long n,                                       */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_read_ntc Query ausführen und Ergebnis einfügen tablecols nicht beachten*/
/***************************************************************************************/
int do_mysql_read(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_mysql_read, pa: %d, %s, %s, quote: %d.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "", quote);

  if( pa & P4 )
  { if( atol(prg_pars[3]) < n )
      n = atol(prg_pars[3]);
  }

  if( out )
    return mysql2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, quote);
  else
    return mysql2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, true);
}


/***************************************************************************************/
/* int do_mysql_read_raw_send(int pa,                                                  */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_read_raw_tc Query ausführen und Ergebnis ohne HTML-Quoting ausgeben    */
/*                          tablecols beachten                                         */
/***************************************************************************************/
int do_mysql_read_raw_send(int pa,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_mysql_read_tc, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( (pa & (P2|P3)) || !tablecols || u2w_mode != U2W_MODE )
    return mysql2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, false);
  else
    return mysql2net_tbl(prg_pars[0], true, false);
}


/***************************************************************************************/
/* int do_mysql_read_raw(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_read_raw_ntc Query ausführen und Ergebnis ohne HTML-Quoting ausgeben   */
/*                          tablecols nicht beachten                                   */
/***************************************************************************************/
int do_mysql_read_raw(int pa, char **out, long n,
                      char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_mysql_read, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( out )
    return mysql2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, quote);
  else
    return mysql2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, false);
}


/***************************************************************************************/
/* int do_mysql_list_read(int *listlen,                                                */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_list_read Query ausführen und Ergebnis in Liste                        */
/***************************************************************************************/
int do_mysql_list_read(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ MYSQL_RES *mres;
  MYSQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i;

  LOG(3, "do_mysql_list_read, q: %s.\n", prg_pars[0]);

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( MYSQL_QUERY(&mh, prg_pars[0]) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\n", mysql_error(&mh));
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { mres = mysql_store_result(&mh);
    if( mres )
    { nf = mysql_num_fields(mres);
      LOG(11, "do_mysql_list_read, nf: %d.\n", nf);
      if( (mrow = mysql_fetch_row(mres)) )
      { lengths = mysql_fetch_lengths(mres);
        for( i = 0; i < nf && *listlen < MAX_LIST_LEN; i++ )
        { if( lengths[i] && mrow[i] )
            strcpynm(list_pars[(*listlen)++], mrow[i], MAX_LEN_LIST_PARS, lengths[i]);
          else
            list_pars[(*listlen)++][0] = '\0';
        }
      }
      mysql_free_result(mres);
      return false;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* int do_mysql_wert(int pa, char **out, long n,                                       */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_wert Einen Wert einer Tabelle bestimemn                                */
/***************************************************************************************/
int do_mysql_wert(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[1024];

  if( prg_pars[0][0] && prg_pars[1][0] && prg_pars[2][0] )
  { snprintf(query, 1024, "select %s from %s where ID='%s'", prg_pars[1], prg_pars[0],
             prg_pars[2]);

    if( out )
      return mysql2s(query, out, n, false, "", "", quote);
    else
      return mysql2net(query, false, "", "", true);
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_mysql_ins(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_ins testen, ob Datensatz, der beschrieben wird vohanden ist, dann      */
/*                ID zurück, sonst neuen Datensatz erzeugen und neue ID zurück         */
/***************************************************************************************/
int do_mysql_ins(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE];
  char *p, *q;
  int q_flag;

  if( prg_pars[0][0] && prg_pars[1][0] )
  { q = query + snprintf(query, MAX_ZEILENLAENGE, "select ID from %s where ",
                         prg_pars[0]);
    p = prg_pars[1];
    q_flag = false;
    while( *p )
    { if( *p == '\\' && *(p+1) )
      { *q++ = *p++;
        *q++ = *p++;
      }
      else if( !q_flag )
      { if( *p == ',' )
        { strcpyn_z(&q, " AND ", MAX_ZEILENLAENGE -(q-query));
          p++;
        }
        else
        { if( *p == '\'' )
            q_flag = true;
          *q++ = *p++;
        }
      }
      else
      { if( *p == '\'' )
          q_flag = false;
        *q++ = *p++;
      }
    }
    strcpyn_z(&q, " limit 1", MAX_ZEILENLAENGE -(q-query));
    *q = '\0';
    LOG(3, "do_mysql_ins, query-select: %s.\n", query);

    p = *out;
    mysql2s(query, out, n, false, "", "", quote);
    if( *out == p )
    { snprintf(query, MAX_ZEILENLAENGE, "insert into %s set %s", prg_pars[0],
               prg_pars[1]);
      LOG(3, "do_mysql_ins, query-insert: %s.\n", query);
      return mysqlinsert2s(query, out, n, false);
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_mysql_enums(int pa, char **out, long n,                                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_enums Möglichkeiten eines ENUMs oder SETs bestimmen                    */
/***************************************************************************************/
int do_mysql_enums(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE];
  char qresult[MAX_ZEILENLAENGE], *qr;

  LOG(3, "do_mysql_enums, pa: %d, %s, %s.\n", pa, prg_pars[0], prg_pars[1]);

  snprintf(query, MAX_ZEILENLAENGE, "show columns from %s like '%s'", prg_pars[0],
           prg_pars[1]);

  qr = qresult;
  if( mysql2s(query, &qr, MAX_ZEILENLAENGE, false, " ", " ", quote) )
    return true;
  if( (qr = strrchr(qresult, '\'')) )
  { *qr = '\0';
    if( (qr = strchr(qresult, '\'')) )
    { while( *qr == '\'' )
      { qr++;
        while( *qr && *qr != '\'' )
          *(*out)++ = *qr++;
        if( *qr )
        { qr++;
          while( *qr && *qr != '\'' )
            qr++;
          if( *qr == '\'' )
            *(*out)++ = ' ';
        }
      }
    }
  }
  return false;
}


/***************************************************************************************/
/* int do_mysql_num_rows(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mysql_num_rows fügt Anzahl der geänderten/selektierten Zeilen der letzten    */
/*              mysql_query ein                                                        */
/***************************************************************************************/
int do_mysql_num_rows(int pa, char **out, long n,
                      char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !mysql_query_flag )
    return true;

  *out += snprintf(*out, n, "%llu", mysql_affected_rows(&mh));
  return false;
}


/***************************************************************************************/
/* int do_mysql_store_ins(int pa, char **out, long n,                                  */
/*                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)*/
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     do_mysql_store_ins Datei als blob in Datenbank speichern - neuen Datensatz      */
/***************************************************************************************/
int do_mysql_store_ins(int pa, char **out, long n,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *puffer, *p;
  long nb, i;
  int hd_in, ret;
  my_ulonglong newid;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#endif

  LOG(1, "u2w_mysql_store_ins, Datei: %s, Tabelle: %s, Feld: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2]);

  if( !mysql_connect_flag )
    return true;

  newid = 0;
  ret = false;
  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_mysql_store_ins, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_mysql_store_ins, grosse Datei.\n");
        if( NULL == (puffer = malloc(MYSQL_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = puffer + snprintf(puffer, MYSQL_PUFFERSIZE, "insert into %s set %s='",
                 prg_pars[1], prg_pars[2]);
        LOG(9, "u2w_mysql_store_ins, grosse Datei vor while.\n");
        while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
        { for( i = 0; i < nb; i++ )
          { switch( zeile[i] )
            { case '\0': *p++ = '\\';
                         *p++ = '0';
                         break;
              case '\\':
              case '\'': *p++ = '\\';
              default:   *p++ = zeile[i];
            }
            if( p-puffer >= MYSQL_PUFFERSIZE-100 )
            { LOG(9, "u2w_mysql_store_ins, grosse Date p-puffer >= 9999900 ...\n");
              if( !newid )
                strcpyn(p, "'", MYSQL_PUFFERSIZE - (p-puffer));
              else
                snprintf(p, MYSQL_PUFFERSIZE - (p-puffer), "') where ID='%llu'",
                         newid);
              LOG(9, "u2w_mysql_store_ins, grosse Datei vor mysql_query.\n");
              if( mysql_query(&mh, puffer) )
              { if( mysql_error_log_flag )
                  logging("mysql_query error: %s.\n", mysql_error(&mh));
                close(hd_in);
                free(puffer);
#ifdef WEBSERVER
                keepalive_flag = false;
#endif
                return true;
              }
              if( !newid )
                newid = mysql_insert_id(&mh);
              LOG(9, "u2w_mysql_store_ins, grosse Datei nach mysql_query.\n");
              p = puffer + snprintf(puffer, MYSQL_PUFFERSIZE,
                                    "update %s set %s=concat(%s,'",
                                    prg_pars[1], prg_pars[2], prg_pars[2]);
            }
          }
        }
        LOG(7, "u2w_mysql_store_ins, grosse Datei, newid: %llu.\n", newid);
        if( !newid )
          strcpyn(p, "'", MYSQL_PUFFERSIZE - (p-puffer));
        else
          snprintf(p, MYSQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        if( mysql_query(&mh, puffer) )
        { if( mysql_error_log_flag )
            logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), puffer);
          free(puffer);
          close(hd_in);
#ifdef WEBSERVER
          keepalive_flag = false;
#endif
          return true;
        }
        if( !newid )
          newid = mysql_insert_id(&mh);
        free(puffer);
      }
      else
      { LOG(5, "u2w_mysql_store_ins, kleine Datei.\n");
        if( NULL != (puffer = malloc(100+2*stat_buf.st_size)) )
        { p = puffer + sprintf(puffer, "insert into %s set %s='", prg_pars[1],
                               prg_pars[2]);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { for( i = 0; i < nb; i++ )
            { switch( zeile[i] )
              { case '\0': *p++ = '\\';
                           *p++ = '0';
                           break;
                case '\\':
                case '\'': *p++ = '\\';
                default:   *p++ = zeile[i];
              }
            }
          }
          strcpy(p, "'");
          LOG(100, "mysql_store_ins, puffer: %s.\n", puffer);
          if( mysql_query(&mh, puffer) )
          { if( mysql_error_log_flag )
              logging("mysql_query error: %s.\n", mysql_error(&mh));
#ifdef WEBSERVER
            keepalive_flag = false;
#endif
            ret = true;
          }
          free(puffer);
          newid = mysql_insert_id(&mh);
        }
        else
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          ret = true;
        }
      }
      close(hd_in);
    }
  }
  if( !ret )
    *out += snprintf(*out, n, "%llu", newid);
  return ret;
}


/***************************************************************************************/
/* int mysql2s(char *query, char **o, long n, int error_flag,                          */
/*             char *ssep, char *zsep, int quote)                                      */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             return     : true bei kein Wert                                         */
/*     mysql2s fuehrt query aus und schreibt die Ausgabe nach o                        */
/***************************************************************************************/
int mysql2s(char *query, char **o, long n, int error_flag, char *ssep, char *zsep,
            int quote)
{ MYSQL_RES *mres;
  MYSQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  char *oo;
  int i, ret;
  long nz;

  LOG(1, "mysql2s, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");
  LOG(5, "mysql2s, quote: %d.\n", quote);

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = " ";

  ret = false;

  oo = *o;
  LOG(3, "mysql2s, query: %s.\n", query);

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( MYSQL_QUERY(&mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), query);
    if( error_flag && mysql_error_out_flag )
      *o += snprintf(*o, n, "mysql_query error: %s, query: %s.",
                     mysql_error(&mh), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    **o = '\0';
    return true;
  }
  else
  { mres = mysql_store_result(&mh);
    if( mres )
    { nf = mysql_num_fields(mres);
      LOG(11, "mysql2s, nf: %d.\n", nf);
      nz = 0;
      while( (mrow = mysql_fetch_row(mres) ) )
      { if( nz++ )
          strcpyn_z(o, zsep, n - (*o-oo));
        lengths = mysql_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            strqcpyn_z(o, ssep, n - (*o-oo), qf_strings[quote]);
          if( lengths[i] && mrow[i] )
          { strqcpynm_z(o, mrow[i], n - (*o-oo), lengths[i], qf_strings[quote]);
          }
        }
      }
      mysql_free_result(mres);
    }
    else
    { if( error_flag && mysql_errno(&mh) )
        *o += snprintf(*o, n - (*o-oo), "Error: %s.", mysql_error(&mh));
      ret = true;
    }
  }
  **o = '\0';

  return ret;                                 /* Anzahl der Zeichen vom Programm    */
}


/***************************************************************************************/
/* int mysql2net(char *query, int error_flag, char *ssep, char *zsep, int htmlflag)    */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int htmlflag: 1 - HTML-Sonderzeichen umwandeln                          */
/*             return     : true bei kein Wert                                         */
/*     mysql2net fuehrt query aus und sendet Ausgabe an Browser                        */
/***************************************************************************************/
int mysql2net(char *query, int error_flag, char *ssep, char *zsep, int htmlflag)
{ char o[MAX_ZEILENLAENGE];
  MYSQL_RES *mres;
  MYSQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i, ret;
  long nz;

  LOG(1, "mysql2net, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");

  if( !zsep )
    zsep = " ";


  if( !ssep )
    ssep = " ";

  ret = false;

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( MYSQL_QUERY(&mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), query);
    if( error_flag && mysql_error_out_flag )
    { snprintf(o, MAX_ZEILENLAENGE, "mysql_query error: %s, query: %s.",
               mysql_error(&mh), query);
      dosendh(o);
    }
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { mres = mysql_store_result(&mh);
    if( mres )
    { nf = mysql_num_fields(mres);
      nz = 0;
      while( (mrow = mysql_fetch_row(mres) ) )
      { if( nz++ )
        { dosend(zsep);
        }
        lengths = mysql_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
          { dosend(ssep);
          }
          if( lengths[i] && mrow[i] )
          { LOG(9, "mysql2net, length: %lu.\n", lengths[i]);
            if( htmlflag )
              dobinsendh(mrow[i], lengths[i]);
            else
              dobinsend(mrow[i], lengths[i]);
            last_char_sended = last_non_ws_char(mrow[i], lengths[i]);
          }
        }
      }
      mysql_free_result(mres);
    }
    else
    { if( error_flag && mysql_error_out_flag && mysql_errno(&mh) )
      { snprintf(o, MAX_ZEILENLAENGE, "Error: %s.", mysql_error(&mh));
        dosendh(o);
      }
      ret = true;
    }
  }

  return ret;
}


/***************************************************************************************/
/* int mysql2net_tbl(char *query, int error_flag, int htmlflag)                        */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             int htmlflag: 1 - HTML-Sonderzeichen umwandeln                          */
/*             return     : true bei kein Wert                                         */
/*     mysql2net fuehrt query aus und sendet Ausgabe an Browser                        */
/***************************************************************************************/
int mysql2net_tbl(char *query, int error_flag, int htmlflag)
{ char o[MAX_ZEILENLAENGE];
  MYSQL_RES *mres;
  MYSQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i, ret;
  long nz;
  char trclass[MAX_LEN_CLASSNAME+10], tdclass[MAX_LEN_CLASSNAME+10];
  char *tbl_row_end, *tbl_row_start, *tbl_cell_end, *tbl_cell_start;

  LOG(1, "mysql2net_tbl, query: %s.\n", query);

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( !(tbl_row_start = parwert(S2W_TABLE_ROW_START, HP_SYSTEM_LEVEL)) )
  { if( u2w_mode == U2W_MODE )
    { if( css_defs[CSS_TR] )
      { snprintf(trclass, MAX_LEN_CLASSNAME+10, " <tr class=\"%s\">", css_defs[CSS_TR]);
        tbl_row_start = trclass;
      }
      else
        tbl_row_start = "<tr>";
    }
    else
      tbl_row_start = "";
  }
  if( !(tbl_row_end = parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
  { if( u2w_mode == U2W_MODE )
      tbl_row_end = "</tr>\n";
    else
      tbl_row_end = "\n";
  }
  if( !(tbl_cell_start = parwert(S2W_TABLE_CELL_START, HP_SYSTEM_LEVEL)) )
  { if( u2w_mode == U2W_MODE )
    { if( css_defs[CSS_TD] )
      { snprintf(tdclass, MAX_LEN_CLASSNAME+10, "<td class=\"%s\">", css_defs[CSS_TD]);
        tbl_cell_start = tdclass;
      }
      else
        tbl_cell_start = NULL;
    }
    else
      tbl_cell_start = " ";
  }
  if( !(tbl_cell_end = parwert(S2W_TABLE_CELL_END, HP_SYSTEM_LEVEL)) )
  { if( u2w_mode == U2W_MODE )
      tbl_cell_end = "</td>";
    else
      tbl_cell_end = "";
  }

  ret = false;

  if( MYSQL_QUERY(&mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), query);
    if( error_flag && mysql_error_out_flag )
    { snprintf(o, MAX_ZEILENLAENGE, "mysql_query error: %s, query: %s.",
               mysql_error(&mh), query);
      dosendh(o);
    }
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { mres = mysql_store_result(&mh);
    if( mres )
    { nf = mysql_num_fields(mres);
      nz = 0;
      LOG(22, "mysql2net_tbl, akt_tablecols: %d, tablecols: %d, tablelevel: %d, table_aligns: %s\n",
          akt_tablecols, tablecols, tablelevel,
          table_aligns[tablelevel-1] ? "NULL" : table_aligns[tablelevel-1]);
      while( (mrow = mysql_fetch_row(mres) ) )
      { if( nz++ )
        { akt_tablecols = 0;
          dosend(tbl_row_end);
          if( *tbl_row_start )
            dosend(tbl_row_start);
          if( tbl_cell_start )
            dosend(tbl_cell_start);
          else
          { if( akt_tablecols < aligntablecols[tablelevel-1] )
            { if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'r' )
                dosend("<td class=\"u2wtaright\">");
              else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'c' )
                dosend("<td class=\"u2wtacenter\">");
              else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'L' )
                dosend("<td class=\"u2wtaleft\">");
              else
                dosend("<td>");
            }
            else
              dosend("<td>");
          }
          akt_tablecols++;
        }
        lengths = mysql_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
          { if( tbl_cell_start )
              dosend(tbl_cell_start);
            else
            { if( akt_tablecols < aligntablecols[tablelevel-1] )
              { if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'r' )
                  dosend("<td class=\"u2wtaright\">");
                else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'c' )
                  dosend("<td class=\"u2wtacenter\">");
                else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'L' )
                  dosend("<td class=\"u2wtaleft\">");
                else
                  dosend("<td>");
              }
              else
                dosend("<td>");
            }
            akt_tablecols++;
          }
          if( lengths[i] && mrow[i] )
          { LOG(9, "mysql2net_tbl, length: %lu.\n", lengths[i]);
            if( htmlflag )
              dobinsendh(mrow[i], lengths[i]);
            else
              dobinsend(mrow[i], lengths[i]);
          }
          if( i < nf - 1 )
          { dosend(tbl_cell_end);
          }
        }
      }
      mysql_free_result(mres);
    }
    else
    { if( error_flag && mysql_error_out_flag && mysql_errno(&mh) )
      { snprintf(o, MAX_ZEILENLAENGE, "Error: %s.", mysql_error(&mh));
        dosendh(o);
      }
      ret = true;
    }
  }

  return ret;
}


/***************************************************************************************/
/* int mysql2dat(char *query, char *path, char *ssep, char *zsep)                      */
/*             char *query: SQL-query                                                  */
/*             char *path: Datei für die Ausgabe                                       */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zend  : Endezeichen einzelner Zeilen                              */
/*             return     : true bei kein Wert                                         */
/*     mysql2dat fuehrt query aus und schreibt Ausgabe in Datei path                   */
/***************************************************************************************/
int mysql2dat(char *query, char *path, char *ssep, char *zend)
{ MYSQL_RES *mres;
  MYSQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i, ret;
  int hd_out;

  LOG(1, "mysql2dat, query: %s, path: %s, ssep: %s, zend: %s.\n", query, path,
      ssep ? ssep : "NULL", zend ? zend : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zend )
    zend = "\n";

  ret = false;

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( MYSQL_QUERY(&mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { if( (mres = mysql_store_result(&mh)) )
    { if( 0 <= (hd_out = open64(path, O_WRONLY | O_CREAT | O_TRUNC, 00600)) ) 
      { nf = mysql_num_fields(mres);
        while( !ret && (mrow = mysql_fetch_row(mres) ) )
        { lengths = mysql_fetch_lengths(mres);
          for( i = 0; i < nf; i++ )
          { if( i )
            { if( strlen(ssep) < write(hd_out, ssep, strlen(ssep)) )
              { ret = true;
                break;
              }
            }
            if( lengths[i] && mrow[i] )
            { LOG(99, "mysql2dat, length: %lu.\n", lengths[i]);
              if( lengths[i] < write(hd_out, mrow[i], lengths[i]) )
              { ret = true;
                break;
              }
            }
          }
          if( strlen(zend) < write(hd_out, zend, strlen(zend)) )
          { ret = true;
            break;
          }
        }
        close(hd_out);
      }
      else
        ret = true;
      mysql_free_result(mres);
      return ret;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* int mysqlinsert2s(char *query, char **o, long n, int error_flag)                    */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             return     : true bei Fehler                                            */
/*     mysqlinsert2s führt query aus und schreibt eingefügte ID nach o                 */
/***************************************************************************************/
int mysqlinsert2s(char *query, char **o, long n, int error_flag)
{
  LOG(1, "mysqlinsert2s, query: %s.\n", query);

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( MYSQL_QUERY(&mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), query);
    if( error_flag && mysql_error_out_flag )
      *o += snprintf(*o, n, "mysql_query error: %s, query: %s.",
                     mysql_error(&mh), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { LOG(5, "mysqlinsert2s, mysql_insert_id: %llu.\n", mysql_insert_id(&mh));
    *o += snprintf(*o, n, "%llu", mysql_insert_id(&mh));
  }
  return false;
}

#ifdef NEW_VERSION
/***************************************************************************************/
/* int mysql2p(char *query, char *ssep, char *zsep, char *p)                           */
/*             char *query: SQL-query                                                  */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             char *p     : Kommando, an das die Ausgabe geschickt wird               */
/*             return     : true bei kein Wert                                         */
/*     mysql2p fuehrt query aus und schreibt die Ausgabe in einer pipe an *p           */
/***************************************************************************************/
int mysql2p(char *query, char *ssep, char *zsep, char *p)
{ MYSQL_RES *mres;
  MYSQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int ret;

  LOG(1, "mysql2p, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = "\n";

  ret = false;

  if( !mysql_connect_flag )                      /* Aktive MySQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( MYSQL_QUERY(&mh, query) )
  { if( mysql_error_log_flag )
      logging("mysql_query error: %s.\nquery: %s.\n", mysql_error(&mh), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { if( (mres = mysql_store_result(&mh)) )
    { nf = mysql_num_fields(mres);
      nz = 0;
      while( (mrow = mysql_fetch_row(mres) ) )
      { if( nz++ )
          strcpyn_z(o, zsep, n - (*o-oo));
        lengths = mysql_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            strcpyn_z(o, ssep, n - (*o-oo));
          if( lengths[i] && mrow[i] )
          { *o += snprintf(*o, n - (*o-oo), "%.*s",
                           (int)lengths[i], mrow[i]);
          }
        }
      }
      mysql_free_result(mres);
    }
    else
    { ret = true;
    }
  }

  return ret;
}
#endif
#endif

