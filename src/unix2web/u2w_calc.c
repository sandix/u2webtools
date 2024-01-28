/*******************************************/
/* File: u2w_calc.c                        */
/* Funktionen zur Berechnung von Formeln   */
/* timestamp: 2017-02-12 20:52:08          */
/*******************************************/


#include "u2w.h"

#ifndef COMPILER

#include <math.h>
#ifdef WITH_REGEX
#include <regex.h>
#endif

#define MAX_LEN_OP 4                           /* längste Operator ist 4 Zeichen lang  */
/* Array mit den operatoren: Zeichen, Code, Priorität */
unaryoperatortype unaryops[] = {
  {'-',    T_UMINUS,              1},   /* d                                      */
  {'~',    T_NEG,                 1},   /* b                                      */
  {'!',    T_NOT,                 1},   /* n                                      */
  {'(',    T_KLAMMERAUF,         11},   /* (                                      */
  {'\0',   '\0',                  0}
};

                                        //   C: calc, S: string
operatortype operatoren[] = {
  {"+",    T_PLUS,                4},   /* + C                                      */
  {"-",    T_MINUS,               4},   /* - C                                      */
  {"*",    T_STERN,               3},   /* * C                                      */
  {"/",    T_GETEILT,             3},   /* / C                                      */
  {"%",    T_PROZENT,             3},   /* % C                                      */
  {"=",    T_GLEICH,              5},   /* =                                        */
  {"=|",   T_GLEICHABS,           5},   /* I                                        */
  {"==",   T_GLEICHGLEICH,        5},   /* G S  erzw. Stringvergleich               */
  {"==|",  T_GLEICHGLEICHICASE,   5},   /* H S  Stringvergleich ignore case         */
  {"!=",   T_UNGELICH,            5},   /* !                                        */
  {"!==",  T_UNGLEICHGLEICH,      5},   /* U S  erzw. Stringvergleich               */
  {"!=|",  T_UNGLEICHABS,         5},   /* J                                        */
  {"!==|", T_UNGLEICHGLEICHICASE, 5},   /* K S                                      */
  {"<",    T_KLEINER,             5},   /* <    muss vor <= stehen                  */
  {">",    T_GROESSER,            5},   /* >    muss vor >= stehen                  */
  {"<=",   T_KLEINERGLEICH,       5},   /* l                                        */
  {">=",   T_GROESSERGLEICH,      5},   /* g                                        */
  {"<=|",  T_KLEINERGLEICHABS,    5},   /* h                                        */
  {"<|",   T_KLEINERABS,          5},   /* i                                        */
  {">=|",  T_GROESSERGLEICHABS,   5},   /* j                                        */
  {">|",   T_GROESSERABS,         5},   /* k                                        */
#ifdef WITH_REGEX
  {"~",    T_MATCH,               5},   /* ~ S                                      */
  {"!~",   T_NOTMATCH,            5},   /* M                                        */
  {"~|",   T_MATCHICASE,          5},   /* m                                        */
  {"!~|",  T_NOTMATCHICASE,       5},   /* N                                        */
#endif
  {"~~",   T_SUBSTRING,           5},   /* s S  substring a ~~ b -> b in a          */
  {"!~~",  T_NOTSUBSTRING,        5},   /* S S  !substring                          */
  {"~~|",  T_SUBSTRINGICASE,      5},   /* t S  ignore case substr a ~~ b -> b in a */
  {"!~~|", T_NOTSUBSTRINGICASE,   5},   /* T S  !ignore case substring              */
  {"<<",   T_LINKSSHIFT,          3},   /* L C                                      */
  {">>",   T_RECHTSSHIFT,         3},   /* R C                                      */
  {".",    T_KONKAT,              4},   /* c S  String Konkatenation                */
  {"&",    T_UND,                 3},   /* & C  muss vor && stehen                  */
  {"&&",   T_UNDUND,              7},   /* a                                        */
  {"|",    T_ODER,                4},   /* | C  muss vor || stehen                  */
  {"||",   T_ODERODER,            6},   /* o                                        */
  {"?",    T_FRAGEZEICHEN,       10},   /* ?                                        */
  {":",    T_DOPPELPUNKT,         9},   /* :                                        */
  {")",    T_KLAMMERZU,          11},   /* )                                        */
  {"",     T_UMINUS,              1},   /* d                                        */
  {"",     T_NEG,                 1},   /* b                                        */
  {"",     T_NOT,                 1},   /* n                                        */
  {"",     T_KLAMMERAUF,         11},   /* (                                        */
  {"",     '\0',                  0}
};


#define OPS  "+-*/%=!&|()><?:~"
#define OPS2 "."                                               /* extra, wegen Dez.Zahl*/
#define isop(c) (c && (NULL != strchr(OPS OPS2, c)))           /* erlaubte Zeichen     */
#define isunaryop(c) (c && (NULL != strchr("!(-~", c)))        /* unärer Op?           */
#define is_calc_op(c) (NULL != strchr("+-*/%&|LR", c))         /* Codes                */
#define is_long_op(c) (NULL != strchr("%&|LRbaon", c))         /* Codes für Long Ops   */
#define is_string_op(c) (NULL != strchr("~cGHUsStTK", c))      /* Codes für String Ops */
#define is_unary_op(c) (NULL != strchr("nbd", c))              /* Codes für Unäre Ops  */
#define is_bool_op(c)  (NULL != strchr("aon", c))              /* Codes für Bool Ops   */

char opstack[MAXANZOPS];
int anz_ops;
wert wstack[MAXANZWERTE];
int anz_wstack;
int skipcalc_opstack_pos;     // Position im Operanden Stack des "skip-calc"
                              // erst wenn dieser Operand gepopt wird, wird die
                              // skipcalc_opstack_pos gelöscht.
                              // es gilt dann der Wert des Wertestacks.
                              // wird gesetzt bei: 0 *
                              //                   true ||
                              //                   false &&
                              //                   ? ... : ... im Block, der nicht gilt
                              // Wenn gesetzt, dann werden keine Berechnungen ausgeführt,
                              // es werden nur die Stacks gefüllt und geleert.

/***************************************************************************************/
/* char *getstring(char **s)                                                           */
/*                 char **s: Position des Strings                                      */
/*                 return  : Speicherort des Strings im Stringpuffer                   */
/*       getstring kopiert alle Zeichen aus *s bis ' ' oder Operator gefunden          */
/***************************************************************************************/
char *getstring(char **s)
{ char wert[MAX_ZEILENLAENGE], *w;
  char c;

  LOG(1, "getstring, s: %s.\n", *s);

  w = wert;

  if( **s == '"' )
  { (*s)++;
    if( !scan_to_z(&w, s, MAX_ZEILENLAENGE, 0, "\"", '\0', QF_NONE) )
    { if( **s == '"' )
      { (*s)++;
        return store_str(wert);
      }
    }
    return "";
  }
  else if( **s == '\'' )                          /* String in "'" oder '"'?           */
    c = *(*s)++;                                  /* Begrenzung merken                 */
  else
  { c = '\0';                                     /* keine Begrenzung                  */
    *w++ = *(*s)++;                               /* aber mindestens ein Zeichen lesen */
  }

  while( **s && ((c && **s != c)
                 || (!c && **s != ' ' && !isop(**s))) )
    *w++ = *(*s)++;
  *w = '\0';
  if( c && **s == c )
    (*s)++;
  return store_str(wert);
}


#ifdef WITH_REGEX
/***************************************************************************************/
/* int match(char *s, char *r, int icase_flag)                                         */
/*           char *s: String, der verglichen werden soll                               */
/*           char *r: Reg.Expression, mit der verglichen wird                          */
/*           int icase_flag: true, CASE wird ignoriert                                 */
/*           return: true, r matcht s                                                  */
/***************************************************************************************/
int match(char *s, char *r, int icase_flag)
{ regex_t re;
  int ret;

  LOG(3, "match, s: %s, r: %s.\n", s, r);

  if( icase_flag )
  { if( 0 != regcomp(&re, r, REG_ICASE|REG_EXTENDED|REG_NOSUB) )
      return false;
  }
  else
  { if( 0 != regcomp(&re, r, REG_EXTENDED|REG_NOSUB) )
      return false;
  }
  ret = !regexec(&re, s, (size_t) 0, NULL, 0);
  regfree(&re);
  LOG(5, "/match, ret: %d.\n", ret);
  return ret;
}
#endif


/***************************************************************************************/
/* void skip(char **s)                                                                 */
/*           char **s: Position des Strings                                            */
/*      skip überliest einen Wert                                                      */
/***************************************************************************************/
void skip(char **s)
{ char c;

  if( **s == '"' || **s == '\'' )
    c = *(*s)++;
  else
    c = '\0';

  while( **s && ((c && **s != c)
                 || (!c && **s != ' ' && !isop(**s))) )
    (*s)++;

  if( c && **s == c )
    (*s)++;
}


/***************************************************************************************/
/* int pushwert(wert w)                                                                */
/*              wert w: Wert                                                           */
/*              return : true bei Fehler Stack voll                                    */
/*     pushwert legt einen Wert auf den Operandenstack                                 */
/***************************************************************************************/
int pushwert(wert w)
{
  LOG(1, "pushwert.\n");

  if( anz_wstack < MAXANZWERTE )
  { wstack[anz_wstack++] = w;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int pushstring(char *s)                                                             */
/*                char *s: Zeiger auf String                                           */
/*                return : true bei Fehler Stack voll                                  */
/*     pushstring legt einen String-Wert auf den Operandenstack                        */
/***************************************************************************************/
int pushstring(char *s)
{
  LOG(1, "pushstring, s: %s.\n", s);

  if( anz_wstack < MAXANZWERTE )
  { wstack[anz_wstack].type = STRINGW;
    wstack[anz_wstack++].s = s;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int pushsdouble(double d)                                                           */
/*                 double d: Double Wert                                               */
/*                 return : true bei Fehler Stack voll                                 */
/*     pushdouble legt einen Double-Wert auf den Operandenstack                        */
/***************************************************************************************/
int pushdouble(double d)
{
  LOG(1, "pushdouble, d: %f.\n", d);

  if( anz_wstack < MAXANZWERTE )
  { wstack[anz_wstack].type = DOUBLEW;
    wstack[anz_wstack++].w.d = d;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int pushnonew(void)                                                                 */
/*                 return : true bei Fehler Stack voll                                 */
/*     pushnonew legt einen ungültigen-Wert auf den Operandenstack                     */
/***************************************************************************************/
int pushnonew(void)
{
  LOG(1, "pushnonew.\n");

  if( anz_wstack < MAXANZWERTE )
  { wstack[anz_wstack++].type = NONEW;
    return false;
  }
  else
    return true;
}


#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
// verhindert: warning: ‘...’ may be used uninitialized in this function [-Wmaybe-uninitialized]
#endif
#endif
/***************************************************************************************/
/* int pushlong(LONGWERT l)                                                            */
/*              LONGWERT l: Long Wert                                                  */
/*              return : true bei Fehler Stack voll                                    */
/*     pushlong legt einen Long-Wert auf den Operandenstack                            */
/***************************************************************************************/
int pushlong(LONGWERT l)
{
  LOG(1, "pushlong, l: " LONGFORMAT ".\n", l);

  if( anz_wstack < MAXANZWERTE )
  { wstack[anz_wstack].type = LONGW;
    wstack[anz_wstack++].w.l = l;
    return false;
  }
  else
    return true;
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif


/***************************************************************************************/
/* double popdouble(void)                                                              */
/*                  return: Double-Wert vom Operandenstack                             */
/*        popdouble holt einen Double-Wert vom Operandenstack                          */
/***************************************************************************************/
double popdouble(void)
{ if( anz_wstack )
    return wert2double(wstack[--anz_wstack]);
  else
    return 0.0;
}


/***************************************************************************************/
/* LONGWERT poplong(void)                                                              */
/*              return: Long-Wert vom Operandenstack                                   */
/*      poplong holt einen Long-Wert vom Operandenstack                                */
/***************************************************************************************/
LONGWERT poplong(void)
{ if( anz_wstack )
    return wert2long(wstack[--anz_wstack]);
  else
    return 0;
}


/***************************************************************************************/
/* LONGWERT popbool(void)                                                              */
/*              return: Long-Wert vom Operandenstack                                   */
/*      popbool holt einen Bool-Wert vom Operandenstack                                */
/***************************************************************************************/
LONGWERT popbool(void)
{ if( anz_wstack )
    return wert2bool(wstack[--anz_wstack]);
  else
    return 0;
}


/***************************************************************************************/
/* char *popstring(void)                                                               */
/*                 return: String-Wert vom Operandenstack                              */
/*       popstromg holt einen String-Wert vom Operandenstack                           */
/***************************************************************************************/
char *popstring(void)
{ if( anz_wstack )
    return wert2string(wstack[--anz_wstack]);
  else
    return "";
}


/***************************************************************************************/
/* wtype nextwtype(void)                                                               */
/*                 return: type des obersten Wertes auf dem Operandenstack             */
/*       nextwtype bestimmt den Typ des obersten Wertes des Operandenstacks            */
/***************************************************************************************/
wtype nextwtype(void)
{ if( anz_wstack )
    return wstack[anz_wstack-1].type;
  else
    return EMPTYW;
}


/***************************************************************************************/
/* wtype nextnextwtype(void)                                                           */
/*                 return: type des zweitobersten Wertes auf dem Operandenstack        */
/*       nextnextwtype bestimmt den Typ des zweitobersten Wertes des Operandenstacks   */
/***************************************************************************************/
wtype nextnextwtype(void)
{ if( anz_wstack >= 2 )
    return wstack[anz_wstack-2].type;
  else
    return EMPTYW;
}


/***************************************************************************************/
/* int getprio(char o)                                                                 */
/*             char o: opcode des Operators                                            */
/*             return: Prioritaet des Operators                                        */
/*     getprio bestimmt die Prioritaet des Operator mit dem Opcode o                   */
/***************************************************************************************/
int getprio(char o)
{ int i;
  for( i = 0; operatoren[i].c && operatoren[i].c != o; i++ ); 
  return operatoren[i].p;
}


/***************************************************************************************/
/* char popop(void)                                                                    */
/*            return: Opcode des Operators                                             */
/*      popop holt den obersten Opcode vom Operatorenstack                             */
/***************************************************************************************/
char popop(void)
{
  if( anz_ops )
    return opstack[--anz_ops];
  else
    return '\0';
}


/***************************************************************************************/
/* char testop(char *op)                                                               */
/*             char *op: String mit Operator                                           */
/*             return  : 0, wenn op kein Operator, sonst Opcode                        */
/*      testop bestimmt den Opcode von op, oder liefert 0, wenn kein Operator          */
/***************************************************************************************/
char testop(char *op)
{ int i;

  for( i = 0; operatoren[i].o[0] && strncmp(operatoren[i].o, op, strlen(op)) != 0; i++ );
  return operatoren[i].o[0] ? operatoren[i].c : '\0';
}


/***************************************************************************************/
/* int pushop(char **s, int *op_flag)                                                  */
/*            char **s: Zeiger auf String mit Operator                                 */
/*            int *op_flag: wird auf true gesetzt, wenn ein Operator folgen darf       */
/*            return  : true, wenn kein Operator gefunden oder Operatorenstack voll    */
/***************************************************************************************/
int pushop(char **s, int *op_flag)
{ char opstr[MAX_LEN_OP+2];                      /* +1 zum Testen, +1 für '\0'         */
  char *o;
  char op;

  LOG(1, "pushop, s: %s, opstack: %c.\n", *s, anz_ops ? opstack[anz_ops-1] : '#');

  o = opstr;
  do                                              /* lesen, bis Operator komplett      */
  { if( !(*o++ = *(*s)++) )                       /* nächstes Zeichen anhängen         */
      break;                                      /* Stringende, dann Schluss          */
    *o = '\0';
  } while( testop(opstr) );                       /* ist opstr noch Operator?          */
  (*s)--;                                         /* nicht mehr, Zeichen zurück        */
  *(o-1) = '\0';                                  /* Ende Operator setzen              */
  if( !(op = testop(opstr)) )                     /* ungültiger Operator?              */
    return true;                                  /* Fehler                            */

  *op_flag = false;

  if( op == ')' )                                 /* Klammer zu?                       */
  { while( anz_ops && opstack[anz_ops-1] != '(' ) /* Stack bis '(' abarbeiten          */
      if( calc() )
        break;
    if( anz_ops && opstack[anz_ops-1] == '(' )
      popop();
    *op_flag = true;
    return false;
  }
  else
  { while( anz_ops && getprio(op) >= getprio(opstack[anz_ops-1]) )
      if( calc() )
        break;
  }

  if( op == T_DOPPELPUNKT )
  { if( skipcalc_opstack_pos )
    { LOG(19, "pushop, anz_ops: %d, skipcalc_opstack_pos: %d, topop: %c.\n",
          anz_ops, skipcalc_opstack_pos, opstack[skipcalc_opstack_pos-1]);
      if( opstack[skipcalc_opstack_pos-1] == T_FRAGEZEICHEN && skipcalc_opstack_pos == anz_ops )
      { skipcalc_opstack_pos = 0;
        anz_ops--;
      }
    }
    else if( anz_ops && opstack[anz_ops-1] == T_FRAGEZEICHEN )
    { skipcalc_opstack_pos = anz_ops;
    }
    else
      logging(_("Error in expression - ':' without '?'\n"));
    return false;
  }


  if( anz_ops < MAXANZOPS )
  { opstack[anz_ops++] = op;                      // Operanden auf Stack
    if( ! skipcalc_opstack_pos && anz_wstack )
    { if( op == T_STERN )
      { if( !wert2long(wstack[anz_wstack-1]) )
          skipcalc_opstack_pos = anz_ops;
      }
      else if( op == T_UNDUND )
      { if( !wert2bool(wstack[anz_wstack-1]) )
          skipcalc_opstack_pos = anz_ops;
      }
      else if( op == T_ODERODER )
      { if( wert2bool(wstack[anz_wstack-1]) )
          skipcalc_opstack_pos = anz_ops;
      }
      else if( op == T_FRAGEZEICHEN )
      { if( ! poplong() )
          skipcalc_opstack_pos = anz_ops;
      }
    }
    LOG(11, "pushop, skipcalc_opstack_pos: %d.\n", skipcalc_opstack_pos);
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int pushunaryop(char **s)                                                           */
/*            char **s: Zeiger auf String mit Operator                                 */
/*            return  : true, wenn kein Operator gefunden oder Operatorenstack voll    */
/***************************************************************************************/
int pushunaryop(char **s)
{ int i;

  LOG(1, "pushunaryop, s: %s, opstack: %c.\n", *s, anz_ops ? opstack[anz_ops-1] : ' ');

  if( anz_ops < MAXANZOPS )
  { for( i = 0; unaryops[i].o; i++ )
    { if( unaryops[i].o == **s )
      { opstack[anz_ops++] = unaryops[i].c;
        (*s)++;
        return false;
      }
    }
  }
  return true;
}


/***************************************************************************************/
/* char *double2string(double d)                                                       */
/*                     double d: Wert, der in String umgewandelt werden soll           */
/*                     return  : Pointer auf String                                    */
/*       double2string Wandelt Double-Wert in Zeichenkette um                          */
/***************************************************************************************/
char *double2string(double d)
{ char wert[128];
  char *p, *q;

  LOG(6, "double2string, d: %f.\n", d);

  snprintf(wert, 128, "%f", d);
#ifdef WITH_GETTEXT
  if( NULL != (p = strchr(wert, *dezimalpunkt)) )
#else
  if( NULL != (p = strchr(wert, '.')) )
#endif
  { q = p + strlen(p) - 1;
    while( *q == '0' && q-p > 1 )
      *q-- = '\0';
  }
  return store_str(wert);
}


/***************************************************************************************/
/* char *long2string(LONGWERT l)                                                       */
/*                   LONGWERT l: Wert, der in String umgewandelt werden soll           */
/*                   return: Pointer auf String                                        */
/*       long2string Wandelt Long-Wert in Zeichenkette um                              */
/***************************************************************************************/
char *long2string(LONGWERT l)
{ char wert[128];

  LOG(6, "long2string, l: " LONGFORMAT  ".\n", l);

  snprintf(wert, 128, LONGFORMAT, l);
  return store_str(wert);
}


/***************************************************************************************/
/* char *wert2string(wert w)                                                           */
/*                   wert w: Wert, der in String umgewandelt werden soll               */
/*                   return  : Pointer auf String                                      */
/*       wert2string Wandelt Wert in Zeichenkette um                                   */
/***************************************************************************************/
char *wert2string(wert w)
{ 
  switch( w.type )
  { case BINARYW:
    case STRINGW: LOG(21, "wert2string, w.s: %.30s.\n", w.s);
                  return w.s;
                  break;
    case DOUBLEW: return double2string(w.w.d);
                  break;
    case LONGW:   return long2string(w.w.l);
                  break;
    default:      return "";
  }
}


/***************************************************************************************/
/* LONGWERT wert2long(wert w)                                                          */
/*                    wert w: Wert, der in long umgewandelt werden soll                */
/*                    return  : long-Wert                                              */
/*          wert2long Wandelt Wert in long um                                          */
/***************************************************************************************/
LONGWERT wert2long(wert w)
{
  switch( w.type )
  { case STRINGW: return getlong((char **)&w.s);
                  break;
    case DOUBLEW: return w.w.d;
                  break;
    case LONGW:   return w.w.l;
                  break;
    default:      return 0;
  }
}


/***************************************************************************************/
/* LONGWERT wert2bool(wert w)                                                          */
/*                    wert w: Wert, der in long umgewandelt werden soll                */
/*                    return  : long-Wert                                              */
/*          wert2bool Wandelt Wert in bool(long) um                                    */
/***************************************************************************************/
LONGWERT wert2bool(wert w)
{
  switch( w.type )
  { case STRINGW: return *(char *)w.s != '\0';
                  break;
    case DOUBLEW: return w.w.d != 0;
                  break;
    case LONGW:   return w.w.l != 0;
                  break;
    default:      return 0;
  }
}


/***************************************************************************************/
/* double wert2double(wert w)                                                          */
/*                   wert w: Wert, der in double umgewandelt werden soll               */
/*                   return  : double-Wert                                             */
/*       wert2double Wandelt Wert in double um                                         */
/***************************************************************************************/
double wert2double(wert w)
{
  switch( w.type )
  { case STRINGW: return getdouble((char **)&w.s);
                  break;
    case DOUBLEW: return w.w.d;
                  break;
    case LONGW:   return w.w.l;
                  break;
    default:      return 0.0;
  }
}


#define MAX_LEN_ERGEBNIS MAX_ZEILENLAENGE

/***************************************************************************************/
/* int calc_string(char op)                                                            */
/*                 char op : Opcode des Operators                                      */
/*                 return  : true, wenn op nicht moeglich                              */
/*     calc_string berechnet sl op sr und legt Ergebnis auf den Stack                  */
/***************************************************************************************/
int calc_string(char op)
{ char ergebnis[MAX_LEN_ERGEBNIS];
  char *sl, *sr;
  char slt[MAX_ZEILENLAENGE], srt[MAX_ZEILENLAENGE];

  sr = popstring();
  sl = popstring();

  LOG(1, "calc_string, op: %c, sl: %s, sr: %s.\n", op, sl, sr);

  switch(op)
  { case T_KONKAT:            snprintf(ergebnis, MAX_LEN_ERGEBNIS, "%s%s", sl, sr);
                              pushstring(store_str(ergebnis));
                              break;
    case T_KLEINER:           pushlong(strcmp(sl, sr) < 0);
                              break;
    case T_KLEINERGLEICH:     pushlong(strcmp(sl, sr) <= 0);
                              break;
    case T_GROESSER:          pushlong(strcmp(sl, sr) > 0);
                              break;
    case T_GROESSERGLEICH:    pushlong(strcmp(sl, sr) >= 0);
                              break;
    case T_GLEICH:
    case T_GLEICHGLEICH:      pushlong(0 == strcmp(sl, sr));
                              break;
    case T_UNGELICH:
    case T_UNGLEICHGLEICH:    pushlong(0 != strcmp(sl, sr));
                              break;
    case T_SUBSTRING:         pushlong(NULL != strstr(sl, sr));
                              break;
    case T_NOTSUBSTRING:      pushlong(NULL == strstr(sl, sr));
                              break;
    case T_SUBSTRINGICASE:    strtolower(slt, sl, 10240);
                              strtolower(srt, sr, 10240);
                              pushlong(NULL != strstr(slt, srt));
                              break;
    case T_NOTSUBSTRINGICASE: strtolower(slt, sl, 10240);
                              strtolower(srt, sr, 10240);
                              pushlong(NULL == strstr(slt, srt));
                              break;
    case T_KLEINERGLEICHABS:  pushlong(strcasecmp(sl, sr) <= 0);
                              break;
    case T_KLEINERABS:        pushlong(strcasecmp(sl, sr) < 0);
                              break;
    case T_GROESSERGLEICHABS: pushlong(strcasecmp(sl, sr) >= 0);
                              break;
    case T_GROESSERABS:       pushlong(strcasecmp(sl, sr) > 0);
                              break;
    case T_GLEICHGLEICHICASE:
    case T_GLEICHABS:         pushlong(strcasecmp(sl, sr) == 0);
                              break;
    case T_UNGLEICHGLEICHICASE:
    case T_UNGLEICHABS:       pushlong(strcasecmp(sl, sr) != 0);
                              break;
#ifdef WITH_REGEX
    case T_MATCH:             pushlong(match(sl, sr, false));
                              break;
    case T_NOTMATCH:          pushlong(!match(sl, sr, false));
                              break;
    case T_MATCHICASE:        pushlong(match(sl, sr, true));
                              break;
    case T_NOTMATCHICASE:     pushlong(!match(sl, sr, true));
                              break;
#endif
    default :                 pushstring("");
                              return true;
  }
  return false;
}


#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
// verhindert: warning: ‘dl’ may be used uninitialized in this function [-Wmaybe-uninitialized]
#endif
#endif
/***************************************************************************************/
/* int calc_double(char op)                                                            */
/*                 char op  : Opcode des Operators                                     */
/*                 return   : true, wenn op nicht moeglich                             */
/*     calc_double berechnet dl op dr und legt Ergebnis auf den Stack                  */
/***************************************************************************************/
int calc_double(char op)
{ double dl, dr;

  if( !anz_wstack )
    return true;
  if( wstack[anz_wstack-1].type == NONEW )
  { anz_wstack--;
    if( !is_unary_op(op) )
      dl = popdouble();
    pushnonew();
    return false;
  }
  dr = popdouble();
  if( !is_unary_op(op) )
  { if( !anz_wstack )
      return true;
    if( wstack[anz_wstack-1].type == NONEW )
    { anz_wstack--;
      pushnonew();
      return false;
    }
    dl = popdouble();
  }
#ifdef DEBUG
  else
    dl = 0;
  LOG(1, "calc_double, op: %c, dl: %f, dr: %f.\n", op, dl, dr);
#endif

  switch(op)
  { case T_NOT:               pushlong(!dr);
                              break;
    case T_UMINUS:            pushdouble(-dr);
                              break;
    case T_PLUS:              pushdouble(dl + dr);
                              break;
    case T_MINUS:             pushdouble(dl - dr);
                              break;
    case T_STERN:             pushdouble(dl * dr);
                              break;
    case T_GETEILT:           if( dr == 0 )
                              { pushnonew();
                                logging("error: Divide by zero.\n");
                              }
                              else
                                pushdouble(dl/dr);
                              break;
    case T_KLEINER:           pushlong(dl < dr);
                              break;
    case T_GROESSER:          pushlong(dl > dr);
                              break;
    case T_KLEINERGLEICH:     pushlong(dl <= dr);
                              break;
    case T_GROESSERGLEICH:    pushlong(dl >= dr);
                              break;
    case T_GLEICH:            pushlong(dl == dr);
                              break;
    case T_UNGELICH:          pushlong(dl != dr);
                              break;
    case T_KLEINERGLEICHABS:  pushlong(fabs(dl) <= fabs(dr));
                              break;
    case T_KLEINERABS:        pushlong(fabs(dl) < fabs(dr));
                              break;
    case T_GROESSERGLEICHABS: pushlong(fabs(dl) >= fabs(dr));
                              break;
    case T_GROESSERABS:       pushlong(fabs(dl) > fabs(dr));
                              break;
    case T_GLEICHABS:         pushlong(fabs(dl) == fabs(dr));
                              break;
    case T_UNGLEICHABS:       pushlong(fabs(dl) != fabs(dr));
                              break;
    default :                 pushlong(0);
                              return true;
  }
  return false;
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif


#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
// verhindert: warning: ‘ll’ may be used uninitialized in this function [-Wmaybe-uninitialized]
#endif
#endif
/***************************************************************************************/
/* int calc_long(char op)                                                              */
/*               char op: Opcode des Operators                                         */
/*               return : true, wenn op nicht moeglich                                 */
/*     calc_double berechnet dl op dr und legt Ergebnis auf den Stack                  */
/***************************************************************************************/
int calc_long(char op)
{ LONGWERT ll, lr;

  LOG(1, "calc_long, op: %c.\n", op);

  if( !anz_wstack )
    return true;
  if( wstack[anz_wstack-1].type == NONEW )
  { anz_wstack--;
    if( !is_unary_op(op) )
      ll = poplong();
    pushnonew();
    return false;
  }

  if( is_bool_op(op) )
  { lr = popbool();
    if( !is_unary_op(op) )
    { if( !anz_wstack )
        return true;
      if( wstack[anz_wstack-1].type == NONEW )
      { anz_wstack--;
        pushnonew();
        return false;
      }
      ll = popbool();
    }
#ifdef DEBUG
    else
      ll = 0;
#endif
  }
  else
  { lr = poplong();
    if( !is_unary_op(op) )
    { if( !anz_wstack )
        return true;
      if( wstack[anz_wstack-1].type == NONEW )
      { anz_wstack--;
        pushnonew();
        return false;
      }
      ll = poplong();
    }
#ifdef DEBUG
    else
      ll = 0;
#endif
  }

  LOG(1, "calc_long, ll: " LONGFORMAT ", lr: " LONGFORMAT ".\n", ll, lr);

  switch(op)
  { case T_NOT:               pushlong(!lr);
                              break;
    case T_UMINUS:            pushlong(-lr);
                              break;
    case T_NEG:               pushlong(~lr);
                              break;
    case T_PLUS:              pushlong(ll + lr);
                              break;
    case T_MINUS:             pushlong(ll - lr);
                              break;
    case T_STERN:             pushlong(ll * lr);
                              break;
    case T_PROZENT:           if( lr == 0 )
                                pushlong(0);
                              else
                                pushlong(ll % lr);
                              break;
    case T_GETEILT:           if( lr == 0 )
                              { pushnonew();
                                logging("error: Divide by zero.\n");
                              }
                              else
                                pushlong(ll / lr);
                              break;
    case T_GLEICH:            pushlong(ll == lr);
                              break;
    case T_KLEINER:           pushlong(ll < lr);
                              break;
    case T_GROESSER:          pushlong(ll > lr);
                              break;
    case T_KLEINERGLEICH:     pushlong(ll <= lr);
                              break;
    case T_GROESSERGLEICH:    pushlong(ll >= lr);
                              break;
    case T_KLEINERGLEICHABS:  pushlong(ABS(ll) <= ABS(lr));
                              break;
    case T_KLEINERABS:        pushlong(ABS(ll) < ABS(lr));
                              break;
    case T_GROESSERGLEICHABS: pushlong(ABS(ll) >= ABS(lr));
                              break;
    case T_GROESSERABS:       pushlong(ABS(ll) > ABS(lr));
                              break;
    case T_GLEICHABS:         pushlong(ABS(ll) == ABS(lr));
                              break;
    case T_UNGLEICHABS:       pushlong(ABS(ll) != ABS(lr));
                              break;
    case T_UNDUND:            pushlong(ll && lr);
                              break;
    case T_ODERODER:          pushlong(ll || lr);
                              break;
    case T_UNGELICH:          pushlong(ll != lr);
                              break;
    case T_ODER:              pushlong(ll | lr);
                              break;
    case T_UND:               pushlong(ll & lr);
                              break;
    case T_LINKSSHIFT:        pushlong(ll << lr);
                              break;
    case T_RECHTSSHIFT:       pushlong(ll >> lr);
                              break;
    default :                 pushlong(0);
                              return true;
  }
  return false;
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif


/***************************************************************************************/
/* int calc(void)                                                                      */
/*          return: true, wenn nichts mehr zu berechnen ist                            */
/*     calc berechnet Ergebnis aus obersten Operanden und oberstem Operator            */
/***************************************************************************************/
int calc(void)
{ char op;
  wtype wl, wr;

  LOG(1, "calc, anz_wstack: %d, skipcalc: %d, top: %s, next: %s.\n", anz_wstack,
      skipcalc_opstack_pos,
      anz_wstack ? wert2string(wstack[anz_wstack-1]) : "",
      anz_wstack >= 2 ? wert2string(wstack[anz_wstack-2]) : "");
  LOG(1, "calc, anz_opstack: %d, top: %c.\n", anz_ops,
      anz_ops ? opstack[anz_ops-1] : '#');

  if( ! anz_ops )
    return true;
  else if( skipcalc_opstack_pos && anz_ops >= skipcalc_opstack_pos )
  { if( skipcalc_opstack_pos == anz_ops )
      skipcalc_opstack_pos = 0;
    popop();
    return false;
  }
  else
  { if( (op = popop()) )
    { if( is_unary_op(op) && anz_wstack >= 1 )
      { LOG(3, "calc, is_unary_op && anz_wstack >= 1\n");
        if( is_long_op(op) || nextwtype() == LONGW )
          calc_long(op);
        else
          calc_double(op);
        return false;
      }
      else if( anz_wstack >= 2 )
      { LOG(3, "calc, anz_wstack >= 2\n");
        wl = nextnextwtype();
        wr = nextwtype();
        LOG(13, "calc wl: %d, wr: %d\n", wl, wr);

        if( is_string_op(op) )
          calc_string(op);
        else if( is_long_op(op) )
        { if( wl == NONEW || wr == NONEW )
            anz_wstack--;
          else
            calc_long(op);
        }
        else
        { LOG(5, "calc, anz_wstack >= 2, else\n");
          if( !is_calc_op(op)
              && (wl == STRINGW || wr == STRINGW) )
            calc_string(op);
          else if( wl == NONEW )
          { LOG(15, "calc, wl == NONEW\n");
            anz_wstack--;
          }
          else if( wr == NONEW )
          { LOG(15, "calc, wr == NONEW\n");
            anz_wstack--;
            wstack[anz_wstack-1].type = NONEW;
          }
          else if( wl == DOUBLEW || wr == DOUBLEW )
            calc_double(op);
          else if( wl == LONGW || wr == LONGW )
            calc_long(op);
          else if( is_calc_op(op) )
          { LOG(7, "calc, anz_wstack >= 2, else, is_calc_op\n");
            if( test_long_wert(wstack[anz_wstack-1])
                && test_long_wert(wstack[anz_wstack-2]) )
            { LOG(9, "calc, anz_wstack >= 2, else, is_calc_op, test_long\n");
              calc_long(op);
            }
            else if( test_double_wert(wstack[anz_wstack-1])
                     || test_double_wert(wstack[anz_wstack-2]) )
            { LOG(9, "calc, anz_wstack >= 2, else, is_calc_op, test_double\n");
              calc_double(op);
            }
            else
            { LOG(9, "calc, anz_wstack >= 2, else, is_calc_op, else\n");
              calc_long(op);
            }
          }
          else
            calc_string(op);
        }
        LOG(2, "/calc false, anz_wstack: %d, top: %s, next: %s.\n", anz_wstack,
            anz_wstack ? wert2string(wstack[anz_wstack-1]) : "",
            anz_wstack >= 2 ? wert2string(wstack[anz_wstack-2]) : "");
        LOG(2, "/calc false, anz_opstack: %d, top: %c.\n", anz_ops,
            anz_ops ? opstack[anz_ops-1] : '#');
        return false;
      }
    }
  }
  LOG(2, "/calc true, anz_wstack: %d, top: %s, next: %s.\n", anz_wstack,
      anz_wstack ? wert2string(wstack[anz_wstack-1]) : "",
      anz_wstack >= 2 ? wert2string(wstack[anz_wstack-2]) : "");
  LOG(2, "/calc true, anz_opstack: %d, top: %c.\n", anz_ops,
      anz_ops ? opstack[anz_ops-1] : '#');
  return true;
}


/***************************************************************************************/
/* int berechne(wert *w, char *f)                                                      */
/*              wert *w: Ergebnis                                                      */
/*              char *f: Formel zur Berechnung                                         */
/*              return : true bei Fehler                                               */
/*     berechne Berechnet das Ergebnis der Formel aus f                                */
/***************************************************************************************/
int berechne(wert *w, char *f)
{ char *b, **pb, *p;
  char parwert[MAX_ZEILENLAENGE];
  int op_flag = false;                      /* Es wird kein Operator erwartet          */
  int parflag;                              /* 0, normal, 1: wwert, 2: lwert, 3: swert */
  char swert[MAX_ZEILENLAENGE];
  wert wwert;
  long lwert;
  int nf;

  LOG(1, "berechne, f: %s.\n", f);

  b = f;
  anz_ops = 0;
  anz_wstack = 0;
  skipcalc_opstack_pos = 0;

  while( *b )
  { while( *b == ' ' || *b == '\n' )
      b++;
    if( !*b )
      break;

    parflag = 0;

    if( *b == ATTRIB_CHAR )
    { b++;
      if( *b == REM_CHAR )
        break;
#ifdef WITH_GETTEXT
      if( *b == GETTEXT )
      { p = swert;
        scan_gettext_z(&p, &b, MAX_ZEILENLAENGE, "", '\0', QF_NONE);
        *p = '\0';
        parflag = 3;
      }
      else if( *b == GETTEXTCONST )
      { p = swert;
        scan_gettextconst_z(&p, &b, MAX_ZEILENLAENGE, "", '\0', QF_NONE);
        *p = '\0';
        parflag = 3;
      }
      else if( *b == GETTEXTFKT )
      { p = swert;
        scan_gettextfkt_z(&p, &b, MAX_ZEILENLAENGE, "", '\0', QF_NONE);
        *p = '\0';
        parflag = 3;
      }
      else
#endif  /* WITH_GETTEXT */
      if( 0 <= (nf = is_x2w_command_z(&b, command_progs)) )
      { if( skipcalc_opstack_pos )
          skip_x2w_command(&b, command_progs[nf]);
        else
        { if( do_x2w_command(&b, command_progs[nf]) )
            lwert = 0;
          else
            lwert = 1;
        }
        parflag = 2;
      }
      else if( 0 <= (nf = is_x2w_list_command_z(&b, command_list_progs)) )
      { if( skipcalc_opstack_pos )
          skip_x2w_list_command(&b, command_list_progs[nf]);
        else
        { if( do_x2w_list_command(&b, command_list_progs[nf]) )
            lwert = 0;
          else
            lwert = 1;
        }
        parflag = 2;
      }
      else if( 0 <= (nf = is_scan_command_z(&b, scan_progs)) )
      { if( skipcalc_opstack_pos )
          skip_scan_command(&b, scan_progs[nf]);
        else
        { p = swert;
          do_scan_command(&p, &b, MAX_ZEILENLAENGE, scan_progs[nf], QF_NONE);
          *p = '\0';
        }
        parflag = 3;
      }
      else if( 0 <= (nf = is_scan_command_z(&b, scan_send_progs)) )
      { if( skipcalc_opstack_pos )
          skip_scan_command(&b, scan_progs[nf]);
        else
        { p = swert;
          do_scan_command(&p, &b, MAX_ZEILENLAENGE, scan_send_progs[nf], QF_NONE);
          *p = '\0';
        }
        parflag = 3;
      }
      else if( 0 <= (nf = is_scan_command_z(&b, scan_send_progs_out)) )
      { if( skipcalc_opstack_pos )
          skip_scan_command(&b, scan_progs[nf]);
        else
        { p = swert;
          do_scan_command(&p, &b, MAX_ZEILENLAENGE, scan_send_progs_out[nf], QF_NONE);
          *p = '\0';
        }
        parflag = 3;
      }
      else
      { b--;                            /* '%' wiederherstellen                        */
        if( op_flag )
        { if( pushop(&b, &op_flag) )
            return false;
          continue;
        }
      }
    }
    else if( *b == PAR_CHAR )
    { wwert = getfirstparwert(&b);
      parflag = 1;
    }
    else if( *b == SHELL_INLINE || *b == SHELL_ESCAPE )
    { if( skipcalc_opstack_pos )
        scan_shell_skip(&b, *(b-1));
      else
      { p = swert;
        b++;
        scan_shell(&p, &b, MAX_ZEILENLAENGE, *(b-1));
        *p = '\0';
      }
      parflag = 3;
    }

    LOG(3, "berechne, parflag: %d.\n", parflag);

    if( parflag == 1 && wwert.type == STRINGW )
    { p = wwert.s;
      pb = &p;
    }
    else if( parflag == 3 )
    { p = swert;
      pb = &p;
    }
    else
      pb = &b;

    LOG(5, "berechne, pb: %s, op_flag: %d.\n", *pb, op_flag);
    if( op_flag )
    { if( pushop(pb, &op_flag) )
        return false;
    }
    else if( !parflag && (isunaryop(**pb) && !(**pb == '-' && isdigit(*(*pb+1))) ) )
    { if( pushunaryop(pb) )
        return false;
    }
    else
    { if( skipcalc_opstack_pos )
      { if( !parflag )
          skip(&b);
      }
      else if( parflag == 1 )
        pushwert(wwert);
      else if( parflag == 2 )
        pushlong(lwert);
      else if( parflag == 3 )
      { p = swert;
        if( test_long(swert) )
        { if( pushlong(getlong(&p)) )
            return false;
        }
        else if( test_double(swert) )
        { if( pushdouble(getdouble(&p)) )
            return false;
        }
        else
        { if( pushstring(store_str(swert)) )
            return false;
        }
      }
      else
      { p = b;
        if( *p == '-' )                           /* '-' überlesen, wird nicht getestet*/
          p++;
        strcpyn_str(parwert, &p, OPS " \t", MAX_ZEILENLAENGE);
        LOG(4, "berechne, parwert: %s.\n", parwert);
        if( test_long(parwert) )
        { if( pushlong(getlong(&b)) )
            return false;
        }
        else if( test_double(parwert) )
        { if( pushdouble(getdouble(&b)) )
            return false;
        }
        else
        { if( pushstring(getstring(&b)) )
            return false;
        }
      }
      op_flag = true;
    }
    LOG(3, "berechne, b: %s, opstack: %c.\n", b, anz_ops ? opstack[anz_ops-1] : ' ');
  }

  while( anz_ops && !calc() )
    ;
  LOG(2, "/berechne, anz_wstack: %d, Wert: %s.\n",
      anz_wstack, wert2string(wstack[0]));
  if( anz_wstack == 1 )
  { *w = wstack[0];
    return false;
  }
  else
  { LOG(1, "/berechne, Fehler in Formel: %s.\n", f);
    return true;
  }
}


/***************************************************************************************/
/* int berechnestr(char **out, char *f, long n)                                        */
/*              char **out: Ergebnis                                                   */
/*              char *f: Formel zur Berechnung                                         */
/*              long n: Platz in out                                                   */
/*              return : true bei Fehler                                               */
/*     berechnestr Berechnet das Ergebnis der Formel aus f, dabei werden keine         */
/*               U2W Funktionen oder Parameter ausgewertet                             */
/***************************************************************************************/
int berechnestr(char **out, char *f, long n)
{ char *b, **pb, *p;
  char parwert[MAX_ZEILENLAENGE];
  int op_flag = false;                      /* Es wird kein Operator erwartet          */
  char swert[MAX_ZEILENLAENGE];
  wert wwert;
  long lwert;
  int nf;

  LOG(1, "berechnestr, f: %s.\n", f);

  b = f;
  anz_ops = 0;
  anz_wstack = 0;
  skipcalc_opstack_pos = 0;

  while( *b )
  { while( *b == ' ' || *b == '\n' )
      b++;
    if( !*b )
      break;

    pb = &b;

    LOG(5, "berechnestr, pb: %s, op_flag: %d.\n", *pb, op_flag);
    if( op_flag )
    { if( pushop(pb, &op_flag) )
        return false;
    }
    else if( isunaryop(**pb) && !(**pb == '-' && isdigit(*(*pb+1))) )
    { if( pushunaryop(pb) )
        return false;
    }
    else
    { if( skipcalc_opstack_pos )
      { if( !parflag )
          skip(&b);
      }
      else
      { p = b;
        if( *p == '-' )                           /* '-' überlesen, wird nicht getestet*/
          p++;
        strcpyn_str(parwert, &p, OPS " \t", MAX_ZEILENLAENGE);
        LOG(4, "berechnestr, parwert: %s.\n", parwert);
        if( test_long(parwert) )
        { if( pushlong(getlong(&b)) )
            return false;
        }
        else if( test_double(parwert) )
        { if( pushdouble(getdouble(&b)) )
            return false;
        }
        else
        { if( pushstring(getstring(&b)) )
            return false;
        }
      }
      op_flag = true;
    }
    LOG(3, "berechnestr, b: %s, opstack: %c.\n", b, anz_ops ? opstack[anz_ops-1] : ' ');
  }

  while( anz_ops && !calc() )
    ;
  LOG(2, "/berechnestr, anz_wstack: %d, Wert: %s.\n",
      anz_wstack, wert2string(wstack[0]));
  if( anz_wstack == 1 )
  { strcpyn_z(out, wert2string(wstack[0]), n);
    return false;
  }
  else
  { LOG(1, "/berechnestr, Fehler in Formel: %s.\n", f);
    return true;
  }
}


/***************************************************************************************/
/* int test(char *bed)                                                                 */
/*          char *bed: zu testende Bedingung                                           */
/*          return, wenn Bedingung erfuellt                                            */
/*     test wertet die Bedingung bed aus und liefert Wahrheitswert                     */
/***************************************************************************************/
int test(char *bed)
{ wert w;

  LOG(1, "test, bed: %s.\n", bed);

  if( berechne(&w, bed) )
    return true;                                       /* keine Bedingung ist true     */
  else
  { if( w.type == LONGW )
      return !!(int)w.w.l;
    else if( w.type == DOUBLEW )
      return 0.0 != w.w.d;
    else if( w.type == STRINGW )
      return !!*(char *)w.s;
    else
      return false;
  }
}
#else  /* #ifndef COMPILER */


/***************************************************************************************/
/* int comp_pushelem(char **s)                                                         */
/*               char **s: gelesenen Eingabezeile                                      */
/*                             nach ')' kommt ein Operator                             */
/*     comp_pushelem kompiliert %{ Formel } oder Bedinung und es wird ein Element      */
/***************************************************************************************/
int comp_pushop(char **s)
{ char comp[MAX_ZEILENLAENGE];

  if( **s == ATTRIB_CHAR )
  { (*s)++;
    
    
}


/***************************************************************************************/
/* int comp_pushop(char **s, int *op_flag)                                             */
/*               char **s: gelesenen Eingabezeile                                      */
/*               in *op_flag: wird auf true gesetzt, wenn ein Operator folgen darf,    */
/*                             nach ')' kommt ein Operator                             */
/*     comp_pushop kompiliert %{ Formel } oder Bedinung und es wird ein OP gelesen     */
/***************************************************************************************/
int comp_pushop(char **s, int *op_flag)
{ char opstr[MAX_LEN_OP+2];                      /* +1 zum Testen, +1 für '\0'         */
  char *o;
  char op;

  LOG(1, "comp_pushop, s: %s.\n", *s);

  o = opstr;
  do                                              /* lesen, bis Operator komplett      */
  { if( !(*o++ = *(*s)++) )                       /* nächstes Zeichen anhängen         */
      break;                                      /* Stringende, dann Schluss          */
    *o = '\0';
  } while( testop(opstr) );                       /* ist opstr noch Operator?          */
  (*s)--;                                         /* nicht mehr, Zeichen zurück        */
  *(o-1) = '\0';                                  /* Ende Operator setzen              */
  if( !(op = testop(opstr)) )                     /* ungültiger Operator?              */
    return true;                                  /* Fehler                            */

  *op_flag = false;

  if( op == ')' )                                 /* Klammer zu?                       */
  { while( anz_ops && opstack[anz_ops-1] != '(' )  /* Stack bis '(' abarbeiten         */
      if( comp_calc() )
        break;
    if( anz_ops && opstack[anz_ops-1] == '(' )
      popop();
    *op_flag = true;
    return false;
  }
  else
  { while( anz_ops && getprio(op) >= getprio(opstack[anz_ops-1])
           && getprio(op) && getprio(opstack[anz_ops-1]) )
     if( comp_calc() )
        break;
  }

  if( anz_ops < MAXANZOPS )
  { opstack[anz_ops++] = op;
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int comp_calc_op(char **s, int kflag)                                               */
/*               char **s: gelesenen Eingabezeile                                      */
/*               int kflag: true, es muss bis '}' gelesen werden, sonst bis Zeilenende */
/*     comp_calc_op kompiliert %{ Formel } oder Bedinung und es wird ein OP gelesen    */
/***************************************************************************************/
int comp_calc_op(char **s, int kflag)
{ int op_flag;

  while( **s == ' ' || *ss == '\t' )
    (*s)++;
  if( !**s || (kflag && **s == '}' )
    return false;
  if( comp_pushop(s, &op_flag) )
    return true;
  if( op_flag )
    return comp_calc_op(s, kflag);
  else
    return comp_calc_elem(s, kflag);
}


/***************************************************************************************/
/* int comp_calc_elem(char **s, int kflag)                                             */
/*               char **s: gelesene Eingabezeile                                       */
/*               int kflag: true, es muss bis '}' gelesen werden, sonst bis Zeilenende */
/*     comp_calc kompiliert %{ Formel } oder Bedinung es wird Element gelesen          */
/***************************************************************************************/
int comp_calc_elem(char **s, int kflag)
{ while( **s == ' ' || **s == '\t' )
    (*s)++;
  if( !**s || (kflag && **s == '}') )
    return false;
  if( isunaryop(**s) && !(**s == '-' && isdigit(*(*ss+1))) )
  { if( comp_pushunaryop(s) )
      return true;
    return comp_calc_elem(s, kflag);
  }
  else
  { if( comp_pushelem(s) )
      return true;
    return comp_calc_op(s, kflag;
}


/***************************************************************************************/
/* int comp_calc(char **out, char **s, long n, int kflag)                              */
/*               char **out: Ergegbnis hierinein                                       */
/*               char **s: gelesenen Eingabezeile                                      */
/*               long n    : Platz in out                                              */
/*               int kflag: true, es muss bis '}' gelesen werden, sonst bis Zeilenende */
/*     comp_calc kompiliert %{ Formel } oder Bedinung                                  */
/***************************************************************************************/
int comp_calc(char **out, char **s, long n, int kflag)
{ int op_flag = false;

  comp_calc_elem(s, kflag);
  if( kflag && **s == '}' )
    (*s)++;

}

#endif  /* #else #ifndef COMPILER */
