/**************************************/
/* File: u2w_programm.c               */
/* Funktionen fuer Programmsteuerung  */         
/* (if, elif, fi, for, while, ...)    */
/* timestamp: 2013-11-17 18:11:50     */
/**************************************/


#include "u2w.h"

#ifndef COMPILER

/***************************************************************************************/
/* wertetype *store_forwerte(char *werte, char trenn)                                  */
/*                           char *werte: Werte, die gespeichert werden sollen         */
/*                           char trenn: Trennzeichen der Werte                        */
/*            store_forwerte Speichert die einzelnen Werte einer For-Schleife          */
/***************************************************************************************/
wertetype *store_forwerte(char *werte, char trenn)
{ char *p, *w, c;
  wertetype *last_wert, *first_wert;

  LOG(1, "store_forwerte, werte: %s.\n", werte);

  p = werte;

  while( *p == ' ' || *p == '\t' )
    p++;

  w = p;
  if( trenn == ' ' )
  { while( *p != ' ' && *p != '\t' && *p )
      p++;
  }
  else
  { while( *p != trenn && *p )
      p++;
  }
 
  c = *p;
  *p++ = '\0';

  if( NULL == (last_wert = first_wert = store_wert(w, NONEW)) )
    return NULL;

  while( c )
  { if( trenn == ' ' )
      while( *p == ' ' || *p == '\t' )
        p++;
    if( !*p )
      break;
    w = p;
    if( trenn == ' ' )
    { while( *p != ' ' && *p != '\t' && *p )
        p++;
    }
    else
    { while( *p != trenn && *p )
        p++;
    }
    c = *p;
    *p++ = '\0';
     if( NULL == (last_wert = last_wert->next = store_wert(w, NONEW)) )
       return NULL;
  }
  return first_wert;
}


/***************************************************************************************/
/* int skip_to_fi(char *zeile, FILE *ptr)                                              */
/*             char *zeile: Platz fuer die gelesenen Zeilen                            */
/*             FILE *ptr  : Datei, von der gelesen wird                                */
/*             return true, wenn nicht gefunden                                        */
/*     skip_to liest von der Datei, bis zur %fi Zeile und beachtet %if ... %fi         */
/***************************************************************************************/
int skip_to_fi(char *zeile, FILE *ptr)
{ char *z;

  LOG(1, "skip_to_fi, zeile: %.200s.\n", zeile);

  do
  { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
      return true;
#ifdef DEBUG
    linenumber++;
#endif
    z = zeile;
    skip_blanks(z);
    while( is_command(z, ATTRIB_STR IF) )
    { skip_to_fi(zeile, ptr);
      if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
        return true;
#ifdef DEBUG
      linenumber++;
#endif
      z = zeile;
      skip_blanks(z);
    }
    LOG(5, "skip_to_fi, vor while, z: %.200s.\n", z);
  } while( !is_command(z, ATTRIB_STR FI) );

  LOG(2, "/skip_to_fi, zeile: %.200s.\n", zeile);
  return false;
}


/***************************************************************************************/
/* int skip_to_fi_else(char *zeile, FILE *ptr)                                         */
/*             char *zeile: Platz fuer die gelesenen Zeilen                            */
/*             FILE *ptr  : Datei, von der gelesen wird                                */
/*             return true, wenn nicht gefunden                                        */
/*     skip_to liest von der Datei, bis zur %fi oder %else Zeile und beachtet          */
/*             %if ... %fi                                                             */
/***************************************************************************************/
int skip_to_fi_else(char *zeile, FILE *ptr)
{ char *z;

  LOG(1, "skip_to_fi_else, zeile: %.200s.\n", zeile);

  do
  { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
      return true;
#ifdef DEBUG
    linenumber++;
#endif
    del_cr_lf(zeile);
    LOG(3, "skip_to_fi_else 2, zeile: %.200s.\n", zeile);
    z = zeile;
    skip_blanks(z);
    if( is_command(z, ATTRIB_STR IF) )
    { skip_to_fi(zeile, ptr);
      *z = '\0';                                            /* %fi überlesen           */
                              /* *z ist richtig, da anschliessend auf z getestet wird. */
    }
  } while( !is_command(z, ATTRIB_STR FI)
           && !is_command(z, ATTRIB_STR ELSE)
           && !(is_command_z(&z, ATTRIB_STR ELSEIF) && test(z)) );
  LOG(2, "/skip_to_fi_else, zeile: %.200s.\n", zeile);
  return false;
}


/***************************************************************************************/
/* int skip_to_end_loop(char *zeile, FILE *ptr, char *loop)                            */
/*             char *zeile: Platz fuer die gelesenen Zeilen                            */
/*             FILE *ptr  : Datei, von der gelesen wird                                */
/*             char *loop : "for", "while", ...                                        */
/*             return true, wenn nicht gefunden                                        */
/*     skip_to_end_loop liest von der Datei, bis zur %/loop Zeile und beachtet         */
/*             %loop ... %/loop                                                        */
/***************************************************************************************/
int skip_to_end_loop(char *zeile, FILE *ptr, char *loop)
{ int leloop, lloop;
  char *z;
  char sloop[32], eloop[32];

  if( loop == NULL )
    return skip_to_end_this_loop(zeile, ptr);

  LOG(1, "skip_to_end_loop, zeile: %.200s, loop: %s.\n", zeile, loop);

  sloop[0] = eloop[0] = ATTRIB_CHAR;
  eloop[1] = LOOP_END_CHAR;
  strcpyn(sloop+1, loop, 31);
  strcpyn(eloop+2, loop, 30);

  lloop = strlen(sloop);
  leloop = strlen(eloop);
  do
  { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
      return true;
#ifdef DEBUG
    linenumber++;
#endif
    z = zeile;
    skip_blanks(z);
    if( !strncmp(z, sloop, lloop) )
    { if( skip_to_end_loop(zeile, ptr, loop) )
        return true;
      z = "";                                    /* Schleifenende löschen              */
    }
  } while( strncmp(z, eloop, leloop) );

  return false;
}


/***************************************************************************************/
/* int skip_to_end_this_loop(char *zeile, FILE *ptr)                                   */
/*             char *zeile: Platz fuer die gelesenen Zeilen                            */
/*             FILE *ptr  : Datei, von der gelesen wird                                */
/*             return true, wenn nicht gefunden                                        */
/*     skip_to_end_this_loop liest von der Datei, bis die aktuelle Schliefe beendet    */
/*             wird und beachtet %loop ... %/loop, fuer %break                         */
/***************************************************************************************/
int skip_to_end_this_loop(char *zeile, FILE *ptr)
{ char *z;
  char loop[32];

  LOG(1, "skip_to_end_this_loop, zeile: %.200s.\n", zeile);

  for(;;)
  { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
      return true;
#ifdef DEBUG
    linenumber++;
#endif
    z = zeile;
    skip_blanks(z);
    if( *z++ == ATTRIB_CHAR )
    { if( *z == LOOP_END_CHAR )
      { z++;
        strcpyn_l(loop, &z, 32);
        if( is_elem(LOOP_STRINGS, loop) )
          return false;
      }
      else
      { strcpyn_l(loop, &z, 32);
        if( is_elem(LOOP_STRINGS, loop) )
        { if( skip_to_end_loop(zeile, ptr, loop) )
            return true;
          continue;
        }
      }
    }
    continue;
  }
  return false;
}


/***************************************************************************************/
/* int skip_to_case(char *zeile, char *swcase, FILE *ptr)                              */
/*                  char *zeile: Platz fuer die gelesenen Zeilen                       */
/*                  char *secase: case Muster, das gesucht werden soll                 */
/*                  FILE *ptr  : Datei, von der gelesen wird                           */
/*                  return true, wenn nicht gefunden                                   */
/*     skip_to_case liest von der Datei, bis passender case, default oder end_switch   */
/*                  gefunden                                                           */
/***************************************************************************************/
int skip_to_case(char *zeile, char *swcase, FILE *ptr)
{ char *z, *p;

  LOG(1, "skip_to_case, zeile: %s, swcase: %.200s.\n", zeile, swcase);

  while(  NULL != fgets(zeile, MAX_ZEILENLAENGE, ptr) )
  { z = zeile;
#ifdef DEBUG
    linenumber++;
#endif
    skip_blanks(z);
    if( *z++ != ATTRIB_CHAR )
      continue;
    if( is_command(z, DEFAULT) )
      return false;
    else if( is_command(z, END_SWITCH) )
      return false;
    if( is_command_z(&z, CASE) )
    { p = z + (strlen(z) - 1);
      while( p > z && (*p == ' ' || *p == '\n' || *p == '\t') )
        p--;
      *(p+1) = '\0';
      scan_teil(z, MAX_ZEILENLAENGE-(z-zeile));
      LOG(3, "skip_to_case, z: %.200s.\n", z);
      if( !strcmp(z, swcase) )
        return false;
      else
        continue;
    }
  }
  return false;
}
#endif  /* #ifndef COMPILER */
