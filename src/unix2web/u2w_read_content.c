/*******************************************/
/* File: u2w_read_content.c                */
/* Funktionen zum Lesen des Content-Blocks */
/* timestamp: 2015-03-07 18:52:22          */
/*******************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* int read_content(void)                                                              */
/*                    return        : true bei Fehler                                  */
/*     read_content Content-Block in Datei schreiben                                   */
/***************************************************************************************/
int read_content(void)
{ char *b;
  long nb;                                        /* Anzahl Zeichen im Inputpuffer     */

  LOG(1, "read_content.\n");

  if( upload_post_mode != NO_POST )               /* Uploads erlaubt?                  */
  { int h;
    char pathname[2*MAX_PATH_LEN];                /* Name der Datei aus multipart cont */

    snprintf(pathname, 2*MAX_PATH_LEN, "%s/CONTENT.%u", upload_post_path, getpid());
    LOG(5, "read_content, speichern pathname: %s.\n", pathname);
    if( upload_post_mode != POST_WRITE || test_access(pathname, user) )
    { int w_flag, dummy;

      w_flag = true;
      if( upload_post_mode == POST_WRITE )
      { test_passwd(pathname, auth_basic, &dummy, &w_flag, &dummy, NULL, NULL);
        test_hosts(pathname, clientip, &dummy, &w_flag, &dummy);
      }
      if( w_flag )
      { if( 0 <= (h = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0400)) )
        { if( auto_delete_post_files && anz_auto_delete_files < MAX_ANZ_AUTO_DELETE_FILES )
            strcpyn(auto_delete_files[anz_auto_delete_files++], pathname,
                    MAX_PATH_LEN);
          for(;;)
          { if( 0 < (nb = read_http(&b, 2)) )
            { if( nb < write(h, b, nb) )            /* und schreiben                   */
              { LOG(100, "read_content, writefail.\n");
                close(h);
                return true;
              }
            }
            else
              break;
          }
          close(h);
          content_path = store_str(pathname);
        }
        else
        { LOG(5, "read_content, Fehler: Datei %s kann nicht geöffnet werden\n", pathname);
          return true;
        }
      }
      else
      { return true;
      }
    }
    else
      return true;
  }
  else
    return true;

  LOG(2, "/read_content.\n");
  return false;
}
#endif  /* #ifdef WEBSERVER */
