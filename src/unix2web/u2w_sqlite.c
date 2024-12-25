/**********************************************/
/* File: u2w_sqlite.c                         */
/* Funktionen zum Ansprechen von sqlite3      */
/* timestamp: 2021-02-13 18:50:20             */
/**********************************************/

#include "u2w.h"


/* Anfang globale Variablen */
#ifdef SQLITE3
int sqlite3_error_log_flag = false;   /* true: sql3-Error ins Logfile/stderr           */
int sqlite3_error_out_flag = false;   /* true: sql3-Error an stdout / HTML             */
int sqlite3_open_flag = false;
sqlite3 *sql3db;
#endif
/* Ende globale Variablen */


#ifdef SQLITE3

#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
#define stat64(a,b) stat(a,b)
#define open64(...) open(__VA_ARGS__)
#endif
#else
#define stat64(a,b) stat(a,b)
#define open64(...) open(__VA_ARGS__)
#endif

int sql3_write_flag = false;
int sql3_id_flag = false;

#define MAX_P_QUERIES 5
sqlite3_stmt *sql3res[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
int sql3_res_flag[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
int sql3_line_flag[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
int sql3_next_value[MAX_ANZ_INCLUDE+MAX_P_QUERIES];

#define SQLITEABORT 0
#define SQLITECONTINUE 1
#define SQLITEMAXATTEMPTS 10


/***************************************************************************************/
/* int sql3_busy_handler(void *data, int attempt)                                      */
/*        void *data:                                                                  */
/*        int attempt: Versuchszähler                                                  */
/***************************************************************************************/
int sql3_busy_handler(void *data, int attempt)
{
  if( attempt < SQLITEMAXATTEMPTS )
  { sqlite3_sleep(10);
    return SQLITECONTINUE;
  }
  return SQLITEABORT;
}

/***************************************************************************************/
/* void sql3_init_flags(void)                                                          */
/*      sql3_init_flags, sql3-Flags initialisieren                                     */
/***************************************************************************************/
void sql3_init_flags(void)
{ int i;

  LOG(11, "sql3_init_flags.\n");

  for( i = 0; i < MAX_ANZ_INCLUDE+MAX_P_QUERIES; i++ )
  { sql3_res_flag[i] = 0;
  }
}


/***************************************************************************************/
/* void sql3_free_one_res(int i)                                                       */
/*      sql3_free_one_res Speicher der sql3res freigeben                               */
/***************************************************************************************/
void sql3_free_one_res(int i)
{
  LOG(1, "sql3_free_one_res, i: %d.\n", i);

  if( sql3_res_flag[i] )
  { sqlite3_finalize(sql3res[i]);
    sql3_res_flag[i] = 0;
  }
  LOG(2, "/sql3_free_one_res.\n");
}


/***************************************************************************************/
/* void sql3_free_res(void)                                                            */
/*      sql3_free_res Speicher der sql3res freigeben                                   */
/***************************************************************************************/
void sql3_free_res(void)
{ int i;

  LOG(1, "sql3_free_res.\n");

  for( i = MAX_ANZ_INCLUDE; i < MAX_ANZ_INCLUDE+MAX_P_QUERIES; i++ )
    sql3_free_one_res(i);
}


/***************************************************************************************/
/* short sql3_open(char *dbpath, int errorflag, int flags)                             */
/*             char *dbpath: Pfad zur slite3 DB Datei                                  */
/*             int errorflag: bei true werden Fehlermeldungen auf stderr ausgegeben    */
/*             int flags: open flags to sqlite3_open_v2                                */
/*             return: true bei Fehler                                                 */
/*     sql3_open öffnet eine sqlite3 DB Datei                                          */
/***************************************************************************************/
short sql3_open(char *path, int errorflag, int flags)
{ static char cursql3path[MAX_PATH_LEN];

  LOG(1, "sql3_open, path: %s.\n", path);

  if( sqlite3_open_flag )
  { if( !strcmp(path, cursql3path) )
      return false;
    else
    { sql3_free_res();
      sqlite3_close(sql3db);
      sql3_write_flag = false;
    }
  }

  if( SQLITE_OK != sqlite3_open_v2(path, &sql3db, flags, NULL) )
  { if( errorflag || sqlite3_error_log_flag )
      logging("Failed to open database %s - Error: %s.\n",
              path, sqlite3_errmsg(sql3db));
    sqlite3_close(sql3db);
    sqlite3_open_flag = false;
    return true;
  }

  sqlite3_busy_handler(sql3db, sql3_busy_handler, NULL);

  strcpyn(cursql3path, path, MAX_PATH_LEN);
  sqlite3_open_flag = true;
  return false;
}


/***************************************************************************************/
/* short u2w_sql3_open(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_open öffnet eine sqlite3 Datei                                         */
/***************************************************************************************/
short u2w_sql3_open(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int flags;

  if( pa & P2 )
  { flags = 0;
    if( strchr(prg_pars[1][0], 'R') || strchr(prg_pars[1][0], 'r') )
      flags = SQLITE_OPEN_READONLY;
    if( strchr(prg_pars[1][0], 'U') || strchr(prg_pars[1][0], 'u') )
      flags |= SQLITE_OPEN_URI;
    if( strchr(prg_pars[1][0], 'M') || strchr(prg_pars[1][0], 'm') )
      flags |= SQLITE_OPEN_MEMORY;
    if( strchr(prg_pars[1][0], 'C') || strchr(prg_pars[1][0], 'c') )
      flags |= SQLITE_OPEN_CREATE;
    if( strchr(prg_pars[1][0], 'W') || strchr(prg_pars[1][0], 'w') )
      flags |= SQLITE_OPEN_READWRITE;
    if( !flags )
      flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  }
  else
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  return sql3_open(prg_pars[0], false, flags);
}


/***************************************************************************************/
/* short u2w_sql3_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_query Stellt Anfrage an sqlite3-Datenbank, Ergebnisse können dann mit  */
/*                 u2w_sql3_next_line gelesen und mit sql3_get_value gelesen werden    */
/***************************************************************************************/
short u2w_sql3_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_sql3_query.\n");

  if( !sqlite3_open_flag )
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

  if( sql3_res_flag[cn] )
  { sql3_res_flag[cn] = sql3_line_flag[cn] = false;
    sqlite3_finalize(sql3res[cn]);
  }

  LOG(3, "u2w_sql3_query, query: %s.\n", prg_pars[0]);

  if( SQLITE_OK != sqlite3_prepare_v2(sql3db, prg_pars[0], -1, &sql3res[cn], 0) )
  { if( sqlite3_error_log_flag )
      logging("sqlite3_query error: %s\nquery: %s.\n", sqlite3_errmsg(sql3db), prg_pars[0]);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { sql3_res_flag[cn] = true;
    return false;
  }
}


/***************************************************************************************/
/* short u2w_sql3_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_write Stellt Anfrage an sql3-Datenbank, für insert und update          */
/***************************************************************************************/
short u2w_sql3_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *err_msg = NULL;

  LOG(1, "u2w_sql3_write, query: %s.\n", prg_pars[0]);

  if( !sqlite3_open_flag )
    return true;

  if( SQLITE_OK != sqlite3_exec(sql3db, prg_pars[0], 0, 0, &err_msg) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s\nquery: %s.\n", err_msg, prg_pars[0]);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { sql3_write_flag = true;
    sql3_id_flag = true;
    return false;
  }
}


/***************************************************************************************/
/* short u2w_sql3_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_test Stellt Anfrage an sql3-Datenbank, und liefert nur Wahrheitswert   */
/***************************************************************************************/
short u2w_sql3_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *err_msg = NULL;

  LOG(1, "u2w_sql3_test, query: %s.\n", prg_pars[0]);

  if( !sqlite3_open_flag )
    return true;

  if( SQLITE_OK != sqlite3_exec(sql3db, prg_pars[0], 0, 0, &err_msg) )
    return true;
  else
  { sql3_id_flag = true;
    return false;
  }
}


#define SQL3_STORE_MODE_APPEND 1
#define SQL3_BUFFERSIZE 10000000

/***************************************************************************************/
/* short u2w_sql3_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_store Datei als blob in Datenbank speichern - Update                   */
/***************************************************************************************/
short u2w_sql3_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *buffer, *p;
  long nb, i;
  int hd_in, ret;
  int mode = 0;
  char *err_msg = NULL;
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#else
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#endif
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#endif

  LOG(1, "u2w_sql3_store, Datei: %s, Tabelle: %s, Feld: %s, ID: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3]);

  if( !sqlite3_open_flag )
    return true;

  if( pa & P5 )
  { switch( prg_pars[4][0] )
    { case 'a': mode = SQL3_STORE_MODE_APPEND | mode ;
                break;
    }
  }

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_sql3_store, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_sql3_store, grosse Datei.\n");
        if( NULL == (buffer = malloc(SQL3_BUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = buffer;
        if( mode & SQL3_STORE_MODE_APPEND )
        { p += snprintf(p, SQL3_BUFFERSIZE, "update %s set %s=%s||'",
                        prg_pars[1], prg_pars[2], prg_pars[2]);
        }
        else
        { p += snprintf(p, SQL3_BUFFERSIZE, "update %s set %s='",
                        prg_pars[1], prg_pars[2]);
        }
        LOG(9, "u2w_sql3_store, grosse Datei vor while.\n");
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
            if( p-buffer >= SQL3_BUFFERSIZE-100 )
            { LOG(9, "u2w_sql3_store, grosse Date p-buffer >= 9999900 ...\n");
              snprintf(p, SQL3_BUFFERSIZE - (p-buffer), "' where ID='%s'",
                       prg_pars[3]);
              LOG(9, "u2w_sql3_store, grosse Datei vor sql3_query.\n");
              if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
              { if( sqlite3_error_log_flag )
                  logging("sql3_query error: %s.\n", err_msg);
                close(hd_in);
                free(buffer);
#ifdef WEBSERVER
                keepalive_flag = false;
#endif
                return true;
              }
              LOG(9, "u2w_sql3_store, grosse Datei nach sql3_query.\n");
              p = buffer + snprintf(buffer, SQL3_BUFFERSIZE, "update %s set %s=%s||'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
            }
          }
        }
        snprintf(p, SQL3_BUFFERSIZE - (p-buffer), "' where ID='%s'", prg_pars[3]);
        if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
        { if( sqlite3_error_log_flag )
            logging("sql3_query error: %s.\n", err_msg);
          free(buffer);
          close(hd_in);
#ifdef WEBSERVER
          keepalive_flag = false;
#endif
          return true;
        }
        free(buffer);
        close(hd_in);
        return false;
      }
      else
      { LOG(5, "u2w_sql3_store, kleine Datei.\n");
        if( NULL != (buffer = malloc(100+2*stat_buf.st_size)) )
        { LOG(11, "u2w_sql3_store, kleine Datei, buffer allokiert.\n");
          if( mode & SQL3_STORE_MODE_APPEND )
            p = buffer + sprintf(buffer, "update %s set %s=%s||'",
                                 prg_pars[1], prg_pars[2], prg_pars[2]);
          else
            p = buffer + sprintf(buffer, "update %s set %s='", prg_pars[1], prg_pars[2]);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { LOG(50, "u2w_sql3_store, kleine Datei, nb: %d.\n", nb);
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
          sprintf(p, "' where ID='%s'", prg_pars[3]);
          LOG(100, "sql3_store, buffer: %s.\n", buffer);
          if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
          { if( sqlite3_error_log_flag )
              logging("sql3_query error: %s.\n", err_msg);
#ifdef WEBSERVER
            keepalive_flag = false;
#endif
            ret = true;
          }
          else
            ret = false;
          free(buffer);
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
/* short u2w_sql3_storev(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_storev Datei als blob in @-Variable speichern                          */
/***************************************************************************************/
short u2w_sql3_storev(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *buffer, *p;
  long nb, i;
  int hd_in, ret;
  char *err_msg = NULL;
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#else
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#endif
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#endif

  LOG(1, "u2w_sql3_storev, Datei: %s, Var: %s.\n",
      prg_pars[0], prg_pars[1]);

  if( !sqlite3_open_flag )
    return true;

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_sql3_store, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_sql3_storev, grosse Datei.\n");
        if( NULL == (buffer = malloc(SQL3_BUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = buffer;
        p += snprintf(p, SQL3_BUFFERSIZE, "set %s='", prg_pars[1]);
        LOG(9, "u2w_sql3_storev, grosse Datei vor while.\n");
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
            if( p-buffer >= SQL3_BUFFERSIZE-100 )
            { LOG(9, "u2w_sql3_storev, grosse Date p-buffer >= 9999900 ...\n");
              strcpyn(p, "'", SQL3_BUFFERSIZE - (p-buffer));
              LOG(9, "u2w_sql3_storev, grosse Datei vor sql3_query.\n");
              if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
              { if( sqlite3_error_log_flag )
                  logging("sql3_query error: %s.\n", err_msg);
                close(hd_in);
                free(buffer);
#ifdef WEBSERVER
                keepalive_flag = false;
#endif
                return true;
              }
              LOG(9, "u2w_sql3_storev, grosse Datei nach sql3_query.\n");
              p = buffer + snprintf(buffer, SQL3_BUFFERSIZE, "set %s=%s||'",
                                    prg_pars[1], prg_pars[1]);
            }
          }
        }
        strcpyn(p, "'", SQL3_BUFFERSIZE - (p-buffer));
        if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
        { if( sqlite3_error_log_flag )
            logging("sql3_query error: %s.\n", err_msg);
          free(buffer);
          close(hd_in);
#ifdef WEBSERVER
          keepalive_flag = false;
#endif
          return true;
        }
        free(buffer);
        close(hd_in);
        return false;
      }
      else
      { LOG(5, "u2w_sql3_store, kleine Datei.\n");
        if( NULL != (buffer = malloc(100+2*stat_buf.st_size)) )
        { LOG(11, "u2w_sql3_store, kleine Datei, buffer allokiert.\n");
          p = buffer + sprintf(buffer, "set %s='", prg_pars[1]);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { LOG(50, "u2w_sql3_store, kleine Datei, nb: %d.\n", nb);
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
          LOG(100, "sql3_storev, buffer: %s.\n", buffer);
          if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
          { if( sqlite3_error_log_flag )
              logging("sql3_query error: %s.\n", err_msg);
#ifdef WEBSERVER
            keepalive_flag = false;
#endif
            ret = true;
          }
          else
            ret = false;
          free(buffer);
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
/* short u2w_sql3_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_out Query ausführen und Ergebnis in Datei schreiben                    */
/***************************************************************************************/
short u2w_sql3_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(3, "u2w_sql3_out, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  return sql3_2dat(prg_pars[0], prg_pars[1], pa & P3 ? prg_pars[2] : NULL,
                   pa & P4 ? prg_pars[3] : NULL);
}


/***************************************************************************************/
/* short u2w_sql3_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])  */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_get_line bereitet eine Zeile zum lesen vor                             */
/***************************************************************************************/
short u2w_sql3_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_sql3_get_line.\n");

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

  if( sql3_res_flag[cn] )
  { if( SQLITE_ROW == sqlite3_step(sql3res[cn]) )
    { sql3_line_flag[cn] = true;
      sql3_next_value[cn] = 0;
      return false;
    }
    else
    { sql3_line_flag[cn] = false;
      return true;
    }
  }
  sql3_line_flag[cn] = false;
  return true;
}


/***************************************************************************************/
/* short do_sql3_list_get_line(int *listlen,                                           */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_list_get_line Eine Zeile in Liste                                       */
/***************************************************************************************/
short do_sql3_list_get_line(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn, i;

  LOG(1, "do_sql3_list_get_line, listlen: %d.\n", *listlen);

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

  if( sql3_res_flag[cn] )
  { if( SQLITE_ROW == sqlite3_step(sql3res[cn]) )
    { sql3_line_flag[cn] = true;
      sql3_next_value[cn] = 0;
      for( i = 0; i < sqlite3_data_count(sql3res[cn]) && *listlen < MAX_LIST_LEN; i++ )
      { if( sqlite3_column_bytes(sql3res[cn], i) )
          strcpynm(list_pars[(*listlen)++], sqlite3_column_text(sql3res[cn], i),
                   MAX_LEN_LIST_PARS, sqlite3_column_bytes(sql3res[cn], i));
        else
          list_pars[(*listlen)++][0] = '\0';
      }
      LOG(17, "/do_sql3_list_get_line, list_pars[0]: %s.\n", list_pars[0]);
      return false;
    }
    else
    { sql3_line_flag[cn] = false;
      *listlen = -1;
      return true;
    }
  }
  sql3_line_flag[cn] = false;
  *listlen = -1;
  return true;
}


/***************************************************************************************/
/* short u2w_sql3_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql3_isvalue testet, ob noch ein Wert für sql3readvalue vorliegt            */
/***************************************************************************************/
short u2w_sql3_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_sql3_isvalue.\n");

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

  return !sql3_line_flag[cn] || sql3_next_value[cn] >= sqlite3_data_count(sql3res[cn]);
}


/***************************************************************************************/
/* short do_sql3_writeline(int pa, char **out, long n,                                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_sql3_writeline fügt Zeile des letzten sql3_getline ein                      */
/***************************************************************************************/
short do_sql3_writeline(int pa, char **out, long n,
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

  if( sql3_line_flag[cn] )
  { if( out )
    { oo = *out;
      for( i = 0; i < sqlite3_data_count(sql3res[cn]); i++ )
      { if( i )
          strqcpyn_z(out, prg_pars[1], n - (*out-oo), qf_strings[quote]);
        if( sqlite3_column_bytes(sql3res[cn], i) )
          strqcpynm_z(out, sqlite3_column_text(sql3res[cn], i), n -(*out-oo),
                      sqlite3_column_bytes(sql3res[cn], i), qf_strings[quote]);
      }
    }
    else
    { for( i = 0; i < sqlite3_data_count(sql3res[cn]); i++ )
      { if( i )
          dosendh(prg_pars[1]);
        if( sqlite3_column_bytes(sql3res[cn], i) )
        { dobinsendh(sqlite3_column_text(sql3res[cn], i),
                     sqlite3_column_bytes(sql3res[cn], i));
          last_char_sended = last_non_ws_char(sqlite3_column_text(sql3res[cn], i),
                                              sqlite3_column_bytes(sql3res[cn], i));
        }
      }
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* short do_sql3_readwriteline(int pa, char **out, long n,                             */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_readwriteline fügt eine Zeile der letzten sql3_query ein                */
/***************************************************************************************/
short do_sql3_readwriteline(int pa, char **out, long n,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !u2w_sql3_get_line(pa, prg_pars) )
    return do_sql3_writeline(pa, out, n, prg_pars, quote);
  else
    return true;
}


/***************************************************************************************/
/* short do_sql3_num_fields(int pa, char **out, long n,                                */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_num_fields ergibt Anzahl Spalten der sql3-Abfrage                       */
/***************************************************************************************/
short do_sql3_num_fields(int pa, char **out, long n,
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

 if( !sql3_res_flag[cn] )
    return true;
  *out += snprintf(*out, n, "%d", sqlite3_data_count(sql3res[cn]));
  return false;
}


/***************************************************************************************/
/* short do_sql3_value(int pa, char **out, long n,                                     */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_value fügt einen Wert einer Zeile ein                                   */
/***************************************************************************************/
short do_sql3_value(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_sql3_value, qoute: %d\n", quote);

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

  LOG(3, "do_sql3_value, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( sql3_line_flag[cn] && isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_sql3_value, i: %d, sqlite3_data_count(sql3res[%d]): %d.\n",
        i, cn, sqlite3_data_count(sql3res[cn]));
    if( i < sqlite3_data_count(sql3res[cn]) )
    { LOG(50, "do_sql3_value, sqliterow[%d][%d]: %s\n", cn, i,
          sqlite3_column_text(sql3res[cn], i));
      if( sqlite3_column_bytes(sql3res[cn], i) )
      { if( out )
        { if( quote == QF_HTML )
            code_html_nm(out, sqlite3_column_text(sql3res[cn], i), n,
                         sqlite3_column_bytes(sql3res[cn], i));
          else
            strqcpynm_z(out, sqlite3_column_text(sql3res[cn], i), n,
                        sqlite3_column_bytes(sql3res[cn], i), qf_strings[quote]);
        }
        else
        { dobinsendh(sqlite3_column_text(sql3res[cn], i),
                     sqlite3_column_bytes(sql3res[cn], i));
          last_char_sended = last_non_ws_char(sqlite3_column_text(sql3res[cn], i),
                                              sqlite3_column_bytes(sql3res[cn], i));
        }
      }
    }
    LOG(1, "/do_sql3_value\n");
    return false;
  }
  LOG(1, "/do_sql3_value - ERROR\n");
  return true;
}


/***************************************************************************************/
/* short do_sql3_rawvalue(int pa, char **out, long n,                                  */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_rawvalue fügt einen Wert einer Zeile ein ohne HTML Konvertierung        */
/***************************************************************************************/
short do_sql3_rawvalue(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_sql3_rawvalue\n");

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

  LOG(3, "do_sql3_rawvalue, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_rawsql3_value, i: %d, sqlite3_data_count(sql3res[%s]): %d.\n",
        i, cn, sqlite3_data_count(sql3res[cn]));
    if( i < sqlite3_data_count(sql3res[cn]) )
    { if( sqlite3_column_bytes(sql3res[cn], i) )
      { if( out )
          strqcpynm_z(out, sqlite3_column_text(sql3res[cn], i), n,
                      sqlite3_column_bytes(sql3res[cn], i), qf_strings[quote]); 
        else
        { dobinsend(sqlite3_column_text(sql3res[cn], i), sqlite3_column_bytes(sql3res[cn], i));
          last_char_sended = last_non_ws_char(sqlite3_column_text(sql3res[cn], i),
                                              sqlite3_column_bytes(sql3res[cn], i));
        }
      }
    }
    LOG(1, "/do_sql3_rawvalue\n");
    return false;
  }
  LOG(1, "/do_sql3_rawvalue - ERROR\n");
  return true;
}


/***************************************************************************************/
/* short do_sql3_read_value(int pa, char **out, long n,                                */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_read_value fügt näcshten Wert einer Zeile ein                           */
/***************************************************************************************/
short do_sql3_read_value(int pa, char **out, long n,
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

  if( !sql3_line_flag[cn] || sql3_next_value[cn] >= sqlite3_data_count(sql3res[cn]) )
    return true;

  if( sqlite3_column_bytes(sql3res[cn], sql3_next_value[cn]) )
  { if( out )
      strqcpynm_z(out, sqlite3_column_text(sql3res[cn], sql3_next_value[cn]), n,
                  sqlite3_column_bytes(sql3res[cn], sql3_next_value[cn]), qf_strings[quote]); 
    else
    { dobinsendh(sqlite3_column_text(sql3res[cn], sql3_next_value[cn]),
                 sqlite3_column_bytes(sql3res[cn], sql3_next_value[cn]));
      last_char_sended = last_non_ws_char(sqlite3_column_text(sql3res[cn], sql3_next_value[cn]),
                                          sqlite3_column_bytes(sql3res[cn], sql3_next_value[cn]));
    }
  }

  sql3_next_value[cn]++;
  return false;
}


/***************************************************************************************/
/* short do_sql3_id(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3 fügt letzte ID (nach insert) ein                                        */
/***************************************************************************************/
short do_sql3_id(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( !sql3_id_flag )
    return true;
  *out += snprintf(*out, n, "%llu", sqlite3_last_insert_rowid(sql3db));
  return false;
}


/***************************************************************************************/
/* short do_sql3_read_send(int pa,                                                     */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_read_tc Query ausführen und Ergebnis einfügen tablecols beachten        */
/***************************************************************************************/
short do_sql3_read_send(int pa,
                     char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_sql3_read_send, pa: %d, %s, %s, quote: %d, last_char_sended: %d.\n", pa,
      pa & P2 ? prg_pars[1] : "", pa & P3 ? prg_pars[2] : "", quote, (int)last_char_sended);

  if( (pa & (P2|P3)) || !tablecols || u2w_mode != U2W_MODE )
    return sql3_2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, true);
  else
    return sql3_2net_tbl(prg_pars[0], true, true);
}


/***************************************************************************************/
/* short do_sql3_read(int pa, char **out, long n,                                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_read_ntc Query ausführen und Ergebnis einfügen tablecols nicht beachten */
/***************************************************************************************/
short do_sql3_read(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_sql3_read, pa: %d, %s, %s, quote: %d.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "", quote);

  if( pa & P4 )
  { if( atol(prg_pars[3]) < n )
      n = atol(prg_pars[3]);
  }

  if( out )
    return sql3_2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, quote);
  else
    return sql3_2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, true);
}


/***************************************************************************************/
/* short do_sql3_read_raw_send(int pa,                                                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_read_raw_tc Query ausführen und Ergebnis ohne HTML-Quoting ausgeben     */
/*                          tablecols beachten                                         */
/***************************************************************************************/
short do_sql3_read_raw_send(int pa,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_sql3_read_tc, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( (pa & (P2|P3)) || !tablecols || u2w_mode != U2W_MODE )
    return sql3_2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, false);
  else
    return sql3_2net_tbl(prg_pars[0], true, false);
}


/***************************************************************************************/
/* short do_sql3_read_raw(int pa, char **out, long n,                                  */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_read_raw_ntc Query ausführen und Ergebnis ohne HTML-Quoting ausgeben    */
/*                          tablecols nicht beachten                                   */
/***************************************************************************************/
short do_sql3_read_raw(int pa, char **out, long n,
                      char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_sql3_read, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( out )
    return sql3_2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, quote);
  else
    return sql3_2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, false);
}


/***************************************************************************************/
/* short do_sql3_list_read(int *listlen,                                               */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_list_read Query ausführen und Ergebnis in Liste                         */
/***************************************************************************************/
short do_sql3_list_read(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ sqlite3_stmt *sql3res;
  int i;

  LOG(3, "do_sql3_list_read, q: %s.\n", prg_pars[0]);

  if( !sqlite3_open_flag )                       /* Aktive sql3-Verbindung?            */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( SQLITE_OK != sqlite3_prepare_v2(sql3db, prg_pars[0], -1, &sql3res, 0) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s.\n", sqlite3_errmsg(sql3db));
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { while( SQLITE_ROW == sqlite3_step(sql3res) )
    { for( i = 0; i < sqlite3_data_count(sql3res) && *listlen < MAX_LIST_LEN; i++ )
      { if( sqlite3_column_bytes(sql3res, i) )
          strcpynm(list_pars[(*listlen)++], sqlite3_column_text(sql3res, i), MAX_LEN_LIST_PARS,
                   sqlite3_column_bytes(sql3res, i));
        else
          list_pars[(*listlen)++][0] = '\0';
      }
    }
    sqlite3_finalize(sql3res);
    return false;
  }
}


/***************************************************************************************/
/* short do_sql3_idvalue(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_idvalue Einen Wert einer Tabelle bestimemn                              */
/***************************************************************************************/
short do_sql3_idvalue(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[1024];

  if( prg_pars[0][0] && prg_pars[1][0] && prg_pars[2][0] )
  { snprintf(query, 1024, "select %s from %s where ID='%s'", prg_pars[1], prg_pars[0],
             prg_pars[2]);

    if( out )
      return sql3_2s(query, out, n, false, "", "", quote);
    else
      return sql3_2net(query, false, "", "", true);
  }
  else
    return true;
}


/***************************************************************************************/
/* short do_sql3_ins(int pa, char **out, long n,                                       */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_ins testen, ob Datensatz, der beschrieben wird vohanden ist, dann       */
/*                ID zurück, sonst neuen Datensatz erzeugen und neue ID zurück         */
/***************************************************************************************/
short do_sql3_ins(int pa, char **out, long n,
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
    LOG(3, "do_sql3_ins, query-select: %s.\n", query);

    p = *out;
    sql3_2s(query, out, n, false, "", "", quote);
    if( *out == p )
    { snprintf(query, MAX_ZEILENLAENGE, "insert into %s set %s", prg_pars[0],
               prg_pars[1]);
      LOG(3, "do_sql3_ins, query-insert: %s.\n", query);
      return sql3insert2s(query, out, n, false);
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* short do_sql3_changes(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql3_changes fügt Anzahl der geänderten Zeilen des letzten sqlitewrite ein   */
/***************************************************************************************/
short do_sql3_changes(int pa, char **out, long n,
                    char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ LOG(1, "do_sql3_changes, sql3_write_flag: %d.\n", sql3_write_flag);

  if( !sql3_write_flag )
    return true;

  *out += snprintf(*out, n, "%llu", sqlite3_changes(sql3db));
  return false;
}


/***************************************************************************************/
/* short do_sql3_store_ins(int pa, char **out, long n,                                 */
/*                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)*/
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     do_sql3_store_ins Datei als blob in Datenbank speichern - neuen Datensatz       */
/***************************************************************************************/
short do_sql3_store_ins(int pa, char **out, long n,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *buffer, *p;
  long nb, i;
  int hd_in;
  short ret;
  sqlite3_int64 newid;
  char *err_msg = NULL;
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#else
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#endif
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#endif

  LOG(1, "u2w_sql3_store_ins, Datei: %s, Tabelle: %s, Feld: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2]);

  if( !sqlite3_open_flag )
    return true;

  newid = 0;
  ret = false;
  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_sql3_store_ins, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_sql3_store_ins, grosse Datei.\n");
        if( NULL == (buffer = malloc(SQL3_BUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = buffer + snprintf(buffer, SQL3_BUFFERSIZE, "insert into %s set %s='",
                 prg_pars[1], prg_pars[2]);
        LOG(9, "u2w_sql3_store_ins, grosse Datei vor while.\n");
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
            if( p-buffer >= SQL3_BUFFERSIZE-100 )
            { LOG(9, "u2w_sql3_store_ins, grosse Date p-buffer >= 9999900 ...\n");
              if( !newid )
                strcpyn(p, "'", SQL3_BUFFERSIZE - (p-buffer));
              else
                snprintf(p, SQL3_BUFFERSIZE - (p-buffer), "') where ID='%llu'",
                         newid);
              LOG(9, "u2w_sql3_store_ins, grosse Datei vor sql3_query.\n");
              if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
              { if( sqlite3_error_log_flag )
                  logging("sql3_query error: %s.\n", err_msg);
                close(hd_in);
                free(buffer);
#ifdef WEBSERVER
                keepalive_flag = false;
#endif
                return true;
              }
              if( !newid )
                newid = sqlite3_last_insert_rowid(sql3db);
              LOG(9, "u2w_sql3_store_ins, grosse Datei nach sql3_query.\n");
              p = buffer + snprintf(buffer, SQL3_BUFFERSIZE,
                                    "update %s set %s=concat(%s,'",
                                    prg_pars[1], prg_pars[2], prg_pars[2]);
            }
          }
        }
        LOG(7, "u2w_sql3_store_ins, grosse Datei, newid: %llu.\n", newid);
        if( !newid )
          strcpyn(p, "'", SQL3_BUFFERSIZE - (p-buffer));
        else
          snprintf(p, SQL3_BUFFERSIZE - (p-buffer), "') where ID='%s'",
                   prg_pars[3]);
        if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
        { if( sqlite3_error_log_flag )
            logging("sql3_query error: %s.\nquery: %s.\n", err_msg, buffer);
          free(buffer);
          close(hd_in);
#ifdef WEBSERVER
          keepalive_flag = false;
#endif
          return true;
        }
        if( !newid )
          newid = sqlite3_last_insert_rowid(sql3db);
        free(buffer);
      }
      else
      { LOG(5, "u2w_sql3_store_ins, kleine Datei.\n");
        if( NULL != (buffer = malloc(100+2*stat_buf.st_size)) )
        { p = buffer + sprintf(buffer, "insert into %s set %s='", prg_pars[1],
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
          LOG(100, "sql3_store_ins, buffer: %s.\n", buffer);
          if( SQLITE_OK != sqlite3_exec(sql3db, buffer, 0, 0, &err_msg) )
          { if( sqlite3_error_log_flag )
              logging("sql3_query error: %s.\n", err_msg);
#ifdef WEBSERVER
            keepalive_flag = false;
#endif
            ret = true;
          }
          free(buffer);
          newid = sqlite3_last_insert_rowid(sql3db);
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
/* short sql3_2s(char *query, char **o, long n, int error_flag,                        */
/*             char *ssep, char *zsep, int quote)                                      */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             return     : true bei kein Wert                                         */
/*     sql3_2s fuehrt query aus und schreibt die Ausgabe nach o                        */
/***************************************************************************************/
short sql3_2s(char *query, char **o, long n, int error_flag, char *ssep, char *zsep,
            int quote)
{ sqlite3_stmt *sql3res;
  char *oo;
  int i;
  short ret;
  long nz;

  LOG(1, "sql3_2s, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");
  LOG(5, "sql3_2s, quote: %d.\n", quote);

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = " ";

  ret = false;

  oo = *o;
  LOG(3, "sql3_2s, query: %s.\n", query);

  if( !sqlite3_open_flag )                       /* Aktive sql3-Verbindung?            */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( SQLITE_OK != sqlite3_prepare_v2(sql3db, query, -1, &sql3res, 0) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s.\nquery: %s.\n", sqlite3_errmsg(sql3db), query);
    if( error_flag && sqlite3_error_out_flag )
      *o += snprintf(*o, n, "sql3_query error: %s, query: %s.",
                     sqlite3_errmsg(sql3db), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    **o = '\0';
    return true;
  }
  else
  { nz = 0;
    while( SQLITE_ROW == sqlite3_step(sql3res) )
    { if( nz++ )
        strcpyn_z(o, zsep, n - (*o-oo));
      for( i = 0; i < sqlite3_data_count(sql3res); i++ )
      { if( i )
          strqcpyn_z(o, ssep, n - (*o-oo), qf_strings[quote]);
        if( sqlite3_column_bytes(sql3res, i) )
        { strqcpynm_z(o, sqlite3_column_text(sql3res, i), n - (*o-oo),
                      sqlite3_column_bytes(sql3res, i), qf_strings[quote]);
        }
      }
    }
    sqlite3_finalize(sql3res);
  }
  **o = '\0';

  return ret;                                    /* Anzahl der Zeichen vom Programm    */
}


/***************************************************************************************/
/* short sql3_2net(char *query, int error_flag, char *ssep, char *zsep, int htmlflag)  */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int htmlflag: 1 - HTML-Sonderzeichen umwandeln                          */
/*             return     : true bei kein Wert                                         */
/*     sql3_2net fuehrt query aus und sendet Ausgabe an Browser                        */
/***************************************************************************************/
short sql3_2net(char *query, int error_flag, char *ssep, char *zsep, int htmlflag)
{ char o[MAX_ZEILENLAENGE];
  sqlite3_stmt *sql3res;
  int i;
  short ret;
  long nz;

  LOG(1, "sql3_2net, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");

  if( !zsep )
    zsep = " ";


  if( !ssep )
    ssep = " ";

  ret = false;

  if( !sqlite3_open_flag )                       /* Aktive sql3-Verbindung?            */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( SQLITE_OK != sqlite3_prepare_v2(sql3db, query, -1, &sql3res, 0) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s.\nquery: %s.\n", sqlite3_errmsg(sql3db), query);
    if( error_flag && sqlite3_error_out_flag )
    { snprintf(o, MAX_ZEILENLAENGE, "sql3_query error: %s, query: %s.",
               sqlite3_errmsg(sql3db), query);
      dosendh(o);
    }
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { nz = 0;
    while( SQLITE_ROW == sqlite3_step(sql3res) )
    { if( nz++ )
      { dosend(zsep);
      }
      for( i = 0; i < sqlite3_data_count(sql3res); i++ )
      { if( i )
        { dosend(ssep);
        }
        if( sqlite3_column_bytes(sql3res, i) )
        { LOG(9, "sql3_2net, length: %lu.\n", sqlite3_column_bytes(sql3res, i));
          if( htmlflag )
            dobinsendh(sqlite3_column_text(sql3res, i), sqlite3_column_bytes(sql3res, i));
          else
            dobinsend(sqlite3_column_text(sql3res, i), sqlite3_column_bytes(sql3res, i));
          last_char_sended = last_non_ws_char(sqlite3_column_text(sql3res, i), sqlite3_column_bytes(sql3res, i));
        }
      }
    }
    sqlite3_finalize(sql3res);
  }

  return ret;
}


/***************************************************************************************/
/* short sql3_2net_tbl(char *query, int error_flag, int htmlflag)                      */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             int htmlflag: 1 - HTML-Sonderzeichen umwandeln                          */
/*             return     : true bei kein Wert                                         */
/*     sql3_2net fuehrt query aus und sendet Ausgabe an Browser                        */
/***************************************************************************************/
short sql3_2net_tbl(char *query, int error_flag, int htmlflag)
{ char o[MAX_ZEILENLAENGE];
  sqlite3_stmt *sql3res;
  int i;
  short ret;
  long nz;
  char trclass[MAX_LEN_CLASSNAME+10], tdclass[MAX_LEN_CLASSNAME+10];
  char *tbl_row_end, *tbl_row_start, *tbl_cell_end, *tbl_cell_start;

  LOG(1, "sql3_2net_tbl, query: %s.\n", query);

  if( !sqlite3_open_flag )                       /* Aktive sql3-Verbindung?            */
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

  if( SQLITE_OK != sqlite3_prepare_v2(sql3db, query, -1, &sql3res, 0) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s.\nquery: %s.\n", sqlite3_errmsg(sql3db), query);
    if( error_flag && sqlite3_error_out_flag )
    { snprintf(o, MAX_ZEILENLAENGE, "sql3_query error: %s, query: %s.",
               sqlite3_errmsg(sql3db), query);
      dosendh(o);
    }
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { nz = 0;
    while( SQLITE_ROW == sqlite3_step(sql3res) )
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
      for( i = 0; i < sqlite3_data_count(sql3res); i++ )
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
        if( sqlite3_column_bytes(sql3res, i) )
        { LOG(9, "sql3_2net_tbl, length: %lu.\n", sqlite3_column_bytes(sql3res, i));
          if( htmlflag )
            dobinsendh(sqlite3_column_text(sql3res, i), sqlite3_column_bytes(sql3res, i));
          else
            dobinsend(sqlite3_column_text(sql3res, i), sqlite3_column_bytes(sql3res, i));
        }
        if( i < sqlite3_data_count(sql3res) - 1 )
        { dosend(tbl_cell_end);
        }
      }
    }
    sqlite3_finalize(sql3res);
  }

  return ret;
}


/***************************************************************************************/
/* short sql3_2dat(char *query, char *path, char *ssep, char *zsep)                    */
/*             char *query: SQL-query                                                  */
/*             char *path: Datei für die Ausgabe                                       */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zend  : Endezeichen einzelner Zeilen                              */
/*             return     : true bei kein Wert                                         */
/*     sql3_2dat fuehrt query aus und schreibt Ausgabe in Datei path                   */
/***************************************************************************************/
short sql3_2dat(char *query, char *path, char *ssep, char *zend)
{ sqlite3_stmt *sql3res;
  int i;
  short ret;
  int hd_out;

  LOG(1, "sql3_2dat, query: %s, path: %s, ssep: %s, zend: %s.\n", query, path,
      ssep ? ssep : "NULL", zend ? zend : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zend )
    zend = "\n";

  ret = false;

  if( !sqlite3_open_flag )                       /* Aktive sql3-Verbindung?            */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( SQLITE_OK != sqlite3_prepare_v2(sql3db, query, -1, &sql3res, 0) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s.\nquery: %s.\n", sqlite3_errmsg(sql3db), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { if( 0 <= (hd_out = open64(path, O_WRONLY | O_CREAT | O_TRUNC, 00600)) ) 
    { while( !ret && (SQLITE_ROW == sqlite3_step(sql3res)) )
      { for( i = 0; i < sqlite3_data_count(sql3res); i++ )
        { if( i )
          { if( strlen(ssep) < write(hd_out, ssep, strlen(ssep)) )
            { ret = true;
              break;
            }
          }
          if( sqlite3_column_bytes(sql3res, i) )
          { LOG(99, "sql3_2dat, length: %lu.\n", sqlite3_column_bytes(sql3res, i));
            if( sqlite3_column_bytes(sql3res, i)
                < write(hd_out, sqlite3_column_text(sql3res, i), sqlite3_column_bytes(sql3res, i)) )
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
    sqlite3_finalize(sql3res);
    return ret;
  }
}


/***************************************************************************************/
/* short sql3insert2s(char *query, char **o, long n, int error_flag)                   */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             return     : true bei Fehler                                            */
/*     sql3insert2s führt query aus und schreibt eingefügte ID nach o                  */
/***************************************************************************************/
short sql3insert2s(char *query, char **o, long n, int error_flag)
{ char *err_msg = NULL;

  LOG(1, "sql3insert2s, query: %s.\n", query);

  if( !sqlite3_open_flag )                       /* Aktive sql3-Verbindung?            */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( SQLITE_OK != sqlite3_exec(sql3db, query, 0, 0, &err_msg) )
  { if( sqlite3_error_log_flag )
      logging("sql3_query error: %s.\nquery: %s.\n", sqlite3_errmsg(sql3db), query);
    if( error_flag && sqlite3_error_out_flag )
      *o += snprintf(*o, n, "sql3_query error: %s, query: %s.",
                     sqlite3_errmsg(sql3db), query);
#ifdef WEBSERVER
    keepalive_flag = false;
#endif
    return true;
  }
  else
  { LOG(5, "sql3insert2s, sql3_insert_id: %llu.\n", sqlite3_last_insert_rowid(sql3db));
    *o += snprintf(*o, n, "%llu", sqlite3_last_insert_rowid(sql3db));
  }
  return false;
}

#endif
