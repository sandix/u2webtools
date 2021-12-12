/*************************************************/
/* File: u2wtime.c                               */
/* Funktionen zur Umwandlung von Datumswerten    */
/* timestamp: 2013-10-27 11:31:21                */
/*************************************************/


#include "u2w.h"

#ifndef COMPILER


char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/***************************************************************************************/
/* time_t datestr2time_t(char *s)                                                      */
/*                       char *s: Datum als String: ttt, dd mmm yyyy HH:MM:SS GMT      */
/*        datestr2time_t wandelt den Datumsstring nach time_t um                       */
/***************************************************************************************/
time_t datestr2time_t(char *s)
{ struct tm tms;                                            /* zum Aufbauen des Datums */
  char month[4];                                            /* Monat als String        */
  int jahr;
  int i;
  int e;

  LOG(1, "datestr2time_t, s: %s.\n", s);
  if( *(s+3) == ',' )
    e = sscanf(s, "%*s %d %3s %d %d:%d:%d", &tms.tm_mday, month, &jahr,
               &tms.tm_hour, &tms.tm_min, &tms.tm_sec);
  else if( strchr(s, '-') != NULL )
  { e = sscanf(s, "%*s %d-%3s-%d %d:%d:%d", &tms.tm_mday, month, &jahr,
               &tms.tm_hour, &tms.tm_min, &tms.tm_sec);
    if( jahr < 80 )
      jahr += 2000;
    else
      jahr += 1900;
  }
  else
    e = sscanf(s, "%*s %3s %d %d:%d:%d %d", month, &tms.tm_mday,
               &tms.tm_hour, &tms.tm_min, &tms.tm_sec, &jahr);
  tms.tm_isdst = 0;

  LOG(3, "datestr2time_t, e: %d.\n", e);
  if( 6 == e )
  { tms.tm_year = jahr-1900;
    for( i = 0; i < 12; i++ )
      if( strcmp(months[i], month) == 0 )
      { tms.tm_mon = i;
        break;
      }
    LOG(4, "datestr2time_t, mktime: %ld.\n", mktime(&tms));
    LOG(4, "datestr2time_t, asctime: %s.\n", asctime(&tms));
    return mktime(&tms);                                    /* nach time_t umwandeln   */
  }
  else
    return 0;
}


/***************************************************************************************/
/* char *httptime(const struct tm *tp)                                                 */
/*                const struct tm *tp: zu konvertierendes Datum                        */
/*       httptime erzeugt einen Datumsstring nach RFC 1123                             */
/***************************************************************************************/
char *httptime(const struct tm *tp)
{ static char date[30];

  snprintf(date, 30, "%3.3s, %02d %3.3s %04d %02d:%02d:%02d GMT",
           days[tp->tm_wday], tp->tm_mday, months[tp->tm_mon], 1900+tp->tm_year,
           tp->tm_hour, tp->tm_min, tp->tm_sec);
  return date;
}


/***************************************************************************************/
/* char *timestr(const struct tm *tp)                                                  */
/*               const struct tm *tp: zu konvertierendes Datum                         */
/*       timestr erzeugt einen Datumsstring: tt.mm.jj HH:MM:SS                         */
/***************************************************************************************/
char *timestr(const struct tm *tp)
{ static char date[20];

  snprintf(date, 20, "%02d.%02d.%02d %02d:%02d:%02d",
           tp->tm_mday, tp->tm_mon+1, 
           tp->tm_year > 100 ? tp->tm_year - 100 : tp->tm_year,
           tp->tm_hour, tp->tm_min, tp->tm_sec);
  return date;
}
#endif  /* #ifndef COMPILER */
