/**********************************************/
/* File: u2w_pgsql.c                          */
/* Funktionen zum Ansprechen von PostgreQL    */
/* timestamp: 2013-10-27 11:13:51             */
/**********************************************/

#ifdef POSTGRESQLCLIENT
#include "u2w.h"
#endif


/* Anfang globale Variablen */
#ifdef POSTGRESQLCLIENT
int pgsql_error_log_flag = false;    /* true: PGSQL-Error ins Logfile/stderr           */
int pgsql_error_out_flag = false;    /* true: PGSQL-Error an stdout / HTML             */
int pgsql_connect_flag = false;
PGconn *ph;
int pgsqlport = STD_PGSQLPORT;
#endif
/* Ende globale Variablen */


#ifdef POSTGRESQLCLIENT

int pgsql_write_flag = false;
int pgsql_id_flag = false;
char pgsql_num_rows[32];
unsigned long long pgsql_insert_id;

#define MAX_PP_QUERIES 5
PGresult *pres[MAX_ANZ_INCLUDE+MAX_PP_QUERIES];
int pgsql_res_flag[MAX_ANZ_INCLUDE+MAX_PP_QUERIES];
int pgsql_line[MAX_ANZ_INCLUDE+MAX_PP_QUERIES];
int pgsql_next_value[MAX_ANZ_INCLUDE+MAX_PP_QUERIES];


/***************************************************************************************/
/* PGconn *pgsql_connect(char *server, char *user, char *pwd, char *db, int port)      */
/*                      char *server: Servername                                       */
/*                      char *user  : Username                                         */
/*                      char *pwd   : Kennwort                                         */
/*                      char *db    : Datenbank                                        */
/*                      int port    : Port                                             */
/*        pgsql_connect öffnet Verbindung mit PostgreSQL-Datenbank, wird von           */
/*                      sql_connect aufgerufen.                                        */
/*                      Return: PGconn-Handle bei Erfolg, sonst NULL                   */
/***************************************************************************************/
PGconn *mysql_connect(char *server, char *user, char *pwd, char *db, int port)
{ PGconn *ph;

  LOG(1, "pgsql_connect.\n");

  LOG(3, "pgsql_connect, server: %s, user: %s, db: %s.\n", server, user, db);

  snprintf(zeile, MAX_ZEILENLAENGE,
           "host = '%s' port = '%d' dbname = '%s' user = '%s' password = '%s'",
           server, port, db, user, pwd);

  ph = PQconnectdb(zeile);
  if( PQstatus(ph) != CONNECTION_OK )
  { if( pgsql_error_log_flag )
      logging("Failed to connect to database: Error: %s.\n", PQerrorMessage(ph));
    return NULL;
  }
  return ph;
}


/***************************************************************************************/
/* PGresult *sql_pgsql_query(PGconn *ph, char *query)                                  */
/*                           PGconn *ph: Handle zur DB                                 */
/*                           char *query: SQL-Query                                    */
/*            sql_pgsql_query: Query ausführen, wird von u2w_sql_query aufgerufen      */
/***************************************************************************************/
PGresult *sql_pgsql_query(PGconn *ph, char *query)
{ PGresult *res;

  LOG(1, "sql_pgsql_query.\n");

  LOG(3, "sql_pgsql_query, query: %s.\n", query);

  res = PQexec(ph, query);
  if( PQresultStatus(res) != PGRES_TUPLES_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), query);
    PQclear(res);
    return NULL;
  }
  else
    return res;
}


/***************************************************************************************/
/* void pgsql_init_flags(void)                                                         */
/*      pgsql_init_flags, PGSQL-Flags initialisieren                                   */
/***************************************************************************************/
void pgsql_init_flags(void)
{ int i;

  LOG(11, "pgsql_init_flags.\n");

  for( i = 0; i < MAX_ANZ_INCLUDE+MAX_PP_QUERIES; i++ )
  { pgsql_res_flag[i] = 0;
  }
}


/***************************************************************************************/
/* void pgsql_free_one_res(int i)                                                      */
/*      pgsql_free_one_res Speicher der pres freigeben                                 */
/***************************************************************************************/
void pgsql_free_one_res(int i)
{
  LOG(1, "pgsql_free_one_res, i: %d.\n", i);

  if( pgsql_res_flag[i] )
  { PQclear(pres[i]);
    pgsql_res_flag[i] = 0;
  }
  LOG(2, "/pgsql_free_one_res.\n");
}


/***************************************************************************************/
/* void pgsql_free_res(void)                                                           */
/*      pgsql_free_res Speicher der mres freigeben                                     */
/***************************************************************************************/
void pgsql_free_res(void)
{ int i;

  LOG(1, "pgsql_free_res.\n");

  for( i = MAX_ANZ_INCLUDE; i < MAX_ANZ_INCLUDE+MAX_PP_QUERIES; i++ )
    pgsql_free_one_res(i);
}


/***************************************************************************************/
/* int u2w_pgsql_port(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_port Port für PGSQL-Server ändern                                     */
/***************************************************************************************/
int u2w_pgsql_port(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( isdigit(prg_pars[0][0]) )
  { pgsqlport = atoi(prg_pars[0]);
    return false;
  }
  return true;
}


/***************************************************************************************/
/* int u2w_pgsql_connect(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_connect öffnet eine Verbindung zum PGSQL-Server                       */
/***************************************************************************************/
int u2w_pgsql_connect(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ static char cursqlserver[128], cursqluser[128], cursqlpwd[128], cursqldb[128];

  LOG(1, "u2w_pgsql_connect.\n");

  LOG(3, "u2w_pgsql_connect, server: %s, user: %s, pwd, %s, db: %s.\n", prg_pars[0],
      prg_pars[1], prg_pars[2], prg_pars[3]);

  LOG(5, "u2w_pgsql_conncet, port: %d.\n", pgsqlport);

  if( pgsql_connect_flag )
  { if( !strcmp(prg_pars[0], cursqlserver) && !strcmp(prg_pars[1], cursqluser)
        && !strcmp(prg_pars[2], cursqlpwd) && !strcmp(prg_pars[3], cursqldb) )
      return false;
    else
    { pgsql_free_res();
      PQfinish(ph);
    }
  }


           "host = '%s' port = '%d' dbname = '%s' user = '%s' password = '%s'",
           prg_pars[0], pgsqlport, prg_pars[3], prg_pars[1], prg_pars[2]);

  if( NULL == (ph = PQconnectdb(prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3],
                    pgsqlport)) )
  { pgsql_connect_flag = false;
    return true;
  }
  strcpyn(cursqlserver, prg_pars[0], 128);
  strcpyn(cursqluser, prg_pars[1], 128);
  strcpyn(cursqlpwd, prg_pars[2], 128);
  strcpyn(cursqldb, prg_pars[3], 128);
  pgsql_connect_flag = true;
  return false;
}


/***************************************************************************************/
/* int u2w_pgsql_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_query Stellt Anfrage an PGsql-Datenbank, Ergebnisse können dann mit   */
/*                 u2w_pgsql_next_line gelesen und mit pgsql_get_value gelesen werden  */
/***************************************************************************************/
int u2w_pgsql_query(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_pgsql_query.\n");

  if( !pgsql_connect_flag )
    return true;

  if( pa & P2 )
  { if( !isdigit(prg_pars[1][0]) )
      return true;
    cn = atoi(prg_pars[1]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( pgsql_res_flag[cn] )
  { pgsql_res_flag[cn] = false;
    PQclear(pres[cn]);
  }

  LOG(3, "u2w_pgsql_query, query: %s.\n", prg_pars[0]);

  if( NULL == (pres[cn] = u2w_pgsql_query(ph, prg_pars[0])) )
    return true;
  else
  { pgsql_res_flag[cn] = true;
    pgsql_line[cn] = -1;
    return false;
  }
}


/***************************************************************************************/
/* int u2w_pgsql_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_write Stellt Anfrage an PGsql-Datenbank, für insert und update        */
/***************************************************************************************/
int u2w_pgsql_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ PGresult *pres;

  LOG(1, "u2w_pgsql_write, query: %s.\n", prg_pars[0]);

  if( !pgsql_connect_flag )
    return true;

  pres = PQexec(ph, prg_pars[0]);
  if( PQresultStatus(pres) != PGRES_COMMAND_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), prg_pars[0]);
    PQclear(pres);
    return true;
  }
  else
  { pgsql_write_flag = true;
    strcpyn(pgsql_num_rows, PQcmdTuples(pres), 32);
    pgsql_id_flag = true;
    PQclear(pres);
    return false;
  }
}


/***************************************************************************************/
/* int u2w_pgsql_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_test Stellt Anfrage an PGsql-Datenbank, und liefert nur Wahrheitswert */
/***************************************************************************************/
int u2w_pgsql_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ PGresult *lpres;

  LOG(1, "u2w_pgsql_test, query: %s.\n", prg_pars[0]);

  if( !pgsql_connect_flag )
    return true;

  lpres = PQexec(ph, prg_pars[0]);
  if( PQresultStatus(lpres) != PGRES_COMMAND_OK
      && PQresultStatus(lpres) != PGRES_TUPLES_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), prg_pars[0]);
    PQclear(lpres);
    return true;
  }
  else
  { PQclear(lpres);
    pgsql_id_flag = true;
    return false;
  }
}


#define PGSQL_STORE_MODE_APPEND 1
#define PGSQL_PUFFERSIZE 10000000

#ifdef NOCHNICHTFERTIG

/***************************************************************************************/
/* int u2w_pgsql_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_store Datei als blob in Datenbank speichern - Update                  */
/***************************************************************************************/
int u2w_pgsql_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *puffer, *p;
  long nb, i;
  int hd_in, ret;
  int mode = 0;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#endif

  LOG(1, "u2w_pgsql_store, Datei: %s, Tabelle: %s, Feld: %s, ID: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3]);

  if( !pgsql_connect_flag )
    return true;

  if( pa & P5 )
  { switch( prg_pars[4][0] )
    { case 'a': mode = PGSQL_STORE_MODE_APPEND | mode ;
                break;
    }
  }

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_pgsql_store, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { int concatflag;

        LOG(5, "u2w_pgsql_store, grosse Datei.\n");
        if( NULL == (puffer = malloc(PGSQL_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        p = puffer;
        if( mode & PGSQL_STORE_MODE_APPEND )
        { concatflag = true;
          p += snprintf(p, PGSQL_PUFFERSIZE, "update %s set %s=concat(%s,'",
                        prg_pars[1], prg_pars[2], prg_pars[2]);
        }
        else
        { concatflag = false;
          p += snprintf(p, PGSQL_PUFFERSIZE, "update %s set %s='",
                        prg_pars[1], prg_pars[2]);
        }
        LOG(9, "u2w_pgsql_store, grosse Datei vor while.\n");
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
            if( p-puffer >= PGSQL_PUFFERSIZE-100 )
            { LOG(9, "u2w_pgsql_store, grosse Date p-puffer >= 9999900 ...\n");
              if( concatflag )
                snprintf(p, PGSQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                         prg_pars[3]);
              else
              { snprintf(p, PGSQL_PUFFERSIZE - (p-puffer), "' where ID='%s'",
                         prg_pars[3]);
                concatflag = true;
              }
              LOG(9, "u2w_pgsql_store, grosse Datei vor pgsql_query.\n");
              if( pgsql_query(&mh, puffer) )
              { if( pgsql_error_log_flag )
                  logging("pgsql_query error: %s.\n", pgsql_error(&mh));
                close(hd_in);
                free(puffer);
                return true;
              }
              LOG(9, "u2w_pgsql_store, grosse Datei nach pgsql_query.\n");
              snprintf(puffer, PGSQL_PUFFERSIZE, "update %s set %s=concat(%s,'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
              p = puffer + strlen(puffer);
            }
          }
        }
        LOG(7, "u2w_pgsql_store, grosse Datei, concatflag: %d.\n", concatflag);
        if( concatflag )
          snprintf(p, PGSQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        else
          snprintf(p, PGSQL_PUFFERSIZE - (p-puffer), "' where ID='%s'",
                   prg_pars[3]);
        if( pgsql_query(&mh, puffer) )
        { if( pgsql_error_log_flag )
            logging("pgsql_query error: %s.\n", pgsql_error(&mh));
          free(puffer);
          close(hd_in);
          return true;
        }
        free(puffer);
      }
      else
      { LOG(5, "u2w_pgsql_store, kleine Datei.\n");
        if( NULL != (puffer = malloc(100+2*stat_buf.st_size)) )
        { LOG(11, "u2w_pgsql_store, kleine Datei, Puffer allokiert.\n");
          if( mode & PGSQL_STORE_MODE_APPEND )
            sprintf(puffer,"update %s set %s=concat(%s,'",
                     prg_pars[1], prg_pars[2], prg_pars[2]);
          else
            sprintf(puffer, "update %s set %s='", prg_pars[1], prg_pars[2]);
          p = puffer + strlen(puffer);
          while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )
          { LOG(50, "u2w_pgsql_store, kleine Datei, nb: %d.\n", nb);
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
          if( mode & PGSQL_STORE_MODE_APPEND )
            sprintf(p, "') where ID='%s'", prg_pars[3]);
          else
            sprintf(p, "' where ID='%s'", prg_pars[3]);
          LOG(100, "pgsql_store, puffer: %s.\n", puffer);
          if( pgsql_query(&mh, puffer) )
          { if( pgsql_error_log_flag )
              logging("pgsql_query error: %s.\n", pgsql_error(&mh));
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
#endif  /* NOCHNICHTFERTIG */

/***************************************************************************************/
/* int u2w_pgsql_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_out Query ausführen und Ergebnis in Datei schreiben                   */
/***************************************************************************************/
int u2w_pgsql_out(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(3, "u2w_pgsql_out, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  return pgsql2dat(prg_pars[0], prg_pars[1], pa & P3 ? prg_pars[2] : NULL,
                   pa & P4 ? prg_pars[3] : NULL);
}


/***************************************************************************************/
/* int u2w_pgsql_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_get_line bereitet eine Zeile zum lesen vor                            */
/***************************************************************************************/
int u2w_pgsql_get_line(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_pgsql_get_line.\n");

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( pgsql_res_flag[cn] )
  { if( pgsql_line[cn]+1 < PQntuples(pres[cn]) )
    { pgsql_line[cn]++;
      pgsql_next_value[cn] = 0;
      return false;
    }
    else
      return true;
  }
  return true;
}


/***************************************************************************************/
/* int do_pgsql_list_get_line(int *listlen,                                            */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_list_get_line Eine Zeile in Liste                                      */
/***************************************************************************************/
int do_pgsql_list_get_line(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn, i;

  LOG(1, "do_pgsql_list_get_line, listlen: %d.\n", *listlen);

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( pgsql_res_flag[cn] )
  { if( pgsql_line[cn]+1 < PQntuples(pres[cn]) )
    { pgsql_line[cn]++;
      pgsql_next_value[cn] = 0;
      for( i = 0; i < PQnfields(pres[cn]) && *listlen < MAX_LIST_LEN; i++ )
      { if( PQgetvalue(pres[cn], pgsql_line[cn], i) )
          strcpyn(list_pars[(*listlen)++], PQgetvalue(pres[cn], pgsql_line[cn], i),
                  MAX_LEN_LIST_PARS);
        else
          list_pars[(*listlen)++][0] = '\0';
      }
      LOG(17, "/do_pgsql_list_get_line, list_pars[0]: %s.\n", list_pars[0]);
      return false;
    }
    else
    { *listlen = -1;
      return true;
    }
  }
  *listlen = -1;
  return true;
}


/***************************************************************************************/
/* int u2w_pgsql_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])    */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_pgsql_isvalue testet, ob noch ein Wert für pgsqlreadvalue vorliegt          */
/***************************************************************************************/
int u2w_pgsql_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_pgsql_isvalue.\n");

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  return pgsql_line[cn] >= PQntuples(pres[cn])
         || pgsql_next_value[cn] >= PQnfields(pres[cn]);
}


/***************************************************************************************/
/* int do_pgsql_writeline(int pa, char **out, long n,                                  */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_pgsql_writeline fügt Zeile des letzten pgsql_getline ein                    */
/***************************************************************************************/
int do_pgsql_writeline(int pa, char **out, long n,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  char *oo;
  int i;

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( !(pa & P2) )
  { prg_pars[1][0] = ' ';
    prg_pars[1][1] = '\0';
  }

  if( pgsql_line[cn] < PQntuples(pres[cn]) )
  { if( out )
    { oo = *out;
      for( i = 0; i < PQnfields(pres[cn]); i++ )
      { if( i )
          strqcpyn_z(out, prg_pars[1], n - (*out-oo), qf_strings[quote]);
        if( PQgetvalue(pres[cn], pgsql_line[cn], i) )
          strqcpyn_z(out, PQgetvalue(pres[cn], pgsql_line[cn], i), n -(*out-oo),
                      qf_strings[quote]);
      }
    }
    else
    { for( i = 0; i < PQnfields(pres[cn]); i++ )
      { if( pa & P2 )
          dosend(prg_pars[1]);
        if( PQgetvalue(pres[cn], pgsql_line[cn], i) )
        { dobinsendh(PQgetvalue(pres[cn], pgsql_line[cn], i),
                    PQgetlength(pres[cn], pgsql_line[cn], i));
          last_char_sended = last_non_ws_char(PQgetvalue(pres[cn], pgsql_line[cn], i),
                    PQgetlength(pres[cn], pgsql_line[cn], i));
        }
      }
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_pgsql_readwriteline(int pa, char **out, long n,                              */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_readwriteline fügt eine Zeile der letzten pgsql_query ein              */
/***************************************************************************************/
int do_pgsql_readwriteline(int pa, char **out, long n,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !u2w_pgsql_get_line(pa, prg_pars) )
    return do_pgsql_writeline(pa, out, n, prg_pars, quote);
  else
    return true;
}


/***************************************************************************************/
/* int do_pgsql_num_fields(int pa, char **out, long n,                                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_num_fields ergibt Anzahl Spalten der PGsql-Abfrage                     */
/***************************************************************************************/
int do_pgsql_num_fields(int pa, char **out, long n,
                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

 if( !pgsql_res_flag[cn] )
    return true;
  *out += snprintf(*out, n, "%d", PQnfields(pres[cn]));
  return false;
}


/***************************************************************************************/
/* int do_pgsql_value(int pa, char **out, long n,                                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_value fügt einen Wert einer Zeile ein                                  */
/***************************************************************************************/
int do_pgsql_value(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_pgsql_value\n");

  i = 0;

  if( pa & P2 )
  { if( !isdigit(prg_pars[1][0]) )
      return true;
    cn = atoi(prg_pars[1]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  LOG(3, "do_pgsql_value, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_pgsql_value, i: %d, mnf[%d]: %d.\n", i, cn, PQnfields(pres[cn]));
    if( i < PQnfields(pres[cn]) )
    { if( PQgetvalue(pres[cn], pgsql_line[cn], i) )
      { if( out )
          strqcpyn_z(out, PQgetvalue(pres[cn], pgsql_line[cn], i), n,
                     qf_strings[quote]); 
        else
        { dobinsendh(PQgetvalue(pres[cn], pgsql_line[cn], i),
                    PQgetlength(pres[cn], pgsql_line[cn], i));
          last_char_sended = last_non_ws_char(PQgetvalue(pres[cn], pgsql_line[cn], i),
                                              PQgetlength(pres[cn], pgsql_line[cn], i));
        }
      }
    }
    LOG(1, "/do_pgsql_value\n");
    return false;
  }
  LOG(1, "/do_pgsql_value - ERROR\n");
  return true;
}


/***************************************************************************************/
/* int do_pgsql_read_value(int pa, char **out, long n,                                 */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_read_value fügt näcshten Wert einer Zeile ein                          */
/***************************************************************************************/
int do_pgsql_read_value(int pa, char **out, long n,
                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;

  if( pa & P1 )
  { if( !isdigit(prg_pars[0][0]) )
      return true;
    cn = atoi(prg_pars[0]);
    if( cn >= MAX_PP_QUERIES )
      return true;
    cn += MAX_ANZ_INCLUDE;
  }
  else
    cn = include_counter;

  if( pgsql_line[cn] >= PQntuples(pres[cn])
      || pgsql_next_value[cn] >= PQnfields(pres[cn]) )
    return true;

  if( PQgetvalue(pres[cn], pgsql_line[cn], pgsql_next_value[cn]) )
  { if( out )
      strqcpyn_z(out, PQgetvalue(pres[cn], pgsql_line[cn], pgsql_next_value[cn]), n,
                 qf_strings[quote]); 
    else
    { dobinsendh(PQgetvalue(pres[cn], pgsql_line[cn], pgsql_next_value[cn]),
                PQgetlength(pres[cn], pgsql_line[cn], pgsql_next_value[cn]));
      last_char_sended = last_non_ws_char(
                           PQgetvalue(pres[cn], pgsql_line[cn], pgsql_next_value[cn]),
                           PQgetlength(pres[cn], pgsql_line[cn], pgsql_next_value[cn]));
    }
  }
  pgsql_next_value[cn]++;
  return false;
}


/***************************************************************************************/
/* int do_pgsql_id(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_id fügt letzte ID (nach insert) ein                                    */
/***************************************************************************************/
int do_pgsql_id(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( !pgsql_id_flag )
    return true;
  *out += snprintf(*out, n, "%llu", pgsql_insert_id);
  return false;
}


/***************************************************************************************/
/* int do_pgsql_read(int pa, char **out, long n,                                       */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_read Query ausführen und Ergebnis einfügen                             */
/***************************************************************************************/
int do_pgsql_read(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_pgsql_read, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( out )
    return pgsql2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, tablecols, quote);
  else
    return pgsql2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, tablecols);
}


/***************************************************************************************/
/* int do_pgsql_list_read(int *listlen,                                                */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_list_read Query ausführen und Ergebnis in Liste                        */
/***************************************************************************************/
int do_pgsql_list_read(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ PGresult *pres;
  unsigned int nf;
  int i;
  long nz;

  LOG(3, "do_pgsql_list_read, q: %s.\n", prg_pars[0]);

  if( !pgsql_connect_flag )                      /* Aktive PGSQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  
  pres = PQexec(ph, prg_pars[0]);
  if( PQresultStatus(pres) != PGRES_TUPLES_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), prg_pars[0]);
    PQclear(pres);
    return true;
  }
  else
  { LOG(11, "do_pgsql_list_read, nf: %d.\n", nf);
    for( i = 0; i < PQnfields(pres) && *listlen < MAX_LIST_LEN; i++ )
    { if( PQgetvalue(pres, 0, i) )
        strcpyn(list_pars[(*listlen)++], PQgetvalue(pres, 0, i),
                MAX_LEN_LIST_PARS);
       else
        list_pars[(*listlen)++][0] = '\0';
    }
    PQclear(pres);
    return false;
  }
}


/***************************************************************************************/
/* int do_pgsql_idvalue(int pa, char **out, long n,                                    */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_idvalue Einen Wert einer Tabelle bestimemn                             */
/***************************************************************************************/
int do_pgsql_idvalue(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[1024];

  if( prg_pars[0][0] && prg_pars[1][0] && prg_pars[2][0] )
  { snprintf(query, 1024, "select %s from %s where ID='%s'", prg_pars[1], prg_pars[0],
             prg_pars[2]);

    if( out )
      return pgsql2s(query, out, n, false, "", "", false, quote);
    else
      return pgsql2net(query, false, "", "", false);
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_pgsql_ins(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_ins testen, ob Datensatz, der beschrieben wird vohanden ist, dann      */
/*                ID zurück, sonst neuen Datensatz erzeugen und neue ID zurück         */
/***************************************************************************************/
int do_pgsql_ins(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE], iquery[MAX_ZEILENLAENGE];
  char *p, *q;
  int q_flag;

  if( prg_pars[0][0] && prg_pars[1][0] )
  { snprintf(query, MAX_ZEILENLAENGE, "select id from %s where ", prg_pars[0]);
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
    LOG(3, "do_pgsql_ins, query-select: %s.\n", query);

    p = *out;
    pgsql2s(query, out, n, false, "", "", false, quote);
    if( *out == p )
    { snprintf(iquery, MAX_ZEILENLAENGE, "insert into %s set %s", prg_pars[0],
               prg_pars[1]);
      LOG(3, "do_pgsql_ins, query-insert: %s.\n", query);
      return pgsql2s(query, out, n, false, "", "", false, quote);
    }
    return false;
  }
  else
    return true;
}


#ifdef NOCH_NICHT_IMPLEMENTIERT
/***************************************************************************************/
/* int do_pgsql_enums(int pa, char **out, long n,                                      */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_enums Möglichkeiten eines ENUMs oder SETs bestimmen                    */
/***************************************************************************************/
int do_pgsql_enums(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE];
  char qresult[MAX_ZEILENLAENGE], *qr;

  LOG(3, "do_pgsql_enums, pa: %d, %s, %s.\n", pa, prg_pars[0], prg_pars[1]);

  snprintf(query, MAX_ZEILENLAENGE, "show columns from %s like '%s'", prg_pars[0],
           prg_pars[1]);

  qr = qresult;
  if( pgsql2s(query, &qr, MAX_ZEILENLAENGE, false, " ", " ", 0, quote) )
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
#endif


/***************************************************************************************/
/* int do_pgsql_num_rows(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_pgsql_num_rows fügt Anzahl der geänderten/selektierten Zeilen der letzten    */
/*              pgsql_query ein                                                        */
/***************************************************************************************/
int do_pgsql_num_rows(int pa, char **out, long n,
                      char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;

  if( !pgsql_write_flag )
    return true;

  strcpyn_z(out, pgsql_num_rows, n);
  return false;
}


#ifdef NOCH_NICHT_IMPLEMENTIERT
/***************************************************************************************/
/* int do_pgsql_store_ins(int pa, char **out, long n,                                  */
/*                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)*/
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     do_pgsql_store_ins Datei als blob in Datenbank speichern - neuen Datensatz      */
/***************************************************************************************/
int do_pgsql_store_ins(int pa, char **out, long n, int tabelrows,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *puffer, *p;
  long nb, i;
  int hd_in, ret;
  my_ulonglong newid;
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#endif

  LOG(1, "u2w_pgsql_store_ins, Datei: %s, Tabelle: %s, Feld: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2]);

  if( !pgsql_connect_flag )
    return true;

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_pgsql_store_ins, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_pgsql_store_ins, grosse Datei.\n");
        if( NULL == (puffer = malloc(PGSQL_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        newid = 0;
        p = puffer;
        p += snprintf(p, PGSQL_PUFFERSIZE, "insert into %s set %s='",
                      prg_pars[1], prg_pars[2]);
        LOG(9, "u2w_pgsql_store_ins, grosse Datei vor while.\n");
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
            if( p-puffer >= PGSQL_PUFFERSIZE-100 )
            { LOG(9, "u2w_pgsql_store_ins, grosse Date p-puffer >= 9999900 ...\n");
              if( !newid )
                strcpyn(p, "'", PGSQL_PUFFERSIZE - (p-puffer));
              else
                snprintf(p, PGSQL_PUFFERSIZE - (p-puffer), "') where ID='%llu'",
                         newid);
              LOG(9, "u2w_pgsql_store_ins, grosse Datei vor pgsql_query.\n");
              if( pgsql_query(&mh, puffer) )
              { if( pgsql_error_log_flag )
                  logging("pgsql_query error: %s.\n", pgsql_error(&mh));
                close(hd_in);
                free(puffer);
                return true;
              }
              if( !newid )
                newid = pgsql_insert_id(&mh);
              LOG(9, "u2w_pgsql_store_ins, grosse Datei nach pgsql_query.\n");
              snprintf(puffer, PGSQL_PUFFERSIZE, "update %s set %s=concat(%s,'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
              p = puffer + strlen(puffer);
            }
          }
        }
        LOG(7, "u2w_pgsql_store_ins, grosse Datei, newid: %llu.\n", newid);
        if( !newid )
          strcpyn(p, "'", PGSQL_PUFFERSIZE - (p-puffer));
        else
          snprintf(p, PGSQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        if( pgsql_query(&mh, puffer) )
        { if( pgsql_error_log_flag )
            logging("pgsql_query error: %s.\nquery: %s.\n", pgsql_error(&mh), puffer);
          free(puffer);
          close(hd_in);
          return true;
        }
        if( !newid )
          newid = pgsql_insert_id(&mh);
        free(puffer);
      }
      else
      { LOG(5, "u2w_pgsql_store_ins, kleine Datei.\n");
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
          LOG(100, "pgsql_store_ins, puffer: %s.\n", puffer);
          if( pgsql_query(&mh, puffer) )
          { if( pgsql_error_log_flag )
              logging("pgsql_query error: %s.\n", pgsql_error(&mh));
            ret = true;
          }
          else
            ret = false;
          free(puffer);
          newid = pgsql_insert_id(&mh);
        }
        else if( logflag & NORMLOG )
          fprintf(stderr, _("Unable to allocate memory.\n"));
      }
      close(hd_in);
    }
  }
  *out += snprintf(*out, n, "%llu", newid);
  return ret;
}
#endif


/***************************************************************************************/
/* int pgsql2s(char *query, char **o, long n, int error_flag,                          */
/*             char *ssep, char *zsep, int tablecols, int quote)                       */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     pgsql2s fuehrt query aus und schreibt die Ausgabe nach o                        */
/***************************************************************************************/
int pgsql2s(char *query, char **o, long n, int error_flag, char *ssep, char *zsep,
             int tablecols, int quote)
{ PGresult *pres;
  unsigned int nf;
  char *oo;
  int i, ret;
  long nz;

  LOG(1, "pgsql2s, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");
  LOG(5, "pgsql2s, quote: %d.\n", quote);

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
  LOG(3, "pgsql2s, query: %s.\n", query);

  if( !pgsql_connect_flag )                      /* Aktive PGSQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  pres = PQexec(ph, query);
  if( PQresultStatus(pres) != PGRES_TUPLES_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), query);
    if( error_flag && pgsql_error_out_flag )
      *o += snprintf(*o, n, "pgsql_query error: %s, query: %s.",
                     PQerrorMessage(ph), query);
    ret = true;
  }
  else
  { for( nz = 0; nz < PQntuples(pres); nz++ )
    { if( nz )
        strcpyn_z(o, zsep, n - (*o-oo));
      for( i = 0; i < PQnfields(pres); i++ )
      { if( i )
          strqcpyn_z(o, ssep, n - (*o-oo), qf_strings[quote]);
        if( PQgetvalue(pres, nz, i) )
        { if( tablecols && test_leer(PQgetvalue(pres, nz, i), n - (*o-oo)) )
            strcpyn_z(o, "&nbsp;", n - (*o-oo));
          else
            strqcpyn_z(o, PQgetvalue(pres, nz, i), n - (*o-oo), qf_strings[quote]);
        }
        else if( tablecols )
          strcpyn_z(o, "&nbsp;", n - (*o-oo));
      }
    }
  }
  **o = '\0';
  PQclear(pres);

  return ret;                                 /* Anzahl der Zeichen vom Programm    */
}


/***************************************************************************************/
/* int pgsql2net(char *query, int error_flag,                                          */
/*             char *ssep, char *zsep, int tablecols)                                  */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     pgsql2net fuehrt query aus und sendet Ausgabe an Browser                        */
/***************************************************************************************/
int pgsql2net(char *query, int error_flag, char *ssep, char *zsep,
             int tablecols)
{ char o[MAX_ZEILENLAENGE];
  PGresult *pres;
  unsigned int nf;
  int i, ret;
  long nz;

  LOG(1, "pgsql2net, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
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

  if( !pgsql_connect_flag )                      /* Aktive PGSQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  pres = PQexec(ph, query);
  if( PQresultStatus(pres) != PGRES_TUPLES_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), query);
    if( error_flag && pgsql_error_out_flag )
      snprintf(o, MAX_ZEILENLAENGE, "pgsql_query error: %s, query: %s.",
               PQerrorMessage(ph), query);
    ret = true;
  }
  else
  { LOG(9, "do_query2net - Query OK: %s.");
    for( nz = 0; nz < PQntuples(pres); nz++ )
    { if( nz )
        dosend(zsep);
      for( i = 0; i < PQnfields(pres); i++ )
      { if( i )
          dosend(ssep);
        if( PQgetvalue(pres, nz, i) )
        { if( tablecols && test_leer(PQgetvalue(pres, nz, i), PQgetlength(pres, nz, i)) )
            dosend("&nbsp;");
          else
          { dobinsendh(PQgetvalue(pres, nz, i), PQgetlength(pres, nz, i));
              last_char_sended = last_non_ws_char(PQgetvalue(pres, nz, i),
                                                  PQgetlength(pres, nz, i));
          }
        }
        else if( tablecols )
          ret = ret | dosend("&nbsp;");
      }
    }
  }
  PQclear(pres);
  return ret;
}


/***************************************************************************************/
/* int pgsql2dat(char *query, char *path, char *ssep, char *zsep)                      */
/*             char *query: SQL-query                                                  */
/*             char *path: Datei für die Ausgabe                                       */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     pgsql2dat fuehrt query aus und schreibt Ausgabe in Datei path                   */
/***************************************************************************************/
int pgsql2dat(char *query, char *path, char *ssep, char *zsep)
{ PGresult *pres;
  unsigned int nf;
  int i, ret;
  long nz;
  int hd_out;
#ifndef _LARGEFILE64_SOURCE
#define open64(a,b) open(a,b)
#endif

  LOG(1, "pgsql2dat, query: %s, path: %s, ssep: %s, zsep: %s.\n", query, path,
      ssep ? ssep : "NULL", zsep ? zsep : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = "\n";

  ret = false;

  if( !pgsql_connect_flag )                      /* Aktive PGSQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  pres = PQexec(ph, query);
  if( PQresultStatus(pres) != PGRES_TUPLES_OK )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s\nquery: %s.\n", PQerrorMessage(ph), query);
    ret = true;
  }
  else
  { if( 0 <= (hd_out = open64(path, O_WRONLY | O_CREAT | O_TRUNC, 00600)) ) 
    { for( nz = 0; nz < PQntuples(pres); nz++ )
      { if( nz )
        { if( strlen(zsep) < write(hd_out, zsep, strlen(zsep)) )
          { ret = true;
            break;
          }
        }
        for( i = 0; i < PQnfields(pres); i++ )
        { if( i )
          { if( strlen(ssep) < write(hd_out, ssep, strlen(ssep)) )
            { ret = true;
              break;
            }
          }
          if( PQgetlength(pres, nz, i) && PQgetvalue(pres, nz, i) )
          { if( PQgetlength(pres, nz, i) < write(hd_out, PQgetvalue(pres, nz, i),
                                                 PQgetlength(pres, nz, i)) )
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

    PQclear(pres);
    return ret;
  }
}


#ifdef NEW_VERSION
/***************************************************************************************/
/* int pgsql2p(char *query, char *ssep, char *zsep, char *p)                           */
/*             char *query: SQL-query                                                  */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             char *p     : Kommando, an das die Ausgabe geschickt wird               */
/*             return     : true bei kein Wert                                         */
/*     pgsql2p fuehrt query aus und schreibt die Ausgabe in einer pipe an *p           */
/***************************************************************************************/
int pgsql2p(char *query, char *ssep, char *zsep, char *p)
{ PGSQL_RES *mres;
  PGSQL_ROW mrow;
  unsigned int nf;
  int ret;

  LOG(1, "pgsql2p, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = "\n";

  ret = false;

  if( !pgsql_connect_flag )                      /* Aktive PGSQL-Verbindung?           */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( pgsql_query(&mh, query) )
  { if( pgsql_error_log_flag )
      logging("pgsql_query error: %s.\nquery: %s.\n", pgsql_error(&mh), query);
    ret = true;
  }
  else
  { mres = pgsql_store_result(&mh);
    if( mres )
    { nf = pgsql_num_fields(mres);
      nz = 0;
      while( (mrow = pgsql_fetch_row(mres) ) )
      { if( nz++ )
          strcpyn_z(o, zsep, n - (*o-oo));
        for( i = 0; i < nf; i++ )
        { if( i )
            strcpyn_z(o, ssep, n - (*o-oo));
          if( lengths[i] && mrow[i] )
          { if( tablecols && test_leer(mrow[i], lengths[i]) )
              strcpyn_z(o, "&nbsp;", n - (*o-oo));
            else
              *o += snprintf(*o, n - (*o-oo), "%.*s",
                             (int)lengths[i], mrow[i]);
          }
          else if( tablecols )
            strcpyn_z(o, "&nbsp;", n - (*o-oo));
        }
      }
      pgsql_free_result(mres);
    }
    else
    { ret = true;
    }
  }

  return ret;
}
#endif
#endif

