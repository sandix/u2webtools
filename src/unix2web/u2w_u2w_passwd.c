/****************************************************************/
/* File: set_u2w_passwd.c                                       */
/* Programm zum setzen der Kennwörter in der .passwd Datei      */
/* timestamp: 2015-02-05 14:13:11                               */
/****************************************************************/

#include "u2w.h"

#ifdef U2W_PASSWD

#define false 0
#define true !false


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

  strcatn(userpwd, pwdneu, MAX_AUTH_LEN);         /* String user:pwd ergaenzen         */
  encode_base64((unsigned char *)a, (unsigned char *)userpwd);
  calc_md5sum(md5sum, (unsigned char *)a, strlen(a));
  fprintf(ptrneu, "%s %s:%s %s %s\n", r, user, md5sum, setuser, userpfad);

  return 0;
}


/***************************************************************************************/
/* int update_passwd(char *pwddat, char *user, char *pwd, char *realm)                 */
/*                   char *pwddat: Pfad der .passwd-Datei                              */
/*                   char *user:   Username                                            */
/*                   char *pwd:    Kennwort                                            */
/*                   char *realm: Wenn gesetzt, dann Authorization - Digest            */
/*                   return: true bei Fehler                                           */
/*     update_passwd ändert Benutzerpasswort in passwd-Datei                           */
/***************************************************************************************/
int update_passwd(char *pwddat, char *user, char *pwd, char *realm)
{ FILE *ptralt, *ptrneu;                         /* ptr auf ACCESS-Datei               */
  char r[MAX_ACC_LEN];                           /* access-Zeichen aus paswd-Datei     */
  char a[MAX_AUTH_LEN];                          /* authorize aus passwd-Datei         */
  char setuser[USER_LENGTH];                     /* User aus passwd-Datei              */
  char userpfad[MAX_LEN_FILENAME];               /* Userpfad aus passwd-Datei          */
  char *z;
  size_t nb_pwd;                                 /* Anzahl Zeichen der Passwortdatei   */
  char *dp;
  char userpwd[MAX_AUTH_LEN];
  char zeile[MAX_ZEILENLAENGE];
  int done_flag;
  char md5sum[MD5_DIGEST_LENGTH*2+1];  

  done_flag = false;

  strcpyn(userpwd, user, MAX_AUTH_LEN);
  strcatn(userpwd, ":", MAX_AUTH_LEN);
  if( realm )
  { strcatn(userpwd, realm, MAX_AUTH_LEN);
    strcatn(userpwd, ":", MAX_AUTH_LEN);
    strcatn(userpwd, pwd, MAX_AUTH_LEN);          /* String user:pwd ergaenzen         */
    LOG(7, "update_passwd, userpwd: %s.\n", userpwd);
    calc_md5sum(md5sum, (unsigned char *)userpwd, strlen(userpwd)); 
    LOG(7, "update_passwd, md5sum: %s.\n", md5sum);
  }
  else
  { strcatn(userpwd, pwd, MAX_AUTH_LEN);          /* String user:pwd ergaenzen         */
    encode_base64((unsigned char *)a, (unsigned char *)userpwd);
    calc_md5sum(md5sum, (unsigned char *)a, strlen(a));
  }

  if( NULL != (ptralt = fopen(pwddat, "r")) )       /* alte passwd-Datei öffnen        */
  { nb_pwd = fread(zeile, 1, MAX_ZEILENLAENGE-1, ptralt);
    zeile[nb_pwd] = '\0';
    fclose(ptralt);
  }
  else
  { fprintf(stderr, "Creating new password file '%s'.\n", pwddat);
    zeile[0] = '\0';
  }
  if( NULL != (ptrneu = fopen(pwddat, "w")) )       /* neue passwd-Datei öffnen        */
  { z = zeile;
    while( *z )
    { strcpyn_l(r, &z, MAX_ACC_LEN);             /* Access Zeichen nach r              */
      strcpyn_l(a, &z, MAX_AUTH_LEN);            /* Authorize String nach a            */
      strcpyn_l(setuser, &z, USER_LENGTH);       /* User nach setuser                  */
      strcpyn_l(userpfad, &z, MAX_LEN_FILENAME);  /* Userpfad nach userpfad            */

      if( NULL != (dp = strchr(a, ':')) && !strncmp(a, userpwd, dp-a+1) )
                                                 /* mit Anmeldung vergleichen          */
      { fprintf(ptrneu, "%s %s:%s %s %s\n", r, user, md5sum, setuser, userpfad);
        done_flag = true;
      }
      else
        fprintf(ptrneu, "%s %s %s %s\n", r, a, setuser, userpfad);
      while( *z && *z != '\n' && *z != '\r' )
        z++;
      while( *z && (*z == '\n' || *z == '\r') )
        z++;
    }
    if( ! done_flag )
      fprintf(ptrneu, "r %s:%s\n", user, md5sum);

    fclose(ptrneu);
    return false;
  }
  else
    fprintf(stderr, "Can't create file '%s'.\n", pwddat);

  return true;
}


/***************************************************************************************/
/* void usage(char *prg)                                                               */
/*            char *prg: Name, mit dem Programm aufgerufen wurde                       */
/*      usage zeigt Aufrufparameter                                                    */
/***************************************************************************************/
void usage(char *prg)
{ char *p;

  if( (p=strrchr(prg, '/')) )
    p++;
  else
    p = prg;

  fprintf(stderr, "usage: %10s [-r <realm>] [-p <pwd>] <u2w-pwdfile> <user>\n", p);
  fputs(" -r <realm>   : set realm and use digest access authentication\n"
        " -p <pwd>     : password\n"
        " <u2w-pwdfile>: U2W password file\n"
        " <user>       : username\n", stderr);
}


/***************************************************************************************/
/* int readpwd(char *zeile, size_t len)                                                */
/*             char *zeile: Platz fuer eingelesene Zeichen                             */
/*             size_t len: Laenge von zeile                                            */
/*             return: -1 bei Fehler, 0 sonst                                          */
/***************************************************************************************/
int readpwd(char *zeile, size_t len)
{ struct termios tsave, tset;
  char c, *z;

  z = zeile;
  if( tcgetattr(0, &tsave) )
    return -1;

  tset = tsave;
  tset.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
  if( tcsetattr(0, TCSAFLUSH, &tset) )
    return -1;

  while( (c = getchar()) != '\n' && (z-zeile) < len - 1 )
  { if( c == 127 )
    { if( z > zeile )
      { z--;
        fprintf(stderr, "\b \b");
      }
    }
    else
    { *z++ = c;
      fprintf(stderr, "*");
    }
  }
  fprintf(stderr, "\n");
  *z = '\0';
  tcsetattr(0, TCSAFLUSH, &tsave);
  return 0;
}

int main(int argc, char **argv)
{ char pwddat[2048], user[1024], pwd[128], *realm;
  int options = 0;
  char *pwdp;

  pwdp = pwd;
  realm = NULL;

  while( argc > options+1
         && argv[options+1][0] == '-' && argv[options+1][1] != '\0' )
  { options++;
    switch( argv[options][1] )
    { case 'r': if( argc < ++options+1 )
                { usage(argv[0]);
                  return 3;
                }
                realm = argv[options];
                break;
      case 'p': if( argc < ++options+1 )
                { usage(argv[0]);
                  return 3;
                }
                pwdp = argv[options];
                break;
      case 'v': show_version(argv[0], true);
                return 0;
      default:  usage(argv[0]);
                return 1;
    }
  }

  if( argc > 1+options )
    strcpy(pwddat, argv[1+options]);
  else
  { fprintf(stderr, "Path of U2W-Passwordfile: ");
    scanf("%s", pwddat);
  }

  if( argc > 2+options )
    strcpy(user, argv[2+options]);
  else
  { fprintf(stderr, "Username to set: ");
    scanf("%s", user);
    getchar();
  }
  if( pwdp == pwd )
  { fprintf(stderr, "Password for %s: ", user);
    readpwd(pwd, 128);
  }

  update_passwd(pwddat, user, pwdp, realm);

  return 0;
}

#endif
