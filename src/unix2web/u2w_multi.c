/***********************************************************/
/* File: u2w_multi.c                                       */
/* Funktionen zum Auswerten eines multipart Content-Blocks */
/* timestamp: 2017-02-12 14:49:46                          */
/***********************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* char *datname(char *path)                                                           */
/*               char *path: Pfad der Datei                                            */
/*               return: Datei ohne Pfad                                               */
/*       datname bestimmt zu einem Pfad den Anteil der Datei                           */
/***************************************************************************************/
char *datname(char *path)
{ char *p;

  if( NULL != (p = strrchr(path, '/')) )
    return p+1;
  else if( NULL != (p = strrchr(path, '\\')) )
    return p+1;
  else if( NULL != (p = strrchr(path, ':')) )
    return p+1;
  else
    return path;
}


char *read_multip_p;                              /* Pointer auf gelesene Zeichen      */
char *read_multip_b;                              /* Inputpuffer von read_http gesetzt */
long read_multip_nb;                              /* Anzahl Zeichen im Inputpuffer     */
/***************************************************************************************/
/* int read_multipart(char *puffer, long n, long *nb, char *term)                      */
/*                    char *puffer: Platz für die gelesene Zeile                       */
/*                    long n      : Anz Byte Platz im Puffer                           */
/*                    long *gb    : Anz Byte gelesen                                   */
/*                    char *term  : Sting, bis zu dem gelesen werden soll              */
/*                                  "", wenn Boundary-Header gelesen werden soll       */
/*                    return      : 1, es muss noch mehr gelesen werden                */
/*                                  0, Multipart-Block komplett gelesen                */
/*                                 -1, Fehler oder Timout                              */
/*     read_multipart liest multipart Content-Blocks bis term gefunden wird, term      */
/*                    wird nicht zurückgeliefert. Ist term nicht angegeben, wird       */
/*                    einschließlich CR LF CR LF eingelesen.                           */
/***************************************************************************************/
int read_multipart(char *puffer, long n, long *gb, char *term)
{ char *pp;                                       /* Pointer auf puffer                */
  char multipart_term_start[MAX_BOUNDARY_LEN];
  char *pm, *m;

  pp = puffer;
  *gb = 0;

  LOG(1, "read_multipart, term: %s.\n", term);
  LOG(21, "read_multipart, p-b: %ld, nb: %ld.\n", (long)(read_multip_p-read_multip_b),
      read_multip_nb);
  if( *term )                                     /* Multipart term angegeben?         */
  { int termlen;

    LOG(3, "read_multipart, then fall.\n");       /* Boundary-Inhalt lesen             */
    termlen = strlen(term);
    for(;;)
    { LOG(20, "read_multipart - for, p-b: %ld, nb: %ld, n: %ld.\n",
          read_multip_p-read_multip_b, read_multip_nb, n);
      while( read_multip_p - read_multip_b < read_multip_nb )/* noch Zeichen im Puffer?*/
      { if( *read_multip_p != *term )             /* nicht 1. Z. der Endemarke         */
        { *pp++ = *read_multip_p++;               /* Zeichen übertragen                */
          if( ++(*gb) >= n )                      /* zählen und Ausgabepuffer voll?    */
            return 1;                             /* ja, zurück                        */
        }
        else if( read_multip_p - read_multip_b < read_multip_nb - 1
                 && *(read_multip_p+1) != *(term+1) )
        { *pp++ = *read_multip_p++;               /* Zeichen übertragen                */
          if( ++(*gb) >= n )                      /* zählen und Ausgabepuffer voll?    */
            return 1;                             /* ja, zurück                        */
          if( *read_multip_p != *term )
          { *pp++ = *read_multip_p++;             /* Zeichen übertragen                */
            if( ++(*gb) >= n )                    /* zählen und Ausgabepuffer voll?    */
              return 1;                           /* ja, zurück                        */
          }
        }
        else
        {                                         /* nein, 1. Zeichen Endemarke        */
          LOG(100, "read_multipart, len(p): %d, len(term): %d, nb-(p-b): %d.\n",
              strlen(read_multip_p), strlen(term),
              read_multip_nb-(read_multip_p-read_multip_b));
                                                  /* ggf. Anfang von term im Pufer     */
          if( read_multip_nb - (read_multip_p-read_multip_b) < termlen )
          { LOG(100, "read_multipart, if %d >= %d.\n",
                *gb + termlen, n);
            if( *gb + termlen >= n )              /* wenn Term-Puffer nicht passt      */
              return 1;                           /* erst mal zurück                   */
            pm = m = multipart_term_start;        /* Termtest start merken             */
            while( (read_multip_nb - (read_multip_p-read_multip_b)) + (m-pm) < termlen )
            { while( read_multip_p - read_multip_b < read_multip_nb )
                *m++ = *read_multip_p++;
              LOG(50, "read_multipart - neue Daten lesen, m-pm: %ld, termb:\n%.20s.\n",
       	          m - pm, multipart_term_start);
              LOG(50, "read_multipart - neue Daten lesen, m-pm: %ld, termb: %s.\n",
       	          m - pm, hexstring(multipart_term_start, m-multipart_term_start));
                                                  /* Puffer neu füllen                 */
              if( 0 >= (read_multip_nb = read_http(&read_multip_b, 1)) )
              { while( pm < m )                   /* nichts gelesen, dann term_start   */
                { *pp++ = *pm++;                  /* auch zurück                       */
                  if( ++(*gb) >= n )              /* zählen und Ausgabepuffer voll?    */
                    return 1;
                }
                *pp = '\0';
                return read_multip_nb;            /* es wurde nichts gelesen           */
              }
              read_multip_p = read_multip_b;
            }
            while( pm < m )                       /* Zwischenpuffer ausgeben           */
            { if( *pm != *term )
              { *pp++ = *pm++;
                if( ++(*gb) >= n )                /* zählen und Ausgabepuffer voll?    */
                  return 1;                       /* ja, zurück                        */
              }
                                                  /* Endemarke gefunden?               */
              else if( str_starts_conc(pm, m, read_multip_p, term) )
              { *pp = '\0';                       /* ja                                */
                return 0;
              }
              else
              { *pp++ = *pm++;                    /* 1. Z. überspringen                */
                if( ++(*gb) >= n )                /* zählen und Ausgabepuffer voll?    */
                  return 1;                       /* ja, zurück                        */
              }
            }
          }
          else
          { if( str_starts(read_multip_p, term) ) /* Endemarkierung gefunden?          */
            { *pp = '\0';
              return 0;                           /* ja                                */
            }
            *pp++ = *read_multip_p++;             /* 1. Z. der Endem. überlesen        */
            if( ++(*gb) >= n )                    /* zählen und Ausgabepuffer voll?    */
              return 1;                           /* ja, zurück                        */
          }
        }
      }                                           /* ende while - Puffer leer          */
      if( 0 >= (read_multip_nb = read_http(&read_multip_b, 1)) ) /* Puffer neu füllen  */
      { *pp = '\0';
        return read_multip_nb;                    /* es wurde nichts gelesen           */
      }
      read_multip_p = read_multip_b;
      LOG(10, "read_multipart - read_http 1, b: %ld, nb: %ld.\n",
          read_multip_b, read_multip_nb);
    }  /* for(;;) */
  }
  else                                            /* Term nicht angegeben, dann        */
  {                                               /* Boundary-Header lesen             */
    LOG(3, "read_multipart, else fall, nb: %ld.\n", read_multip_nb);
    for(;;)
    { while( read_multip_p - read_multip_b < read_multip_nb )                         
      { if( *gb >= n )                            /* kein Platz im Ausgabepuffer?      */
          return 1;                               /* ja, zurück                        */
        *pp++ = *read_multip_p++;                 /* Zeichen übernehmen                */
        (*gb)++;                                  /* Zeichen zählen                    */
        if( *gb >= 4
            && *(pp-4) == '\xd' &&  *(pp-3) == '\xa' /* CRLFCRLF gefunden?             */
            &&  *(pp-2) == '\xd' &&  *(pp-1) == '\xa' )
        { *pp = '\0';
          return 0;                               /* ja, dann zurück                   */
        }
      }
      LOG(5, "read_multipart, lesen weiterer Daten.\n");
      if( 0 >= (read_multip_nb = read_http(&read_multip_b, 2)) )
      { *pp = '\0';
        return read_multip_nb;
      }
      LOG(10, "read_multipart - read_http 2, b: %ld, nb: %ld, gb: %ld, n: %ld.\n",
          read_multip_b, read_multip_nb, *gb, n);
      read_multip_p = read_multip_b;
    }
  }
}


/***************************************************************************************/
/* void conv_filename(char *s)                                                         */
/*                    char *s: Dateiname, des Clients                                  */
/*      conv_filename ersetzt \ durch /, wenn s ein Dateiname im Dos-Format ist und    */
/*                    der Server unter UNIX läuft.                                     */
/*      conv_filename ersetzt / durch \, wenn s ein Dateiname im UNIX-Format ist und   */
/*                    der Server unter Win läuft.                                      */
/***************************************************************************************/
void conv_filename(char *s)
{
#ifdef UNIX
  if( *(s+1) == ':' && *(s+2) == '\\' )
  { while( *s )
    { if( *s == '\\' || *s == ':' )
        *s++ = '/';
      else
        s++;
    }
  }
#else
  while( *s )
  { if( *s == '/' )
      *s++ = '\\';
    else
      s++;
  }
#endif
}


/***************************************************************************************/
/* int get_multiparts(char *boundary)                                                  */
/*                    char *boundary: String, der die Teile unterteilt                 */
/*                    return        : true bei Fehler                                  */
/*     get_multiparts extrahiert die Parameter und Dateien aus dem Content-Block       */
/***************************************************************************************/
int get_multiparts(char *boundary)
{ char puffer[MAX_MULTIPART_BLOCK];               /* Zwischenspeicher                  */
  char *p;                                        /* Zeiger auf puffer                 */
  long nb;                                        /* Anzahl Zeichen im Inputpuffer     */
  char term[MAX_BOUNDARY_LEN];                    /* enthält "CRLF--<boundary>"        */
  char filename[MAX_NAME_LEN];                    /* Dateiname, der Datei              */
  short cont_flag;                                /* wurde schon alles gelesen?        */
  short maxfilesizeflag;                          /* Maximale Dateigroesse erreicht    */
  char name[MAX_PAR_NAME_LEN];                    /* Name des Content-Eintrages        */
  char error[MAX_PAR_NAME_LEN];                   /* Name des Fehlerparameters         */
  char clientname[MAX_PAR_NAME_LEN];              /* Name des Clientfilenameparameters */
  char mime[MAX_MIME_LEN];                        /* Mime-Type                         */
  unsigned long filesize;                         /* geschriebene Zeichen              */

  if( !boundary || !*boundary )
  { LOG(1, "get_multiparts, KEIN boundary.\n");
    return false;
  }

  read_multip_p = NULL;                           /* Pointer auf gelesene Zeichen      */
  read_multip_b = NULL;                           /* Inputpuffer von read_http gesetzt */
  read_multip_nb = 0;                             /* Anzahl Zeichen im Inputpuffer     */

  LOG(1, "get_multiparts, boundary: %s.\n", boundary);

  snprintf(term, MAX_BOUNDARY_LEN, "\xd\xa--%s", boundary);
  LOG(11, "get_multiparts, term: %s.\n", term);

                                                  /* Boundary-Header einlesen          */
  if( read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, "") )
    return true;                                  /* Header nicht vollständig gelesen  */
  LOG(7, "get_multiparts - read_multipart, gelesen: %ld.\n", nb);
  p = puffer;
  while( nb > 0 )
  { LOG(4, "get_multiparts, pufferlen: %ld.\n", nb);
    LOG(40, "get_multiparts, puffer: %s.\n", p);
    if( !str_starts_z(&p, term+2) )               /* Vergleichen                       */
    { LOG(2, "/get_multiparts, !str_starts.\n");
      return true;                                /* nicht Boundary, dann Fehler       */
    }
    if( *p == '-' && *(p+1) == '-' )              /* wars der letzte Block?            */
    { LOG(2, "/get_multiparts, --.\n");
      return false;                               /* ja, dann fertig                   */
    }
    getcontdisp(name, p, "name=", MAX_PAR_NAME_LEN);
    getcontdisp(mime, p, "Content-Type: ", MAX_MIME_LEN);
    if( getcontdisp(filename, p, "filename=", MAX_NAME_LEN) )  /* Datei?               */
    { strcpyn(clientname, name, MAX_PAR_NAME_LEN);
      strcatn(clientname, CLIENTPATHNAME, MAX_PAR_NAME_LEN);
      store_parwert(clientname, filename, HP_BROWSER_SYS_LEVEL);
      strcpyn(clientname, name, MAX_PAR_NAME_LEN);
      strcatn(clientname, CLIENTMIME, MAX_PAR_NAME_LEN);
      store_parwert(clientname, mime, HP_BROWSER_SYS_LEVEL);
      LOG(5, "get_multiparts, clientname: %s, filename: %s.\n", clientname, filename);
      strcpyn(clientname, name, MAX_PAR_NAME_LEN);
      strcatn(clientname, CLIENTFILENAME, MAX_PAR_NAME_LEN);
      conv_filename(filename);
      store_parwert(clientname, datname(filename), HP_BROWSER_SYS_LEVEL);
      if( upload_post_mode != NO_POST )           /* Uploads erlaubt?                  */
      { strcpy(error, name);
        strcatn(error, ERRORNAME, MAX_PAR_NAME_LEN);

        LOG(8, "get_multiparts, vor NULL == ..., name: %s.\n", name);
      
        if( NULL == strstr(name, "../") && NULL == strstr(name, "/..") ) /* nicht ..   */
        { int h;
          char pathname[2*MAX_PATH_LEN];          /* Name der Datei aus multipart cont */

          if( store_client_filename_flag )
          { strcpyn(pathname, upload_post_path, 2*MAX_PATH_LEN);
            strcatn(pathname, "/", 2*MAX_PATH_LEN);
            strcatn(pathname, datname(filename), 2*MAX_PATH_LEN);
          }
          else
            snprintf(pathname, 2*MAX_PATH_LEN, "%s/%s.%u", upload_post_path, name,
                     getpid());
          LOG(5, "get_multiparts, speichern pathname: %s.\n", pathname);
          if( upload_post_mode != POST_WRITE || test_access(pathname, user) )
          { int w_flag, dummy;

            w_flag = true;
            if( upload_post_mode == POST_WRITE )
            { test_passwd(pathname, auth_basic, &dummy, &w_flag, &dummy, NULL, NULL);
              test_hosts(pathname, &dummy, &w_flag, &dummy);
            }
            if( w_flag )
            { if( 0 <= (h = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0600)) )
              { if( auto_delete_post_files
                    && anz_auto_delete_files < MAX_ANZ_AUTO_DELETE_FILES )
                  strcpyn(auto_delete_files[anz_auto_delete_files++], pathname,
                          MAX_PATH_LEN);
                filesize = 0;
                maxfilesizeflag = false;
                do
                { cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term);
                                                  /* Inhalt der Datei lesen            */
                  LOG(17, "get_multiparts - read_multipart, cont_flag: %d, numb: %ld.\n",
                      cont_flag, nb);
                  if( max_post_filesize > 0 && filesize + nb > max_post_filesize )
                  { nb = max_post_filesize - filesize;
                    if( !maxfilesizeflag )
                    { maxfilesizeflag = true;
                      store_parwert(error, _("Max filesize reached."), HP_BROWSER_SYS_LEVEL);
                    }
                  }
                  if( 0 < nb )
                  { if( nb > write(h, puffer, nb) ) /* und schreiben                   */
                    {                             /* Schreibfehler                     */
                      store_parwert(error, _("Can't write to file."), HP_BROWSER_SYS_LEVEL);
                      if( cont_flag > 0 )         /* Muss noch was gelesen werden?     */
                        while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
                                                  /* Datei bis Ende lesen              */
                      break;                      /* Weiter im Content-Block           */
                    }
                    else
                      filesize += nb;
                  }
                } while( cont_flag > 0 );
                close(h);
                if( cont_flag < 0 )
                 return true;
                strcpyn(clientname, name, MAX_PAR_NAME_LEN);
                strcatn(clientname, STOREPATHNAME, MAX_PAR_NAME_LEN);
                store_parwert(clientname, pathname, HP_BROWSER_SYS_LEVEL);
              }
              else
              { store_parwert(error, _("Can't open file."), HP_BROWSER_SYS_LEVEL);
                LOG(5, "get_multipart, Fehler: Datei %s kann nicht geöffnet werden\n",
                    pathname);
                while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
                if( cont_flag < 0 )
                  return true;
              }
            }
            else
            { store_parwert(error, _("Access denied."), HP_BROWSER_SYS_LEVEL);
                                                               /* .passwd oder .hosts  */
              while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
              if( cont_flag < 0 )
                return true;
            }
          }
          else
          { store_parwert(error, _("Access denied."), HP_BROWSER_SYS_LEVEL); /* .access    */
            while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
            if( cont_flag < 0 )
              return true;
          }
        }
        else
        { store_parwert(error, _("Filename not allowed."), HP_BROWSER_SYS_LEVEL);
                                                   /* Datname mit /.. oder ../ im Pfad */
          while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
          if( cont_flag < 0 )
            return true;
        }
      }
      else
      { if( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK-1, &nb, term)) >= 0)
        { LOG(4, "get_multiparts, name: %s, puffer: %s.\n", name, puffer);
          store_parwert_bin(name, puffer, nb);      /* erste 32k Speichern             */
        }
        if( cont_flag > 0 )
          while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
        if( cont_flag < 0 )
          return true;
      }
    }
    else
    { if( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK-1, &nb, term)) >= 0)
      { puffer[nb] = '\0';
        LOG(4, "get_multiparts, name: %s, puffer: %s.\n", name, puffer);
        store_parwert(name, puffer, HP_BROWSER_LEVEL);  /* erste 32k Speichern         */
      }
      if( cont_flag > 0 )
        while( (cont_flag = read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, term)) > 0 );
      if( cont_flag < 0 )
        return true;
    }
    if( read_multipart(puffer, MAX_MULTIPART_BLOCK, &nb, "") ) /* Header               */
    { LOG(2, "/get_multiparts, read_multipart.\n");
      return true;
    }
    p = puffer+2;
  }
  LOG(2, "/get_multiparts.\n");
  return false;
}
#endif  /* #ifdef WEBSERVER */
