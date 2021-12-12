/*****************************************/
/* File: u2w_dirlist.c                   */
/* Funktion zum Anzeigen von Directories */
/* timestamp: 2016-03-13 00:03:15        */
/*****************************************/


#include "u2w.h"

#ifdef WEBSERVER

#ifdef SCANDIR


/***************************************************************************************/
/* int file_name_match(const char *file, const char *match)                            */
/*                     char *file: Datei, die auf match getestet werden soll           */
/*                     char *match: Pattern für Datei                                  */
/*     file_name_match vergleicht file mit match und liefert true, wenn Dateiname dem  */
/*                     Pattern entspricht. Als Wildcards gelten nur * und ?            */
/***************************************************************************************/
int file_name_match(const char *file, const char *match)
{ const char *f, *m;
  const char *p;

  f = file;
  m = match;

  while( *m )
  { switch(*m)
    { case '*': while( *m == '*' )
                  m++;
                if( !*m )
                  return true;
                else
                { while( (p = (*m == '?' ? f : strchr(f, *m))) && *p )
                  { f = p+1;
                    if( file_name_match(p, m) )
                      return true;
                  }
                  return false;
                }
                break;
      case '?': f++;
                m++;
                break;
      default:  if( *f != *m )
                  return false;
                f++;
                m++;
    }
  }
  return !*f;
}


char *dir_select_match = NULL;

/***************************************************************************************/
/* int dir_select(const struct dirent *d)                                              */
/*                const struct dirent *d: Dirstruktur von scandir                      */
/*     dir_select liefert 1, wenn *d dir_select_match matched                          */
/***************************************************************************************/
int dir_select(const struct dirent *d)
{
  if( dir_select_match && dir_select_match[0] )
  { return file_name_match(d->d_name, dir_select_match);    
  }
  else
    return 1;
}

char *dirsortdir;

/***************************************************************************************/
/* int compare_dir_entries(const char *n1, const char *n2, int m)                      */
/*                         char *n1: Name1                                             */
/*                         char *n2: Name2                                             */
/*                         int m: 1 -> Datumsvergleich, 2 -> Größenvergleich           */
/***************************************************************************************/
int compare_dir_entries(const char *n1, const char *n2, int m)
{ char p1[MAX_PATH_LEN], p2[MAX_PATH_LEN];
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat statb1, statb2;                        /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#else
  struct stat64 statb1, statb2;                      /* fuer stat-Aufruf               */
#endif
#else
  struct stat statb1, statb2;                        /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#endif

  snprintf(p1, MAX_PATH_LEN, "%s/%s", dirsortdir, n1);
  snprintf(p2, MAX_PATH_LEN, "%s/%s", dirsortdir, n2);

  if( !stat64(p1, &statb1) && !stat64(p2, &statb2) )
  { switch(m)
    { case 1: return statb1.st_mtime - statb2.st_mtime;
              break;
      case 2: return statb1.st_blocks - statb2.st_blocks;
              break;
    }
  }
  return 0;
}


/***************************************************************************************/
/* int dir_sizesort(const struct dirent **d1, const struct dirent **d2)                */
/*                const struct dirent **d1: Dirstruktur1 von scandir                   */
/*                const struct dirent **d2: Dirstruktur2 von scandir                   */
/*     dir_sizesort liefert x<0, wenn d1 kleiner als d2                                */
/*                            0, bei gleicher Größe, x>0 sonst                         */
/***************************************************************************************/
int dir_sizesort(const struct dirent **d1, const struct dirent **d2)
{
  return compare_dir_entries((*d1)->d_name, (*d2)->d_name,  2);
}


/***************************************************************************************/
/* int dir_datsort(const struct dirent **d1, const struct dirent **d2)                 */
/*                const struct dirent **d1: Dirstruktur1 von scandir                   */
/*                const struct dirent **d2: Dirstruktur2 von scandir                   */
/*     dir_datsort liefert x<0, wenn datum der d1 vor dem datum der d2 liegt           */
/*                           0, bei gleichem datum, x>0 sonst                          */
/***************************************************************************************/
int dir_datsort(const struct dirent **d1, const struct dirent **d2)
{
  return compare_dir_entries((*d1)->d_name, (*d2)->d_name,  1);
}


/***************************************************************************************/
/* int dirlist(char *dir)                                                              */
/*             char *dir: Directoriepfad, der Angezeigt werden soll                    */
/*             return   : true bei Fehler                                              */
/*     dirlist Zeigt den Inhalt eines Directories als Links an                         */
/***************************************************************************************/
int dirlist(char *dir)
{ struct dirent **namelist;
  char datei[MAX_DIR_LEN];
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat statb;                                 /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#else
  struct stat64 statb;                               /* fuer stat-Aufruf               */
#endif
#else
  struct stat statb;                                 /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#endif
  int i, n;
  int dirflag;
  int datlen;

  LOG(1, "dirlist (dir_select), dir: %s, clientgetpath: %s.\n", dir, clientgetpath);

  if( dosendf(PAGE_HEADER PAGE_HEADER_ENDE PAGE_BODY_START, u2w_charset) )
    return true;

  http_head_flag = 3;
  if( (dir_select_match = parwert(DIR_SUCH_PATTERN, HP_BROWSER_LEVEL)) )
  {  if( dosendf(DIRLIST_START_SUCH, _("Content from"), clientgetpath, dir_select_match) )
      return true;
    n = scandir(dir, &namelist, dir_select, alphasort);
  }
  else
  { if( dosendf(DIRLIST_START, _("Content from"), clientgetpath) )
      return true;
    n = scandir(dir, &namelist, 0, alphasort);
  }
  for( i = 0; i < n; i++ )
  { if( namelist[i]->d_name[0] == '.' )
    { if( namelist[i]->d_name[1] == '\0' )
        continue;
      if( *clientgetpath == '/' && *(clientgetpath+1) == '\0' 
          && namelist[i]->d_name[1] == '.' && namelist[i]->d_name[2] == '\0' )
        continue;
    }
    snprintf(datei, MAX_DIR_LEN, "%s/%s", dir, namelist[i]->d_name);
    LOG(19, "dirlist, d_name: %s.\n", namelist[i]->d_name);
    if( !stat64(datei, &statb) )
    { strcpyn(datei, namelist[i]->d_name, MAX_DIR_LEN);
      dirflag = (S_IFDIR & statb.st_mode) == S_IFDIR;
      if( dirflag )
        strcatn(datei, "/", MAX_DIR_LEN);
      datlen = strlen(datei);
      while( datlen > 23 )
        datlen -= 6;
      free(namelist[i]);
      if( dosendf(DIRLIST_ENTRY, datei, datei, 40-datlen,
            timestr(localtime(&statb.st_mtime)), (unsigned LONGWERT)statb.st_size) )
        return true;
    }
  }
  LOG(11, "dirlist, n: %d.\n", n);
  if( n > 0 )
    free(namelist);
  return dosend(DIRLIST_END) || dosend(PAGE_END);
}

#else /* SCANDIR */

/***************************************************************************************/
/* int dirlist(char *dir)                                                              */
/*             char *dir: Directoriepfad, der Angezeigt werden soll                    */
/*             return   : true bei Fehler                                              */
/*     dirlist Zeigt den Inhalt eines Directories als Links an                         */
/***************************************************************************************/
int dirlist(char *dir)
{ DIR *dirp;
  struct dirent *direntp;
#ifdef _LARGEFILE64_SOURCE
#ifdef CYGWIN
  struct stat statb;                                 /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#else
  struct stat64 statb;                               /* fuer stat-Aufruf               */
#endif
#else
  struct stat statb;                                 /* fuer stat-Aufruf               */
#define stat64(a,b) stat(a,b)
#endif
  char datei[MAX_DIR_LEN];
  int dirflag;
  int datlen;

  LOG(1, "dirlist (readdir), dir: %s.\n", dir);

  if( dosendf(PAGE_HEADER PAGE_HEADER_ENDE PAGE_BODY_START DIRLIST_START, _("Content from"),
              u2w_charset, clientgetpath) )
    return true;

  http_head_flag = 3;
  if( (dirp = opendir(dir)) )
  { while( (direntp = readdir(dirp)) != NULL )
    { if( direntp->d_name[0] == '.' )
      { if( direntp->d_name[1] == '\0' )
          continue;
        if( *clientgetpath == '/' && *(clientgetpath+1) == '\0' 
            && direntp->d_name[1] == '.' && direntp->d_name[2] == '\0' )
          continue;
      }
      snprintf(datei, MAX_DIR_LEN, "%s/%s", dir, direntp->d_name);
      if( !stat64(datei, &statb) )
      { strcpyn(datei, direntp->d_name, MAX_DIR_LEN);
        dirflag = (S_IFDIR & statb.st_mode) == S_IFDIR;
        if( dirflag )
          strcatn(datei, "/", MAX_DIR_LEN);
        datlen = strlen(datei);
        while( datlen > 23 )
          datlen -= 6;
        if( dosendf(DIRLIST_ENTRY, datei, datei, 40-datlen,
              timestr(localtime(&statb.st_mtime)), (unsigned LONGWERT)statb.st_size) )
          return true;
      }
    }
    closedir(dirp);
  }
  return dosend(DIRLIST_END) || dosend(PAGE_END);
}
#endif /* SCANDIR */
#endif /* #ifdef WEBSERVER */
