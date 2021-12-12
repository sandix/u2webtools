/*******************************************************/
/* File: httpget_str.c                                 */
/* main function and variables                         */
/* timestamp: see COMPILEDATE                          */
/*******************************************************/

#include "httpget.h"


void encode_base64(unsigned char *c, unsigned char *t)
{ unsigned char *cc, *tt;
  int n;
  char *b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  n = strlen((char *)t);
  tt = t;
  cc = c;
  tt[n+1] = 0;

  while( tt-t < n )
  { *cc++ = b64[*tt >> 2];
    *cc++ = b64[(*tt&0x3) << 4 | (*(tt+1)&0xf0) >> 4];
    *cc++ = b64[(*(tt+1)&0xf) << 2 | *(tt+2) >> 6];
    *cc++ = b64[(*(tt+2)&0x3f)];
    tt += 3;
  }
  *cc = '\0';
  if( !*(tt-1) )
    *(cc-1) = '=';
  if( !*(tt-2) )
    *(cc-2) = '=';
}


/***************************************************************************************/
/* char *strcasestr(const char *s, const char *t)                                      */
/*                  const char *s: String, in dem gesucht wird                         */
/*                  const char *t: String, der gesucht werden soll                     */
/*                  return       : Pointer auf Fundstelle des Strings t                */
/*                                 NULL, wenn t nicht gefunden                         */
/*       strcasestr sucht den String t im String s ohne Gross/Klein zu unterscheiden   */
/***************************************************************************************/
char *strcasestr(const char *s, const char *t)
{ char c;
  size_t len;

  c = tolower(*t++);
  if( c )
  { len = strlen(t);
    while( *s )
    { if( tolower(*s) == c )
      { if( strncasecmp(s+1, t, len) == 0 )
          return (char *)s;
      }
      s++;
    }
    return NULL;
  }
  return (char *)s;
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
