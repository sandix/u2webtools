/***********************************/
/* File: u2wuser.c                 */
/* Funktion zum Aendern des Users  */
/* timestamp: 2015-03-07 18:54:22  */
/***********************************/


#include "u2w.h"


#ifdef UNIX
char home_env[MAX_PATH_LEN];
char logname_env[USER_LENGTH];
char user_env[USER_LENGTH];
#endif

/***************************************************************************************/
/* int change_user(struct passwd *pass, int env_flag)                                  */
/*                 struct passwd *pass: Passwd Struct aus pwd.h                        */
/*                 int env_flag: true, Environment wird gesetzt                        */
/*                 return   : true bei Fehler                                          */
/*     change_user Programm als User uid ausfuehren                                    */
/***************************************************************************************/
int change_user(struct passwd *pass, int env_flag)
{
#ifdef WEBSERVER
  int i;
#endif

  if( pass == NULL || pass->pw_uid == 0 )
    return false;

#ifdef WEBSERVER
  for( i = 0; i < anz_auto_delete_files; i++ )         /* POST-Dateien chown           */
    chown(auto_delete_files[i], pass->pw_uid, pass->pw_gid);

  if( set_user_group_flag == USERGROUPSET )
  { if( setgid(set_user_group) )
      return true;
  }
  else
  { if( set_user_group_flag == USERALLGROUPS )
    { int i = 1000;
      gid_t gl[1000];
      getgrouplist(pass->pw_name, pass->pw_gid, gl, &i);
      setgroups(i, gl);
    }
    if( setgid(pass->pw_gid) )
      return true;
  }
#else
    if( setgid(pass->pw_gid) )
      return true;
#endif

  if( !setuid(pass->pw_uid) )
  { LOG(2, "/change_user.\n");
#ifdef UNIX
    if( env_flag )
    { strcpy(home_env, "HOME=");
      strcatn(home_env, pass->pw_dir, MAX_PATH_LEN);
      strcpy(logname_env, "LOGNAME=");
      strcatn(logname_env, pass->pw_name, USER_LENGTH);
      strcpy(user_env, "USER=");
      strcatn(user_env, pass->pw_name, USER_LENGTH);
      putenv(home_env);
      putenv(logname_env);
      putenv(user_env);
    }
#endif
    return false;
  }
  
  return true;
}
