/*****************************************/
/* File: u2w_put_header_progs.c          */
/* Funktionen aus Arrays u2w_put_header  */
/* Funktionen nach HTTP, vor HTML Header */
/* timestamp: 2016-03-28 15:56:14        */
/*****************************************/


#include "u2w.h"

/***************************************************************************************/
/* Returnwerte der do_* Kommandos:                                                     */
/* -3: exit wurde aufgerufen - Seite beenden                                           */
/* -2: Zeile in inz erneut auswerten                                                   */
/* -1: contiue                                                                         */
/*  0: Vrelassen ohne Fehler                                                           */
/* >0: Fehler                                                                          */
/***************************************************************************************/



/***************************************************************************************/
/* int test_argument(char *s)                                                          */
/*                   char *s: ab hier testen                                           */
/*                   return : true, wenn in s nicht nur whitespaces stehen             */
/***************************************************************************************/
int test_argument(char *s)
{ skip_blanks(s);
  return *s;
} 


/***************************************************************************************/
/* int do_parameter_get(void)                                                          */
/*             char **inz: nächste Leseposition                                        */
/*     do_parameter_get                                                                */
/***************************************************************************************/
int do_parameter_get(void)
{ if( test_argument(inz) ? !test(inz) : NULL != parwert(FORMMAGIC, HP_BROWSER_LEVEL) )
               /* Bedingung angegeben? dann testen : sonst OK gedrückt?                */
  { skip_to(inzeile, ATTRIB_STR ENDBLOCK, akt_ptr);            /* ja, dann weiter      */
  }
  else
  { getflag = true;
    multipart_flag = false;
    if( html_head_flag > 1 )                             /* start_parabfrage           */
    { if( start_formular(getflag, multipart_flag,
          *resultpage ? resultpage : clientgetfile) )
        return 0;
      parflag = 2;
    }
    else
      parflag = 1;                                      /* Parameterblock beginnt      */
  }
  return -1;
}


/***************************************************************************************/
/* int do_parameter_multipart(void)                                                    */
/*             char **inz: nächste Leseposition                                        */
/*     do_parameter_multipart                                                          */
/***************************************************************************************/
int do_parameter_multipart(void)
{ if( test_argument(inz) ? !test(inz) : NULL != parwert(FORMMAGIC, HP_BROWSER_LEVEL) )
               /* Bedingung angegeben? dann testen : sonst OK gedrückt?                */
    skip_to(inzeile, ATTRIB_STR ENDBLOCK, akt_ptr);            /* ja, dann weiter      */
  else
  { getflag = false;
    multipart_flag = true;
    if( html_head_flag > 1 )                             /* start_parabfrage           */
    { if( start_formular(getflag, multipart_flag,
          *resultpage ? resultpage : clientgetfile) )
        return false;
      parflag = 2;
    }
    else
      parflag = 1;                                      /* Parameterblock beginnt      */
  }
  return -1;
}


/***************************************************************************************/
/* int do_parameter(void)                                                              */
/*             char **inz: nächste Leseposition                                        */
/*     do_parameter                                                                    */
/***************************************************************************************/
int do_parameter(void)
{ LOG(4, "u2w_put, parameter, inz: %s.\n", inz);
  if( test_argument(inz) ? !test(inz) : NULL != parwert(FORMMAGIC, HP_BROWSER_LEVEL) )
               /* Bedingung angegeben? dann testen : sonst OK gedrückt?                */
  { skip_to(inzeile, ATTRIB_STR ENDBLOCK, akt_ptr); /* ja, dann weiter       */
  }
  else
  { getflag = false;
    multipart_flag = false;
    if( html_head_flag > 1 )                               /* start_parabfrage         */
    { if( start_formular(getflag, multipart_flag,
          *resultpage ? resultpage : clientgetfile) )
        return false;
      parflag = 2;
    }
    else
      parflag = 1;                            /* Parameterblock beginnt      */
  }
  LOG(6, "u2w_put, parameter, parflag: %d.\n", parflag);
  return -1;
}


/***************************************************************************************/
/* int do_html_head_on(void)                                                           */
/*             char **inz: nächste Leseposition                                        */
/*     do_html_head_on                                                                 */
/***************************************************************************************/
int do_html_head_on(void)
{ LOG(21, "do_html_head_on - u2w_mode: %d\n", u2w_mode);
  if( u2w_mode >= S2W_MODE )
    skip_to(inzeile, ATTRIB_STR HTML_HEAD_OFF, akt_ptr);
  else
  { html_head_u2w_mode = u2w_mode;
    u2w_mode = RAW_MODE;
  }
  return -1;
}


/***************************************************************************************/
/* int do_html_heads_on(void)                                                          */
/*             char **inz: nächste Leseposition                                        */
/*     do_html_heads_on                                                                */
/***************************************************************************************/
int do_html_heads_on(void)
{ if( u2w_mode >= S2W_MODE )
    skip_to(inzeile, ATTRIB_STR HTML_HEADS_OFF, akt_ptr);
  else
  { html_head_u2w_mode = u2w_mode;
    u2w_mode = S2W_MODE;
  }
  return -1;
}


/***************************************************************************************/
/* int do_html_head_off(void)                                                          */
/*             char **inz: nächste Leseposition                                        */
/*     do_html_head_off                                                                */
/***************************************************************************************/
int do_html_head_off(void)
{ u2w_mode = html_head_u2w_mode;
  html_head_u2w_mode = NO_MODE;
  return -1;
}


/***************************************************************************************/
/* int do_http_head_on(void)                                                           */
/*             char **inz: nächste Leseposition                                        */
/*     do_http_head_on                                                                 */
/***************************************************************************************/
int do_http_head_on(void)
{ http_head_u2w_mode = u2w_mode;
  u2w_mode = RAW_MODE|RAW_HTTPHEAD_MODE;
  return -1;
}


/***************************************************************************************/
/* int do_http_heads_on(void)                                                          */
/*             char **inz: nächste Leseposition                                        */
/*     do_http_heads_on                                                                */
/***************************************************************************************/
int do_http_heads_on(void)
{ http_head_u2w_mode = u2w_mode;
  u2w_mode = S2W_MODE|S2W_HTTPHEAD_MODE;
  return -1;
}


/***************************************************************************************/
/* int do_http_head_off(void)                                                          */
/*             char **inz: nächste Leseposition                                        */
/*     do_http_head_off                                                                */
/***************************************************************************************/
int do_http_head_off(void)
{ u2w_mode = http_head_u2w_mode;
  http_head_u2w_mode = NO_MODE;
  return -1;
}


/***************************************************************************************/
/* int do_frame(void)                                                                  */
/*             char **inz: nächste Leseposition                                        */
/*     do_frame                                                                        */
/***************************************************************************************/
int do_frame(void)
{ char *p, *q, *r;

  if( eqbpar(FRAMENAME, FRAMETOPNAME) || eqbpar(FRAMENAME, FRAMELEFTNAME) )
  { return -1;
  }
  if( eqbpar(FRAMENAME, FRAMEBOTTOMNAME) || eqbpar(FRAMENAME, FRAMERIGHTNAME) )
  { skip_to(inzeile, ATTRIB_STR FRAME_END, akt_ptr);
    return -1;
  }
  p = q = r = "";
  if( *inz == PAR_PAR_CHAR )
  { if( *(++inz) != PAR_PAR_CHAR )
    { p = inz;
      while( *inz && *inz != PAR_PAR_CHAR )
        inz++;
    }
  }
  if( *inz == PAR_PAR_CHAR )
  { *inz++ = '\0';
    if( *inz != PAR_PAR_CHAR )
    { q = inz;
      while( *inz && *inz != PAR_PAR_CHAR )
        inz++;
    }
  }
  if( *inz == PAR_PAR_CHAR )
  { *inz++ = '\0';
    if( *inz != PAR_PAR_CHAR )
    { r = inz;
      while( *inz && *inz != PAR_PAR_CHAR )
        inz++;
    }
  }
  if( *inz == PAR_PAR_CHAR )
    *inz++ = '\0';
  if( dosend(PAGE_HEADER_ENDE) )
    return true;
  return send_frame(p, q, r, inz);
}


/***************************************************************************************/
/* int do_frame_end(void)                                                              */
/*             char **inz: nächste Leseposition                                        */
/*     do_frame_end                                                                    */
/***************************************************************************************/
int do_frame_end(void)
{ while( include_counter )
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


/***************************************************************************************/
/* int do_menuhead(void)                                                               */
/*             char **inz: nächste Leseposition                                        */
/*     do_menuhead                                                                     */
/***************************************************************************************/
int do_menuhead(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  LOG(11, "do_menuhead\n");

  if( menu == MENURIGHT )
    skip_to(inzeile, ATTRIB_STR ENDBLOCK, akt_ptr);
  else if( menu != INMENUHEAD )
  { if( menu == NOMENU )
    { if( *inz == PAR_PAR_CHAR )
      { inz++;
        if( *inz != PAR_PAR_CHAR )
          strcpyn_d(headsize, &inz, MAX_SIZELENGTH);
        else
          headsize[0] = '\0';
        if( *inz == PAR_PAR_CHAR )
        { inz++;
          if( *inz != ' ' && *inz != '0' )
            noframeborders |= 1;
        }
      }
      else
        headsize[0] = '\0';
      skip_next_blanks(inz);
      LOG(4, "u2w_put - MENUHEAD, inz: %s.\n", inz); 
      if( *inz )
      { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
        headframe = store_str(out);
      }
      else
        headframe = NULL;
    }
    inzeile[0] = '\0';                         /* MENUHEAD ueberspringen     */
    skip_to(inzeile, ATTRIB_STR MENU, akt_ptr);
    inz = inzeile;
    skip_blanks(inz);
    skip_attrib_char(inz);
    return -2;                                           /* nack skip_to KEIN continue!*/
  }
  return -1;
}


/***************************************************************************************/
/* int do_menu(void)                                                                   */
/*             char **inz: nächste Leseposition                                        */
/*     do_menu                                                                         */
/***************************************************************************************/
int do_menu(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  char leftframe[MAX_ZEILENLAENGE];

  LOG(11, "do_menu, menu: %d, inz: %s.\n", menu, inz);
  if( menu == INMENUHEAD )
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
  else if( menu != INMENU && menu != SUBITEM )
  { char leftsize[MAX_SIZELENGTH];

    if( menu == NOMENU )
    { if( *inz == PAR_PAR_CHAR )
      { inz++;
        if( *inz != PAR_PAR_CHAR )
          strcpyn_d(leftsize, &inz, MAX_SIZELENGTH);
        else
          leftsize[0] = '\0';
        if( *inz == PAR_PAR_CHAR )
        { inz++;
          if( *inz != ' ' && *inz != '0' )
            noframeborders |= 2;
        }
      }
      else
        leftsize[0] = '\0';
      skip_next_blanks(inz);
      LOG(19, "do_menu - NOMENU, inz: %s.\n", inz); 
      if( *inz )
        scan_to_teil(leftframe, inz, MAX_ZEILENLAENGE);
      else
        leftframe[0] = '\0';
      if( parwert(RECHTSMENU, HP_BROWSER_LEVEL) )
        return send_menu(parwert(RECHTSMENU, HP_BROWSER_LEVEL), leftframe,
                         headsize, leftsize, noframeborders);
    }
    else
      leftsize[0] = leftframe[0] = '\0';
    skip_to(inzeile, ATTRIB_STR ENDBLOCK, akt_ptr);          /* auf %end Positionieren */
    inz = inzeile;
    skip_blanks(inz);
    skip_attrib_char(inz);
    if( is_command_z(&inz, ENDBLOCK) )                       /* Auf Parameter nach %end*/
    { scan_to_teil(out, inz, MAX_ZEILENLAENGE);              /* als def. Seite         */
      return send_menu(out, leftframe, headsize, leftsize, noframeborders);
    }
    else
      return send_menu("", leftframe, headsize, leftsize, noframeborders);
  }
  return -1;
}


/***************************************************************************************/
/* int do_settop(void)                                                                 */
/*             char **inz: nächste Leseposition                                        */
/*     do_settop                                                                       */
/***************************************************************************************/
int do_settop(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, MENUFRAMETOPNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_setleft(void)                                                                */
/*             char **inz: nächste Leseposition                                        */
/*     do_setleft                                                                      */
/***************************************************************************************/
int do_setleft(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, MENUFRAMELEFTNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_setright(void)                                                               */
/*             char **inz: nächste Leseposition                                        */
/*     do_setright                                                                     */
/***************************************************************************************/
int do_setright(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, MENUFRAMERIGHTNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_fsettop(void)                                                                */
/*             char **inz: nächste Leseposition                                        */
/*     do_fsettop                                                                      */
/***************************************************************************************/
int do_fsettop(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, FRAMETOPNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_fsetbottom(void)                                                             */
/*             char **inz: nächste Leseposition                                        */
/*     do_fsetbottom                                                                   */
/***************************************************************************************/
int do_fsetbottom(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, FRAMEBOTTOMNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_fsetleft(void)                                                               */
/*             char **inz: nächste Leseposition                                        */
/*     do_fsetleft                                                                     */
/***************************************************************************************/
int do_fsetleft(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, FRAMELEFTNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_fsetright(void)                                                              */
/*             char **inz: nächste Leseposition                                        */
/*     do_fsetright                                                                    */
/***************************************************************************************/
int do_fsetright(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(SETFRAMESCRIPT, FRAMERIGHTNAME, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_setframe(void)                                                               */
/*             char **inz: nächste Leseposition                                        */
/*     do_setframe                                                                     */
/***************************************************************************************/
int do_setframe(void)
{ char *p;
  char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  p = inz;
  skip_to_blank(inz);
  if( *inz )
  { *inz++ = '\0';
    skip_blanks(inz);
    if( *inz )
    { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
      code_link(zeile, out, MAX_ZEILENLAENGE, false);
      dosendf(SETFRAMESCRIPT, p, zeile);
    }
  }
  return -1;
}


/***************************************************************************************/
/* int do_csslink(void)                                                                */
/*             char **inz: nächste Leseposition                                        */
/*     do_csslink                                                                      */
/***************************************************************************************/
int do_csslink(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */

  if( *inz && (u2w_mode & (U3W_MODE|U2W_HTML_MODE|U2W_MODE|U4W_MODE|U5W_MODE)) )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    code_link(zeile, out, MAX_ZEILENLAENGE, false);
    dosendf(CSSLINKHEADER, zeile);
  }
  return -1;
}


/***************************************************************************************/
/* int do_refresh(void)                                                                */
/*             char **inz: nächste Leseposition                                        */
/*     do_refresh                                                                      */
/***************************************************************************************/
int do_refresh(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  int seks;

  LOG(3, "do_refresh, inz: %s\n", inz);

  seks = get_int_z(&inz);
  if( seks > 0 )
  { scan_to_teil(out, inz, MAX_ZEILENLAENGE);
    dosendf(META_REFRESH, seks, out);
  }
  return -1;
}
