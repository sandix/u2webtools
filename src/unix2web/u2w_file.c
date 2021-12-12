/*************************************************/
/* File: u2w_file.c                              */
/* Funktionen zum Lesen und schreiben in Dateien */
/* timestamp: 2015-12-19 10:52:40                */
/*************************************************/


#include "u2w.h"

#ifndef COMPILER

/***************************************************************************************/
/* int skip_to(char *zeile, char *to, FILE *ptr)                                       */
/*             char *zeile: Platz fuer die gelesenen Zeilen                            */
/*             char *to   : String, bis zu dem gelesen werden soll                     */
/*             FILE *ptr  : Datei, von der gelesen wird                                */
/*             return true, wenn nicht gefunden                                        */
/*     skip_to liest von der Datei, bis to in Zeile steht                              */
/***************************************************************************************/
int skip_to(char *zeile, char *to, FILE *ptr)
{ int l;
  char *z;

  LOG(1, "skip_to, zeile: %s, to: %s.\n", zeile, to);

  l = strlen(to);
  z = zeile;
  skip_blanks(z);
  while( strncmp(z, to, l) || isalnum(*(z+l)) )
  { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
      return true;
#ifdef DEBUG
    linenumber++;
#endif
    z = zeile;
    skip_blanks(z);
  }
  del_cr_lf(zeile);                              /* cr lf entfernen                    */
  return false;
}


/***************************************************************************************/
/* int skip_to_next(char *zeile, char *to, FILE *ptr)                                  */
/*                  char *zeile: Platz fuer die gelesenen Zeilen                       */
/*                  char *to   : String, bis zu dem gelesen werden soll                */
/*                  FILE *ptr  : Datei, von der gelesen wird                           */
/*                  return true, wenn nicht gefunden                                   */
/*     skip_to_next liest von der Datei, bis to in Zeile steht, mindestens eine Zeile  */
/***************************************************************************************/
int skip_to_next(char *zeile, char *to, FILE *ptr)
{
  LOG(1, "skip_to_next, zeile: %s, to: %s.\n", zeile, to);

  if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )
    return true;
  return skip_to(zeile, to, ptr);
}
#endif  /* #ifndef COMPILER */
