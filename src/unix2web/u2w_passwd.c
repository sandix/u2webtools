/****************************************************************/
/* File: u2w_passwd.c                                           */
/* Funktionen zur .passwd Datei                                 */
/* timestamp: 2016-03-13 00:16:10                               */
/****************************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* calc_md5sum(char *md5sum, unsigned char *s, long n)                                 */
/*             char *md5sum:     MD5-Summe wird hier abgelegt                          */
/*             unsigned char *s: String, aus dem die MD5-Summe berechnet wird          */
/*             long n:           Länge des Strings s                                   */
/***************************************************************************************/
void calc_md5sum(char *md5sum, unsigned char *s, long n)
{ unsigned char *p, md5digest[MD5_DIGEST_LENGTH];
  MD5_CTX ctx;
  int i, d1, d2;

  MD5_Init(&ctx);
  MD5_Update(&ctx, s, n);
  MD5_Final(md5digest, &ctx);
  for( i = 0, p = (unsigned char *)md5sum; i < MD5_DIGEST_LENGTH; i++ )
  { d1 = (md5digest[i] & 0xF0) >> 4;
    d2 = md5digest[i] & 0xF;
    if( d1 > 9 )
      *p++ = d1 - 10 + 'a';
    else
      *p++ = d1 + '0';
    if( d2 > 9 )
      *p++ = d2 - 10 + 'a';
    else
      *p++ = d2 + '0';
  }
  *p = '\0';
}


/***************************************************************************************/
/* auth_type test_pwd_file(char *pd, char *auth, int *r_flag, int *w_flag, int *a_flag,*/
/*                   char *setuser, char *userpfad)                                    */
/*                   char *pd: Pfad der .passwd-Datei                                  */
/*                   char *auth: Authorize-String, wie vom Browser bekommen            */
/*                   int *r_flag: Lesen im Verzeichnis erlaubt                         */
/*                   int *w_flag: Schreiben im Verzeichnis erlaubt                     */
/*                   int *a_flag: Ändern der .passwd-Datei erlaubt                     */
/*                   char *setuser: User, auf den gewechselt werden soll, NULL, wenn   */
/*                                  setuser nicht übergeben werden soll                */
/*                   char *userpfad: Pfad, in den gewchselt werden soll, NULL, wenn    */
/*                                   userpfad nicht übergebe werden soll               */
/*                   return: true, Eintrag gefunden                                    */
/*     test_ppwd_file bestimmt anhand der Datei .passwd die Zugriffsrechte             */
/***************************************************************************************/
auth_type test_pwd_file(char *pd, char *auth, int *r_flag, int *w_flag, int *a_flag,
                        char *setuser, char *userpfad)
{ FILE *ptr;                                     /* ptr auf ACCESS-Datei               */
  char *z;                                       /* eingelesene Zeilen aus passwd-Dat. */
  struct stat stat_buf;                          /* Zum Test, ob passwd-Datei vorhanden*/
  char a[MAX_AUTH_LEN];                          /* authorize aus passwd-Datei         */
  char r[MAX_ACC_LEN];                           /* access-Zeichen in passwd-Datei     */
  int all_user_flag = false;                     /* true; alle User sind erlaubt       */
  int all_user_a_flag, all_user_r_flag, all_user_w_flag;
  char all_user_setuser[USER_LENGTH];
  char all_user_userpfad[MAX_LEN_FILENAME];
  int af, rf, wf;
  auth_type auth_ret;
  char *dp, md5sum[MD5_DIGEST_LENGTH*2+1];

  calc_md5sum(md5sum, (unsigned char *)auth, strlen(auth));

  LOG(1, "test_pwd_file, pd: %s, auth: %s.\n", pd, auth);

  if( !stat(pd, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
  { if( NULL != (ptr = fopen(pd, "r")) )         /* passwd-Datei oeffnen               */
    { LOG(4, "test_pwd_file, pwd_datei offen.\n");
      while( !feof(ptr) )
      { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )   /* Zeile einlesen          */
          break;

        LOG(4, "test_passwd, zeile: %s.\n", zeile);

        z = zeile;
        strcpyn_l(r, &z, MAX_ACC_LEN);            /* access nach r                     */
        strcpyn_l(a, &z, MAX_AUTH_LEN);           /* Authorize nach a                  */
        if( !strcmp(a, GLOB_USER_AUTH_B64) )      /* globale Passwd einlesen?          */
        { if( (password_file_mode == GLOBAL || password_file_mode == GLOBAL_X)
              && pd != passwd_path )
          { rf = wf = af = true;
            auth_ret = test_pwd_file(passwd_path, auth, &rf, &wf, &af, setuser,
                              userpfad);
            if( auth_ret == AUTH )
            { *a_flag = *a_flag && af;
              *r_flag = *r_flag && rf;
              *w_flag = *w_flag && wf;
              return AUTH;
            }
            else if( auth_ret == AUTH_ALL_USER )
            { all_user_flag = true;
              all_user_a_flag = af;
              all_user_r_flag = rf;
              all_user_w_flag = wf;
              if( setuser )
                strcpyn(all_user_setuser, setuser, USER_LENGTH);
              if( userpfad )
                strcpyn(all_user_userpfad, userpfad, MAX_LEN_FILENAME);
            }
          }
          continue;
        }
        if( !strcmp(a, ALL_USER_AUTH_B64) )       /* "*" in Passwd-Datei               */
        { all_user_flag = true;
          all_user_a_flag = strchr(r, 'a') != NULL;
          all_user_r_flag = strchr(r, 'r') != NULL;
          all_user_w_flag = strchr(r, 'w') != NULL;
          if( setuser )
          { strcpyn_l(all_user_setuser, &z, USER_LENGTH);
            if( all_user_setuser[0] == '.' )
              all_user_setuser[0] = '\0';
          }
          else
            skip_next_blanks(z);
          if( userpfad )
            strcpyn_l(all_user_userpfad, &z, MAX_LEN_FILENAME);
          continue;
        }
        if( !strcmp(a, auth)
            || (NULL != (dp = strchr(a, ':')) && !strcmp(dp+1, md5sum)) )
                                                 /* mit Anmeldung vergleichen          */
        { fclose(ptr);                           /* gefunden, ACCESS-Datei schliessen  */
          *a_flag = *a_flag && (strchr(r, 'a') != NULL);
          *r_flag = *r_flag && (strchr(r, 'r') != NULL);
          *w_flag = *w_flag && (strchr(r, 'w') != NULL);
          if( setuser )
          { strcpyn_l(setuser, &z, USER_LENGTH); /* User nach user                     */
            if( *setuser == '.' )
              *setuser = '\0';
          }
          else
            skip_next_blanks(z);
          if( userpfad )
            strcpyn_l(userpfad, &z, MAX_LEN_FILENAME);

          return AUTH;                           /* Eintrag gefunden                   */
        }
      }
      fclose(ptr);
      if( all_user_flag && strcmp(auth, NO_USER_AUTH_B64) )
      { *a_flag = *a_flag && all_user_a_flag;
        *r_flag = *r_flag && all_user_r_flag;
        *w_flag = *w_flag && all_user_w_flag;
        if( setuser )
          strcpyn(setuser, all_user_setuser, USER_LENGTH);
        if( userpfad )
          strcpyn(userpfad, all_user_userpfad, MAX_LEN_FILENAME);
        return AUTH;
      }
    }
    return BAD;                                  /* Eintrag nicht gefunden             */
  }
  return NONE;                                   /* .passwd nicht gefunden             */
}


/***************************************************************************************/
/* auth_type test_passwd(char *pfad, char *auth, int *r_flag, int *w_flag, int *a_flag,*/
/*                       char *setuser, char *userpfad)                                */
/*                       char *pfad: Pfad der angeforderten Seite                      */
/*                       char *auth: Authorize-String, wie vom Browser bekommen        */
/*                       int *r_flag: Lesen im Verzeichnis erlaubt                     */
/*                       int *w_flag: Schreiben im Verzeichnis erlaubt                 */
/*                       int *a_flag: Ändern der .passwd-Datei erlaubt                 */
/*                       char *setuser: User, auf den gewechselt werden soll,          */
/*                                      NULL, wenn setuser nicht übergeben werden soll */
/*                       char *userpfad: Pfad, in den gewchselt werden soll, NULL,     */
/*                                       wenn userpfad nicht übergebe werden soll      */
/*                       return: BAD: user+pwd falsch, AUTH: user*pwd richtig,         */
/*                               NONE: keine .passwd-Datei                             */
/*           test_passwd bestimmt anhand der Datei .passwd die Zugriffsrechte          */
/***************************************************************************************/
auth_type test_passwd(char *pfad, char *auth, int *r_flag, int *w_flag, int *a_flag,
                      char *setuser, char *userpfad)
{ char *p;
  char pwddat[MAX_DIR_LEN];                      /* Aufbau des passwd-Pfades+Datei     */
  auth_type ret;

  LOG(1, "test_passwd, pfad: %s, auth: %s.\n", pfad, auth);

  if( password_file_mode == NOACCESSFILE )
  { LOG(2, "/test_passwd, ret: NONE.\n");
    return NONE;
  }
  else if( password_file_mode == PATH )
    ret = test_pwd_file(passwd_path, auth, r_flag, w_flag, a_flag, setuser, userpfad);
  else
  { strcpyn(pwddat, pfad, MAX_DIR_LEN);          /* Pfad, der angefordert wird         */

    if( NULL == (p = strrchr(pwddat, '/')) )     /* letzten '/'                        */
      return BAD;

    strcpyn(p+1, PASSWORD_FILE, MAX_DIR_LEN+pwddat-p-1); /* Pfad+Name der ACCESS-Datei */
    LOG(3, "test_passwd, pwddat: %s.\n", pwddat);

    ret = test_pwd_file(pwddat, auth, r_flag, w_flag, a_flag, setuser, userpfad);

    if( (ret != AUTH && password_file_mode == GLOBAL_X)
        || (ret == NONE && password_file_mode == GLOBAL) )
      ret = test_pwd_file(passwd_path, auth, r_flag, w_flag, a_flag, setuser, userpfad);
  }

  if( ret == AUTH )
  { LOG(2, "/test_passwd, ret: AUTH.\n");
    return AUTH;                                 /* Zugriff gewaehrt zurueck           */
  }
  else if( ret == NONE )
  { *r_flag = *r_flag && ( password_file_mode == NORMAL);
    *w_flag = *w_flag && ( password_file_mode == NORMAL);
    *a_flag = *a_flag && ( password_file_mode == NORMAL);
    LOG(2, "/test_passwd, ret: %s.\n", password_file_mode == NORMAL ? "NONE" : "BAD");
    return password_file_mode == NORMAL ? NONE : BAD; /* keine .passwd, aus flag       */
  }
  else
  { *r_flag = *w_flag = *a_flag = false;
    LOG(2, "/test_passwd, ret: BAD.\n");
    return BAD;                                  /* kein Zugriff                       */
  }
}


/***************************************************************************************/
/* void strcpy_l_z(char **out, char **in)                                              */
/*                char **out: Ausgabezeile                                             */
/*                char **in: Eingabezeile                                              */
/*                long n   : Maximal n Zeichen                                         */
/*      strcpy_l kopiert in nach out bis ein Leerzeichen, Tab oder \n gefunden wird    */
/***************************************************************************************/
inline void strcpy_l_z(char **out, char **in)
{
  while( **in && **in != ' ' && **in != '\t' && **in != '\n' && **in != '\r' )
    *(*out)++ = *(*in)++;
}


/***************************************************************************************/
/* auth_type get_pwd_md5_file(char *pd, char *user, int *r_flag, int *w_flag,          */
/*                          int *a_flag, char *setuser, char *userpfad, char **md5sum) */
/*                 char *pd: Pfad der .passwd-Datei                                    */
/*                 char *auth: Authorize-String, wie vom Browser bekommen              */
/*                 int *r_flag: Lesen im Verzeichnis erlaubt                           */
/*                 int *w_flag: Schreiben im Verzeichnis erlaubt                       */
/*                 int *a_flag: Ändern der .passwd-Datei erlaubt                       */
/*                 char *setuser: User, auf den gewechselt werden soll, NULL, wenn     */
/*                                setuser nicht übergeben werden soll                  */
/*                 char *userpfad: Pfad, in den gewchselt werden soll, NULL, wenn      */
/*                                 userpfad nicht übergebe werden soll                 */
/*                 char **md5sum: Platz für md5sum                                     */
/*                 return: 0, User gefunden                                            */
/*     test_pwd_md5 bestimmt anhand der passwd-Datei pd die Zugriffsrechte             */
/*                 und MD5 zu User user                                                */
/***************************************************************************************/
auth_type get_pwd_md5_file(char *pd, char *user, int *r_flag, int *w_flag, int *a_flag,
                           char *setuser, char *userpfad, char **md5sum)
{ FILE *ptr;                                     /* ptr auf ACCESS-Datei               */
  char *z;                                       /* eingelesene Zeilen aus passwd-Dat. */
  struct stat stat_buf;                          /* Zum Test, ob passwd-Datei vorhanden*/
  char r[MAX_ACC_LEN];                           /* access-Zeichen in passwd-Datei     */
  int all_user_flag = false;                     /* true; alle User sind erlaubt       */
  int all_user_a_flag, all_user_r_flag, all_user_w_flag;
  char all_user_setuser[USER_LENGTH];
  char all_user_userpfad[MAX_LEN_FILENAME];
  int af, rf, wf;
  char line[MAX_LEN_GLOBALPWDLINE];
  auth_type auth_ret;

  LOG(1, "get_pwd_md5_file, pd: %s, user: %s.\n", pd, user);

  if( !stat(passwd_path, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
  { if( NULL != (ptr = fopen(passwd_path, "r")) )  /* passwd-Datei oeffnen             */
    { LOG(14, "get_pwd_md5, pwd_datei offen.\n");
      while( !feof(ptr) )
      { if( NULL == fgets(line, MAX_LEN_GLOBALPWDLINE, ptr) )  /* Zeile einlesen       */
          break;

        LOG(6, "get_pwd_md5, line: %s.\n", line);

        z = line;
        strcpyn_l(r, &z, MAX_ACC_LEN);            /* access nach r                     */
        skip_blanks(z);
        if( str_starts(z, GLOB_USER_AUTH_B64) )   /* globale Passwd einlesen?          */
        { if( (password_file_mode == GLOBAL || password_file_mode == GLOBAL_X)
              && pd != passwd_path )
          { rf = wf = af = true;
            auth_ret = get_pwd_md5_file(passwd_path, user, &rf, &wf, &af, setuser,
                                        userpfad, md5sum);
            if( auth_ret == AUTH_MD5 )
            { *a_flag = *a_flag && af;
              *r_flag = *r_flag && rf;
              *w_flag = *w_flag && wf;
              return AUTH_MD5;
            }
            else if( auth_ret == AUTH_ALL_USER )
            { all_user_flag = true;
              all_user_a_flag = af;
              all_user_r_flag = rf;
              all_user_w_flag = wf;
              if( setuser )
                strcpyn(all_user_setuser, setuser, USER_LENGTH);
              if( userpfad )
                strcpyn(all_user_userpfad, userpfad, MAX_LEN_FILENAME);
            }
          }
          continue;
        }
        if( str_starts_z(&z, ALL_USER_AUTH_B64) )  /* "*" in Passwd-Datei              */
        { all_user_flag = true;
          all_user_a_flag = strchr(r, 'a') != NULL;
          all_user_r_flag = strchr(r, 'r') != NULL;
          all_user_w_flag = strchr(r, 'w') != NULL;
          if( setuser )
          { strcpyn_l(all_user_setuser, &z, USER_LENGTH);
            if( all_user_setuser[0] == '.' )
              all_user_setuser[0] = '\0';
          }
          else
            skip_next_blanks(z);
          if( userpfad )
            strcpyn_l(all_user_userpfad, &z, MAX_LEN_FILENAME);
          continue;
        }
        if( str_starts_z(&z, user) && *z++ == ':' )
        { fclose(ptr);                           /* gefunden, ACCESS-Datei schliessen  */
          strcpy_l_z(md5sum, &z);
          while( *z == ' ' )
            z++;
          *a_flag = *a_flag && (strchr(r, 'a') != NULL);
          *r_flag = *r_flag && (strchr(r, 'r') != NULL);
          *w_flag = *w_flag && (strchr(r, 'w') != NULL);
          if( setuser )
          { strcpyn_l(setuser, &z, USER_LENGTH); /* User nach user                     */
            if( *setuser == '.' )
              *setuser = '\0';
          }
          else
            skip_next_blanks(z);
          if( userpfad )
            strcpyn_l(userpfad, &z, MAX_LEN_FILENAME);

          return AUTH_MD5;                       /* Eintrag gefunden                   */
        }
      }
      fclose(ptr);
      if( all_user_flag && strcmp(user, NO_USER_AUTH) )
      { *a_flag = *a_flag && all_user_a_flag;
        *r_flag = *r_flag && all_user_r_flag;
        *w_flag = *w_flag && all_user_w_flag;
        if( setuser )
          strcpyn(setuser, all_user_setuser, USER_LENGTH);
        if( userpfad )
          strcpyn(userpfad, all_user_userpfad, MAX_LEN_FILENAME);
        return AUTH;
      }
    }
    return BAD;                                  /* Eintrag nicht gefunden             */
  }
  return NONE;                                   /* .passwd nicht gefunden             */
}

/***************************************************************************************/
/* auth_type get_pwd_md5(char *pfad, char *user, int *r_flag, int *w_flag,             */
/*                       char *pfad: Pfad der angeforderten Seite                      */
/*                       int *a_flag, char *setuser, char *userpfad, char **md5sum)    */
/*                 char *auth: Authorize-String, wie vom Browser bekommen              */
/*                 int *r_flag: Lesen im Verzeichnis erlaubt                           */
/*                 int *w_flag: Schreiben im Verzeichnis erlaubt                       */
/*                 int *a_flag: Ändern der .passwd-Datei erlaubt                       */
/*                 char *setuser: User, auf den gewechselt werden soll, NULL, wenn     */
/*                                setuser nicht übergeben werden soll                  */
/*                 char *userpfad: Pfad, in den gewchselt werden soll, NULL, wenn      */
/*                                 userpfad nicht übergebe werden soll                 */
/*                 char **md5sum: Platz für md5sum                                     */
/*                 return: 0, User gefunden                                            */
/*     get_pwd_md5 bestimmt anhand der .passwd Datei die Zugriffsrechte                */
/*                 und MD5 zu User user                                                */
/***************************************************************************************/
auth_type get_pwd_md5(char *pfad, char *user, int *r_flag, int *w_flag, int *a_flag,
                      char *setuser, char *userpfad, char **md5sum)
{ char *p;
  char pwddat[MAX_DIR_LEN];                      /* Aufbau des passwd-Pfades+Datei     */
  auth_type ret;

  LOG(1, "get_pwd_md5, pfad: %s, user: %s.\n", pfad, user);

  if( password_file_mode == NOACCESSFILE )
  { LOG(2, "/test_passwd, ret: NONE.\n");
    return NONE;
  }
  else if( password_file_mode == PATH )
    ret = get_pwd_md5_file(passwd_path, user, r_flag, w_flag, a_flag, setuser, userpfad,
                           md5sum);
  else
  { strcpyn(pwddat, pfad, MAX_DIR_LEN);          /* Pfad, der angefordert wird         */

    if( NULL == (p = strrchr(pwddat, '/')) )     /* letzten '/'                        */
      return BAD;

    strcpyn(p+1, PASSWORD_FILE, MAX_DIR_LEN+pwddat-p-1); /* Pfad+Name der ACCESS-Datei */

    ret = get_pwd_md5_file(pwddat, user, r_flag, w_flag, a_flag, setuser, userpfad,
                           md5sum);

    if( (ret < AUTH && password_file_mode == GLOBAL_X)
        || (ret == NONE && password_file_mode == GLOBAL) )
      ret = get_pwd_md5_file(passwd_path, user, r_flag, w_flag, a_flag, setuser,
                             userpfad, md5sum);
  }

  if( ret >= AUTH )
  { LOG(2, "/get_pwd_md5, ret: %s.\n", ret == AUTH ? "AUTH" : "AUTH_MD5");
    **md5sum = '\0';
    return ret;                                  /* Zugriff gewaehrt zurueck           */
  }
  else if( ret == NONE )
  { *r_flag = *r_flag && ( password_file_mode == NORMAL);
    *w_flag = *w_flag && ( password_file_mode == NORMAL);
    *a_flag = *a_flag && ( password_file_mode == NORMAL);
    LOG(2, "/get_pawd_md5, ret: %s.\n", password_file_mode == NORMAL ? "NONE" : "BAD");
    **md5sum = '\0';
    return password_file_mode == NORMAL ? NONE : BAD; /* keine .passwd, aus flag       */
  }
  else
  { *r_flag = *w_flag = *a_flag = false;
    LOG(2, "/test_passwd, ret: BAD.\n");
    **md5sum = '\0';
    return BAD;                                  /* kein Zugriff                       */
  }
}


#define PASSWD_NOT_CHANGED "X13243546576879Y"

/***************************************************************************************/
/* int edit_passwd(char *pwddat, int globflag)                                         */
/*                 char *pfad: Pfad der .passwd-Datei                                  */
/*                 int globflag: true, wenn globale .passwd-Datei bearbeitet wird      */
/*                 return: true bei Fehler                                             */
/*     edit_passwd Erzeugt Formular zum Ändern der .passwd-Datei                       */
/***************************************************************************************/
int edit_passwd(char *pwddat, int globflag)
{ FILE *ptr;                                     /* ptr auf ACCESS-Datei               */
  char *z;                                       /* eingelesene Zeilen aus passwd-Dat. */
  char a[MAX_AUTH_LEN];                          /* authorize aus passwd-Datei         */
  char r[MAX_ACC_LEN];                           /* access-Zeichen in passwd-Datei     */
  int a_flag, r_flag, w_flag;
  char setuser[USER_LENGTH];                     /* User in passwd-Datei               */
  char userpfad[MAX_LEN_FILENAME];               /* Userpfad in passwd-Datei           */

  LOG(1, "edit_passwd, pwddat: %s.\n", pwddat);

  if( password_file_mode == NOACCESSFILE )
  {                                                  /* Fehlerseite ausgeben           */
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  if( passwd_start_form(globflag) )              /* Formularanfang ausgeben            */
    return true;

  if( NULL != (ptr = fopen(pwddat, "r")) )       /* passwd-Datei oeffnen               */
  { while( !feof(ptr) )
    { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) ) /* Zeile einlesen              */
        break;
      z = zeile;
      strcpyn_l(r, &z, MAX_ACC_LEN);             /* Access Zeichen nach r              */
      strcpyn_l(a, &z, MAX_AUTH_LEN);            /* Authorize String nach a            */
      strcpyn_l(setuser, &z, USER_LENGTH);       /* User nach setuser                  */
      if( *setuser == '.' )
        *setuser = '\0';
      strcpyn_l(userpfad, &z, MAX_LEN_FILENAME); /* userpfad nach uesrpfad             */
      a_flag = strchr(r, 'a') != NULL;           /* Access erlaubt?                    */
      r_flag = strchr(r, 'r') != NULL;           /* Read erlaubt?                      */
      w_flag = strchr(r, 'w') != NULL;           /* Write erlaubt?                     */
      if( *r && *a )
      { if( passwd_eintrag_form(r_flag, w_flag, a_flag, a, setuser, userpfad, false) )
                                                 /* Formularzeile                      */
        { fclose(ptr);                           /* bei Fehler abbruch                 */
          return true;
        }
      }
    }
    fclose(ptr);
  }
  if( passwd_eintrag_form(false, false, false, "NEU", "", "", true) ) /* Neuer Eintrag */
    return true;
  return passwd_end_form();                      /* Formularende ausgeben              */
}

#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"
// verhindert: warning: ‘p’ may be used uninitialized in this function [-Wsequence-point]
#endif
#endif
/***************************************************************************************/
/* int save_passwd(char *pwddat, int globflag)                                         */
/*                 char *pfad: Pfad der .passwd-Datei                                  */
/*                 int globflag: true, wenn globale .passwd-Datei bearbeitet wird      */
/*                 return: true bei Fehler                                             */
/*     save_passwd Speichert neue Access-Rechte aus Formularergebnis                   */
/***************************************************************************************/
int save_passwd(char *pwddat, int globflag)
{ FILE *ptralt, *ptrneu;                         /* ptr auf ACCESS-Datei               */
  char r[MAX_ACC_LEN];                           /* access-Zeichen aus paswd-Datei     */
  char a[MAX_AUTH_LEN+1];                        /* authorize für passwd-Datei         */
  char *pnpwd, neupwd[MAX_AUTH_LEN];             /* Neues Kennwort                     */
  char setuser[USER_LENGTH];                     /* Setuser-Pfad                       */
  char parname[MAX_AUTH_LEN];                    /* Zum Aufbauen des Parameternamen    */
  int r_flag, w_flag, a_flag;                    /* read, write, access_flag           */
  char *z, *p;
  size_t nb_pwd;                                 /* Anzahl Zeichen der Passwortdatei   */

  LOG(1, "save_passwd, pwddat: %s.\n", pwddat);

  if( password_file_mode == NOACCESSFILE )
  {                                                  /* Fehlerseite ausgeben           */
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  if( NULL != (ptralt = fopen(pwddat, "r")) )       /* alte passwd-Datei öffnen        */
  { nb_pwd = fread(zeile, 1, MAX_ZEILENLAENGE-1, ptralt);
    zeile[nb_pwd] = '\0';
    fclose(ptralt);
    if( NULL != (ptrneu = fopen(pwddat, "w")) )     /* neue passwd-Datei öffnen        */
    { z = zeile;
      while( *z )
      { skip_next_blanks(z);
        strcpyn_l(a, &z, MAX_AUTH_LEN);
        strcpy(parname, "L_");                   /* Löschbuttonname                    */
        strcatn(parname, a, MAX_AUTH_LEN);       /* zusammensetzen                     */
        if( (p = strchr(parname, '=')) )
          *p = '\0';
        if( !eqbpar(parname, "on") )             /* nicht angeklickt, dann             */
        { parname[0] = 'R';                      /* Readbuttonname zusammensetzen      */
            r_flag = eqbpar(parname, "on");
          parname[0] = 'W';                      /* Writebuttonname zusammensetzen     */
          w_flag = eqbpar(parname, "on");
          parname[0] = 'A';                      /* Accessbuttonname zusammensetzen    */
          a_flag = eqbpar(parname, "on");
          parname[0] = 'P';                      /* Passworteintrag                    */
          if( (p=parwert(parname, HP_BROWSER_LEVEL)) )
          { strcpyn(neupwd, p, MAX_AUTH_LEN);
            pnpwd = neupwd;
          }
          else
          { neupwd[0] = '\0';
            pnpwd = NULL;
          }
          parname[0] = 'S';                      /* Setuser                            */
          if( (p=parwert(parname, HP_BROWSER_LEVEL)) )
            strcpyn(setuser, p, USER_LENGTH);
          else
            setuser[0] = '\0';
          parname[0] = 'F';                      /* Userpfad                           */
          if( (p=parwert(parname, HP_BROWSER_LEVEL)) && !setuser[0] )
          { setuser[0] = '.';
            setuser[1] = '\0';
          }
          snprintf(r, MAX_ACC_LEN, "%s%s%s%s",
                  r_flag ? "r" : "", w_flag ? "w" : "", a_flag ? "a" : "",
                  (r_flag || w_flag || a_flag) ? "" : ".");
          store_password_line(ptrneu, r, a, setuser,
                              p ? p : "",
                              strcmp(neupwd, PASSWD_NOT_CHANGED)
                                ? pnpwd
                                : NULL);
        }
        while( *z && *z != '\n' && *z != '\r' )
          z++;
        while( *z && (*z == '\n' || *z == '\r') )
          z++;
      }
      if( (p=parwert("U_NEU", HP_BROWSER_LEVEL)) && *p )
      { pnpwd = parwert("P_NEU", HP_BROWSER_LEVEL);
        if( *(p+1) == '\0' )
        { switch(*p)
          { case NO_USER_AUTH_CHAR:   strncpy(a, NO_USER_AUTH_B64, MAX_AUTH_LEN);
                                      pnpwd = NULL;
                                      break;
            case GLOB_USER_AUTH_CHAR: strncpy(a, GLOB_USER_AUTH_B64, MAX_AUTH_LEN);
                                      pnpwd = NULL;
                                      break;
            case ALL_USER_AUTH_CHAR:  strncpy(a, ALL_USER_AUTH_B64, MAX_AUTH_LEN);
                                      pnpwd = NULL;
                                      break;
            default:                  strncpy(a, p, MAX_AUTH_LEN);
                                      strcatn(a, ":", MAX_AUTH_LEN);
          }
        }
        else
        { strncpy(a, p, MAX_AUTH_LEN);
          strcatn(a, ":", MAX_AUTH_LEN);
        }
        snprintf(r, MAX_ACC_LEN, "%s%s%s",
                 eqbpar("R_NEU", "on") ? "r" : "",
                 eqbpar("W_NEU", "on") ? "w" : "",
                 eqbpar("A_NEU", "on") ? "a" : "");
        store_password_line(ptrneu, *r ? r : ".", a,
                            (p=parwert("S_NEU", HP_BROWSER_LEVEL)) ? p : ".",
                            (p=parwert("F_NEU", HP_BROWSER_LEVEL)) ? p : "",
                            pnpwd);
      }
      fclose(ptrneu);
      return edit_passwd(pwddat, globflag);
    }
    else
    { if( send_http_header("text/html", "", true, 0, u2w_charset) )
        return true;
      http_head_flag = 3;
      return dosendf(PAGE_HEADER PAGE_HEADER_ENDE PAGE_BODY_START, u2w_charset) 
           || send_headline(_("ERROR on changing password file."))
           || dosend(PAGE_END);
    }
  }
                                                     /* Fehlerseite ausgeben           */
  send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                  SEITE_NICHT_GEFUNDEN_TEXT);
  return true;
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif


/***************************************************************************************/
/* int passwd_start_form(int globflag)                                                 */
/*                       int globflag: true, wenn globale .passwd-Datei bearbeitet wird*/
/*     passwd_start_form Gibt Anfang des Editierfensters für die .passwd-Datei aus     */
/***************************************************************************************/
int passwd_start_form(int globflag)
{ if( send_http_header("text/html", "", true, 0, u2w_charset) )
    return 1;
  http_head_flag = 3;
  return dosendf(PAGE_HEADER "<title>PASSWD</title>" PAGE_HEADER_ENDE, u2w_charset)
         || send_body_start() || send_headline(_("edit .passwd file"))
         || start_formular(false, false, globflag ? SAVE_GLOBAL_ACC : clientgetfile)
         || dosend(globflag
                   ? "<input name=\"" PWDEDITMODE "\" type=\"hidden\" value=\"" PWDGLOBSAVE "\">"
                   : "<input name=\"" PWDEDITMODE "\" type=\"hidden\" value=\"" PWDSAVE "\">"
                  )
         || dosend("<table>\n")
         || dosendf("<tr><td style=\"text-align:center\">R</td><td style=\"text-align:center\">W</td><td style=\"text-align:center\">A</td><td>%s</td><td>%s</td>",
                    _("user"), _("password"))
         || ( set_user_mode == USERSETUSER ? dosendf("<td>%s</td>", _("setuser")) : false )
         || ( verzeichnis_mode == VZ_PWD ? dosendf("<td>%s</td>", _("userpath")) : false )
         || dosendf("<td>%s</td></tr>\n", _("delete"));
}


/***************************************************************************************/
/* int passwd_end_form(void)                                                           */
/*     passwd_end_form gibt Ende des Editierfensters für die .passwd-Datei mit Buttons */
/*                     aus                                                             */
/***************************************************************************************/
int passwd_end_form(void)
{
  if( dosend("</table>\n") || end_formular(BUTTONS, false) || dosend(PAGE_END) )
    return true;
  else
    return false;
}


/***************************************************************************************/
/* int passwd_eintrag_form(int r_flag, int w_flag, int a_flag, char *auth,             */
/*                         char *setuser, char *userpfad, int newflag)                 */
/*     passwd_eintrag_form gibt eine Zeile der .passwd-Datei aus                       */
/***************************************************************************************/
int passwd_eintrag_form(int r_flag, int w_flag, int a_flag, char *auth, char *setuser,
                        char *userpfad, int newflag)
{ char user[MAX_AUTH_LEN];
  char *p;

  LOG(3, "passwd_eintrag_form, auth: %s.\n", auth);

  if( newflag )
    user[0] = '\0';
  else
  { if( NULL != (p = strchr(auth, ':')) )
    { strcpyn(user, auth, p-auth+1);
    }
    else
    { strcpyn(zeile, auth, MAX_ZEILENLAENGE);
      decode_base64((unsigned char*)zeile);
      if( (p = strchr(zeile, ':')) != NULL )
      { *p = '\0';
        strcpyn(user, zeile, MAX_AUTH_LEN);
      }
      else
        user[0] = '\0';
    }
  }
  if( NULL != (p = strchr(auth, '=')) )
    *p = '\0';

  if( !strcmp(user, NO_USER_AUTH) || !strcmp(user, GLOB_USER_AUTH)
      || !strcmp(user, ALL_USER_AUTH) )
  { if( dosendf("<tr><td><input name=\"R_%s\" type=\"checkbox\"%s></td>"
                "<td><input name=\"W_%s\" type=\"checkbox\"%s></td>"
                "<td><input name=\"A_%s\" type=\"checkbox\"%s></td>"
                "<td>%s</td>"
                "<td>&nbsp;</td>",
                auth, r_flag ? " CHECKED" : "",
                auth, w_flag ? " CHECKED" : "",
                auth, a_flag ? " CHECKED" : "",
                user) )
      return true;
  }
  else
  { if( dosendf("<tr><td><input name=\"R_%s\" type=\"checkbox\"%s></td>"
                "<td><input name=\"W_%s\" type=\"checkbox\"%s></td>"
                "<td><input name=\"A_%s\" type=\"checkbox\"%s></td>"
                "<td>%s</td>"
                "<td><input name=\"P_%s\" type=\"password\" size=12 value=\"%s\"></td>",
                auth, r_flag ? " CHECKED" : "",
                auth, w_flag ? " CHECKED" : "",
                auth, a_flag ? " CHECKED" : "",
                *user ? user : "<input name=\"U_NEU\" type=\"text\" size=12>",
                auth, *user ? PASSWD_NOT_CHANGED : "") )
      return true;
  }
  if( set_user_mode == USERSETUSER )
  { if( dosendf("<td><input name=\"S_%s\" type=\"text\" size=12 value=\"%s\"></td>",
                auth, setuser) )
      return true;
  }
  if( verzeichnis_mode == VZ_PWD )
  { if( dosendf("<td><input name=\"F_%s\" type=\"text\" size=12 value=\"%s\"></td>",
                auth, userpfad) )
      return true;
  }
  if( newflag )
    return dosendf("<td>%s</td></tr>\n", _("new entry"));
  else
    return dosendf("<td><input name=\"L_%s\" type=\"checkbox\"></td></tr>\n", auth);
}


/***************************************************************************************/
/* int change_passwd(char *pwddat, int globflag)                                       */
/*                   char *pfad: Pfad der .passwd-Datei                                */
/*                   int globflag: true, wenn globale .passwd-Datei bearbeitet wird    */
/*                   return: true bei Fehler                                           */
/*     change_passwd Erzeugt Formular zum Ändern des eigenen Passwortes                */
/***************************************************************************************/
int change_passwd(char *pwddat, int globflag)
{
  LOG(1, "change_passwd, pwddat: %s.\n", pwddat);

  if( password_file_mode == NOACCESSFILE )
  {                                                  /* Fehlerseite ausgeben           */
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  snprintf(zeile, MAX_ZEILENLAENGE, _("change password for %s."), user);

  if( send_http_header("text/html", "", true, 0, u2w_charset) )
    return true;
  http_head_flag = 3;
  return dosendf(PAGE_HEADER PAGE_HEADER_ENDE PAGE_BODY_START, u2w_charset) || send_headline(zeile)
        || start_formular(false, false, globflag ? CHANGE_GLOBAL_PWD : clientgetfile)
        || dosend(globflag
                  ? "<input name=\"" PWDEDITMODE "\" type=\"hidden\" value=\"" PWDUSERGLOBSAVE "\">"
                  : "<input name=\"" PWDEDITMODE "\" type=\"hidden\" value=\"" PWDUSERSAVE "\">"
                 )
        || dosend("<table>\n")
        || dosendf("<tr><td>%s</td><td><input name=\""PWDALT_PAR_NAME"\" type=\"password\" size=12></td></tr>\n", _("old password"))
        || dosendf("<tr><td>%s</td><td><input name=\""PWDNEU_PAR_NAME"\" type=\"password\" size=12></td></tr>\n", _("new password"))
        || dosendf("<tr><td>%s</td><td><input name=\""PWDVER_PAR_NAME"\" type=\"password\" size=12></td></tr>\n", _("new password"))
        || passwd_end_form();

}


/***************************************************************************************/
/* int store_password_line(FILE *ptrneu, char *r, char *a, char *setuser,              */
/*                         char *userpfad, char *pwdneu)                               */
/*                      FILE *ptrneu: Ausgabedatei                                     */
/*                      char *r: Rechte                                                */
/*                      char *a: alter auth-String aus Password-Datei                  */
/*                      char *setuser: Setuser-Eintrag                                 */
/*                      char *userpfad: Userpfad-Eintrag                               */
/*                      char *pwdneu: neues Kennwort, NULL: pwd nicht ändern           */
/*                      return: true bei Fehler                                        */
/*     store_password_line speichert eine Zeile in die Password-Datei                  */
/***************************************************************************************/
int store_password_line(FILE *ptrneu, char *r, char *a, char *setuser,
                        char *userpfad, char *pwdneu)
{ char userpwd[MAX_AUTH_LEN];
  char *dp;
  char user[MAX_AUTH_LEN];                       /* user zum authorize aus passwd-Dat. */
  char md5sum[MD5_DIGEST_LENGTH*2+1];

  LOG(23, "store_password_line, r: %s, a: %s, setuser: %s, userpfad: %s, pwdneu: %s.\n",
      r, a, setuser, userpfad, pwdneu?pwdneu:"(null)");

  if( NULL == pwdneu )
  { fprintf(ptrneu, "%s %s %s %s\n", r, a, setuser, userpfad);
    return 0;
  }
  if( NULL != (dp = strchr(a, ':')) )
  { strcpyn(userpwd, a, dp-a+2);                  /* String user:pwd beginnen          */
    strcpyn(user, a, dp-a+1);
  }
  else
  { strcpyn(userpwd, a, MAX_AUTH_LEN);
    decode_base64((unsigned char *)userpwd);
    if( (dp = strchr(userpwd, ':')) != NULL )
    { *(dp+1) = '\0';
      strcpyn(user, userpwd, dp-userpwd+1);
    }
    else
      user[0] = '\0';
  }

  if( digestnum )
  { strcatn(userpwd, realm, MAX_AUTH_LEN);
    strcatn(userpwd, ":", MAX_AUTH_LEN);
    strcatn(userpwd, pwdneu, MAX_AUTH_LEN);       /* String user:pwd ergaenzen         */
    calc_md5sum(md5sum, (unsigned char *)userpwd, strlen(userpwd));
  }
  else
  { strcatn(userpwd, pwdneu, MAX_AUTH_LEN);       /* String user:pwd ergaenzen         */
    encode_base64((unsigned char *)a, (unsigned char *)userpwd);
    calc_md5sum(md5sum, (unsigned char *)a, strlen(a));
  }
  fprintf(ptrneu, "%s %s:%s %s %s\n", r, user, md5sum, setuser, userpfad);

  return 0;
}


/***************************************************************************************/
/* int update_passwd(char *pwddat)                                                     */
/*                   char *pfad: Pfad der .passwd-Datei                                */
/*                   return: true bei Fehler                                           */
/*     update_passwd ändert Benutzerpasswort in passwd-Datei                           */
/***************************************************************************************/
int update_passwd(char *pwddat)
{ FILE *ptralt, *ptrneu;                         /* ptr auf ACCESS-Datei               */
  char r[MAX_ACC_LEN];                           /* access-Zeichen aus paswd-Datei     */
  char a[MAX_AUTH_LEN];                          /* authorize aus passwd-Datei         */
  char setuser[USER_LENGTH];                     /* User aus passwd-Datei              */
  char userpfad[MAX_LEN_FILENAME];               /* Userpfad aus passwd-Datei          */
  char *z;
  size_t nb_pwd;                                 /* Anzahl Zeichen der Passwortdatei   */
  char *dp, md5sum[MD5_DIGEST_LENGTH*2+1];

  calc_md5sum(md5sum, (unsigned char *)auth_basic, strlen(auth_basic));

  LOG(1, "updat_passwd, pwddat: %s.\n", pwddat);

  if( password_file_mode == NOACCESSFILE
      || !parwert(PWDALT_PAR_NAME, HP_BROWSER_LEVEL)
      || !parwert(PWDNEU_PAR_NAME, HP_BROWSER_LEVEL)
      || !parwert(PWDVER_PAR_NAME, HP_BROWSER_LEVEL) )
  {                                                  /* Fehlerseite ausgeben           */
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  if( strcmp(passwd, parwert(PWDALT_PAR_NAME, HP_BROWSER_LEVEL)) )
  { if( send_http_header("text/html", "", true, 0, u2w_charset) )
      return true;
    http_head_flag = 3;
    return dosendf(PAGE_HEADER "<title>PASSWD</title>" PAGE_HEADER_ENDE, u2w_charset)
           || send_body_start() || send_headline(_("wrong password."))
           || dosend(PAGE_END);
  }

  if( strcmp(parwert(PWDNEU_PAR_NAME, HP_BROWSER_LEVEL),
             parwert(PWDVER_PAR_NAME, HP_BROWSER_LEVEL)) )
  { if( send_http_header("text/html", "", true, 0, u2w_charset) )
      return true;
    http_head_flag = 3;
    return dosendf(PAGE_HEADER "<title>PASSWD</title>" PAGE_HEADER_ENDE, u2w_charset)
           || send_body_start() || send_headline(_("password wasn't retyped correctly."))
           || dosend(PAGE_END);
  }


  if( NULL != (ptralt = fopen(pwddat, "r")) )       /* alte passwd-Datei öffnen        */
  { nb_pwd = fread(zeile, 1, MAX_ZEILENLAENGE-1, ptralt);
    zeile[nb_pwd] = '\0';
    fclose(ptralt);
    if( NULL != (ptrneu = fopen(pwddat, "w")) )     /* neue passwd-Datei öffnen        */
    { z = zeile;
      while( *z )
      { strcpyn_l(r, &z, MAX_ACC_LEN);           /* Access Zeichen nach r              */
        strcpyn_l(a, &z, MAX_AUTH_LEN);          /* Authorize String nach a            */
        strcpyn_l(setuser, &z, USER_LENGTH);     /* User nach setuser                  */
        strcpyn_l(userpfad, &z, MAX_LEN_FILENAME);  /* Userpfad nach userpfad          */

        if( !strcmp(a, auth_basic)
            || (NULL != (dp = strchr(a, ':')) && !strcmp(dp+1, md5sum)) )
                                                 /* mit Anmeldung vergleichen          */
          store_password_line(ptrneu, r, a, setuser, userpfad,
                              parwert(PWDNEU_PAR_NAME, HP_BROWSER_LEVEL));
        else
          store_password_line(ptrneu, r, a, setuser, userpfad, NULL);
        while( *z && *z != '\n' && *z != '\r' )
          z++;
        while( *z && (*z == '\n' || *z == '\r') )
          z++;
      }
      fclose(ptrneu);
      if( send_http_header("text/html", "", true, 0, u2w_charset) )
        return true;
      http_head_flag = 3;
      return dosendf(PAGE_HEADER "<title>PASSWD</title>" PAGE_HEADER_ENDE, u2w_charset)
             || send_body_start() || send_headline(_("changed password."))
             || dosend(PAGE_END);
    }
    else
    { if( send_http_header("text/html", "", true, 0, u2w_charset) )
        return true;
      http_head_flag = 3;
      return dosendf(PAGE_HEADER "<title>PASSWD</title>" PAGE_HEADER_ENDE, u2w_charset)
             || send_body_start() || send_headline(_("ERROR on changing password."))
             || dosend(PAGE_END);
    }
  }
                                                     /* Fehlerseite ausgeben           */
  send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                  SEITE_NICHT_GEFUNDEN_TEXT);
  return true;
}
#endif  /* #ifdef WEBSERVER */
