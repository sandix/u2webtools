/**********************************************/
/* File: u2w_shell.c                          */
/* Funktionen zum Starten von UNIX-Funktionen */
/* timestamp: 2013-10-27 11:27:20             */
/**********************************************/

#include "u2w.h"

#ifndef COMPILER

/***************************************************************************************/
/* int do_f2s(char *kommand, char *b, long *l, long n, int error_flag)                 */
/*             char *kommand : Name des Kommandos, das ausgefuehrt werden soll         */
/*             char *b       : Ausgaben des Programmes kommand hier hinein             */
/*             long *l       : Return Anzahl Zeichen in b                              */
/*             long n        : Platz in b                                              */
/*             int error_flag: true, der Fehlerkanal wird mit ausgegeben               */
/*             return        : Ergebnis des Shell-Aufrufes                             */
/*      do_f2s fuehrt kommand aus und schreibt die Ausgabe nach b                      */
/***************************************************************************************/
int do_f2s(char *kommand, char *b, long *l, long n, int error_flag)
{ FILE *ptr;                                     /* Filepointer fuer Pipe              */
  size_t br;                                     /* Anzahl gelesener Zeichen           */
  char shell[MAX_ZEILENLAENGE+10];               /* zum Aufbauen des Shellaufrufes     */

  *l = 0;
  LOG(1, "do_f2s: %s.\n", kommand);

  strcpyn(shell, kommand, MAX_ZEILENLAENGE+10);

  if( error_flag && !(logflag & LOGNOCMDERRORS) )
    strcatn(shell, " 2>&1", MAX_ZEILENLAENGE+10);
  else
    strcatn(shell, " 2>/dev/null", MAX_ZEILENLAENGE+10);
  LOG(3, "do_f2s: shell: %s.\n", shell);
  if( NULL != (ptr = popen(shell, "r")) )        /* Programm starten                   */
  { while( !feof(ptr) && n > *l )                /* Ausgabe des Prorgramms             */
    { if( 0 < (br = fread(b+*l, 1, n-*l, ptr)) ) /* an den String anhaengen            */
      { *l += br;
      }
    }
    pclose(ptr);
    b[*l] = '\0';
  }
  else
    return true;
  LOG(2, "/do_f2s: %s.\n", b);
  return false;                                  /* Rückgabewert des Programms         */
                                                 /* Funktioniert noch nicht!!!         */
}


/***************************************************************************************/
/* int inline_shell(char *kommand, char **o, long n, int bin_flag, int error_flag)     */
/*                  char *kommand: Name des Kommandos, das ausgefuehrt werden soll     */
/*                  char **o     : Ausgaben des Programmes kommand hier hinein         */
/*                  long n       : Platz in *o                                         */
/*                  int bin_flag : true, es findet keine Zeichenersetzung statt        */
/*                  int error_flag: true, der Fehlerkanal wird mit ausgegeben          */
/*                  return       : Ergebniswert des Shell-Kommandos                    */
/*     inline_shell fuehrt kommand aus und schreibt die Ausgabe nach b, dabei werden   */
/*                  \n durch ' ' ersetzt.                                              */
/***************************************************************************************/
int inline_shell(char *kommand, char **o, long n, int bin_flag, int error_flag)
{ char *in, *oo;
  char zeile[MAX_ZEILENLAENGE];
  long l;
  int ret;

  LOG(1, "inline_shell, kommand: %s, bin_flag: %d.\n", kommand, bin_flag);

  ret = do_f2s(kommand, zeile, &l, MAX_ZEILENLAENGE, error_flag);  /* kdo ausführen    */

  LOG(3, "inline_shell, zeile: %s, ret: %d.\n", zeile, ret);

  in = zeile;
  oo = *o;

  while( in - zeile < l && *o - oo < n)
  { 
    if( bin_flag )
      *(*o)++ = *in++;
    else if( *in == '\n' )                                  /* '\n' durch              */
    { *(*o)++ = ' ';                                        /* ' ' ersetzen            */
      in++;
    }
    else if( (unsigned int)*in >= ' ' )                     /* nur druckbare Zeichen   */
      *(*o)++ = *in++;
    else
      in++;
  }
  if( !bin_flag )
    while( *(*o-1) == ' ' )                                /* Leerzeichen am Ende weg */
      (*o)--;
  LOG(2, "/inline_shell, o: %.*s.\n", (int)(*o - oo), oo);
  return ret;
}


/***************************************************************************************/
/* int appstr(char *o, char **s, long n)                                               */
/*            char  *o       : zum Aufbauen vom Programmaufruf mit Parametern          */
/*            char **s       : *s zeigt auf den gelesenen String                       */
/*            long n         : noch n Zeichen Platz in p                               */
/*            return         : true, wenn am Ende kein SHELL_ESCAPE Zeichen steht      */
/*              Bei Pufferueberlauf wird *o auf '\0' gesetzt.                          */
/*     appstr extrahiert den Namen eines aufzurufenden Programms aus der gelesenen     */
/*            Eingabezeile, *s zeigt anschliessend auf das naechste Zeichen            */
/***************************************************************************************/
int appstr(char *o, char **s, long n)
{ char *p, *po;
  int kflag = false;                        /* false: nicht in '...' Block             */
  int nf;

  LOG(1, "appstr, *s: %s.\n", *s);

  po = o;
  while( **s && **s != SHELL_ESCAPE && (po-o) < n-3 ) /* bis SHELL_ESCAPE o. Zeilenende*/
  { if( **s == '\'' )
      kflag = !kflag;
    if( kflag )
      *po++ = *(*s)++;
    else if( **s == ESCAPE_CHAR && NULL != strchr(CHARS_TO_ESCAPE, *(*s+1)) )
    { (*s)++;
      if( **s )
        *po++ = *(*s)++;
    }
    else if( **s == '"' )
    { (*s)++;
      *po++ = '\'';
      scan_to_z(&po, s, n-(po-o)-2, 0, "\"", '\0', QF_SHELL);
      *po++ = '\'';
      if( **s == '"' )
        (*s)++;
    }
    else if( **s == PAR_CHAR )
    { if( getpar(&po, s, n-(po-o)-2, '\'', QF_SHELL) )
        *o = '\0';
    }
    else if( **s == ATTRIB_CHAR )
    { (*s)++;
      LOG(19, "appstr - ATTRIB_CHAR, *s: %s.\n", *s);
      *po++ = '\'';
      if( **s == REM_CHAR )
        break;
#ifdef WITH_GETTEXT
      if( **s == GETTEXT )
        scan_gettext_z(&po, s, MAX_ZEILENLAENGE, "", '\0', QF_SHELL);
      else if( **s == GETTEXTCONST )
        scan_gettextconst_z(&po, s, MAX_ZEILENLAENGE, "", '\0', QF_SHELL);
      else if( **s == GETTEXTFKT )
        scan_gettextfkt_z(&p, s, MAX_ZEILENLAENGE, "", '\0', QF_SHELL);
      else
#endif  /* WITH_GETTEXT */
      if( 0 <= (nf = is_x2w_command_z(s, command_progs)) )
      { LOG(19, "appstr - command_progs, nf: %d.\n", nf);
        if( do_x2w_command(s, command_progs[nf]) )
          *po++ = '0';
        else
          *po++ = '1';
      }
      else if( 0 <= (nf = is_x2w_list_command_z(s, command_list_progs)) )
      { LOG(19, "appstr - command_list_progs, nf: %d.\n", nf);
        if( do_x2w_list_command(s, command_list_progs[nf]) )
          *po++ = '0';
        else
          *po++ = '1';
      }
      else if( 0 <= (nf = is_scan_command_z(s, scan_progs)) )
      { LOG(19, "appstr - scan_progs, nf: %d.\n", nf);
        do_scan_command(&po, s, MAX_ZEILENLAENGE, scan_progs[nf], QF_SHELL);
      }
      else if( 0 <= (nf = is_scan_command_z(s, scan_send_progs)) )
      { LOG(19, "appstr - scan_send_progs, nf: %d.\n", nf);
        do_scan_command(&po, s, MAX_ZEILENLAENGE, scan_send_progs[nf], QF_SHELL);
      }
      else if( 0 <= (nf = is_scan_command_z(s, scan_send_progs_out)) )
      { LOG(19, "appstr - scan_send_progs_out, nf: %d.\n", nf);
        do_scan_command(&po, s, MAX_ZEILENLAENGE, scan_send_progs_out[nf], QF_SHELL);
      }
      else
        *po++ = '%';
      *po++ = '\'';
    }
    else
      *po++ = *(*s)++;                      /* gehoert zum Programmaufruf              */
  }
  *po = '\0';
  if( **s == SHELL_ESCAPE )                 /* SHELL_ESCAPE?                           */
  { (*s)++;
    return false;
  }
  LOG(2, "/appstr, o: %s.\n", o);
  return true;
}


/***************************************************************************************/
/* int doshell(char *kommand, char *par, int form_flag, int error_flag, int binflag)   */
/*             char *kommand : Name des Kommandos, das ausgefuehrt werden soll         */
/*             char *par     : Parameter eines CGI-Aufrufes                            */
/*             int form_flag : bei true Ausgabe wird im Fixed Font gesetzt             */
/*             int error_flag: true, der Fehlerkanal wird mit ausgegeben               */
/*             int binflag   : true, es werden nicht nur Strings gesendet (Binärdaten) */
/*             return        : true bei fehler, sonst false                            */
/*     doshell fuehrt kommand aus und fuegt die Ausgebe in die HTML-Seite ein          */
/***************************************************************************************/
int doshell(char *kommand, char *par, int form_flag, int error_flag, int binflag)
{ FILE *ptr;                                     /* Filepointer fuer Pipe              */
  char inzeile[MAX_ZEILENLAENGE];                /* aus Pipe eingelesene Zeile         */
  size_t br;                                     /* Anzahl gelesener Zeichen           */
  int fd;                                        /* Filedeskriptor auf ptr             */
  char shell[MAX_ZEILENLAENGE];                  /* zum Aufbauen des Shellaufrufes     */

  LOG(1, "doshell: %s.\n", kommand);

  strcpyn(shell, kommand, MAX_ZEILENLAENGE);

  if( *par )
  { strcatn(shell, " '", MAX_ZEILENLAENGE);
    strcatn(shell, par, MAX_ZEILENLAENGE);
    strcatn(shell, "'", MAX_ZEILENLAENGE);
  }

  if( form_flag )                                /* soll Ausgabe Formatiert werden     */
    if( dosend(START_FIXED_FONT) )
      return true;
  if( error_flag && !binflag && !(logflag & LOGNOCMDERRORS) )
    strcatn(shell, " 2>&1", MAX_ZEILENLAENGE);
  else
    strcatn(shell, " 2>/dev/null", MAX_ZEILENLAENGE);

  if( NULL != (ptr = popen(shell, "r")) )      /* Programm starten                   */
  { fd = fileno(ptr);
    if( fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) < 0 )
    { if( logflag & NORMLOG )
        perror("fcntl");
    }
    while( !feof(ptr)
#ifdef WEBSERVER
           && term_flag < 2
#endif
         )                                     /* Ausgabe des Prorgramms             */
    { struct timeval tv;
      fd_set fds;
      long ret;

      do
      { tv.tv_sec = 3;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        ret = select(fd+1, &fds, NULL, NULL, &tv);
      }while( ret < 0 && errno == EINTR );

      if( ret < 0 && logflag & NORMLOG )
        perror("shell");

      if( ret > 0 && 0 < (br = fread(inzeile, 1, MAX_ZEILENLAENGE-1, ptr)) )
      { if( binflag )
        { if( dobinsend(inzeile, br) )
          { pclose(ptr);
            return true;
          }
        }
        else
        { inzeile[br] = '\0';
          last_char_sended = last_non_ws_char(inzeile, br);
          if( dosend(inzeile) )                  /* senden                             */
          { pclose(ptr);
            return true;
          }
        }
#ifdef WEBSERVER
        if( !buffer_programdata )
          send_chunk();
#endif
      }
    }

    pclose(ptr);
  }
  if( form_flag )                                 /* ggf. Ende Formatierung            */
    if( dosend(END_FIXED_FONT) )
      return true;
  LOG(1, "/doshell.\n");
  return false;
}
#endif  /* #ifndef COMPILER */
