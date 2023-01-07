/**************************************************/
/* File: u2w_logging.c                            */
/* Funktionen zum ausgeben oder Speichern von     */
/* Logginginformationen                           */
/* timestamp: 2015-10-10 19:12:07                 */
/**************************************************/


#include "u2w.h"


/***************************************************************************************/
/* void logging(char *f, ...)                                                          */
/*              char *f: Formatstring für vfprintf                                     */
/*              ...    : Weitere Parameter für vfprintf                                */
/*      logging schreibt die Ausgabe nach stderr oder, wenn logpath != NULL hängt an   */
/*              die Datei an                                                           */
/***************************************************************************************/
void logging(char *f, ...)
{ FILE *ptr;
  va_list ap;
  time_t tt;
  struct tm *tm;

  time(&tt);
  tm = localtime(&tt);
  va_start(ap, f);
  if( logpath[0] )
  { if( NULL != (ptr = fopen(logpath, "a")) )
    {
#ifdef DEBUG
      int i;
      fprintf(ptr, "%04d-%02d-%02d %02d:%02d:%02d %d %s",
              tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
              tm->tm_hour, tm->tm_min, tm->tm_sec,
              (int)getpid(), clientgetpath ? clientgetpath : "");
      for( i = 0; i < include_counter; i++ )
        fprintf(ptr, " %s", curfile_stack[i]);
      fprintf(ptr, " %d - ", linenumber);
#else
      fprintf(ptr, "%04d-%02d-%02d %02d:%02d:%02d %d %s - ",
              tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
              tm->tm_hour, tm->tm_min, tm->tm_sec, (int)getpid(),
              clientgetpath ? clientgetpath : "");
#endif
      vfprintf(ptr, f, ap);
      fclose(ptr);
    }
  }
  else
  {
#ifdef DEBUG
    int i;
    fprintf(stderr, "%04d-%02d-%02d %02d:%02d:%02d %d %s",
            tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            (int)getpid(), clientgetpath ? clientgetpath : "");
    for( i = 0; i < include_counter; i++ )
      fprintf(stderr, " %s", curfile_stack[i]);
    fprintf(stderr, " %d - ", linenumber);
#else
    fprintf(stderr, "%04d-%02d-%02d %02d:%02d:%02d %d %s - ",
              tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
              tm->tm_hour, tm->tm_min, tm->tm_sec, (int)getpid(),
              clientgetpath ? clientgetpath : "");
#endif
    vfprintf(stderr, f, ap);
  }

  va_end(ap);
}

#ifdef DEBUG

char *hexstring(char *s, int n)
{ char *p, *q;
  static char rh[MAX_LEN_DEBUG_HEXSTRING];

  p = s;
  q = rh;
  while( *p && p-s < n && q-rh < MAX_LEN_DEBUG_HEXSTRING-4 )
  { if( *p < ' ' || '~' < *p )
      q += sprintf(q, "<%02X>", (unsigned char)*p++);
    else
      *q++ = *p++;
  }
  *q = '\0';
  return rh;
}

#endif

#ifdef WEBSERVER
/***************************************************************************************/
/* void connectionlogging(char *ipaddress)                                             */
/*              char *ipaddress: ipaddress des clients                                 */
/*      connectionlogging schreibt ausführliche Verbindungsinfos                       */
/***************************************************************************************/
void connectionlogging(char *ipaddress)
{ FILE *ptr;
  time_t tt;
  struct tm *tm;

  LOG(30, "connectionlogging, longlogpath: %s, ipaddress: %s\n", longlogpath, ipaddress);
  time(&tt);
  tm = localtime(&tt);
  if( longlogpath[0] )
  { if( NULL != (ptr = fopen(longlogpath, "a")) )
    { if( longlogformat && *longlogformat )
      { char *z;

        LOG(30, "connectionlogging, longlogformat: %s\n", longlogformat);
        z = zeile + sprintf(zeile, "%04d-%02d-%02d %02d:%02d:%02d %d %s " ULONGFORMAT " ",
                            tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
                            tm->tm_hour, tm->tm_min, tm->tm_sec,
                            (int)getpid(), methodstr, content_length);
        LOG(40, "connectionlogging, zeile: %s\n", zeile);
        scan_to_teil(z, longlogformat, MAX_ZEILENLAENGE-40);
        fprintf(ptr, "%s\n", zeile);
      }
      else
      { LOG(30, "connectionlogging, else\n");
        fprintf(ptr, "%04d-%02d-%02d %02d:%02d:%02d %d %s " ULONGFORMAT " %s %s\n",
                tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec, (int)getpid(),
                methodstr, content_length, ipaddress, clientgetpath ? clientgetpath : "");
      }
      fclose(ptr);
    }
  }
}
#endif
