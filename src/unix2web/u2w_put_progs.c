/*******************************************/
/* File: u2w_put_progs.c                   */
/* Funktionen aus Arrays u2w_put_progs     */
/* Funktionen nach Senden des HTML-Headers */
/* timestamp: 2017-02-25 20:00:36          */
/*******************************************/


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
/* short do_shellmenu(void)                                                            */
/*     do_shellmenu                                                                    */
/***************************************************************************************/
short do_shellmenu(void)
{ if( menu == SUBSHELL )               /* Subshell suchen?                  */
  { return do_subshell(inz);           /*  1: Fehler, ABBRUCH               */
                                       /* -1: falsche Zeile: weiter         */
                                       /*  0: gefunden und ausgeführt       */
  }
  else
  { if( send_subshell(inz) )
      return true;
  }
  return -1;
}


/***************************************************************************************/
/* short do_subfile(void)                                                              */
/*     do_subfile                                                                      */
/***************************************************************************************/
short do_subfile(void)
{ if( send_submenufile(inz) )
    return true;
  return -1;
}


/***************************************************************************************/
/* short do_exitmenu(void)                                                             */
/*     do_exitmenu                                                                     */
/***************************************************************************************/
short do_exitmenu(void)
{ if( send_subfile(inz, EXITTARGET) )
    return true;
  return -1;
}


/***************************************************************************************/
/* short do_menulink(void)                                                             */
/*     do_menulink                                                                     */
/***************************************************************************************/
short do_menulink(void)
{ if( send_subfile(inz, menu != NOMENU ? MENUFRAMERIGHTNAME : "") )
    return true;
  return -1;
}


/***************************************************************************************/
/* short do_newwind(void)                                                              */
/*     do_newwind                                                                      */
/***************************************************************************************/
short do_newwind(void)
{ if( send_subfile(inz, NEWTARGET) )
    return true;
  return -1;
}


/***************************************************************************************/
/* short do_newmenu(void)                                                              */
/*     do_newmenu                                                                      */
/***************************************************************************************/
short do_newmenu(void)
{ if( send_subfile(inz, TOPTARGET) )
    return true;
  return -1;
}


/***************************************************************************************/
/* short do_submenu(void)                                                              */
/*     do_submenu                                                                      */
/***************************************************************************************/
short do_submenu(void)
{ if( menu_submenu(inz, &headlineflag, background) )    /* Untermenüzeile    */
    return true;                                        /* auswerten         */
  return -1;
}


/***************************************************************************************/
/* short do_table_start(void)                                                          */
/*     do_table_start                                                                  */
/***************************************************************************************/
short do_table_start(void)
{ char out[MAX_ZEILENLAENGE];                        /* Aufbau des Ergebnisses         */
  char *p;

  LOG(1, "do_table_start, inz: %s.\n", inz);
  if( tablelevel < TABLEROWSSTACKSIZE )
  { if( tablelevel )
    { tablecolsstack[tablelevel-1] = tablecols;
      if( akt_tablecols++ )
      { if( (p=parwert(S2W_TABLE_CELL_START, HP_SYSTEM_LEVEL)) )
        { if( dosend(p) )
            return true;
        }
        else if( u2w_mode == U2W_MODE )
        { if( css_defs[CSS_TD] )
          { if( dosendf(TABLE_COL_CLASS_START, css_defs[CSS_TD]) )
              return true;
          }
          else if( dosend(TABLE_COL_START) )
            return true;
        }
      }
      else
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
        if( (p=parwert(S2W_TABLE_CELL_START, HP_SYSTEM_LEVEL)) )
        { if( dosend(p) )
            return true;
        }
        else if( u2w_mode == U2W_MODE )
        { if( css_defs[CSS_TD] )
          { if( dosendf(TABLE_COL_CLASS_START, css_defs[CSS_TD]) )
              return true;
          }
          else if( dosend(TABLE_COL_START) )
            return true;
        }
      }
      akt_tablecolsstack[tablelevel-1] = akt_tablecols;
    }
    tablelevel++;
    tablecols = 0;

    if( *inz == PAR_PAR_CHAR )
    { inz++;
      p = inz;
      skip_next_blanks(inz);
      if( *(inz-1) == ' ' )
        *(inz-1) = '\0';
    }
    else
      p = NULL;

    if( u2w_mode == U2W_MODE && css_defs[CSS_TABLE] )
    { if( dosendf("<table class=\"%s\">\n", css_defs[CSS_TABLE]) )
        return true;
    }
    else if( u2w_mode == U2W_MODE && is_command_z(&inz, TABLE_BORDER) )
    { if( dosend("<table class=\"u2wborder\">\n") )
        return true;
    }
    else
    { char *q;
      if( (q=parwert(S2W_TABLE_START, HP_SYSTEM_LEVEL)) )
      { if( dosend(q) )
          return true;
      }
      else if( u2w_mode == U2W_MODE && dosend("<table class=\"u2wnoborder\">\n") )
        return true;
    }
    if( *inz == BESCHREIBUNG_CHAR )
    { inz++;
      skip_blanks(inz);
      if( dosend("<caption><h2>") )
        return true;
      do_scan(inz, NOMENU);
      if( dosend("</h2></caption>\n") )
        return true;
    }
    if( p )
    { scan_to_teil(out, p, MAX_ZEILENLAENGE);
      LOG(7, "do_table_start, table_aligns[%d]: %s.\n", tablelevel-1, out);
      table_aligns[tablelevel-1] = store_str(out);
      aligntablecols[tablelevel-1] = tablecols = strlen(out);
    }
    else
    { aligntablecols[tablelevel-1] = 0;
      table_aligns[tablelevel-1] = NULL;
      tablecols = 2;
    }
    akt_tablecols = 0;
  }
  return -1;
}


/***************************************************************************************/
/* short do_table_head(void)                                                           */
/*     do_table_head                                                                   */
/***************************************************************************************/
short do_table_head(void)
{ char *p;

  LOG(3, "do_table_head, tablecols: %d.\n", tablecols);
  if( (tablecols = -tablecols) )
  { skip_blanks(inz);
    if( *inz )
    { if( !akt_tablecols++ )
      { if( (p=parwert(S2W_TABLE_ROW_START, HP_SYSTEM_LEVEL)) )
        { if( dosend(p) )
            return true;
        }
        else if( u2w_mode == U2W_MODE && dosend(TABLE_ROW_START) )
          return true;
      }
      do_scan(inz, menu);                                     /* Zeile auswerten       */
      if( newlineflag && akt_tablecols )
      { if( tablecols < 0 )
          tablecols = -tablecols;
        if( tablecols < akt_tablecols )
          tablecols = akt_tablecols;
        akt_tablecols = 0;
        if( (p=parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
        { if( dosend(p) )
            return true;
        }
        else if( u2w_mode == U2W_MODE && dosend(TABLE_ROW_END) )
          return true;
      }
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_table_end(void)                                                            */
/*     do_table_end                                                                    */
/***************************************************************************************/
short do_table_end(void)
{ char *p;

  if( tablelevel )
  { if( akt_tablecols )
    { if( (p=parwert(S2W_TABLE_ROW_END, HP_SYSTEM_LEVEL)) )
      { if( dosend(p) )
          return true;
      }
      else if( u2w_mode == U2W_MODE && dosend(TABLE_ROW_END) )
        return true;
    }
    if( tablelevel > 1 )
    { if( (p=parwert(S2W_TABLE_END, HP_SYSTEM_LEVEL)) )
      { if( dosend(p) )
          return true;
      }
      else if( u2w_mode == U2W_MODE && dosend("</table>\n") )
        return true;
      if( (p=parwert(S2W_TABLE_CELL_END, HP_SYSTEM_LEVEL)) )
      { if( dosend(p) )
          return true;
      }
      else if( u2w_mode == U2W_MODE && dosend("</td>\n") )
        return true;
    }
    else
    { if( (p=parwert(S2W_TABLE_END, HP_SYSTEM_LEVEL)) )
      { if( dosend(p) )
           return true;
      }
      else if( u2w_mode == U2W_MODE && dosend("</table><br>\n") )
        return true;
    }
    if( --tablelevel )
    { tablecols = tablecolsstack[tablelevel-1];
      akt_tablecols = akt_tablecolsstack[tablelevel-1];
    }
    else
    { tablecols = akt_tablecols = 0;
      LOG(22, "do_table_end - sendhiddenvars, tablelevel: %d.\n", tablelevel);
      if( sendhiddenvars() )
        return true;
    }
  }
  return -1;
}


/***************************************************************************************/
/* short do_center(void)                                                               */
/*     do_center                                                                       */
/***************************************************************************************/
short do_center(void)
{  if( !centerflag )
  { if( dosend("<center>") )
      return true;
    centerflag = 1;                           /* Zentrieren nicht im Formular        */
  }
  return -1;
}


/***************************************************************************************/
/* short do_left(void)                                                                 */
/*     do_left                                                                         */
/***************************************************************************************/
short do_left(void)
{ if( centerflag )
  { if( dosend("</center>") )
      return true;
    centerflag = false;                               /* Linksbuendig                */
  }
  return -1;
}


/***************************************************************************************/
/* short do_pre_on(void)                                                               */
/*             char **inz: nächste Leseposition                                        */
/*     do_pre_on                                                                       */
/***************************************************************************************/
short do_pre_on(void)
{ if( dosend(START_FIXED_FONT) )
    return true;
  u2w_mode = S2W_MODE;
  return -1;
}


/***************************************************************************************/
/* short do_pre_off(void)                                                              */
/*             char **inz: nächste Leseposition                                        */
/*     do_pre_off                                                                      */
/***************************************************************************************/
short do_pre_off(void)
{ if( dosend(END_FIXED_FONT) )
    return true;
  u2w_mode = U2W_MODE;
  return -1;
}
