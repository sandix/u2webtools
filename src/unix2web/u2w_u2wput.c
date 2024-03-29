/*************************************************/
/* File: u2w_u2wput.c                            */
/* Funktionen zum konvertieren der *.u2w Dateien */
/* timestamp: 2017-02-25 20:01:11                */
/*************************************************/


#include "u2w.h"

/* Anfang globale Variablen */

int getflag;                                             /* true: Formular mit GET,    */
                                                         /*  false: Formular mit POST  */
FILE *ptr_stack[MAX_ANZ_INCLUDE];                        /* ptr bei %include           */
int file_flag_stack[MAX_ANZ_INCLUDE];                    /* 1: %include, 0: %input     */
int inc_anz_for_stack[MAX_ANZ_INCLUDE];                  /* anz_for_stack merken       */
int inc_anz_while_stack[MAX_ANZ_INCLUDE];                /* anz_while_stack merken     */
int inc_anz_loop_stack[MAX_ANZ_INCLUDE];                 /* anz_loop_stack merken      */
char *inz;                                               /* Leseposition in inzeile    */
char background[MAX_NAME_LEN];                           /* Dateiname Hintergrund      */
char bodypars[MAX_ZEILENLAENGE];                         /* Body Parameter             */
char title[MAX_NAME_LEN];                                /* Title                      */
int headlineflag;                                        /* Headl. im Submenue gesendet*/
u2w_mode_type html_head_u2w_mode;                        /* nach %head letzter u2w_mode*/
u2w_mode_type http_head_u2w_mode;                        /* im HTTP-Head alter u2w_mode*/
int parflag;                                             /* 1: im Parameterblock       */
                                                         /* -1: nach %endpar           */
int centerflag = 0;                                      /* 1: zentriert               */
long int last_line_seek;                                 /* Seek-Position letzte Zeile */
int tablecolsstack[TABLEROWSSTACKSIZE];                  /* Stack für tablecols        */
int akt_tablecolsstack[TABLEROWSSTACKSIZE];              /* Stack für akt_tablecols    */
char *table_aligns[TABLEROWSSTACKSIZE];                  /* Optional Align-Strings     */
int aligntablecols[TABLEROWSSTACKSIZE];
int tablelevel;                                          /* Verschachtelte Tabellen    */
int multipart_flag;                                      /* true: multipart-content    */

int html_head_flag;                                      /* HTML - Headflags           */
                                                         /* 0: noch nichts gesendet    */
                                                         /* 1: HTML-Head begonnen      */
                                                         /* 2: HTML-Head gesendet      */

#ifndef CYGWIN
#ifdef WEBSERVER
  int set_user_flag;                                     /* User noch nicht geändert   */
#endif
#endif


#ifdef WITH_MALLOC

char *inzeile = NULL;
long len_inzeile = 0;

#else  /* WITH_MALLOC */

char inzeile[MAX_ZEILENLAENGE];
long len_inzeile = MAX_ZEILENLAENGE;

#endif  /* WITH_MALLOC */
/* Ende globale Variablen */


/***************************************************************************************/
/* int readline(FILE *ptr, int nl_flag)                                                */
/*                FILE *ptr: von hier lesen                                            */
/*                int nl_flag:  RAW_MODE: Newline wird mit zurückgegeben               */
/*                             >RAW_MODE: Newline wird zu CRLF                         */
/*                return : 0 kein Fehler                                               */
/*       readline liest eine Zeile ein un gibt Zeiger auf Zeile zurück. NULL bei EOF   */
/***************************************************************************************/
int readline(FILE *ptr, int nl_flag)
{
#ifdef WITH_MALLOC
  char *z2, *z;
  int c;

  LOG(11, "readline, len_inzeile: %d, nl_flag: %d.\n", len_inzeile, nl_flag);

  if( inzeile == NULL )
  { if( NULL == (inzeile = malloc(MAX_ZEILENLAENGE) ) )
      return 1;
    len_inzeile = MAX_ZEILENLAENGE;
  }

  z = inzeile;
  for(;;)
  { do
    { *z++ = c = getc(ptr);
    } while( c != EOF && c != CR && c != LF && z - inzeile < len_inzeile);
    if( c == EOF )
    { if( z-1 == inzeile )
        return 1;
      *(z-1) = '\0';
#ifdef DEBUG
      linenumber++;
#endif
      return 0;
    }
    if( c == CR )                                 /* CR gelesen?                       */
    { c = getc(ptr);                              /* naechstes Zeichen                 */
      if( c != LF )                               /* kein LF?                          */
        ungetc(c, ptr);                           /* zuruecklegen                      */
      if( nl_flag >= RAW_MODE )                   /* CR/LF auch zurueckgeben           */
      { if( c == LF || nl_flag > RAW_MODE )       /* LF gelesen oder CRLF zurueck?     */
          *z++ = LF;                              /* LF auch zurueck                   */
        *z = '\0';
      }
      else
        *(z-1) = '\0';                            /* sonst CR abschneiden              */
#ifdef DEBUG
      linenumber++;
#endif
      return 0;
    }
    else if( c == LF )                            /* LF gelesen?                       */
    { c = getc(ptr);                              /* naechstes Zeichen                 */
      if( c != CR )                               /* kein CR?                          */
        ungetc(c, ptr);                           /* dann zuruecklegen                 */
      if( nl_flag >= RAW_MODE )                   /* CR/LF auch zurueckliefern         */
      { if( nl_flag > RAW_MODE )                  /* Zeilenende zu CRLF machen?        */
        { *(z-1) = CR;
          *z++ = LF;
        }
        else if( c == CR )                        /* folgezeichen auf LF ist CR?       */
          *z++ = c;                               /* dann auch zurueck                 */
        *z = '\0';
      }
      else
        *(z-1) = '\0';                            /* sonst LF abschneiden              */
#ifdef DEBUG
      linenumber++;
#endif
      return 0;
    }
    LOG(9, "readline - vor malloc, n: %ld.\n", len_inzeile);
    if( NULL == (z2 = malloc(2*len_inzeile)) )
      return 0;
    memcpy(z2, inzeile, z-inzeile);
    z = z2+(z-inzeile);
    free(inzeile);
    inzeile = z2;
    len_inzeile = 2*len_inzeile;
    LOG(9, "readline - nach malloc, len_inzeile: %ld.\n", len_inzeile);
  }

#else   /* WITH_MALLOC */

  if( NULL == fgets(inzeile, MAX_ZEILENLAENGE, ptr) )
    return 1;
  if( nl_flag != RAW_MODE )
    del_cr_lf(inzeile);
  if( nl_flag > RAW_MODE )
    strcatn(inzeile, CRLF, MAX_ZEILENLAENGE);
#ifdef DEBUG
  linenumber++;
#endif
  return 0;
#endif  /* else WITH_MALLOC */  
}


#ifdef WEBSERVER
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
// verhindert: warning: enumeration value ‘...’ not handled in switch [-Wswitch]
#endif
#endif
/***************************************************************************************/
/* int u2w_send_http_header(void)                                                      */
/*             return      : true bei Fehler, false sonst                              */
/*     u2w_send_http_header HTTP-Header senden                                         */
/***************************************************************************************/
int u2w_send_http_header(void)
{
  LOG(2, "u2w_send_http_header, Header senden.\n");
  switch( html_head_u2w_mode ? html_head_u2w_mode : u2w_mode )
  { case U2W_MODE:
    case U3W_MODE:
    case U4W_MODE:
    case U5W_MODE: return send_http_header("text/html", "", true, 0, u2w_charset);
                   break;
    case S2W_MODE:
    case S4W_MODE: return send_http_header("text/plain", "", true, 0, u2w_charset);
                   break;
    case S3W_MODE:
    case S5W_MODE: return send_http_header("application/plain", "", true, 0, NULL);
  }
  return true;
}
#ifndef OLDUNIX
#if __GNUC_PREREQ(5,0)
#pragma GCC diagnostic pop
#endif
#endif
#endif


/***************************************************************************************/
/* int u2w_put(FILE *ptr)                                                              */
/*             FILE *ptr   : Datei, aus der gelesen wird                               */
/*             return      : true bei Fehler, false sonst                              */
/*     u2w_put wertet eine *.u2w Eingabedatei, die mit ptr angesprochen wird,          */
/*             aus und erzeugt eine entsprechende HTML Seite                           */
/***************************************************************************************/
int u2w_put(FILE *ptr)
{ char *p;
  unsigned int tblid, fktid;
  char cmd[MAX_PAR_NAME_LEN];  
  int cflag;                                             /* continueflag: -1: read line*/
                                                         /*               -2: no read  */
                                                         /*               -3: break    */
                                                         /*              >=0: return   */
  multipart_flag = false;
  headlineflag = false;                                  /* Headl. im Submenue gesendet*/
  http_head_u2w_mode = html_head_u2w_mode = NO_MODE;
  parflag = 0;
#ifndef CYGWIN
#ifdef WEBSERVER
  set_user_flag = false;                                 /* User noch nicht geändert   */
#endif
#endif

  LOG(1, "u2w_put.\n");

  anz_for_stack = 0;                                     /* Größe ForStack             */
  anz_while_stack = 0;                                   /* Aufsetzpunkt whileSchleife */
  anz_loop_stack = 0;
  include_counter = 0;
  akt_ptr = ptr;                                         /* aktueller File Pointer     */

  title[0] = '\0';                                       /* def. kein Title            */
  background[0] = '\0';                                  /* def. kein Hintergrund      */
  bodypars[0] = '\0';
  kzaehler = 0;                                          /* ausserhalb < >             */
  tablelevel = 0;

#ifdef WEBSERVER
  html_head_flag = 0;
  if( (p=parwert(RECHTSMENU, HP_BROWSER_LEVEL)) || parwert(LINKSMENU, HP_BROWSER_LEVEL)
      || parwert(OBENMENU, HP_BROWSER_LEVEL) )
    return send_menu(p ? p : "", "", "", "", 0);

  if( firstwert(MENUNAME, HP_BROWSER_LEVEL) )
  { if( eqbpar(MENUNAME, MENUTOP) )
      menu = INMENUHEAD;
    else if( eqbpar(MENUNAME, MENURECHTS) )
      menu = MENURIGHT;
    else if( eqbpar(MENUNAME, MENUON) || eqbpar(MENUNAME, MENUSUBFILE) )
      menu = INMENU;
    else if( eqbpar(MENUNAME, MENUSHELL) )
      menu = SUBSHELL;
    else
      menu = NOMENU;
  }
  else if( firstwert(SUBMENUPAR, HP_BROWSER_LEVEL) )
    menu = SUBITEM;
  else
    menu = NOMENU;
  LOG(5, "u2w_put, menu: %d.\n", menu);
#endif  /* #ifdef WEBSERVER */

  cflag = -1;
  inz = "";

  while( cflag != -3
#ifdef WEBSERVER
         && term_flag < 2
#endif
       )
  { LOG(11, "u2w_put, for(;;), inzeile(alt): %.10s, inz(alt): %.10s, cflag: %d.\n",
        inzeile, inz, cflag);
    LOG(21, "u2w_put, for(;;), include_counter: %d, akt_ptr: %lx.\n",
        include_counter, akt_ptr);
    LOG(13, "u2w_put, ptr_stack[0]: %lx.\n", ptr_stack[0]);
    last_line_seek = ftell(akt_ptr);
                                                  /* ggf. Zeile einlesen               */
    if( cflag != -2 && readline(akt_ptr, u2w_mode & (RAW_MODE|RAW_HTTPHEAD_MODE)) )
    { LOG(7, "u2w_put, EOF(ptr).\n");
      cflag = -1;
      if( include_counter )
      {
#ifdef MAYDBCLIENT
        mysql_free_one_res(include_counter);
#endif
        clean_local_vars(include_counter);
        fclose(akt_ptr);
        akt_ptr = ptr_stack[--include_counter];
        file_flag = file_flag_stack[include_counter];
        anz_for_stack = inc_anz_for_stack[include_counter];
        anz_while_stack = inc_anz_while_stack[include_counter];
        anz_loop_stack = inc_anz_loop_stack[include_counter];
#ifdef DEBUG
        linenumber = line_number_stack[include_counter];
#endif
        LOG(5, "u2w_put, nach fclose, include_counter: %d.\n", include_counter);
        continue;
      }
      else
      { LOG(15, "u2w_put, vor break.\n");
        break;
      }
    }
    else
    { LOG(3, "u2w_put, inzeile: %.200s, akt_tablecols: %d.\n", inzeile,
          akt_tablecols);
      LOG(7, "u2w_put, file_flag: %d, html_head_flg: %d, html_hmode: %d, http_hmode: %d.\n",
          file_flag, html_head_flag, html_head_u2w_mode, http_head_u2w_mode);
      if( print_lines_flag )
      {
#ifdef DEBUG
        if( include_counter )
          fprintf(stderr, "+ %s %4d - %s\n", curfile_stack[include_counter-1], linenumber, inzeile);
        fprintf(stderr, "+ %s %4d - %s\n", clientgetpath, linenumber, inzeile);
#else
        fprintf(stderr, "+ %s - %s\n", clientgetpath, inzeile);
#endif
      }
      inz = inzeile;
      skip_blanks(inz);

      if( !*inz && !html_head_flag )
        continue;

      if( html_head_u2w_mode )
      { if( *inz == ATTRIB_CHAR
            && (is_command(inz+1, HTML_HEAD_OFF) || is_command(inz+1, HTML_HEADS_OFF)) )
        { do_html_head_off();
          continue;
        }
        if( u2w_mode == RAW_MODE )
        { if( html_head_flag == 1 )
            dosend(inzeile);
          continue;
        }
      }
#ifdef WEBSERVER
      else if( http_head_u2w_mode )
      { if( *inz == ATTRIB_CHAR
            && (is_command(inz+1, HTTP_HEAD_OFF) || is_command(inz+1, HTTP_HEADS_OFF)) )
        { do_http_head_off();
          continue;
        }
        if( u2w_mode & RAW_MODE )
        { if( http_head_flag == 1 )
            dosend(inzeile);
          continue;
        }
      }
#endif

      LOG(19, "u2w_put, u2w_mode: %d.\n", u2w_mode);
      if( u2w_mode & (U4W_MODE | U5W_MODE | S4W_MODE | S5W_MODE) )
      { LOG(3, "u2w_put - U4W_MODE | U5W_MODE | S4W_MODE | S5W_MODE.\n");

        if( *inz == REM_CHAR && *(inz+1) == REM_CHAR )       /* Kommentarzeile         */
          continue;

        if( is_command(inz, NL) && tablecols )
        { if( akt_tablecols )
          { if( tablecols < 0 )
              tablecols = -tablecols;
            if( akt_tablecols > tablecols )
              tablecols = akt_tablecols;
            akt_tablecols = 0;
            if( (p=parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
            { if( dosend(p) )
                return true;
            }
            else if( u2w_mode == U2W_MODE && dosend(TABLE_ROW_END) )
              return true;
          }
          continue;
        }

        get_command_z(&inz, cmd);
        LOG(11, "u2w_put - get_command-A, cmd: %s.\n", cmd);

        if( *cmd && !get_u2w_hash(cmd, T_U2W_PUT_PRE_HEADER|T_COMMAND_PROGS
                                       |T_COMMAND_LIST_PROGS|T_U2W_PUT_HEADER
                                       |T_U2W_COMMAND_PROGS|T_U2W_PUT|T_U2W_TABLE_PROGS
                                       |T_U2W_PUT_HTTP_HEADER,
                                  &tblid, &fktid) )
        { LOG(9, "u2w_put - get_hash, cmd: %s, tblid: %u, fktid: %u.\n",
              cmd, tblid, fktid);
          switch(tblid)
          { case T_U2W_PUT_PRE_HEADER:
              if( *inz == ' ' )
                inz++;
              if( u2w_put_pre_http_commands[fktid].do_fkt )
              { if( 0 <= (cflag = (*(u2w_put_pre_http_commands[fktid].do_fkt))()) )
                  return cflag;
              }
              continue;
            case T_COMMAND_PROGS:
              do_x2w_command(&inz, command_progs[fktid]);
              continue;
            case T_COMMAND_LIST_PROGS:
              do_x2w_list_command(&inz, command_list_progs[fktid]);
              continue;
#ifdef WEBSERVER
          }

          /*****************************************************************************/
          /* erst mal den HTTP-Header senden                                           */
          /*****************************************************************************/

          LOG(15, "u2w_put, Http-Header, http_head_flag: %d.\n", http_head_flag);
          if( http_head_flag == 0 )                      /* HTTP-Header schon gesendet?*/
          {
#ifndef CYGWIN
            if( !set_user_flag && set_user_mode == U2WSETUSER && defuser )
            { set_user_flag = true;
              if( set_user(defuser, false) )
                return true;
            }
#endif
            if( u2w_send_http_header() )
              return true;
            if( headflag )                               /* Methode HEAD?              */
              return false;                              /* nur HTTP Header senden     */
          }

          /*****************************************************************************/
          /* ab hier Kommandos, die Ausgaben nach dem HTTP-Header senden               */
          /*****************************************************************************/

          if( tblid == T_U2W_PUT_HTTP_HEADER )
          { if( *inz == ' ' )
              inz++;
            if( u2w_put_http_header_commands[fktid].do_fkt )
            { if( 0 <= (cflag = (*(u2w_put_http_header_commands[fktid].do_fkt))()) )
                return cflag;
            }
            continue;
          }

          /*****************************************************************************/
          /* kein HTTP-Head Kommando, es kann der HTML-Header begonnen werden          */
          /*****************************************************************************/

          LOG(35, "u2w_put, html_head_flag 1: %d, http_head_u2w_mode: %d.\n",
              html_head_flag, http_head_u2w_mode);
          if( html_head_flag == 0 && !http_head_u2w_mode )
          { http_head_flag |= 3;                 /* ab hier keine HTTP-Header-Daten    */
            if( (html_head_u2w_mode ? html_head_u2w_mode : u2w_mode)
                & (U3W_MODE|U2W_HTML_MODE|U2W_MODE|U4W_MODE|U5W_MODE) )
            { if( dosendf(PAGE_HEADER, u2w_charset) )
                return true;
              if( dosend("<title>") || dosend(*title ? title : clientgetfile)
                  || dosend("</title>\n") )
                return true;
              html_head_flag = 1;
            }
            else
              html_head_flag = 2;
            if( flushmode & 1 )
              send_chunk();
          }

          /*****************************************************************************/
          /* ab hier Kommandos, die Ausgaben des HTML-Headers senden                   */
          /*****************************************************************************/

          if( tblid == T_U2W_PUT_HEADER )
          { if( *inz == ' ' )
              inz++;
            if( u2w_put_header_commands[fktid].do_fkt )
            { if( 0 <= (cflag = (*(u2w_put_header_commands[fktid].do_fkt))()) )
                return cflag;
            }
            continue;
          }

          /*****************************************************************************/
          /* HTML-Header beenden und senden                                            */
          /*****************************************************************************/

          LOG(15, "u2w_put, Html-Header, html_head_flag: %d, parflag: %d.\n",
              html_head_flag, parflag);
          if( html_head_flag == 1 && html_head_u2w_mode == NO_MODE )
          { http_head_flag |= 3;                 /* ab hier keine HTTP-Header-Daten    */
            if( dosend(PAGE_HEADER_ENDE) || send_body_start() )
              return true;
            if( parflag == 1 )                           /* start_parabfrage           */
            { if( start_formular(getflag, multipart_flag,
                  *resultpage ? resultpage : clientgetfile) )
                return false;
              parflag = 2;
            }
            html_head_flag = 2;
            if( flushmode & 2 )
              send_chunk();
          }

          /*****************************************************************************/
          /* ab hier Kommandos, die Ausgaben nach dem HTML-Header senden               */
          /*****************************************************************************/

          switch(tblid)
          { case T_U2W_COMMAND_PROGS:
              if( *inz == '(' )
                break;
              if( do_x2w_token_command(&inz, u2w_command_progs[fktid]) )
                return true;
              continue;
            case T_U2W_TABLE_PROGS:
              if( *inz == ' ' )
                inz++;
              if( do_x2w_command(&inz, u2w_table_progs[fktid]) )
                return true;
              last_char_sended = '\1';
              if( *inz )
              { if( tablecols )
                { unsigned int tblid;

                  if( *inz == ATTRIB_CHAR )
                  { if( *(inz+1) == REM_CHAR )
                      tblid = T_U2W_TABLE_PROGS;
                    else
                    { unsigned int fktid;
                      char cmd[MAX_PAR_NAME_LEN], *p;

                      p = inz;
                      get_command_z(&p, cmd);
                      if( *p == ' ' )
                        p++;
                      get_u2w_hash(cmd, T_U2W_TABLE_PROGS, &tblid, &fktid);
                    }
                  }
                  else
                    tblid = 0;
                  if( tblid != T_U2W_TABLE_PROGS && last_char_sended != '\1' )
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
                    last_char_sended = '\0';
                  }
                }
                do_scan_lineend(inz, menu);                   /* Zeile auswerten       */
              }
              continue;
#endif  /* #ifdef WEBSERVER */

            case T_U2W_PUT:
              if( *inz == ' ' )
                inz++;
              if( u2w_put_commands[fktid].do_fkt )
              { if( 0 <= (cflag = (*(u2w_put_commands[fktid].do_fkt))()) )
                  return cflag;
              }
              continue;
          }
        }   /* if( *cmd && !get_u2w_hash(cmd, T_U2W_PUT_PRE_HEADER|T_COMMAND_PROGS.. */
      }
      else    /**********************************   U2W    *****************************/
      { LOG(3, "u2w_put - ! U4W...\n");
        LOG(13, "       - inz: %s.\n", inz);
        if( *inz++ == ATTRIB_CHAR )                          /* erste Zeichen '%'?     */
        { if( *inz == REM_CHAR )                             /* Kommentarzeile         */
            continue;

          if( is_command(inz, NL) && tablecols )
          { if( akt_tablecols )
            { if( tablecols < 0 )
                tablecols = -tablecols;
              if( akt_tablecols > tablecols )
                tablecols = akt_tablecols;
              akt_tablecols = 0;
              if( (p=parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
              { if( dosend(p) )
                  return true;
              }
              else if( u2w_mode == U2W_MODE && dosend(TABLE_ROW_END) )
                return true;
            }
            continue;
          }

          get_command_z(&inz, cmd);
          LOG(11, "u2w_put - get_command-B, cmd: %s.\n", cmd);

          if( *cmd && !get_u2w_hash(cmd, T_U2W_PUT_PRE_HEADER|T_COMMAND_PROGS
                                         |T_COMMAND_LIST_PROGS|T_U2W_PUT_HEADER
                                         |T_U2W_COMMAND_PROGS|T_U2W_PUT
                                         |T_U2W_TABLE_PROGS
                                         |T_U2W_PUT_HTTP_HEADER,
                                    &tblid, &fktid) )
          { LOG(9, "u2w_put - get_hash, cmd: %s, tblid: %u, fktid: %u, u2w_mode: %d.\n",
                cmd, tblid, fktid, u2w_mode);
            if( u2w_mode >= S2W_MODE
                && (tblid & T_U2W_COMMAND_PROGS
                    || (tblid & T_U2W_PUT && fktid > 0)
                    || (tblid & T_U2W_PUT_HEADER && fktid >= T_U2W_PUT_HEADER_S_ELEMENTS)) )
              continue;
            switch(tblid)
            { case T_U2W_PUT_PRE_HEADER:
                if( u2w_put_pre_http_commands[fktid].do_fkt )
                { if( *inz == ' ' )
                    inz++;
                  if( 0 <= (cflag = (*(u2w_put_pre_http_commands[fktid].do_fkt))()) )
                    return cflag;
                }
                LOG(35, "u2w_put, nach case T_U2W_PUT_PRE_HEADER\n");
                continue;
              case T_COMMAND_PROGS:
                do_x2w_command(&inz, command_progs[fktid]);
                continue;
              case T_COMMAND_LIST_PROGS:
                do_x2w_list_command(&inz, command_list_progs[fktid]);
                continue;
#ifdef WEBSERVER
            }

            /***************************************************************************/
            /* erst mal den HTTP-Header senden                                         */
            /***************************************************************************/

            LOG(15, "u2w_put, Http-Header, http_head_flag: %d.\n", http_head_flag);
            if( http_head_flag == 0 )                    /* HTTP-Header schon gesendet?*/
            {
#ifndef CYGWIN
              if( !set_user_flag && set_user_mode == U2WSETUSER )
              { set_user_flag = true;
                if( set_user(defuser, false) )
                  return true;
              }
#endif
              if( u2w_send_http_header() )
                return true;
              if( headflag )                             /* Methode HEAD?              */
                return false;                            /* nur HTTP Header senden     */
            }

            /***************************************************************************/
            /* ab hier Kommandos, die Ausgaben im HTTP-Header senden                   */
            /***************************************************************************/

            if( tblid == T_U2W_PUT_HTTP_HEADER )
            { if( *inz == ' ' )
                inz++;
              if( u2w_put_http_header_commands[fktid].do_fkt )
              { if( 0 <= (cflag = (*(u2w_put_http_header_commands[fktid].do_fkt))()) )
                  return cflag;
              }
              continue;
            }

            /***************************************************************************/
            /* kein HTTP-Head Kommando, es kann der HTML-Header begonnen werden        */
            /***************************************************************************/

            LOG(35, "u2w_put, html_head_flag 2: %d, http_head_u2w_mode: %d.\n",
                html_head_flag, http_head_u2w_mode);
            if( html_head_flag == 0 && !http_head_u2w_mode )
            { http_head_flag |= 3;               /* ab hier keine HTTP-Header-Daten    */
              if( u2w_mode & (U3W_MODE|U2W_HTML_MODE|U2W_MODE|U4W_MODE|U5W_MODE) )
              { if( dosendf(PAGE_HEADER, u2w_charset) )
                  return true;
                if( dosend("<title>") || dosend(*title ? title : clientgetfile)
                    || dosend("</title>\n") )
                  return true;
                html_head_flag = 1;
              }
              else
                html_head_flag = 2;
              if( flushmode & 1 )
                send_chunk();
            }

            /***************************************************************************/
            /* ab hier Kommandos, die Ausgaben des HTTP-Headers senden                 */
            /***************************************************************************/

            if( tblid == T_U2W_PUT_HEADER )
            { if( *inz == ' ' )
                inz++;
              if( u2w_put_header_commands[fktid].do_fkt )
              { if( 0 <= (cflag = (*(u2w_put_header_commands[fktid].do_fkt))()) )
                  return cflag;
              }
              continue;
            }

            /***************************************************************************/
            /* HTML-Header beenden und senden                                          */
            /***************************************************************************/

            LOG(15, "u2w_put, Html-Header, html_head_flag: %d, parflag: %d.\n",
                html_head_flag, parflag);
            if( html_head_flag == 1 && html_head_u2w_mode == NO_MODE )
            { http_head_flag |= 3;               /* ab hier keine HTTP-Header-Daten    */
              if( dosend(PAGE_HEADER_ENDE) || send_body_start() )
                return true;
              if( parflag == 1 )                         /* start_parabfrage           */
              { if( start_formular(getflag, multipart_flag,
                    *resultpage ? resultpage : clientgetfile) )
                  return false;
                parflag = 2;
              }
              html_head_flag = 2;
              if( flushmode & 2 )
                send_chunk();
            }

            /***************************************************************************/
            /* ab hier Kommandos, die Ausgaben nach dem HTML-Header senden             */
            /***************************************************************************/

            switch( tblid )
            { case T_U2W_COMMAND_PROGS:
                if( fktid >= T_U2W_COMMAND_PROGS_IGNORE_BLANKS && *inz == '(')
                  break;
                if( do_x2w_token_command(&inz, u2w_command_progs[fktid]) )
                  return true;
                continue;
              case T_U2W_TABLE_PROGS:
                if( do_x2w_command(&inz, u2w_table_progs[fktid]) )
                  return true;
                last_char_sended = '\1';
                if( *inz )
                { if( tablecols )
                  { unsigned int tblid;

                    if( *inz == ATTRIB_CHAR )
                    { if( *(inz+1) == REM_CHAR )
                        tblid = T_U2W_TABLE_PROGS;
                      else
                      { unsigned int fktid;
                        char cmd[MAX_PAR_NAME_LEN], *p;

                        p = inz+1;
                        get_command_z(&p, cmd);
                        if( *p == ' ' )
                          p++;
                        if( get_u2w_hash(cmd, T_U2W_TABLE_PROGS, &tblid, &fktid) )
                          tblid = 0;
                      }
                    }
                    else
                      tblid = 0;
                    if( tblid != T_U2W_TABLE_PROGS && last_char_sended != '\1' )
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
                      last_char_sended = '\0';
                    }
                  }
                  do_scan_lineend(inz, menu);                 /* Zeile auswerten       */
                }
                continue;
#endif  /* #ifdef WEBSERVER */

              case T_U2W_PUT:
                if( *inz == ' ' )
                  inz++;
                if( u2w_put_commands[fktid].do_fkt )
                { if( 0 <= (cflag = (*(u2w_put_commands[fktid].do_fkt))()) )
                    return cflag;
                }
                continue;
            }

            /***************************************************************************/


          }   /* if( *cmd && !get_u2w_hash(cmd, T_U2W_PUT_PRE_HEADER|T_COMMAND_PROGS.. */
        }   /* if( u2w_mode != U3W_MODE && *inz++ == ATTRIB_CHAR ) */

        if( menu == SUBSHELL
            || (menu == SUBITEM && html_head_flag != 0) )   /* Untermenue, dann weiter */
          continue;

#ifdef WEBSERVER
        LOG(50, "u2w_put, http_head_flag: %d, html_head_flag: %d, u2w_mode: %d.\n",
            http_head_flag, html_head_flag, u2w_mode);
        if( http_head_flag == 0 )                        /* HTTP-Header schon gesendet?*/
        {
#ifndef CYGWIN
          if( !set_user_flag && set_user_mode == U2WSETUSER )
          { set_user_flag = true;
            if( set_user(defuser, false) )
              return true;
          }
#endif
          if( u2w_send_http_header() )
            return true;
          if( flushmode & 1 )
            send_chunk();

          if( headflag )                                 /* Methode HEAD?              */
            return false;                                /* nur HTTP Header senden     */

        }
        if( html_head_flag == 0 && !http_head_u2w_mode )
        { http_head_flag |= 3;                   /* ab hier keine HTTP-Header-Daten    */
          if( u2w_mode & (U3W_MODE|U2W_HTML_MODE|U2W_MODE|U4W_MODE|U5W_MODE) )
          { if( dosendf(PAGE_HEADER, u2w_charset) )
              return true;
            if( dosend("<title>") || dosend(*title ? title : clientgetfile)
                || dosend("</title>\n") )
              return true;
            html_head_flag = 1;
          }
          else
            html_head_flag = 2;
        }
        if( html_head_flag == 1 )
        { if( u2w_mode == U2W_MODE )
          { if( dosend(PAGE_HEADER_ENDE) || send_body_start() )
              return true;
            if( parflag == 1 )                           /* start_parabfrage           */
            { if( start_formular(getflag, multipart_flag,
                  *resultpage ? resultpage : clientgetfile) )
                return false;
              parflag = 2;
            }
            html_head_flag = 2;
          }
          if( flushmode & 2 )
            send_chunk();
        }
#endif

        if( tablecols && !akt_tablecols )
        { if( (p=parwert(S2W_TABLE_ROW_START, HP_SYSTEM_LEVEL)) )
          { if( dosend(p) )
              return true;
          }
          else if( u2w_mode == U2W_MODE )
          { if( css_defs[CSS_TR] )
            { if( dosendf(TABLE_ROW_CLASS_START, css_defs[CSS_TR]) )
                return true;
            }
            else if( dosend(TABLE_ROW_START) )
              return true;
          }
        }
        do_scan(inzeile, menu);                               /* Zeile auswerten       */
        if( tablecols && newlineflag && akt_tablecols )
        { if( tablecols < 0 )
            tablecols = -tablecols;
          if( akt_tablecols > tablecols )
            tablecols = akt_tablecols;
          if( (p=parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
          { if( dosend(p) )
              return true;
          }
          else if( u2w_mode == U2W_MODE )
          { if( table_autofill_cols )
            { while( akt_tablecols < tablecols-1 )
              { if( dosend(TABLE_COL_START TABLE_COL_END) )
                  return true;
                akt_tablecols++;
              }
            }
            if( dosend(TABLE_ROW_END) )
              return true;
          }
          akt_tablecols = 0;
        }
      }
    }
    LOG(5, "u2w_put, nach if( fgets... ).\n");
  }   /* for(;;) */
  LOG(3, "u2w_put, nach for(;;).\n");


#ifdef WEBSERVER
  if( http_head_flag == 0 )                              /* HTTP-Header schon gesendet?*/
  { if( u2w_send_http_header() )
      return true;
    if( headflag )                                       /* Methode HEAD?              */
      return false;                                      /* nur HTTP Header senden     */
  }
#endif

  if( menu == SUBITEM )
  { if( (p=parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) && strchr(p, MENUSUBTRENN) )
    { char s[MAX_ZEILENLAENGE];

      strcpyn(s, p, MAX_ZEILENLAENGE);
      if( (p = strrchr(s, MENUSUBTRENN)) )
        *p = '\0';
      if( dosendf(
            "<form action=\"%s\"><input name=\"" SUBMENUPAR "\" type=  value=\"%s\">"
            "<input type=\"submit\" value=\"%s\"></form>\n", clientgetpath, s, _("   Back    ")) )
        return true;
    }
    if( dosendf(
          "<form action=\"%s\"><input name=\"" MENUNAME "\" type=\"hidden\" value=\"" MENUON "\"><input type=\"submit\" value=\"%s\"></form>\n",
             (p=parwert(HAUPTMENUPAR, HP_BROWSER_LEVEL))
             ? p
             : clientgetfile, _("Main Menu")) )
      return true;
  }

  if( eqbpar(MENUNAME, MENUSUBFILE) )
    if( send_submenufile_buttons() )
      return true;

  LOG(2, "/u2w_put.\n");

  if( tablecols )
    if( dosend("</table><br>\n") )
      return true;

  if( u2w_mode == U2W_MODE || u2w_mode == U2W_HTML_MODE )  /* nicht html-Format        */
    if( dosend(PAGE_END) )                                 /* Page Ende senden         */
      return true;

  return false;
}
