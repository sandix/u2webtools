/************************************************/
/* File: u2w_pwd.c                              */
/* Funktionen zum Auswerten der Passworteingabe */
/* timestamp: 2013-12-24 11:11:24               */
/************************************************/


#include "u2w.h"

#ifdef WEBSERVER

#ifdef HAS_TRUSTED

#define MAX_LEN_AUTHFILE_PATH 256                           /* Max Laenge Autfilepfad  */


/***************************************************************************************/
/* int test_pwd(char *user, char *pwd)                                                 */
/*              char *user: Username                                                   */
/*              char *pwd : Passwort                                                   */
/*              return    : false, wenn user und pwd nicht stimmen                     */
/***************************************************************************************/
int test_pwd(char *user, char *pwd)
{ char pwd_crypt[64], authfile[MAX_LEN_AUTHFILE_PATH];
  FILE *ptr;
  char *p, *pc;
  char salt[3];
  char newcrypt[64];
  char inzeile[MAX_ZEILENAENGE];

  LOG(1, "test_pwd (TRUSTED), user: %s.\n", user);

  snprintf(authfile, MAX_LEN_AUTHFILE_PATH, "/tcb/files/auth/%c/%s", user[0], user);

  if( NULL != (ptr = fopen(authfile, "r")) )
  { while( !feof(ptr) )
    { fgets(inzeile, MAX_ZEILENLAENGE-1, ptr);
      if( NULL != (p = strstr(inzeile, "u_pwd=")) )
      { strcpy(pwd_crypt, p+6);
        if( NULL != (p = strchr(pwd_crypt, ':')) )
          *p = '\0';
        LOG(4, "Crypt:  %s.\n", pwd_crypt);
        strncpy(salt, pwd_crypt, 2);
        salt[2] = '\0';
        p = pwd;
        strcpy(newcrypt, salt);
        for(;;)
        { pc = crypt(p, salt);
          strcatn(newcrypt, pc+2, 64);
          if( strlen(p) > 8 )
            p += 8;
          else
            break;
          strncpy(salt, pc+2, 2);
          salt[2] = '\0';
        }
        LOG(4, "Ncrypt: %s.\n", newcrypt);
        return  0 == strcmp(newcrypt, pwd_crypt);
      }
    }
    fclose(ptr);
  }
  return false;
}

#else  /* ifdef HAS_TRUSTED */

#ifdef HAS_SHADOW

#include <shadow.h>

/***************************************************************************************/
/* int test_pwd(char *user, char *pwd)                                                 */
/*              char *user: Username                                                   */
/*              char *pwd : Passwort                                                   */
/*              return    : false, wenn user und pwd nicht stimmen                     */
/***************************************************************************************/
int test_pwd(char *user, char *pwd)
{ char pwd_crypt[64];
  char *p, *pc;
  char salt[3];
  char newcrypt[64];
  struct spwd *pspwd;
  struct passwd *pass;

  LOG(1, "test_pwd (SHADOW), user: %s.\n", user);

  if( NULL != (pspwd = getspnam(user)) || NULL != (pass = getpwnam(user)) )
  { if( NULL != pspwd )
      strcpy(pwd_crypt, pspwd->sp_pwdp);
    else
      strcpy(pwd_crypt, pass->pw_passwd);
    LOG(6, "Crypt:  %s.\n", pwd_crypt);
    strncpy(salt, pwd_crypt, 2);
    LOG(6, "Crypt 1.\n");
    salt[2] = '\0';
    LOG(6, "Crypt 2.\n");
    p = pwd;
    LOG(6, "Crypt 3.\n");
    strcpy(newcrypt, salt);
    LOG(6, "Crypt 4.\n");
    for(;;)
    { pc = crypt(p, salt);
      LOG(6, "Crypt 5.\n");
      strcatn(newcrypt, pc+2, 64);
      if( strlen(p) > 8 )
        p += 8;
      else
        break;
      strncpy(salt, pc+2, 2);
      salt[2] = '\0';
    }
    LOG(6, "Ncrypt: %s.\n", newcrypt);
    return  0 == strcmp(newcrypt, pwd_crypt);
  }
  return false;
}

#else   /* ifdef HAS_SHADOW */

#ifdef CYGWIN

/***************************************************************************************/
/* int test_pwd(char *user, char *pwd)                                                 */
/*              char *user: Username                                                   */
/*              char *pwd : Passwort                                                   */
/*              return    : false, wenn user und pwd nicht stimmen                     */
/***************************************************************************************/
int test_pwd(char *user, char *pwd)
{ HANDLE token;
  struct passwd *pass;

  LOG(1, "test_pwd (CYGWIN), user: %s.\n", user);

  if( NULL != (pass = getpwnam(user)) )
  { token = cygwin_logon_user(pass, pwd);
    if( token == INVALID_HANDLE_VALUE )
      return false;
    cygwin_set_impersonation_token(token);
    return true;
  }
  return false;
}


#else  /* ifdef CYGWIN */

#ifdef HAS_PAM

pamuserstruct pamuser = {NULL, NULL};

int auth_pam_conv(int num_msg, const struct pam_message **msg,
                  struct pam_response **response, void *appdata_ptr)
{ int i, j;
  struct pam_response *reply;
  pamuserstruct *user = (pamuserstruct *)appdata_ptr;

  if( num_msg <= 0 || !response || !msg || !user )
    return PAM_CONV_ERR;

  reply = (struct pam_response *)calloc(num_msg, sizeof(struct pam_response));
  if( reply == NULL )
    return PAM_CONV_ERR;

  for( i = 0; i < num_msg; i++ )
  { reply[i].resp_retcode = 0;
    reply[i].resp = 0;

    switch(msg[i]->msg_style)
    { case PAM_PROMPT_ECHO_OFF:
            reply[i].resp = strdup(user->pwd);
            break;
      case PAM_PROMPT_ECHO_ON:
            reply[i].resp = strdup(user->name);
            break;
      default:
            for( j = 0; j < i; j++ )
            { if( reply[j].resp )
                free(reply[j].resp);
            }
            free(reply);
            return PAM_CONV_ERR;
    }
  }
  *response = reply;
  return PAM_SUCCESS;
}

static struct pam_conv conv = {
    auth_pam_conv,
    (void *)&pamuser
};

/***************************************************************************************/
/* int test_pwd(char *user, char *pwd)                                                 */
/*              char *user: Username                                                   */
/*              char *pwd : Passwort                                                   */
/*              return    : false, wenn user und pwd nicht stimmen                     */
/***************************************************************************************/
int test_pwd(char *user, char *pwd)
{ pam_handle_t *pamh = NULL;
  int ret;

  LOG(1, "test_pwd (PAM), user: %s.\n", user);

  pamuser.name = user;
  pamuser.pwd = pwd;

  ret = pam_start("unix2webd", user, &conv, &pamh);

  if( ret == PAM_SUCCESS )
    ret = pam_authenticate(pamh, 0);

  if( ret == PAM_SUCCESS )
    ret = pam_acct_mgmt(pamh, 0);

  if( pam_end(pamh, ret) != PAM_SUCCESS )
    logging("pam_end: failed to release authenticator.\n");

  LOG(3, "/test_pwd (PAM), ret: %d.\n", ret == PAM_SUCCESS);
  return ret == PAM_SUCCESS;
}

#else  /* ifdef HAS_PAM */

/***************************************************************************************/
/* int test_pwd(char *user, char *pwd)                                                 */
/*              char *user: Username                                                   */
/*              char *pwd : Passwort                                                   */
/*              return    : false, wenn user und pwd nicht stimmen                     */
/***************************************************************************************/
int test_pwd(char *user, char *pwd)
{ char pwd_crypt[64];
  char *p, *pc;
  char salt[3];
  char newcrypt[64];
  struct passwd *pass;

  LOG(1, "test_pwd (CRYPT), user: %s.\n", user);

  if( NULL != (pass = getpwnam(user)) )
  { strcpy(pwd_crypt, pass->pw_passwd);
    LOG(6, "Crypt:  %s.\n", pwd_crypt);
    strncpy(salt, pwd_crypt, 2);
    LOG(1, "Crypt 1.\n");
    salt[2] = '\0';
    LOG(6, "Crypt 2.\n");
    p = pwd;
    LOG(6, "Crypt 3.\n");
    strcpy(newcrypt, salt);
    LOG(6, "Crypt 4.\n");
    for(;;)
    { pc = crypt(p, salt);
      LOG(6, "Crypt 5.\n");
      strcatn(newcrypt, pc+2, 64);
      if( strlen(p) > 8 )
        p += 8;
      else
        break;
      strncpy(salt, pc+2, 2);
      salt[2] = '\0';
    }
    LOG(6, "Ncrypt: %s.\n", newcrypt);
    return  0 == strcmp(newcrypt, pwd_crypt);
  }
  return false;
}

#endif  /* ifdef HAS_PAM */
#endif  /* ifdef CYGWIN */
#endif  /* ifdef HAS_SHADOW */
#endif  /* ifdef HAS_TRUSTED */

/***************************************************************************************/
/* int send_authorize(int staleflag)                                                   */
/*                  int staleflag: tue, wenn neuer nonce geniert wird                  */
/*                  return : true bei fehler, sonst false                              */
/*     send_authorize veranlasst den Browser ein Fenster fuer Kennung und PWD zu zeigen*/
/***************************************************************************************/
int send_authorize(int staleflag)
{ char zeile[MAX_ZEILENLAENGE];

  LOG(35, "send_authorize\n");
  if( digestnum )
  { snprintf(zeile, MAX_LEN_DIGEST_NONCE*2/3, "%s%x", clientip, rand());
    encode_base64((unsigned char *)servernonce, (unsigned char *)zeile);
    if( staleflag )
      snprintf(zeile, MAX_ZEILENLAENGE, AUTHORIZE_DIGEST_STALE_HTTP, realm, servernonce);
    else
      snprintf(zeile, MAX_ZEILENLAENGE, AUTHORIZE_DIGEST_HTTP, realm, servernonce);
  }
  else
    snprintf(zeile, MAX_ZEILENLAENGE, AUTHORIZE_SEITE_HTTP, realm);
  return send_error_page(AUTHORIZE_SEITE_NUM, AUTHORIZE_SEITE_DESC,
                         zeile, AUTHORIZE_SEITE_TEXT);
}


/***************************************************************************************/
/* int set_user_pass(char *user, char *pwd)                                            */
/*                   char *user: User, zu dem der Prozess wechseln soll                */
/*                   char *pwd: Passwort, das vom Browser geliefert wurde              */
/*                   return    : true, bei Fehlerhafter Authentifizierung              */
/*     set_user_pass User authentifizieren und User wechseln                           */
/***************************************************************************************/
int set_user_pass(char *user, char *pwd)
{ struct passwd *pass;

  if( *user == '\0' || *pwd == '\0' )
    return true;
  else
  { LOG(1, "set_user_pass, user: %s.\n", user);
    if( (pass = getpwnam(user)) == NULL )
      return true;
    LOG(5, "pwdcheck, username: %s, pwd: %s.\n", pass->pw_name, pass->pw_passwd);
    if( test_pwd(user, pwd) )
      return change_user(pass, false);            /* Password ok                       */
    else
      return true;
  }
  return false;
}


/***************************************************************************************/
/* int test_user(char *user)                                                           */
/*               char *user: Username, der getestet werden soll                        */
/*               return    : true, wenn user nicht gefunden                            */
/*     test_user sucht user in der Userdatenbank                                       */
/***************************************************************************************/
int test_user(char *user)
{ struct passwd *pass;

  if( (pass = getpwnam(user)) == NULL )
    return true;
  else
    return false;
}


/***************************************************************************************/
/* char *get_user_home(char *user)                                                     */
/*                     char *user: Username                                            */
/*                     return    : Pointer auf HOME-Verzeichnis des Users oder NULL    */
/*       get_user_home bestimmt Homeverzeichnis des users in der Userdatenbank         */
/***************************************************************************************/
char *get_user_home(char *user)
{ struct passwd *pass;

  if( (pass = getpwnam(user)) == NULL )
    return NULL;
  else
    return pass->pw_dir;
}


/***************************************************************************************/
/* int set_user(char *user, int env_flag)                                              */
/*              char *user: Username, der gesetzt werden soll                          */
/*              int env_flag: wenn true, dann wird Environment gesetzt                 */
/*              return    : true, wenn user nicht gesetzt                              */
/*     set_user Prozess als user ausfuehren                                            */
/***************************************************************************************/
int set_user(char *user, int env_flag)
{ struct passwd *pass;

  LOG(1, "set_user, user: %s.\n", user);
  if( (pass = getpwnam(user)) == NULL )
  { LOG(3, "set_user, getpwnam fehlgeschlagen.\n");
    return true;
  }
  else
    return change_user(pass, env_flag);
}
#endif  /* #ifdef WEBSERVER */


#if defined(WEBSERVER) || defined(U2W_PASSWD)

char *b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/***************************************************************************************/
/* unsigned char i64(char c)                                                           */
/*                   char c: Zeichen, zu dem Index im base64 Str. bestimmt werden soll */
/*               i64 bestimmt Index im base64 Code-String                              */
/***************************************************************************************/
unsigned char i64(char c)
{ int i;

  for( i = 0; b64[i] != c && b64[i]; i++ );
  return i&0x3f;
}


/***************************************************************************************/
/* void decode_base64(char *c)                                                         */
/*                    char *c: nach base64 codierter String                            */
/*      decode_base64 entschluesselt c, Ergebnis steht anchliessend in c               */
/***************************************************************************************/
void decode_base64(unsigned char *c)
{ unsigned char *cc, *tt;

  tt = cc = c;

  if( !*c )
    return;

  do
  { *tt++ = i64(*cc)<<2 | i64(*(cc+1))>>4;
    *tt++ = i64(*(cc+1))<<4 | i64(*(cc+2))>>2;
    *tt++ = i64(*(cc+2))<<6 | i64(*(cc+3));
  } while( *++cc && *++cc && *++cc && *++cc );
  *tt = 0;
}


/***************************************************************************************/
/* void encode_base64(unsigned char *c, unsigned char *t)                              */
/*                    char *c: Platz für codierten String                              */
/*                    char *t: zu codierender String                                   */
/*      encode_base64 verschluesselt t, Ergebnis steht anchliessend in c               */
/***************************************************************************************/
void encode_base64(unsigned char *c, unsigned char *t)
{ unsigned char *cc, *tt;
  int n;

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
#endif  /* #if defined(WEBSERVER) || defined(U2W_PASSWD) */
