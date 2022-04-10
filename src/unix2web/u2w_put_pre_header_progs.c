/**************************************/
/* File: u2w_put_pre_header_progs.c   */
/* Funktionen aus Arrays u2w_put_*    */
/* timestamp: 2016-04-10 22:54:24     */
/**************************************/


#include "u2w.h"

/***************************************************************************************/
/* Returnwerte der do_* Kommandos:                                                     */
/* -3: exit wurde aufgerufen - Seite beenden                                           */
/* -2: Zeile in inz erneut auswerten                                                   */
/* -1: continue                                                                        */
/*  0: Verlassen ohne Fehler                                                           */
/* >0: Fehler                                                                          */
/***************************************************************************************/

long int for_seek[FOR_STACK_SIZE];                       /* Aufsetzpunkt forSchleife   */
long int while_seek[WHILE_STACK_SIZE];                   /* Groesse WhileStack         */
int loop_stack[FOR_STACK_SIZE+WHILE_STACK_SIZE];         /* Schleifenstack             */
wertetype *next_for_wert[FOR_STACK_SIZE];                /* nächster Wert              */
char *for_names[FOR_STACK_SIZE];                         /* Name der For-Variable      */
#ifdef DEBUG
int for_linenumber[FOR_STACK_SIZE];                      /* Zeilennummer merken        */
int while_linenumber[WHILE_STACK_SIZE];
#endif


/* Anfang globale Variablen */

int anz_for_stack;                                       /* Größe ForStack             */
int anz_while_stack;                                     /* Aufsetzpunkt whileSchleife */
int anz_loop_stack;

/* Ende globale Variablen */

#define WHILE_LOOP 1
#define FOR_LOOP   2


#ifdef WEBSERVER
/***************************************************************************************/
/* short do_mime(void)                                                                 */
/*     do_mime Mimetype setzen                                                         */
/***************************************************************************************/
short do_mime(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  mime = store_str(out);
  return -1;
}


/***************************************************************************************/
/* short do_setcookie(void)                                                            */
/*     do_setcoolie, String Set-Cookie: zu den http-headerzeilen hinzufügen            */
/***************************************************************************************/
short do_setcookie(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( anz_httpheadlines < MAX_ANZ_HTTPHEADLINES )
  { strcpy(out, "Set-Cookie: ");
    scan_to_teil(out+12, inz, MAX_ZEILENLAENGE);
    httpheadlines[anz_httpheadlines++] = store_str(out);
  }
  return -1;
}


/***************************************************************************************/
/* short do_charset(void)                                                              */
/*     do_charset, content-charset setzen                                              */
/***************************************************************************************/
short do_charset(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  charset = store_str(out);
  return -1;
}


/***************************************************************************************/
/* short do_httpnum(void)                                                              */
/*     do_httpnum: HTTP Response-Code setzen                                           */
/***************************************************************************************/
short do_httpnum(void)
{ char out[MAX_ZEILENLAENGE], *o;                    /* Aufbau des Ergebnisses         */

  o = out;
  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  httpnum = get_int_z(&o);
  return -1;
}


/***************************************************************************************/
/* short do_httpdesc(void)                                                             */
/*     do_httpdesc HTTP Description setzen                                             */
/***************************************************************************************/
short do_httpdesc(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  httpdesc = store_str(out);
  return -1;
}


/***************************************************************************************/
/* short do_savename(void)                                                             */
/*     do_savename Savename fuer Download setzen                                       */
/***************************************************************************************/
short do_savename(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  savename = store_str(out);
  return -1;
}


/***************************************************************************************/
/* short do_redirect(void)                                                             */
/*     do_redirect                                                                     */
/***************************************************************************************/
short do_redirect(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  char z1[MAX_ZEILENLAENGE];
  char z2[MAX_ZEILENLAENGE];

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  snprintf(z1, MAX_ZEILENLAENGE, PAGE_MOVED_PERMANENTLY_LOC, out);
  snprintf(z2, MAX_ZEILENLAENGE, PAGE_MOVED_PERMANENTLY_TXT, out);

  return send_error_page(PAGE_MOVED_PERMANENTLY_NUM, PAGE_MOVED_PERMANENTLY_DESC, z1, z2);
}


#ifndef CYGWIN
/***************************************************************************************/
/* short do_set_user(void)                                                             */
/*     do_set_user                                                                     */
/***************************************************************************************/
short do_set_user(void)
{ if( !set_user_flag )
  { char newuser[USER_LENGTH];

    strcpyn_l(newuser, &inz, USER_LENGTH);      /* Neuer User                  */
    if( strcmp(newuser, NO_SETUSER_USER) )      /* User "-" -> nicht ändern    */
    { if( set_user(newuser, true) )
        return false;
    }
    set_user_flag = true;
  }
  return -1;
}
#endif


/***************************************************************************************/
/* short do_authorize(void)                                                            */
/*     do_authorize                                                                    */
/***************************************************************************************/
short do_authorize(void)
{ LOG(40, "do_authorize, auth_mode: %d.\n", auth_mode);

  if( auth_mode != AUTH )                               /* User und PWD eingegeben?    */
  { if( *user )                                         /* Wenn Username, dann         */
      sleep(1);                                         /* Ablehnung verzögern         */
    send_authorize(false);                              /* nein, Fehlerseite           */
    return 0;
  }
  return -1;
}


/***************************************************************************************/
/* short do_page_not_found(void)                                                       */
/*     do_page_not_found                                                               */
/***************************************************************************************/
short do_page_not_found(void)
{ if( http_head_flag == 0 )
  { send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);

    return true;
  }
  return -1;
}


/***************************************************************************************/
/* short do_login(void)                                                                */
/*     do_login                                                                        */
/***************************************************************************************/
short do_login(void)
{ if( http_head_flag == 0 )                              /* nur, wenn noch kein http   */
  { send_authorize(false);
    return 0;
  }
  return -1;
}
#endif  /* WEBSERVER */


/***************************************************************************************/
/* short do_include(void)                                                              */
/*     do_include                                                                      */
/***************************************************************************************/
short do_include(void)
{ char out[MAX_ZEILENLAENGE];

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  if( include_counter < MAX_ANZ_INCLUDE )
  { ptr_stack[include_counter] = akt_ptr;
    file_flag_stack[include_counter] = file_flag;
    inc_anz_for_stack[include_counter] = anz_for_stack;
    inc_anz_while_stack[include_counter] = anz_while_stack;
    inc_anz_loop_stack[include_counter] = anz_loop_stack;
#ifdef DEBUG
    line_number_stack[include_counter] = linenumber;
#endif
    LOG(4, "do_include, in: %s, include_counter: %d.\n", out, include_counter);
    if( NULL != (akt_ptr = fopen(out, "r")) )
    { file_flag = true;
#ifdef DEBUG
      linenumber = 0;
      curfile_stack[include_counter] = store_str(out);
#endif
      include_counter++;
    }
    else
    { strcpyn(zeile, i2w_include_path, MAX_ZEILENLAENGE);
      strcatn(zeile, out, MAX_ZEILENLAENGE);
      if( NULL != (akt_ptr = fopen(zeile, "r")) )
      { file_flag = true;
#ifdef DEBUG
       linenumber = 0;
       curfile_stack[include_counter] = store_str(zeile);
#endif
       include_counter++;
      }
      else
        akt_ptr = ptr_stack[include_counter];
    }
  }
  LOG(5, "/do_include, include_counter: %d.\n", include_counter);
  return -1;
}


/***************************************************************************************/
/* short do_input(void)                                                                */
/*     do_input                                                                        */
/***************************************************************************************/
short do_input(void)
{ if( include_counter < MAX_ANZ_INCLUDE )
  { ptr_stack[include_counter] = akt_ptr;
    file_flag_stack[include_counter] = file_flag;
    inc_anz_for_stack[include_counter] = anz_for_stack;
    inc_anz_while_stack[include_counter] = anz_while_stack;
    inc_anz_loop_stack[include_counter] = anz_loop_stack;
    LOG(4, "Input, in: %s.\n", inz);
    appstr(zeile, &inz, MAX_ZEILENLAENGE);
    strcatn(zeile, " 2>/dev/null", MAX_ZEILENLAENGE);
    LOG(4, "Input, Kommando: %s.\n", zeile);
    if( NULL != (akt_ptr = popen(zeile, "r")) )
    { include_counter++;
      file_flag = false;
#ifdef DEBUG
      linenumber = 0;
#endif
    }
    else
      akt_ptr = ptr_stack[include_counter];
  }
  return -1;
}


/***************************************************************************************/
/* short do_systemcmd(void)                                                            */
/*     do_systemcmd                                                                    */
/***************************************************************************************/
short do_systemcmd(void)
{ LOG(4, "System, in: %s.\n", inz);
  appstr(zeile, &inz, MAX_ZEILENLAENGE);
  strcatn(zeile, " 2>/dev/null", MAX_ZEILENLAENGE);
  LOG(4, "System, Kommando: %s.\n", zeile);
  if( doshell(zeile, "", false, false, true) )
    return true;
  return -1;
}


/***************************************************************************************/
/* short do_continue(void)                                                             */
/*     do_continue                                                                     */
/***************************************************************************************/
short do_continue(void)
{ strcpyn_l(zeile, &inz, MAX_ZEILENLAENGE);
  if( is_elem(LOOP_STRINGS, zeile) )
  { if( skip_to_end_loop(inzeile, akt_ptr, zeile) > 0 )
    { logging("Error: %%continue without loop.\n");
      return true;
    }
  }
  else if( skip_to_end_loop(inzeile, akt_ptr, NULL) > 0 )
  { logging("Error: %%continue without loop.\n");
    return true;
  }
  inz = inzeile;
  skip_blanks(inz);
  skip_attrib_char(inz);
  LOG(13, "u2w_put - CONTINUE nach skip, inz: %s.\n", inz);
  return -2;
}


/***************************************************************************************/
/* short do_for(void)                                                                  */
/*     do_for                                                                          */
/***************************************************************************************/
short do_for(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  char *q, *p;

  if( anz_for_stack < FOR_STACK_SIZE && *inz )
  { if( *inz == PAR_PAR_CHAR )
    { if( *(inz+1) == ATTRIB_CHAR && *(inz+2) == NL_CHAR )
        q = "\n";
      else
        q = ++inz;                            /* Trennzeichen zwischen Werten*/
      skip_next_blanks(inz);
    }
    else
      q = " ";                                /* Standard: " "               */
    scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    for_seek[anz_for_stack] = ftell(akt_ptr);
#ifdef DEBUG
    for_linenumber[anz_for_stack] = linenumber;
#endif
    p = out;
    while( *p != ' ' && *p != '\t' && *p )    /* bis zum Ende des Parnamens  */
      p++;
    if( !*p )                                 /* keine Werte!                */
    { if( skip_to_end_loop(inzeile, akt_ptr, FOR) > 0 )
      { logging("Error: %%for without %%/for.\n");
        return true;
      }
      return -1;
    }
    *p++ = '\0';                              /* Ende Parname                */
    while( *p == ' ' || *p == '\t' )
      p++;
    if( !*p )                                 /* keine Werte!                */
    { if( skip_to_end_loop(inzeile, akt_ptr, FOR) > 0 )
      { logging("Error: %%for without %%/for.\n");
        return true;
      }
      return -1;
    }
    for_names[anz_for_stack] = store_str(out);  /* Parname merken            */
    next_for_wert[anz_for_stack] = store_forwerte(p, *q);
    if( next_for_wert[anz_for_stack] )
    { set_parwert(out, next_for_wert[anz_for_stack]->wert, include_counter);
      next_for_wert[anz_for_stack] = next_for_wert[anz_for_stack]->next;
      anz_for_stack++;
      loop_stack[anz_loop_stack++] = FOR_LOOP;
    }
    else if( skip_to_end_loop(inzeile, akt_ptr, FOR) > 0 )
    { logging("Error: %%for without %%/for.\n");
      return true;
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_foreach(void)                                                              */
/*     do_foreach                                                                      */
/***************************************************************************************/
short do_foreach(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  char *p;

  if( anz_for_stack < FOR_STACK_SIZE && *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    for_seek[anz_for_stack] = ftell(akt_ptr);
#ifdef DEBUG
    for_linenumber[anz_for_stack] = linenumber;
#endif
    p = out;
    while( *p != ' ' && *p != '\t' && *p )    /* bis zum Ende des Parnamens  */
      p++;
    if( !*p )                                 /* kein Werte-Parname!         */
    { if( skip_to_end_loop(inzeile, akt_ptr, FOR) > 0 )
      { logging("Error: %%foreach without %%/for.\n");
        return true;
      }
      return -1;
    }
    *p++ = '\0';                              /* Ende Parname                */
    while( *p == ' ' || *p == '\t' )
      p++;
    if( !*p )                                 /* kein Werte-Parname!         */
    { if( skip_to_end_loop(inzeile, akt_ptr, FOR) > 0 )
      { logging("Error: %%foreach without %%/for.\n");
        return true;
      }
      return -1;
    }
    for_names[anz_for_stack] = store_str(out);  /* Parname merken            */
    next_for_wert[anz_for_stack] = firstwert(p, include_counter);
    if( next_for_wert[anz_for_stack]
      && next_for_wert[anz_for_stack]->wert.type != EMPTYW )
    { set_parwert(out, next_for_wert[anz_for_stack]->wert, include_counter);
      next_for_wert[anz_for_stack] = next_for_wert[anz_for_stack]->next;
      anz_for_stack++;
      loop_stack[anz_loop_stack++] = FOR_LOOP;
    }
    else if( skip_to_end_loop(inzeile, akt_ptr, FOR) > 0 )
    { logging("Error: %%foreach without %%/for.\n");
      return true;
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_end_for(void)                                                              */
/*     do_end_for                                                                      */
/***************************************************************************************/
short do_end_for(void)
{ if( anz_for_stack )
  { if( next_for_wert[anz_for_stack-1] )
    { set_parwert(for_names[anz_for_stack-1], next_for_wert[anz_for_stack-1]->wert,
                  include_counter);
      next_for_wert[anz_for_stack-1] = next_for_wert[anz_for_stack-1]->next;
      fseek(akt_ptr, for_seek[anz_for_stack-1], SEEK_SET);
#ifdef DEBUG
      linenumber = for_linenumber[anz_for_stack-1];
#endif
    }
    else
    { anz_for_stack--;
      anz_loop_stack--;
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_while(void)                                                                */
/*     do_while                                                                        */
/***************************************************************************************/
short do_while(void)
{ if( anz_while_stack < WHILE_STACK_SIZE && *inz )
  { if( test(inz) )
    {
#ifdef DEBUG
      while_linenumber[anz_while_stack] = linenumber;
#endif
      while_seek[anz_while_stack++] = last_line_seek;
      loop_stack[anz_loop_stack++] = WHILE_LOOP;
    }
    else if( skip_to_end_loop(inzeile, akt_ptr, WHILE) > 0 )
    { logging("Error: %%while without %%/while.\n");
      return true;
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_end_while(void)                                                            */
/*     do_end_while                                                                    */
/***************************************************************************************/
short do_end_while(void)
{ LOG(9, "u2w_put, end_while, while_seek[%d]: %lx.\n",
      anz_while_stack-1, while_seek[anz_while_stack-1]);
  if( anz_while_stack )
  { fseek(akt_ptr, while_seek[--anz_while_stack], SEEK_SET);
#ifdef DEBUG
    linenumber = while_linenumber[anz_while_stack];
#endif
    anz_loop_stack--;
  }
  return -1;
}


/***************************************************************************************/
/* short do_break(void)                                                                */
/*     do_break                                                                        */
/***************************************************************************************/
short do_break(void)
{ strcpyn_l(zeile, &inz, MAX_ZEILENLAENGE);
  if( is_elem(LOOP_STRINGS, zeile) )
  { int i;
    if( is_command(zeile, FOR) )
      i = FOR_LOOP;
    else if( is_command(zeile, WHILE) )
      i = WHILE_LOOP;
    else
      i = 0;
    while( anz_loop_stack > 0 && loop_stack[anz_loop_stack-1] != i )
    { switch( loop_stack[--anz_loop_stack] )
      { case FOR_LOOP:   anz_for_stack--;
                         break;
        case WHILE_LOOP: anz_while_stack--;
                         break;
      }
    }
    if( anz_loop_stack > 0 )
    { switch( loop_stack[--anz_loop_stack] )
      { case FOR_LOOP:   anz_for_stack--;
                         break;
        case WHILE_LOOP: anz_while_stack--;
                         break;
      }
    }
    if( skip_to_end_loop(inzeile, akt_ptr, zeile) > 0 )
    { if( *zeile )
        logging("Error: %%break %s without loop.\n", zeile);
      else
        logging("Error: %%break without loop.\n", zeile);
      return true;
    }
  }
  else
  { if( anz_loop_stack > 0 )
    { switch( loop_stack[--anz_loop_stack] )
      { case FOR_LOOP:   anz_for_stack--;
                         break;
        case WHILE_LOOP: anz_while_stack--;
                         break;
      }
    }
    if( skip_to_end_loop(inzeile, akt_ptr, NULL) > 0 )
    { logging("Error: %%break without loop.\n");
      return true;
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_switch(void)                                                               */
/*     do_switch                                                                       */
/***************************************************************************************/
short do_switch(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  skip_to_case(inzeile, out, akt_ptr);
  return -1;
}


/***************************************************************************************/
/* short do_case_default(void)                                                         */
/*     do_default                                                                      */
/***************************************************************************************/
short do_case_default(void)
{ skip_to(inzeile, ATTRIB_STR END_SWITCH, akt_ptr);
  return -1;
}


/***************************************************************************************/
/* short do_if(void)                                                                   */
/*     do_if                                                                           */
/***************************************************************************************/
short do_if(void)
{ if( !test(inz) )
    skip_to_fi_else(inzeile, akt_ptr);
  return -1;
}


/***************************************************************************************/
/* short do_else_elseif(void)                                                          */
/*     do_else_elseif                                                                  */
/***************************************************************************************/
short do_else_elseif(void)
{ skip_to_fi(inzeile, akt_ptr);
  return -1;
}


/***************************************************************************************/
/* short do_skip_empty_flag(void)                                                      */
/*     do_skip_empty_flag                                                              */
/***************************************************************************************/
short do_skip_empty_flag(void)
{ skip_empty_flag = true;
  return -1;
}


/***************************************************************************************/
/* short do_not_skip_empty_flag(void)                                                  */
/*     do_not_skip_empty_flag                                                          */
/***************************************************************************************/
short do_not_skip_empty_flag(void)
{ skip_empty_flag = false;
  return -1;
}


/***************************************************************************************/
/* short do_exit(void)                                                                 */
/*     do_exit                                                                         */
/***************************************************************************************/
short do_exit(void)
{
#ifdef INTERPRETER
  char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  char *o;

  scan_to_teil(out, inz, MAX_ZEILENLAENGE);
  o = out;
  return get_int_z(&o);
#else
  while( include_counter )
  {
#ifdef MAYDBCLIENT
    mysql_free_one_res(include_counter);
#endif
    fclose(akt_ptr);
    akt_ptr = ptr_stack[--include_counter];
    file_flag = file_flag_stack[include_counter];
  }
  return -3;
#endif
}


/***************************************************************************************/
/* short do_return(void)                                                               */
/*     do_return                                                                       */
/***************************************************************************************/
short do_return(void)
{ fseek(akt_ptr, 0, SEEK_END);
  return -1;
}


/***************************************************************************************/
/* short do_title(void)                                                                */
/*     do_title                                                                        */
/***************************************************************************************/
short do_title(void)
{ scan_to(title, &inz, MAX_NAME_LEN, 0, "", '\0');
  return -1;
}


/***************************************************************************************/
/* short do_background(void)                                                           */
/*     do_background                                                                   */
/***************************************************************************************/
short do_background(void)
{ strcpyn(background, inz, MAX_NAME_LEN);     /* Hintergrundbild             */
  return -1;
}


/***************************************************************************************/
/* short do_bodypars(void)                                                             */
/*     do_background                                                                   */
/***************************************************************************************/
short do_bodypars(void)
{ strcpyn(bodypars, inz, MAX_NAME_LEN);                 /* Body-Parameter              */
  return -1;
}


/***************************************************************************************/
/* short do_endblock(void)                                                             */
/*     do_endblock                                                                     */
/***************************************************************************************/
short do_endblock(void)
{ LOG(3, "do_endblock, parflag: %d, tablelevel: %d, tablecols: %d, akt_tablecols: %d.\n",
      parflag, tablelevel, tablecols, akt_tablecols);
  if( parflag > 0 )                           /* %end nach %parameter?       */
  { if( tablelevel > 1 )
    { tablecols = tablecolsstack[0];
      akt_tablecols = akt_tablecolsstack[0];
    }
    if( tablelevel )
    { while( --tablelevel )
      { akt_tablecols = akt_tablecolsstack[tablelevel];
        if( akt_tablecols )
        { if( dosend(TABLE_ROW_END) )
            return true;
        }
        if( dosend("</table><br>\n") )
          return true;
      }
    }
    if( akt_tablecols )
    { if( dosend(TABLE_ROW_END) )
        return true;
    }
    if( akt_tablecols > tablecols )
      tablecols = akt_tablecols;
    if( end_formular(inz, tablecols) )  /* Formularende          */
      return 0;
    tablecols = akt_tablecols = 0;
    while( include_counter )
    {
#ifdef MAYDBCLIENT
      mysql_free_one_res(include_counter);
#endif
      fclose(akt_ptr);
      akt_ptr = ptr_stack[--include_counter];
      file_flag = file_flag_stack[include_counter];
    }
    return -3;                              /* aufbauen und Ende             */
  }
  if( parflag < 0 || menu != NOMENU )       /* %end nach %endpar oder %menu? */
  {
    while( include_counter )
    {
#ifdef MAYDBCLIENT
      mysql_free_one_res(include_counter);
#endif
      fclose(akt_ptr);
      akt_ptr = ptr_stack[--include_counter];
      file_flag = file_flag_stack[include_counter];
    }
    return -3;
  }
  tablecols = akt_tablecols = 0;
  return -1;
}


/***************************************************************************************/
/* short do_endpar(void)                                                               */
/*     do_endpar                                                                       */
/***************************************************************************************/
short do_endpar(void)
{ LOG(3, "do_endpar, parflag: %d, tablelevel: %d, tablecols: %d, akt_tablecols: %d.\n",
      parflag, tablelevel, tablecols, akt_tablecols);
  if( parflag > 0 )                           /* %end nach %parameter?       */
  { if( tablelevel > 1 )
    { tablecols = tablecolsstack[0];
      akt_tablecols = akt_tablecolsstack[0];
    }
    if( tablelevel )
    { while( --tablelevel )
      { akt_tablecols = akt_tablecolsstack[tablelevel];
        if( akt_tablecols )
        { if( dosend(TABLE_ROW_END) )
            return true;
        }
        if( dosend("</table><br>\n") )
          return true;
      }
    }
    if( akt_tablecols )
    { if( dosend(TABLE_ROW_END) )
        return true;
    }
    if( akt_tablecols > tablecols )
      tablecols = akt_tablecols;
    if( end_formular(inz, tablecols) )  /* Formularende          */
      return 0;
    tablecols = akt_tablecols = 0;
  }
  parflag = -1;
  tablecols = akt_tablecols = 0;
  return -1;
}


/***************************************************************************************/
/* short do_resultpage(void)                                                           */
/*     do_resultpage                                                                   */
/***************************************************************************************/
short do_resultpage(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    resultpage = store_str(out);
  }
  return -1;
}


/***************************************************************************************/
/* short do_parameter_ext(void)                                                        */
/*     do_parameter_ext                                                                */
/***************************************************************************************/
short do_parameter_ext(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    parameter_ext = store_str(out);
  }
  return -1;
}


/***************************************************************************************/
/* short do_no_newline(void)                                                           */
/*     do_no_newline                                                                   */
/***************************************************************************************/
short do_no_newline(void)
{ newlineflag = false;
  return -1;
}


/***************************************************************************************/
/* short do_not_no_newline(void)                                                       */
/*     do_not_no_newline                                                               */
/***************************************************************************************/
short do_not_no_newline(void)
{ char *p;

  if( akt_tablecols )
  { akt_tablecols = 0;
    if( tablecols < 0 )
      tablecols = -tablecols;
    if( (p=parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
    { if( dosend(p) )
        return true;
    }
    else if( u2w_mode == U2W_MODE && dosend(TABLE_ROW_END) )
      return true;
  }
  newlineflag = true;
  return -1;
}


/***************************************************************************************/
/* short do_table_autocol_on(void)                                                     */
/*     Tabllen mit leerspalten auffüllen ein                                           */
/***************************************************************************************/
short do_table_autocol_on(void)
{ table_autofill_cols = true;
  return -1;
}


/***************************************************************************************/
/* short do_table_autocol_off(void)                                                    */
/*     Tabllen mit leerspalten auffüllen aus                                           */
/***************************************************************************************/
short do_table_autocol_off(void)
{ table_autofill_cols = false;
  return -1;
}
