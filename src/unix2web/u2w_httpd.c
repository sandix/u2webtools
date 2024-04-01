/*************************************************/
/* File: u2w_httpd.c                             */
/* Auswertung der Anforderungen                  */
/* timestamp: 2016-03-27 20:33:24                */
/*************************************************/


#include "u2w.h"


#ifdef WEBSERVER


#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
// verhindert: warning: ‘boundary’ may be used uninitialized in this function [-Wmaybe-uninitialized]
#endif
#endif
/***************************************************************************************/
/* int do_httpd(void)                                                                  */
/*              return: 1 bei Fehler                                                   */
/*      do_httpd liest die Anforderung und wertet sie aus                              */
/***************************************************************************************/
int do_httpd(void)
{ char getfile[MAX_LEN_FILENAME];            /* Pfad der angeforderten Datei (server)  */
  char *par;                                 /* Parameter eines cgi Aufrufs            */
  char *boundary;                            /* Boundary-String                        */
  int multipart_flag;                        /* true bei multipart Content-Type        */
  time_t test_modified_time;                 /* Zeit aus If-Modified-String            */
  char *range;                               /* Range einer GET-Anforderung            */
  methodtype method;                         /* Vom Browser angeforderte Methode       */
  long nb;                                   /* gelesene Zeichen                       */
  int read_flag;                             /* true, Lesen durch .passwd oder .hosts  */
  int write_flag;                            /* true, Schreiben durch .passwd o. .hosts*/
  int access_flag;                           /* true, Access auf .passwd erlaubt       */
  int gzip_flag;                             /* true, wenn gzip im HTTP-Header erl.    */
  int keepalive_found;                       /* true, wenn keep-alive im HTTP-Header   */
  char *p, *q;
  char setuser[USER_LENGTH];                 /* User aus passwd-Datei                  */
  char userpfad[MAX_LEN_FILENAME];           /* Userpfad aus passwd-Datei              */
  char home[HOME_LENGTH];
  char *auth_digest;

  access_flag = read_flag = write_flag = true;
  par = clientgetpath = auth_basic = http_accept_language = range = auth_digest = "";
  keepalive_found = gzip_flag = test_modified_time = 0;

  LOG(1, "do_httpd, keepalive_flag: %d.\n", keepalive_flag);

  if( 0 >= (nb = read_http_header(&method, &par, &auth_digest, &test_modified_time,
                                  &range, &gzip_flag, &keepalive_found)) )
  { LOG(2, "/do_httpd, ERROR, nb: %ld.\n", nb);
    return 1;
  }

  if( !keepalive_found && !digestnum && keepalive_flag )
    keepalive_flag = 0;
  LOG(21, "do_httpd, keepalive_flag: %d, keepalive_found: %d.\n",
      keepalive_flag, keepalive_found);

  headflag = method == HEAD;

  if( *content_type )
  { if( (multipart_flag = is_command(content_type, "multipart/form-data")) )
    { if( NULL != (p = strcasestr_hs(content_type, "boundary=")) )
        boundary = p+9;
      else
      { boundary = NULL;
        multipart_flag = false;
      }
    }
  }
  else
    multipart_flag = false;

  if( NULL != (clientgetfile = strrchr(clientgetpath, '/')) )
    clientgetfile++;
  else
    clientgetfile = clientgetpath;

                                        /* Authorize - String bestimmen                */
  auth_mode = NONE;
  LOG(7, "do_httpd, digestnum: %d.\n", digestnum);
  if( digestnum )
  { if( auth_digest )
    { char md5sum[MD5_DIGEST_LENGTH*2+1];
      char clientnonce[MAX_LEN_DIGEST_NONCE];

      LOG(11, "do_httpd - Authorization: Digest: %s.\n", auth_digest);
      uri = "";
      p = auth_digest;
      while( *p && auth_mode != BAD )
      { switch( *p++ )
        { case 'u': if( str_starts_z(&p, "sername=\"") )
                    { for( q = user; *p && *p != '"' && q-user < MAX_LEN_AUTHORIZE-1;
                           *q++ = *p++ )
                        ;
                      *q = '\0';
                      if( *p )
                        p++;
                    }
                    else if( str_starts_z(&p, "ri=\"") )
                    { q = p;
                      while( *p && *p != '"' )
                        p++;
                      if( *p )
                        *p++ = '\0';
                      uri = store_str(q);
                    }
                    while( *p && (*p == ',' || *p == ' ') )
                      p++;
                    break;
          case 'r': if( str_starts_z(&p, "ealm=\"") )
                    { if( !str_starts_z(&p, realm) || *p != '"' )
                      { auth_mode = BAD;
                        break;
                      }
                    }
                    else if( str_starts_z(&p, "esponse=\"") )
                    { for( q = response;
                           *p && *p != '"' && q-response < MD5_DIGEST_LENGTH*2;
                           *q++ = *p++ )
                        ;
                      *q = '\0';
                    }
                    while( *p && (*p == ',' || *p == ' ' || *p == '"') )
                      p++;
                    break;
          case 'n': if( str_starts_z(&p, "once=\"") )
                    { for( q = clientnonce;
                           *p && *p != '"' && q-clientnonce < MAX_LEN_DIGEST_NONCE;
                           *q++ = *p++ )
                        ;
                      *q = '\0';
                      if( *p )
                        p++;
                    }
                    while( *p && (*p == ',' || *p == ' ' || *p == '"') )
                      p++;
                    break;
          default:  while( *p && *p != ',' && *p != '"' )
                      p++;
                    if( *p == '"' )
                    { while( *p && *p != '"' )
                        p++;
                      while( *p && *p != ',' )
                        p++;
                    }
                    while( *p && (*p == ',' || *p == ' ') )
                      p++;
        }
      }
      LOG(7, "do_httpd - digest, browser uri: %s.\n", uri);
      LOG(7, "do_httpd - digest, browser user: %s.\n", user);
      LOG(7, "do_httpd - digest, aut_mode: %d.\n", auth_mode);
      LOG(7, "do_httpd - digest, server nonce: %s.\n", servernonce);
      LOG(7, "do_httpd - digest, client nonce: %s.\n", clientnonce);
      if( auth_mode != BAD )
      { LOG(7, "do_httpd - digest, auth_mode != BAD\n");
        p = auth_digest;
        q = methodstr;
        while( *q )
          *p++ = *q++;
        *p++ = ':';
        q = uri;
        while( *q )
          *p++ = *q++;
        calc_md5sum(md5sum, (unsigned char *)auth_digest, p-auth_digest);
        LOG(7, "do_httpd - digest, MD5(method:digestURI): %s.\n", md5sum);
        p = auth_digest;
        auth_mode = get_pwd_md5(getfile, user, &read_flag, &write_flag, &access_flag,
                                setuser, userpfad, &p);
        if( auth_mode == AUTH_MD5 )
        { LOG(7, "do_httpd - digest, server md5: %s.\n", auth_digest);
          *p++ = ':';
          q = clientnonce;
          while( *q )
            *p++ = *q++;
          *p++ = ':';
          q = md5sum;
          while( *q )
            *p++ = *q++;
          calc_md5sum(md5sum, (unsigned char *)auth_digest, p-auth_digest);
          LOG(7, "do_httpd - digest, browser response: %s.\n", response);
          LOG(7, "do_httpd - digest, server response : %s.\n", md5sum);
          if( strcmp(response, md5sum) )
            auth_mode = BAD;
          else if( strcmp(clientnonce, servernonce) )
          {
#ifdef WITH_HTTPS
            if( ssl_mode )
              fcntl(sh, F_SETFL, fcntlmode);
#endif
            send_authorize(true);
            return 0;
          }
          else
            auth_mode = AUTH;
        }
      }
      if( auth_mode == BAD )
      {
#ifdef WITH_HTTPS
        if( ssl_mode )
          fcntl(sh, F_SETFL, fcntlmode);
#endif
        send_authorize(false);
        return 0;
      }
    }
    else
    {
#ifdef WITH_HTTPS
      if( ssl_mode )
        fcntl(sh, F_SETFL, fcntlmode);
#endif
      send_authorize(false);
      return 0;
    }
  }
  else if( *auth_basic )
  {                                            /* User und PWD vom Browser erhalten    */
    strcpyn(user, auth_basic, MAX_LEN_AUTHORIZE);
    decode_base64((unsigned char *)user);      /* und decodieren, um aktuellen         */
    if( (p = strchr(user, ':')) != NULL )
    { *p = '\0';
      passwd =  p+1;                           /* Pwd merken                           */
    }
    else
      passwd = "";
    auth_mode = AUTH;
  }
  else if( sec_flag )                          /* Muss User, PWD angegeben sein?       */
  {
#ifdef WITH_HTTPS
    if( ssl_mode )
      fcntl(sh, F_SETFL, fcntlmode);
#endif
    send_authorize(false);                     /* nicht angegeben, erfragen            */
    return !keepalive_flag;
  }
  else                                         /* Kein Username (Anmeldung) vom Browser*/
  { auth_mode = NONE;
    *user = '\0';
  }

  LOG(40, "do_httpd, vor Pfad zusammenbauen, auth_mode: %d.\n", auth_mode);
                                        /* Pfad der Anforderung zusammenbauen          */

  strcpyn(getfile, method == PUT ? put_home : get_home, MAX_LEN_FILENAME);
  if( home_host_flag )
  { strcatn(getfile, "/", MAX_LEN_FILENAME);
    if( http_host && *http_host
        && (*http_host != '.' || *(http_host+1) != '.' || *(http_host+2))
        && !strchr(http_host, '/') )
      strcatnc(getfile, http_host, ':', MAX_LEN_FILENAME);
    else
      strcatn(getfile, http_host_unknown, MAX_LEN_FILENAME);
  }
                                            /* Home des Servers                  */
  if( verzeichnis_mode == VZ_HOME                 /* User bestimmt HOME?               */
      || verzeichnis_mode == VZ_USER )
  { strcatn(getfile, "/", MAX_LEN_FILENAME);

    if( *user )                                   /* User eingeloggt?                  */
    { if( verzeichnis_mode == VZ_HOME )           /* HOME aus /etc/passwd?             */
      { char *hs;

        hs = get_user_home(user);                 /* ja                                */
        if( hs == NULL )                          /* ungültiger Benutzer?              */
        {
#ifdef WITH_HTTPS
          if( ssl_mode )
            fcntl(sh, F_SETFL, fcntlmode);
#endif

          send_authorize(false);                  /* Abweisen                          */
          return !keepalive_flag;
        }
        else
          strcpyn(getfile, hs, MAX_LEN_FILENAME); /* HOME des Users anhängen           */
      }
      else
       strcatn(getfile, user, MAX_LEN_FILENAME);  /* HOME == Username anhängen         */
    }
    else
     strcatn(getfile, everybody_path, MAX_LEN_FILENAME);  /* EVERYBODY anhängen        */
  }
  LOG(40, "do_httpd, vor Dateiname an Pfad anhängen, auth_mode: %d.\n", auth_mode);
                                        /* Dateiname an Pfad anhängen                  */
  if( *clientgetpath != '/' )
    strcatn(getfile, "/", MAX_LEN_FILENAME);
  if( *clientgetpath )
    strcatn(getfile, clientgetpath, MAX_LEN_FILENAME);   /* angeforderte Datei         */

  if( (par && !strcmp(par, ACCESS_GLOB_PAR))    /* globale .<datei> bearbeiten         */
      || !strcmp(clientgetpath, SAVE_GLOBAL_ACC) ) /* oder speichern                   */
  { if( hosts_file_mode == GLOBAL )
      hosts_file_mode = PATH;
    if( password_file_mode == GLOBAL )
      password_file_mode = PATH;
  }
#ifdef WITH_MMAP
  if( status_mode & (STAT_REQUEST|STATDIFF_REQUEST) )
    status_counter->requests++;
#endif
  if( !digestnum )
  { if( *user )
    { if( password_file_mode == FIX )
      { if( strcmp(fix_user, user) || strcmp(fix_pwd, passwd) )
          auth_mode = BAD;
        else
          auth_mode = AUTH;
      }
      else if( password_file_mode == FIX64 )
        auth_mode =  strcmp(fix_user, auth_basic) ? BAD : AUTH;
      else if( password_file_mode != NOACCESSFILE )   /* .passwd-Datei                 */
      { auth_mode = test_passwd(getfile, auth_basic, &read_flag, &write_flag, &access_flag,
                                setuser, userpfad);
                                               /* und Rechte über .passwd-Datei testen */
        LOG(4, "do_httpd, set_user_mode: %d, setuser: %s.\n", set_user_mode, setuser);
        LOG(5, "do_httpd, read_flag: %d.\n", read_flag);
#ifndef CYGWIN
        LOG(25, "do_httpd, auth_mode: %d, setuser: %s.\n", auth_mode, setuser);
        if( (auth_mode != AUTH || !*setuser) && defuser ) /* kein setuser angegeben,   */
          strcpyn(setuser, defuser, USER_LENGTH);  /* dann defuser                     */
        LOG(15, "do_httpd, set_user: %s.\n", setuser);
        if( set_user_mode == USERSETUSER         /* soll user gewechselt werden?       */
            && (setuser[0] != NO_SETUSER_USER_CHAR || setuser[1] != '\0') )
                                                 /* setuser != "-"                     */
        { LOG(25, "do_httpd, vor set_user(...)\n");
          if( set_user(setuser, false) )         /* auf defuser aus .passwd wechseln   */
            return 1;
        }
#endif
        LOG(35, "do_httpd - nach ifndef CYGWIN\n");
      }
      else if( set_user_mode == USERSETUSER )
      { if( set_user_pass(user, passwd) )        /* User wechseln                      */
        {
#ifndef CYGWIN
          if( defuser && set_user(defuser, false) )
                                                 /* User kann nicht gesetzt werden,    */
                                                 /* dann defuser                       */
            return 1;
#endif
          auth_mode = BAD;                       /* nicht autorisiert merken           */
        }
      }
      LOG(35, "do_httpd - nach ifndef CYGWIN - 2\n");
    }
    else
    { if( password_file_mode != NOACCESSFILE )
      { if( BAD == test_passwd(getfile, NO_USER_AUTH_B64, &read_flag, &write_flag,
                               &access_flag, setuser, userpfad) )
          auth_mode = BAD;
                                               /* und Rechte über .passwd-Datei testen */
                                               /* (Eintrag für nicht angemeldet)       */
        LOG(5, "do_httpd, read_flag: %d, auth_mode: %d.\n", read_flag, auth_mode);
  
#ifndef CYGWIN
        if( !*setuser && defuser )
          strcpyn(setuser, defuser, USER_LENGTH);
        if( set_user_mode == USERSETUSER
            && setuser[0] != '-' && set_user(defuser, false) )
                                                 /* auf defuser aus .passwd wechseln   */
                                                 /* (Eintrag für nicht angemeldet)     */
          return 1;
#endif
      }
#ifndef CYGWIN
      else if( set_user_mode == USERSETUSER && defuser )
      { if( set_user(defuser, false) )           /* kein User vom Browser, dann defuser*/
          return 1;
      }
#endif
    }
  }

  if( num_execute_paths )
  { int i;

    LOG(21, "do_httpd - executes, clientgetpath: %s\n", clientgetpath);
    executeall = NULL;
    for( i = 0; i < num_execute_paths; i++ )
    { LOG(21, "do_httpd - executes, i: %d, executepath[i]: %s\n", i, executepath[i]);
      if( path_starts(clientgetpath + (*executepath[i] == '/' ? 0 : 1), executepath[i]) )
      { executeall = executex2w[i];
        break;
      }
    }
  }

  if( executeall )
  { if( auth_mode == BAD )                     /* User + PWD falsch                    */
    {
#ifdef WITH_HTTPS
      if( ssl_mode )
        fcntl(sh, F_SETFL, fcntlmode);
#endif
      send_authorize(false);                   /* nicht angegeben, erfragen            */
      return !keepalive_flag;
    }

    if( logflag & LOGCONNECTIONLONG )
      connectionlogging(clientip);

    if( method == PUT )
      read_content();
    else if( method == POST && multipart_flag )  /* POST Daten einlesen                */
    { LOG(16, "do_httpd - method == POST, get_multiparts.\n");
      get_multiparts(boundary);                  /* Parameter bestimmen                */
    }
    else if( *content_type )
    { if( strcmp(content_type, "application/x-www-form-urlencoded") )
        read_content();
      else
      { if( 0 > (nb = read_http(&par, 0)) )          /* Content-Block lesen             */
        { if( nb == -2 )
          { send_error_page(REQUEST_TOO_LARGE_NUM, REQUEST_TOO_LARGE_DESC, "",
                            REQUEST_TOO_LARGE_TEXT);
            return !keepalive_flag;
          }
          par = NULL;                                /* kein Content-Block              */
        }
        else
          *(par+nb) = '\0';
        LOG(17, "do_httpd - Content-Block gelesen.\n");
        if( par )
        { LOG(17, "do_httpd - par: %s.\n", par);
          get_parwerte(par);
        }
      }
    }
    else if( par )
    { LOG(17, "do_httpd - par: %s.\n", par);
      get_parwerte(par);
    }

#ifdef WITH_HTTPS
    if( ssl_mode )
      fcntl(sh, F_SETFL, fcntlmode);
#endif
    if( *executeall == '/' )
      strcpyn(getfile, executeall, MAX_LEN_FILENAME);
    else
    { strcpyn(getfile, method == PUT ? put_home : get_home, MAX_LEN_FILENAME);
                                                     /* neu zusammenbauen              */
      strcatn(getfile, "/", MAX_LEN_FILENAME);
      strcatn(getfile, executeall, MAX_LEN_FILENAME);       /* angeforderte Datei      */
    }
    strcpyn(home, getfile, HOME_LENGTH);
    if( NULL != (p = strrchr(home, '/')) )
      *p = '\0';

    chdir(home);                            /* Verzeichnis setzen, fuer Programmaufrufe */

    http_head_flag = 0;
    doput(executeall, par ? par : "", test_modified_time, read_flag, access_flag, range);
                                               /* Ausgabe erzeugen und senden          */
    LOG(11, "do_httpd, nach doput(1), http_head_flag: %d.\n", http_head_flag);
    if( http_head_flag & 3 )
      send_last_chunk();

    if( putdata )
    { unlink(putdata);
      putdata = NULL;
    }

    return !keepalive_flag;
  }

  LOG(25, "do_httpd - vor verzeichnis_mode == VZ_PWD\n");
  if( verzeichnis_mode == VZ_PWD )                /* Home aus .passwd, dann Pfad       */
  { strcpyn(getfile, method == PUT ? put_home : get_home, MAX_LEN_FILENAME);
                                                     /* neu zusammenbauen              */
    strcatn(getfile, "/", MAX_LEN_FILENAME);
    if( userpfad[0] == '.' || !userpfad[0] )
      strcatn(getfile, everybody_path, MAX_LEN_FILENAME);
    else
      strcatn(getfile, userpfad, MAX_LEN_FILENAME);
    if( *clientgetpath != '/' )
      strcatn(getfile, "/", MAX_LEN_FILENAME);
    if( *clientgetpath )
      strcatn(getfile, clientgetpath, MAX_LEN_FILENAME);      /* angeforderte Datei       */
  }

  LOG(25, "do_httpd - vor test_hosts\n");
  if( hosts_file_mode != NOACCESSFILE
      && BAD == test_hosts(getfile, &read_flag, &write_flag, &access_flag) )
                                               /* Zugriff über .hosts geregelt?        */
  {                                            /* Rechner hat keinen Zugriff           */
    return 1;
  }

  LOG(3, "do_httpd, nach VZ_PWD, getfile: %s\n", getfile);
  LOG(5, "do_httpd - hosts_file_mode, read_flag: %d.\n", read_flag);

  if( auth_mode == BAD )                       /* User + PWD falsch                    */
  { LOG(50, "do_httpd, send_authorize...\n");
#ifdef WITH_HTTPS
    if( ssl_mode )
      fcntl(sh, F_SETFL, fcntlmode);
#endif
    send_authorize(false);                     /* nicht angegeben, erfragen            */
    return !keepalive_flag;
  }

  strcpyn(home, getfile, HOME_LENGTH);
  if( NULL != (p = strrchr(home, '/')) )
    *p = '\0';

  chdir(home);                             /* Verzeichnis setzen, fuer Programmaufrufe */

  LOG(3, "do_httpd, par: %s.\n", par ? par : "(NULL)");
  LOG(5, "do_httpd - chdir, read_flag: %d, method: %d.\n", read_flag, method);

  switch( method )
  { case NO_METHOD:
#ifdef WITH_HTTPS
                    if( ssl_mode )
                      fcntl(sh, F_SETFL, fcntlmode);
#endif
                    if( logflag & LOGCONNECTIONLONG )
                      connectionlogging(clientip);
                    send_method_not_allowed();
                    break;
    case OPTIONS:
#ifdef WITH_HTTPS
                    if( ssl_mode )
                      fcntl(sh, F_SETFL, fcntlmode);
#endif
                    if( no_options_flag )
                      send_method_not_allowed();
                    else
                      send_options(read_flag, read_flag, write_flag && upload_flag);
                    if( logflag & LOGCONNECTIONLONG )
                      connectionlogging(clientip);
                    break;
    case TRACE:
#ifdef WITH_HTTPS
                    if( ssl_mode )
                      fcntl(sh, F_SETFL, fcntlmode);
#endif
                    if( no_trace_flag )
                      send_method_not_allowed();
                    else
                    { send_trace(httpheader, strlen(httpheader));
                      if( http_head_flag & 1 )
                        send_last_chunk();
                    }
                    if( logflag & LOGCONNECTIONLONG )
                      connectionlogging(clientip);
                    break;
    case PUT:
#ifdef WITH_HTTPS
                    if( ssl_mode )
                      fcntl(sh, F_SETFL, fcntlmode);
#endif
                    if( upload_flag && write_flag )
                    { if( auth_mode == AUTH || upload_noauth_flag )
                        do_upload(getfile);
                      else
                        send_authorize(false);
                    }
                    else
                      send_method_not_allowed();
                    if( logflag & LOGCONNECTIONLONG )
                      connectionlogging(clientip);
                    break;
    case GET:
#ifdef WITH_MMAP
      LOG(17, "do_http - GET - getfile: %s, file: %s, par: %s.\n",
          getfile, clientgetpath, par);
      if( status_mode && *clientgetpath == '/' && *(clientgetpath+1) == '\0'
          && par && *par == '&' )
      { if( !strcmp(par+1, STATUS_PAR) )
          return send_status(false);
        else if( (status_mode & STAT_ENABLE_FLUSH) && !strcmp(par+1, STATUS_FLUSH_PAR) )
          return send_status(true);
      }
#endif
    case POST:
    case HEAD:
      LOG(5, "do_httpd - case, read_flag: %d.\n", read_flag);
      if( !read_flag && !access_flag )         /* kein read und kein access            */
      {
#ifdef WITH_HTTPS
        if( ssl_mode )
          fcntl(sh, F_SETFL, fcntlmode);
#endif
        send_method_not_allowed();
      }
      else
      { if( method == POST )                   /* POST                                 */
        { LOG(16, "do_httpd - method == POST, multipart_flag: %d.\n", multipart_flag);
          if( multipart_flag )                 /* Mit multi-part/form-data?            */
            get_multiparts(boundary);          /* Parameter bestimmen                  */
          else if( *content_type
                   && strcmp(content_type, "application/x-www-form-urlencoded") )
            read_content();
          else
          { if( 0 > (nb = read_http(&par, 0)) )     /* Content-Block lesen             */
            { if( nb == -2 )                        /* Content zu gross                */
              { send_error_page(REQUEST_TOO_LARGE_NUM, REQUEST_TOO_LARGE_DESC, "",
                                REQUEST_TOO_LARGE_TEXT);
                return !keepalive_flag;
              }
              return 1;                             /* kein Content-Block, dann Ende   */
            }
            *(par+nb) = '\0';
            LOG(17, "do_httpd - Content-Block gelesen.\n");
            if( par )
              get_parwerte(par);
          }
          LOG(7, "do_httpd, read_flag: %d.\n", read_flag);
        }
        else if( par )
        {
#ifdef WITH_CGI
          query_string = store_query_str(par);
#endif
          get_parwerte(par);
          LOG(7, "do_httpd - !POST.\n");
          if( str_starts(par, NEW_LOGIN) )     /* Neue Anmeldung auf neuen User?       */
          { LOG(4, "do_httpd, parwert(" LOGIN_PAR "): %s, user: %s.\n",
                parwert(LOGIN_PAR, HP_BROWSER_LEVEL), user);
            keepalive_flag = false;
#ifdef WITH_HTTPS
            if( ssl_mode )
              fcntl(sh, F_SETFL, fcntlmode);
#endif
            if( !(p=parwert(LOGIN_PAR, HP_BROWSER_LEVEL))
                || strcmp(p, user) )
                                                        /* schon richtigier User?      */
              return send_authorize(false);
            else
              return send_moved_permanently(http_host ? http_host : hostname,
                                            clientgetpath, "");
          }
        }
#ifdef WITH_CGI
        else
          query_string = "";
#endif

        if( logflag & LOGCONNECTIONLONG )
          connectionlogging(clientip);

#ifdef WITH_HTTPS
        if( ssl_mode )
          fcntl(sh, F_SETFL, fcntlmode);
#endif
        http_head_flag = 0;
        doput(getfile, par, test_modified_time, read_flag, access_flag, range);
                                               /* Ausgabe erzeugen und senden          */
        LOG(11, "do_httpd, nach doput(2), http_head_flag: %d.\n", http_head_flag);
        if( http_head_flag & 1 )
          send_last_chunk();
      }
  }
  LOG(2, "/do_httpd\n");
  return !keepalive_flag;
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif

#else  /* #ifdef WEBSERVER */

/***************************************************************************************/
/* int do_httpd(int options, int argc, char **argv)                                    */
/*               int options: ausgewertete Optionen                                    */
/*               int argc: argc von main()                                             */
/*               char **argv: argv von main()                                          */
/*              return: 1 bei Fehler                                                   */
/*      do_httpd liest die Anforderung und wertet sie aus                              */
/***************************************************************************************/
int do_httpd(int options, int argc, char **argv)
{
  LOG(1, "do_httpd, argv[options+1]: %s\n", argv[options+1]);

  get_parwerte(options+2, argc, argv);

  clientgetpath = argv[options+1];
  if( NULL != (clientgetfile = strrchr(clientgetpath, '/')) )
    clientgetfile++;
  else
    clientgetfile = clientgetpath;

  return doput(argv[options+1]);                         /* Ausgabe erzeugen und senden*/

}
#endif  /* #else #ifdef WEBSERVER */
