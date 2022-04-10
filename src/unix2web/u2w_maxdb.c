/**********************************************/
/* File: u2w_mysql.c                          */
/* Funktionen zum Ansprechen von MySQL        */
/* timestamp: 2013-10-27 11:09:34             */
/**********************************************/

#ifdef MAXDBCLIENT
#include "u2w.h"
#endif


/* Anfang globale Variablen */
#ifdef MAXDBCLIENT
short maxdb_connect_flag = false;
SQLDBC_IRuntime *mdbr;
int maxdbport = STD_MAXDBPORT;
#endif
/* Ende globale Variablen */


#ifdef MAXDBCLIENT

short maxdb_write_flag = false;
short maxdb_id_flag = false;

#define MAX_P_QUERIES 5
MAXDB_RES *mres[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
short maxdb_res_flag[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
short maxdb_line_flag[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
short maxdb_next_value[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
MAXDB_ROW mrow[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
unsigned int mnf[MAX_ANZ_INCLUDE+MAX_P_QUERIES];
unsigned long *mlengths[MAX_ANZ_INCLUDE+MAX_P_QUERIES];

#ifdef DBCLIENT
/***************************************************************************************/
/* MAXDB *maxdb_connect(char *server, char *user, char *pwd, char *db, int port)       */
/*                      char *server: Servername                                       */
/*                      char *user  : Username                                         */
/*                      char *pwd   : Kennwort                                         */
/*                      char *db    : Datenbank                                        */
/*                      int port    : Port                                             */
/*        maxdb_connect öffnet Verbindung mit Maxdb-Datenbank, wird von sql_connect    */
/*                      aufgerufen.                                                    */
/*                      Return: Maxdb-Handle bei Erfolg, sonst NULL                    */
/***************************************************************************************/
MAXDB *maxdb_connect(char *server, char *user, char *pwd, char *db, int port)
{ MAXDB *mh;

  if( NULL != (mh = calloc(1, sizeof(MAXDB))) )
  { maxdb_init(mh);
    if( NULL == maxdb_real_connect(mh, prg_pars[1], prg_pars[2], prg_pars[3],
                                   prg_pars[4], maxdbport, NULL, 0) )
    { if( logflag & NORMLOG )
        fprintf(stderr, "Failed to connect to database: Error: %s.\n",
                maxdb_error(mh));
      if( maxdb_error_log_flag )
        logging("Failed to connect to database: Error: %s.\n", maxdb_error(&mh));
      free(mh);
    }
    else
      return mh;
  }
  return NULL;
}


/***************************************************************************************/
/* MAXDB_RES *sql_maxdb_query(MAXDB *mh, char *query)                                  */
/*                            MAXDB *mh: Handle zur DB                                 */
/*                            char *query: SQL-Query                                   */
/*            sql_maxdb_query: Query ausführen, wird von u2w_sql_query aufgerufen      */
/***************************************************************************************/
MAXDB_RES *sql_maxdb_query(MAXDB *mh, char *query)
{ MAXDB_RES *res;

  LOG(1, "sql_maxdb_query.\n");

  LOG(3, "sql_maxdb_query, query: %s.\n", query);

  if( maxdb_query(mh, query) )
  { if( logflag & NORMLOG )
      fprintf(stderr, "maxdb_query error: %s\nquery: %s.\n", maxdb_error(mh),
        query);
    if( maxdb_error_log_flag )
      logging("maxdb_query error: %s\nquery: %s.\n", maxdb_error(mh), query);
    return true;
  }
  else
    return maxdb_store_result(mh);
}
#endif  /* #ifdef DBCLIENT */

/***************************************************************************************/
/* void maxdb_init_flags(void)                                                         */
/*      maxdb_init_flags, Maxdb-Flags initialisieren                                   */
/***************************************************************************************/
void maxdb_init_flags(void)
{ int i;

  LOG(11, "maxdb_init_flags.\n");

  for( i = 0; i < MAX_ANZ_INCLUDE+MAX_P_QUERIES; i++ )
  { maxdb_res_flag[i] = 0;
    mnf[i] = 0;
  }
}


/***************************************************************************************/
/* void maxdb_free_one_res(int i)                                                      */
/*      maxdb_free_one_res Speicher der mres freigeben                                 */
/***************************************************************************************/
void maxdb_free_one_res(int i)
{
  LOG(1, "maxdb_free_one_res, i: %d.\n", i);

  if( maxdb_res_flag[i] )
  { maxdb_free_result(mres[i]);
    maxdb_res_flag[i] = 0;
    mnf[i] = 0;
  }
  LOG(2, "/maxdb_free_one_res.\n");
}


/***************************************************************************************/
/* void maxdb_free_res(void)                                                           */
/*      maxdb_free_res Speicher der mres freigeben                                     */
/***************************************************************************************/
void maxdb_free_res(void)
{ int i;

  LOG(1, "maxdb_free_res.\n");

  for( i = MAX_ANZ_INCLUDE; i < MAX_ANZ_INCLUDE+MAX_P_QUERIES; i++ )
    maxdb_free_one_res(i);
}


/***************************************************************************************/
/* short u2w_maxdb_port(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_port Port für Maxdb-Server ändern                                     */
/***************************************************************************************/
short u2w_maxdb_port(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( isdigit(prg_pars[0][0]) )
  { maxdbport = atoi(prg_pars[0]);
    return false;
  }
  return true;
}


/***************************************************************************************/
/* short u2w_maxdb_connect(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])  */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_connect öffnet eine Verbindung zum Maxdb-Server                       */
/***************************************************************************************/
short u2w_maxdb_connect(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ static char cursqlserver[128], cursqluser[128], cursqlpwd[128], cursqldb[128];

  LOG(1, "u2w_maxdb_connect.\n");

  LOG(3, "u2w_maxdb_connect, server: %s, user: %s, pwd, %s, db: %s.\n", prg_pars[0],
      prg_pars[1], prg_pars[2], prg_pars[3]);

  LOG(5, "u2w_maxdb_conncet, port: %d.\n", maxdbport);

  if( maxdb_connect_flag )
  { if( !strcmp(prg_pars[0], cursqlserver) && !strcmp(prg_pars[1], cursqluser)
        && !strcmp(prg_pars[2], cursqlpwd) && !strcmp(prg_pars[3], cursqldb) )
      return false;
    else
    { maxdb_free_res();
      maxdb_close(&mh);
    }
  }

  maxdb_init(&mh);

  if( NULL == maxdb_real_connect(&mh, prg_pars[0], prg_pars[1], prg_pars[2],
                                 prg_pars[3], maxdbport, NULL, 0) )
  { if( logflag & NORMLOG )
      fprintf(stderr, "Failed to connect to database: Error: %s.\n", maxdb_error(&mh));
    if( maxdb_error_log_flag )
      logging("Failed to connect to database: Error: %s.\n", maxdb_error(&mh));
    maxdb_connect_flag = false;
    return true;
  }
  strcpyn(cursqlserver, prg_pars[0], 128);
  strcpyn(cursqluser, prg_pars[1], 128);
  strcpyn(cursqlpwd, prg_pars[2], 128);
  strcpyn(cursqldb, prg_pars[3], 128);
  maxdb_connect_flag = true;
  return false;
}


/***************************************************************************************/
/* short u2w_maxdb_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_query Stellt Anfrage an Maxdb-Datenbank, Ergebnisse können dann mit   */
/*                 u2w_maxdb_next_line gelesen und mit maxdb_get_value gelesen werden  */
/***************************************************************************************/
short u2w_maxdb_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_maxdb_query.\n");

  if( !maxdb_connect_flag )
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

  if( maxdb_res_flag[cn] )
  { maxdb_res_flag[cn] = maxdb_line_flag[cn] = false;
    maxdb_free_result(mres[cn]);
    mnf[cn] = 0;
  }

  LOG(3, "u2w_maxdb_query, query: %s.\n", prg_pars[0]);

  if( maxdb_query(&mh, prg_pars[0]) )
  { if( logflag & NORMLOG )
      fprintf(stderr, "maxdb_query error: %s\nquery: %s.\n", maxdb_error(&mh),
        prg_pars[0]);
    if( maxdb_error_log_flag )
      logging("maxdb_query error: %s\nquery: %s.\n", maxdb_error(&mh), prg_pars[0]);
    return true;
  }
  else
  { if( (mres[cn] = maxdb_store_result(&mh)) )
    { maxdb_res_flag[cn] = true;
      mnf[cn] = maxdb_num_fields(mres[cn]);
      return false;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* short u2w_maxdb_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_write Stellt Anfrage an Maxdb-Datenbank, für insert und update        */
/***************************************************************************************/
short u2w_maxdb_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(1, "u2w_maxdb_write, query: %s.\n", prg_pars[0]);

  if( !maxdb_connect_flag )
    return true;

  if( maxdb_query(&mh, prg_pars[0]) )
  { if( logflag & NORMLOG )
      fprintf(stderr, "maxdb_query error: %s\nquery: %s.\n", maxdb_error(&mh),
              prg_pars[0]);
    if( maxdb_error_log_flag )
      logging("maxdb_query error: %s\nquery: %s.\n", maxdb_error(&mh), prg_pars[0]);
    return true;
  }
  else
  { maxdb_write_flag = true;
    maxdb_id_flag = true;
    return false;
  }
}


/***************************************************************************************/
/* short u2w_maxdb_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_test Stellt Anfrage an Maxdb-Datenbank, und liefert nur Wahrheitswert */
/***************************************************************************************/
short u2w_maxdb_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ MAXDB_RES *lmres;

  LOG(1, "u2w_maxdb_test, query: %s.\n", prg_pars[0]);

  if( !maxdb_connect_flag )
    return true;

  if( maxdb_query(&mh, prg_pars[0]) )
  { if( logflag & NORMLOG )
      fprintf(stderr, "maxdb_query error: %s\nquery: %s.\n", maxdb_error(&mh),
              prg_pars[0]);
    if( maxdb_error_log_flag )
      logging("maxdb_query error: %s\nquery: %s.\n", maxdb_error(&mh), prg_pars[0]);
    return true;
  }
  else
  { if( (lmres = maxdb_store_result(&mh)) )
      maxdb_free_result(lmres);
    maxdb_id_flag = true;
    return false;
  }
}


#define MAXDB_STORE_MODE_APPEND 1
#define MAXDB_PUFFERSIZE 10000000

/***************************************************************************************/
/* short u2w_maxdb_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_store Datei als blob in Datenbank speichern - Update                  */
/***************************************************************************************/
short u2w_maxdb_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *puffer, *p;
  long nb, i;
  int hd_in;
  short ret;
  int mode = 0;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#endif

  LOG(1, "u2w_maxdb_store, Datei: %s, Tabelle: %s, Feld: %s, ID: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3]);

  if( !maxdb_connect_flag )
    return true;

  if( pa & P5 )
  { switch( prg_pars[4][0] )
    { case 'a': mode = MAXDB_STORE_MODE_APPEND | mode ;
                break;
    }
  }

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_maxdb_store, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { int concatflag;

        LOG(5, "u2w_maxdb_store, grosse Datei.\n");
        if( NULL == (puffer = malloc(MAXDB_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = puffer;
        if( mode & MAXDB_STORE_MODE_APPEND )
        { concatflag = true;
          snprintf(p, MAXDB_PUFFERSIZE, "update %s set %s=concat(%s,'",
                   prg_pars[1], prg_pars[2], prg_pars[2]);
        }
        else
        { concatflag = false;
          snprintf(p, MAXDB_PUFFERSIZE, "update %s set %s='",
                   prg_pars[1], prg_pars[2]);
        }
        p += strlen(p);
        LOG(9, "u2w_maxdb_store, grosse Datei vor while.\n");
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
            if( p-puffer >= MAXDB_PUFFERSIZE-100 )
            { LOG(9, "u2w_maxdb_store, grosse Date p-puffer >= 9999900 ...\n");
              if( concatflag )
                snprintf(p, MAXDB_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                         prg_pars[3]);
              else
              { snprintf(p, MAXDB_PUFFERSIZE - (p-puffer), "' where ID='%s'",
                         prg_pars[3]);
                concatflag = true;
              }
              LOG(9, "u2w_maxdb_store, grosse Datei vor maxdb_query.\n");
              if( maxdb_query(&mh, puffer) )
              { if( logflag & NORMLOG )
                  fprintf(stderr, "maxdb_query error: %s.\n", maxdb_error(&mh));
                if( maxdb_error_log_flag )
                  logging("maxdb_query error: %s.\n", maxdb_error(&mh));
                close(hd_in);
                free(puffer);
                return true;
              }
              LOG(9, "u2w_maxdb_store, grosse Datei nach maxdb_query.\n");
              snprintf(puffer, MAXDB_PUFFERSIZE, "update %s set %s=concat(%s,'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
              p = puffer + strlen(puffer);
            }
          }
        }
        LOG(7, "u2w_maxdb_store, grosse Datei, concatflag: %d.\n", concatflag);
        if( concatflag )
          snprintf(p, MAXDB_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        else
          snprintf(p, MAXDB_PUFFERSIZE - (p-puffer), "' where ID='%s'",
                   prg_pars[3]);
        if( maxdb_query(&mh, puffer) )
        { if( logflag & NORMLOG )
            fprintf(stderr, "maxdb_query error: %s.\n",
                    maxdb_error(&mh));
          if( maxdb_error_log_flag )
            logging("maxdb_query error: %s.\n", maxdb_error(&mh));
          free(puffer);
          close(hd_in);
          return true;
        }
        free(puffer);
      }
      else
      { LOG(5, "u2w_maxdb_store, kleine Datei.\n");
        if( NULL != (puffer = malloc(100+2*stat_buf.st_size)) )
        { LOG(11, "u2w_maxdb_store, kleine Datei, Puffer allokiert.\n");
          if( mode & MAXDB_STORE_MODE_APPEND )
            sprintf(puffer,"update %s set %s=concat(%s,'",
                     prg_pars[1], prg_pars[2], prg_pars[2]);
          else
            sprintf(puffer, "update %s set %s='", prg_pars[1], prg_pars[2]);
          p = puffer + strlen(puffer);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { LOG(50, "u2w_maxdb_store, kleine Datei, nb: %d.\n", nb);
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
          if( mode & MAXDB_STORE_MODE_APPEND )
            sprintf(p, "') where ID='%s'", prg_pars[3]);
          else
            sprintf(p, "' where ID='%s'", prg_pars[3]);
          LOG(100, "maxdb_store, puffer: %s.\n", puffer);
          if( maxdb_query(&mh, puffer) )
          { if( logflag & NORMLOG )
              fprintf(stderr, "maxdb_query error: %s.\n", maxdb_error(&mh));
            if( maxdb_error_log_flag )
              logging("maxdb_query error: %s.\n", maxdb_error(&mh));
            ret = true;
          }
          else
            ret = false;
          free(puffer);
        }
        else if( logflag & NORMLOG )
          fprintf(stderr, _("Unable to allocate memory.\n"));
      }
      close(hd_in);
    }
  }
  return ret;
}


/***************************************************************************************/
/* short u2w_maxdb_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_out Query ausführen und Ergebnis in Datei schreiben                   */
/***************************************************************************************/
short u2w_maxdb_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(3, "u2w_maxdb_out, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  return maxdb2dat(prg_pars[0], prg_pars[1], pa & P3 ? prg_pars[2] : NULL,
                   pa & P4 ? prg_pars[3] : NULL);
}


/***************************************************************************************/
/* short u2w_maxdb_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS]) */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_get_line bereitet eine Zeile zum lesen vor                            */
/***************************************************************************************/
short u2w_maxdb_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_maxdb_get_line.\n");

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

  if( maxdb_res_flag[cn] && mres[cn] )
  { if( (mrow[cn] = maxdb_fetch_row(mres[cn])) )
    { mlengths[cn] = maxdb_fetch_lengths(mres[cn]);
      maxdb_line_flag[cn] = true;
      maxdb_next_value[cn] = 0;
      return false;
    }
    else
    { maxdb_line_flag[cn] = false;
      mnf[cn] = 0;
      return true;
    }
  }
  maxdb_line_flag[cn] = false;
  mnf[cn] = 0;
  return true;
}


/***************************************************************************************/
/* short do_maxdb_list_get_line(int *listlen,                                          */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_list_get_line Eine Zeile in Liste                                      */
/***************************************************************************************/
short do_maxdb_list_get_line(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn, i;

  LOG(1, "do_maxdb_list_get_line, listlen: %d.\n", *listlen);

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

  if( maxdb_res_flag[cn] && mres[cn] )
  { if( (mrow[cn] = maxdb_fetch_row(mres[cn])) )
    { mlengths[cn] = maxdb_fetch_lengths(mres[cn]);
      maxdb_line_flag[cn] = true;
      maxdb_next_value[cn] = 0;
      for( i = 0; i < mnf[cn] && *listlen < MAX_LIST_LEN; i++ )
      { if( mlengths[cn][i] && mrow[cn][i] )
          strcpynm(list_pars[(*listlen)++], mrow[cn][i], MAX_LEN_LIST_PARS,
                   mlengths[cn][i]);
        else
          list_pars[(*listlen)++][0] = '\0';
      }
      LOG(17, "/do_maxdb_list_get_line, list_pars[0]: %s.\n", list_pars[0]);
      return false;
    }
    else
    { maxdb_line_flag[cn] = false;
      mnf[cn] = 0;
      *listlen = -1;
      return true;
    }
  }
  maxdb_line_flag[cn] = false;
  mnf[cn] = 0;
  *listlen = -1;
  return true;
}


/***************************************************************************************/
/* short u2w_maxdb_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])  */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_maxdb_isvalue testet, ob noch ein Wert für maxdbreadvalue vorliegt          */
/***************************************************************************************/
short u2w_maxdb_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_maxdb_isvalue.\n");

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

  return !maxdb_line_flag[cn] || maxdb_next_value[cn] >= mnf[cn];
}


/***************************************************************************************/
/* short do_maxdb_writeline(int pa, char **out, long n, int tablecols,                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_maxdb_writeline fügt Zeile des letzten maxdb_getline ein                    */
/***************************************************************************************/
short do_maxdb_writeline(int pa, char **out, long n, int tablecols,
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

  if( maxdb_line_flag[cn] )
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
      { if( pa & P2 )
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
/* short do_maxdb_readwriteline(int pa, char **out, long n, int tablecols,             */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_readwriteline fügt eine Zeile der letzten maxdb_query ein              */
/***************************************************************************************/
short do_maxdb_readwriteline(int pa, char **out, long n, int tablecols,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !u2w_maxdb_get_line(pa, prg_pars) )
    return do_maxdb_writeline(pa, out, n, tablecols, prg_pars, quote);
  else
    return true;
}


/***************************************************************************************/
/* short do_maxdb_num_fields(int pa, char **out, long n, int tablecols,                */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_num_fields ergibt Anzahl Spalten der Maxdb-Abfrage                     */
/***************************************************************************************/
short do_maxdb_num_fields(int pa, char **out, long n, int tablecols,
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

 if( !maxdb_res_flag[cn] )
    return true;
  snprintf(*out, n, "%d", mnf[cn]);
  *out += strlen(*out);
  return false;
}


/***************************************************************************************/
/* short do_maxdb_value(int pa, char **out, long n, int tablecols,                     */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_value fügt einen Wert einer Zeile ein                                  */
/***************************************************************************************/
short do_maxdb_value(int pa, char **out, long n, int tablecols,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_maxdb_value\n");

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

  LOG(3, "do_maxdb_value, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_maxdb_value, i: %d, mnf[%d]: %d.\n", i, cn, mnf[cn]);
    if( i < mnf[cn] )
    { if( mrow[cn][i] )
      { if( out )
          strqcpynm_z(out, mrow[cn][i], n, mlengths[cn][i], qf_strings[quote]); 
        else
        { dobinsendh(mrow[cn][i], mlengths[cn][i]);
          last_char_sended = last_non_ws_char(mrow[cn][i], mlengths[cn][i]);
        }
      }
    }
    LOG(1, "/do_maxdb_value\n");
    return false;
  }
  LOG(1, "/do_maxdb_value - ERROR\n");
  return true;
}


/***************************************************************************************/
/* short do_maxdb_read_value(int pa, char **out, long n, int tablecols,                */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_read_value fügt näcshten Wert einer Zeile ein                          */
/***************************************************************************************/
short do_maxdb_read_value(int pa, char **out, long n, int tablecols,
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

  if( !maxdb_line_flag[cn] || maxdb_next_value[cn] >= mnf[cn] )
    return true;

  if( mrow[cn][maxdb_next_value[cn]] )
  { if( out )
      strqcpynm_z(out, mrow[cn][maxdb_next_value[cn]], n,
                  mlengths[cn][maxdb_next_value[cn]], qf_strings[quote]); 
    else
    { dobinsendh(mrow[cn][maxdb_next_value[cn]], mlengths[cn][maxdb_next_value[cn]]);
      last_char_sended = last_non_ws_char(mrow[cn][maxdb_next_value[cn]],
                                          mlengths[cn][maxdb_next_value[cn]]);
    }
  }

  maxdb_next_value[cn]++;
  return false;
}


/***************************************************************************************/
/* short do_maxdb_id(int pa, char **out, long n, int tablecols,                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb fügt letzte ID (nach insert) ein                                       */
/***************************************************************************************/
short do_maxdb_id(int pa, char **out, long n, int tablecols,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( !maxdb_id_flag )
    return true;
  snprintf(*out, n, "%llu", maxdb_insert_id(&mh));
  *out += strlen(*out);
  return false;
}


/***************************************************************************************/
/* short do_maxdb_read(int pa, char **out, long n, int tablecols,                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_read Query ausführen und Ergebnis einfügen                             */
/***************************************************************************************/
short do_maxdb_read(int pa, char **out, long n, int tablecols,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_maxdb_read, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( out )
    return maxdb2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, tablecols, quote);
  else
    return maxdb2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, tablecols);
}


/***************************************************************************************/
/* short do_maxdb_list_read(int *listlen,                                              */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_list_read Query ausführen und Ergebnis in Liste                        */
/***************************************************************************************/
short do_maxdb_list_read(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ MAXDB_RES *mres;
  MAXDB_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i;

  LOG(3, "do_maxdb_list_read, q: %s.\n", prg_pars[0]);

  if( !maxdb_connect_flag )                      /* Aktive Maxdb-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( maxdb_query(&mh, prg_pars[0]) )
  { return true;
  }
  else
  { mres = maxdb_store_result(&mh);
    if( mres )
    { nf = maxdb_num_fields(mres);
      LOG(11, "do_maxdb_list_read, nf: %d.\n", nf);
      if( (mrow = maxdb_fetch_row(mres)) )
      { lengths = maxdb_fetch_lengths(mres);
        for( i = 0; i < nf && *listlen < MAX_LIST_LEN; i++ )
        { if( lengths[i] && mrow[i] )
            strcpynm(list_pars[(*listlen)++], mrow[i], MAX_LEN_LIST_PARS, lengths[i]);
          else
            list_pars[(*listlen)++][0] = '\0';
        }
      }
      maxdb_free_result(mres);
      return false;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* short do_maxdb_idvalue(int pa, char **out, long n, int tablecols,                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_idvalue Einen Wert einer Tabelle bestimemn                             */
/***************************************************************************************/
short do_maxdb_idvalue(int pa, char **out, long n, int tablecols,
                     char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[1024];

  if( prg_pars[0][0] && prg_pars[1][0] && prg_pars[2][0] )
  { snprintf(query, 1024, "select %s from %s where ID='%s'", prg_pars[1], prg_pars[0],
             prg_pars[2]);

    if( out )
      return maxdb2s(query, out, n, false, "", "", false, quote);
    else
      return maxdb2net(query, false, "", "", false);
  }
  else
    return true;
}


/***************************************************************************************/
/* short do_maxdb_ins(int pa, char **out, long n, int tablecols,                       */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_ins testen, ob Datensatz, der beschrieben wird vohanden ist, dann      */
/*                ID zurück, sonst neuen Datensatz erzeugen und neue ID zurück         */
/***************************************************************************************/
short do_maxdb_ins(int pa, char **out, long n, int tablecols,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE];
  char *p, *q;
  int q_flag;

  if( prg_pars[0][0] && prg_pars[1][0] )
  { snprintf(query, MAX_ZEILENLAENGE, "select ID from %s where ", prg_pars[0]);
    q = query+strlen(query);
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
    LOG(3, "do_maxdb_ins, query-select: %s.\n", query);

    p = *out;
    maxdb2s(query, out, n, false, "", "", false, quote);
    if( *out == p )
    { snprintf(query, MAX_ZEILENLAENGE, "insert into %s set %s", prg_pars[0],
               prg_pars[1]);
      LOG(3, "do_maxdb_ins, query-insert: %s.\n", query);
      return maxdbinsert2s(query, out, n, false);
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* short do_maxdb_enums(int pa, char **out, long n, int tablecols,                     */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_enums Möglichkeiten eines ENUMs oder SETs bestimmen                    */
/***************************************************************************************/
short do_maxdb_enums(int pa, char **out, long n, int tablecols,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE];
  char qresult[MAX_ZEILENLAENGE], *qr;

  LOG(3, "do_maxdb_enums, pa: %d, %s, %s.\n", pa, prg_pars[0], prg_pars[1]);

  snprintf(query, MAX_ZEILENLAENGE, "show columns from %s like '%s'", prg_pars[0],
           prg_pars[1]);

  qr = qresult;
  if( maxdb2s(query, &qr, MAX_ZEILENLAENGE, false, " ", " ", 0, quote) )
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
/* short do_maxdb_num_rows(int pa, char **out, long n, int tablecols,                  */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_maxdb_num_rows fügt Anzahl der geänderten/selektierten Zeilen der letzten    */
/*              maxdb_query ein                                                        */
/***************************************************************************************/
short do_maxdb_num_rows(int pa, char **out, long n, int tablecols,
                      char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !maxdb_write_flag )
    return true;

  snprintf(*out, n, "%llu", maxdb_affected_rows(&mh));
  *out += strlen(*out);
  return false;
}


/***************************************************************************************/
/* short do_maxdb_store_ins(int pa, char **out, long n, int tablecols,                 */
/*                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)*/
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     do_maxdb_store_ins Datei als blob in Datenbank speichern - neuen Datensatz      */
/***************************************************************************************/
short do_maxdb_store_ins(int pa, char **out, long n, int tabelrows,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *puffer, *p;
  long nb, i;
  int hd_in;
  short ret;
  my_ulonglong newid;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#endif

  LOG(1, "u2w_maxdb_store_ins, Datei: %s, Tabelle: %s, Feld: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2]);

  if( !maxdb_connect_flag )
    return true;

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_maxdb_store_ins, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_maxdb_store_ins, grosse Datei.\n");
        if( NULL == (puffer = malloc(MAXDB_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        newid = 0;
        p = puffer;
        snprintf(p, MAXDB_PUFFERSIZE, "insert into %s set %s='",
                 prg_pars[1], prg_pars[2]);
        p += strlen(p);
        LOG(9, "u2w_maxdb_store_ins, grosse Datei vor while.\n");
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
            if( p-puffer >= MAXDB_PUFFERSIZE-100 )
            { LOG(9, "u2w_maxdb_store_ins, grosse Date p-puffer >= 9999900 ...\n");
              if( !newid )
                strcpyn(p, "'", MAXDB_PUFFERSIZE - (p-puffer));
              else
                snprintf(p, MAXDB_PUFFERSIZE - (p-puffer), "') where ID='%llu'",
                         newid);
              LOG(9, "u2w_maxdb_store_ins, grosse Datei vor maxdb_query.\n");
              if( maxdb_query(&mh, puffer) )
              { if( logflag & NORMLOG )
                  fprintf(stderr, "maxdb_query error: %s.\n", maxdb_error(&mh));
                if( maxdb_error_log_flag )
                  logging("maxdb_query error: %s.\n", maxdb_error(&mh));
                close(hd_in);
                free(puffer);
                return true;
              }
              if( !newid )
                newid = maxdb_insert_id(&mh);
              LOG(9, "u2w_maxdb_store_ins, grosse Datei nach maxdb_query.\n");
              snprintf(puffer, MAXDB_PUFFERSIZE, "update %s set %s=concat(%s,'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
              p = puffer + strlen(puffer);
            }
          }
        }
        LOG(7, "u2w_maxdb_store_ins, grosse Datei, newid: %llu.\n", newid);
        if( !newid )
          strcpyn(p, "'", MAXDB_PUFFERSIZE - (p-puffer));
        else
          snprintf(p, MAXDB_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        if( maxdb_query(&mh, puffer) )
        { if( logflag & NORMLOG )
            fprintf(stderr, "maxdb_query error: %s.\nquery: %s.\n",
                    maxdb_error(&mh), puffer);
          if( maxdb_error_log_flag )
            logging("maxdb_query error: %s.\nquery: %s.\n", maxdb_error(&mh), puffer);
          free(puffer);
          close(hd_in);
          return true;
        }
        if( !newid )
          newid = maxdb_insert_id(&mh);
        free(puffer);
      }
      else
      { LOG(5, "u2w_maxdb_store_ins, kleine Datei.\n");
        if( NULL != (puffer = malloc(100+2*stat_buf.st_size)) )
        { sprintf(puffer, "insert into %s set %s='", prg_pars[1], prg_pars[2]);
          p = puffer + strlen(puffer);
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
          LOG(100, "maxdb_store_ins, puffer: %s.\n", puffer);
          if( maxdb_query(&mh, puffer) )
          { if( logflag & NORMLOG )
              fprintf(stderr, "maxdb_query error: %s.\n", maxdb_error(&mh));
            if( maxdb_error_log_flag )
              logging("maxdb_query error: %s.\n", maxdb_error(&mh));
            ret = true;
          }
          else
            ret = false;
          free(puffer);
          newid = maxdb_insert_id(&mh);
        }
        else if( logflag & NORMLOG )
          fprintf(stderr, _("Unable to allocate memory.\n"));
      }
      close(hd_in);
    }
  }
  snprintf(*out, n, "%llu", newid);
  *out+= strlen(*out);
  return ret;
}


/***************************************************************************************/
/* short maxdb2s(char *query, char **o, long n, int error_flag,                        */
/*             char *ssep, char *zsep, int tablecols, int quote)                       */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     maxdb2s fuehrt query aus und schreibt die Ausgabe nach o                        */
/***************************************************************************************/
short maxdb2s(char *query, char **o, long n, int error_flag, char *ssep, char *zsep,
             int tablecols, int quote)
{ MAXDB_RES *mres;
  MAXDB_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  char *oo;
  int i;
  short ret;
  long nz;

  LOG(1, "maxdb2s, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");
  LOG(5, "maxdb2s, quote: %d.\n", quote);

  if( !ssep )
  { if( tablecols )
      ssep = "</td><td>";
    else
      ssep = " ";
  }

  if( !zsep )
  { if( tablecols )
      zsep = "</td></tr>\n<tr><td>";
    else
      zsep = " ";
  }

  ret = false;

  oo = *o;
  LOG(3, "maxdb2s, query: %s.\n", query);

  if( !maxdb_connect_flag )                      /* Aktive Maxdb-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( maxdb_query(&mh, query) )
  { if( maxdb_error_log_flag )
      logging("maxdb_query error: %s.\nquery: %s.\n", maxdb_error(&mh), query);
    if( error_flag )
    { snprintf(*o, n, "maxdb_query error: %s, query: %s.",
               maxdb_error(&mh), query);
      *o += strlen(*o);
    }
    ret = true;
  }
  else
  { mres = maxdb_store_result(&mh);
    if( mres )
    { nf = maxdb_num_fields(mres);
      LOG(11, "maxdb2s, nf: %d.\n", nf);
      nz = 0;
      while( (mrow = maxdb_fetch_row(mres) ) )
      { if( nz++ )
          strcpyn_z(o, zsep, n - (*o-oo));
        lengths = maxdb_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            strqcpyn_z(o, ssep, n - (*o-oo), qf_strings[quote]);
          if( lengths[i] && mrow[i] )
          { if( tablecols && test_leer(mrow[i], lengths[i]) )
              strcpyn_z(o, "&nbsp;", n - (*o-oo));
            else
              strqcpynm_z(o, mrow[i], n - (*o-oo), lengths[i], qf_strings[quote]);
          }
          else if( tablecols )
            strcpyn_z(o, "&nbsp;", n - (*o-oo));
        }
      }
      maxdb_free_result(mres);
    }
    else
    { if( error_flag && maxdb_errno(&mh) )
      { snprintf(*o, n - (*o-oo), "Error: %s.", maxdb_error(&mh));
        *o += strlen(*o);
      }
      ret = true;
    }
  }
  **o = '\0';

  return ret;                                 /* Anzahl der Zeichen vom Programm    */
}


/***************************************************************************************/
/* short maxdb2net(char *query, int error_flag,                                        */
/*             char *ssep, char *zsep, int tablecols)                                  */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     maxdb2net fuehrt query aus und sendet Ausgabe an Browser                        */
/***************************************************************************************/
short maxdb2net(char *query, int error_flag, char *ssep, char *zsep,
             int tablecols)
{ char o[MAX_ZEILENLAENGE];
  MAXDB_RES *mres;
  MAXDB_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i;
  short ret;
  long nz;

  LOG(1, "maxdb2net, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");

  if( !ssep )
  { if( tablecols )
      ssep = "</td><td>";
    else
      ssep = " ";
  }

  if( !zsep )
  { if( tablecols )
      zsep = "</td></tr>\n<tr><td>";
    else
      zsep = " ";
  }

  ret = false;

  LOG(3, "do_query2net: %s.\n", query);

  if( !maxdb_connect_flag )                      /* Aktive Maxdb-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( maxdb_query(&mh, query) )
  { if( maxdb_error_log_flag )
      logging("maxdb_query error: %s.\nquery: %s.\n", maxdb_error(&mh), query);
    if( error_flag )
    { snprintf(o, MAX_ZEILENLAENGE, "maxdb_query error: %s, query: %s.",
               maxdb_error(&mh), query);
      dosendh(o);
    }
    ret = true;
  }
  else
  { mres = maxdb_store_result(&mh);
    if( mres )
    { nf = maxdb_num_fields(mres);
      nz = 0;
      while( (mrow = maxdb_fetch_row(mres) ) )
      { if( nz++ )
          dosend(zsep);
        lengths = maxdb_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            dosend(ssep);
          if( lengths[i] && mrow[i] )
          { if( tablecols && test_leer(mrow[i], lengths[i]) )
              dosend("&nbsp;");
            else
            { LOG(9, "maxdb2net, length: %lu.\n", lengths[i]);
              dobinsendh(mrow[i], lengths[i]);
              last_char_sended = last_non_ws_char(mrow[i], lengths[i]);
            }
          }
          else if( tablecols )
            ret = ret | dosend("&nbsp;");
        }
      }
      maxdb_free_result(mres);
    }
    else
    { if( error_flag && maxdb_errno(&mh) )
      { snprintf(o, MAX_ZEILENLAENGE, "Error: %s.", maxdb_error(&mh));
        dosendh(o);
      }
      ret = true;
    }
  }

  return ret;
}


/***************************************************************************************/
/* short maxdb2dat(char *query, char *path, char *ssep, char *zsep)                    */
/*             char *query: SQL-query                                                  */
/*             char *path: Datei für die Ausgabe                                       */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     maxdb2dat fuehrt query aus und schreibt Ausgabe in Datei path                   */
/***************************************************************************************/
short maxdb2dat(char *query, char *path, char *ssep, char *zsep)
{ MAXDB_RES *mres;
  MAXDB_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i;
  short ret;
  long nz;
  int hd_out;
#ifndef _LARGEFILE64_SOURCE
#define open64(a,b) open(a,b)
#endif

  LOG(1, "maxdb2dat, query: %s, path: %s, ssep: %s, zsep: %s.\n", query, path,
      ssep ? ssep : "NULL", zsep ? zsep : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = "\n";

  ret = false;

  if( !maxdb_connect_flag )                      /* Aktive Maxdb-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( maxdb_query(&mh, query) )
  { if( maxdb_error_log_flag )
      logging("maxdb_query error: %s.\nquery: %s.\n", maxdb_error(&mh), query);
    return true;
  }
  else
  { if( (mres = maxdb_store_result(&mh)) )
    { if( 0 <= (hd_out = open64(path, O_WRONLY | O_CREAT | O_TRUNC, 00600)) ) 
      { nf = maxdb_num_fields(mres);
        nz = 0;
        while( !ret && (mrow = maxdb_fetch_row(mres) ) )
        { if( nz++ )
          { if( strlen(zsep) < write(hd_out, zsep, strlen(zsep)) )
            { ret = true;
              break;
            }
          }
          lengths = maxdb_fetch_lengths(mres);
          for( i = 0; i < nf; i++ )
          { if( i )
            { if( strlen(ssep) < write(hd_out, ssep, strlen(ssep)) )
              { ret = true;
                break;
              }
            }
            if( lengths[i] && mrow[i] )
            { LOG(99, "maxdb2dat, length: %lu.\n", lengths[i]);
              if( lengths[i] < write(hd_out, mrow[i], lengths[i]) )
              { ret = true;
                break;
              }
            }
          }
        }
        close(hd_out);
      }
      else
        ret = true;
      maxdb_free_result(mres);
      return ret;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* short maxdbinsert2s(char *query, char **o, long n, int error_flag)                  */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             return     : true bei Fehler                                            */
/*     maxdbinsert2s führt query aus und schreibt eingefügte ID nach o                 */
/***************************************************************************************/
short maxdbinsert2s(char *query, char **o, long n, int error_flag)
{
  LOG(1, "maxdbinsert2s, query: %s.\n", query);

  if( !maxdb_connect_flag )                      /* Aktive Maxdb-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( maxdb_query(&mh, query) )
  { if( maxdb_error_log_flag )
      logging("maxdb_query error: %s.\nquery: %s.\n", maxdb_error(&mh), query);
    if( error_flag )
    { snprintf(*o, n, "maxdb_query error: %s, query: %s.",
               maxdb_error(&mh), query);
      *o += strlen(*o);
    }
    return true;
  }
  else
  { LOG(5, "maxdbinsert2s, maxdb_insert_id: %llu.\n", maxdb_insert_id(&mh));
    snprintf(*o, n, "%llu", maxdb_insert_id(&mh));
    *o += strlen(*o);
  }
  return false;
}

#ifdef NEW_VERSION
/***************************************************************************************/
/* short maxdb2p(char *query, char *ssep, char *zsep, char *p)                         */
/*             char *query: SQL-query                                                  */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             char *p     : Kommando, an das die Ausgabe geschickt wird               */
/*             return     : true bei kein Wert                                         */
/*     maxdb2p fuehrt query aus und schreibt die Ausgabe in einer pipe an *p           */
/***************************************************************************************/
short maxdb2p(char *query, char *ssep, char *zsep, char *p)
{ MAXDB_RES *mres;
  MAXDB_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  short ret;

  LOG(1, "maxdb2p, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = "\n";

  ret = false;

  if( !maxdb_connect_flag )                      /* Aktive Maxdb-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( maxdb_query(&mh, query) )
  { if( maxdb_error_log_flag )
      logging("maxdb_query error: %s.\nquery: %s.\n", maxdb_error(&mh), query);
    ret = true;
  }
  else
  { mres = maxdb_store_result(&mh);
    if( mres )
    { nf = maxdb_num_fields(mres);
      nz = 0;
      while( (mrow = maxdb_fetch_row(mres) ) )
      { if( nz++ )
          strcpyn_z(o, zsep, n - (*o-oo));
        lengths = maxdb_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            strcpyn_z(o, ssep, n - (*o-oo));
          if( lengths[i] && mrow[i] )
          { if( tablecols && test_leer(mrow[i], lengths[i]) )
              strcpyn_z(o, "&nbsp;", n - (*o-oo));
            else
            { snprintf(*o, n - (*o-oo), "%.*s",
                       (int)lengths[i], mrow[i]);
              *o += strlen(*o);
            }
          }
          else if( tablecols )
            strcpyn_z(o, "&nbsp;", n - (*o-oo));
        }
      }
      maxdb_free_result(mres);
    }
    else
    { ret = true;
    }
  }

  return ret;
}
#endif
#endif

