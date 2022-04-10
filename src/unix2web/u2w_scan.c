/********************************************************/
/* File: u2w_scan.c                                     */
/* Funktionen zum auswerten der eingelesenen Zeilen     */
/* timestamp: 2016-07-25 18:57:03                       */
/********************************************************/


#include "u2w.h"


#ifndef COMPILER


/***************************************************************************************/
/* short scan_zeile(char *out, char **s, menutype menu, long n)                        */
/*                char *out    : Ausgabe hier aufbauen                                 */
/*                char **s     : gelesene Eingabezeile                                 */
/*                menutype menu: auf Menueseiten gesetzt                               */
/*                long n       : Platz in out                                          */
/*                return: 1 bei Fehler oder Rest der Zeile ist Kommentar               */
/*     scan_zeile wertet die eingelesene Zeile s aus. Dazu werden Hyperlinks           */
/*                und Kommandoaufrufe ausgewertet                                      */
/***************************************************************************************/
short scan_zeile(char *out, char **s, menutype menu, long n)
{ short ret;                                         /* Ergebniswert                   */
  char *o;                                           /* zeigt auf das Ende der Ausgabe */
  char shell[MAX_ZEILENLAENGE];                      /* zum Aufbauen des Shellaufrufs  */
  char *p;
  static char last_headline = '\0';                  /* Merker der Headlinegröße       */
  static int last_tablecols;

  ret = false;                                       /* noch kein Fehler!              */

  LOG(1, "scan_zeile, s: %.200s.\n", *s);

  o = out;                                           /* hier wird die Ausgabe erzeugt  */
  while( **s && o-out < n-5 )
  { LOG(89, "scan_zeile - while, s: %.20s.\n", *s);
    if( **s == ESCAPE_CHAR && NULL != strchr(CHARS_TO_ESCAPE, *(*s+1)) ) /* ESCAPE Z.  */
    { (*s)++;                                        /* ueberspringen                  */
      if( **s == '&' )                               /* '&'?                           */
      { strcpyn_z(&o, "&amp;", (n-6)-(o-out));       /* Code senden                    */
        (*s)++;
      }
      else if( **s == ' ' )
      { strcpyn_z(&o, "&nbsp;", (n-7)-(o-out));
        (*s)++;
      }
      else if( **s )
        *o++ = *(*s)++;                              /* naechstes Zeichen senden       */
    }
    else if( (**s == '<' || **s == '>') && u2w_mode == U2W_MODE )
    { *o++ = '&';
      *o++ = *(*s)++ == '<' ? 'l' : 'g';
      *o++ = 't';
      *o++ = ';';
    }
    else if( **s == PAR_CHAR )
    { if( u2w_mode == U2W_MODE )
      { p = zeile;
        getpar(&o, s, n-(o-out)-1, '\0', QF_HTML);
      }
      else if( u2w_mode >= S2W_MODE )
        getpar(&o, s, n-(o-out)-1, '\0', QF_NONE);
      else
        *o++ = *(*s)++;
    }
    else if( kzaehler == 0 && u2w_mode != U2W_HTML_MODE
             && file_flag && (**s == SHELL_ESCAPE || **s == SHELL_INLINE) )
                                                     /* Programmaufruf?                */
    { if( NULL != (p = strchr(*s+1, **s)) )          /* Inline wie #...# oder `...`?   */
      { int bin_flag;

        bin_flag = SHELL_ESCAPE == *(*s)++;          /* Escape-Char                    */
        *p = '\0';
        appstr(shell, s, MAX_ZEILENLAENGE);
        if( *shell )
          ret = inline_shell(shell, &o, n - (o-out), bin_flag, u2w_mode != S3W_MODE)
                || ret;
        else
        { logging(_("Error: Buffer overflow on calling shell.\n"));
          ret = true;
        }
        *s = p+1;
      }
      else
      { if( o == out )
        { (*s)++;
          appstr(shell, s, MAX_ZEILENLAENGE);        /* Programmaufruf bestimmen       */
          if( *shell )
            doshell(shell, "", u2w_mode == U2W_MODE,
                    u2w_mode != S2W_MODE, u2w_mode == S3W_MODE);    /* ausführen       */
          else
            logging(_("Error: Buffer overflow on calling shell.\n"));
        }
        else
        { *o = '\0';
          return ret;
        }
      }
    }
    else if( **s == ATTRIB_CHAR )
    { LOG(4, "scan_zeile, ATTRIB_CHAR, s: %.200s.\n", *s);

      (*s)++;

      if( u2w_mode == U2W_HTML_MODE )
      { if( is_command_z(s, HTML_OFF) )
        { u2w_mode = U2W_MODE;
          tablecols = last_tablecols;
        }
        else
          *o++ = ATTRIB_CHAR;
        continue;
      }

      if( **s == REM_CHAR )
      { *o = '\0';
        return true;
      }
#ifdef WITH_GETTEXT
      else if( **s == GETTEXT )
        scan_gettext_z(&o, s, n-(o-out), "", '\0', QF_NONE);
      else if( **s == GETTEXTCONST )
        scan_gettextconst_z(&o, s, n-(o-out), "", '\0', QF_NONE);
      else if( **s == GETTEXTFKT )
        scan_gettextfkt_z(&o, s, n-(o-out), "", '\0', QF_NONE);
#endif  /* WITH_GETTEXT */
      else if( **s == ' ' )
      { strcpyn_z(&o, "&nbsp;", (n-6)-(o-out));
        (*s)++;
      }
      else if( **s == CALC )
      { (*s)++;
        scan_calc(&o, s, n-(o-out));
      }
      else if( is_command(*s, HTML_ON) && *(*s+strlen(HTML_ON)) != '(' )
      { *s += strlen(HTML_ON);
        if( **s == ' ' )
          (*s)++;
        u2w_mode = U2W_HTML_MODE;
        last_tablecols = tablecols;
        tablecols = 0;
        continue;
      }
      else if( is_num_befehl_z(s, HL) )
      { LOG(20, "scan_zeile, HL, Num: %c.\n", **s);
        if( '1' <= **s && **s <= '6' )
        { last_headline = **s;
          strcpyn_z(&o, "<h", n-(o-out));
          *o++ = **s;
          if( css_defs[CSS_H1+**s-'1'] )
            o+= snprintf(o, n-(o-out), " class=\"%s\">", css_defs[CSS_H1+**s-'1']);
          else if( n-(o-out) > 1 )
            *o++ = '>';
          if( *++(*s) == ' ' )
            (*s)++;
        }
        else
          last_headline = '\0';
      }
      else if( is_befehl_z(s, AUS HL) )
      { if( last_headline && n-(o-out) > 5 )
        { if( **s == ' ' )
            (*s)++;
          strcpyn_z(&o, "</h", n-(o-out));
          *o++ = last_headline;
          last_headline = '\0';
          if( n-(o-out) > 1 )
            *o++ = '>';
        }
      }
      else
      { unsigned int tblid, fktid;
        char cmd[MAX_PAR_NAME_LEN];

        get_command_z(s, cmd);
        if( *cmd && !get_u2w_hash(cmd, T_U2W_COMMAND_PROGS|T_SCAN_PROGS|T_SCAN_SEND_PROGS
                                       |T_SCAN_ONLY_SEND_PROGS|T_FORMAT|T_U2W_TABLE_PROGS,
                                  &tblid, &fktid) )
        { LOG(9, "scan_zeile, cmd: %s, tblid: %u, fktid: %u, last_char_sended: %d.\n",
              cmd, tblid, fktid, (int)last_char_sended);
          switch(tblid)
          {
#ifdef WEBSERVER
            case T_U2W_COMMAND_PROGS:
              if( o != out )
              { *o = '\0';
                last_char_sended = *(o-1);
                dosend(out);
                o = out;
              }
              ret = do_x2w_token_command(s, u2w_command_progs[fktid]) || ret;
              continue;
            case T_U2W_TABLE_PROGS:
              if( o != out )
              { *o = '\0';
                last_char_sended = *(o-1);
                dosend(out);
                o = out;
                if( tablecols && last_char_sended != '\1' )
                { if( tablecols < 0 )
                  { if( dosend(TABLE_COL_HEAD_END) )
                      return true;
                  }
                  else if( dosend(TABLE_COL_END) )
                    return true;
                }
              }
              ret = do_x2w_command(s, u2w_table_progs[fktid]) || ret;
              if( **s && tablecols )
              { unsigned int tblid;

                if( **s == ATTRIB_CHAR )
                { if( *(*s+1) == REM_CHAR )
                    tblid = T_U2W_TABLE_PROGS;
                  else
                  { unsigned int fktid;
                    char cmd[MAX_PAR_NAME_LEN], *p;

                    p = *s+1;
                    get_command_z(&p, cmd);
                    if( get_u2w_hash(cmd, T_U2W_TABLE_PROGS, &tblid, &fktid) )
                      tblid = 0;
                  }
                }
                else
                  tblid = 0;
                if( tblid != T_U2W_TABLE_PROGS )
                { if( tablecols < 0 )
                  { if( css_defs[CSS_TH] )
                    { if( dosendf(TABLE_COL_HEAD_CLASS_START, css_defs[CSS_TH]) )
                        return true;
                    }
                    else
                    { if( dosend(TABLE_COL_HEAD_START) )
                        return true;
                    }
                  }
                  else if( css_defs[CSS_TD] )
                  { if( dosendf(TABLE_COL_CLASS_START, css_defs[CSS_TD]) )
                      return true;
                  }
                  else
                  { if( dosend(TABLE_COL_START) )
                      return true;
                  }
                }
              }
              continue;
#endif
            case T_SCAN_PROGS:
              ret = do_scan_command(&o, s, n-(o-out), scan_progs[fktid],
                                    u2w_mode == U2W_MODE ? QF_HTML : QF_NONE)
                    || ret;
              continue;
            case T_SCAN_SEND_PROGS:
              if( o != out )
              { *o = '\0';
                last_char_sended = *(o-1);
                dosend(out);
                o = out;
              }
              ret = do_scan_command(NULL, s, 0, scan_send_progs[fktid],
                                    u2w_mode == U2W_MODE ? QF_HTML : QF_NONE)
                    || ret;
              continue;
            case T_SCAN_ONLY_SEND_PROGS:
              if( o != out )
              { *o = '\0';
                last_char_sended = *(o-1);
                dosend(out);
                o = out;
              }
              ret = do_scan_prog_send_command(NULL, s, 0, scan_only_send_progs[fktid],
                                    u2w_mode == U2W_MODE ? QF_HTML : QF_NONE)
                    || ret;
              continue;
            case T_FORMAT:
              if( **s == ' ' )
                (*s)++;
              if( u2w_mode == U2W_MODE )
              { if( format_commands[fktid].numhtml[0] && isalnum(0|**s) )
                { p = zeile;
                  while( isalnum(0|**s) && p-zeile < MAX_ZEILENLAENGE )
                    *p++ = *(*s)++;
                  if( **s == ' ' )
                    (*s)++;
                  *p = '\0';
                  o += snprintf(o, n-(o-out), format_commands[fktid].numhtml, zeile);
                }
                else
                  strcpyn_z(&o, format_commands[fktid].html, n-(o-out));
              }
              else
                strcpyn_z(&o, format_commands[fktid].tty, n-(o-out));
              continue;
          }
        }
        *o++ = ATTRIB_CHAR;
        strcpyn_z(&o, cmd, n-(o-out));
      }
    }
    else if( tablecols && **s == TABLE_SEP )
    { (*s)++;
      LOG(7, "scan_zeile, tablecols: %d, akt_tablecols: %d, last_char_sended: %c.\n",
          tablecols, akt_tablecols, last_char_sended);
      if( tablecols < 0 )
      { if( css_defs[CSS_TH] )
          o += snprintf(o, n-(o-out), TABLE_COLUMN_HEAD_CLASS_TRENN, css_defs[CSS_TH]);
        else
          strcpyn_z(&o, TABLE_COLUMN_HEAD_TRENN, n-(o-out));
      }
      else
      { if( css_defs[CSS_TD] )
          o += snprintf(o, n-(o-out), TABLE_COLUMN_CLASS_TRENN, css_defs[CSS_TD]);
        else if( akt_tablecols < aligntablecols[tablelevel-1] )
        { if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'r' )
            strcpyn_z(&o, TABLE_COLUMN_TRENN_RIGHT, n-(o-out));
          else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'c' )
            strcpyn_z(&o, TABLE_COLUMN_TRENN_CENTER, n-(o-out));
          else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'L' )
            strcpyn_z(&o, TABLE_COLUMN_TRENN_LEFT, n-(o-out));
          else
            strcpyn_z(&o, TABLE_COLUMN_TRENN, n-(o-out));
        }
        else
          strcpyn_z(&o, TABLE_COLUMN_TRENN, n-(o-out));
      }
      if( **s == ' ' || **s== '\t' )
      { (*s)++;
        strcpyn_z(&o, "&nbsp;", n-(o-out));
      }
      akt_tablecols++;
    }
    else
    { if( u2w_mode <= U2W_HTML_MODE )                /* u3w oder u2w_html-Modus?       */
      { if( **s == '<' )                             /* '<...>' Bloecke beachten       */
          kzaehler++;                                /* '<...>' Block betreten         */
        else if( **s == '>' && kzaehler > 0)         /* '<...>' Block verlassen?       */
          kzaehler--;                                /* ja                             */
      }
      *o++ = *(*s)++;                                /* ein Zeichen uebernehmen        */
    }
  }
  LOG(51, "scan_zeile, tablecols: %d.\n", tablecols);
  *o = '\0';                                         /* Zeilenende                     */
  return ret;
}


/***************************************************************************************/
/* void do_scan(char *in, menutype menu)                                               */
/*              char *in     : gelesene Eingabezeile                                   */
/*              menutype menu: auf Menueseiten gesetzt                                 */
/*              int tablecols: Spalten einer Tabelle (-Zahl, im Tablehead)             */
/*      do_scan wertet die eingelesene Zeile in aus und sendet das Ergebnis            */
/***************************************************************************************/
void do_scan(char *in, menutype menu)
{ char out[MAX_ZEILENLAENGE];
  short ret;
  char *s;

  LOG(1, "do_scan, akt_tablecols: %d, in: %.200s, last_char_sended: %x.\n",
      akt_tablecols, in, (int)last_char_sended);

  if( tablecols )
  { if( tablecols < 0 )
    { if( css_defs[CSS_TH] )
      { if( dosendf(TABLE_COL_HEAD_CLASS_START, css_defs[CSS_TH]) )
          return;
      }
      else
      { if( dosend(TABLE_COL_HEAD_START) )
          return;
      }
    }
    else if( css_defs[CSS_TD] )
    { if( dosendf(TABLE_COL_CLASS_START, css_defs[CSS_TD]) )
        return;
    }
    else if( akt_tablecols < aligntablecols[tablelevel-1] )
    { if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'r' )
      { if( dosend(TABLE_COL_START_RIGHT) )
          return;
      }
      else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'c' )
      { if( dosend(TABLE_COL_START_CENTER) )
          return;
      }
      else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'L' )
      { if( dosend(TABLE_COL_START_LEFT) )
          return;
      }
      else
      { if( dosend(TABLE_COL_START) )
          return;
      }
    }
    else
    { if( dosend(TABLE_COL_START) )
        return;
    }

    akt_tablecols++;
  }
  else
    last_char_sended = '\n';

  s = in;

  LOG(11, "do_scan, a - last_char_sended: %x.\n", (int)last_char_sended);
  do
  { ret = scan_zeile(out, &s, menu, MAX_ZEILENLAENGE);
    if( out[0] )
      dosend(out);                                   /* Zeile abschicken               */
    LOG(4, "do_scan, ret: %d, out: %.200s.\n", ret, out);
    LOG(11, "do_scan, b - last_char_sended: %x.\n", (int)last_char_sended);
  } while( *s && !ret );

  LOG(19, "do_scan, c - u2w_mode: %d, newlineflag: %d.\n", u2w_mode, newlineflag);
  LOG(19, "do_scan, d - in: %s, out: %s.\n", in, out);
  if( tablecols )
  { if( last_char_sended != '\1' )
    { if( tablecols < 0 )
        dosend(TABLE_COL_HEAD_END);
      else
        dosend(TABLE_COL_END);
    }
  }
  else
  { if( !*in )
    { if( u2w_mode == U2W_MODE )
        dosend(PAREND);
      else if( u2w_mode == S2W_MODE && newlineflag )
        dosend("\n");
    }
    else if( u2w_mode & (S2W_HTTPHEAD_MODE|RAW_HTTPHEAD_MODE) )
      dosend(CRLF);
    else if( (u2w_mode == U2W_MODE
              || (u2w_mode == S2W_MODE && (out[0] || last_char_sended != '\n'))
              || u2w_mode == U2W_HTML_MODE)
             && newlineflag )
      dosend("\n");
  }
  LOG(2, "/do_scan.\n");
}


/***************************************************************************************/
/* void do_scan_lineend(char *in, menutype menu)                                       */
/*              char *in     : gelesene Eingabezeile                                   */
/*              menutype menu: auf Menueseiten gesetzt                                 */
/*              int tablecols: Spalten einer Tabelle (-Zahl, im Tablehead)             */
/*      do_scan_lineend wertet Rest der Zeile in aus und sendet das Ergebnis           */
/***************************************************************************************/
void do_scan_lineend(char *in, menutype menu)
{ char out[MAX_ZEILENLAENGE];
  int ret;
  char *s;

  LOG(1, "do_scan_lineend, in: %.200s.\n", in);

  s = in;

  do
  { ret = scan_zeile(out, &s, menu, MAX_ZEILENLAENGE);
    if( out[0] )
      dosend(out);                                   /* Zeile abschicken               */
    LOG(4, "do_scan, out: %.200s.\n", out);
  } while( *s && !ret );

  LOG(12, "do_scan_lineend, tablecols: %d, last_char_sended: %d.\n",
      tablecols, last_char_sended);

  if( tablecols )
  { if( last_char_sended != '\1' )
    { if( tablecols < 0 )
        dosend(TABLE_COL_HEAD_END);
      else
        dosend(TABLE_COL_END);
    }
  }
  else
  { if( !*in )
    { if( u2w_mode == U2W_MODE )
        dosend(PAREND);
      else if( u2w_mode == S2W_MODE )
        dosend("\n");
    }
    else if( u2w_mode == U2W_MODE
             || (u2w_mode == S2W_MODE && (out[0] || last_char_sended != '\n'))
             || u2w_mode == U2W_HTML_MODE )
      dosend("\n");
  }
  LOG(2, "/do_scan_lineend.\n");
}


/***************************************************************************************/
/* void scan_to_teil(char *out, char *in, long n)                                      */
/*                   char *in: Teile der gelesenen Eingabezeile                        */
/*                   long n    : Platz in out                                          */
/*      scan_to_teil wertet den Teil innerhalb %{..}, #..#, `..` aus                   */
/***************************************************************************************/
void scan_to_teil(char *out, char *in, long n)
{ char zeile[MAX_ZEILENLAENGE];                      /* Platz für Aufrufe              */
  char *s;                                           /* Zeiger auf teil                */
  char *o;                                           /* zeigt auf das Ende der Ausgabe */
  char *p;

  LOG(1, "scan_to_teil, teil: %.200s.\n", in);

  s = in;
  o = out;                                           /* hier wird die Ausgabe erzeugt  */
  while( *s && o-out < n-1 )
  { if( *s == ESCAPE_CHAR && NULL != strchr(CHARS_TO_ESCAPE, *(s+1)) )   /* ESCAPE Z.  */
    { s++;                                           /* ueberspringen                  */
      if( *s == '&' )                                /* '&'?                           */
      { strcpyn_z(&o, "&amp;", (n-5)-(o-out));       /* Code senden                    */
        s++;
      }
      else if( *s == ' ' )
      { strcpyn_z(&o, "&nbsp;", (n-6)-(o-out));
        s++;
      }
      else if( *s )
        *o++ = *s++;                                 /* naechstes Zeichen senden       */
    }
    else if( *s == PAR_CHAR )
      getpar(&o, &s, n-(o-out)-1, '\0', QF_NONE);
    else if( file_flag && (*s == SHELL_ESCAPE || *s == SHELL_INLINE) )
                                                     /* Programmaufruf?                */
    { int bin_flag;

      bin_flag = SHELL_ESCAPE == *s;
      if( NULL != (p = strchr(s+1, *s)) )            /* Abschliessendes SHELL_*        */
      { s++;                                         /* Escape-Char                    */
        *p = '\0';                                   /* Ende des Shell-Aufrufes        */
        appstr(zeile, &s, MAX_ZEILENLAENGE);
        if( *zeile )
          inline_shell(zeile, &o, n - (o-out), bin_flag, false);
        else
          logging(_("Error: Buffer overflow on calling shell.\n"));
        s = p+1;
      }
      else                                           /* nein, bis Zeilenende           */
      { s++;
        appstr(zeile, &s, MAX_ZEILENLAENGE);
        if( *zeile )
          inline_shell(zeile, &o, n - (o-out), bin_flag, false);
        else
          logging(_("Error: Buffer overflow on calling shell.\n"));
        s += strlen(s);
      }
    }
    else if( *s == ATTRIB_CHAR )
    { s++;
      LOG(7, "scan_to_teil - nach ATTRIB_CHAR, s: %.200s.\n", s);
      if( *s == REM_CHAR )
        break;
#ifdef WITH_GETTEXT
      else if( *s == GETTEXT )
        scan_gettext_z(&o, &s, n-(o-out), "", '\0', QF_NONE);
      else if( *s == GETTEXTCONST )
        scan_gettextconst_z(&o, &s, n-(o-out), "", '\0', QF_NONE);
      else if( *s == GETTEXTFKT )
        scan_gettextfkt_z(&o, &s, n-(o-out), "", '\0', QF_NONE);
#endif  /* WITH_GETTEXT */
      else if( *s == ' ' )
      { strcpyn_z(&o, "&nbsp;", (n-6)-(o-out));
        s++;
      }
      else if( *s == CALC )
      { s++;
        scan_calc(&o, &s, n-(o-out));
      }
      else if( is_command_z(&s, NL) )
      { if( u2w_mode == U2W_MODE )
          strcpyn_z(&o, LINEEND, n-(o-out));
        else
          *o++ = '\n';
      }
      else
      { unsigned int tblid, fktid;
        char cmd[MAX_PAR_NAME_LEN];  
        
        get_command_z(&s, cmd);
        if( *cmd && !get_u2w_hash(cmd, T_SCAN_PROGS|T_SCAN_SEND_PROGS|T_SCAN_SEND_PROGS_OUT|T_FORMAT,
                                  &tblid, &fktid) )
        { LOG(9, "scan_to_teil, cmd: %s, tblid: %u, fktid: %u.\n",
              cmd, tblid, fktid);
          switch(tblid)
          { case T_SCAN_PROGS:
              do_scan_command(&o, &s, n-(o-out), scan_progs[fktid], QF_NONE);
              continue;
            case T_SCAN_SEND_PROGS:
              do_scan_command(&o, &s, n-(o-out), scan_send_progs[fktid], QF_NONE);
              continue;
            case T_SCAN_SEND_PROGS_OUT:
              do_scan_command(&o, &s, n-(o-out), scan_send_progs_out[fktid], QF_NONE);
              continue;
            case T_FORMAT:
              if( *s == ' ' )
                s++;
              strcpyn_z(&o, format_commands[fktid].tty, n-(o-out));
              *o = '\0';                             /* Zeilenende                     */
              return;
          }
        }
        *o++ = ATTRIB_CHAR;
        strcpyn_z(&o, cmd, n-(o-out));
      }
    }
    else
      *o++ = *s++;                                   /* ein Zeichen uebernehmen        */
  }
  *o = '\0';                                         /* Zeilenende                     */
  LOG(2, "/scan_to_teil, out: %.200s.\n", out);
}


/***************************************************************************************/
/* void scan_teil(char *teil, long n)                                                  */
/*                char *teil: Teile der gelesenen Eingabezeile                         */
/*                long n    : Platz in teil                                            */
/*      scan_teil wertet den Teil innerhalb %{..}, #..#, `..` aus                      */
/***************************************************************************************/
void scan_teil(char *teil, long n)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  scan_to_teil(out, teil, MAX_ZEILENLAENGE);
  strcpyn(teil, out, n);
}


#ifdef WITH_GETTEXT
/***************************************************************************************/
/* int scan_gettext_z(char **out, char **in, long n, char *endchars,                   */
/*             char partrenn, int quote)                                               */
/*             char **out: Ergegbnis hierinein                                         */
/*             char **in: gelesenen Eingabezeile                                       */
/*             long n    : Platz in out                                                */
/*             char *endchars: bis zu diesen Zeichen darf gelesen werden               */
/*             char partrenn: Zeichen, das Parameter ($p) umschliesst, '\0' sonst      */
/*             char quote: Zeichen, die mit erst. Zeich. in Params gequotet werden     */
/*     scan_gettext_z liest von der Eingabezeile, bis zum '"' und ruft gettext auf     */
/***************************************************************************************/
int scan_gettext_z(char **out, char **in, long n, char *endchars,
                   char partrenn, int quote)
{ char *t, *z, text[MAX_ZEILENLAENGE];

  LOG(1, "scan_gettext_z, in: %.200s.\n", *in);

  if( *(*in)++ != GETTEXT )                       /* '"' überlesen                     */
    return true;
  z = zeile;
  while( **in != GETTEXT && z-zeile < MAX_ZEILENLAENGE-1 )
  { if( !**in )
    { if( !readline(akt_ptr, false) )                    /* Zeile einlesen             */
      { *in = inzeile;
        *z++ = '\n';
        continue;
      }
      break;
    }
    if( **in == ESCAPE_CHAR )
      (*in)++;
    *z++ = *(*in)++;
  }
  if( **in )
    (*in)++;
  *z = '\0';
  LOG(7, "scan_gettext_z, dgettext(%s, %s).\n", u2wtextdomain, zeile);
  if( *zeile )
    z = dgettext(u2wtextdomain, zeile);
  else
    z = zeile;
  t = text;
  while( *z && t-text < MAX_ZEILENLAENGE-1 )
  { if( strchr("\\%'#&\"", *z)
        && (*z != '%' || *(z+1) != 'n' || isalnum(*(z+2)))
        && (*z != '%' || *(z+1) != ' ') )
      *t++ = '\\';
    *t++ = *z++;
  }
  *t = '\0';
  t = text;
  return scan_to_z(out, &t, n, 0, "", partrenn, quote);
}


/***************************************************************************************/
/* int scan_gettextconst_z(char **out, char **in, long n,                              */
/*                         char *endchars, char partrenn, int quote)                   */
/*             char **out: Ergegbnis hierinein                                         */
/*             char **in: gelesenen Eingabezeile                                       */
/*             long n    : Platz in out                                                */
/*             char *endchars: bis zu diesen Zeichen darf gelesen werden               */
/*             char partrenn: Zeichen, das Parameter ($p) umschliesst, '\0' sonst      */
/*             char quote: Zeichen, die mit erst. Zeich. in Params gequotet werden     */
/*     scan_gettextconst_z liest von der Eingabezeile, bis zum "'" und ruft gettext auf*/
/***************************************************************************************/
int scan_gettextconst_z(char **out, char **in, long n, char *endchars,
                   char partrenn, int quote)
{ char *z;

  LOG(1, "scan_gettextconst_z, in: %.200s.\n", *in);

  if( *(*in)++ != GETTEXTCONST )                  /* "'" überlesen                     */
    return true;
  z = zeile;
  while( **in != GETTEXTCONST && z-zeile < MAX_ZEILENLAENGE-1 )
  { if( !**in )
    { if( !readline(akt_ptr, false) )                    /* Zeile einlesen             */
      { *in = inzeile;
        *z++ = '\n';
        continue;
      }
      break;
    }
    *z++ = *(*in)++;
  }
  if( **in )
    (*in)++;
  *z = '\0';
  LOG(7, "scan_gettextconst_z, dgettext(%s, %s).\n", u2wtextdomain, zeile);
  if( *zeile )
    strcpyn_z(out, dgettext(u2wtextdomain, zeile), n);
  return false;
}


/***************************************************************************************/
/* int scan_gettextfkt_z(char **out, char **in, long n, char *endchars,                */
/*             char partrenn, int quote)                                               */
/*             char **out: Ergegbnis hierinein                                         */
/*             char **in: gelesenen Eingabezeile                                       */
/*             long n    : Platz in out                                                */
/*             char *endchars: bis zu diesen Zeichen darf gelesen werden               */
/*             char partrenn: Zeichen, das Parameter ($p) umschliesst, '\0' sonst      */
/*             char quote: Zeichen, die mit erst. Zeich. in Params gequotet werden     */
/*     scan_gettextfkt_z liest von der Eingabezeile, bis zum ')', wertet Teil aus und  */
/*             ruft gettext auf                                                        */
/***************************************************************************************/
int scan_gettextfkt_z(char **out, char **in, long n, char *endchars,
                      char partrenn, int quote)
{ char *z, text[MAX_ZEILENLAENGE];

  LOG(1, "scan_gettextfkt_z, in: %s.\n", *in);

  if( *(*in)++ != GETTEXTFKT )                    /* '(' überlesen                     */
    return true;
  z = text;
  if( scan_to_z(&z, in, MAX_ZEILENLAENGE, 0, GETTEXTFKTENDESTR, '\0', QF_NONE) )
    return true;
  LOG(7, "scan_gettextfkt_z - nach scan_to_z, in: %s, text: %s.", *in, text);
  if( **in == GETTEXTFKTENDE )
    (*in)++;
  LOG(7, "scan_gettextfkt_z, dgettext(%s, %s).\n", u2wtextdomain, text);
  if( *text )
    strcpyn_z(out, dgettext(u2wtextdomain, text), n);
  return false;
}
#endif  /* WITH_GETTEXT */


/***************************************************************************************/
/* int scan_to_z(char **out, char **in, long n, int tablecols, char *endchars,         */
/*             char partrenn, int quote)                                               */
/*             char **out: Ergegbnis hierinein                                         */
/*             char **in: gelesenen Eingabezeile                                       */
/*             long n    : Platz in out                                                */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             char *endchars: bis zu diesen Zeichen darf gelesen werden               */
/*             char partrenn: Zeichen, das Parameter ($p) umschliesst, '\0' sonst      */
/*             char quote: Zeichen, die mit erst. Zeich. in Params gequotet werden     */
/*     scan_to_z liest von der Eingabezeile, bis zu einem Kommando oder endchar        */
/***************************************************************************************/
int scan_to_z(char **out, char **in, long n, int tablecols, char *endchars,
              char partrenn, int quote)
{ char *o;                                           /* zeigt auf Anfang der Ausgabe   */
  int ret;
  int shell_flag;
  u2w_mode_type std_u2w_mode;              /* bei %textarea u2w_mode temporaer auf S2W */

  LOG(1, "scan_to_z, in: %.30s, quote: %d, endchars: %s.\n", *in, quote, endchars);

  std_u2w_mode = u2w_mode;
  if( quote == QF_NOHTML )
    u2w_mode = S2W_MODE;

  shell_flag = *endchars == SHELL_ESCAPE || *endchars == SHELL_INLINE;

  ret = false;
  o = *out;

  while( !*endchars || *endchars != **in )                  /* Endchar schon gefunden?*/
  { if( !**in )
    { if( *endchars == '"' || *endchars == '\'' || *endchars == ')' )
      { if( !readline(akt_ptr, false) )                     /* Zeile einlesen          */
        { *in = inzeile;
          if( *endchars == ')' )
            skip_blanks(*in);
          else
            *(*out)++ = '\n';
          continue;
        }
        else
          *in = "";
      }
      break;
    }
    if( o == *out )
    { if( **in == '"' && *endchars != '\'' )
      { (*in)++;
        ret = scan_to_z(out, in, n, tablecols, "\"", '\0', quote) || ret;
        LOG(19, "...scan_to_z, ret: %d.\n", ret);
        if( **in == '"' )
          (*in)++;
        continue;
      }
      else if( **in == '\'' && *endchars != '"' )
      { (*in)++;
        ret = read_to(out, in, n, '\'') || ret;
        if( **in == '\'' )
          (*in)++;
        continue;
      }
    }
    if( *endchars && strchr(endchars, **in) )
      break;
    if( *out-o < n-1 )                                      /* noch Platz in out?      */
    { LOG(49, "scan_to_z; vor switch, in: %.10s.\n", *in);
      switch( **in )
      { case ESCAPE_CHAR: if( !shell_flag
                              || NULL != strchr(CHARS_TO_ESCAPE, *(*in+1)) )
                            (*in)++;
                          if( **in )
                            *(*out)++ = *(*in)++;
                          break;
        case '\'':        if( shell_flag )
                          { *(*out)++ = *(*in)++;
                            while( **in && **in != '\'' )
                              *(*out)++ = *(*in)++;
                          }
                          *(*out)++ = *(*in)++;
                          break;
        case PAR_CHAR:    ret = getpar(out, in, n-(*out-o)-1, partrenn, quote) || ret;
                          break;
        case SHELL_ESCAPE: if( *endchars == '"' )      /* '#' innerhalb '"' ignorieren */
                           { *(*out)++ = *(*in)++;
                             break;
                           }
        case SHELL_INLINE: if( file_flag )
                           { (*in)++;
                             ret = scan_shell(out, in, n-(*out-o), *(*in-1)) || ret;
                           }
                           else
                             *(*out)++ = *(*in)++;      /* Zeichen uebernehmen         */
                           break;
        case '<':
        case '>':         if( quote == QF_HTML )
                          { *(*out)++ = '&';
                            *(*out)++ = *(*in)++ == '<' ? 'l' : 'g';
                            *(*out)++ = 't';
                            *(*out)++ = ';';
                          }
                          else
                            *(*out)++ = *(*in)++;      /* Zeichen uebernehmen         */
                          break;
        case ATTRIB_CHAR: (*in)++;
                          if( **in == REM_CHAR )
                            **in = '\0';
                          else if( **in == CALC )
                          { (*in)++;
                            ret = scan_calc(out, in, n-(*out-o)) || ret;
                          }
#ifdef WITH_GETTEXT
                          else if( **in == GETTEXT )
                            ret = scan_gettext_z(out, in, n-(*out-o),
                                                 endchars, partrenn, quote) || ret;
                          else if( **in == GETTEXTCONST )
                            ret = scan_gettextconst_z(out, in, n-(*out-o),
                                                 endchars, partrenn, quote) || ret;
                          else if( **in == GETTEXTFKT )
                            ret = scan_gettextfkt_z(out, in, n-(*out-o),
                                                    endchars, partrenn, quote) || ret;
#endif  /* WITH_GETTEXT */
                          else if( **in == ' ' )
                          { if( quote == QF_HTML )
                            { strcpyn_z(out, "&nbsp;", n-(*out-o));
                              (*in)++;
                            }
                            else
                              *(*out)++ = *(*in)++;
                          }
                          else if( is_command_z(in, NL) )
                          { if( quote == QF_HTML )
                              strcpyn_z(out, LINEEND, n-(*out-o));
                            else
                              *(*out)++ = '\n';
                          }
                          else
                          { unsigned int tblid, fktid;
                            char cmd[MAX_PAR_NAME_LEN];  

                            get_command_z(in, cmd);
                            if( *cmd && !get_u2w_hash(cmd, T_SCAN_PROGS|T_SCAN_SEND_PROGS
                                                           |T_SCAN_SEND_PROGS_OUT|T_FORMAT,&tblid,
                                                      &fktid) )
                            { LOG(9, "scan_to_z, cmd: %s, tblid: %u, fktid: %u.\n",
                                  cmd, tblid, fktid);
                              switch(tblid)
                              { case T_SCAN_PROGS:
                                  ret = do_scan_command(out, in, n-(*out-o),
                                                        scan_progs[fktid], quote)
                                        || ret;
                                  continue;
                                case T_SCAN_SEND_PROGS:
                                  ret = do_scan_command(out, in, n-(*out-o),
                                                        scan_send_progs[fktid], quote)
                                        || ret;
                                  continue;
                                case T_SCAN_SEND_PROGS_OUT:
                                  ret = do_scan_command(out, in, n-(*out-o),
                                                        scan_send_progs_out[fktid], quote)
                                        || ret;
                                  continue;
                                case T_FORMAT:
                                  if( **in == ' ' )
                                    (*in)++;
                                  ret = strcpyn_z(out, format_commands[fktid].tty, n-(*out-o)) || ret;
                                  continue;
                              }
                            }
                            *(*out)++ = ATTRIB_CHAR;
                            ret = strcpyn_z(out, cmd, n-(*out-o)) || ret;
                          }
                          break;
        default:          *(*out)++ = *(*in)++;      /* Zeichen uebernehmen            */
      }
    }
    else
      (*in)++;
#ifdef DEBUG
    **out = '\0';
    LOG(49, "scan_to_z, while-ende, in: %.100s.\n", *in);
    LOG(49, "scan_to_z, while-ende, o: %.100s.\n", o);
#endif
  }

  **out = '\0';                                      /* Zeilenende                     */
  LOG(2, "/scan_to_z, out: %.30s.\n", o);
  LOG(8, "/scan_to_z, in: %.30s.\n", *in);

  u2w_mode = std_u2w_mode;

  LOG(11, "/scan_to_z, ret: %d, *in: %.30s.\n", ret, *in);
  return ret || *out-o >= n-1;
}


/***************************************************************************************/
/* int scan_to(char *out, char **in, long n, int tablecols, char *endchars,            */
/*             char partrenn)                                                          */
/*             char **out: Ergegbnis hierinein                                         */
/*             char **in: gelesenen Eingabezeile                                       */
/*             long n    : Platz in out                                                */
/*             int tablecols: Spalten einer Tabelle                                    */
/*             char *endchars: bis zu diesen Zeichen darf gelesen werden               */
/*             char partrenn: Zeichen, das Parameter ($p) umschliesst, '\0' sonst      */
/*     scan_to liest von der Eingabezeile, bis zu einem Kommando oder endchar          */
/***************************************************************************************/
int scan_to(char *out, char **in, long n, int tablecols, char *endchars,
            char partrenn)
{ char *o;

  o = out;
  return scan_to_z(&o, in, n, tablecols, endchars, partrenn,
                   partrenn == '\'' ? QF_SHELL : QF_NONE);
}


/***************************************************************************************/
/* int list_scan_to(int *len, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS],         */
/*                  char **in)                                                         */
/*             int *len: Anzahl Elemente in prg_pars                                   */
/*             char list_pars: Ziel der Listelemente                                   */
/*             char **in: gelesenen Eingabezeile                                       */
/*     list_scan_to liest in eine Liste bis endchar                                    */
/***************************************************************************************/
int list_scan_to(int *len, char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS], char **in)
{ int ret;
  char *o;

  LOG(1, "list_scan_to, in: %.200s, *len: %d.\n", *in, *len);

  ret = false;

  o = list_pars[*len];
  while( **in )
  { skip_blanks(*in);

    if( **in == ',' )
    { *o = '\0';
      o = list_pars[++(*len)];
      (*in)++;
      skip_blanks(*in);
    }
    else
    { switch( **in )
      { case PAR_CHAR:    getpar(&o, in, MAX_LEN_LIST_PARS-(o-list_pars[*len])-1,
                                 '\0', QF_NONE);
                          break;
        case SHELL_ESCAPE:
        case SHELL_INLINE: if( !file_flag )
                             break;
                           (*in)++;
                           ret = scan_shell(&o, in,
                                            MAX_LEN_LIST_PARS-(o-list_pars[*len]),
                                            *(*in-1)) || ret;
                           break;
        case ATTRIB_CHAR: (*in)++;
                          if( **in == REM_CHAR )
                            **in = '\0';
                          else if( **in == CALC )
                          { (*in)++;
                            ret = scan_calc(&o, in,
                                            MAX_LEN_LIST_PARS-(o-list_pars[*len]))
                                  || ret;
                          }
                          else
                          { unsigned int tblid, fktid;
                            char cmd[MAX_PAR_NAME_LEN];  

                            get_command_z(in, cmd);
                            if( *cmd && !get_u2w_hash(cmd, T_SCAN_LIST_PROGS,
                                                      &tblid, &fktid) )
                            { LOG(9, "list_scan_to, cmd: %s, tblid: %u, fktid: %u.\n",
                                  cmd, tblid, fktid);
                              switch(tblid)
                              { case T_SCAN_LIST_PROGS:
                                  ret = do_scan_list_command(len, list_pars,
                                                             in, scan_list_progs[fktid])
                                        || ret;
                                  skip_blanks(*in);
                                  if( **in == ',' )
                                    (*in)++;
                                  skip_blanks(*in);
                                  o = list_pars[*len];
                                  continue;
                              }
                            }
                            *o++ = ATTRIB_CHAR;
                            strcpyn_z(&o, cmd, MAX_LEN_LIST_PARS-(o-list_pars[*len]));
                          }
                          break;
        default:          *o++ = *(*in)++;       /* Zeichen übernehmen               */
      }
    }
  }
  *o = '\0';                                         /* Zeilenende                     */
  if( o > list_pars[*len] )
    (*len)++;
  LOG(2, "/list_scan_to, *len: %d.\n", *len);
  return ret;
}


/***************************************************************************************/
/* int read_to(char **out, char **in, long n, char endchar)                            */
/*             char **out: Ergegbnis hierinein                                         */
/*             char **in: gelesene Eingabezeile                                        */
/*             long n    : Platz in out                                                */
/*             char endchar: bis zu diesem Zeichen darf gelesen werden                 */
/*     read_to liest von der Eingabezeile, bis zu endchar                              */
/***************************************************************************************/
int read_to(char **out, char **in, long n, char endchar)
{ char *o;

  LOG(1, "read_to, endchar: %c, in: %.200s.\n", endchar, *in);

  o = *out;

  while( **in != endchar && (*out - o) < n )
  { if( !**in )
    { if( endchar == '"' || endchar == '\'' || endchar == '}' )
      { if( !readline(akt_ptr, false) )                       /* Zeile einlesen        */
        { *in = inzeile;
          *(*out)++ = '\n';
          continue;
        }
      }
      return true;
    }
    if( endchar == '\'' || endchar == '"' )
      *(*out)++ = *(*in)++;
    else if( **in == '"' )
    { *(*out)++ = *(*in)++;
      read_to(out, in, n - (*out - o), '"');
      if( **in == '"' )
        *(*out)++ = *(*in)++;
      else
        return true;
    }
    else if( **in == '\'' )
    { *(*out)++ = *(*in)++;
      read_to(out, in, n - (*out - o), '\'');
      if( **in == '\'' )
        *(*out)++ = *(*in)++;
      else
        return true;
    }
    else if( **in == '{' )
    { *(*out)++ = *(*in)++;
      read_to(out, in, n - (*out - o), '}');
      if( **in == '}' )
        *(*out)++ = *(*in)++;
      else
        return true;
    }
    else if( **in == '\\' )
    { *(*out)++ = *(*in)++;
      if( **in )
        *(*out)++ = *(*in)++;
      else
        return true;
    }
    else
      *(*out)++ = *(*in)++;
  }
  return false;
}


/***************************************************************************************/
/* int scan_shell(char **out, char **in, long n, char c)                               */
/*                char **out: Ergegbnis hierinein                                      */
/*                char **in: gelesenen Eingabezeile                                    */
/*                long n    : Platz in out                                             */
/*                char c: Startzeichen '#' oder '`'                                    */
/*     scan_shell liest Block in '#' oder '`'                                          */
/***************************************************************************************/
int scan_shell(char **out, char **in, long n, char c)
{ char kommand[MAX_ZEILENLAENGE];
  char endstring[2];
  int ret;

  LOG(1, "scan_shell, in: %.200s, c: %c.\n", *in, c);

  endstring[0] = c;
  endstring[1] = '\0';

  ret = scan_to(kommand, in, MAX_ZEILENLAENGE, 0, endstring, '\'');

  if( **in == c )
  { (*in)++;
  }
  if( !ret )
    return inline_shell(kommand, out, n, c == SHELL_ESCAPE, false);
  else
  { LOG(9, "/scan_shell, return ret: %d.\n", ret);
    logging(_("Error: Buffer overflow on calling shell.\n"));
    return ret;
  }
}


/***************************************************************************************/
/* int scan_calc(char **out, char **in, long n)                                        */
/*               char **out: Ergegbnis hierinein                                       */
/*               char **in: gelesenen Eingabezeile                                     */
/*               long n    : Platz in out                                              */
/*     scan_calc wertet %{ Formel }                                                    */
/***************************************************************************************/
int scan_calc(char **out, char **in, long n)
{ char *k, kommand[MAX_ZEILENLAENGE];
  wert w;

  LOG(1, "scan_calc, in: %.200s.\n", *in);

  k = kommand;
  if( read_to(&k, in, MAX_ZEILENLAENGE, '}') )
    return true;
  if( **in == '}' )
  { (*in)++;
    *k = '\0';

    LOG(3, "scan_calc, nach read_to, in: %.200s.\n", *in); 

    if( !berechne(&w, kommand) )
    { strcpyn_z(out, wert2string(w), n);
      return false;
    }
  }
  else
    strcpyn_z(out, "%{...}: '}' fehlt.", n);
  return true;
}


/***************************************************************************************/
/* int scan_getchecks(char **out, char **s, long n, char trenn)                        */
/*                     char **out: Ziel des Ergebnisses                                */
/*                     char **s  : Zeigt auf Parname                                   */
/*                     long n    : Platz in out                                        */
/*                     char trenn: Trennzeichen, das einzelne Werte umschliesst        */
/*      scan_getchecks fügt Wert einer Environmentvariablen ein                        */
/*                     Wird nur aus u2w_shell aufgerufen                               */
/***************************************************************************************/
int scan_getchecks(char **out, char **s, long n, char trenn)
{ char parname[MAX_PAR_NAME_LEN];
  char tr[50];
  char *o;
  wertetype *w;
  int pflag = false;
  char name[2*MAX_PAR_NAME_LEN], *pn;

  LOG(1,  "scan_getchecks, *s: %s.\n", *s);

  o = *out;

  tr[0] = ' ';
  tr[1] = '\0';
  if( 1 < get_prg_pars(s, 2, parname, MAX_PAR_NAME_LEN, QF_NONE, tr, 50, QF_NONE) )
    return true;

  if( *parname )
  { strcpyn(name, parname, MAX_PAR_NAME_LEN);
    pn = name + strlen(name);
    *pn++ = CHECKPARS_TRENN_CHAR;

    w = firstwert(parname, HP_BROWSER_LEVEL);

    while( w != NULL )
    { LOG(4, "scan_getchecks.\n");
      strcpyn(pn, wert2string(w->wert), MAX_PAR_NAME_LEN);
      if( strcaseeq(parwert(name, HP_BROWSER_LEVEL), "on") )
      { char *t;
        if( pflag )
          strcpyn_z(out, tr, n-(*out-o));
        else
          pflag = true;
        if( trenn && (*out-o) < n )
          *(*out)++ = trenn;
        t = *out;
        strcpyn_z(out, wert2string(w->wert), n - (*out-o));
        if( trenn )                               /* durch Trennzeichen trennen?       */
        { while( t < *out )
          { if( *t++ == trenn )                   /* alle Trennzeichen im Wert         */
              *(t-1) = '"';                       /* durch '"' ersetzen                */
          }
          *(*out)++ = trenn;
        }
      }
      w = w->next;           
    }
    LOG(1,  "/scan_getchecks, o: %.*s.\n", *out-o, o);
    return false;
  }
  else
    return true;
}


/***************************************************************************************/
/* int scan_getenv(char **out, char **s, long n)                                       */
/*                 char **out: Ziel des Ergebnisses                                    */
/*                 char **s  : Zeigt auf Parname                                       */
/*                 long n    : Platz in out                                            */
/*      scan_getenv fügt Wert einer Environmentvariablen ein                           */
/***************************************************************************************/
int scan_getenv(char **out, char **s, long n)
{ char *p, *q;

  if( **s == '(' )
  { (*s)++;
    if( NULL != (p = strchr(*s, ')')) )
    { *p = '\0';
      if( (q = getenv(*s)) )
        strcpyn_z(out, q, n);
      *s = p+1;
    }
  }
  return false;
}


/***************************************************************************************/
/* int scan_u2w_isbset(char **s)                                                       */
/*               char **s  : Zeigt auf Parname                                         */
/*     u2w_isbset prüft, ob die Browser Variable, auf die **s zeigt, gesetzt ist       */
/*               Wird nur aus u2w_shell aufgerufen                                     */
/***************************************************************************************/
int scan_u2w_isbset(char **in)
{ char par[MAX_PAR_NAME_LEN];

  if( get_prg_pars(in, 1, par, MAX_PAR_NAME_LEN, QF_NONE) )
    return true;

  if( *par )
  { return NULL == parwert(par, HP_BROWSER_LEVEL);
  }
  return true;
}
#endif  /* #ifndef COMPILER */
