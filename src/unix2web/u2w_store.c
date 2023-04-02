/*******************************************************/
/* File: u2w_store.c                                   */
/* Funktionen zur Speicherung von Werten und Variablen */
/* timestamp: 2014-07-16 22:14:28                      */
/*******************************************************/


#include "u2w.h"


#ifdef WITH_MALLOC

char *strbuff = NULL;
char *sb = NULL + STRINGBUFFERSIZE;

wertetype *werte = NULL;
wertetype *next_wert = NULL;
int rest_werte = 0;

hparstype *hpars = NULL;
hparstype *next_hpars = NULL;
int rest_hpars = 0;

char *stribuff = NULL;
char *sib = NULL + STRINGINITBUFFERSIZE;

hiparstype *hipars = NULL;
hiparstype *next_hipars = NULL;
int rest_hipars = 0;

char *hiddenvars = NULL;
char *hv = NULL;
int shv = 0;

#else

char strbuff[STRINGBUFFERSIZE];
char *sb = strbuff;

wertetype werte[MAX_ANZ_WERTE];      /* Werte - Parameter, Variablen oder Berechnungen */
int anz_werte = 0;                   /* Anzal in werte                                 */

hparstype hpars[MAX_ANZ_HPARS];      /* Parameter Namen, Hashtabelle zeigt darauf      */
int anz_hpars = 0;

char stribuff[STRINGINITBUFFERSIZE];
char *sib = stribuff;

hiparstype hipars[MAX_ANZ_HIPARS];   /* Parameter Namen, Hashtabelle zeigt darauf      */
int anz_hipars = 0;

char hiddenvars[MAX_SIZE_HIDDENVARS];
char *hv = hiddenvars;

#endif


/***************************************************************************************/
/* unsigned int var_hash(char *s)                                                      */
/*                 char *s: String                                                     */
/*                 return : Hashwert zum String s                                      */
/*              u2w_hash berechnet Hashwert                                            */
/***************************************************************************************/
unsigned int var_hash(char *s)
{ unsigned short c;
  unsigned int h;

  h = HASH_INIT;
  while( (c = *s++) )
    h = h<<HASH_SHIFT ^ c;
  return h % HASH_TABLE_SIZE;
}


/***************************************************************************************/
/* void init_global_buffer(void)                                                       */
/*      init_global_buffer Speicherbereiche des Sting- und Wertepuffers freigeben      */
/***************************************************************************************/
void init_global_buffer(void)
{ int i;
#ifdef WITH_MALLOC
  char *oldstrbuff;
  wertetype *oldwerte;
  hparstype *oldhpars;

  LOG(7, "init_gloabl_buffer.\n");
#ifdef DEBUG
  clientgetpath = NULL;
#endif

  while( strbuff )
  { oldstrbuff = *((char **)strbuff);
    LOG(5, "init_global_buffer, strbuff: %lx, oldstrbuff: %lx.\n", strbuff, oldstrbuff);
    free(strbuff);
    strbuff = oldstrbuff;
  }
  sb = strbuff + STRINGBUFFERSIZE;

  while( werte )
  { oldwerte = werte->next;
    LOG(5, "init_global_buffer, werte: %lx, oldwerte: %lx.\n", werte, oldwerte);
    free(werte);
    werte = oldwerte;
  }
  rest_werte = 0;

  while( hpars )
  { oldhpars = hpars->next;
    free(hpars);
    hpars = oldhpars;
  }
  rest_hpars = 0;

  LOG(9, "/init_gloabl_buffer.\n");

#else

  sb = strbuff;
  anz_werte = 0;                     /* Anzal in werte                                 */
  anz_hpars = 0;
#endif

  for(i = 0; i < HASH_TABLE_SIZE; phash[i++] = NULL);
}


/***************************************************************************************/
/* void clean_local_vars(int level)                                                    */
/*      clean_local_vars lokale Variablen des Levels level löschen                     */
/***************************************************************************************/
void clean_local_vars(int level)
{ int i;
  hparstype **php;

  for( i = 0; i < HASH_TABLE_SIZE; i++ )
  { php = &phash[i];
    while( *php )
    { if( (*php)->level == level )
        *php = (*php)->next;
      else
        php = &((*php)->next);
    }
  }
}


/***************************************************************************************/
/* char *store_str(char *s)                                                            */
/*                 char *s: String, der im Stringbuffer gespeichert werden soll        */
/*                 return : Speicherort des Strings, Pointer auf "", bei kein Platz    */
/*       store_str speichert den String s im Stringbuffer                              */
/***************************************************************************************/
char *store_str(char *s)
{ char *r;

#ifdef WITH_MALLOC
  if( strlen(s)+1 > STRINGBUFFERSIZE - (sb - strbuff) )
  { char **newstrbuff;

    if( NULL != (newstrbuff = malloc(STRINGBUFFERSIZE)) )
    { *newstrbuff = strbuff;                   /* Zeiger auf bisherigen Puffer merken  */
      strbuff = (char *)newstrbuff;
      sb = (char *)(newstrbuff+1);
      LOG(5, "store_str, newstrbuff: %lx, strbuff: %lx, sb: %lx.\n", newstrbuff,
          strbuff, sb);
      if( strlen(s)+1 > STRINGBUFFERSIZE - (sb - strbuff) )
        return "";
    }
    else
      return "";
  }
#else
  if( strlen(s)+1 > STRINGBUFFERSIZE - (sb - strbuff) )
    return "";
#endif

  r = sb;
  while( (*sb++ = *s++) );
  return r;
}


/***************************************************************************************/
/* char *store_istr(char *s)                                                           */
/*                 char *s: String, der im Init-Stringbuffer gespeichert werden soll   */
/*                 return : Speicherort des Strings, Pointer auf "", bei kein Platz    */
/*       store_istr speichert den String s im Init-Stringbuffer                        */
/***************************************************************************************/
char *store_istr(char *s)
{ char *r;

#ifdef WITH_MALLOC
  if( strlen(s)+1 > STRINGINITBUFFERSIZE - (sib - stribuff) )
  { if( strlen(s) > STRINGINITBUFFERSIZE-1 )
    { char *sbuff;
      if( NULL != (sbuff = malloc(strlen(s)+1)) )
      { strcpy(sbuff, s);
        return s;
      }
      else
        return "";
    }
    else
    { char *newstribuff;
    
      if( NULL != (newstribuff = malloc(STRINGINITBUFFERSIZE)) )
      { stribuff = newstribuff;
        sib = newstribuff;
      }
      else
        return "";
    }
  }
#else
  if( strlen(s)+1 > STRINGINITBUFFERSIZE - (sib - stribuff) )
    return "";
#endif

  r = sib;
  while( (*sib++ = *s++) );
  return r;
}


#ifdef WEBSERVER
/***************************************************************************************/
/* char *store_bin(char *s, long *nb)                                                  */
/*                 char *s: Daten, die im Stringbuffer gespeichert werden sollen       */
/*                 long *nb: Länge der Binärdaten                                      */
/*                 return : Speicherort des Strings, Pointer auf "", bei kein Platz    */
/*       store_bin speichert Daten im Stringbuffer                                     */
/*                 *nb == 0, wenn nichts gespeichert werden konnte                     */
/***************************************************************************************/
char *store_bin(char *s, long *nb)
{ char *r;

#ifdef WITH_MALLOC
  if( *nb+1 > STRINGBUFFERSIZE - (sb - strbuff) )
  { char **newstrbuff;

    if( NULL != (newstrbuff = malloc(STRINGBUFFERSIZE)) )
    { *newstrbuff = strbuff;
      strbuff = (char *)newstrbuff;
      sb = (char *)(newstrbuff+1);
      if( *nb+1 > STRINGBUFFERSIZE - (sb - strbuff) )
      { *nb = 0;
        return "";
      }
    }
    else
    { *nb = 0;
      return "";
    }
  }
#else
  if( *nb > STRINGBUFFERSIZE - (sb - strbuff) )
  { *nb = 0;
    return "";
  }
#endif

  r = sb;
  memcpy(sb, s, *nb);
  sb += *nb;
  *sb++ = '\0';
  return r;
}
#endif  /* #ifdef WEBSERVER */


#ifdef WITH_CGI
/***************************************************************************************/
/* char *store_query_str(char *s)                                                      */
/*                       char *s: String, der im Stringbuffer nach QUERY_STRING=       */
/*                                gespeichert werden soll                              */
/*                       return : Speicherort des Strings                              */
/*       store_str speichert den String s im Stringbuffer                              */
/***************************************************************************************/
char *store_query_str(char *s)
{ static char query_str[MAX_ZEILENLAENGE];

  strcpy(query_str, "QUERY_STRING=");
  strcatn(query_str, s, MAX_ZEILENLAENGE);
  return query_str;
}
#endif


/***************************************************************************************/
/* wertetype *alloc_wert(void)                                                         */
/*                 return : Speicherort für Wert                                       */
/*            alloc_wert: Speicher für Wert anfordern bzw zuweisen                     */
/***************************************************************************************/
wertetype *alloc_wert(void)
{
  LOG(5, "alloc_wert\n");
#ifdef WITH_MALLOC

  if( rest_werte > 0 )
  { rest_werte--;
    return next_wert++;
  }
  else
  { wertetype *newwerte;

    if( NULL != (newwerte = calloc(ANZ_WERTE_ALLOC, sizeof(wertetype))) )
    { newwerte->next = werte;                  /* Zeiger auf bisherigen Puffer merken  */
      LOG(7, "alloc_wert, werte: %lx, newwerte: %lx.\n", werte, newwerte);
      werte = newwerte;
      rest_werte = ANZ_WERTE_ALLOC - 2;
      next_wert = werte;
      next_wert++;
      return next_wert++;
    }
    else
      return NULL;
  }
#else
  if( anz_werte < MAX_ANZ_WERTE )
    return &werte[anz_werte++];
  else
    return NULL;
#endif
}


/***************************************************************************************/
/* wertetype *store_wert(char *in, wtype wt)                                           */
/*                       char *in : zu speichernder Wert                               */
/*                       wtpye wt: Type des Wertes                                     */
/*            store_wert Speichert in im Werte-Array                                   */
/***************************************************************************************/
wertetype *store_wert(char *in, wtype wt)
{ wertetype *w;
  wtype nwt;

  if( NULL != (w = alloc_wert()) )
  { if( in )
      nwt = test_wtype(in);
    else
      nwt = NONEW;
    switch( wt == NONEW || nwt == EMPTYW ? nwt : wt )
    { case STRINGW: w->wert.type = STRINGW;
                    w->wert.s = store_str(in);
                    w->wert.len = strlen(in);
                    break;
      case DOUBLEW: w->wert.type = DOUBLEW;
                    w->wert.w.d = getdouble(&in);
                    w->wert.len = 0;
                    break;
      case LONGW:   w->wert.type = LONGW;
                    w->wert.w.l = getlong(&in);
                    w->wert.len = 0;
                    break;
      default:      w->wert.type = nwt;
                    w->wert.len = 0;
                    break;
    }
    w->next = NULL;
    return w;
  }
  else
    return NULL;
}


/***************************************************************************************/
/* wertetype *store_stringwert(char *in)                                               */
/*                       char *in : zu speichernder String-Wert, ohne Allokiereung     */
/*                       wtpye wt: Type des Wertes                                     */
/*            store_stringwert Speichert in im Werte-Array ohne Speicher zu allokieren */
/***************************************************************************************/
wertetype *store_stringwert(char *in)
{ wertetype *w;

  if( NULL != (w = alloc_wert()) )
  { w->wert.type = STRINGW;
    w->wert.s = in;
    w->wert.len = strlen(in);
    w->next = NULL;
    return w;
  }
  else
    return NULL;
}


#ifdef WEBSERVER
/***************************************************************************************/
/* wertetype *store_bin_wert(char *in, long nb)                                        */
/*                           char *in : zu speichernder Wert                           */
/*            store_bin_wert Speichert Binärewrt im Werte-Array                        */
/***************************************************************************************/
wertetype *store_bin_wert(char *in, long nb)
{ wertetype *w;

  LOG(1, "store_bin_wert, nb: %ld\n", nb);

  if( NULL != (w = alloc_wert()) )
  { if( nb > 0 )
    { w->wert.type = BINARYW;
      w->wert.s = store_bin(in, &nb);
      w->wert.binlen = w->wert.len = nb;
    }
    else
      w->wert.type = EMPTYW;
    w->next = NULL;
    return w;
  }
  else
    return NULL;
}
#endif  /* #ifdef WEBSERVER */


/***************************************************************************************/
/* hparstype *alloc_hpars(void)                                                        */
/*                 return : Speicherort für Parameter                                  */
/*            alloc_hpars: Speicher für Parameter(Variable) anfordern bzw zuweisen     */
/***************************************************************************************/
hparstype *alloc_hpars(void)
{
  LOG(5, "alloc_hpars\n");
#ifdef WITH_MALLOC

  if( rest_hpars > 0 )
  { rest_hpars--;
    return next_hpars++;
  }
  else
  { hparstype *newhpars;

    if( NULL != (newhpars = calloc(ANZ_HPARS_ALLOC, sizeof(hparstype))) )
    { newhpars->next = hpars;                  /* Zeiger auf bisherigen Puffer merken  */
      LOG(7, "alloc_hpars, hpars: %lx, newhpars: %lx.\n", hpars, newhpars);
      hpars = newhpars;
      rest_hpars = ANZ_HPARS_ALLOC - 2;
      next_hpars = hpars;
      next_hpars++;
      return next_hpars++;
    }
    else
      return NULL;
  }
#else
  if( anz_hpars < MAX_ANZ_HPARS )
    return &hpars[anz_hpars++];
  else
    return NULL;
#endif
}


/***************************************************************************************/
/* hiparstype *alloc_hipars(void)                                                      */
/*                 return : Speicherort für INIT-Parameter                             */
/*            alloc_hipars: Speicher für INIT-Parameter anfordern bzw zuweisen         */
/***************************************************************************************/
hiparstype *alloc_hipars(void)
{
  LOG(5, "alloc_hipars\n");
#ifdef WITH_MALLOC

  if( rest_hipars > 0 )
  { rest_hipars--;
    return next_hipars++;
  }
  else
  { hiparstype *newhipars;

    if( NULL != (newhipars = calloc(ANZ_HINITPARS_ALLOC, sizeof(hiparstype))) )
    { newhipars->next = hipars;                /* Zeiger auf bisherigen Puffer merken  */
      LOG(7, "alloc_hipars, hipars: %lx, newhipars: %lx.\n", hipars, newhipars);
      hipars = newhipars;
      rest_hipars = ANZ_HINITPARS_ALLOC - 2;
      next_hipars = hipars;
      next_hipars++;
      return next_hipars++;
    }
    else
      return NULL;
  }
#else
  if( anz_hipars < MAX_ANZ_HINITPARS )
    return &hipars[anz_hipars++];
  else
    return NULL;
#endif
}


/***************************************************************************************/
/* int store_parstring(char *name, char *wert, short level)                            */
/*                  char *name     : Name des Parameters                               */
/*                  char *wert     : String-Pointer, der gespeichert werden soll       */
/*                  short level    : Scope level                                       */
/*     store_parstring Speichert Stringpointer in hpars[]                              */
/*                  Nur für die Speicherung von Strings, die nicht allokiert werden    */
/*                  müssen, weil Speicher nicht fluechtig                              */
/***************************************************************************************/
int store_parstring(char *name, char *wert, short level)
{ hparstype *hp;
  wertetype *w;
  unsigned int h;

  LOG(1, "store_parstring, name: %s, wert: %.200s.\n", name, wert);

  h = var_hash(name);
  hp = phash[h];
  while( hp )                               /* in Parameterliste suchen                */
  { if( hp->level == level && 0 == strcmp(name, hp->name) )   /* gefunden?             */
    { w = hp->werte;
      while( w->next )
        w = w->next;
      w->next = store_stringwert(wert);     /* Wert an Kommando anhaengen              */
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    hp->quote = NULL;
    hp->level = level;
    hp->werte = store_stringwert(wert);
    hp->next = phash[h];
    phash[h] = hp;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* hparstype *store_parwert(char *name, char *wert, short level)                       */
/*                  char *name     : Name des Parameters                               */
/*                  char *wert     : Wert, der gespeichert werden soll                 */
/*                  short level    : Scope level                                       */
/*     store_parwert Speichert die Parameter mit ihren Werten in hpars[]               */
/*                  Nur für die Speicherung von Übergebenen Parametern, denn alles     */
/*                  wird als Strings gespeichert.                                      */
/*      return: *hp bei Erfolg, sonst NULL                                             */
/***************************************************************************************/
hparstype *store_parwert(char *name, char *wert, short level)
{ hparstype *hp;
  wertetype *w;
  unsigned int h;

  LOG(1, "store_parwert, name: %s, wert: %.200s.\n", name, wert);

  h = var_hash(name);
  hp = phash[h];
  while( hp )                               /* in Parameterliste suchen                */
  { if( hp->level == level && 0 == strcmp(name, hp->name) )   /* gefunden?             */
    { w = hp->werte;
      while( w->next )
        w = w->next;
      w->next = store_wert(wert, STRINGW);  /* Wert an Kommando anhaengen              */
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    hp->quote = NULL;
    hp->level = level;
    hp->werte = store_wert(wert, STRINGW);
    hp->next = phash[h];
    phash[h] = hp;
    return hp;
  }
  else
    return NULL;
}


/***************************************************************************************/
/* int store_inipar(char *name, char *wert, int strflag)                               */
/*                  char *name     : Name des Parameters                               */
/*                  char *wert     : Wert, der gespeichert werden soll                 */
/*                  int strflag    : force to string                                   */
/*     store_inipar Speichert die Parameter mit ihren Werten in ipars[]                */
/*                  Nur für die Speicherung von INIT-Variablen.                        */
/***************************************************************************************/
int store_inipar(char *name, char *wert, int strflag)
{ hiparstype *hip;
  unsigned int h;
  wtype nwt;

  LOG(1, "store_inipar, name: %s, wert: %.200s.\n", name, wert);

  h = var_hash(name);
  hip = inihash[h];

  if( strflag )
    nwt = STRINGW;
  else
    nwt = test_wtype(wert);
  while( hip )                                  /* in Parameterliste suchen            */
  { if( 0 == strcmp(name, hip->name) )          /* gefunden?                           */
    { switch( nwt )
      { case STRINGW: hip->wert.type = STRINGW;
                      hip->wert.s = store_istr(wert);
                      hip->wert.len = strlen(wert);
                      break;
        case DOUBLEW: hip->wert.type = DOUBLEW;
                      hip->wert.w.d = getdouble(&wert);
                      hip->wert.len = 0;
                      break;
        case LONGW:   hip->wert.type = LONGW;
                      hip->wert.w.l = getlong(&wert);
                      hip->wert.len = 0;
                      break;
        default:      hip->wert.type = EMPTYW;
                      hip->wert.len = 0;
                      break;
      }
      return false;
    }
    else
      hip = hip->next;
  }
  if( NULL != (hip = alloc_hipars()) )
  { strcpyn(hip->name, name, MAX_PAR_NAME_LEN);
    switch( nwt )
    { case STRINGW: hip->wert.type = STRINGW;
                    hip->wert.s = store_istr(wert);
                    hip->wert.len = strlen(wert);
                    break;
      case DOUBLEW: hip->wert.type = DOUBLEW;
                    hip->wert.w.d = getdouble(&wert);
                    hip->wert.len = 0;
                    break;
      case LONGW:   hip->wert.type = LONGW;
                    hip->wert.w.l = getlong(&wert);
                    hip->wert.len = 0;
                    break;
      default:      hip->wert.type = EMPTYW;
                    hip->wert.len = 0;
                    break;
    }
    hip->next = inihash[h];
    inihash[h] = hip;
    return false;
  }
  else
    return true;
}


#ifdef WEBSERVER
/***************************************************************************************/
/* int store_parwert_bin(char *name, char *wert, long nb)                              */
/*                  char *name     : Name des Parameters                               */
/*                  char *wert     : Wert, der gespeichert werden soll                 */
/*                  long nb        : Anzahl Bytes                                      */
/*     store_parwert_bin Speichert die Parameter mit ihren Werten in hpars[]           */
/*                  Für die Speicherung von Binärwerten                                */
/*                  Da nur vom Browser ist level immer HP_BROWSER_LEVEL                */
/***************************************************************************************/
int store_parwert_bin(char *name, char *wert, long nb)
{ hparstype *hp;
  wertetype *w;
  unsigned int h;

  LOG(1, "store_parwert_bin, name: %s.\n", name);

  h = var_hash(name);
  hp = phash[h];
  while( hp )
  { if( hp->level == HP_BROWSER_LEVEL && 0 == strcmp(name, hp->name) )  /* gefunden?   */
    { w = hp->werte;
      while( w->next )
        w = w->next;
      w->next = store_bin_wert(wert, nb);   /* Wert an Kommando anhaengen              */
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    hp->quote = NULL;
    hp->level = HP_BROWSER_LEVEL;
    hp->werte = store_bin_wert(wert, nb);
    hp->next = phash[h];
    phash[h] = hp;
    LOG(2, "/store_parwert_bin, OK.\n");
    return false;
  }
  else
  { LOG(2, "/store_parwert_bin, ERROR.\n");
    return true;
  }
}


/***************************************************************************************/
/* int store_cookies(char *cookies)                                                    */
/*                  char *cookies: Cookies vom Browser erhalten                        */
/*     store_cookies Speichert die Cookie Variablen                                    */
/***************************************************************************************/
int store_cookies(char *cookies)
{ char *p;                                               /* zeigt auf cookies          */
  char name[MAX_PAR_NAME_LEN];                           /* Name eines Parameters      */
  char wert[PARLEN];                                     /* Wert eines Parameters      */
  char *w;                                               /* zeigt auf Werte            */

  LOG(1, "store_cookies, cookies: %s.\n", cookies);

  p = cookies;
  while( *p )
  { while( *p == ' ' )                                   /* Leerzeichen ignorieren     */
      p++;
    strcpyn_str(name, &p, "=", MAX_PAR_NAME_LEN);        /* bis zum '=' zum Namen      */
    if( *p == '=' )                                      /* '=' gefunden?              */
    { p++;                                               /* '=' ueberspringen          */
      while( *p == ' ' )                                 /* Leerzeichen ignorieren     */
        p++;
      w = wert;                                          /* Wert dahin                 */
      while( *p && *p != ';' && w-wert < PARLEN )        /* bis zum ';' | '\0' ist Wert*/
        *w++ = *p++;
      *w = '\0';
      conv_get(name);
      conv_get(wert);
      store_parwert(name, wert, HP_COOKIE_LEVEL);        /* Parameter speichern        */
    }
    while( *p == ';' )                                   /* ';' ueberspringen          */
      p++;
  }
  LOG(2, "/store_cookies.\n");

  return true;
}
#endif  /* #ifdef WEBSERVER */


/***************************************************************************************/
/* int change_wert(wert *cwert, char *wert, wtype wt)                                  */
/*                 wert *cwert: Wert, der geändert werden soll                         */
/*                 char *wert : Wert, der gespeichert werden soll                      */
/*                 wtype wt   : Type des Wertes, der gespeichert werden soll           */
/*     change_wert Ersetzt Parameter cwert mit wert                                    */
/***************************************************************************************/
int change_wert(wert *cwert, char *wert, wtype wt)
{ wtype nwt;

  LOG(1, "change_wert, wert: %.200s.\n", wert);

  nwt = test_wtype(wert);

  switch( wt == NONEW || nwt == EMPTYW ? nwt : wt )
  { case STRINGW: LOG(6, "change_wert - STRINGW.\n");
                  if( cwert->len < strlen(wert) )
                  { cwert->s = store_str(wert);     /* Wert ersetzen  */
                    cwert->len = strlen(wert);
                  }
                  else
                  { LOG(7, "change_wert, alter Wert: %s.\n", cwert->s);
                    strcpy(cwert->s, wert);
                  }
                  cwert->type = STRINGW;
                  break;
    case DOUBLEW: LOG(6, "change_wert - DOUBLEW.\n");
                  cwert->type = DOUBLEW;
                  cwert->w.d = getdouble(&wert);
                  break;
    case LONGW:   LOG(6, "change_wert - LONGW.\n");
                  cwert->type = LONGW;
                  cwert->w.l = getlong(&wert);
                  break;
    default:      LOG(6, "change_wert - default.\n");
                  cwert->type = EMPTYW;
                  break;
  }
  return false;
}


/***************************************************************************************/
/* int change_parwert(char *name, char *wert, wtype wt, short level)                   */
/*                    char *name     : Name des Parameters                             */
/*                    char *wert     : Wert, der gespeichert werden soll               */
/*                    wtype wt       : Type des Wertes, der gespeichert werden soll    */
/*     change_parwert Ersetzt Parameter name mit wert in hpars[] oder trägt ein        */
/***************************************************************************************/
int change_parwert(char *name, char *wert, wtype wt, short level)
{ wtype nwt;
  hparstype *hp;
  unsigned int h;

  LOG(1, "change_parwert, name: %s, level: %hd, wert: %.200s.\n", name, level,
      wert ? wert : "(NULL)");

  if( wert )
    nwt = test_wtype(wert);
  else
    nwt = wt;
  h = var_hash(name);
  hp = phash[h];
  while( hp )                               /* in Parameterliste suchen                */
  { if( hp->level == level && 0 == strcmp(name, hp->name) )  /* gefunden?              */
    { LOG(5, "change_parwert - gefunden, h: %d, len: %d.\n", h, hp->werte->wert.len);
      hp->werte->next = NULL;               /* alle weiteren Werte löschen             */
      switch( wt == NONEW || nwt == EMPTYW ? nwt : wt )
      { case STRINGW: LOG(6, "change_parwert - STRINGW.\n");
                      if( hp->werte->wert.len < strlen(wert) )
                      { hp->werte->wert.s = store_str(wert);     /* Wert ersetzen  */
                        hp->werte->wert.len = strlen(wert);
                      }
                      else
                      { LOG(7, "change_parwert, alter Wert: %s.\n",
                            hp->werte->wert.s);
                        strcpy(hp->werte->wert.s, wert);
                      }
                      hp->werte->wert.type = STRINGW;
                      break;
        case DOUBLEW: LOG(6, "change_parwert - DOUBLEW.\n");
                      hp->werte->wert.type = DOUBLEW;
                      hp->werte->wert.w.d = getdouble(&wert);
                      break;
        case LONGW:   LOG(6, "change_parwert - LONGW.\n");
                      hp->werte->wert.type = LONGW;
                      hp->werte->wert.w.l = getlong(&wert);
                      break;
        default:      LOG(6, "change_parwert - default.\n");
                      hp->werte->wert.type = nwt;
                      break;
      }
      LOG(2, "/change_parwert - gefunden.\n");
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    hp->quote = NULL;
    hp->level = level;
    hp->werte = store_wert(wert, wt);
    hp->next = phash[h];
    phash[h] = hp;
    LOG(2, "/change_parwert - nicht gefunden.\n");
    return false;
  }
  else
  { LOG(2, "/change_parwert - Kein Platz.\n");
    return true;
  }
}


/***************************************************************************************/
/* int add_parwert(char *name, char *wert, wtype wt, short level)                      */
/*                    char *name     : Name des Parameters                             */
/*                    char *wert     : Wert, der gespeichert werden soll               */
/*                    wtype wt : Type des wertes                                       */
/*     add_parwert fügt an Parameter name wert an                                      */
/***************************************************************************************/
int add_parwert(char *name, char *wert, wtype wt, short level)
{ wertetype *w;
  hparstype *hp;
  unsigned int h;

  LOG(1, "add_parwert, name: %s, wert: %.200s.\n", name, wert);

  if( wt != STRINGW && test_wtype(wert) == EMPTYW )
    return false;

  h = var_hash(name);
  hp = phash[h];
  while( hp )                               /* in Parameterliste suchen                */
  { if( hp->level == level && 0 == strcmp(name, hp->name) )  /* gefunden?              */
    { if( hp->werte->wert.type == EMPTYW )
        return change_wert(&(hp->werte->wert), wert, wt);
      w = hp->werte;
      while( w->next )
        w = w->next;
      w->next = store_wert(wert, wt);       /* Wert an Parameter anhaengen             */
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    hp->quote = NULL;
    hp->level = level;
    hp->werte = store_wert(wert, wt);
    hp->next = phash[h];
    phash[h] = hp;
    LOG(2, "/add_parwert - nicht gefunden.\n");
    return false;
  }
  else
  { LOG(2, "/add_parwert - Kein Platz.\n");
    return true;
  }
}


/***************************************************************************************/
/* int set_parwert(char *name, wert wert, short level)                                 */
/*                 char *name : Name des Parameters                                    */
/*                 wert wert  : Wert, der gespeichert werden soll                      */
/*                 short level: Skope Level                                            */
/*     set_parwert Ersetzt Parameter name mit wert in pars[] oder trägt ein            */
/***************************************************************************************/
int set_parwert(char *name, wert wert, short level)
{ wertetype *w;
  hparstype *hp;
  unsigned int h;

  LOG(1, "set_parwert, name: %s, level: %hd.\n", name, level);

  h = var_hash(name);
  hp = phash[h];
  while( hp )                                               /* in Parameterliste suchen*/
  { if( hp->level == level && 0 == strcmp(name, hp->name) ) /* gefunden?               */
    { hp->werte->next = NULL;                               /* weitere Werte löschen   */
      hp->werte->wert = wert;
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) && NULL != (w = alloc_wert()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    w->wert = wert;
    w->next = NULL;
    hp->quote = NULL;
    hp->level = level;
    hp->werte = w;
    hp->next = phash[h];
    phash[h] = hp;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int set_parwerte(char *name, wertetype *werte, short level)                         */
/*                 char *name      : Name des Parameters                               */
/*                 wertetype *werte: Werte, der gespeichert werden soll                */
/*                 short level     : Skope Level                                       */
/*     set_parwerte Ersetzt Parameter name durch Kopie der werte                       */
/***************************************************************************************/
int set_parwerte(char *name, wertetype *werte, short level)
{ hparstype *hp;
  unsigned int h;

  LOG(1, "set_parwerte, name: %s, level: %hd.\n", name, level);

  h = var_hash(name);
  hp = phash[h];
  while( hp )                                               /* in Parameterliste suchen*/
  { if( hp->level == level && 0 == strcmp(name, hp->name) ) /* gefunden?               */
    { hp->werte = werte;                                    /* weitere Werte löschen   */
      return false;
    }
    else
      hp = hp->next;
  }
  if( NULL != (hp = alloc_hpars()) )
  { strcpyn(hp->name, name, MAX_PAR_NAME_LEN);
    hp->quote = NULL;
    hp->level = level;
    hp->werte = werte;
    hp->next = phash[h];
    phash[h] = hp;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int unset_parwert(char *name, short level)                                          */
/*                 char *name     : Name des Parameters                                */
/*                 short level    : Skope Level                                        */
/*     unset_parwert Ersetzt Parameter name mit EMPTYW                                 */
/***************************************************************************************/
int unset_parwert(char *name, short level)
{ hparstype *hp;
  unsigned int h;

  LOG(1, "unset_parwert, name: %s, level: %hd.\n", name, level);

  h = var_hash(name);
  hp = phash[h];
  while( hp )                                               /* in Parameterliste suchen*/
  { if( hp->level == level && 0 == strcmp(name, hp->name) ) /* gefunden?               */
    { hp->werte->next = NULL;                               /* weitere Werte löschen   */
      hp->werte->wert.type = EMPTYW;
      return false;
    }
    else
      hp = hp->next;
  }
  return true;
}


/***************************************************************************************/
/* int trim_parwert(char *name, char *cs, short level, short lrflag)                   */
/*                 char *name     : Name des Parameters                                */
/*                 char *cs       : zu trimmende Zeichen                               */
/*                 short level    : Skope Level                                        */
/*                 short lrflag   : 1: Anfang, 2: Ende, 3: Anfang und Ende             */
/*     trim_parwert Zeichen aus cs vom wert in pars[] entfernen                        */
/***************************************************************************************/
int trim_parwert(char *name, char *cs, short level, short lrflag)
{ wertetype *w;
  hparstype *hp;
  unsigned int h;
  char *q, *p;

  LOG(1, "trim_parwert, name: %s, level: %hd.\n", name, level);

  if( !*cs )
    return false;

  h = var_hash(name);
  hp = phash[h];
  while( hp )                                               /* in Parameterliste suchen*/
  { if( hp->level == level && 0 == strcmp(name, hp->name) ) /* gefunden?               */
    { w = hp->werte;
      while( w )
      { if( w->wert.type == STRINGW && w->wert.s[0] )
        { if( lrflag & 2 )
          { q = w->wert.s + w->wert.len - 1;
            while( strchr(cs, *q) )
              q--;
            *(q+1) = '\0';
            w->wert.len = q - w->wert.s + 1;
          }
          if( lrflag & 1 )
          { q = w->wert.s;
            while( strchr(cs, *q) )
              q++;
            if( q > w->wert.s )
            { w->wert.len = w->wert.len - (q - w->wert.s);
              p = w->wert.s;
              while( *q )
                *p++ = *q++;
              *p = '\0';
            }
          }
        }
        w = w->next;
      }
      return false;
    }
    else
      hp = hp->next;
  }
  return false;
}


/***************************************************************************************/
/* int trim_parwert_blanks(char *name, short level, short lrflag)                      */
/*                 char *name     : Name des Parameters                                */
/*                 short level    : Skope Level                                        */
/*                 short lrflag   : 1: Anfang, 2: Ende, 3: Anfang und Ende             */
/*     trim_parwert_blanks Blanks vom wert in pars[] entfernen                         */
/***************************************************************************************/
int trim_parwert_blanks(char *name, short level, short lrflag)
{ wertetype *w;
  hparstype *hp;
  unsigned int h;
  char *q, *p;

  LOG(1, "trim_parwert_blanks, name: %s, level: %hd.\n", name, level);

  h = var_hash(name);
  hp = phash[h];
  while( hp )                                               /* in Parameterliste suchen*/
  { if( hp->level == level && 0 == strcmp(name, hp->name) ) /* gefunden?               */
    { w = hp->werte;
      while( w )
      { if( w->wert.type == STRINGW && w->wert.s[0] )
        { if( lrflag & 2 )
          { q = w->wert.s + w->wert.len - 1;
            while( isblank(*q) )
              q--;
            *(q+1) = '\0';
            w->wert.len = q - w->wert.s + 1;
          }
          if( lrflag & 1 )
          { q = w->wert.s;
            while( isblank(*q) )
              q++;
            if( q > w->wert.s )
            { w->wert.len = w->wert.len - (q - w->wert.s);
              p = w->wert.s;
              while( *q )
                *p++ = *q++;
              *p = '\0';
            }
          }
        }
        w = w->next;
      }
      return false;
    }
    else
      hp = hp->next;
  }
  return false;
}


/***************************************************************************************/
/* int newhiddenvar(char *name, char *value)                                           */
/*                  char *name: Name des Hidden Vars                                   */
/*                  char *value: Wert des Hidden Vars                                  */
/*     return: 1: Fehler, 0 sonst                                                      */
/*     newhiddenvar speichert Hidden Vars innerhalb von Tabellen                       */
/***************************************************************************************/
int newhiddenvar(char *name, char *value)
{
  LOG(1, "newhiddenvar, name: %s, value: %s, hv: %ld.\n", name, value, (long)hv);

#ifdef WITH_MALLOC
  char *newhv;

  if( (hv - hiddenvars) + strlen(name) + strlen(value) + 2 > shv )
  { if( NULL == (newhv = realloc(hiddenvars, shv + ALLOC_SIZE_HIDDENVARS)) )
      return true;
    hv = newhv + (hv - hiddenvars);
    hiddenvars = newhv;
    shv += ALLOC_SIZE_HIDDENVARS;
  }
#else
  if( (hv - hiddenvars) + strlen(name) + strlen(value) + 2 > MAX_SIZE_HIDDENVARS )
    return true;
#endif

  while( *name )
    *hv++ = *name++;
  *hv++ = '\0';
  while( *value )
    *hv++ = *value++;
  *hv++ = '\0';
  LOG(2, "/newhiddenvar.\n");
  return false;
}


/***************************************************************************************/
/* int sendhiddenvars(void)                                                            */
/*     return: 1: Fehler, 0 sonst                                                      */
/*     sendhiddenvars gespeicherte Hidden Vars nach einer Tabelle senden               */
/***************************************************************************************/
int sendhiddenvars(void)
{ char *n, *v;

  LOG(1, "sendhiddenvar.\n");

  n = hiddenvars;
  while( n < hv )
  { v = n+1;
    while( *v )
      v++;
    v++;
    if( dosendf("<input name=\"%s\" type=\"hidden\" value=\"%s\">",
                 n, v) )
    {
#ifdef WITH_MALLOC
      free(hiddenvars);
      hv = hiddenvars = NULL;
      shv = 0;
#else
      hv = hiddenvars;
#endif
      return true;
    }
    n = v+1;
    while( *n )
      n++;
    n++;
  }
#ifdef WITH_MALLOC
  free(hiddenvars);
  hv = hiddenvars = NULL;
  shv = 0;
#else
  hv = hiddenvars;
#endif
  LOG(2, "/sendhiddenvar.\n");
  return false;
}
