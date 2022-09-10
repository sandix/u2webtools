/**************************************/
/* File: u2w_scan_progs.c             */
/* Funktionen aus Array scan_progs    */
/* timestamp: 2017-02-12 18:26:32     */
/**************************************/


#include "u2w.h"

#ifndef COMPILER

#ifdef WEBSERVER
/***************************************************************************************/
/* short do_image(int pa, char **out, long n,                                          */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_image fügt image ein                                                         */
/***************************************************************************************/
short do_image(int pa, char **out, long n,
             char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *o;

  LOG(1, "do_image, par[0]: %s, out: %lx.\n", prg_pars[0], out);

  if( pa & P3 && !strcmp(prg_pars[2], "0") )
     return false;

  if( out )
  { o = *out;
    strcpyn_z(out, IMAGE_START, n-(*out-o));
    code_link(zeile, prg_pars[0], MAX_ZEILENLAENGE, false);
    strcpyn_z(out, zeile, n-(*out-o));
    if( pa & P2 )
    { strcpyn_z(out, "\" align=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[1], n-(*out-o));
    }
    if( pa & P3 )
    { strcpyn_z(out, "\" width=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[2], n-(*out-o));
    }
    if( pa & P4 )
    { strcpyn_z(out, "\" height=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[3], n-(*out-o));
    }
    if( pa & P5 )
    { strcpyn_z(out,  "\" alt=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[4], n-(*out-o));
    }
    else
      strcpyn_z(out, "\" alt=\"Image", n-(*out-o));
    strcpyn_z(out, IMAGE_END, n-(*out-o));
    return false;
  }
  else
  { o = zeile;
    strcpyn_z(&o, IMAGE_START, MAX_ZEILENLAENGE-(o-zeile));
    code_link(o, prg_pars[0], MAX_ZEILENLAENGE-(o-zeile), false);
    o += strlen(o);
    if( pa & P2 )
    { strcpyn_z(&o, "\" align=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[1], MAX_ZEILENLAENGE-(o-zeile));
    }
    if( pa & P3 )
    { strcpyn_z(&o, "\" width=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[2], MAX_ZEILENLAENGE-(o-zeile));
    }
    if( pa & P4 )
    { strcpyn_z(&o, "\" height=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[3], MAX_ZEILENLAENGE-(o-zeile));
    }
    if( pa & P5 )
    { strcpyn_z(&o,  "\" alt=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[4], MAX_ZEILENLAENGE-(o-zeile));
    }
    else
      strcpyn_z(&o, "\" alt=\"Image", MAX_ZEILENLAENGE-(o-zeile));
    strcpyn_z(&o, IMAGE_END, MAX_ZEILENLAENGE-(o-zeile));
    return dobinsend(zeile, o-zeile);
  }
}


/***************************************************************************************/
/* short make_object(int pa, char **out, long n,                                       */
/*                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int eflag)       */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     make_object fügt <object ...> und ggf. </object> bei eflag == true ein          */
/***************************************************************************************/
short make_object(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int eflag)
{ char *o;

  if( (pa & P2 && !strcmp(prg_pars[1], "0"))
      || (pa & P3 && !strcmp(prg_pars[2], "0")) )
     return false;

  if( out )
  { o = *out;
    return false;
    strcpyn_z(out, "<object data=\"", n-(*out-o));
    strcpyn_z(out, prg_pars[0], n-(*out-o));
    if( pa & P2 )
    { strcpyn_z(out, "\" width=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[1], n-(*out-o));
    }
    if( pa & P3 )
    { strcpyn_z(out, "\" heigth=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[2], n-(*out-o));
    }
    if( pa & P4 )
    { strcpyn_z(out, "\" type=\"", n-(*out-o));
      strcpyn_z(out, prg_pars[3], n-(*out-o));
    }
    if( eflag )
      strcpyn_z(out, "\"></object>", n-(*out-o));
    else
      strcpyn_z(out, "\">", n-(*out-o));
  }
  else
  { o = zeile;
    strcpyn_z(&o, "<object data=\"", MAX_ZEILENLAENGE-(o-zeile));
    strcpyn_z(&o, prg_pars[0], MAX_ZEILENLAENGE-(o-zeile));
    if( pa & P2 )
    { strcpyn_z(&o, "\" width=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[1], MAX_ZEILENLAENGE-(o-zeile));
    }
    if( pa & P3 )
    { strcpyn_z(&o, "\" heigth=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[2], MAX_ZEILENLAENGE-(o-zeile));
    }
    if( pa & P4 )
    { strcpyn_z(&o, "\" type=\"", MAX_ZEILENLAENGE-(o-zeile));
      strcpyn_z(&o, prg_pars[3], MAX_ZEILENLAENGE-(o-zeile));
    }
    if( eflag )
      strcpyn_z(&o, "\"></object>", MAX_ZEILENLAENGE-(o-zeile));
    else
      strcpyn_z(&o, "\">", MAX_ZEILENLAENGE-(o-zeile));
    return dobinsend(zeile, o-zeile);
  }
}


/***************************************************************************************/
/* short do_object(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_object fügt object mit <object ...></object> ein                             */
/***************************************************************************************/
short do_object(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ return make_object(pa, out, n, prg_pars, true);
}


/***************************************************************************************/
/* short do_sobject(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_sobject fügt object nur mit <object> ein                                     */
/***************************************************************************************/
short do_sobject(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ return make_object(pa, out, n, prg_pars, false);
}


/***************************************************************************************/
/* short do_eobject(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_eobject fügt </object> ein                                                   */
/***************************************************************************************/
short do_eobject(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ return dosend("</object>");
}


/***************************************************************************************/
/* short do_referer(int pa, char **out, long n,                                        */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_referer fügt Referer ein                                                    */
/***************************************************************************************/
short do_referer(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p;

  if( (p = parwert("referer", HP_HTTP_HEADER_LEVEL)) )
    strqcpyn_z(out, p, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_http_accept_language(int pa, char **out, long n,                           */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_http_accept_language fügt http_accept_language ein                          */
/***************************************************************************************/
short do_http_accept_language(int pa, char **out, long n,
                            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( http_accept_language && *http_accept_language )
    strqcpyn_z(out, http_accept_language, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short test_lang(char *langs, char *lang)                                            */
/*               char *langs: Sprachen codes in denen gesucht wird, durch ' ' oder ',' */
/*                            getrennt                                                 */
/*               char *lang: Sprache, die gesucht wird                                 */
/*     test_lang sucht lang in langs, ignoriert case und '-' == '_'                    */
/***************************************************************************************/
short test_lang(char *langs, char *lang)
{ char *s, *l;

  LOG(1, "test_lang, langs: %s, lang: %s.\n", langs, lang);

  s = langs;
  l = lang;
  while( *s )
  { LOG(11, "test_lang - while, s: %s, l: %s.\n", s, l);
    if( tolower(*s) == tolower(*l)
        || ((*s == '-' || *s == '_') && (*l == '-' || *l == '_')) )
    { s++;
      l++;
    }
    else
    { if( *s == ' ' || *s == ',' )
      { if( !*l )
          return true;
      }
      else
      { while( *s && *s != ' ' && *s != ',' )
          s++;
      }
      while( *s && (*s == ' ' || *s == ',') )
        s++;
      l = lang;
    }
    LOG(17, "test_lang - /while, s: %s, l: %s.\n", s, l);
  }
  LOG(3, "/test_lang, l: %s.\n", l);
  return !*l;
}


/***************************************************************************************/
/* short do_preflang(int pa, char **out, long n,                                       */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_preflang fügt bevorzugte Sprache des Browsers ein                           */
/***************************************************************************************/
short do_preflang(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *z;
  int f;

  LOG(1, "do_preflang, pa: %s.\n", pa & P1 ? prg_pars[0] : "");

  if( http_accept_language && *http_accept_language )
  { LOG(11, "do_preflang, http_accept_language: %s.\n", http_accept_language);
    if( pa & P1 )
    { z = http_accept_language;
      for(;;)
      { strcpyn_str(zeile, &z, ",; ", 128);
        if( (test_lang(prg_pars[0], zeile)) )
          break;
        while( *z && *z != ',' )
          z++;
        while( *z == ',' || *z == ' ' )
          z++;
        if( !*z )
          return true;
      }
    }
    else
      strcpyn_str_s(zeile, http_accept_language, ",; ", MAX_ZEILENLAENGE);

    LOG(11, "do_preflang, lang: %s.\n", zeile);

    if( *zeile )
    { f = 0;
      z = zeile;
      while( *z )
      { if( f )
          *z = toupper(*z);
        else if( *z == '-' )
        { *z = '_';
          f = 1;
        }
        z++;
      }
      strqcpyn_z(out, zeile, n, qf_strings[quote]);
      return false;
    }
  }
  return true;
}


/***************************************************************************************/
/* short do_useragent(int pa, char **out, long n,                                      */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_useragent fügt User-Agent                                                   */
/***************************************************************************************/
short do_useragent(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p;

  if( (p = parwert("user-agent", HP_HTTP_HEADER_LEVEL)) )
    strqcpyn_z(out, p, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_uatype(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_uatype fügt Kennung für Type des User-Agents ein                            */
/*              B(rowser), T(ablet), M(obile), C(onsole)                               */
/***************************************************************************************/
short do_uatype(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p;

  if( n > 0 && (p = parwert("user-agent", HP_HTTP_HEADER_LEVEL)) && *p )
  { if( strcasestr(p, "mobile") || strcasestr(p, "phone")
        || strcasestr(p, "ipod") || strcasestr(p, "blackberry")
        || strcasestr(p, "opera mobi") )
      *(*out)++ = 'M';
    else if( strcasestr(p, "android") || strcasestr(p, "ipad")
        || strcasestr(p, "playbook") || strcasestr(p, "kindle") )
      *(*out)++ = 'T';
    else
      *(*out)++ = 'B';
  }
  return false;
}


/***************************************************************************************/
/* short do_var_contenttype(int pa, char **out, long n,                                */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_var_contenttype %content_type                                                */
/***************************************************************************************/
short do_var_contenttype(int pa, char **out, long n,
                       char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( content_path )
    strqcpyn_z(out, content_type, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_var_content(int pa, char **out, long n,                                    */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_var_content %content                                                         */
/***************************************************************************************/
short do_var_content(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( content_path )
    strqcpyn_z(out, content_path, n, qf_strings[quote]);
  return false;
}


#endif  /* #ifdef WEBSERVER */


/***************************************************************************************/
/* short do_getenv(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_getenv fügt Wert einer Environmentvariablen ein                             */
/***************************************************************************************/
short do_getenv(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *s;

  if( (s = getenv(prg_pars[0])) )
    strqcpyn_z(out, s, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_getpid(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_getpid fügt Prozess PID ein                                                 */
/***************************************************************************************/
short do_getpid(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  *out += snprintf(*out, n, "%lu", (unsigned long)getpid());
  return false;
}


/***************************************************************************************/
/* short do_getppid(int pa, char **out, long n,                                        */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_getppid fügt Parent PID ein                                                 */
/***************************************************************************************/
short do_getppid(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  *out += snprintf(*out, n, "%lu", (unsigned long)getppid());
  return false;
}


/***************************************************************************************/
/* short do_myport(int pa, char **out, long n,                                         */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_myport %myport Ausgabe des Ports des u2w-Servers                             */
/***************************************************************************************/
short do_myport(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strqcpyn_z(out, myport, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_https(int pa, char **out, long n,                                          */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_https %https Ausgabe "https" oder "http"                                     */
/***************************************************************************************/
short do_https(int pa, char **out, long n,
             char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
#ifdef WITH_HTTPS
  if( ssl_mode )
    strqcpyn_z(out, "https", n, qf_strings[quote]);
  else
#endif
    strqcpyn_z(out, "http", n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_printuser(int pa, char **out, long n,                                      */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_printuser %user                                                              */
/***************************************************************************************/
short do_printuser(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strqcpyn_z(out, user, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_printpwd(int pa, char **out, long n,                                       */
/*                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)       */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_printpwd %pwd                                                                */
/***************************************************************************************/
short do_printpwd(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strqcpyn_z(out, passwd, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_clientip(int pa, char **out, long n,                                       */
/*                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)       */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_clientip %clientip                                                           */
/***************************************************************************************/
short do_clientip(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strcpyn_z(out, clientip, n);
#endif
  return false;
}


/***************************************************************************************/
/* short do_clientip6(int pa, char **out, long n,                                      */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_clientip6 %clientip6                                                         */
/***************************************************************************************/
short do_clientip6(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strcpyn_z(out, clientip6, n);
#endif
  return false;
}


/***************************************************************************************/
/* short do_var_ok(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_var_ok %ok                                                                   */
/***************************************************************************************/
short do_var_ok(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p;

 if( (p=parwert(OKBUTTONNAME, HP_BROWSER_LEVEL)) )
    strqcpyn_z(out, p, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_thisfile(int pa, char **out, long n,                                       */
/*                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)       */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_thisfile %this                                                               */
/***************************************************************************************/
short do_thisfile(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ strqcpyn_z(out, clientgetfile, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_myhost(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_myhost %thishost                                                             */
/***************************************************************************************/
short do_myhost(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strqcpyn_z(out, http_host ? http_host : hostname, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_mypage(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_mypage %thispage                                                             */
/***************************************************************************************/
short do_mypage(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ LOG(59, "do_mypage, clientgetpath: %s.\n", clientgetpath ? clientgetpath : "(NULL)");
  strqcpyn_z(out, clientgetpath, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_method(int pa, char **out, long n,                                         */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_method %method HTTP-Methode zurueck                                          */
/***************************************************************************************/
short do_method(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  if( methodstr && *methodstr )
    strqcpyn_z(out, methodstr, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_putdata(int pa, char **out, long n,                                        */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_putdata Pfad zu den Put-Daten                                                */
/***************************************************************************************/
short do_putdata(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  if( putdata && *putdata )
    strqcpyn_z(out, putdata, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_subname(int pa, char **out, long n,                                        */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_subname == $SUBMENUPAR                                                       */
/***************************************************************************************/
short do_subname(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p;

  if( (p=parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )
    strqcpyn_z(out, p, n, qf_strings[quote]);
  return false;
}


/***************************************************************************************/
/* short do_allvars(int pa, char **out, long n,                                        */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_allvars fügt alle globalen Variablennamen ein (nicht vom Browser)           */
/***************************************************************************************/
short do_allvars(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *o;
  short firstflag = 0;
  unsigned int h;
  hparstype *hp;

  o = *out;

  for( h = 0; h < HASH_TABLE_SIZE; h++ )
  { hp = phash[h];
    while( hp )
    { if( hp->level == HP_GLOBAL_LEVEL )
      { if( firstflag )
          *(*out)++ = ' ';
        else
          firstflag++;
        strqcpyn_z(out, hp->name, n - (*out-o), qf_strings[quote]);
      }
      hp = hp->next;
    }
  }
  LOG(1,  "/do_allvars, o: %.*s.\n", *out-o, o);
  return false;
}


/***************************************************************************************/
/* short do_allpars(int pa, char **out, long n,                                        */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_allpars fügt alle Variablennamen vom Browser erhalten ein                   */
/***************************************************************************************/
short do_allpars(int pa, char **out, long n,               
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *o;
  short firstflag = 0;
  unsigned int h;
  hparstype *hp;

  o = *out;

  if( pa & P1 && *prg_pars[0] )
  { for( h = 0; h < HASH_TABLE_SIZE; h++ )
    { hp = phash[h];
      while( hp )
      { if( hp->level == HP_BROWSER_LEVEL && !str_starts(hp->name, SYSVARSTART)
            && !is_elem(prg_pars[0], hp->name) )
        { if( firstflag )
            *(*out)++ = ' ';
          else
            firstflag++;
          strqcpyn_z(out, hp->name, n - (*out-o), qf_strings[quote]);
        }
        hp = hp->next;
      }
    }
  }
  else
  { for( h = 0; h < HASH_TABLE_SIZE; h++ )
    { hp = phash[h];
      while( hp )
      { if( hp->level == HP_BROWSER_LEVEL && !str_starts(hp->name, SYSVARSTART) )
        { if( firstflag )
            *(*out)++ = ' ';
          else
            firstflag++;
          strqcpyn_z(out, hp->name, n - (*out-o), qf_strings[quote]);
        }
        hp = hp->next;
      }
    }
  }
  LOG(1,  "/do_allpars, o: %.*s.\n", *out-o, o);
  return false;
}


/***************************************************************************************/
/* short do_par(int pa, char **out, long n,                                            */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_par fügt Variable ein, ggf. mit Trennzeichen                                */
/***************************************************************************************/
short do_par(int pa, char **out, long n,
           char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *sep;

  if( pa & P2 )
  { if( tablecols && prg_pars[1][0] == TABLE_SEP && !prg_pars[1][1] )
    { if( tablecols < 0 )
        sep = TABLE_COLUMN_HEAD_TRENN;
      else
        sep = TABLE_COLUMN_TRENN;
    }
    else
      sep = prg_pars[1];
  }
  else
    sep = " ";

  getpar_sep(out, prg_pars[0], n, pa & P3 ? prg_pars[2][0] : '\0', quote, sep,
             include_counter, 0);

  return false;
}


/***************************************************************************************/
/* short do_gpar(int pa, char **out, long n,                                           */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_gpar fügt globale Variable ein, ggf. mit Trennzeichen                       */
/***************************************************************************************/
short do_gpar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *sep;

  if( pa & P2 )
  { if( tablecols && prg_pars[1][0] == TABLE_SEP && !prg_pars[1][1] )
    { if( tablecols < 0 )
        sep = TABLE_COLUMN_HEAD_TRENN;
      else
        sep = TABLE_COLUMN_TRENN;
    }
    else
      sep = prg_pars[1];
  }
  else
    sep = " ";

  getpar_sep_level(out, prg_pars[0], n, pa & P3 ? prg_pars[2][0] : '\0', quote, sep,
                   HP_GLOBAL_LEVEL, 0);

  return false;
}


/***************************************************************************************/
/* short do_bpar(int pa, char **out, long n,                                           */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*               int pa: Anzahl Parameter in prg_pars                                  */
/*               char **out: Ziel des Ergebnisses                                      */
/*               long n    : Platz in out                                              */
/*               char prg_pars: übergebene Funktionsparameter                          */
/*      do_bpar fügt Parameter des Browsers oder CMD (u2w_interpreter) ein,            */
/*               ggf. mit Trennzeichen                                                 */
/***************************************************************************************/
short do_bpar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *sep;

  if( pa & P2 )
  { if( tablecols && prg_pars[1][0] == TABLE_SEP && !prg_pars[1][1] )
    { if( tablecols < 0 )
        sep = TABLE_COLUMN_HEAD_TRENN;
      else
        sep = TABLE_COLUMN_TRENN;
    }
    else
      sep = prg_pars[1];
  }
  else
    sep = " ";

  getpar_sep_level(out, prg_pars[0], n, pa & P3 ? prg_pars[2][0] : '\0', quote, sep,
                   HP_BROWSER_LEVEL, 0);

  return false;
}


/***************************************************************************************/
/* short do_bfpar(int pa, char **out, long n,                                          */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*               int pa: Anzahl Parameter in prg_pars                                  */
/*               char **out: Ziel des Ergebnisses                                      */
/*               long n    : Platz in out                                              */
/*               char prg_pars: übergebene Funktionsparameter                          */
/*      do_bfpar fügt Parameter des Browsers bei Multipart Dateien                     */
/*               ggf. mit Trennzeichen                                                 */
/***************************************************************************************/
short do_bfpar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *sep;

  if( pa & P2 )
  { if( tablecols && prg_pars[1][0] == TABLE_SEP && !prg_pars[1][1] )
    { if( tablecols < 0 )
        sep = TABLE_COLUMN_HEAD_TRENN;
      else
        sep = TABLE_COLUMN_TRENN;
    }
    else
      sep = prg_pars[1];
  }
  else
    sep = " ";

  getpar_sep_level(out, prg_pars[0], n, pa & P3 ? prg_pars[2][0] : '\0', quote, sep,
                   HP_BROWSER_SYS_LEVEL, 0);

  return false;
}


/***************************************************************************************/
/* short do_cpar(int pa, char **out, long n,                                           */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*               int pa: Anzahl Parameter in prg_pars                                  */
/*               char **out: Ziel des Ergebnisses                                      */
/*               long n    : Platz in out                                              */
/*               char prg_pars: übergebene Funktionsparameter                          */
/*      do_cpar fügt cookie Parameter ein, ggf. mit Trennzeichen                       */
/***************************************************************************************/
short do_cpar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *sep;

  if( pa & P2 )
  { if( tablecols && prg_pars[1][0] == TABLE_SEP && !prg_pars[1][1] )
    { if( tablecols < 0 )
        sep = TABLE_COLUMN_HEAD_TRENN;
      else
        sep = TABLE_COLUMN_TRENN;
    }
    else
      sep = prg_pars[1];
  }
  else
    sep = " ";

  getpar_sep_level(out, prg_pars[0], n, pa & P3 ? prg_pars[2][0] : '\0', quote, sep,
                   HP_COOKIE_LEVEL, 0);

  return false;
}


/***************************************************************************************/
/* short do_spar(int pa, char **out, long n,                                           */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_spar fügt system Variable ein, ggf. mit Trennzeichen                        */
/***************************************************************************************/
short do_spar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  getpar_sep_level(out, prg_pars[0], n, '\0', quote, "", HP_SYSTEM_LEVEL, 0);

  return false;
}


/***************************************************************************************/
/* short do_ipar(int pa, char **out, long n,                                           */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_ipar fügt init Variable ein, ggf. mit Trennzeichen                          */
/***************************************************************************************/
short do_ipar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  getipar(out, prg_pars[0], n, '\0', quote);

  return false;
}


/***************************************************************************************/
/* short do_hpar(int pa, char **out, long n,                                           */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_hpar fügt Elemente des HTTP-Headers ein                                     */
/***************************************************************************************/
short do_hpar(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  char *p;
  for( p = prg_pars[0]; *p; p++ )
    if( isupper(*p) )
      *p =  tolower((unsigned char)*p);
  getpar_sep_level(out, prg_pars[0], n, '\0', quote, "", HP_HTTP_HEADER_LEVEL, 0);

  return false;
}


/***************************************************************************************/
/* short do_countpar(int pa, char **out, long n,                                       */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_countpar fügt Anzahl der Werte einer Variablen ein                          */
/***************************************************************************************/
short do_countpar(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  countpar(out, prg_pars[0], n, include_counter);
  return false;
}


/***************************************************************************************/
/* short do_gcountpar(int pa, char **out, long n,                                      */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_gcountpar fügt Anzahl der Werte einer globalen Variable ein                 */
/***************************************************************************************/
short do_gcountpar(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  countpar(out, prg_pars[0], n, 1);
  return false;
}


/***************************************************************************************/
/* short do_bccountpar(int pa, char **out, long n,                                     */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_bccountpar fügt Anzahl der Werte einer Variablen ein Browser/CMD            */
/***************************************************************************************/
short do_bccountpar(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  countpar(out, prg_pars[0], n, 0);
  return false;
}


/***************************************************************************************/
/* short do_countchar(int pa, char **out, long n,                                      */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*      do_countchar Zahlt Zeichen in String                                           */
/***************************************************************************************/
short do_countchar(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int i = 0;
  char *p;

  LOG(3, "do_countchar, p0: %s, p1: %s.\n", prg_pars[0], prg_pars[1]);

  for( p = prg_pars[0]; *p; p++ )
    if( *p == prg_pars[1][0] )
      i++;
  *out += snprintf(*out, n, "%d", i);
  return false;
}


/***************************************************************************************/
/* short do_split(int pa, char **out, long n,                                          */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_split zerlegt String am Zeichen                                              */
/***************************************************************************************/
short do_split(int pa, char **out, long n,
             char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int v, b, i;
  char *s, *t, *o;

  o = *out;

  if( pa & P3 )
    v = atoi(prg_pars[2]);
  else
    v = 1;

  if( pa & P4 )
    b = atoi(prg_pars[3]);
  else
    b = MAX_ZEILENLAENGE;

  if( pa & P5 )
    t = prg_pars[4];
  else
    t = " ";

  LOG(3, "split, sc: %s, v: %d, b: %d.\n", prg_pars[1], v, b);

  i = prg_pars[1][0] ? 1 : 0;
  s = prg_pars[0];
  while( *s )
  { if( !prg_pars[1][0] )
      i++;
    if( *s == prg_pars[1][0] )
    { if( i++ >= b )
      { LOG(2, "/split");
        return false;
      }
      if( v < i && i <= b )
        strqcpyn_z(out, t, n-(*out-o), qf_strings[quote]);
      s++;
    }
    else if( v <= i && i <= b && n > *out - o)
      *(*out)++ = *s++;
    else
      s++;
  }

  LOG(2, "/split");
  return false;
}


/***************************************************************************************/
/* short do_list_split(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],  */
/*                   int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_list_split zerlegt String am Zeichen und Ergebnis in Liste                   */
/***************************************************************************************/
short do_list_split(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                  int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int v, b, i;
  char *s, *o;

  if( pa & P3 )
    v = atoi(prg_pars[2]);
  else
    v = 1;

  if( pa & P4 )
    b = atoi(prg_pars[3]);
  else
    b = MAX_ZEILENLAENGE;

  LOG(3, "do_list_split, sc: %s, v: %d, b: %d.\n", prg_pars[1], v, b);

  i = prg_pars[1][0] ? 1 : 0;
  s = prg_pars[0];
  o = list_pars[*listlen];
  while( *s )
  { if( !prg_pars[1][0] )
      i++;
    if( *s == prg_pars[1][0] )
    { if( v <= i && i <= b)
      { *o = '\0';
        o = list_pars[++(*listlen)];
      }
      if( i++ >= b )
      { LOG(2, "/split");
        return false;
      }
      s++;
    }
    else if( v <= i && i <= b && MAX_LEN_LIST_PARS > o - list_pars[*listlen] )
      *o++ = *s++;
    else
      s++;
  }
  if( o > list_pars[*listlen] )
  { (*listlen)++;
    *o = '\0';
  }
  LOG(2, "/split");
  return false;
}


/***************************************************************************************/
/* short do_substr(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_substr Substring                                                             */
/***************************************************************************************/
short do_substr(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int v, b;
  char *pv, *pb;

  v = atoi(prg_pars[1]);

  LOG(3, "substr, str: %s, v: %d, b: %s.\n", prg_pars[0], v, prg_pars[2]);

  if( v >= 0 )
    pv = prg_pars[0] + v;
  else
    pv = prg_pars[0] + (strlen(prg_pars[0])+v);

  if( pa & P3 )
  { b = atoi(prg_pars[2]);
    if( b >= 0 )
      pb = pv + b;
    else
      pb = prg_pars[0] + (strlen(prg_pars[0])+b);
  }
  else
    pb = prg_pars[0] + strlen(prg_pars[0]);

  if( *qf_strings[quote] )
  { char *o;

    o = *out;
    while( *pv && pv < pb && *out - o < n-1)
    { if( strchr(qf_strings[quote], *pv) )
        *(*out)++ = *qf_strings[quote];
      *(*out)++ = *pv++;
    }
  }
  else
  { if( pb - pv > n )
      pb = pv+n;
    while( *pv && pv < pb )
      *(*out)++ = *pv++;
  }

  LOG(2, "/substr\n");
  return false;
}


/***************************************************************************************/
/* short do_strpos(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_strpos String suchen                                                         */
/***************************************************************************************/
short do_strpos(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int v;
  char *p;

  if( pa & P3 )
    v = atoi(prg_pars[2]);
  else
    v = 0;

  LOG(3, "strpos, str: %s, suche: %s, v: %d.\n", prg_pars[0], prg_pars[1], v);

  if( v > strlen(prg_pars[0])
      || NULL == (p = strstr(prg_pars[0]+v, prg_pars[1])) )
  { if( n > 2 )
    { *(*out)++ = '-';
      *(*out)++ = '1';
    }
  }
  else
    *out += snprintf(*out, n, "%ld", (long)(p - prg_pars[0]));

  LOG(2, "/do_strpos\n");
  return false;
}


/***************************************************************************************/
/* short do_strcpos(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              int tablecols: Spalten einer Tabelle                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_strcpos Zeichen aus String suchen                                            */
/***************************************************************************************/
short do_strcpos(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ long v, m;

  if( pa & P3 )
    v = atol(prg_pars[2]);
  else
    v = 0;

  LOG(3, "strcpos, str: %s, suche: %s, v: %ld.\n", prg_pars[0], prg_pars[1], v);

  m = strlen(prg_pars[0]);
  while( v < m )
  { if( NULL != strchr(prg_pars[1], prg_pars[0][v]) )
      break;
    v++;
  }
  if( v >= m )
  { if( n > 2 )
    { *(*out)++ = '-';
      *(*out)++ = '1';
    }
  }
  else
    *out += snprintf(*out, n, "%ld", v);

  LOG(2, "/do_strcpos\n");
  return false;
}


/***************************************************************************************/
/* short do_getpar(int pa, char **out, long n,                                         */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_getpar Parameter lesen                                                       */
/***************************************************************************************/
short do_getpar(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *o, *p, *e;
  char c;

  if( pa & P2 )
    p = prg_pars[0] + atoi(prg_pars[1]);
  else
    p = prg_pars[0];

  LOG(3, "getpar, str: %s.\n", prg_pars[0]);

  for(e = prg_pars[0]; *e; e++);
  if( p >= e )
    return false;

  o = *out;
  while( *p == ' ' )
    p++;
  if( *p == '\'' || *p == '"' )
    c = *p++;
  else
    c = '\0';

  while( *p && *out - o < n )
  { if( c ? (*p == c) : (*p == ',' || *p == ')') )
      break;
    *(*out)++ = *p++;
  }

  LOG(2, "/do_getpar\n");
  return false;
}


/***************************************************************************************/
/* short do_list_getpar(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS], */
/*                   int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_list_getpar Parameter lesen                                                  */
/***************************************************************************************/
short do_list_getpar(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                   int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *o, *p, *e;
  char c;

  if( pa & P2 )
    p = prg_pars[0] + atoi(prg_pars[1]);
  else
    p = prg_pars[0];
  for(e = prg_pars[0]; *e; e++);

  LOG(3, "do_list_getpar, str: %s, start: %s.\n", prg_pars[0], p<e ? p : "");
  LOG(5, "do_list_getpar, *listlen: %d.\n", *listlen);

  if( p >= e )
    return false;

  o = list_pars[(*listlen)++];
  while( *p == ' ' )
    p++;
  if( *p == '\'' || *p == '"' )
    c = *p++;
  else
    c = '\0';

  while( *p && MAX_LEN_LIST_PARS > o - list_pars[*listlen-1] )
  { if( c ? (*p == c) : (*p == ',' || *p == ')') )
      break;
    *o++ = *p++;
  }
  *o++ = '\0';
  if( c )
  { while( *p && *p != ',' && *p != ')' )
      p++;
  }
  c = *p++;
  snprintf(list_pars[(*listlen)++], MAX_LEN_LIST_PARS, "%ld", (long)(p-prg_pars[0]));
  snprintf(list_pars[(*listlen)++], MAX_LEN_LIST_PARS, "%d", c == ',');

  LOG(2, "/do_list_getpar\n");
  return false;
}


/***************************************************************************************/
/* short do_list_getname(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],*/
/*                   int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*              int *listlen: Anzahl der Listenelemente                                */
/*              char list_pars: Ergebnisse                                             */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_list_getname Name z. B. Funktionsname einlesen (Alnums)                      */
/***************************************************************************************/
short do_list_getname(int *listlen, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],
                    int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ char *o, *p, *e;
  int kflag;

  if( pa & P2 )
    p = prg_pars[0] + atoi(prg_pars[1]);
  else
    p = prg_pars[0];
  for(e = prg_pars[0]; *e; e++);

  LOG(1, "do_list_getname, str: %s, start: %s.\n", prg_pars[0], p<e ? p : "");
  LOG(3, "do_list_getname, *listlen: %d.\n", *listlen);

  if( p >= e )
    return false;

  if( (kflag = (*p == '{')) )
    p++;

  o = list_pars[(*listlen)++];

  if( pa & P3 )
  { while( *p && (*p == '_' || isalnum(*p) || strchr(prg_pars[2], *p))
           && MAX_LEN_LIST_PARS > o - list_pars[*listlen-1] )
      *o++ = *p++;
  }
  else
  { while( (*p == '_' || isalnum(*p)) && MAX_LEN_LIST_PARS > o - list_pars[*listlen-1] )
      *o++ = *p++;
  }
  *o++ = '\0';

  if( kflag )
  { if( *p == '}' )
      p++;
    else
      return false;
  }

  LOG(13, "do_list_getname, len: %ld.\n", (long)(p-prg_pars[0]));
  snprintf(list_pars[(*listlen)++], MAX_LEN_LIST_PARS, "%ld", (long)(p-prg_pars[0]));
  list_pars[*listlen][0] = kflag ? '}' : *p;
  list_pars[(*listlen)++][1] = '\0';

  LOG(2, "/do_list_getname, listlen: %d\n", *listlen);
  return false;
}


/***************************************************************************************/
/* short do_isodate(int pa, char **out, long n,                                        */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_isodate wandelt Datum ins ISO-Format                                         */
/***************************************************************************************/
short do_isodate(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int t, m, j;

  LOG(1, "do_isodate, prg_pars[0]: %s.\n", prg_pars[0]);

  if( strchr(prg_pars[0], '-') )
    sscanf(prg_pars[0], "%d-%d-%d", &j, &m, &t);
  else
    sscanf(prg_pars[0], "%d.%d.%d", &t, &m, &j);

  if( j < 100 )
    j += 2000;
  if( m < 1 || 12 < m )
    m = 1;
  if( t < 1 || 31 < t )
    t = 1;

  *out += snprintf(*out, n, "%04d-%02d-%02d", j, m, t);
  return false;
}


char wotag[7][3] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
char monam[12][4] = {"Jan", "Feb", "Mär", "Apr", "Mai", "Jun",
                     "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"};

/***************************************************************************************/
/* short do_date(int pa, char **out, long n,                                           */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_date Datum ausgeben                                                          */
/***************************************************************************************/
short do_date(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int j, lj;
  char *s, *o;
  time_t tt;
  struct tm *tm;
  int it, im, ij, istd, imin, isek;

  LOG(1, "do_date, prg_pars[0]: %s, prg_pars[1]: %s.\n",
      pa & P1 ? prg_pars[0] : "<NULL>", pa & P2 ? prg_pars[1] : "<NULL>");

  if( pa & P2 )
  { struct tm tms;

    if( prg_pars[1][3] == '-' )
      sscanf(prg_pars[1], "%d-%d-%d %d:%d:%d", &ij, &im, &it, &istd, &imin, &isek);
    else
    { sscanf(prg_pars[1], "%d.%d.%d %d:%d:%d", &it, &im, &ij, &istd, &imin, &isek);
      if( ij < 100 )
      { if( ij > 90 )
          ij += 1900;
        else
          ij += 2000;
      }
    }
    tms.tm_hour = istd;
    tms.tm_min = imin;
    tms.tm_sec = isek;
    tms.tm_mday = it;
    tms.tm_mon = im-1;
    tms.tm_year = ij-1900;
    tms.tm_isdst = -1;

    tt = mktime(&tms);
  }
  else
    time(&tt);

  tm = localtime(&tt);
  lj = tm->tm_year+1900;
  if( 1900 <= lj && lj < 2000 )
    j = lj - 1900;
  else if( 2000 <= lj && lj < 2100 )
    j = lj - 2000;
  else
    j = lj;

  if( pa & P1 )
    s = prg_pars[0];
  else
    s = "%d.%m.%y %H:%M:%S";

  for(o = *out; *s && (*out-o) < n-3 ; s++ )
  { if( *s == '%' && *(s+1) )
    { s++;
      switch( *s )
      { case 'a' : *(*out)++ =  wotag[tm->tm_wday][0];
		   *(*out)++ = wotag[tm->tm_wday][1];
		   break;
        case 'b' : *(*out)++ = monam[tm->tm_mon][0];
                   *(*out)++ = monam[tm->tm_mon][1];
                   *(*out)++ = monam[tm->tm_mon][2];
                   break;
        case 'm' : *out += sprintf(*out, "%02d", tm->tm_mon+1);
                   break;
        case 'd' : *out += sprintf(*out, "%02d", tm->tm_mday);
                   break;
        case 'y' : *out += sprintf(*out, "%02d", j);
                   break;
        case 'Y' : *out += sprintf(*out, "%04d", lj);
                   break;
        case 'H' : *out += sprintf(*out, "%02d", tm->tm_hour);
                   break;
        case 'M' : *out += sprintf(*out, "%02d", tm->tm_min);
                   break;
        case 's' : *out += sprintf(*out, "%lu", tt);
                   break;
        case 'S' : *out += sprintf(*out, "%02d", tm->tm_sec);
                   break;
        case 'F' : *out += sprintf(*out, "%04d-%02d-%02d", lj, tm->tm_mon+1, tm->tm_mday);
                   break;
        case 'T' : *out += sprintf(*out, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
                   break;
        case 'w' : *out += sprintf(*out, "%d", tm->tm_wday);
                   break;
        case '%' : *(*out)++ = '%';
                   break;
        default  : *(*out)++ = '%';
                   *(*out)++ = *s;
      }
    }
    else
    { if( *qf_strings[quote] && strchr(qf_strings[quote], *s) )
        *(*out)++ = *qf_strings[quote];
      *(*out)++ = *s;
    }
  }
  return false;
}


/***************************************************************************************/
/* short do_trim(int pa, char **out, long n,                                           */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_trim entfernt alle Zeichen aus P1 bzw. Blanks am Anfang und Ende             */
/***************************************************************************************/
short do_trim(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p, *q, *o;

  LOG(1, "do_trim, r: %s, s: %s.\n", prg_pars[0], prg_pars[1]);

  p = prg_pars[0];
  o = *out;
  if( pa & P2 )
  { while( strchr(prg_pars[1], *p) )
      p++;
  }
  else
  { while( isspace(*p) )
      p++;
  }

  if( *p )
  { q = p;
  
    while( *q )
      q++;
    q--;
    if( pa & P2 )
    { while( strchr(prg_pars[1], *q) )
        q--;
    }
    else
    { while( isspace(*q) )
        q--;
    }
  
    while( p <= q )
    { if( (*out-o) < n-1)
      { if( *qf_strings[quote] && strchr(qf_strings[quote], *p) )
          *(*out)++ = *qf_strings[quote];
        *(*out)++ = *p++;
      }
    }
  }
  return false;
}


/***************************************************************************************/
/* short do_ltrim(int pa, char **out, long n,                                          */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_ltrim entfernt alle Zeichen aus P1 bzw. Blanks am Anfang                     */
/***************************************************************************************/
short do_ltrim(int pa, char **out, long n,
             char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p, *o;

  LOG(1, "do_ltrim, r: %s, s: %s.\n", prg_pars[0], prg_pars[1]);

  p = prg_pars[0];
  o = *out;
  if( pa & P2 )
  { while( strchr(prg_pars[1], *p) )
      p++;
  }
  else
  { while( isspace(*p) )
      p++;
  }

  while( *p )
  { if( (*out-o) < n-1)
    { if( *qf_strings[quote] && strchr(qf_strings[quote], *p) )
        *(*out)++ = *qf_strings[quote];
      *(*out)++ = *p++;
    }
  }
  return false;
}


/***************************************************************************************/
/* short do_rtrim(int pa, char **out, long n,                                          */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_rtrim entfernt alle Zeichen aus P1 bzw. Blanks am Ende                       */
/***************************************************************************************/
short do_rtrim(int pa, char **out, long n,
             char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p, *q, *o;

  LOG(1, "do_rtrim, r: %s, s: %s.\n", prg_pars[0], prg_pars[1]);

  p = prg_pars[0];
  o = *out;

  if( *p )
  { q = p;
  
    while( *q )
      q++;
    q--;
    if( pa & P2 )
    { while( strchr(prg_pars[1], *q) )
        q--;
    }
    else
    { while( isspace(*q) )
        q--;
    }
  
    while( p <= q )
    { if( (*out-o) < n-1)
      { if( *qf_strings[quote] && strchr(qf_strings[quote], *p) )
          *(*out)++ = *qf_strings[quote];
        *(*out)++ = *p++;
      }
    }
  }
  return false;
}


/***************************************************************************************/
/* short do_replace(int pa, char **out, long n,                                        */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_replace suchen und ersetzen im String                                        */
/***************************************************************************************/
short do_replace(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char *p, *o;

  LOG(1, "do_replace, r: %s, s: %s, t: %s.\n", prg_pars[0], prg_pars[1], prg_pars[2]);

  o = *out;
  p = prg_pars[2];
  while( *p )
  { if( *p == prg_pars[0][0] && str_starts_z(&p, prg_pars[0]) )
      strqcpyn_z(out, prg_pars[1], n-(*out-o), qf_strings[quote]);
    else
    { if( (*out-o) < n-1)
      { if( *qf_strings[quote] && strchr(qf_strings[quote], *p) )
          *(*out)++ = *qf_strings[quote];
        *(*out)++ = *p++;
      }
    }
  }
  return false;
}


/***************************************************************************************/
/* short do_random(int pa, char **out, long n,                                         */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_random Zufallswert erzeugen                                                  */
/***************************************************************************************/
short do_random(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char zahl[128], *q, *p;
  double von, bis, r;
  short intflag = true;

  LOG(1, "do_random, p1: %s, p2: %s.\n", prg_pars[0], prg_pars[1]);

  if( (pa & P1) && isdigit(prg_pars[0][0]) )
  { if( strchr(prg_pars[0], '.') )
      intflag = false;
    von = atof(prg_pars[0]);
  }
  else
    von = 0.0;
  if( (pa & P2) && isdigit(prg_pars[1][0]) )
  { if( strchr(prg_pars[1], '.') )
      intflag = false;
    bis = atof(prg_pars[1]);
  }
  else
    bis = 1.0;

  if( intflag )
    bis = bis + 1;

  LOG(5, "do_random, von: %lf, bis: %lf.\n", von, bis);

  if( bis > von )
  {
#ifdef OLDUNIX
    r = rand();
#else
    r = random();
#endif
    LOG(5, "do_random, r: %lf.\n", r);
    r = r/(RAND_MAX + 1.0)*(bis-von)+von;
    LOG(5, "do_random, r: %lf.\n", r);

    if( intflag )
      snprintf(zahl, 128, "%ld", (long)r);
    else
      snprintf(zahl, 128, "%lf", r);
    if( NULL != (p = strchr(zahl, '.')) )
    { q = p + strlen(p) - 1;
      while( *q == '0' && q-p > 1 )
        *q-- = '\0';
      if( *q == '.' )
        *q = '\0';
    }
    strcpyn_z(out, zahl, n);
  }
  return false;
}


/***************************************************************************************/
/* short do_readline(int pa, char **out, long n,                                       */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_readline eine Zeile von Datei lesen                                          */
/***************************************************************************************/
short do_readline(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int i;

  LOG(1, "do_readline, f: %s.\n", prg_pars[0]);

  if( pa & P1 )
  { if( isdigit(prg_pars[0][0]) )
      i = atoi(prg_pars[0]);
    else
      return true;
  }
#ifdef INTERPRETER
  else
    i = 0;
#else
#ifdef HAS_DAEMON
  else if( !daemonflag )
    i = 0;
#endif
  else
    return true;
#endif

  if( file_ptrs[i] )
  { if( *qf_strings[quote] )
    { int c;
      char *o;

      o = *out;
      c = getc(file_ptrs[i]);
      while( *out - o < n-1 && c != '\n' && c != '\r' && c != EOF )
      { if( strchr(qf_strings[quote], (char)c) )
          *(*out)++ = *qf_strings[quote];
        *(*out)++ = c;
      }
      if( c == '\n' || c == '\r' )
      { int d;
        d = getc(file_ptrs[i]);
        if( d != c && d != '\n' && d != '\r' )
          ungetc(d, file_ptrs[i]);
      }
    }
    else
    { if( fgets(*out, n, file_ptrs[i]) )
      { while( **out
               && (*(*out+strlen(*out)-1) == '\n' || *(*out+strlen(*out-1)) == '\r') )
          *(*out+strlen(*out)-1) = '\0';
        *out += strlen(*out);
      }
      else
        return true;
    }
  }
  return false;
}


/***************************************************************************************/
/* short do_fileopen(int pa, char **out, long n,                                       */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_fileopen Datei zum Lesen/Schreiben öffnen                                    */
/***************************************************************************************/
short do_fileopen(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int i;

  LOG(1, "do_fileopen, f: %s.\n", prg_pars[0]);

  for( i = 0; i < MAX_ANZ_OPEN_FILES; i++)
    if( !file_ptrs[i] )
      break;
  if( i >= MAX_ANZ_OPEN_FILES )
    return true;

  if( !(file_ptrs[i] = fopen(prg_pars[0], "r+")) )
    return true;

  LOG(5, "do_fileopen, i: %d.\n", i);

  *out += snprintf(*out, n, "%d", i);

  return false;
}


/***************************************************************************************/
/* short do_fileappend(int pa, char **out, long n,                                     */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_fileapend Datei zum Lesen/Schreiben ans Ende der Datei öffnen                */
/***************************************************************************************/
short do_fileappend(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int i;

  LOG(1, "do_fileappend, f: %s.\n", prg_pars[0]);

  for( i = 0; i < MAX_ANZ_OPEN_FILES; i++)
    if( !file_ptrs[i] )
      break;
  if( i >= MAX_ANZ_OPEN_FILES )
    return true;

  if( !(file_ptrs[i] = fopen(prg_pars[0], "a")) )
    return true;

  LOG(5, "do_fileappend, i: %d.\n", i);

  *out += snprintf(*out, n, "%d", i);

  return false;
}


/***************************************************************************************/
/* short do_filecreate(int pa, char **out, long n,                                     */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_filecreate Datei zum Schreiben anlegen                                       */
/***************************************************************************************/
short do_filecreate(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ int i;

  LOG(1, "do_filecreate, f: %s.\n", prg_pars[0]);

  for( i = 0; i < MAX_ANZ_OPEN_FILES; i++)
    if( !file_ptrs[i] )
      break;
  if( i >= MAX_ANZ_OPEN_FILES )
    return true;

  if( !(file_ptrs[i] = fopen(prg_pars[0], "w+")) )
    return true;

  *out += snprintf(*out, n, "%d", i);

  return false;
}


/***************************************************************************************/
/* short do_readfile(int pa, char **out, long n,                                       */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_readfile fügt Inhalt einer Datei ein, oder wenn Parameter angegeben ist,    */
/*              setzt Variable mit Inhalt der Datei                                    */
/***************************************************************************************/
short do_readfile(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ FILE *ptr;
  size_t nb;

  LOG(1, "do_readfile, p1: %s.\n", prg_pars[0]);

  if( NULL != (ptr = fopen(prg_pars[0], "r")) )
  { if( pa & P2 )                                 /* Variable angegeben?               */
    { LOG(3, "do_readfile, p2: %s.\n", prg_pars[1]);
      if( 0 < (nb = fread(zeile, 1, MAX_ZEILENLAENGE, ptr)) )
      { if( nb > STRINGW )
          nb = STRINGW;
        change_parwert(prg_pars[1], zeile, nb, HP_GLOBAL_LEVEL);
      }
    }
    else
    { if( out )
      { while( 0 < (nb = fread(zeile, 1, MAX_ZEILENLAENGE, ptr)) )
        { strqcpyn_z(out, zeile, min(nb, n), qf_strings[quote]);
          n -= nb;
        }
      }
      else
      { while( 0 < (nb = fread(zeile, 1, MAX_ZEILENLAENGE, ptr)) )
          dobinsend(zeile, nb);
      }
    }
    fclose(ptr);
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* short do_send_html(int pa, char **out, long n,                                      */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*     do_send_html HMTL-Code einfügen                                                 */
/***************************************************************************************/
short do_send_html(int pa, char **out, long n,
                 char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  return dosend(prg_pars[0]);
}


/***************************************************************************************/
/* short do_filesize(int pa, char **out, long n,                                       */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_filesize Größe einer Datei ausgeben                                          */
/***************************************************************************************/
short do_filesize(int pa, char **out, long n,
                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char zahl[128];
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#else
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#endif
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#endif

  LOG(1, "do_filesize, p1: %s.\n", prg_pars[0]);

  if( !stat64(prg_pars[0], &stat_buf) )
  { snprintf(zahl, 128, "%ld", (long)stat_buf.st_size);
    strcpyn_z(out, zahl, n);
  }
  return false;
}


/***************************************************************************************/
/* short u2w_hink(int pa, char **out, long n,                                          */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*          int pa: Bitflag mit gesetzten Parameterfeldern                             */
/*          char prg_pars: Parameter                                                   */
/*     u2w_hlink Hypertextlink erzeugen                                                */
/***************************************************************************************/
short u2w_hlink(int pa, char **out, long n,
              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char z[MAX_ZEILENLAENGE], *p;

  LOG(1, "u2w_hlink, ref: %s, pa: %d.\n", prg_pars[0], pa);

  code_link(zeile, prg_pars[0], MAX_ZEILENLAENGE, false);
  p = z;

  if( pa & P3 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " target=\"%s\"", prg_pars[2]);
  if( pa & P4 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", prg_pars[3]);
  else if( css_defs[CSS_LINK])
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", css_defs[CSS_LINK]);
  if( pa & P5 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " %s", prg_pars[4]);
  *p = '\0';
  if( out )
  { if( pa & P6 )
      *out += snprintf(*out, n, HREF_HTML_ADD, zeile, prg_pars[5], z, prg_pars[1]);
    else
      *out += snprintf(*out, n, HREF_HTML, zeile, z, prg_pars[1]);
    return false;
  }
  else
  { if( pa & P6 )
      return dosendf(HREF_HTML_ADD, zeile, prg_pars[5], z, prg_pars[1]);
    else
      return dosendf(HREF_HTML, zeile, z, prg_pars[1]);
  }
}


/***************************************************************************************/
/* short u2w_iframe(int pa, char **out, long n,                                        */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*          int pa: Bitflag mit gesetzten Parameterfeldern                             */
/*          char prg_pars: Parameter                                                   */
/*     u2w_iframe iframe Elemente erzeugen                                             */
/***************************************************************************************/
short u2w_iframe(int pa, char **out, long n,
               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ char z[MAX_ZEILENLAENGE], *p;

  LOG(1, "u2w_iframe, ref: %s, pa: %d.\n", prg_pars[0], pa);

  code_link(zeile, prg_pars[0], MAX_ZEILENLAENGE, false);
  p = z;
  if( pa & P2 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " name=\"%s\"", prg_pars[1]);
  if( pa & P3 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", prg_pars[2]);
  else if( css_defs[CSS_LINK])
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", css_defs[CSS_LINK]);
  if( pa & P4 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " %s", prg_pars[3]);
  *p = '\0';
  if( out )
  { if( pa & P5 )
      *out += snprintf(*out, n, IFRAME_HTML_ADD, zeile, prg_pars[4], z);
    else
      *out += snprintf(*out, n, IFRAME_HTML, zeile, z);
    return false;
  }
  else
  { if( pa & P5 )
      return dosendf(IFRAME_HTML_ADD, zeile, prg_pars[4], z);
    else
      return dosendf(IFRAME_HTML, zeile, z);
  }
}


/***************************************************************************************/
/* short u2w_url(int pa, char **out, long n,                                           */
/*              char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)          */
/*          int pa: Bitflag mit gesetzten Parameterfeldern                             */
/*          char prg_pars: Parameter                                                   */
/*     u2w_url Text für URL codieren                                                   */
/***************************************************************************************/
short u2w_url(int pa, char **out, long n,
            char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{ if( out )
  { *out = code_link(*out, prg_pars[0], n, (pa & P2) && atoi(prg_pars[1]));
    return false;
  }
  else
  { char z[MAX_ZEILENLAENGE];

    code_link(z, prg_pars[0], MAX_ZEILENLAENGE, (pa & P2) && atoi(prg_pars[1]));
    return dosend(zeile);
  }
}


/***************************************************************************************/
/* short u2w_queryvars(int pa, char **out, long n,                                     */
/*                char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)        */
/*                int pa: Anzahl Parameter in prg_pars                                 */
/*                char **out: Ziel des Ergebnisses                                     */
/*                long n    : Platz in out                                             */
/*                char prg_pars: übergebene Funktionsparameter                         */
/*     u2w_queryvars belegte Variablen aus P1 zu <var1>=$>var1>&<varn>=$<varn> wandeln */
/***************************************************************************************/
/* steht in u2w_par.c, weil inline Funktion genutzt wird.                              */
/***************************************************************************************/



#ifdef WITH_GETTEXT
/***************************************************************************************/
/* short do_decimalpoint(int pa, char **out, long n,                                   */
/*               char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*      do_decimalpoint fügt Wert des Dezimalpunktes entsprechend der locale ein       */
/***************************************************************************************/
short do_decimalpoint(int pa, char **out, long n,
                    char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
  strqcpyn_z(out, dezimalpunkt, n, qf_strings[quote]);
  return false;
}
#endif


#ifdef WITH_HTTPS
/***************************************************************************************/
/* short do_ssl_subject(int pa, char **out, long n,                                    */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_ssl_subject %sslsubject                                                      */
/***************************************************************************************/
short do_ssl_subject(int pa, char **out, long n,
                   char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strqcpyn_z(out, ssl_client_subject, n, qf_strings[quote]);
#endif
  return false;
}


/***************************************************************************************/
/* short do_ssl_issuer(int pa, char **out, long n,                                     */
/*                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char **out: Ziel des Ergebnisses                                       */
/*              long n    : Platz in out                                               */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*     do_ssl_issuer %sslissuer                                                        */
/***************************************************************************************/
short do_ssl_issuer(int pa, char **out, long n,
                  char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], int quote)
{
#ifdef WEBSERVER
  strqcpyn_z(out, ssl_client_issuer, n, qf_strings[quote]);
#endif
  return false;
}
#endif  /* WITH_HTTPS */

#endif  /* #ifndef COMPILER */
