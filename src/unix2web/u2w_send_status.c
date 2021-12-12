/*********************************/
/* File: u2w_send_status.c       */
/* Seite /?&status[_flush]       */
/* timestamp: 2015-10-10 19:16:29*/
/*********************************/


#include "u2w.h"


#ifdef WEBSERVER


#ifdef WITH_MMAP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
// verhindert: warning: ‘...’ may be used uninitialized in this function [-Wmaybe-uninitialized]
/***************************************************************************************/
/* int send_status(int flush_flag)                                                     */
/*             int flush_flag: true: counter werden nach Auslesen genullt              */
/*             return      : true bei fehler, false sonst                              */
/*     send_status sendet Status                                                       */
/***************************************************************************************/
int send_status(int flush_flag)
{ char *z;
  int sb;
  time_t nt, dt;
  long long unsigned lc, lr, lbs, lbr, le;
  long long unsigned dc, dr, dbs, dbr, de;

  LOG(1, "send_status, flush_flag: %d.\n", flush_flag);

  z = zeile;
  time(&nt);
  if( status_mode & STAT_TIMESTAMP )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "timestatmp: %ld\n", nt);
  if( status_mode & STAT_UPTIME )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "uptime-secs: %ld\n", nt-starttime);
  LOG(11, "send_status, timestamp: %ld.\n", nt);

  if( flush_flag )
  { if( !status_counter )
      lc = connectioncounter;
    else if( status_mode > STAT_CONNECT )
    { time_t lt;

      dc = (lc=connectioncounter) - status_counter->flushconnectioncounter;
      status_counter->flushconnectioncounter += lc;
      dr = (lr=status_counter->requests) - status_counter->lastrequests;
      status_counter->lastrequests = status_counter->requests -= lr;
      dbs = (lbs=status_counter->bytessend) - status_counter->lastbytessend;
      status_counter->lastbytessend = status_counter->bytessend -= lbs;
      dbr = (lbr=status_counter->bytesreceived) - status_counter->lastbytesreceived;
      status_counter->lastbytesreceived = status_counter->bytesreceived -= lbr;
      de = (le=status_counter->errors) - status_counter->lasterrors;
      status_counter->lasterrors = status_counter->errors -= le;
      if( status_mode >= STATDIFF_CONNECT )
      { lt = status_counter->time_last_read;
        dt = (status_counter->time_last_read = nt) - lt;
      }
    }
    else
      lc = connectioncounter;
    kill(parentpid, SIGUSR1);
  }
  else
  { lc = connectioncounter;
    if( status_counter && status_mode > STAT_CONNECT )
    { dc = lc - status_counter->lastconnectioncounter;
      status_counter->lastconnectioncounter += dc;
      dr = (lr=status_counter->requests) - status_counter->lastrequests;
      status_counter->lastrequests += dr;
      dbs = (lbs=status_counter->bytessend) - status_counter->lastbytessend;
      status_counter->lastbytessend += dbs;
      dbr = (lbr=status_counter->bytesreceived) - status_counter->lastbytesreceived;
      status_counter->lastbytesreceived += dbr;
      de = (le=status_counter->errors) - status_counter->lasterrors;
      status_counter->lasterrors += de;
      if( status_mode >= STATDIFF_CONNECT )
      { dt = nt - status_counter->time_last_read;
        status_counter->time_last_read += dt;
      }
    }
  }

  if( status_mode & STAT_CONNECT )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "connects: %lld\n", lc);
  if( status_mode & STAT_REQUEST )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "requests: %lld\n", lr);
  if( status_mode & STAT_SEND )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "bytessend: %lld\n", lbs);
  if( status_mode & STAT_RECEIVED )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "bytesreceived: %lld\n", lbr);
  if( status_mode & STAT_ERROR )
    z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "errors: %lld\n", le);

  if( status_mode >= STATDIFF_CONNECT )
  { z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "delta-time-secs: %ld\n", dt);

    if( !(status_mode & STATDIFF_ONLYPS) )
    { if( status_mode & STATDIFF_CONNECT )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "delta-connects: %lld\n", dc);
      if( status_mode & STATDIFF_REQUEST )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "delta-requests: %lld\n", dr);
      if( status_mode & STATDIFF_SEND )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "delta-bytessend: %lld\n", dbs);
      if( status_mode & STATDIFF_RECEIVED )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "delta-bytesreceived: %lld\n", dbr);
      if( status_mode & STATDIFF_ERROR )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "delta-errors: %lld\n", de);
    }
    if( (status_mode & STATDIFF_PS) && dt > 0 )
    { if( status_mode & STATDIFF_CONNECT )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "connects-persecond: %lld\n", dc/dt);
      if( status_mode & STATDIFF_REQUEST )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "requests-persecond: %lld\n", dr/dt);
      if( status_mode & STATDIFF_SEND )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "bytessend-persecond: %lld\n", dbs/dt);
      if( status_mode & STATDIFF_RECEIVED )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "bytesreceived-persecond: %lld\n", dbr/dt);
      if( status_mode & STATDIFF_ERROR )
        z += snprintf(z, MAX_ZEILENLAENGE-(z-zeile), "errors-persecond: %lld\n", de/dt);
    }
  }

  if( send_http_header("text/plain", "", true, z-zeile, u2w_charset) )
    return true;

#ifdef WITH_HTTPS
  if( ssl_mode )
    sb = SSL_write(ssl, zeile, z-zeile);
  else
#endif  /* WITH_HTTPS */
    sb = send(sh, zeile, z-zeile, 0);
  if( sb == -1 )                                        /* und absenden            */
  { if( logflag & NORMLOG )
      perror("send");
    return true;
  }

  LOG(2, "/send_status.\n");
  return false;
}
#pragma GCC diagnostic pop
#endif
#endif
