/**************************************************/
/* File: u2w_par.c                                */
/* Funktionen fuer Aufrufparameter aus Formularen */
/* timestamp: 2017-02-12 21:22:46                 */
/**************************************************/


#include "u2w.h"


#ifdef WEBSERVER
/***************************************************************************************/
/* void conv_get(char *s)                                                              */
/*               char *s: String, der konvertiert werden soll                          */
/*      conv_get wertet Sonderzeichen in der Anforderung aus                           */
/***************************************************************************************/
void conv_get(char *s)
{ char *in, *out;
  int i;

  in = out = s;
  while( *in )
  { if( *in == '%' )                         /* EscapeZeichen                          */
    { if( 0 < sscanf(in, "%%%2x", &i) )      /* Sonderzeichen auslesen                 */
#ifdef UNIX
        if( i != 13 )                        /* Unter Unix CR nicht auswerten          */
#endif
          *out++ = (char)i;
      in += 3;
    }
    else if( *in == '+' )
    { *out++ = ' ';
      in++;
    }
    else
      *out++ = *in++;
  }
  *out = '\0';
}


/***************************************************************************************/
/* int get_parwerte(char *par)                                                         */
/*                  char *par      : Parameter vom Browser erhalten                    */
/*     get_parwerte Speichert die Parameter mit ihren Werten in pars[]                 */
/***************************************************************************************/
int get_parwerte(char *par)
{ char *p;                                               /* zeigt auf par              */
  char name[MAX_PAR_NAME_LEN];                           /* Name eines Parameters      */
  char wert[PARLEN];                                     /* Wert eines Parameters      */
  char *w;                                               /* zeigt auf Werte            */

  LOG(1, "get_parwerte, par: %s.\n", par);

  p = par;
  while( *p )
  { strcpyn_str(name, &p, "=", MAX_PAR_NAME_LEN);        /* bis zum '=' zum Namen      */
    if( *p == '=' )                                      /* '=' gefunden?              */
    { p++;                                               /* '=' ueberspringen          */
      w = wert;                                          /* Wert dahin                 */
      while( *p && *p != '&' && w-wert < PARLEN )        /* bis zum '&' | '\0' ist Wert*/
      { *w++ = *p++;
      }
      *w = '\0';
      conv_get(name);
      conv_get(wert);
      store_parwert(name, wert, HP_BROWSER_LEVEL);       /* Parameter speichern        */
    }
    if( *p == '&' )                                      /* '&' ueberspringen          */
      p++;
  }
  LOG(2, "/get_parwerte, par: %s.\n", par);

  return true;
}

#else  /* WEBSERVER */

/***************************************************************************************/
/* int get_parwerte(int options, int argc, char **argv)                                */
/*                  int options: Anzahl bereits ausgewerteter Optionen                 */
/*                  int argc: argc von main                                            */
/*                  char **argv: argv von main                                         */
/*     get_parwerte Speichert die Parameter mit ihren Werten in pars[]                 */
/***************************************************************************************/
int get_parwerte(int options, int argc, char **argv)
{ char *p;                                               /* zeigt auf *argv            */
  char name[MAX_PAR_NAME_LEN];                           /* Name eines Parameters      */

  LOG(1, "get_parwerte, options: %d, argc: %d, argv[options]: %s.\n",
      options, argc, argv[options]);

  while( options < argc )
  { p = argv[options++];
    strcpyn_str(name, &p, "=", MAX_PAR_NAME_LEN);        /* bis zum '=' zum Namen      */
    if( *p == '=' )                                      /* '=' gefunden?              */
    { p++;                                               /* '=' ueberspringen          */
      store_parwert(name, p, HP_BROWSER_LEVEL);          /* Parameter speichern        */
    }
  }
  LOG(2, "/get_parwerte.\n");

  return true;
}
#endif  /* #else WEBSERVER */


/***************************************************************************************/
/* int set_initpar(char *arg)                                                          */
/*                 char *arg: arg hinter -Xd                                           */
/*     set_initpar Speichert die Parameter mit ihren Werten in initpars[]              */
/***************************************************************************************/
int set_initpar(char *arg)
{ char *p;                                               /* zeigt auf *argv            */
  char name[MAX_PAR_NAME_LEN];                           /* Name eines Parameters      */

  LOG(1, "set_systempar, arg: %s.\n", arg);

  p = arg;
  strcpyn_str(name, &p, "=", MAX_PAR_NAME_LEN);          /* bis zum '=' zum Namen      */
  if( *p == '=' )                                        /* '=' gefunden?              */
  { p++;                                                 /* '=' ueberspringen          */
    store_inipar(name, p, false);                        /* Parameter speichern        */
  }

  return true;
}


/***************************************************************************************/
/* hparstype *gethp(char *parname, short level)                                        */
/*             char *parname  : Parametername                                          */
/*             short level    : Skope bzw. bei < 0 Browser, cookie, HTTP-header, sys   */
/*      gethp Bestimmt Zeiger auf Wert in Hash-Tabelle                                 */
/***************************************************************************************/
#ifdef __clang__
hparstype *gethp(char *parname, short level)
#else
inline hparstype *gethp(char *parname, short level)
#endif
{ hparstype *hp;
  unsigned int h;

  LOG(7, "gethp, parname: %s, level: %hd.\n", parname, level);

  h = var_hash(parname);
  if( (hp = phash[h]) )
  { if( level < 0 )
    { while( hp )
      { if( hp->level == level && 0 == strcmp(parname, hp->name) )
          return hp;
        else
          hp = hp->next;
      }
    }
    else if( no_browservars_flag )
    { short m;
      hparstype *mhp;

      m = -100;
      while( hp )
      { if( 0 <= hp->level && hp->level <= level && 0 == strcmp(parname, hp->name) )
        { if( hp->level > m )
          { m = hp->level;
            mhp = hp;
          }
        }
        hp = hp->next;
      }
      if( m != -100 )
        return mhp;
    } 
    else
    { short m;
      hparstype *mhp;

      m = -100;
      while( hp )
      { if( HP_BROWSER_SYS_LEVEL <= hp->level && hp->level <= level && 0 == strcmp(parname, hp->name) )
        { if( hp->level > m )
          { m = hp->level;
            mhp = hp;
          }
        }
        hp = hp->next;
      }
      if( m != -100 )
        return mhp;
    } 
  }
  LOG(9, "/gethp, return: NULL.\n");
  return NULL;
}


/***************************************************************************************/
/* hiparstype *gethip(char *parname)                                                   */
/*             char *parname  : Parametername                                          */
/*      gethip Bestimmt Zeiger auf INIT-Wert in Hash-Tabelle                           */
/***************************************************************************************/
#ifdef CYGWIN
hiparstype *gethip(char *parname)
#else
inline hiparstype *gethip(char *parname)
#endif
{ hiparstype *hip;
  unsigned int h;

  LOG(7, "gethip, parname: %s.\n", parname);

  h = var_hash(parname);
  if( (hip = inihash[h]) )
  { while( hip )
    { if( 0 == strcmp(parname, hip->name) )
        return hip;
      else
        hip = hip->next;
    }
  }
  LOG(9, "/gethip, return: NULL.\n");
  return NULL;
}


/***************************************************************************************/
/* wert getfirstparwert(char **in)                                                     */
/*                      char **in: *in zeigt auf den gelesenen String                  */
/*      getfirstparwert Bestimmt ersten Wert zum Parameter, auf den *in zeigt          */
/***************************************************************************************/
wert getfirstparwert(char **in)
{ char parname[PARLEN];
  char *pp;
  static wert nullwert;
  hparstype *hp;
  hiparstype *hip;

  nullwert.type = EMPTYW;

  LOG(1, "getfirstparwert, in: %s.\n", *in);

  (*in)++;                                  /* PAR_CHAR ueberlesen                     */

  if( **in == '{' )
  { (*in)++;
    scan_to(parname, in, PARLEN, 0, "}", '\0');
    if( **in == '}' )
      (*in)++;
  }
  else if( **in == PAR_CHAR )
  { pp = parname;
    getpar(&pp, in, PARLEN, '\0', QF_NONE);
    *pp = '\0';
  }
  else
  { pp = parname;
    if( **in == ATTRIB_CHAR )
      *pp++ = *(*in)++;
    while( pp-parname < PARLEN && (isalnum(**in) || **in == '_') )
      *pp++ = *(*in)++;
    *pp = '\0';
  }

  LOG(3, "getfirstparwert, parname: %s.\n", parname);

  if( parname[0] == ATTRIB_CHAR )
  { if( (hip = gethip(parname+1)) )
      return hip->wert;
    else
      return nullwert;
  }
  else if( (hp = gethp(parname, include_counter)) )
  { LOG(4, "getfirstparwert, parname: %s, hp->name: %s, wert: %s.\n",
        parname, hp->name, wert2string(hp->werte->wert));
    return hp->werte->wert;
  }
  return nullwert;
}


/***************************************************************************************/
/* int wpar_sep(char **p, wertetype *w, char *quote, size_t n, char trenn,             */
/*               int quote_mode, char *sep, unsigned int flags)                        */
/*               char **p      : zum Aufbauen des Parameters                           */
/*               wertetype *w  : Zeiger auf Wert(e)                                    */
/*               char *quote   : ggf. Quotingzeichen                                   */
/*               size_t n      : noch n Zeichen Platz in p                             */
/*               char trenn    : Zeichen, das die einzelnen Werte umschliesst          */
/*               int quote_mode: Quotingmodus                                          */
/*               char *sep     : Trennung zwischen den einzelenen Werten               */
/*             unsigned int flags: 1: Bei Pufferüberlauf return 2 statt 1              */
/*                                 2: links trimmen  (noch nicht implementiert)        */
/*                                 4: rechts trimmen (noch nicht implementiert)        */
/*   return 1: wenn n ueberschritten wird, bzw. 2, wenn (flags&1)                      */
/*      wpar_sep Werte zu w ausgeben                                                   */
/***************************************************************************************/
int wpar_sep(char **p, wertetype *w, char *quote, size_t n, char trenn,
              int quote_mode, char *sep, unsigned int flags)
{ char *pp;
  char *q;

  LOG(3, "wpar_sep, w: %x.\n", w);

  if( *sep == '\n' && *(sep+1) == '\0' && quote_mode == QF_HTML )
    sep = LINEEND;

  pp = *p;
  while( w != NULL && *p - pp < n - 3 )
  {
    if( skip_empty_flag && w->wert.type == EMPTYW ) /* ggf. leere Param. ignorieren*/
    { LOG(7, "wpar_sep, EMPTYW.\n");
      return 0;
    }
    LOG(7, "wpar_sep, NO EMPTYW, wert: %.30s.\n", wert2string(w->wert));
    if( trenn )
      *(*p)++ = trenn;

    if( quote_mode == QF_HTML )
    { if( code_html(p, wert2string(w->wert), n-(*p-pp)-2) )
        return 1;
    }
    else
#ifdef MAYDBCLIENT
    if( !quote && quote_mode == QF_MYSQL )
    { if( w_mysql_cpyn_z(p, &(w->wert), n-(*p-pp)-2) )
        return 1;
    }
    else
#endif
#ifdef SQLITE3
    if( !quote && quote_mode == QF_SQLITE3 )
    { if( w_sqlite3_cpyn_z(p, &(w->wert), n-(*p-pp)-2) )
        return 1;
    }
    else
#endif
    { if( quote )
        q = quote;
      else if( quote_mode != QF_NONE )
      {
#ifdef MAYDBCLIENT
        switch( quote_mode )
        { case QF_MYSQL: q = "\\'";
            break;
          default:       q = "";
        }
#else
       q = "";
#endif
      }
      else
        q = globalquote;
      if( quote_mode == QF_SHELL )
      { if( strcpyn_shell_z(p, wert2string(w->wert), n-(*p-pp)-2) )
          return 1;
      }
      else if( *q && *(q+1) )
      { if( strqcpyn_z(p, wert2string(w->wert), n-(*p-pp)-2, q) )
          return 1;
      }
      else
      { if( strcpyn_z(p, wert2string(w->wert), n-(*p-pp)-2) )
          return 1;
      }
    }
    if( trenn )
      *(*p)++ = trenn;

    if( NULL != (w = w->next) )
    { q = sep;
      while( *q && *p - pp < n - 3 )
        *(*p)++ = *q++;
    }
  }
  LOG(33, "/wpar_sep, ret: %d, n: %ld, *p-pp: %ld.\n",
      *p - pp >= n - 3, (long)n, (long)(*p-pp));
  if( *p - pp >= n - 3 )
  { if( flags & 1 )
      return 2;
    else
      return 1;
  }
  return 0;
}


/***************************************************************************************/
/* int wipar(char **p, wert *w, char *quote, size_t n, char trenn, int quote_mode)     */
/*               char **p      : zum Aufbauen des Parameters                           */
/*               wert *w       : Zeiger auf Wert                                       */
/*               char *quote   : ggf. Quotingzeichen                                   */
/*               size_t n      : noch n Zeichen Platz in p                             */
/*               char trenn    : Zeichen, das die einzelnen Werte umschliesst          */
/*               int quote_mode: Quotingmodus                                          */
/*      wipar Werte ausgeben                                                           */
/***************************************************************************************/
int wipar(char **p, wert *w, char *quote, size_t n, char trenn, int quote_mode)
{ char *pp;
  char *q;

  LOG(3, "wipar, w: %x.\n", w);

  pp = *p;
  if( w != NULL && *p - pp < n )
  { if( skip_empty_flag && w->type == EMPTYW )          /* ggf. leere Param. ignorieren*/
    { LOG(7, "wipar, EMPTYW.\n");
      return 0;
    }
    LOG(7, "wipar, NO EMPTYW, wert: %s.\n", wert2string(*w));
    if( trenn )
      *(*p)++ = trenn;
    if( quote_mode == QF_HTML )
    { if( code_html(p, wert2string(*w), n-(*p-pp)-2) )
        return 1;
    }
    else
#ifdef MAYDBCLIENT
    if( !quote && quote_mode == QF_MYSQL )
    { if( w_mysql_cpyn_z(p, w, n-(*p-pp)-2) )
        return 1;
    }
    else
#endif
    { if( quote )
        q = quote;
      else if( quote_mode != QF_NONE )
      { switch( quote_mode )
        { case QF_SHELL: q = "\\'";
            break;
          default:       q = "";
        }
      }
      else
        q = globalquote;
      if( *q && *(q+1) )
      { if( strqcpyn_z(p, wert2string(*w), n-(*p-pp)-2, q) )
          return 1;
      }
      else
      { if( strcpyn_z(p, wert2string(*w), n-(*p-pp)-2) )
          return 1;
      }
    }
    if( trenn )
      *(*p)++ = trenn;
  }
  return *p - pp >= n;
}


/***************************************************************************************/
/* int getpar_sep(char **p, char *parname, size_t n, char trenn, int quote_mode,       */
/*                 char *sep, short level, unsigned int flags)                         */
/*             char **p       : zum Aufbauen des Parameters                            */
/*             char *parname  : Parametername                                          */
/*             size_t n       : noch n Zeichen Platz in p                              */
/*             char trenn     : Zeichen, das die einzelnen Werte umschliesst           */
/*             int quote_mode : Quotingmodus                                           */
/*             char *sep      : Trennung zwischen den einzelenen Werten                */
/*             short level    : -2: SystemP., -1: Browser/CMD, 0: global, x: level <= x*/
/*             unsigned int maxlen: > 0: Maximal maxlen Zeichen ausgeben               */
/*             unsigned int flags: 1: Bei Pufferüberlauf return 2 statt 1              */
/*                                 2: links trimmen  (noch nicht implementiert)        */
/*                                 4: rechts trimmen (noch nicht implementiert)        */
/*    return: 1, wenn n ueberschritten wird, bzw. 2, wenn (flags&1)                    */
/*      getpar_sep Bestimmt alle Werte zum Parameter parname                           */
/***************************************************************************************/
int getpar_sep(char **p, char *parname, size_t n, char trenn, int quote_mode,
                char *sep, short level, unsigned int flags)
{ hparstype *hp;

  LOG(1, "getpar_sep, parname: %s, trenn: %c, quote_mode: %d, level: %hd.\n",
      parname, trenn, quote_mode, level);

  if( (hp = gethp(parname, level)) )
    return wpar_sep(p, hp->werte, hp->quote, n, trenn, quote_mode, sep, flags);
  else
  { if( trenn && !skip_empty_flag )
    { if( n >= 2 )                             /* ggf. '' bei nicht def. Parametern    */
      { *(*p)++ = trenn;
        *(*p)++ = trenn;
      }
      else
        return 1;
    }
  }
  return 0;
}


/***************************************************************************************/
/* int getipar(char **p, char *parname, size_t n, char trenn, int quote_mode)          */
/*             char **p       : zum Aufbauen des Parameters                            */
/*             char *parname  : Parametername                                          */
/*             size_t n       : noch n Zeichen Platz in p                              */
/*             char trenn     : Zeichen, das die einzelnen Werte umschliesst           */
/*             int quote_mode : Quotingmodus                                           */
/*    return: 1, wenn zu wenig n ueberschritten wird                                   */
/*      getipar_sep Bestimmt Wert zum INIT-Parameter parname                           */
/***************************************************************************************/
int getipar(char **p, char *parname, size_t n, char trenn, int quote_mode)
{ hiparstype *hip;

  LOG(1, "getipar, parname: %s, quote_mode: %d.\n", parname, quote_mode);

  if( (hip = gethip(parname)) )
    return wipar(p, &(hip->wert), NULL, n, trenn, quote_mode);
  else
  { if( trenn && !skip_empty_flag )            /* ggf. '' bei nicht def. Parametern    */
    { if( n >= 2 )
      { *(*p)++ = trenn;
        *(*p)++ = trenn;
      }
      else
        return 1;
    }
  }
  return 0;
}


/***************************************************************************************/
/* void countpar(char **p, char *parname, size_t n, short level)                       */
/*             char **p       : zum Aufbauen des Parameters                            */
/*             char *parname  : Parametername                                          */
/*             size_t n       : noch n Zeichen Platz in p                              */
/*             short level    : 0: Browser/CMD Parameter, 1: globale, x: level < x     */
/*      countpar Bestimmt Anzahl Werte zum Parameter parname                           */
/***************************************************************************************/
void countpar(char **p, char *parname, size_t n, short level)
{ int z;
  hparstype *hp;

  LOG(1, "countpar, parname: %s.\n", parname);

  z = 0;
  if( (hp = gethp(parname, include_counter)) )
  { wertetype *w;

    LOG(4, "countpar, parname: %s, hp->name: %s.\n", parname, hp->name);

    w = hp->werte;
    while( w != NULL )
    { z++;
      w = w->next;
    }
  }
  snprintf(*p, n, "%d", z);
  *p += strlen(*p);
}


/***************************************************************************************/
/* int getpar(char **p, char **s, size_t n, char trenn, int quote_mode)                */
/*             char **p       : zum Aufbauen des Parameters                            */
/*             char **s       : *s zeigt auf den gelesenen String                      */
/*             size_t n       : noch n Zeichen Platz in p                              */
/*             char trenn     : Zeichen, das die einzelnen Werte umschliesst           */
/*             int quote_mode : Modus fürs Quoting                                     */
/*    return: 0, wenn in **p genug Platz, sonst != 0                                   */
/*      getpar Bestimmt den Wert zum Parameter, auf den *s zeigt                       */
/***************************************************************************************/
int getpar(char **p, char **s, size_t n, char trenn, int quote_mode)
{ char parname[PARLEN];
  char *pp;

  LOG(1, "getpar, *s: %s.\n", *s);

  (*s)++;                                   /* PAR_CHAR ueberlesen                     */

  if( **s == '{' )
  { (*s)++;
    scan_to(parname, s, PARLEN, 0, "}", '\0');
    if( **s == '}' )
      (*s)++;
  }
  else if( **s == PAR_CHAR )
  { pp = parname;
    getpar(&pp, s, PARLEN, '\0', QF_NONE);
    *pp = '\0';
  }
  else
  { pp = parname;
    if( **s == ATTRIB_CHAR )
      *pp++ = *(*s)++;
    while( pp-parname < PARLEN && (isalnum(**s) || **s == '_') )
      *pp++ = *(*s)++;
    *pp = '\0';
  }

  LOG(3, "getpar, parname: %s.\n", parname);

  if( quote_mode == QF_NOPARWERT )             /* Parwerte werden nicht bestimmt       */
  { return strcpyn_z(p, parname, n-(*p-pp)-2); /* Parametername zurück                 */
  }

  if( parname[0] == ATTRIB_CHAR )
    return getipar(p, parname+1, n, trenn, quote_mode);
  else
    return getpar_sep(p, parname, n, trenn, quote_mode, " ", include_counter, 0);
}


/***************************************************************************************/
/* char *parwert(char *p, short level)                                                 */
/*               char *p: Parameter, der gesucht weden soll                            */
/*               short level : 0: Browser/CMD Parameter, 1: globale, x: level < x      */
/*       parwert liefert ersten Wert zum Parameter p oder NULL, wenn nicht gefunden    */
/***************************************************************************************/
char *parwert(char *p, short level)
{ hparstype *hp;

  LOG(1, "parwert, p: %s.\n", p);

  if( (hp = gethp(p, level)) )
  { wertetype *w;

    if( (w = hp->werte) )
      return wert2string(w->wert);           /* Wert zurueck                         */
    else
      return NULL;
  }
  return NULL;
}


/***************************************************************************************/
/* wertetype *firstwert(char *p, short level)                                          */
/*                      char *p: Parameter, der gesucht weden soll                     */
/*                      short level: -2, -1, 0, >0 -> sys, Browser, global, lokal      */
/*            firstwert liefert ersten Wert zum Parameternamen p                       */
/***************************************************************************************/
wertetype *firstwert(char *p, short level)
{ hparstype *hp;

  if( (hp = gethp(p, level)) )
    return hp->werte;                     /* Wert zurück                             */
  else
    return NULL;
}


/***************************************************************************************/
/* int set_par_quote(char *name, char *quote)                                          */
/*                 char *name : Name des Parameters                                    */
/*                 char *quote: Quoting das gesetzt werden soll                        */
/*     set_par_quote setzt für Parameter name die Quotingzeichen                       */
/***************************************************************************************/
int set_par_quote(char *name, char *quote)
{ hparstype *hp;

  LOG(1, "set_par_quote, name: %s, quote: %s.\n", name, quote);

  if( (hp = gethp(name, include_counter)) )
  { if( *quote )
      hp->quote = store_str(quote);
    else
      hp->quote = NULL;
    return false;
  }
  return true;
}



/***************************************************************************************/
/* int u2w_queryvars(int pa, char **out, long n,                                       */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*     u2w_queryvars belegte Variablen aus P1 zu <var1>=$>var1>&<varn>=$<varn> wandeln */
/***************************************************************************************/
int u2w_queryvars(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *o, *p, *q, *z, sep;
  char first;
  int aflag;
  hparstype *hp;
  wertetype *w;

  if( pa & P2 )
    sep = prg_pars[1][0];
  else
    sep = ' ';

  if( pa & P3 )
    aflag = atoi(prg_pars[2]);
  else
    aflag = 0;

  if( pa & P4 )
    first = prg_pars[3][0];
  else
    first = '\0';

  p = prg_pars[0];
  o = *out;

  while( *p )
  { z = zeile;
    while( *p && *p != sep )
      *z++ = *p++;
    *z = '\0';
    LOG(20, "u2w_queryvars, zeile: %s, aflag: %d.\n", zeile, aflag);
    hp = gethp(zeile, include_counter);
    LOG(22, "u2w_queryvars, hp: %ld, w: %ld.\n", (long)hp, hp?(long)hp->werte:0);
    if( hp && (w = hp->werte) && w->wert.type != EMPTYW )
    { while( w != NULL )
      { q = wert2string(w->wert);
        if( *q )
        { z = zeile;                               /* in Zeile steht der Parametername */
          if( *out != o && n - 5 > *out - o )
          { *(*out)++ = '&';
            if( quote == QF_HTML )
            { *(*out)++ = 'a';
              *(*out)++ = 'm';
              *(*out)++ = 'p';
              *(*out)++ = ';';
            }
          }
          else if( first )
            *(*out)++ = first;
          while( *z && n > *out - o )
            *(*out)++ = *z++;
          if( n > *out - o )
            *(*out)++ = '=';
          if( quote == QF_HTML )
            code_html(out, q, n - (*out -o));
          else
          { while( *q && n > *out - o )
              *(*out)++ = *q++;
          }
        }
        w = w->next;
      }
    }
    else if( aflag )
    { z = zeile;                                   /* in Zeile steht der Parametername */
      if( *out != o && n - 5 > *out - o )
      { *(*out)++ = '&';
        if( quote == QF_HTML )
        { *(*out)++ = 'a';
          *(*out)++ = 'm';
          *(*out)++ = 'p';
          *(*out)++ = ';';
        }
      }
      else if( first )
        *(*out)++ = first;
      while( *z && n > *out - o )
        *(*out)++ = *z++;
      if( n > *out - o )
        *(*out)++ = '=';
    }
    while( *p && *p == sep )
      p++;
  }
  return false;
}



/***************************************************************************************/
/* int u2w_bglobal(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bglobal Browser/CMD-Line Parameter zu globalen kopieren optional trimmen    */
/***************************************************************************************/
int u2w_bglobal(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *p, *z, sep;
  int trimflag;
  hparstype *hp;
  wertetype *w;

  LOG(11, "u2w_bglobal, p0: %s.\n", prg_pars[0]);

  if( pa & P2 )
    sep = prg_pars[1][0];
  else
    sep = ' ';

  if( pa & P3 )
    trimflag = atoi(prg_pars[2]);
  else
    trimflag = 0;

  p = prg_pars[0];

  while( *p )
  { z = zeile;
    while( *p && *p != sep )
      *z++ = *p++;
    *z = '\0';
    hp = gethp(zeile, HP_BROWSER_LEVEL);
    if( hp && (w = hp->werte) && w->wert.type != EMPTYW )
    { if( trimflag )
      { while( w != NULL )
        {  if( w->wert.type == STRINGW && w->wert.s[0] )
          { char *tp, *tq;

            tq = w->wert.s + w->wert.len - 1;
            while( isblank(*tq) )
              tq--;
            *(tq+1) = '\0';
            w->wert.len = tq - w->wert.s + 1;
            tq = w->wert.s;
            while( isblank(*tq) )
              tq++;
            if( tq > w->wert.s )
            { w->wert.len = w->wert.len - (tq - w->wert.s);
              tp = w->wert.s;
              while( *tq )
                *tp++ = *tq++;
              *tp = '\0';
            }
          }
          w = w->next;
        }
      }
      set_parwerte(zeile, hp->werte, HP_GLOBAL_LEVEL);
    }
    else
    { unset_parwert(zeile, HP_GLOBAL_LEVEL);
    }
    while( *p && *p == sep )
      p++;
  }
  return false;
}


/***************************************************************************************/
/* int u2w_bpardef(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bpardef Browser/CMD-Line Parameter default-Wert festlegen und optional      */
/*                 zu globalen kopieren und trimmen                                    */
/***************************************************************************************/
int u2w_bpardef(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int trimflag;
  hparstype *hp;
  wertetype *w;

  LOG(11, "u2w_bpardef, p0: %s, p1: %s.\n", prg_pars[0], prg_pars[1]);

  hp = gethp(prg_pars[0], HP_BROWSER_LEVEL);
  if( !hp || !(w = hp->werte) || w->wert.type == EMPTYW )
  { if( NULL == (hp = store_parwert(prg_pars[0], prg_pars[1], HP_BROWSER_LEVEL)) )
      return true;
  }

  if( pa & P3 && atoi(prg_pars[2]) )
  { if( pa & P4 )
      trimflag = atoi(prg_pars[3]);
    else
      trimflag = 0;

    if( hp && (w = hp->werte) && w->wert.type != EMPTYW )
    { if( trimflag )
      { while( w != NULL )
        {  if( w->wert.type == STRINGW && w->wert.s[0] )
          { char *tp, *tq;

            tq = w->wert.s + w->wert.len - 1;
            while( isblank(*tq) )
              tq--;
            *(tq+1) = '\0';
            w->wert.len = tq - w->wert.s + 1;
            tq = w->wert.s;
            while( isblank(*tq) )
              tq++;
            if( tq > w->wert.s )
            { w->wert.len = w->wert.len - (tq - w->wert.s);
              tp = w->wert.s;
              while( *tq )
                *tp++ = *tq++;
              *tp = '\0';
            }
          }
          w = w->next;
        }
      }
      set_parwerte(prg_pars[0], hp->werte, HP_GLOBAL_LEVEL);
    }
    else
    { unset_parwert(prg_pars[0], HP_GLOBAL_LEVEL);
    }
  }
  return false;
}


#ifdef WEBSERVER
/***************************************************************************************/
/* int u2w_hiddenvars(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*                  int pa: Bitflag mit gesetzten Parameterfeldern                     */
/*                  char prg_pars: Parameter                                           */
/*     u2w_hiddenvars %hiddenvars: hidden Parameter nach Liste                         */
/***************************************************************************************/
int u2w_hiddenvars(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], char token)
{ char *p, *z, sep;
  int aflag;
  hparstype *hp;
  wertetype *w;

  if( pa & P2 )
    sep = prg_pars[1][0];
  else
    sep = ' ';

  if( pa & P3 )
    aflag = atoi(prg_pars[2]);
  else
    aflag = 0;

  p = prg_pars[0];

  while( *p )
  { z = zeile;
    while( *p && *p != sep )
      *z++ = *p++;
    *z = '\0';
    hp = gethp(zeile, include_counter);
    if( hp && (w = hp->werte) && w->wert.type != EMPTYW )
    { if( tablelevel )
      { while( w != NULL )
        { if( newhiddenvar(zeile, wert2string(w->wert)) )
            return true;
          w = w->next;
        }
      }
      else
      { while( w != NULL )
        { if( dosendf("<input name=\"%s\" type=\"hidden\" value=\"%s\">",
                      zeile, wert2string(w->wert)) )
            return true;
          w = w->next;
        }
      }
    }
    else if( aflag )
    { if( tablelevel )
      { if( newhiddenvar(zeile, "") )
          return true;
      }
      else if( dosendf("<input name=\"%s\" type=\"hidden\" value=\"\">", zeile) )
        return true;
    }
    while( *p && *p == sep )
      p++;
  }
  return false;
}
#endif
