/*************************************************/
/* File: u2w_str.c                               */
/* Funktionen zur Stringverabreitung             */
/* timestamp: 2016-03-12 19:54:59                */
/*************************************************/


#include "u2w.h"

#if defined(WEBSERVER) || defined(INTERPRETER)

/***************************************************************************************/
/* char last_non_ws_char(char *s, long n)                                              */
/*                char *s: Zeile                                                       */
/*                long n   : Länge von s                                               */
/*      last_non_ws_char bestimmt letztes nicht Leerzeichen des Strings s              */
/***************************************************************************************/
char last_non_ws_char(char *s, long n)
{ long i;

  i = n-1;
  while( i >= 0 && *(s+i) == ' ' )
    i--;
  if( i < 0 )
    return '\0';
  else
    return *(s+i);
}


/***************************************************************************************/
/* void strcpyn_qs(char *out, char **in, long n)                                       */
/*                 char *out: Ausgabezeile                                             */
/*                 char **in: Eingabezeile                                             */
/*                 long n   : Maximal n Zeichen                                        */
/*      strcpyn_qs kopiert in nach out bis ein Leerzeichen, Tab oder \n gefunden wird, */
/*                 dabei werden Strings, die durch "..." umgeben sind beachtet.        */
/*                 '\' ist Quotingchar innerhalb "..."                                 */
/***************************************************************************************/
void strcpyn_qs(char *out, char **in, long n)
{
  skip_blanks(*in);
  if( **in == '"' )
  { (*in)++;
    while( **in && **in != '"' && --n > 0 )
    { if( **in == '\\' )
      { if( *++(*in) )
          *out++ = *(*in)++;
      }
      else
        *out++ = *(*in)++;
    }
    if( **in == '"' )
      (*in)++;
  }
  else
    while( **in && **in != ' ' && **in != '\t' && **in != '\n' && **in != '\r' && --n > 0 )
      *out++ = *(*in)++;
  *out = '\0';
}


/***************************************************************************************/
/* void strcpyn_s(char *out, char **in, long n)                                        */
/*                char *out: Ausgabezeile                                              */
/*                char **in: Eingabezeile                                              */
/*                long n   : Maximal n Zeichen                                         */
/*      strcpyn_s kopiert in nach out bis ein Leerzeichen, Tab oder \n gefunden wird,  */
/*                dabei werden Strings, die durch "..." umgeben sind beachtet.         */
/***************************************************************************************/
void strcpyn_s(char *out, char **in, long n)
{
  skip_blanks(*in);
  if( **in == '"' )
  { (*in)++;
    while( **in && **in != '"' && --n > 0 )
      *out++ = *(*in)++;
    if( **in == '"' )
      (*in)++;
  }
  else
    while( **in && **in != ' ' && **in != '\t' && **in != '\n' && **in != '\r' && --n > 0 )
      *out++ = *(*in)++;
  *out = '\0';
}


/***************************************************************************************/
/* void strcpyn_alnum(char *out, char **in, long n)                                    */
/*                    char *out: Ausgabezeile                                          */
/*                    char **in: Eingabezeile                                          */
/*                    long n   : Maximal n Zeichen                                     */
/*      strcpyn_alnum kopiert in nach out bis kein Alnum gefunden wird                 */
/***************************************************************************************/
void strcpyn_alnum(char *out, char **in, long n)
{
  skip_blanks(*in);
  while( --n > 0 && (isalnum(0 | **in) || **in == '_') )
    *out++ = *(*in)++;
  *out = '\0';
}


/***************************************************************************************/
/* char *strcpyn_str(char *out, char **in, char *ez, long n)                           */
/*                  char *out: Ausgabezeile                                            */
/*                  char **in: Eingabezeile                                            */
/*                  char *ez : Zeichen, bei denen nicht weiterkopiert wird             */
/*                  long n   : Maximal n Zeichen                                       */
/*                  return   : Ende des Strings ('\0')                                 */
/*      strcpyn_str kopiert in nach out bis Zeichen aus ez gefunden wird               */
/***************************************************************************************/
char *strcpyn_str(char *out, char **in, char *ez, long n)
{
  while( **in && NULL == strchr(ez, **in) && --n > 0 )
    *out++ = *(*in)++;
  *out = '\0';
  return out;
}


/***************************************************************************************/
/* char *strcpyn_str_s(char *out, char *in, char *ez, long n)                          */
/*                  char **out: Ausgabezeile                                           */
/*                  char *in: Eingabezeile                                             */
/*                  char *ez : Zeichen, bei denen nicht weiterkopiert wird             */
/*                  long n   : Maximal n Zeichen                                       */
/*                  return   : Ende des Strings ('\0')                                 */
/*      strcpyn_str_s kopiert in nach out bis Zeichen aus ez gefunden wird             */
/***************************************************************************************/
char *strcpyn_str_s(char *out, char *in, char *ez, long n)
{
  while( *in && NULL == strchr(ez, *in) && --n > 0 )
    *out++ = *in++;
  *out = '\0';
  return out;
}


/***************************************************************************************/
/* int strcpyn_z(char **out, char *in, long n)                                         */
/*                char **out: Ausgabezeile                                             */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen                                        */
/*    return: 0, wenn in **out genug Platz, sonst != 0                                 */
/*      strcpyn_z kopiert in nach out                                                  */
/***************************************************************************************/
int strcpyn_z(char **out, char *in, long n)
{
  while( *in && n-- > 0 )
    *(*out)++ = *in++;
  return *in;
}


/***************************************************************************************/
/* int strcpyn_shell_z(char **out, char *in, long n)                                   */
/*                char **out: Ausgabezeile                                             */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen                                        */
/*    return: 0, wenn in **out genug Platz, sonst != 0                                 */
/*      strcpyn_shell_z kopiert in nach out, ' werden durch '"'"' ersetzt              */
/***************************************************************************************/
int strcpyn_shell_z(char **out, char *in, long n)
{ 
  while( *in && n-- > 0 )
  { if( *in == '\'' )
    { if( n < 4 )
        return true;
      *(*out)++ = '\'';
      *(*out)++ = '"';
      *(*out)++ = '\'';
      *(*out)++ = '"';
      n -= 4;
    }
    *(*out)++ = *in++;
  }
  return *in;
}


/***************************************************************************************/
/* int strqcpyn_z(char **out, char *in, long n, char *quote)                           */
/*                char **out: Ausgabezeile                                             */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen                                        */
/*    return: 0, wenn in **out genug Platz, sonst != 0                                 */
/*      strqcpyn_z kopiert in nach out und Quotet Zeichen aus quote                    */
/***************************************************************************************/
int strqcpyn_z(char **out, char *in, long n, char *quote)
{
  if( *quote )
  { while( *in && --n > 0 )
    { if( strchr(quote, *in) )
      { n--;
        *(*out)++ = *quote;
      }
      *(*out)++ = *in++;
    }
    return *in;
  }
  else
    return strcpyn_z(out, in, n);
}


#if defined(MAYDBCLIENT) || defined(SQLITE3)
/***************************************************************************************/
/* void strqcpynm_z(char **out, char *in, long n, long m, char *quote)                 */
/*                char **out: Ausgabezeile                                             */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen nach *out                              */
/*                long m    : Maximal m Zeichen aus in                                 */
/*      strqcpynm_z kopiert in nach out und Quotet Zeichen aus quote                   */
/***************************************************************************************/
void strqcpynm_z(char **out, const char *in, long n, long m, char *quote)
{ char *o, *i;

  LOG(11, "strqcpynm_z, in: %.200s, n: %ld, m: %ld, quote: %s.\n", in, n, m, quote);

  o = *out;
  i = in;

  if( *quote )
  { while( *in && *out - o < n-1 && in - i < m )
    { if( strchr(quote, *in) )
        *(*out)++ = *quote;
      *(*out)++ = *in++;
    }
  }
  else
    while( *in && *out - o < n && in - i < m )
      *(*out)++ = *in++;
}


/***************************************************************************************/
/* void strcpynm(char *out, char *in, long n, long m)                                  */
/*                char *out: Ausgabezeile                                              */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen nach *out                              */
/*                long m    : Maximal m Ziechen aus in                                 */
/*      strqcpynm_z kopiert in nach out                                                */
/***************************************************************************************/
void strcpynm(char *out, const char *in, long n, long m)
{ char *o, *i;

  LOG(11, "strcpynm, in: %.200s, n: %ld, m: %ld.\n", in, n, m);

  o = out;
  i = in;

  while( *i && o-out-1 < n && i - in < m )
    *o++ = *i++;
  *o = '\0';
}
#endif

#ifdef MAYDBCLIENT
/***************************************************************************************/
/* int w_mysql_cpyn_z(char **out, wertetype *w, long n)                                */
/*                char **out: Ausgabezeile                                             */
/*                wertetype *w: Wert, der eingefügt werden soll                        */
/*                long n    : Maximal n Zeichen                                        */
/*     return: 0, wenn in **out genug Platz, sonst != 0                                */
/*      w_mysql_cpyn_z kopiert Wert nach out und Quotet Zeichen für Mysql              */
/***************************************************************************************/
int w_mysql_cpyn_z(char **out, wert *w, long n)
{ long nb;
  char z[128];
  char *in, *q, *o;

  LOG(1, "w_mysql_cpyn_z.\n");

  switch( w->type )
  { case EMPTYW:  return 0;
    case BINARYW: in = w->s;
                  nb = w->len;
                  break;
    case STRINGW: in = w->s;
                  nb = strlen(w->s);
                  break;
    case DOUBLEW: snprintf(z, 128, "%f", w->w.d);
#ifdef WITH_GETTEXT
                  if( NULL != (in = strchr(z, *dezimalpunkt)) )
#else
                  if( NULL != (in = strchr(z, '.')) )
#endif
                  { q = in + strlen(in) - 1;
                    while( *q == '0' && q-in > 1 )
                      *q-- = '\0';
                  }
                  in = z;
                  nb = strlen(z);
                  break;
    case LONGW:   snprintf(z, 128, LONGFORMAT, w->w.l);
                  in = z;
                  nb = strlen(z);
                  break;
    default:      return 0;
  }

  LOG(5, "w_mysql_cpyn_z, nb: %ld, daten: %.200s.\n", nb, in);

  o = *out;
  q = in;
  while( in-q < nb && *out-o < n-1 )
  { switch( *in )
    { case '\0': *(*out)++ = '\\';
                 *(*out)++ = '0';
                 in++;
                 break;
      case '\\':
      case '\'': *(*out)++ = '\\';
      default:   *(*out)++ = *in++;
    }
  }
  return *out-o >= n-1;
}
#endif  /* #ifdef MAYDBCLIENT */


#ifdef SQLITE3
/***************************************************************************************/
/* int w_sqlite3_cpyn_z(char **out, wertetype *w, long n)                              */
/*                char **out: Ausgabezeile                                             */
/*                wertetype *w: Wert, der eingefügt werden soll                        */
/*                long n    : Maximal n Zeichen                                        */
/*     return: 0, wenn in **out genug Platz, sonst != 0                                */
/*      w_sqlite3_cpyn_z kopiert Wert nach out und Quotet Zeichen für Sqlite3          */
/***************************************************************************************/
int w_sqlite3_cpyn_z(char **out, wert *w, long n)
{ long nb;
  char z[128];
  char *in, *q, *o;

  LOG(1, "w_sqlite3_cpyn_z.\n");

  switch( w->type )
  { case EMPTYW:  return 0;
    case BINARYW: in = w->s;
                  nb = w->len;
                  break;
    case STRINGW: in = w->s;
                  nb = strlen(w->s);
                  break;
    case DOUBLEW: snprintf(z, 128, "%f", w->w.d);
#ifdef WITH_GETTEXT
                  if( NULL != (in = strchr(z, *dezimalpunkt)) )
#else
                  if( NULL != (in = strchr(z, '.')) )
#endif
                  { q = in + strlen(in) - 1;
                    while( *q == '0' && q-in > 1 )
                      *q-- = '\0';
                  }
                  in = z;
                  nb = strlen(z);
                  break;
    case LONGW:   snprintf(z, 128, LONGFORMAT, w->w.l);
                  in = z;
                  nb = strlen(z);
                  break;
    default:      return 0;
  }

  LOG(5, "w_sqlite3_cpyn_z, nb: %ld, daten: %.200s.\n", nb, in);

  o = *out;
  q = in;
  while( in-q < nb && *out-o < n-1 )
  { if( *in == '\'' )
      *(*out)++ = '\'';
    *(*out)++ = *in++;
  }
  return *out-o >= n-1;
}
#endif  /* #ifdef SQLITE3 */


/***************************************************************************************/
/* void strqcpyn(char *out, char *in, long n, char *quote)                             */
/*                char *out: Ausgabezeile                                              */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen                                        */
/*      strqcpyn kopiert in nach out und Quotet Zeichen aus quote                      */
/***************************************************************************************/
void strqcpyn(char *out, char *in, long n, char *quote)
{ char *o;

  o = out;
  strqcpyn_z(&o, in, n, quote);
  *o = '\0';
}


/***************************************************************************************/
/* int strskip_z(char **in, char *such)                                                */
/*               char **in : Eingabezeile                                              */
/*               char *such: wird uebersprungen                                        */
/*               return    : true, wenn such uebersprungen, false, wenn such nicht     */
/*                           komplett gefunden                                         */
/*     strskip_z ueberspringt such und Leerzeichen und Tabs                            */
/***************************************************************************************/
int strskip_z(char **in, char *such)
{ skip_blanks(*in);
  while( **in && **in == *such )
  { (*in)++;
    such++;
  }
  skip_blanks(*in);
  return !*such;
}


/***************************************************************************************/
/* char *strchr_t(char *in, char such)                                                 */
/*                char *in : Eingabezeile                                              */
/*                char such: wird gesucht                                              */
/*                return   : Position von such, sonst NULL                             */
/*       strchr_t sucht such in in, dabei werden "..." Blöcke übersprungen             */
/***************************************************************************************/
char *strchr_t(char *in, char such)
{ while( *in )
  { if( *in == such )
      return in;
    else if( *in == '"' )
    { in++;
      while( *in && *in++ != '"' );
    }
    else
      in++;
  }
  return NULL;
}


/***************************************************************************************/
/* char *strcasestr_hs(char *s, const char *t)                                         */
/*                  char *s: String, in dem gesucht wird                               */
/*                  const char *t: String, der gesucht werden soll                     */
/*                  return       : Pointer auf Fundstelle des Strings t                */
/*                                 NULL, wenn t nicht gefunden                         */
/*       strcasestr_hs sucht den String t im String s ohne Gross/Klein zu unterscheiden*/
/***************************************************************************************/
char *strcasestr_hs(char *s, const char *t)
{ char *p, *pp, c;
  const char *q;

  c = toupper(*t++);
  for( p = s; *p; p++ )
  { if( c == toupper(*p) )
    { q = t;
      pp = p+1;
      while( *q && toupper(*q) == toupper(*pp) )
      { pp++;
        q++;
      }
      if( !*q )
        return p;
    }
  }
  return NULL;
}


/***************************************************************************************/
/* int strcaseeq(const char *s, const char *t)                                         */
/*               const char *s: String, zum Vergleichen                                */
/*               const char *t: String, zum Vergleichen                                */
/*               return       : true, t und s sind gleich                              */
/*     strcaseeq vergelicht s mit t ohne auf Gross/Klein zu achten                     */
/***************************************************************************************/
int strcaseeq(const char *s, const char *t)
{
  if( s )
  { while( *s && *t )
    { if( tolower(*s++) != tolower(*t++) )
        return false;
    }

    return !*s && !*t;
  }
  else
    return false;
}


/***************************************************************************************/
/* int is_elem(char *s, const char *t)                                                 */
/*             char *s      : String, in dem gesucht wird                              */
/*             const char *t: String, der gesucht werden soll                          */
/*             return       : true, String wurde gefunden                              */
/*     is_elem sucht den String t in s als ganzes wort                                 */
/***************************************************************************************/
int is_elem(char *s, const char *t)
{ char elem[PARLEN];

  while( *s )
  { strcpyn_qs(elem, &s, PARLEN);
    if( strcmp(elem, t) == 0 )
      return true;

    skip_blanks(s);
  }
  return false;
}


/***************************************************************************************/
/* int is_command(const char *s, const char *t)                                        */
/*                const char *s: String, der getestet wird                             */
/*                const char *t: String, mit dem verglichen wird                       */
/*                return       : true, t ist erstes Wort von s                         */
/*     is_command testet, ob t erstes Wort von s ist, in s folgen nur Sonderzeichen    */
/***************************************************************************************/
int is_command(const char *s, const char *t)
{ while( *t )
  { if( *t++ != *s++ )
      return false;
  }
  return !isalnum(0 | *s) && *s != '_';
}


/***************************************************************************************/
/* int path_starts(const char *s, const char *t)                                       */
/*                const char *s: String, der getestet wird                             */
/*                const char *t: String, mit dem verglichen wird                       */
/*                return       : true, s ist subpath von t                             */
/*     path_starts testet, ob s ein subpath von t  oder gleich t ist                   */
/***************************************************************************************/
int path_starts(const char *s, const char *t)
{ while( *t )
  { if( *t++ != *s++ )
      return false;
  }
  return !*s || *s == '/';
}


/***************************************************************************************/
/* int is_function_z(char **s, const char *t)                                          */
/*                  char **s: String, der getestet wird                                */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t ist erstes Wort von s                       */
/*     is_function_z testet, ob t erstes Wort von s ist, in s folgen nur Sonderzeichen */
/*                  bei Erfolg zeigt *s auf erstes Zeichen nach t                      */
/***************************************************************************************/
int is_function_z(char **s, const char *t)
{ char *ss;

  ss = *s;
  while( *t )
  { if( *t++ != *ss++ )
      return false;
  }
  if( isalnum(0 | *ss) || *ss == '_' )
    return false;
  else
  { if( *ss == ' ' )
      ss++;
    *s = ss;
    return true;
  }
}


/***************************************************************************************/
/* int is_command_z(char **s, const char *t)                                           */
/*                  char **s: String, der getestet wird                                */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t ist erstes Wort von s                       */
/*     is_command_z testet, ob t erstes Wort von s ist, in s folgen nur Sonderzeichen  */
/*                  bei Erfolg zeigt *s auf erstes nicht Leerzeichen nach t            */
/***************************************************************************************/
int is_command_z(char **s, const char *t)
{ if( is_function_z(s, t) )
  { while( **s == ' ' )
      (*s)++;
    return true;
  }
  else
    return false;
}


/***************************************************************************************/
/* int get_command_z(char **s, char *cmd)                                              */
/*                  char **s: String, der gelesen wird                                 */
/*                  char *cmd: Platz fuer gefundenes Kommando                          */
/*      get_command_z Befehl einlesen - alle Zeichen a-zA-Z_                           */
/***************************************************************************************/
int get_command_z(char **s, char *cmd)
{ char *c;

  c = cmd;
  while( **s && (isalnum(**s) || **s == '_' || **s == '/' )
         && (cmd - c) < MAX_PAR_NAME_LEN )
    *cmd++ = *(*s)++;
  *cmd = '\0';
  if( **s == ' ' )
  { (*s)++;
    return true;
  }
  else
    return false;
}


/***************************************************************************************/
/* int is_befehl_z(char **s, const char *t)                                            */
/*                  char **s: String, der getestet wird                                */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t ist erstes Wort von s                       */
/*     is_befehl_z  testet, ob t erstes Wort von s ist, in s folgen nur Sonderzeichen  */
/*                  bei Erfolg zeigt *s auf erstes Sonderzeichen                       */
/***************************************************************************************/
int is_befehl_z(char **s, const char *t)
{ char *ss;

  ss = *s;
  while( *t )
  { if( *t++ != *ss++ )
      return false;
  }
  if( isalpha(0 | *ss) || *ss == '_' )
    return false;
  else
  { if( *ss == ' ' )
      *s = ss+1;
    else
      *s = ss;
    return true;
  }
}


/***************************************************************************************/
/* int is_num_befehl_z(char **s, const char *t)                                        */
/*                  char **s: String, der getestet wird                                */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t ist erstes Wort von s                       */
/*     is_num_befehl_z  testet, ob t erstes Wort von s ist, in s folgen Ziffern        */
/***************************************************************************************/
int is_num_befehl_z(char **s, const char *t)
{ char *ss;

  ss = *s;
  while( *t )
  { if( *t++ != *ss++ )
      return false;
  }
  if( isdigit(0 | *ss) )
  { *s = ss;
    return true;
  }
  else
    return false;
}


/***************************************************************************************/
/* int str_starts_z(char **s, const char *t)                                           */
/*                  char **s     : String, der getestet wird                           */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t ist Anfang von s                            */
/*     str_starts_z testet, ob t am Anfang von s steht, bei Erfolg zeigt *s auf erstes */
/*                  Zeichen hinter String aus t                                        */
/***************************************************************************************/
int str_starts_z(char **s, const char *t)
{ char *ss;

  ss = *s;
  while( *t )
  { if( *t++ != *ss++ )
      return false;
  }
  *s = ss;
  return true;
}


/***************************************************************************************/
/* int str_lcasestarts_z(char **s, const char *t)                                      */
/*                  char **s     : String, der getestet wird                           */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t ist Anfang von s                            */
/*     str_lcasestarts_z testet, ob t am Anfang von s steht, *s wird zu lowercase      */
/*                  konvertiert, so dass t in Lowercase sein muss. Bei Erfolg zeigt *s */
/*                  auf erstes Zeichen hinter String aus t                             */
/***************************************************************************************/
int str_lcasestarts_z(char **s, const char *t)
{ char *ss;

  ss = *s;
  while( *t )
  { if( tolower(*ss++) != *t++ )
      return false;
  }
  *s = ss;
  return true;
}


/***************************************************************************************/
/* int str_lcasecmp_z(char **s, const char *t)                                         */
/*                  char **s     : String, der getestet wird                           */
/*                  const char *t: String, mit dem verglichen wird                     */
/*                  return       : true, t steht in s und in s folgen nur Sonderzeichen*/
/*     str_lcasecmp_z testet, ob t in s steht, *s wird zu lowercase                    */
/*                  konvertiert, so dass t in Lowercase sein muss. *s zeigt auf        */
/*                  erstes nicht passendes Zeichen Zeichen hinter String aus t         */
/***************************************************************************************/
int str_lcasecmp_z(char **s, const char *t)
{ while( *t )
  { if( tolower(*(*s)++) != *t++ )
      return false;
  }
  return !isalnum(**s);
}


/***************************************************************************************/
/* int str_starts(const char *s, const char *t)                                        */
/*                const char *s: String, der getestet wird                             */
/*                const char *t: String, mit dem verglichen wird                       */
/*                return       : true, t ist Anfang von s                              */
/*     str_starts testet, ob t am Anfang von s steht                                   */
/***************************************************************************************/
int str_starts(const char *s, const char *t)
{ while( *t )
  { if( *t++ != *s++ )
      return false;
  }
  return true;
}


/***************************************************************************************/
/* int str_starts_conc(const char *s1, const char *s1e, const char *s2, const char *t) */
/*                const char *s1: Stringanfang                                         */
/*                const char *s1e: Ende String1,                                       */
/*                const char *s2: Stringende, der getestet wird                        */
/*                const char *t: String, mit dem verglichen wird                       */
/*                return       : true, t ist Anfang von concat(s1,s2)                  */
/*     str_starts testet, ob t am Anfang von s1s2 steht                                */
/***************************************************************************************/
int str_starts_conc(const char *s1, const char *s1e, const char *s2, const char *t)
{ while( *t && s1 < s1e )
  { if( *t++ != *s1++ )
      return false;
  }
  while( *t )
  { if( *t++ != *s2++ )
      return false;
  }
  return true;
}


/***************************************************************************************/
/* int test_ext(const char *s, const char *e)                                          */
/*              const char *s: String, der getestet wird                               */
/*              const char *e: extension, mit der verglichen wird                      */
/*              return       : true, s endet mit t                                     */
/*     test_ext testet, ob s die Extension e hat                                       */
/***************************************************************************************/
int test_ext(const char *s, const char *e)
{ if( strlen(s) < strlen(e) )
    return false;
  s += strlen(s) - strlen(e);
  while( *e )
  { if( tolower(*e) != tolower(*s) )
      return false;
    e++;
    s++;
  }
  return true;
}


/***************************************************************************************/
/* LONGWERT get_int_z(char **z)                                                        */
/*               char **z: Zeiger auf Eingabezeile                                     */
/*               return  : Wert der Zahl an z oder 0, wenn keine Zahl                  */
/***************************************************************************************/
LONGWERT get_int_z(char **z)
{ char a[80], *p;

  p = a;
  if( isdigit(**z) || **z == '-' )
  { *p++ = *(*z)++;
    while( isdigit(**z) )
      *p++ = *(*z)++;
    *p = '\0';
    return atoll(a);
  }
  else
    return 0;
}


/***************************************************************************************/
/* void abs_path(char *p, char *wd, char *d, long n)                                   */
/*               char *p : Pfad dort aufbauen                                          */
/*               char *wd: Workingdir                                                  */
/*               cahr *d : Pfad, der eingetragen werden soll, ggf. rel. zu wd          */
/*               long n  : Platz in p                                                  */
/*      abs_path baut in p einen absoluten Pfad auf                                    */
/***************************************************************************************/
void abs_path(char *p, char *wd, char *d, long n)
{ if( *d == '/' )                       /* *d absoluter Pfad, dann uebernehmen         */
    strcpyn(p, d, n);
  else
  { strcpyn(p, wd, n);
    if( wd[strlen(wd)-1] != '/' )
      strcatn(p, "/", n);               /* sonst an akutelles Verzeichnis anhaengen    */
    strcatn(p, d, n);
  }
}


/***************************************************************************************/
/* const char *conv_target(const char *s)                                              */
/*                const char *s: Framename                                             */
/*                return       : englischer Framename                                  */
/*       conv_target: ggf. deutschen Framenamen nach englsich übersetzen               */
/***************************************************************************************/
const char *conv_target(const char *s)
{ if( !strcmp(s, OLDFRAMEOBENNAME) )
    return MENUFRAMETOPNAME;
  else if( !strcmp(s, OLDFRAMERECHTSNAME) )
    return MENUFRAMERIGHTNAME;
  else if( !strcmp(s, OLDFRAMELINKSNAME) )
    return MENUFRAMELEFTNAME;
  else
    return s;
}


/***************************************************************************************/
/* int get_prg_pars(char **in, int n, ...)                                             */
/*              char **in: Zeiger auf Eingabezeile                                     */
/*              int n  : Anzahl der Parameter                                          */
/*              ...    : Parameter zum Speichern der Positionen                        */
/*                       char *: für Wert des Pars                                     */
/*                       int   : Länge des Wertes                                      */
/*                       char *: Quotingzeichen für Variablen                          */
/*              return: Anzahl nicht gefundener Parameter                              */
/*      get_prg_pars Setzt alle Parameter auf den Anfang des Parameter-Strings         */
/*              "..." werden beachtet und Strings werden mit '\0' beendet.             */
/*              *in Zeigt anschliessend auf Zeichen nach ')'                           */
/***************************************************************************************/
int get_prg_pars(char **in, int n, ...)
{ va_list ap;
  char *p, c, quote;
  int plen;

  LOG(1, "get_prg_pars, s: %.200s, n: %d.\n", *in, n);

  if( **in != '(' )
    return n != 0;

  (*in)++;

  va_start(ap, n);

  while( **in && n > 0 )
  { skip_blanks(*in);

    p = va_arg(ap, char *);
    plen = va_arg(ap, int);
    quote = va_arg(ap, int);
    n--;

    scan_to_z(&p, in, plen, 0, "),", '\0', quote);
    if( (c = **in) == ')' )
      break;
    else if( **in )
      (*in)++;
  }
  va_end(ap);
  while( **in && **in != ')' )
    (*in)++;
  if( **in )
    (*in)++;
  return n;
}


/***************************************************************************************/
/* int test_leer(char *s, int n)                                                       */
/*               const char *s: String, der getestet wird                              */
/*               int n: Länge des Strings                                              */
/*               return: true, s enthält nur Leerezeichen oder Tabs                    */
/*     test_leer, testet, ob s nur Leerzeichzen oder Tabs entählt                      */
/***************************************************************************************/
int test_leer(char *s, int n)
{ char *ss;

  ss = s;
  while( *ss && ss - s < n )
  { if( *ss != ' ' && *ss != '\t' )
      return false;
    ss++;
  }
  return true;
}


/***************************************************************************************/
/* void code_value(char *out, char *in, long n)                                        */
/*                char *out : Ausgabezeile                                             */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen                                        */
/*      code_value codiert " -> &quot;                                                 */
/***************************************************************************************/
void code_value(char *out, char *in, long n)
{ char *o;

  o = out;
  while( *in && (o-out) < n )
  { if( *in == '"' )
    { strcpyn_z(&o, "&quot;", n - (o-out));
      in++;
    }
    else
      *o++ = *in++;
  }
  *o = '\0'; 
}


/***************************************************************************************/
/* int code_html(char **out, char *in, long n)                                         */
/*                char **out : Ausgabezeile                                            */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen                                        */
/*   return: 0, wenn in *out genug Platz, sonst != 0                                   */
/*      code_html codiert <, >, & -> &lt;, &gt;, &amp;                                 */
/***************************************************************************************/
int code_html(char **out, char *in, long n)
{ char *o;

  LOG(1, "code_html, in: %.200s.\n", in);

  o = *out;
  while( *in && (*out-o) < n )
  { switch( *in )
    { case '<': strcpyn_z(out, "&lt;", n - (*out-o));
                in++;
                break;
      case '>': strcpyn_z(out, "&gt;", n - (*out-o));
                in++;
                break;
      case '&': strcpyn_z(out, "&amp;", n - (*out-o));
                in++;
                break;
      default:  *(*out)++ = *in++;
    }
  }

  LOG(2, "/code_html, out: %.*s.\n", (int)(*out-o), o);
  return *in;
}

/***************************************************************************************/
/* void code_html_nm(char **out, char *in, long n, long m)                             */
/*                char **out : Ausgabezeile                                            */
/*                char *in  : Eingabezeile                                             */
/*                long n    : Maximal n Zeichen ausgeben                               */
/*                long m    : Maximal m Ziechen aus in                                 */
/*      code_html_nm codiert <, >, & -> &lt;, &gt;, &amp;                              */
/***************************************************************************************/
void code_html_nm(char **out, char *in, long n, long m)
{ char *o, *i;

  LOG(1, "code_html_nm, in: %.200s.\n", in);

  o = *out;
  i = in;
  while( *i && (i-in) < m && (*out-o) < n )
  { switch( *i )
    { case '<': strcpyn_z(out, "&lt;", n - (*out-o));
                i++;
                break;
      case '>': strcpyn_z(out, "&gt;", n - (*out-o));
                i++;
                break;
      case '&': strcpyn_z(out, "&amp;", n - (*out-o));
                i++;
                break;
      default:  *(*out)++ = *i++;
    }
  }

  LOG(2, "/code_html_nm, out: %.*s.\n", (int)(*out-o), o);
}

/***************************************************************************************/
/* char *code_link(char *out, char *in, long n, int parflag)                           */
/*                char *out: hier wird der String abgelegt                             */
/*                char  *in: der zu konvertierende Link                                */
/*                long n:    Platz in out                                              */
/*                int parflag: true, es werden auch '?' und '&' Konvertiert.           */
/*         return: Ende des *out ('\0')                                                */
/*      code_link ersetzt Sonderzeichen durch %xx                                      */
/***************************************************************************************/
char *code_link(char *out, char *in, long n, int parflag)
{ unsigned char *s;
  char *o;
  int i;

  LOG(1, "code_link, in: %.200s.\n", in);

  s = (unsigned char *)in;
  o = out;
  while( *s && o - out < n - 4 )
  { if( *s == ' ' )
    { *o++ = '+';
      s++;
    }
    else if( *s < ' ' || *s == '"' || *s > '~' || *s == '%' || *s == '#' || *s == '+'
        || (parflag && (*s == '?' || *s == '&')) )     /* Sonderz.?                    */
    { *o++ = '%';                                      /* codieren                     */
      i = *s >> 4;
      *o++ = i + (i > 9 ? 'A'-10 : '0');
      i = *s & 0xf;
      *o++ = i + (i > 9 ? 'A'-10 : '0');
      s++;
    }
    else
      *o++ = *s++;
  }
  *o = '\0';
  return o;
}


/***************************************************************************************/
/* void strtolower(char *s, char *r, long n)                                           */
/*                 char *s: String in Kleinbuchstaben                                  */
/*                 char *t: String, der in kleine Zeichen gewandelt wird               */
/*                 long n: Platz in s                                                  */
/*      strtolower String s in kleine Zeichen konvertieren                             */
/***************************************************************************************/
void strtolower(char *s, char *t, long n)
{
  while( *t )
  { if( isupper(*t) )
      *s++ = tolower(*t++);
    else
      *s++ = *t++;
  }
  *s = *t;
}


/***************************************************************************************/
/* LONGWERT str2l(char *s, char **e)                                                   */
/*                 char *s : Zeichenkette, die nach LONGWERT konvertiert werden soll   */
/*                 char **e: Zeigt anschliessend auf das erste Zeichen nach der Zahl   */
/*                 return  : LONGWERT Wert, der der Zahl aus s entspricht.             */
/***************************************************************************************/
LONGWERT str2l(char *s, char **e)
{ char *p;
  LONGWERT ret;
  int vzflag;

  p = s;
  ret = 0;
  if( *p == '-' )
  { vzflag = 1;
    p++;
  }
  else
    vzflag = 0;
  while( isdigit(*p) )
    ret = 10*ret + (*p++ - '0');
  *e = p;
  if( vzflag )
    return -ret;
  else
    return ret;
}


/***************************************************************************************/
/* int test_double(char *s)                                                            */
/*                 char *s: String, der auf einen Double Wert getestet werden soll     */
/***************************************************************************************/
int test_double(char *s)
{ int pa, ea;

  LOG(5, "test_double, s: %.200s.\n", s);

  if( !s )
  { LOG(5, "/test_double - false (NULL)\n");
    return false;
  }

  if( *s == '-' )
    s++;
  if( !isdigit(*s++) )
    return false;
  pa = ea = 0;

#ifdef WITH_GETTEXT
  while( isdigit(*s) || *s == *dezimalpunkt || *s == 'e' || *s == 'E' )
  { if( *s == *dezimalpunkt )
    { pa++;
      s++;
    }
    else if( *s == 'e' || *s == 'E' )
    { ea++;
      s++;
      if( *s == '+' || *s == '-' )
        s++;
    }
    else
      s++;
  }
#else
  while( isdigit(*s) || *s == '.' || *s == 'e' || *s == 'E' )
  { if( *s == '.' )
    { pa++;
      s++;
    }
    else if( *s == 'e' || *s == 'E' )
    { ea++;
      s++;
      if( *s == '+' || *s == '-' )
        s++;
    }
    else
      s++;
  }
#endif
  skip_blanks(s);
  LOG(5, "/test_double\n");
  return !*s && pa <= 1 && ea <= 1;
}


/***************************************************************************************/
/* int test_long(char *s)                                                              */
/*               char *s: String, der auf einen Long Wert getestet werden soll         */
/***************************************************************************************/
int test_long(char *s)
{ char *ss;

  LOG(5, "test_long, s: %.200s.\n", s);

  if( !s )
  { LOG(5, "/test_long - false (NULL)\n");
    return false;
  }

  if( *s == '-' )
    s++;
  ss = s;
  if( !isdigit(*ss++) )
    return false;
  while( isdigit(*ss) )
    ss++;
  if( ss - s >= NUM_DIGITS_TEST_INT 
      && atof(s) > MAX_INT_VALUE )
    return false;
  skip_blanks(ss);
  LOG(5, "/test_long\n");
  return !*ss;
}


/***************************************************************************************/
/* int test_double_wert(wert w)                                                        */
/*               wert w: Wert, der auf einen Double Wert getestet werden soll          */
/***************************************************************************************/
int test_double_wert(wert w)
{
  LOG(5, "test_double_wert, s: %.200s.\n", wert2string(w));

  if( w.type == LONGW || w.type == DOUBLEW )
    return true;
  else if( w.type == STRINGW )
    return test_double(w.s);
  else
    return false;
}


/***************************************************************************************/
/* int test_long_wert(wert w)                                                          */
/*               wert w: Wert, der auf einen Long Wert getestet werden soll            */
/***************************************************************************************/
int test_long_wert(wert w)
{
  LOG(5, "test_long_wert, s: %.200s.\n", wert2string(w));

  if( w.type == LONGW || w.type == DOUBLEW )
    return true;
  else if( w.type == STRINGW )
    return test_long(w.s);
  else
    return false;
}


/***************************************************************************************/
/* wtype test_wtype(char *s)                                                           */
/*                  char *s: Wert, der getestet werden soll                            */
/*       test_wtype bestimmt den Typ des Wertes (long, double oder string)             */
/***************************************************************************************/
wtype test_wtype(char *s)
{ if( !*s )
    return EMPTYW;
  if( test_long(s) )
    return LONGW;
  if( test_double(s) )
    return DOUBLEW;
  return STRINGW;
}


/***************************************************************************************/
/* double getdouble(char **s)                                                          */
/*                  char **s: Position der Zahl                                        */
/*                  return  : Wert im Double-Format, auf den *s Zeigt                  */
/*        getdouble wandelt Zeichenkette aus *s nach Double-Zahl um                    */
/***************************************************************************************/
double getdouble(char **s)
{ char *e;
  double ret;

  ret = strtod(*s, &e);
  if( *s == e )
    *s = e+1;
  else
    *s = e;
  return ret;
}


/***************************************************************************************/
/* LONGWERT getlong(char **s)                                                          */
/*                  char **s: Position der Zahl                                        */
/*                  return  : Wert als long, auf den *s Zeigt                          */
/*          getlong wandelt Zeichenkette aus *s in Long-Zahl um                        */
/***************************************************************************************/
LONGWERT getlong(char **s)
{ char *e;
  LONGWERT ret;

  ret = str2l(*s, &e);
  if( *s == e )
    *s = e+1;
  else
    *s = e;
  return ret;
}
#endif  /* #if defined(WEBSERVER) || defined(INTERPRETER) */


/***************************************************************************************/
/* void strcpyn(char *out, char *in, long n)                                           */
/*              char *out: Ausgabezeile                                                */
/*              char *in : Eingabezeile                                                */
/*              long n   : Maximal n Zeichen                                           */
/*      strcpyn kopiert in nach out                                                    */
/***************************************************************************************/
void strcpyn(char *out, char *in, long n)
{ long i;

  i = 1;
  while( *in && i++ < n )
    *out++ = *in++;
  *out = '\0';
}


/***************************************************************************************/
/* void strcatn(char *out, char *in, long n)                                           */
/*              char *out: Ausgabezeile                                                */
/*              char *in : Eingabezeile                                                */
/*              long n   : Maximal n Zeichen                                           */
/*      strcatn haent in an out an                                                     */
/***************************************************************************************/
void strcatn(char *out, char *in, long n)
{ long i;

  i = strlen(out);
  out += i++;
  while( *in && i++ < n )
    *out++ = *in++;
  *out = '\0';
}


/***************************************************************************************/
/* void strcatnc(char *out, char *in, char endc, long n)                               */
/*              char *out: Ausgabezeile                                                */
/*              char *in : Eingabezeile                                                */
/*              char endc: nur bis endc (ohne endc) kopieren                           */
/*              long n   : Maximal n Zeichen                                           */
/*      strcatn haent in an out an                                                     */
/***************************************************************************************/
void strcatnc(char *out, char *in, char endc, long n)
{ long i;

  i = strlen(out);
  out += i++;
  while( *in && *in != endc && i++ < n )
    *out++ = *in++;
  *out = '\0';
}


/***************************************************************************************/
/* void strcpyn_l(char *out, char **in, long n)                                        */
/*                char *out: Ausgabezeile                                              */
/*                char **in: Eingabezeile                                              */
/*                long n   : Maximal n Zeichen                                         */
/*      strcpyn_l kopiert in nach out bis ein Leerzeichen, Tab oder \n gefunden wird   */
/***************************************************************************************/
void strcpyn_l(char *out, char **in, long n)
{ long i;

  i = 1;
  skip_blanks(*in);
  while( **in && **in != ' ' && **in != '\t' && **in != '\n' && **in != '\r' && i++ < n )
    *out++ = *(*in)++;
  *out = '\0';
}


/***************************************************************************************/
/* void strcpyn_d(char *out, char **in, long n)                                        */
/*                char *out: Ausgabezeile                                              */
/*                char **in: Eingabezeile                                              */
/*                long n   : Maximal n Zeichen                                         */
/*      strcpyn_d kopiert alle Digits aus in nach out                                  */
/***************************************************************************************/
void strcpyn_d(char *out, char **in, long n)
{ long i;

  i = 1;
  skip_blanks(*in);
  while( isdigit(**in) && i++ < n )
    *out++ = *(*in)++;
  *out = '\0';
}
