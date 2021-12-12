/***********************************/
/* File: u2w_upload.c              */
/* Funktion zum Upload von Daten   */
/* timestamp: 2013-10-27 11:31:56  */
/***********************************/


#include "u2w.h"

#ifdef WEBSERVER


/***************************************************************************************/
/* int do_upload(char *file)                                                           */
/*               char *file: Name der Datei, die geschrieben werden soll               */
/*               return    : true bei Fehler                                           */
/*     do_upload schreibt Datei mit Namen file mit dem Content-Block                   */
/***************************************************************************************/
int do_upload(char *file)
{ long nb;
  char *b;
  int h;
#ifndef _LARGEFILE64_SOURCE
#define open64(a,b,c) open(a,b,c)
#else
#ifdef CYGWIN
#define open64(a,b,c) open(a,b,c)
#endif
#endif

  LOG(1, "do_upload, file: %s.\n", file);

  b = strrchr(file, '/');
  if( (NULL == b && file[0] == '.')
      || (NULL != b && *(b+1) == '.') )
  { if( logflag & NORMLOG )
      perror("open");
    send_error_page(FORBIDDEN_NUM, FORBIDDEN_DESC, "", FORBIDDEN_TEXT);
    return true;
  }

  if( !upload_x2w_flag )
  { b = strrchr(file, '.');
    if( b && (*(b+2) == '3' || *(b+2) == '2') && tolower(*(b+3)) == 'w' && !*(b+4) )
    { send_error_page(FORBIDDEN_NUM, FORBIDDEN_DESC, "", FORBIDDEN_TEXT);
      return true;
    }
  }

  if( 0 <= (h = open64(file, O_WRONLY | O_CREAT | O_TRUNC, 00644)) )
  { for(;;)
    { if( 0 < (nb = read_http(&b, 2)) )
      { LOG(100, "do_upload, nb: %ld.\n", nb);
        if( nb < write(h, b, nb) )
        { LOG(100, "do_upload, writefail.\n"); 
          send_error_page(CONFLICT_NUM, CONFLICT_DESC, "", CONFLICT_TEXT);
          close(h);
          return true;
        }
      }
      else
        break;
      LOG(4, "do_upload, nb: %ld.\n", nb);
    }
    close(h); 
    return send_error_page(CREATED_NUM, CREATED_DESC, "", CREATED_TEXT);
  }
  else
  { if( logflag & NORMLOG )
      perror("open");
    send_error_page(FORBIDDEN_NUM, FORBIDDEN_DESC, "", FORBIDDEN_TEXT);
    return true;
  }
}


/***************************************************************************************/
/* void get_put_file(void)                                                             */
/*               return    : true bei Fehler                                           */
/*     get_put_file schreibt PUT Daten in Datei und setzt putdata                      */
/***************************************************************************************/
void get_put_file(void)
{ long nb;
  char *b;
  int h;
  char pathname[MAX_PATH_LEN+30];

#ifndef _LARGEFILE64_SOURCE
#define open64(a,b,c) open(a,b,c)
#endif

  LOG(1, "get_put_file.\n");

  snprintf(pathname, MAX_PATH_LEN+30, "%s/u2w_put_data.%u", upload_post_path, getpid());

  if( 0 <= (h = open64(pathname, O_WRONLY | O_CREAT | O_TRUNC, 00644)) )
  { for(;;)
    { if( 0 < (nb = read_http(&b, 2)) )
      { LOG(100, "do_upload, nb: %ld.\n", nb);
        if( nb < write(h, b, nb) )
        { LOG(100, "do_upload, writefail.\n"); 
          close(h);
          return;
        }
      }
      else
        break;
      LOG(4, "do_upload, nb: %ld.\n", nb);
    }
    putdata = store_str(pathname);
    close(h); 
  }
}
#endif  /* #ifdef WEBSERVER */
