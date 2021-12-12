/**********************************************/
/* File: u2w_sql.c                            */
/* Funktionen zum Ansprechen von Datenbanken  */
/* timestamp: 2015-12-26 14:40:41             */
/**********************************************/

#ifdef DBCLIENT
#include "u2w.h"


int sql_id_flag = false;
unsigned long long sql_id = 0;


/***************************************************************************************/
/* int u2w_sql_connect(wert *w, int pa,                                                */
/*                     char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])              */
/*             wert *w: Rückgabewert                                                   */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_mysql_connect öffnet eine Verbindung zum MySQL-Server                       */
/***************************************************************************************/
int u2w_sql_connect(wert *w, int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(1, "u2w_sql_connect.\n");

  LOG(3, "u2w_sql_connect, server: %s, user: %s, pwd, %s, db: %s.\n", prg_pars[0],
      prg_pars[1], prg_pars[2], prg_pars[3]);

#ifdef MAYDBCLIENT
  if( strcasecmp(prg_pars[0], "MYSQL") )
  { MYSQL *mh;

    if( NULL == (mh = mysql_connect(prg_pars[1], prg_pars[2], prg_pars[3],
                                    prg_pars[4], mysqlport)) )
      return true;
    else
    { *w.s = mh;
      *w.type = DB_MYSQL;
      return false;
    }
  }
#endif
#ifdef POSTGRESQLCLIENT
  if( strcasecmp(prg_pars[0], "MYSQL") )
  { PGconn *ph;

    if( NULL == (ph = pgsql_connect(prg_pars[1], prg_pars[2], prg_pars[3],
                                    prg_pars[4], pgsqlport)) )
      return true;
    else
    { *w.s = ph;
      *w.type = DB_PGSQL;
      return false;
    }
  }
#endif
}


/***************************************************************************************/
/* int u2w_sql_query(wert *w, int pa,                                                  */
/*                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])                */
/*             wert *w: Rückgabewert                                                   */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql_query Stellt Anfrage an Datenbank, Ergebnisse können dann mit           */
/*                 u2w_sql_next_line gelesen und mit mysql_get_value gelesen werden    */
/***************************************************************************************/
int u2w_sql_query(wert *w, int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ wert p1;

  LOG(1, "u2w_sql_query.\n");

  if( NULL != (p1 = getfirstparwert(prg_pars[0])) )
  { switch(p1.type)
    {
#ifdef MAYDBCLIENT
      case DB_MYSQL: { MYSQL_RES *res;
                       if( NULL != (res = sql_mysql_query((MYSQL *)p1.wert,
                                                          prg_pars[1])) )
                       { *w.s = res;
                         *w.type = RES_MYSQL;
                         return false;
                       }
                     }
                     break;
#endif
#ifdef POSTGRESQLCLIENT
      case DB_PGSQL: { PGresult *res;
                       if( NULL != (res = sql_pgsql_query((PGconn *)p1.wert,
                                                          prg_pars[1])) )
                       { *w.s = res;
                         *w.type = RES_PGSQL;
                         return false;
                       }
                     }
                     break;
#endif
    }
  }
  return true;
}


/***************************************************************************************/
/* int u2w_sql_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql_query Stellt Anfrage an Datenbank, die kein Ergebnis liefert            */
/***************************************************************************************/
int u2w_sql_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ wert p1;

  LOG(1, "u2w_sql_write.\n");

  if( NULL != (p1 = getfirstparwert(prg_pars[0])) )
  { switch(p1.type)
    {
#ifdef MAYDBCLIENT
      case DB_MYSQL: return sql_mysql_write((MYSQL *)p1.wert, prg_pars[1]);
                     break;
#endif
#ifdef POSTGRESQLCLIENT
      case DB_PGSQL: return sql_pgsql_write((PGconn *)p1.wert, prg_pars[1]);
                     break;
#endif
    }
  }
  return true;
}


/***************************************************************************************/
/* int u2w_sql_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql_test Stellt Anfrage an Datenbank, und liefert Wahrheitswert             */
/***************************************************************************************/
int u2w_sql_test(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ wert p1;

  LOG(1, "u2w_sql_write.\n");

  if( NULL != (p1 = getfirstparwert(prg_pars[0])) )
  { switch(p1.type)
    {
#ifdef MAYDBCLIENT
      case DB_MYSQL: return sql_mysql_test((MYSQL *)p1.wert, prg_pars[1]);
                     break;
#endif
#ifdef POSTGRESQLCLIENT
      case DB_PGSQL: return sql_pgsql_test((PGconn *)p1.wert, prg_pars[1]);
                     break;
#endif
    }
  }
  return true;
}



          strcpynm(list_pars[(*listlen)++], mrow[cn][i], MAX_LEN_LIST_PARS,
                   mlengths[cn][i]);
        else
          list_pars[(*listlen)++][0] = '\0';
      }
      LOG(17, "/do_sql_list_get_line, list_pars[0]: %s.\n", list_pars[0]);
      return false;
    }
    else
    { sql_line_flag[cn] = false;
      mnf[cn] = 0;
      *listlen = -1;
      return true;
    }
  }
  sql_line_flag[cn] = false;
  mnf[cn] = 0;
  *listlen = -1;
  return true;
}


/***************************************************************************************/
/* int u2w_sql_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_sql_isvalue testet, ob noch ein Wert für sqlreadvalue vorliegt              */
/***************************************************************************************/
int u2w_sql_isvalue(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int cn;

  LOG(1, "u2w_sql_isvalue.\n");

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

  return !sql_line_flag[cn] || sql_next_value[cn] >= mnf[cn];
}


/***************************************************************************************/
/* int do_sql_writeline(int pa, char **out, long n,                                    */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_sql_writeline fügt Zeile des letzten sql_getline ein                        */
/***************************************************************************************/
int do_sql_writeline(int pa, char **out, long n,
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

  if( sql_line_flag[cn] )
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
          dosend(prg_pars[1]);
        if( mrow[cn][i] )
        { dobinsend(mrow[cn][i], mlengths[cn][i]);
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
/* int do_sql_readwriteline(int pa, char **out, long n,                                */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_readwriteline fügt eine Zeile der letzten sql_query ein                  */
/***************************************************************************************/
int do_sql_readwriteline(int pa, char **out, long n,
                           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !u2w_sql_get_line(pa, prg_pars) )
    return do_sql_writeline(pa, out, n, prg_pars, quote);
  else
    return true;
}


/***************************************************************************************/
/* int do_sql_num_fields(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_num_fields ergibt Anzahl Spalten der Sql-Abfrage                         */
/***************************************************************************************/
int do_sql_num_fields(int pa, char **out, long n,
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

 if( !sql_res_flag[cn] )
    return true;
  snprintf(*out, n, "%d", mnf[cn]);
  *out += strlen(*out);
  return false;
}


/***************************************************************************************/
/* int do_sql_value(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_value fügt einen Wert einer Zeile ein                                    */
/***************************************************************************************/
int do_sql_value(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ unsigned int cn;
  int i;

  LOG(1, "do_sql_value\n");

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

  LOG(3, "do_sql_value, cn: %d, p1: %s.\n", cn, prg_pars[0]);

  if( isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);

    LOG(5, "do_sql_value, i: %d, mnf[%d]: %d.\n", i, cn, mnf[cn]);
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
    LOG(1, "/do_sql_value\n");
    return false;
  }
  LOG(1, "/do_sql_value - ERROR\n");
  return true;
}


/***************************************************************************************/
/* int do_sql_read_value(int pa, char **out, long n,                                   */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_read_value fügt näcshten Wert einer Zeile ein                            */
/***************************************************************************************/
int do_sql_read_value(int pa, char **out, long n,
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

  if( !sql_line_flag[cn] || sql_next_value[cn] >= mnf[cn] )
    return true;

  if( mrow[cn][sql_next_value[cn]] )
  { if( out )
      strqcpynm_z(out, mrow[cn][sql_next_value[cn]], n,
                  mlengths[cn][sql_next_value[cn]], qf_strings[quote]); 
    else
    { dobinsend(mrow[cn][sql_next_value[cn]], mlengths[cn][sql_next_value[cn]]);
      last_char_sended = last_non_ws_char(mrow[cn][sql_next_value[cn]],
                                          mlengths[cn][sql_next_value[cn]]);
    }
  }

  sql_next_value[cn]++;
  return false;
}


/***************************************************************************************/
/* int do_sql_id(int pa, char **out, long n,                                           */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql fügt letzte ID (nach insert) ein                                         */
/***************************************************************************************/
int do_sql_id(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( !sql_id_flag )
    return true;
  snprintf(*out, n, "%llu", sql_insert_id(&mh));
  *out += strlen(*out);
  return false;
}


/***************************************************************************************/
/* int do_sql_read(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_read Query ausführen und Ergebnis einfügen                               */
/***************************************************************************************/
int do_sql_read(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  LOG(3, "do_sql_read, pa: %d, %s, %s.\n", pa, pa & P2 ? prg_pars[1] : "",
      pa & P3 ? prg_pars[2] : "");

  if( out )
    return sql2s(prg_pars[0], out, n, false, pa & P2 ? prg_pars[1] : NULL,
                   pa & P3 ? prg_pars[2] : NULL, tablecols, quote);
  else
    return sql2net(prg_pars[0], true, pa & P2 ? prg_pars[1] : NULL,
                     pa & P3 ? prg_pars[2] : NULL, tablecols);
}


/***************************************************************************************/
/* int do_sql_list_read(int *listlen,                                                  */
/*                        char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],             */
/*                        int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])   */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_list_read Query ausführen und Ergebnis in Liste                          */
/***************************************************************************************/
int do_sql_list_read(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                       int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ SQL_RES *mres;
  SQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i;
  long nz;

  LOG(3, "do_sql_list_read, q: %s.\n", prg_pars[0]);

  if( !sql_connect_flag )                        /* Aktive Sql-Verbindung?             */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( sql_query(&mh, prg_pars[0]) )
  { return true;
  }
  else
  { mres = sql_store_result(&mh);
    if( mres )
    { nf = sql_num_fields(mres);
      LOG(11, "do_sql_list_read, nf: %d.\n", nf);
      if( mrow = sql_fetch_row(mres) )
      { lengths = sql_fetch_lengths(mres);
        for( i = 0; i < nf && *listlen < MAX_LIST_LEN; i++ )
        { if( lengths[i] && mrow[i] )
            strcpynm(list_pars[(*listlen)++], mrow[i], MAX_LEN_LIST_PARS, lengths[i]);
          else
            list_pars[(*listlen)++][0] = '\0';
        }
      }
      sql_free_result(mres);
      return false;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* int do_sql_wert(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_wert Einen Wert einer Tabelle bestimemn                                  */
/***************************************************************************************/
int do_sql_wert(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[1024];

  if( prg_pars[0][0] && prg_pars[1][0] && prg_pars[2][0] )
  { snprintf(query, 1024, "select %s from %s where ID='%s'", prg_pars[1], prg_pars[0],
             prg_pars[2]);

    if( out )
      return sql2s(query, out, n, false, "", "", false, quote);
    else
      return sql2net(query, false, "", "", false);
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_sql_ins(int pa, char **out, long n,                                          */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_ins testen, ob Datensatz, der beschrieben wird vohanden ist, dann        */
/*                ID zurück, sonst neuen Datensatz erzeugen und neue ID zurück         */
/***************************************************************************************/
int do_sql_ins(int pa, char **out, long n,
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
    LOG(3, "do_sql_ins, query-select: %s.\n", query);

    p = *out;
    sql2s(query, out, n, false, "", "", false, quote);
    if( *out == p )
    { snprintf(query, MAX_ZEILENLAENGE, "insert into %s set %s", prg_pars[0],
               prg_pars[1]);
      LOG(3, "do_sql_ins, query-insert: %s.\n", query);
      return sqlinsert2s(query, out, n, false);
    }
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int do_sql_enums(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_enums Möglichkeiten eines ENUMs oder SETs bestimmen                      */
/***************************************************************************************/
int do_sql_enums(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char query[MAX_ZEILENLAENGE];
  char qresult[MAX_ZEILENLAENGE], *qr;

  LOG(3, "do_sql_enums, pa: %d, %s, %s.\n", pa, prg_pars[0], prg_pars[1]);

  snprintf(query, MAX_ZEILENLAENGE, "show columns from %s like '%s'", prg_pars[0],
           prg_pars[1]);

  qr = qresult;
  if( sql2s(query, &qr, MAX_ZEILENLAENGE, false, " ", " ", 0, quote) )
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
/* int do_sql_num_rows(int pa, char **out, long n,                                     */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sql_num_rows fügt Anzahl der geänderten/selektierten Zeilen der letzten      */
/*              sql_query ein                                                          */
/***************************************************************************************/
int do_sql_num_rows(int pa, char **out, long n,
                      char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  if( !sql_write_flag )
    return true;

  snprintf(*out, n, "%llu", sql_affected_rows(&mh));
  *out += strlen(*out);
  return false;
}


/***************************************************************************************/
/* int do_sql_store_ins(int pa, char **out, long n,                                    */
/*                        char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)*/
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     do_sql_store_ins Datei als blob in Datenbank speichern - neuen Datensatz        */
/***************************************************************************************/
int do_sql_store_ins(int pa, char **out, long n, int tabelrows,
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

  LOG(1, "u2w_sql_store_ins, Datei: %s, Tabelle: %s, Feld: %s.\n",
      prg_pars[0], prg_pars[1], prg_pars[2]);

  if( !sql_connect_flag )
    return true;

  if( (!stat64(prg_pars[0], &stat_buf)               /* Ist Datei vorhanden?           */
       && (S_IFREG & stat_buf.st_mode) == S_IFREG))  /* und normale Datei              */
  { if( 0 <= (hd_in = open64(prg_pars[0], O_RDONLY)) )  /* Datei oeffnen               */
    { LOG(3, "u2w_sql_store_ins, dateigroesse: %ld.\n", stat_buf.st_size);
      if( stat_buf.st_size > 8*1024*1024 )
      { LOG(5, "u2w_sql_store_ins, grosse Datei.\n");
        if( NULL == (puffer = malloc(SQL_PUFFERSIZE)) )
        { if( logflag & NORMLOG )
            fprintf(stderr, _("Unable to allocate memory.\n"));
          close(hd_in);
          return true;
        }

        newid = 0;
        p = puffer;
        snprintf(p, SQL_PUFFERSIZE, "insert into %s set %s='",
                 prg_pars[1], prg_pars[2]);
        p += strlen(p);
        LOG(9, "u2w_sql_store_ins, grosse Datei vor while.\n");
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
            if( p-puffer >= SQL_PUFFERSIZE-100 )
            { LOG(9, "u2w_sql_store_ins, grosse Date p-puffer >= 9999900 ...\n");
              if( !newid )
                strcpyn(p, "'", SQL_PUFFERSIZE - (p-puffer));
              else
                snprintf(p, SQL_PUFFERSIZE - (p-puffer), "') where ID='%llu'",
                         newid);
              LOG(9, "u2w_sql_store_ins, grosse Datei vor sql_query.\n");
              if( sql_query(&mh, puffer) )
              { if( logflag & NORMLOG )
                  fprintf(stderr, "sql_query error: %s.\n", sql_error(&mh));
                if( sql_error_log_flag )
                  logging("sql_query error: %s.\n", sql_error(&mh));
                close(hd_in);
                free(puffer);
                return true;
              }
              if( !newid )
                newid = sql_insert_id(&mh);
              LOG(9, "u2w_sql_store_ins, grosse Datei nach sql_query.\n");
              snprintf(puffer, SQL_PUFFERSIZE, "update %s set %s=concat(%s,'",
                       prg_pars[1], prg_pars[2], prg_pars[2]);
              p = puffer + strlen(puffer);
            }
          }
        }
        LOG(7, "u2w_sql_store_ins, grosse Datei, newid: %llu.\n", newid);
        if( !newid )
          strcpyn(p, "'", SQL_PUFFERSIZE - (p-puffer));
        else
          snprintf(p, SQL_PUFFERSIZE - (p-puffer), "') where ID='%s'",
                   prg_pars[3]);
        if( sql_query(&mh, puffer) )
        { if( logflag & NORMLOG )
            fprintf(stderr, "sql_query error: %s.\nquery: %s.\n",
                    sql_error(&mh), puffer);
          if( sql_error_log_flag )
            logging("sql_query error: %s.\nquery: %s.\n", sql_error(&mh), puffer);
          free(puffer);
          close(hd_in);
          return true;
        }
        if( !newid )
          newid = sql_insert_id(&mh);
        free(puffer);
      }
      else
      { LOG(5, "u2w_sql_store_ins, kleine Datei.\n");
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
          LOG(100, "sql_store_ins, puffer: %s.\n", puffer);
          if( sql_query(&mh, puffer) )
          { if( logflag & NORMLOG )
              fprintf(stderr, "sql_query error: %s.\n", sql_error(&mh));
            if( sql_error_log_flag )
              logging("sql_query error: %s.\n", sql_error(&mh));
            ret = true;
          }
          else
            ret = false;
          free(puffer);
          newid = sql_insert_id(&mh);
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
/* int sql2s(char *query, char **o, long n, int error_flag,                            */
/*             char *ssep, char *zsep, int tablecols, int quote)                       */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     sql2s fuehrt query aus und schreibt die Ausgabe nach o                          */
/***************************************************************************************/
int sql2s(char *query, char **o, long n, int error_flag, char *ssep, char *zsep,
             int tablecols, int quote)
{ SQL_RES *mres;
  SQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  char *oo;
  int i, ret;
  long nz;

  LOG(1, "sql2s, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
      zsep ? zsep : "NULL");
  LOG(5, "sql2s, quote: %d.\n", quote);

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
  LOG(3, "sql2s, query: %s.\n", query);

  if( !sql_connect_flag )                        /* Aktive Sql-Verbindung?             */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( sql_query(&mh, query) )
  { if( sql_error_log_flag )
      logging("sql_query error: %s.\nquery: %s.\n", sql_error(&mh), query);
    if( error_flag )
    { snprintf(*o, n, "sql_query error: %s, query: %s.",
               sql_error(&mh), query);
      *o += strlen(*o);
    }
    ret = true;
  }
  else
  { mres = sql_store_result(&mh);
    if( mres )
    { nf = sql_num_fields(mres);
      LOG(11, "sql2s, nf: %d.\n", nf);
      nz = 0;
      while( (mrow = sql_fetch_row(mres) ) )
      { if( nz++ )
          strcpyn_z(o, zsep, n - (*o-oo));
        lengths = sql_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            strqcpyn_z(o, ssep, n - (*o-oo), qf_strings[quote]);
          strqcpynm_z(o, mrow[i], n - (*o-oo), lengths[i], qf_strings[quote]);
        }
      }
      sql_free_result(mres);
    }
    else
    { if( error_flag && sql_errno(&mh) )
      { snprintf(*o, n - (*o-oo), "Error: %s.", sql_error(&mh));
        *o += strlen(*o);
      }
      ret = true;
    }
  }
  **o = '\0';

  return ret;                                    /* Anzahl der Zeichen vom Programm    */
}


/***************************************************************************************/
/* int sql2net(char *query, int error_flag,                                            */
/*             char *ssep, char *zsep, int tablecols)                                  */
/*             char *query: SQL-query                                                  */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             return     : true bei kein Wert                                         */
/*     sql2net fuehrt query aus und sendet Ausgabe an Browser                          */
/***************************************************************************************/
int sql2net(char *query, int error_flag, char *ssep, char *zsep,
             int tablecols)
{ char o[MAX_ZEILENLAENGE];
  SQL_RES *mres;
  SQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i, ret;
  long nz;

  LOG(1, "sql2net, query: %s, ssep: %s, zsep: %s.\n", query, ssep ? ssep : "NULL",
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

  if( !sql_connect_flag )                        /* Aktive Sql-Verbindung?             */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( sql_query(&mh, query) )
  { if( sql_error_log_flag )
      logging("sql_query error: %s.\nquery: %s.\n", sql_error(&mh), query);
    if( error_flag )
    { snprintf(o, MAX_ZEILENLAENGE, "sql_query error: %s, query: %s.",
               sql_error(&mh), query);
      dosend(o);
    }
    ret = true;
  }
  else
  { mres = sql_store_result(&mh);
    if( mres )
    { nf = sql_num_fields(mres);
      nz = 0;
      while( (mrow = sql_fetch_row(mres) ) )
      { if( nz++ )
          dosend(zsep);
        lengths = sql_fetch_lengths(mres);
        for( i = 0; i < nf; i++ )
        { if( i )
            dosend(ssep);
          if( lengths[i] && mrow[i] )
          { LOG(9, "sql2net, length: %lu.\n", lengths[i]);
            dobinsend(mrow[i], lengths[i]);
            last_char_sended = last_non_ws_char(mrow[i], lengths[i]);
          }
          else
            last_char_sended = '\0';
        }
      }
      sql_free_result(mres);
    }
    else
    { if( error_flag && sql_errno(&mh) )
      { snprintf(o, MAX_ZEILENLAENGE, "Error: %s.", sql_error(&mh));
        dosend(o);
      }
      ret = true;
    }
  }

  return ret;
}


/***************************************************************************************/
/* int sql2dat(char *query, char *path, char *ssep, char *zsep)                        */
/*             char *query: SQL-query                                                  */
/*             char *path: Datei für die Ausgabe                                       */
/*             char *ssep  : Trennzeichen zwischen einzelnen Spalten                   */
/*             char *zsep  : Trennzeichen zwischen einzelnen Zeilen                    */
/*             return     : true bei kein Wert                                         */
/*     sql2dat fuehrt query aus und schreibt Ausgabe in Datei path                     */
/***************************************************************************************/
int sql2dat(char *query, char *path, char *ssep, char *zsep)
{ SQL_RES *mres;
  SQL_ROW mrow;
  unsigned int nf;
  unsigned long *lengths;
  int i, ret;
  long nz;
  int hd_out;
#ifndef _LARGEFILE64_SOURCE
#define open64(a,b) open(a,b)
#endif

  LOG(1, "sql2dat, query: %s, path: %s, ssep: %s, zsep: %s.\n", query, path,
      ssep ? ssep : "NULL", zsep ? zsep : "NULL");

  if( !ssep )
    ssep = " ";

  if( !zsep )
    zsep = "\n";

  ret = false;

  if( !sql_connect_flag )                        /* Aktive Sql-Verbindung?             */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( sql_query(&mh, query) )
  { if( sql_error_log_flag )
      logging("sql_query error: %s.\nquery: %s.\n", sql_error(&mh), query);
    return true;
  }
  else
  { if( (mres = sql_store_result(&mh)) )
    { if( 0 <= (hd_out = open64(path, O_WRONLY | O_CREAT | O_TRUNC, 00600)) ) 
      { nf = sql_num_fields(mres);
        nz = 0;
        while( !ret && (mrow = sql_fetch_row(mres) ) )
        { if( nz++ )
          { if( strlen(zsep) < write(hd_out, zsep, strlen(zsep)) )
            { ret = true;
              break;
            }
          }
          lengths = sql_fetch_lengths(mres);
          for( i = 0; i < nf; i++ )
          { if( i )
            { if( strlen(ssep) < write(hd_out, ssep, strlen(ssep)) )
              { ret = true;
                break;
              }
            }
            if( lengths[i] && mrow[i] )
            { LOG(99, "sql2dat, length: %lu.\n", lengths[i]);
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
      sql_free_result(mres);
      return ret;
    }
    else
      return true;
  }
}


/***************************************************************************************/
/* int sqlinsert2s(char *query, char **o, long n, int error_flag)                      */
/*             char *query: SQL-query                                                  */
/*             char **o    : Ausgaben der Query hier hinein                            */
/*             long n     : Platz in b                                                 */
/*             int error_flag: true, Fehler ausgeben                                   */
/*             return     : true bei Fehler                                            */
/*     sqlinsert2s führt query aus und schreibt eingefügte ID nach o                   */
/***************************************************************************************/
int sqlinsert2s(char *query, char **o, long n, int error_flag)
{
  LOG(1, "sqlinsert2s, query: %s.\n", query);

  if( !sql_connect_flag )                        /* Aktive Sql-Verbindung?             */
    return true;                                 /* nein, dann keine Ausgabe           */

  if( sql_query(&mh, query) )
  { if( sql_error_log_flag )
      logging("sql_query error: %s.\nquery: %s.\n", sql_error(&mh), query);
    if( error_flag )
    { snprintf(*o, n, "sql_query error: %s, query: %s.",
               sql_error(&mh), query);
      *o += strlen(*o);
    }
    return true;
  }
  else
  { LOG(5, "sqlinsert2s, sql_insert_id: %llu.\n", sql_insert_id(&mh));
    snprintf(*o, n, "%llu", sql_insert_id(&mh));
    *o += strlen(*o);
  }
  return false;
}
#endif
