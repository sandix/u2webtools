/***********************************************/
/* File: u2w_form.c                            */
/* Funktionen zur Erzeugung von Eingabefeldern */
/* timestamp: 2015-12-19 11:33:53              */
/***********************************************/


#include "u2w.h"


/***************************************************************************************/
/* void getopt_value(char *opt, char *text, char *title, char **in)                    */
/*                   char *opt  : Name des Optionsfeldes                               */
/*                   char *text: Text, wenn angegeben                                  */
/*                   char *title: Titel, wenn angegeben                                */
/*                   char **in  : String mit den Namen                                 */
/*`     getopt_value extrahiert ein Optionsfeldeintrag ggf. mit Value                  */
/***************************************************************************************/
void getopt_value(char *opt, char *text, char *title, char **in)
{
  LOG(3, "getopt_value, in: %s.\n", *in);

  skip_blanks(*in);
  if( **in == '"' )
    strcpyn_qs(opt, in, PARLEN);
  else
    strcpyn_str(opt, in, " :\t", PARLEN);
  if( **in == ':' )
  { (*in)++;
    if( **in == '"' )
      strcpyn_qs(text, in, PARLEN);
    else
      strcpyn_str(text, in, " :\t", PARLEN);
    if( **in == ':' )
    { (*in)++;
      strcpyn_qs(title, in, PARLEN);
    }
    else
      title[0] = '\0';
  }
  else
  { text[0] = '\0';
    title[0] = '\0';
  }

  LOG(5, "/getopt_value, opt: %s, text:%s, title: %s.\n", opt, text, title);
}


/***************************************************************************************/
/* int send_radiobuttons(char *name, char *radios, char *def, int b, char *ro,         */
/*                       char *class)                                                  */
/*              char *name   : Name der Variablen                                      */
/*              char *radioss: Namen der Radiobuttons                                  */
/*              char *def    : Name des markierten Buttons                             */
/*              int b        : Anzahl der Spalten                                      */
/*              char *ro     : ggf: "readonly"                                         */
/*              char *class  : ggf. CSS-Class                                          */
/*              return       : true im Fehlerfall                                      */
/***************************************************************************************/
int send_radiobuttons(char *name, char *radios, char *def, int b, char *ro, char *class)
{ char rad[PARLEN];                                         /* Name des Buttons        */
  char *r;                                                  /* zeigt auf die Buttons   */
  char extend[PARLEN+20], text[PARLEN], title[PARLEN];
  int i;

  if( b < 1 )
    b = 1;
  r = radios;
  if( css_defs[CSS_RADIOBUTTONTABLE] )
  { if( dosendf("<table class=\"%s\">", css_defs[CSS_RADIOBUTTONTABLE]) )
      return true;
  }
  else if( dosend("<table>") )
    return true;
  i = 1;
  while( *r )
  { getopt_value(rad, text, title, &r);                     /* Name des Buttons        */
    if( *rad )
    { if( i == 1 && dosend("<tr>") )
        return true;
      if( *title )
        snprintf(extend, 2*PARLEN+20, " title=\"%s\"", title);
      else
        extend[0] = '\0';
      if( dosendhf("<td><input name=\"%s\" type=\"radio\"%s value=\"%s\"%s%s%s></td><td>%H</td>",
                  name, class, rad, extend,
                  is_elem(def, rad) ? " checked=\"checked\"" : "", ro,
                  *text ? text : rad) )
        return true;
      if( i++ >= b )
      { if( dosend("</tr>\n") )
          return true;
        i = 1;
      }
      else
      { if( dosend("<td></td>\n") )
          return true;
      }
    }
  }
  return dosend("</table>\n");
}


/***************************************************************************************/
/* int send_checkboxen(char *name, char *checks, char *def, int b, char *ro,           */
/*                     char *class)                                                    */
/*                     char *name  : Name der Variablen                                */
/*                     char *checks: Namen der Checkboxen                              */
/*                     char *def   : Namen der markierten Boxen                        */
/*                     int b       : Anzahl der Spalten                                */
/*                     char *ro    : ggf. "readonly"                                   */
/*                     char *class : ggf. CSS-Class                                    */
/*                     return      : true im Fehlerfall                                */
/***************************************************************************************/
int send_checkboxen(char *name, char *checks, char *def, int b, char *ro, char *class)
{ char check[PARLEN];                                       /* Name der Checkbox       */
  char *r;                                                  /* zeigt auf die Buttons   */
  char extend[PARLEN+20], text[PARLEN], title[PARLEN];
  int i;

  if( b < 1 )
    b = 1;
  r = checks;
  if( css_defs[CSS_CHECKBOXTABLE] )
  { if( dosendf("<table class=\"%s\">", css_defs[CSS_CHECKBOXTABLE]) )
      return true;
  }
  else if( dosend("<table>") )
    return true;
  i = 1;
  while( *r )
  { getopt_value(check, text, title, &r);                   /* Name des Buttons        */
    if( *check )
    { if( i == 1 && dosend("<tr>") )
        return true;
      if( *title )
        snprintf(extend, PARLEN+20, " title=\"%s\"", title);
      else
        extend[0] = '\0';
      if( dosendhf("<td><input name=\"%s\" type=\"checkbox\"%s value=\"%s\"%s%s%s></td><td>%H</td>\n",
                  name, class, check, extend,
                  is_elem(def, check) ? " checked=\"checked\"" : "", ro,
                  *text ? text : check) )
        return true;
      if( i++ >= b )
      { if( dosend("</tr>\n") )
          return true;
        i = 1;
      }
      else
      { if( dosend("<td></td>") )
          return true;
      }
    }
  }
  return dosend("</table>");
}


/***************************************************************************************/
/* int isempty(char *s)                                                                */
/*             char *s: String zum prüfen auf Leer                                     */
/*             return: true, wenn s nur Leerzeichen enthält                            */
/***************************************************************************************/
int isempty(char *s)
{ while( *s )
  { if( *s != ' ' )
      return false;
    s++;
  }
  return true;
}


/***************************************************************************************/
/* int send_opteintraege(char *opts, char *defs)                                       */
/*              char *opts: Namen der Optionsfelder                                    */
/*              char *defs: Namen der Optionsfelder die selektiert sind                */
/*              return    : true im Fehlerfall                                         */
/***************************************************************************************/
int send_opteintraege(char *opts, char *defs)
{ char opt[PARLEN];                                         /* Optionsfeldeintrag      */
  char *o;                                                  /* zeigt auf die Eintraege */
  char extend[2*PARLEN+20], text[PARLEN], title[PARLEN];    /* Value=... String        */

  o = opts;
  while( *o )
  { getopt_value(opt, text, title, &o);                     /* Name extrahieren        */
    if( *opt )
    { if( *text && *title )
        snprintf(extend, 2*PARLEN+20, " value=\"%s\" title=\"%s\"", opt, title);
      else if( *text )
        snprintf(extend, 2*PARLEN+20, " value=\"%s\"", opt);
      else if( *title )
        snprintf(extend, 2*PARLEN+20, " title=\"%s\"", title);
      else
        extend[0] = '\0';
      if( dosendhf("<option %s%s%s>%H</option>\n",
               isempty(*text ? text : opt) ? "label=\" \" " : "",
               is_elem(defs, opt) ? " selected" : "",       /* ggf. selektieren        */
               extend,                                      /* ggf. Value/Title        */
               *text ? text : opt) )
        return true;
    }
  }
  return false;
}


/***************************************************************************************/
/* int send_datalistvalues(char *opts)                                                 */
/*              char *opts: Values                                                     */
/*              return    : true im Fehlerfall                                         */
/***************************************************************************************/
int send_datalistvalues(char *opts)
{ char opt[PARLEN];                                         /* Value                   */
  char *o;                                                  /* zeigt auf die Eintraege */
  char dummy[PARLEN];

  o = opts;
  while( *o )
  { getopt_value(opt, dummy, dummy, &o);                    /* Value extrahieren       */
    if( *opt )
    { if( dosendhf("<option value=\"%s\">\n", opt) )
        return true;
    }
  }
  return false;
}


/***************************************************************************************/
/* int send_linklisteintraege(cahr *name, char *links, int b, char *class)             */
/*                            char *name : Name, der gesetzt werden soll               */
/*                            char *links: Namen der Links                             */
/*                            int b      : Anzahl der Spalten                          */
/*                            char *class : ggf. CSS-Class                             */
/*                            return     : true im Fehlerfall                          */
/***************************************************************************************/
int send_linklisteintraege(char *links, char *name, int b, char *class)
{ char link[PARLEN];                                        /* Linkeintrag             */
  char *l;                                                  /* zeigt auf die Eintraege */
  char extend[PARLEN+20], text[PARLEN], title[PARLEN];
  int i;

  l = links;

  if( b > 1 )
  { if( css_defs[CSS_LINKLISTTABLE] )
    { if( dosendf("<table class=\"%s\">", css_defs[CSS_LINKLISTTABLE]) )
        return true;
    }
    else if( dosend("<table>\n") )
      return true;
  }
  i = 1;
  while( *l )
  { getopt_value(link, text, title, &l);                    /* Name extrahieren        */
    if( *link )
    { if( b > 1 && i == 1 && dosend("<tr>") )
        return true;
      if( *title )
        snprintf(extend, 2*PARLEN+20, " title=\"%s\"", title);
      else
        extend[0] = '\0';
      if( dosendhf("%s<a href=\"%s?%s=%s&" FORMMAGIC "=1\"%s%s>%H</a>%s\n",
               b > 1 ? "<td>" : (b == 1 ? "" : " "),
               clientgetfile, name, link, extend, class,
               *text ? text : link, b > 1 ? "</td>" : (b == 1 ? "<br>" : " ")) )
        return true;
      if( b > 1 )
      { if( i++ >= b )
        { if( dosend("</tr>\n") )
            return true;
          i = 1;
        }
        else
        { if( dosend("<td></td>") )
            return true;
        }
      }
    }
  }
  if( b > 1 )
    return dosend("</table>\n");
  return false;
}


/***************************************************************************************/
/* int send_link_button(char *path, char *name, char *target, char *extra)             */
/*                      char *path  : url, die zum Button umgewandelt werden soll      */
/*                      char *name  : Name des Buttons                                 */
/*                      char *target: Kann target="links" enthalten (bei Menuesystem   */
/*                      char *extra: z. B. ' title="..."'                              */
/*     send_link_buttons erzeugt einen Button, der der url aus path entspricht         */
/***************************************************************************************/
int send_link_button(char *path, char *name, char *target, char *extra)
{ char *p, *q, *r;
 
  if( NULL != (p = strchr(path, '?')) )
    *p++ = '\0';
  else
    p = "";

  if( dosendf("<form action=\"%s\"%s>", path, target) )
    return true;
  while( *p )
  { if( NULL != (q = strchr(p, '=')) )
    { *q++ = '\0';
      if( NULL != (r = strchr(q, '&')) )
        *r++ = '\0';
      else
        r = "";
      if( dosendf("<input name=\"%s\" type=\"hidden\" value=\"%s\">",
                  p, q) )
        return true;
      p = r;
    }
    else
      break;
  }
  if( css_defs[CSS_BUTTON] )
    return dosendf("<input type=\"submit\" class=\"%s\"%s value=\"%s\"></form>\n",
                   css_defs[CSS_BUTTON], extra, name);
  else
    return dosendf("<input type=\"submit\"%s value=\"%s\"></form>\n", extra, name);
}


/***************************************************************************************/
/* int send_button(char *name, char *text, char *value, char *extra, char *class)      */
/*                      char *name  : Name des Buttons                                 */
/*                      char *text  : Text des Buttons                                 */
/*                      char *value: optional Wert des Buttons                         */
/*                      char *extra: z. B. ' title="..."'                              */
/*                      char *class: optional Klasse                                   */
/*     send_button erzeugt Button mit Name und Wert                                    */
/***************************************************************************************/
int send_button(char *name, char *text, char *value, char *extra, char *class)
{ if( value && *value )
  { if( class && *class )
      return dosendf("<button type=\"submit\" class=\"%s\"%s name=\"%s\" value=\"%s\">%s</button>\n",
                     class, extra, name, value, text);
    else if( css_defs[CSS_BUTTON] )
      return dosendf("<button type=\"submit\" class=\"%s\"%s name=\"%s\" value=\"%s\">%s</button>\n",
                     css_defs[CSS_BUTTON], extra, name, value, text);
    else
      return dosendf("<button type=\"submit\"%s name=\"%s\" value=\"%s\">%s</button>\n",
                     extra, name, value, text);
  }
  else
  { if( class && *class )
      return dosendf("<input type=\"submit\" class=\"%s\"%s name=\"%s\" value=\"%s\">\n",
                     class, extra, name, text);
    else if( css_defs[CSS_BUTTON] )
      return dosendf("<input type=\"submit\" class=\"%s\"%s name=\"%s\" value=\"%s\">\n",
                     css_defs[CSS_BUTTON], extra, name, text);
    else
      return dosendf("<input type=\"submit\"%s name=\"%s\" value=\"%s\">\n",
                     extra, name, text);
  }
}


/***************************************************************************************/
/* int send_par(char token, char *name, char *opts, char *text, char *def,             */
/*              int b, int h, int rda_flag, char *partitle, char *parclass, char *add) */
/*              char *name   : Name des Eingabefeldes                                  */
/*              char *opts   : Elemente eines Optionsfeldes                            */
/*              char *text   : Text, der vor dem Feld erscheinen soll, bei NULL name   */
/*              char *def    : Text, mit der Vorbelegung                               */
/*              int b        : Parameter Breite, wenn angegeben                        */
/*              int h        : Parameter Höhe, wenn angegeben                          */
/*              int rda_flag : ReadOnly/disabled/autofocus flag  Bit 1: read-only      */
/*                                                               Bit 2: disabled       */
/*                                                               Bit 4: autofocus      */
/*              char *partitle: title                                                  */
/*              char *parclass: css-class                                              */
/*              char *add     : zusätzlicher String für <input ...>                    */
/*              return       : true im Fehlerfall                                      */
/*     send_par sendet den Code für ein Eingabefeld                                    */
/***************************************************************************************/
int send_par(char token, char *name, char *opts, char *text, char *def, int b, int h,
             int rda_flag, char *partitle, char *parclass, char *add)
{ char value[MAX_ZEILENLAENGE+10];                       /* String fuer Def. Werte     */
  char class[MAX_ZEILENLAENGE];                          /* ggf. String für class      */
  char t[MAX_ZEILENLAENGE];                              /* Beschreibung               */
  char ta[50], *ts, *te, *ste, *tdclass;                 /* Zeichenketten für Tabellen */
  char *ro;                                              /* Zeichenkette für readonly  */
  char addstr[2*PARLEN];                                 /* ggf. title="..." und *add  */

  LOG(1, "send_par, name: %s, opts: %s, text: %s, def: %s, rda_flag: %d.\n",
      name, opts, text ? text : "(NULL)", def, rda_flag);
  LOG(3, "send_par, tablelevel: %d, tablecols: %d, akt_tablecols: %d.\n",
      tablelevel, tablecols, akt_tablecols);

  if( partitle )
  { strcpy(addstr, " title=\"");
    te = addstr + 8;
    ste = partitle;
    while( *ste && te - addstr < 2*PARLEN-2 )
    { if( *ste == '"' )
      { if( te - addstr + 6 < 2*PARLEN-2 )
        { *te++ = '&';
          *te++ = 'q';
          *te++ = 'u';
          *te++ = 'o';
          *te++ = 't';
          *te++ = ';';
        }
        ste++;
      }
      else
        *te++ = *ste++;
    }
    *te++ = '"';
    *te = '\0';
  }
  else
  { te = addstr;
    *te = '\0';
  }

  if( add && *add && te - addstr < 2*PARLEN-2 )
  { *te++ = ' ';
    strcpyn(te, add, 2*PARLEN - (te - addstr));
  }

  if( text )                                                /* Beschreibung angegeben? */
  { strcpyn(t, text, MAX_ZEILENLAENGE);
  }
  else if( par_old_format )
  { strcpyn(t, name, MAX_ZEILENLAENGE);
    strcatn(t, ":", MAX_ZEILENLAENGE);
  }
  else
    t[0] = '\0';

  if( !par_old_format )
  { ste = te = ts = "";
    ta[0] = '\0';
  }
  else if( tablecols )
  { if( akt_tablecols < aligntablecols[tablelevel-1] )
    { if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'r' )
        tdclass = " class=\"u2wtaright\"";
      else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'c' )
        tdclass = " class=\"u2wtacenter;\"";
      else if( *(table_aligns[tablelevel-1]+akt_tablecols) == 'L' )
        tdclass = " class=\"u2wtaleft;\"";
      else
        tdclass = "";
    }
    else
      tdclass = "";
    if( newlineflag )
    { snprintf(ta, 50, "<tr><td%s>", tdclass);
      ste = te = "</td></tr>\n";
    }
    else
    { if( akt_tablecols++ )
        snprintf(ta, 50, "<td%s>", tdclass);
      else
        snprintf(ta, 50, "<tr><td%s>", tdclass);
      ste = te = "</td>\n";
    }
    if( akt_tablecols+1 < aligntablecols[tablelevel-1] )
    { if( *(table_aligns[tablelevel-1]+akt_tablecols+1) == 'r' )
        ts = "</td><td class=\"taright;\">";
      else if( *(table_aligns[tablelevel-1]+akt_tablecols+1) == 'c' )
        ts = "</td><td class=\"tacenter;\">";
      else if( *(table_aligns[tablelevel-1]+akt_tablecols+1) == 'L' )
        ts = "</td><td class=\"taleft;\">";
      else
        ts = "</td><td>";
    }
    else
      ts = "</td><td>";
  }
  else
  { ta[0] = '\0';
    ts = "";
    if( newlineflag )
    { te = "<br><br>\n";
      ste = "<br>\n";
    }
    else
      ste = te = "&nbsp;";
  }

  if( (rda_flag & 3) == 3 )
    ro = " readonly disabled";
  else if( rda_flag & 1 )
    ro = " readonly";
  else if( rda_flag & 2 )
    ro = " disabled";
  else if( rda_flag & 4 )
    ro = " autofocus";
  else
    ro = "";

  switch( token )                                           /* Feldart                 */
  { case T_PAR_TEXT:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_TEXT] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_TEXT]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"text\"%s%s size=%d maxlength=%d%s%s>%s",
               ta, t, *t ? ts : "", name, class, addstr,
               b > 0 ? b : PAR_TEXT_SIZE,
               h > 0 ? h : PARLEN-1, value, ro, te);        /* Textfeld                */
      break;
    case T_PAR_COLOR:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PCOLOR] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PCOLOR]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"color\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_DATE:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PDATE] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PDATE]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"date\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_DATETIMETZ:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PDATETIME] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PDATETIME]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"datetime\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_DATETIME:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PDATETIME] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PDATETIME]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"datetime-local\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_EMAIL:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PEMAIL] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PEMAIL]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"email\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_MONTH:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PMONTH] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PMONTH]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"month\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_NUMBER:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PNUMBER] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PNUMBER]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"number\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_RANGE:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PRANGE] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PRANGE]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"range\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_SEARCH:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PSEARCH] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PSEARCH]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"search\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_TEL:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PTEL] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PTEL]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"tel\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_TIME:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PTIME] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PTIME]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"time\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_URL:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PURL] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PURL]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"url\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_WEEK:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PWEEK] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PWEEK]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"week\"%s%s %s%s%s>%s",
               ta, t, *t ? ts: "", name, class, addstr,
               value, opts, ro, te);
      break;
    case T_PAR_TEXTAREA:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_TEXTAREA] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_TEXTAREA]);
      else
        class[0] = '\0';
      return dosendhf("%s%H%s<textarea name=\"%s\"%s%s wrap=\"soft\" rows=%d cols=%d%s>%H</textarea>%s",
               ta, t, *t ? (tablecols ? ts : ste) : "", name, class, addstr,
               h > 0 ? h : PAR_TEXTAREA_ROWS,
               b > 0 ? b : PAR_TEXTAREA_COLS, ro,
               def, te);
                                                            /* Textareafeld            */
      break;
    case T_PAR_PWD:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_PASSWORD] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_PASSWORD]);
      else
        class[0] = '\0';
      if( *def )
      { code_value(zeile, def, MAX_ZEILENLAENGE);
        snprintf(value, MAX_ZEILENLAENGE+10, " value=\"%s\"", zeile);
      }
      else
        value[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"password\"%s%s size=%d maxlength=%d%s%s>%s",
               ta, t, *t ? ts : "", name, class, addstr,
               b > 0 ? b : PAR_TEXT_SIZE,
               h > 0 ? h : PARLEN-1, value, ro, te);        /* Passwortfeld            */
      break;
    case T_PAR_CHECKBOX:                                    /*                Checkbox */
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_CHECKBOX] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_CHECKBOX]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      return dosendhf("%s%H%s<input name=\"%s\" type=\"checkbox\"%s%s value=\"1\"%s%s>%s",
               ta, t, *t ? ts : "", name, class, addstr,
               *def == '1' ? " checked=\"checked\"" : "", ro, te);
      break;
    case T_PAR_CHECKID:                                     /*                Checkbox */
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_CHECKBOX] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_CHECKBOX]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      return dosendhf("%s%H%s<input name=\"%s\" type=\"checkbox\"%s%s value=\"%s\"%s%s>%s",
               ta, t, *t ? ts : "", name, class, addstr, opts,
               *def != '\0' ? " checked=\"checked\"" : "", ro, te);
      break;
    case T_PAR_CHECK:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_CHECKBOX] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_CHECKBOX]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      if( dosendhf("%s%H%s\n",
                  ta, t, *t ? (tablecols ? ts : ste) : "") )
                                                            /* Checkboxen              */
        return true;
      if( send_checkboxen(name, opts, def, b, ro, class) )  /* Buttons senden          */
        return true;
      return dosend(ste);
      break;
    case T_PAR_OPTION:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_OPTION] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_OPTION]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      if( dosendhf("%s%H%s<select name=\"%s\"%s%s%s size=%d>\n",
                  ta, t, *t ? (tablecols || b <= 1 ? ts : ste) : "", name, ro, class,
                  addstr, b > 0 ? b : 1) )
        return true;
      if( send_opteintraege(opts, def) )                    /* Eintraege senden        */
        return true;
      return dosend("</select>") || dosend(te);
      break;
    case T_PAR_SELECT:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_SELECT] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_SELECT]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      if( b == 0 )
      { if( dosendhf("%s%H%s<select name=\"%s\"%s%s%s multiple>\n",
                    ta, t, *t ? (tablecols ? ts : ste) : "", name, ro, class, addstr) )
          return true;
      }
      else
      { if( dosendhf("%s%H%s<select name=\"%s\"%s%s%s size=%d multiple>\n",
                    ta, t, *t ? (tablecols || b <= 1 ? ts : ste) : "", name, ro, class,
                    addstr, b > 0 ? b : 1) )
          return true;
      }
      if( send_opteintraege(opts, def) )                    /* Eintraege senden        */
        return true;
      return dosend("</select>") || dosend(te);
      break;
    case T_PAR_LINKLIST:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_LINKLIST] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_LINKLIST]);
      else
        class[0] = '\0';
      if( dosendhf("%s%H%s\n",                              /* Start der Linkliste     */
                  ta, t, *t ? (tablecols ? ts : ste) : "") ) 
        return true;
      if( send_linklisteintraege(opts, name, b, class) )    /* Eintraege senden        */
        return true;
      return dosend(te);
      break;
    case T_PAR_RADIO_BUTTON:                                /* Radiob. mit Beschreibung*/
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_RADIO] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_RADIO]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      return dosendhf("%s<input name=\"%s\" type=\"radio\"%s%s value=\"%s\"%s%s>%s%H%s",
                     ta, name, class, addstr,
                     opts, *def ? " checked=\"checked\"" : "", ro, *t ? ts : "", t, te);

      break;
    case T_DATALIST:                                       /* Datalist                 */
      if( dosendhf("<datalist id=\"%s\">", name) )
        return true;
      if( send_datalistvalues(opts) )                      /* Datalist values          */
        return true;
      return dosend("</datalist>");
      break;
    case T_PAR_RADIO:                                       /* Radiobuttons            */
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_RADIO] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_RADIO]);
      else
        class[0] = '\0';
      if( rda_flag & 3 )
        ro = " DISABLED";
      if( dosendhf("%s%H%s",
                   ta, t, *t ? (tablecols ? ts : ste) : "") )
        return true;
      if( send_radiobuttons(name, opts, def, b, ro, class) )  /* Buttons senden        */
        return true;
      return dosend(te);
      break;
    case T_PAR_FILE:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_FILE] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_FILE]);
      else
        class[0] = '\0';
      return dosendhf("%s%H%s<input name=\"%s\" type=\"file\"%s%s size=%d maxlength=%d value=\"(upload)\"%s>%s",
                     ta, t, *t ? ts : "", name, class, addstr,
                     b > 0 ? b : PAR_TEXT_SIZE,
                     h > 0 ? h : PARLEN-1, ro, te);
      break; 
    case T_PAR_BUTTON:
      return send_link_button(name, t, "", addstr);
      break;
    case T_PAR_PBUTTON:
      return send_button(name, t, def, addstr, parclass);
      break;
    case T_PAR_BACKBUTTON:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_BUTTON] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_BUTTON]);
      else
        class[0] = '\0';
      return dosendf("%s<input type=\"button\"%s%s value=\"%s\" onClick=\"history.go(-%d)\">%s",
                      ta, class, addstr,
                      text ? t : _("Back"), b > 0 ? b : 1, te);
      break;
    case T_PAR_OKBUTTON:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_BUTTON] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_BUTTON]);
      else
        class[0] = '\0';
      if( text )
      { if( *name )
          return dosendf("%s<button type=\"submit\"%s%s name=\"" OKBUTTONNAME "\" value=\"%s\">%s</button>%s",
                         ta, class, addstr, name, text, te);
        else
          return dosendf("%s<button type=\"submit\"%s%s name=\"" OKBUTTONNAME "\" value=\"OK\">%s</button>%s",
                         ta, class, addstr, text, te);
      }
      else
      { if( *name )
          return dosendf("%s<input type=\"submit\"%s%s name=\"" OKBUTTONNAME "\" value=\"%s\">%s",
                         ta, class, addstr, name, te);
        else
          return dosendf("%s<input type=\"submit\"%s%s name=\"" OKBUTTONNAME "\" value=\"OK\">%s",
                         ta, class, addstr, te);
      }
      break;
    case T_PAR_CLOSEBUTTON:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_BUTTON] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_BUTTON]);
      else
        class[0] = '\0';
      return dosendf("%s<input type=\"button\"%s value=\"%s\" onClick=\"window.close()\">%s",
                     ta, class, text ? t : _("   Close   "), te);
      break;
    case T_PAR_MAINMENU:
      if( parclass )
      { if( *parclass )
          snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", parclass);
        else
          class[0] = '\0';
      }
      else if( css_defs[CSS_BUTTON] )
        snprintf(class, MAX_ZEILENLAENGE, " class=\"%s\"", css_defs[CSS_BUTTON]);
      else
        class[0] = '\0';
      return dosendf("<form action=\"%s\"><input name=\"" MENUNAME "\" type=\"hidden\" value=\"" MENUON "\"><input type=\"submit\"%s%s value=\"%s\"></form>",
               name, class, addstr, text ? t : _("Main Menu"));
      break;
  }
  return false;
}


/***************************************************************************************/
/* int start_formular(int getflag, int multipart_flag, char *action)                   */
/*                    int getflag   : true: Formular mit Methode GET, sonst POST       */
/*                    int multipart_flag: true: Formular mit multipart/form-data       */
/*                    char *action: action Seite                                       */
/*                    return: true im Fehlerfall                                       */
/*     start_formular oeffnet einen HTML-Block fuer Parametereingaben                  */
/***************************************************************************************/
int start_formular(int getflag, int multipart_flag, char *action)
{ char *p;

  LOG(1, "start_formular, action: %s.\n", action);

  if( css_defs[CSS_FORM] )
  { if( dosendf("<form class=\"%s\" %s name=\"Eingaben\" ",
                css_defs[CSS_FORM], parameter_ext) )
      return true;
  }
  else if( dosendf("<form %s name=\"Eingaben\" ", parameter_ext) )
    return true;

  if( *action == PAR_PAR_CHAR )
  { action++;
    strcpyn_l(zeile, &action, MAX_ZEILENLAENGE);
    skip_blanks(action);
    if( dosend("target=\"")
        || dosend(zeile)
        || dosend("\" ") )
      return true;
  }

  if( getflag )
  { if( dosendf("method=get action=\"%s\">\n", action) )
      return true;
  }
  else if( multipart_flag )
  { if( dosendf("method=post enctype=\"multipart/form-data\" action=\"%s\">\n", action) )
      return true;
  }
  else
  { if( dosendf("method=post action=\"%s\">\n", action) )
      return true;
  }

  if( (p=parwert(MENUNAME, HP_BROWSER_LEVEL)) )
    if( dosend("<input name=\"" MENUNAME "\" type=\"hidden\" value=\"")
           || dosend(p)
           || dosend("\">") )
      return true;
  if( (p=parwert(HAUPTMENUPAR, HP_BROWSER_LEVEL)) )
  { if( dosend("<input name=\"" HAUPTMENUPAR "\" type=\"hidden\" value=\"")
           || dosend(p)
           || dosend("\">") )
      return true;
    strcpyn(zeile, clientgetpath, MAX_ZEILENLAENGE);
    code_pars(zeile);
    if( dosend("<input name=\"" LASTMENUPAR "\" type=\"hidden\" value=\"")
           || dosend(zeile)
           || dosend("\">") )
      return true;
  }
  if( *rechtspage )
    if( dosend("<input name=\"" RECHTSMENU "\" type=\"hidden\" value=\"")
           || dosend(rechtspage)
           || dosend("\">") )
      return true;
  if( *obenpage )
    if( dosend("<input name=\"" OBENMENU "\" type=\"hidden\" value=\"")
           || dosend(obenpage)
           || dosend("\">") )
      return true;


  return false;
}


/***************************************************************************************/
/* int end_formular(char *inz, int tablecols)                                          */
/*                  char *inz: Zeile hinter %end/%endpar                               */
/*                  int tablecols : Anzahl Spalten der Tabelle                         */
/*                  return: true im Fehlerfall                                         */
/*     end_formular schliesst den HTML-Block fuer Parametereingaben                    */
/***************************************************************************************/
int end_formular(char *inz, int tablecols)
{ char name[MAX_NAME_LEN];

  LOG(1, "end_formular, tablecols: %d, inz: %s.\n", tablecols, inz);

  if( tablecols )
  { if( dosendf("<tr><td colspan=\"%d\" style=\"height: 2em;text-align: center;vertical-align: middle\">",
                tablecols) )
      return true;
  }

  if( !*inz )
  { if( css_defs[CSS_BUTTON] )
    { if( dosendhf("<input type=\"submit\" class=\"%s\" name=\"" OKBUTTONNAME
                   "\" value=\"OK\">\n", css_defs[CSS_BUTTON]) )
        return true;
      if(dosendhf("<input type=\"reset\" class=\"%s\" value=\"Clear\">\n",
                  css_defs[CSS_BUTTON]) )
        return true;
    }
    else
    { if( dosend("<input type=\"submit\" name=\"" OKBUTTONNAME "\" value=\"OK\">\n") )
        return true;
      if(dosend("<input type=\"reset\" value=\"Clear\">\n") )
        return true;
    }
  }
  else if( !is_command(inz, ENDBLOCK_NB) )
  { while( *inz )
    { if( is_command_z(&inz, END_OK) )
      { if( *inz == PAR_PAR_CHAR )
        { inz++;
          scan_to(name, &inz, MAX_NAME_LEN, 0, " \t", '\0');
          if( css_defs[CSS_BUTTON] )
          { if( dosendhf("<input type=\"submit\" class=\"%s\" name=\"" OKBUTTONNAME "\" value=\"%H\">\n",
                         css_defs[CSS_BUTTON], name) )
              return true;
          }
          else
          { if( dosendhf("<input type=\"submit\" name=\"" OKBUTTONNAME "\" value=\"%H\">\n",
                        name) )
              return true;
          }
        }
        else
        { if( css_defs[CSS_BUTTON] )
          { if( dosendhf("<input type=\"submit\" class=\"%s\" name=\"" OKBUTTONNAME "\" value=\"OK\">\n",
                        css_defs[CSS_BUTTON]) )
              return true;
          }
          else
          { if( dosend("<input type=\"submit\" name=\"" OKBUTTONNAME "\" value=\"OK\">\n") )
              return true;
          }
        }
      }
      else if( is_command_z(&inz, END_AB) )
      { if( css_defs[CSS_BUTTON] )
        { if( dosendhf("<input type=\"reset\" class=\"%s\" value=\"%s\">\n",
                       css_defs[CSS_BUTTON], _("Abort")) )
            return true;
        }
        else
        { if( dosendhf("<input type=\"reset\" value=\"%s\">\n", _("Abort")) )
            return true;
        }
      }
      else if( is_command_z(&inz, END_BA) )
      { if( *inz == PAR_PAR_CHAR )
        { int i;

          inz++;
          scan_to(name, &inz, MAX_NAME_LEN, 0, " \t", '\0');
          if( 0 < (i = atoi(name)) )
          { if( css_defs[CSS_BUTTON] )
            { if( dosendhf("<input type=\"button\" class=\"%s\" value=\"%s\" onClick=\"history.go(-%d)\">\n",
                           css_defs[CSS_BUTTON], _("Back"), i) )
                return true;
            }
            else
            { if( dosendhf("<input type=\"button\" value=\"%s\" onClick=\"history.go(-%d)\">\n",
                          _("Back"), i) )
                return true;
            }
          }
        }
        else
        { if( css_defs[CSS_BUTTON] )
          { if( dosendhf("<input type=\"button\" class=\"%s\" value=\"%s\" onClick=\"history.go(-1)\">\n",
                         css_defs[CSS_BUTTON], _("Back")) )
              return true;
          }
          else
          { if( dosendhf("<input type=\"button\" value=\"%s\" onClick=\"history.go(-1)\">\n",
                         _("Back")) )
              return true;
          }
        }
      }
      else if( is_command_z(&inz, END_CLOSE) )
      { if( css_defs[CSS_BUTTON] )
        { if( dosendhf("<input type=\"button\" class=\"%s\" value=\"%s\" onClick=\"window.close()\">\n",
                       css_defs[CSS_BUTTON], _("Close")) )
            return true;
        }
        else
        { if( dosendhf("<input type=\"button\" value=\"%s\" onClick=\"window.close()\">\n",
                      _("Close")) )
            return true;
        }
      }
      else if( is_command_z(&inz, END_CLEAR) )
      { if( css_defs[CSS_BUTTON] )
        { if(dosendhf("<input type=\"reset\" class=\"%s\" value=\"%s\">\n",
             css_defs[CSS_BUTTON], _("Clear")) )
            return true;
        }
        else
        { if(dosendhf("<input type=\"reset\" value=\"%s\">\n", _("Clear")) )
            return true;
        }
      }
      else
        skip_next_blanks(inz);
    }

    if( tablecols )
      if( dosend("</td></tr>\n") )
        return true;
  }

  if( tablecols )
  { if( dosend("</table>\n") )
      return true;
    LOG(22, "end_formular - sendhiddenvars, tablelevel: %d.\n", tablelevel);
    if( sendhiddenvars() )
      return true;
  }

  return dosend("<input name=\"" FORMMAGIC "\" type=\"hidden\" value=\"1\">\n</form>\n");
}
