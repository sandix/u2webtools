/************************************************/
/* File: u2w_put.c                              */
/* Funktionen zum Senden der HTML-Seiten        */
/* timestamp: 2017-02-18 11:43:52               */
/************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* int bin_put(int hd_in, LONGWERT von, LONGWERT bis)                                  */
/*             int hd_in: Filehandle der zu sendenden Datei                            */
/*             LONGWERT von: Start des zu übertragenden Bereichs                       */
/*             LONGWERT bis: Ende des zu übertragenden Bereichs (-1: bis Ende)         */
/*             return      : true bei fehler, false sonst                              */
/*     bin_put sendet die Datei, die mit ptr angesprochen wird an sh                   */
/***************************************************************************************/
int bin_put(int hd_in, LONGWERT von, LONGWERT bis)
{ size_t nb;                                          /* Anzahl gelesener Zeichen      */
  int sb;                                             /* Anzahl gesendeter Zeichen     */
  size_t b;                                           /* Gesamtzahl gesendeter Zeichen */

  LOG(1, "bin_put.\n");

  while( 0 < (nb = read(hd_in, zeile, MAX_ZEILENLAENGE)) )  /* lesen                   */
  { for( b = 0; b < nb; )
    {
#ifdef WITH_HTTPS
      if( ssl_mode )
        sb = SSL_write(ssl, zeile+b, nb-b);
      else
#endif  /* WITH_HTTPS */
        sb = send(sh, zeile+b, nb-b, 0);
      if( sb == -1 )                                        /* und absenden            */
      { if( logflag & NORMLOG )
          perror("send");
        return true;
      }
      b += sb;
#ifdef WITH_MMAP
      if( status_mode & (STAT_SEND|STATDIFF_SEND) )
        status_counter->bytessend += sb;
#endif
    }
  }

  LOG(2, "/bin_put.\n");
  return false;
}


/***************************************************************************************/
/* int doput(char *file, char *par, time_t test_mod_time, int read_flag,               */
/*           int acc_flag, char *range)                                                */
/*           char *file          : Datei, die gesendet werden soll                     */
/*           char  *par          : Kommando mit Parametern bei cgi Aufruf              */
/*           time_t test_mod_time: Zeit aus If-Modified-String oder 0                  */
/*           int read_flag       : true, lesender Zugriff erlaubt                      */
/*           int acc_flag        : true, Zugriff auf .passwd oder .hosts gewährt       */
/*           char *range         : ggf. Range einer Anforderung                        */
/*           return              : true bei Fehler, false sonst                        */
/*     doput sendet die Datei file. Wenn es sich um eine *.u2w Datei handelt, dann     */
/*           wird sie nach HTML umgewandelt, andernfalls wird die Datei wie sie        */
/*           ist gesendet.                                                             */
/***************************************************************************************/
int doput(char *file, char *par, time_t test_mod_time, int read_flag,
          int acc_flag, char *range)
{ FILE *ptr = NULL;                                  /* einzulesende Datei             */
  int hd_in;
  int ret;                                           /* Rueckgabewert                  */
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
  char *p;                                           /* Ende des Dateinamens           */

  LOG(1, "doput, file: %s, par: %s.\n", file, par);

  LOG(3, "doput, read_flag: %d, acc_flag: %d, range: %s.\n", read_flag, acc_flag,
      range ? range : "<NULL>" );
  LOG(11, "doput, clientgetpath: %s, PWDEDITMODE: %s.\n", clientgetpath,
      parwert(PWDEDITMODE, HP_BROWSER_LEVEL));

  if( file[strlen(file)-1] != '/'                    /* Directory ohne '/' am Ende?    */
      && !stat64(file, &stat_buf) && (S_IFDIR & stat_buf.st_mode) == S_IFDIR )
  { send_moved_permanently(http_host ? http_host : hostname, clientgetpath, "/");
    return true;
  }

  p = strrchr(file, '/');                            /* Dateien, die mit . beginnen    */
#ifdef DEBUG
  linenumber = 0;
#endif

#ifdef DOS
  if( test_ext(file, ACC_EXT)                        /* Dateien, die mit .uaw Enden    */
      || (p == NULL && file[0] == '.')
      || (p != NULL && *(p+1) == '.') )
#else
  if( (p == NULL && file[0] == '.')                  /* nicht ausgeben                 */
      || (p != NULL && *(p+1) == '.') )              /* z. B. .hosts, .passwd ...      */
#endif
  { if( !strcmp(par, PWD_CHANGE_PAR) )               /* eigenes Kennwort ändern?       */
    { LOG(11, "doput, par == PWD_CHANGE_PAR.\nf");
	  if( strstr(file, PASSWORD_FILE) )
      { if( password_file_mode == PATH )
          return change_passwd(passwd_path, true);   /* glob. PWD ändern               */
        else
          return change_passwd(file, false);         /* PWD im Pfad ändern             */
      }
    }
    else if( !strcmp(par, PWD_CHANGE_GLOB_PAR) )
    { LOG(11, "doput, par == PWD_CHANGE_GLOB_PAR.\nf");
	  if( strstr(file, PASSWORD_FILE) )              /* .passwd angegeben?             */
      { if( password_file_mode == GLOBAL             /* globale .passwd-Datei?         */
            || password_file_mode == PATH )          /* oder feste .passwd-Datei?      */
          return change_passwd(passwd_path, true);   /* dann glob. PWD ändern          */
      }
    }
    else if( eqbpar(PWDEDITMODE, "/"PWDUSERSAVE) && strstr(file, PASSWORD_FILE) )
    { LOG(11, "doput, PWDEDITMODE == PWDUSERSAVE.\nf");
	  if( password_file_mode != PATH )               /* lokale .passwd-Datei?          */
        return update_passwd(file);                  /* PWD im Pfad ändern             */
    }
    else if( !strcmp(clientgetpath, "/"CHANGE_GLOBAL_PWD)
             && eqbpar(PWDEDITMODE, PWDUSERGLOBSAVE) )
    { LOG(11, "doput, par == CHANGE_GLOBAL_PWD.\nf");
	  if( password_file_mode == GLOBAL               /* globale .passwd-Datei?         */
          || password_file_mode == PATH )            /* feste .passwd-Datei?           */
        return update_passwd(passwd_path);           /* dann glob. PWD-Datei sichern   */
    }

    if( acc_flag )                                   /* Access auf .<datei>?           */
    { if( !strcmp(par, ACCESS_PAR) )                 /* .<datei> bearbeiten            */
      { if( strstr(file, HOSTS_FILE) )               /* .hosts angegeben?              */
        { if( hosts_file_mode == PATH )              /* globale .hosts-Datei?          */
            return edit_hosts(hosts_path, true);     /* dann glob. HOSTS-Datei bearb.  */
          else
            return edit_hosts(file, false);          /* HOSTS-Datei im Pfad bearbeiten */
        }
        else if( strstr(file, PASSWORD_FILE) )       /* .passwd angegeben?             */
        { if( password_file_mode == PATH )           /* globale .passwd-Datei?         */
            return edit_passwd(passwd_path, true);   /* dann glob. PWD-Datei bearbeiten*/
          else
            return edit_passwd(file, false);         /* PWD-Datei im Pfad bearbeiten   */
        }
      }
      else if( !strcmp(par, ACCESS_GLOB_PAR) )       /* globale .<datei> bearbeiten    */
      { if( strstr(file, HOSTS_FILE) )               /* .hosts angegeben?              */
        { if( hosts_file_mode == GLOBAL              /* globale .hosts-Datei?          */
              || hosts_file_mode == PATH )           /* oder fester Pfad?              */
            return edit_hosts(hosts_path, true);     /* dann glob. HOSTS-Datei bearb.  */
        }
        else if( strstr(file, PASSWORD_FILE) )       /* .passwd angegeben?             */
        { if( password_file_mode == GLOBAL           /* globale .passwd-Datei?         */
              || password_file_mode == PATH )        /* oder feste .passwd-Datei?      */
            return edit_passwd(passwd_path, true);   /* dann glob. PWD-Datei bearbeiten*/
        }
      }
      else if( eqbpar(PWDEDITMODE, PWDSAVE) && strstr(file, PASSWORD_FILE) )
      { if( password_file_mode != PATH )             /* lokale .passwd-Datei?          */
          return save_passwd(file, false);           /* PWD-Datei im Pfad sichern      */
      }
      else if( eqbpar(HOSTSEDITMODE, HOSTSSAVE) && strstr(file, HOSTS_FILE) )
      { if( hosts_file_mode != PATH )                /* lokale .hosts-Datei?           */
          return save_hosts(file, false);            /* HOSTS-Datei im Pfad sichern    */
      }
      else if( !strcmp(clientgetpath, "/"SAVE_GLOBAL_ACC ) )
      { if( eqbpar(PWDEDITMODE, PWDGLOBSAVE) )
        { if( password_file_mode == GLOBAL           /* globale .passwd-Datei?         */
              || password_file_mode == PATH )        /* feste .passwd-Datei?           */
            return save_passwd(passwd_path, true);   /* dann glob. PWD-Datei sichern   */
        }
        else if( eqbpar(HOSTSEDITMODE, HOSTSGLOBSAVE) )
        { if( hosts_file_mode == GLOBAL              /* globale .hosts-Datei?          */
              || hosts_file_mode == PATH )           /* feste .hosts-Datei?            */
            return save_hosts(hosts_path, true);     /* dann glob. HOSTS-Datei sichern */
        }
      }
      LOG(4, "doput, SEITE_NICHT_GEFUNDEN 1.\n");
      send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                      SEITE_NICHT_GEFUNDEN_TEXT);
      return true;
    }
    if( auth_mode == BAD )
      return send_authorize(false);
                                                     /* Fehlerseite ausgeben           */
    LOG(4, "doput, SEITE_NICHT_GEFUNDEN 2.\n");
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  if( !read_flag )
    return send_method_not_allowed();

  if( test_access(file, user) )                      /* Berechtigung testen            */
  { if( file[strlen(file)-1] == '/' )                /* Dateiname nicht angegeben?     */
    { p = file+strlen(file);                         /* Ende des Dateinamens setzen    */
      strcpy(p, STD_PAGE1);                          /* Std-Seiten testen              */
      if( !stat64(file, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
      { send_moved_permanently(http_host ? http_host : hostname, clientgetpath, STD_PAGE1);
        return true;
      }
      strcpy(p, STD_PAGE2);
      if( !stat64(file, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
      { send_moved_permanently(http_host ? http_host : hostname, clientgetpath, STD_PAGE2);
        return true;
      }
      strcpy(p, STD_PAGE3);
      if( !stat64(file, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
      { send_moved_permanently(http_host ? http_host : hostname, clientgetpath, STD_PAGE3);
        return true;
      }
      *p = '\0';                                     /* STD_PAGE? löschen              */
      if( dirlist_flag )
      { if( send_http_header("text/html", "", true, 0, u2w_charset) )
          return true;
        if( headflag )                               /* Methode HEAD?                  */
          return false;
        http_head_flag = 2;
        return dirlist(file);
      }
    }
    else
    { LOG(4, "doput, file: %s.\n", file);
      if( !stat64(file, &stat_buf) )                 /* Ist Datei vorhanden?           */
      { LOG(15, "doput, stat - Datei gefunden, stat_buf.st_mode: %d, %d.\n",
            stat_buf.st_mode, (strcmp(par, EDIT_PAR) || edit_flag ));
#ifdef WITH_CGI
        if( NULL != strstr(file, CGI_PATH)  )        /* cgi-Skript?                    */
        { if( (S_IXUSR | S_IXGRP | S_IXOTH) & stat_buf.st_mode )
          { if( send_http_header("", "", false, 0, NULL) )  /* Header senden                */
              return true;
            LOG(5, "doput, query_string: %s.\n", query_string);
            if( *query_string )
              putenv(query_string);
            return doshell(file, par, false, false, false);  /* Fkt ausfuehren         */
          }
        }
        else
#endif
        if( (S_IFREG & stat_buf.st_mode) == S_IFREG     /* und normale Datei      */
                 && (strcmp(par, EDIT_PAR) || edit_flag ) )  /* ?&edit verboten        */
        { ret = false;
          LOG(13, "doput, Datei gefunden, edit_flag: %d, par: %s.\n", edit_flag, par);
          if( edit_flag > 1 || !strcmp(par, EDIT_PAR) )
          { LOG(23, "doput - nach edit_flag > 1 ...\n");
            if( 0 <= (hd_in = open64(file, O_RDONLY)) )  /* Datei oeffnen              */
            { LOG(23, "doput, nach open64(%s), h: %d.\n", file, hd_in);
              if( mktime(gmtime(&stat_buf.st_mtime)) > test_mod_time )
              {
 // Code fuer Range siehe unten
 // aber noch unfertig!
                { LOG(24, "doput, no range\n");
                  if( send_http_header(get_ct(file),
                                       httptime(gmtime(&stat_buf.st_mtime)),
                                       false, (LONGWERT)stat_buf.st_size, NULL) )
                    ret = true;
                  else if( !headflag )
                    ret = bin_put(hd_in, 0, -1);     /* alle anderen Dateien           */
                }
              }
              else
                ret = send_not_modified();
              close(hd_in);
              return ret;
            }
            else
            { LOG(3, "doput, open error on file: %s.\n", file);
              send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                              SEITE_NICHT_GEFUNDEN_TEXT);
              return true;
	          }
          }
          else if( test_ext(file, STD_EXT) )
          { u2w_mode = U2W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.u2w Datei verarbeiten        */
          }
          else if( test_ext(file, HTML_EXT) )
          { u2w_mode = U3W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.u3w Datei verarbeiten        */
          }
          else if( test_ext(file, U4W_EXT) )
          { u2w_mode = U4W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.u3w Datei verarbeiten        */
          }
          else if( test_ext(file, SHELL_EXT) )
          { u2w_mode = S2W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.s2w Datei verarbeiten        */
          }
          else if( test_ext(file, BIN_SHELL_EXT) )
          { u2w_mode = S3W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.s3w Datei verarbeiten        */
          }
          else if( test_ext(file, S4W_EXT) )
          { u2w_mode = S4W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.s2w Datei verarbeiten        */
          }
          else if( test_ext(file, S5W_EXT) )
          { u2w_mode = S5W_MODE;                     /* Auswertungsmodus der Zeile     */
            if( NULL != (ptr = fopen(file, "r")) )   /* Datei oeffnen                  */
              ret = u2w_put(ptr);                    /* *.s3w Datei verarbeiten        */
          }
          else if( !test_ext(file, STD_INC_EXT) )
          { if( 0 <= (hd_in = open64(file, O_RDONLY)) )  /* Datei oeffnen              */
            { if( mktime(gmtime(&stat_buf.st_mtime)) > test_mod_time )
              {
#ifdef RANGE_IST_NOCH_NICHT_FERTIG
 // Bei range muss auch IF-Range ausgewertet werden:
/*****************************
Range: bytes=3918-
If-Range: Wed, 24 Jun 2015 07:07:57 GMT
******************************/
                if( range )
                { char *p;
                  unsigned LONGWERT von, bis;

                  von = 0;
                  bis = -1;
                  p = range;
                  if( is_command_z(&p, "bytes=") )
                  { if( *p == '-' )
                    { p++;
                      skip_blanks(p);
                      if( *p )
                        bis = get_int_z(&p);
                    }
                    else if( *p )
                    { von = get_int_z(&p);
                      skip_blanks(p);
                      if( *p == '-' )
                      { p++;
                        skip_blanks(p);
                        bis = get_int_z(&p);
                      }
                    }
                  }
                  LOG(24, "doput, range - von: %ld, bis: %ld\n", von, bis);
                  if( von != 0 || bis != -1 )
                  { if( send_http_header(get_ct(file),
                                         httptime(gmtime(&stat_buf.st_mtime)),
                                         false, (LONGWERT)stat_buf.st_size, NULL) )
                      ret = true;
                    else if( !headflag )
                      ret = bin_put(hd_in, von, bis);  /* Datei mit Range              */
                  }
                  else
                  { if( send_http_header(get_ct(file),
                                         httptime(gmtime(&stat_buf.st_mtime)),
                                         false, (LONGWERT)stat_buf.st_size, NULL) )
                      ret = true;
                    else if( !headflag )
                      ret = bin_put(hd_in, 0, -1);   /* alle anderen Dateien           */
                  }
                }
                else
#endif
                { LOG(24, "doput, no range\n");
                  if( send_http_header(get_ct(file),
                                       httptime(gmtime(&stat_buf.st_mtime)),
                                       false, (LONGWERT)stat_buf.st_size, NULL) )
                    ret = true;
                  else if( !headflag )
                    ret = bin_put(hd_in, 0, -1);     /* alle anderen Dateien           */
                }
              }
              else
                ret = send_not_modified();
              close(hd_in);
              return ret;
            }
            else
            { send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                              SEITE_NICHT_GEFUNDEN_TEXT);
              return true;
            }
          }
          if( ptr == NULL )
          { if( errorpage )
            { if( *errorpage == '/' )
                strcpyn(zeile, errorpage, MAX_ZEILENLAENGE);
              else
              { strcpyn(zeile, file, MAX_ZEILENLAENGE);
                if( (p = strrchr(zeile, '/')) )
                { *(p+1) = '\0';
                  strcatn(zeile, errorpage, MAX_ZEILENLAENGE);
                }
              }
              if( strcmp(zeile, file) )
              { if( error_redirect_flag )
                  return send_moved_permanently(http_host ? http_host : hostname, errorpage, "");
                else
                  return doput(zeile, par, 0, read_flag, false, NULL);
              }
            }
            send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                            SEITE_NICHT_GEFUNDEN_TEXT);
              return true;
          }
          fclose(ptr);                               /* Datei schliessen               */
          LOG(2, "/doput.\n");
          return ret;
        }
      }
      LOG(17, "doput, nach stat.\n");
    }
  }
                                                     /* Fehlerseite ausgeben           */
  LOG(5, "doput - Errorpage.\n");
  if( errorpage )
  { if( *errorpage == '/' )
      strcpyn(zeile, errorpage, MAX_ZEILENLAENGE);
    else
    { strcpyn(zeile, file, MAX_ZEILENLAENGE);
      if( (p = strrchr(zeile, '/')) )
      { *(p+1) = '\0';
        strcatn(zeile, errorpage, MAX_ZEILENLAENGE);
      }
    }
    LOG(4, "doput, vor errorpage, zeile: %s.\n", zeile);
    if( strcmp(zeile, file) )
    { if( error_redirect_flag )
        return send_moved_permanently(http_host ? http_host : hostname, errorpage, "");
      else
        return doput(zeile, par, 0, read_flag, false, NULL);
    }
  }
  send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                  SEITE_NICHT_GEFUNDEN_TEXT);
  return true;
}

#endif  /* #ifdef WEBSERVER */
#ifdef INTERPRETER

/***************************************************************************************/
/* int doput(char *file)                                                               */
/*           char *file          : Datei, die gesendet werden soll                     */
/*     doput sendet die Datei file. Wenn es sich um eine *.u2w Datei handelt, dann     */
/*           wird sie nach HTML umgewandelt, *.s2w und *.s3w Dateien werden            */
/*           ausgewertet und das Ergebnis gesendet.                                    */
/***************************************************************************************/
int doput(char *file)
{ FILE *ptr;                                         /* einzulesende Datei             */
  int ret;                                           /* Rueckgabewert                  */
#ifdef _LARGEFILE64_SOURCE
  struct stat64 stat_buf;                            /* fuer stat-Aufruf               */
#else
  struct stat stat_buf;                              /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#define open64(a,b) open(a,b)
#endif


  LOG(1, "doput, file: %s.\n", file);

  if( (!stat64(file, &stat_buf)                      /* Ist Datei vorhanden?           */
      && (S_IFREG & stat_buf.st_mode) == S_IFREG) )  /* und normale Datei              */
  {
    if( test_ext(file, HTML_EXT) )
    { u2w_mode = U3W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.u3w Datei verarbeiten        */
      else
        ret = true;
    }
    else if( test_ext(file, U4W_EXT) )
    { u2w_mode = U4W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.u3w Datei verarbeiten        */
      else
        ret = true;
    }
    else if( test_ext(file, U5W_EXT) )
    { u2w_mode = U5W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.u3w Datei verarbeiten        */
      else
        ret = true;
    }
    else if( test_ext(file, SHELL_EXT) )
    { u2w_mode = S2W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.s2w Datei verarbeiten        */
      else
        ret = true;
    }
    else if( test_ext(file, BIN_SHELL_EXT) )
    { u2w_mode = S3W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.s3w Datei verarbeiten        */
      else
        ret = true;
    }
    else if( test_ext(file, S4W_EXT) )
    { u2w_mode = S4W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.s2w Datei verarbeiten        */
      else
        ret = true;
    }
    else if( test_ext(file, S5W_EXT) )
    { u2w_mode = S5W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.s3w Datei verarbeiten        */
      else
        ret = true;
    }
    else
    { u2w_mode = S2W_MODE;                           /* Auswertungsmodus der Zeile     */
      if( NULL != (ptr = fopen(file, "r")) )         /* Datei oeffnen                  */
        ret = u2w_put(ptr);                          /* *.s2w Datei verarbeiten        */
      else
        ret = true;
    }
    LOG(35, "/doput, ret: %d\n", ret);
    return ret;
  }
                                                     /* Fehlerseite ausgeben           */
  fprintf(stderr, _("File %s not found.\n"), file);
  return true;
}

#endif  /* #ifdef INTERPRETER */
