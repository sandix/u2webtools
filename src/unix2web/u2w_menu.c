/***********************************************/
/* File: u2w_menu.c                            */
/* Funktionen zur Erzeugung von Menueseiten    */
/* timestamp: 2015-05-02 18:08:34              */
/***********************************************/


#include "u2w.h"

#ifndef COMPILER

/***************************************************************************************/
/* void get_subname(char *subname, char **z, long n)                                   */
/*                  char *subname: durch pars ersetzter Subname                        */
/*                  char **z     : Leseposition                                        */
/*                  long n       : Platz in subname                                    */
/*      get_subname liest an z den Namen des Submenüs nach %sub, %shell, %link, %*menu */
/*                  *z Zeigt anschliessen auf ' ' oder ',' oder ';'                    */
/***************************************************************************************/
void get_subname(char *subname, char **z, long n)
{ char *p;

  p = subname;
  while( **z && **z != ' ' && **z != '\t' && **z != ',' && **z != ';' && **z != '%'
         && **z != '"' && **z != '\'' && **z != '(' && p-subname < n-1 )
    *p++ = *(*z)++;
  *p = '\0';
  scan_teil(subname, n);
}



/***************************************************************************************/
/* void get_defpage(char *def, char **z, long n)                                       */
/*                  char *def: Adresse Defpage                                         */
/*                  char **z     : Leseposition                                        */
/*                  long n       : Platz in def                                        */
/*      get_defpage liest an z die Adresse einer Defpage für oben oder rechts          */
/*                  *z Zeigt anschliessen auf ' ' oder ':'                             */
/***************************************************************************************/
void get_def(char *def, char **z, long n)
{ char *p, c;

  p = def;
  while( **z && **z != ' ' && **z != '\t' && **z != PAR_PAR_CHAR
         && p-def < n-1 )
  { if( file_flag && (**z == SHELL_ESCAPE || **z == SHELL_INLINE) )
    { c = *p++ = *(*z)++;
      while( **z && **z != c )
        *p++ = *(*z)++;
      if( **z )
        *p++ = *(*z)++;
    }
    else
      *p++ = *(*z)++;
  }
  *p = '\0';
  scan_teil(def, n);
}



/***************************************************************************************/
/* int send_menu(char *def, char *left, char *headsize, char *leftsize,                */
/*               short noframeborders)                                                 */
/*               char *def: Defaultseite nach %end                                     */
/*               char *left: Default left Frame                                        */
/*               char *headsize: Groesse Kopfbereich                                   */
/*               char *leftsize: Groesse linker Frame                                  */
/*               short noframeborders: Bitmaske - 1: no border, 2: no leftborder       */
/*     send_menu erstellt eine Dreigeteilte Seite                                      */
/***************************************************************************************/
int send_menu(char *def, char *left, char *headsize, char *leftsize,
              short noframeborders)
{ char link[MAX_ZEILENLAENGE], *p;

 LOG(1, "send_menu, def: %s, left: %s, headsize: %s, leftsize: %s, noframeb: %d.\n",
     def, left, headsize, leftsize, noframeborders);

  if( (p = parwert(OBENMENUSIZE, HP_BROWSER_LEVEL)) )
  { if( dosendf("<frameset %s rows=\"%s,*\"><frame name=\"" MENUFRAMETOPNAME  "\" src=\"",
                noframeborders & 1 ? "frameborder=\"0\" framespacing=\"0\"": "border=\"1\" frameborder=\"1\"", p) )
      return true;
  }
  else if( *headsize )
  { if( dosendf("<frameset %s rows=\"%s,*\"><frame name=\"" MENUFRAMETOPNAME  "\" src=\"",
                noframeborders & 1 ? "frameborder=\"0\" framespacing=\"0\"": "border=\"1\" frameborder=\"1\"", headsize) )
      return true;
  }
  else if( noframeborders & 1 )
  { if( dosend("<frameset frameborder=\"0\" framespacing=\"0\" rows=\"10%,*\"><frame name=\"" MENUFRAMETOPNAME  "\" src=\"") )
      return true;
  }
  else
  { if( dosend("<frameset border=\"1\" frameborder=\"1\" rows=\"10%,*\"><frame name=\"" MENUFRAMETOPNAME  "\" src=\"") )
      return true;
  }

  if( (p=parwert(OBENMENU, HP_BROWSER_LEVEL)) )
  { code_link(link, p, MAX_ZEILENLAENGE, false);
    if( dosend(link) )
      return true;
  }
  else if( headframe && *headframe )
  { LOG(17, "send_menu, headframe: %s.\n", headframe);
	  code_link(link, headframe, MAX_ZEILENLAENGE, false);
    LOG(19, "send_menu, headframe-link: %s.\n", link);
    if( dosend(link) )
      return true;
  }
  else
  { if( dosend(clientgetfile) || dosend("?" MENUNAME "=" MENUTOP) )
      return true;
  }

  if( noframeborders == 3 )
  { if( (p=parwert(LINKSMENUSIZE, HP_BROWSER_LEVEL)) )
    { if( dosendf("\"><frameset cols=\"%s,*\"><frame src=\"", p) )
        return true;
    }
    else if( *leftsize )
    { if( dosendf("\"><frameset cols=\"%s, *\"><frame src=\"", leftsize) )
        return true;
    }
    else
    { if( dosend("\"><frameset cols=\"15%, *\"><frame src=\"") )
        return true;
    }
  }
  else
  { if( (p=parwert(LINKSMENUSIZE, HP_BROWSER_LEVEL)) )
    { if( dosendf("\"><frameset %s cols=\"%s,*\"><frame src=\"",
                noframeborders == 2 ? "frameborder=\"0\" framespacing=\"0\"": "border=\"1\" frameborder=\"1\"", p) )
        return true;
    }
    else if( *leftsize )
    { if( dosendf("\"><frameset %s cols=\"%s, *\"><frame src=\"",
                noframeborders == 2 ? "frameborder=\"0\" framespacing=\"0\"": "border=\"1\" frameborder=\"1\"", leftsize) )
        return true;
    }
    else if( noframeborders == 2 )
    { if( dosend("\"><frameset frameborder=\"0\" framespacing=\"0\" cols=\"15%, *\"><frame src=\"") )
        return true;
    }
    else
    { if( dosend("\"><frameset border=\"1\" frameborder=\"1\" cols=\"15%, *\"><frame src=\"") )
        return true;
    }
  }

  if( (p=parwert(LINKSMENU, HP_BROWSER_LEVEL)) )
  { code_link(link, p, MAX_ZEILENLAENGE, false);
    if( dosend(link) )
      return true;
  }
  else if( *left )
  { LOG(17, "send_menu, left: %s.\n", left);
	code_link(link, left, MAX_ZEILENLAENGE, false);
    LOG(19, "send_menu, left-link: %s.\n", link);
    if( dosend(link) )
      return true;
  }
  else if( (p=parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )
  { if( dosend(clientgetfile) || dosend("?" SUBMENUPAR "=") || dosend(p) )
      return true;
  }
  else
  { if( dosend(clientgetfile) || dosend("?" MENUNAME "=" MENUON) )
      return true;
  }

  if( dosend("\" name=\"" MENUFRAMELEFTNAME  "\"><frame src=\"") )
    return true;

  if( *def )
  { code_link(link, def, MAX_ZEILENLAENGE, false);
    if( dosend(link) )
      return true;
  }
  else
  { if( dosend(clientgetfile) || dosend("?" MENUNAME "=" MENURECHTS ))
      return true;
  }

  return dosend("\" name=\"" MENUFRAMERIGHTNAME  "\"></frameset><noframes><body>Diese Seite verwendet Frames, der Browser unterstützt keine Frames!</body></noframes></frameset>\n</HTML>\n");
}


/***************************************************************************************/
/* int send_submenu(char *zeile)                                                       */
/*                  char *zeile: Eintraege des Submenues                               */
/*     send_submenu erstellt Untermenue Eintrag (Link)                                 */
/***************************************************************************************/
int send_submenu(char *zeile)
{ char subname[MAX_ZEILENLAENGE];
  char def[MAX_ZEILENLAENGE], def1[MAX_ZEILENLAENGE], def2[MAX_ZEILENLAENGE];
  char *p, *q, *b, *z;
  char *subpar;

  if( !(subpar = parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )
    subpar = "";

  LOG(1, "send_submenu, zeile: %s, subpar: %s.\n", zeile, subpar);

  if( NULL != (b = strchr(zeile, MENU_HEAD_TRENN)) ) /* ab hier Beschreibung           */
  { *b++ = '\0';
    skip_blanks(b);
    scan_teil(b, MAX_ZEILENLAENGE-(b-zeile));
  }

  z = zeile;
  *def = '\0';
  if( *z == PAR_PAR_CHAR )
  { z++;
    p = z;
    while( *z && *z != ':' && *z != ' ' && *z != '\t' )
      z++;
    if( *z == PAR_PAR_CHAR )
    { *z++ = '\0';
      q = z;
      while( *z && *z != ':' && *z != ' ' && *z != '\t' )
        z++;
    }
    else
      q = "";
    if( *z )
      *z++ = '\0';
    skip_blanks(z);
    scan_to_teil(def1, p, MAX_ZEILENLAENGE);
    scan_to_teil(def2, q, MAX_ZEILENLAENGE);
    if( *def1 )
    { strcatn(def, "&" RECHTSMENU "=", MAX_ZEILENLAENGE);
      strcatn(def, def1, MAX_ZEILENLAENGE);
    }
    if( *def2 )
    { strcatn(def, "&" OBENMENU "=", MAX_ZEILENLAENGE);
      strcatn(def, def2, MAX_ZEILENLAENGE);
    }
  }

  get_subname(subname, &z, MAX_ZEILENLAENGE);

  q = subname;
  LOG(3, "send_submenu, q: %s.\n", q);
  strskip_z(&q, subpar);                             /* Untermenuestufen ueberspringen */
  if( *q == MENUSUBTRENN )                           /* '.' ?                          */
    q++;

  while( *q && *q != MENUSUBTRENN && *q != ' ' && *q != '\t' && *q != ',' && *q != ';' )
    q++;
  LOG(3, "send_submenu, new q: %s.\n", q);

  if( *q == MENUSUBTRENN )                           /* noch weiter Menüstufen         */
    return false;

  *q = '\0';

  if( b == NULL )
    b = subname;

  if( *subname )
  { if( (p=parwert(HAUPTMENUPAR, HP_BROWSER_LEVEL)) )
      return dosendf(SUBMENULINKHPTM, clientgetfile, subname, p, def, *def ? "_parent" : MENUFRAMELEFTNAME, b);
    else
      return dosendf(SUBMENULINK, clientgetfile, subname, def, *def ? "_parent" : MENUFRAMELEFTNAME, b);
  }
  else
    return false;
}


/***************************************************************************************/
/* int menu_submenu(char *zeile, int *headlineflag, char *background)                  */
/*                char *zeile     : Eintraege des Submenues                            */
/*                int *headlineflag: false, bis erste Zeile gedruckt ist               */
/*                char *background: ggf. Hintergrundbild                               */
/*     menu_submenu erstellt Seite mit dem Untermenue                                  */
/***************************************************************************************/
int menu_submenu(char *zeile, int *headlineflag, char *background)
{ char subname[MAX_ZEILENLAENGE];
  char *z;
  char *p, *b;
  char *subpar;

  LOG(1, "menu_submenu, zeile: %s.\n", zeile);

  if( !(subpar = parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )  /* subparwert bestimmen     */
    return send_submenu(zeile);                      /* Menue senden                   */

  LOG(3, "menu_submenu, subpar: %s.\n", subpar);

  z = zeile;
  if( *z == PAR_PAR_CHAR )
    skip_next_blanks(z);

  get_subname(subname, &z, MAX_ZEILENLAENGE);

  p = subname;

  LOG(3, "menu_submenu, subname: %s.\n", subname);

  if( !is_command_z(&p, subpar) )                    /* richtiges Untermenü?           */
    return false;

  if( *p == MENUSUBTRENN )                           /* noch weitere Stufen?           */
    return send_submenu(zeile);                      /* Menue senden                   */

  if( NULL != (b = strchr(z, MENU_HEAD_TRENN)) )     /* ; suchen                       */
  { *b++ = '\0';                                     /* ab hier Beschreibung           */
    skip_blanks(b);
  }
  else
    b = subname;                                     /* sonst subname als Beschreibung */

  if( NULL != (p = strchr(z, MENU_TRENN)) )          /* , vorhanden?                   */
    z = p+1;                                         /* ueberspringen                  */
  skip_blanks(z);
  if( !*headlineflag )                               /* Seitenheader schon gesendet?   */
  { if( send_headline(b) )                           /* nein, dann senden              */
      return true;
    *headlineflag = true;                            /* und merken                     */
  }
  return false;
}


/***************************************************************************************/
/* int send_subshell(char *zeile)                                                      */
/*                   char *zeile: Eintrag der Subshell                                 */
/*     send_subshell erstellt Eintrag fuer direkten Shellaufruf                        */
/***************************************************************************************/
int send_subshell(char *zeile)
{ char link[MAX_ZEILENLAENGE];
  char subname[MAX_ZEILENLAENGE];
  char *b, *z;
  char *subpar;

  if( !(subpar = parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )  /* Unterm.tiefe nach subpar */
    subpar = "";

  LOG(1, "send_subshell, zeile: %s, subpar: %s.\n", zeile, subpar);

  z = zeile;

  if( *z == PAR_PAR_CHAR )
    skip_next_blanks(z);

  LOG(3, "send_subshell, z: %s.\n", z);

  if( NULL != (b = strchr(z, MENU_HEAD_TRENN)) )     /* ';' muss angegeben sein        */
  { *b++ = '\0';                                     /* ab da beginnt die Link-        */
    skip_blanks(b);                                  /* Beschreibung                   */
    scan_teil(b, MAX_ZEILENLAENGE-(b-zeile));
  }
  else
    return false;

  if( NULL == strchr(z, MENU_TRENN) )                /* kein ',' dann                  */
  { if( !*subpar )                                   /* nur im Hauptmenue              */
    { skip_blanks(z);                                /* nach ',' steht Link            */
      if( *z )
      { code_link(link, b, MAX_ZEILENLAENGE, false);
        return dosendf(SUBMENUSHELL, clientgetfile, "", link, b);
      }
    }
  }
  else
  { get_subname(subname, &z, MAX_ZEILENLAENGE);
    if( !strcmp(subname, subpar) )                   /* richtiges Untermenü?           */
    { skip_blanks(z);
      if( *z == MENU_TRENN )                         /* ab , beginnt der Shell-Aufruf  */
      { *z++ = '\0';
        skip_blanks(z);
        if( *z )                                     /* Shell-Aufruf vorhanden?        */
        { code_link(link, b, MAX_ZEILENLAENGE, false);  /* Sonderzeichen codieren      */
          return dosendf(SUBMENUSHELL, clientgetfile, subpar, link, b);
        }
      }
    }
  }
  return false;
}


/***************************************************************************************/
/* int do_subshell(char *zeile)                                                        */
/*                 char *zeile: auszuwertende Zeile                                    */
/*                 return   : 1 Fehler, 0 ausgeführt, -1 falsche Zeile                 */
/*     do_subshell sucht zum aktuellen Inline-Shellaufruf die entsprechende Zeile      */
/*                 und erzeugt die HTML-Seite                                          */
/***************************************************************************************/
int do_subshell(char *zeile)
{ char subname[MAX_ZEILENLAENGE];
  char *subpar;                                      /* Zeigt auf Supbar-String        */
  char *p, *pp;
  char *b;
  char user[USER_LENGTH];                            /* User, auf den gewechselt wird  */

  if( !(subpar = parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )  /* Aktuelles Untermenue     */
    subpar = "";

  LOG(1, "do_subshell, zeile: %s, subpar: %s.\n", zeile, subpar);

  p = zeile;

  if( *p == PAR_PAR_CHAR )                           /* ':', dann folgt user           */
  { p++;
    strcpyn_l(user, &p, USER_LENGTH);                /* User-Parameter bestimmen       */
  }
#ifdef WEBSERVER
#ifndef CYGWIN
  else if( defuser )
    strcpyn(user, defuser, USER_LENGTH);
  else
    user[0] = '\0';
#endif
#endif

  skip_blanks(p);                                    /* Leerzeichen                    */
  pp = p;
  strcpyn_str(subname, &pp, " \t,", MAX_ZEILENLAENGE);   /* subpar Teil                */
  skip_blanks(pp);
  if( *pp != MENU_TRENN )                            /* kein ',', dann Hautpmenü       */
    *subname = '\0';
  else
  { pp++;
    skip_blanks(pp);
    p = pp;
    scan_teil(subname, MAX_ZEILENLAENGE);
  }

  LOG(3, "do_subshell, p: %s, subname: %s.\n", p, subname);

  if( strcmp(subname, subpar) )                      /* subname != subpar              */
    return -1;                                       /* weiter suchen                  */

  if( NULL != (b = strchr(p, MENU_HEAD_TRENN)) )     /* ; suchen                       */
  { *b++ = '\0';
    while( *b == ' ' )
      b++;
    scan_teil(b, MAX_ZEILENLAENGE-(b-zeile));
    if( eqbpar(SHELLMENU, b) )                       /* Beschreibung vergleichen       */
    {
#ifdef WEBSERVER
      if( set_user_mode == U2WSETUSER )              /* User nach u2w-Datei wechseln?  */
      { if( set_user(user, true) )                   /* User wechseln                  */
          return true;
      }
#endif
      if( send_headline(b) )
        return true;
      scan_teil(p, MAX_ZEILENLAENGE-(p-zeile));
      doshell(p, "", true, false, false);            /* Shell aufrufen                 */
      if( dosend(PAGE_END) )
        return true;
      return false;                                  /* gefunden, dann Ende            */
    }
  }
  return -1;                                         /* nicht gefunden, weitersuchen   */
}


/***************************************************************************************/
/* int send_submenufile(char *zeile)                                                   */
/*                      char *zeile: eingelesene Zeile                                 */
/*     send_submenufile erstellt Seite mit Untermenue aus Datei                        */
/***************************************************************************************/
int send_submenufile(char *zeile)
{ char subname[MAX_ZEILENLAENGE];
  char def[MAX_ZEILENLAENGE], def1[MAX_ZEILENLAENGE], def2[MAX_ZEILENLAENGE];
  char lm[MAX_ZEILENLAENGE];
  char *b, *q, *p, *subpar, *z;
  char *hp;

  if( !(subpar = parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )
    subpar = "";

  LOG(1, "send_submenufile, zeile: %s, subpar: %s.\n", zeile, subpar);

  if( NULL != (b = strchr(zeile, MENU_HEAD_TRENN)) ) /* ; muss angegeben sein, ab da   */
  { *b++ = '\0';                                     /* beginnt die Linkbeschreibung   */
    skip_blanks(b);
    scan_teil(b, MAX_ZEILENLAENGE-(b-zeile));
  }
  else
    return false;

  z = zeile;                                         /* nach %subfile                  */
  *def = '\0';
  if( *z == PAR_PAR_CHAR )                           /* %subfile: ?                    */
  { z++;
    p = z;
    while( *z && *z != ':' && *z != ' ' && *z != '\t' )
      z++;
    if( *z == PAR_PAR_CHAR )
    { *z++ = '\0';
      q = z;
      while( *z && *z != ':' && *z != ' ' && *z != '\t' )
        z++;
    }
    else
      q = "";
    if( *z )
      *z++ = '\0';
    skip_blanks(z);                                  /* %subfile:p:q z                 */
    strcpyn(def1, p, MAX_ZEILENLAENGE);              /* r_url                          */
    if( *def1 )
    { scan_teil(def1, MAX_ZEILENLAENGE);             /* auswerten                      */
      code_link(def2, def1, MAX_ZEILENLAENGE, true); /* Sonderzeichen kodieren         */
      strcatn(def, "&" RECHTSMENU "=", MAX_ZEILENLAENGE);
      strcatn(def, def2, MAX_ZEILENLAENGE);          /* url zusammenbauen              */
    }
    strcpyn(def1, q, MAX_ZEILENLAENGE);              /* o_url                          */
    if( *def1 )
    { scan_teil(def1, MAX_ZEILENLAENGE);             /* auswerten                      */
      code_link(def2, def1, MAX_ZEILENLAENGE, true); /* Sonderzeichen kodieren         */
      strcatn(def, "&" OBENMENU "=", MAX_ZEILENLAENGE);
      strcatn(def, def2, MAX_ZEILENLAENGE);          /* url zusammenbauen              */
    }
    if( (p=parwert(OBENMENUSIZE, HP_BROWSER_LEVEL)) )
    { strcatn(def, "&" OBENMENUSIZE "=", MAX_ZEILENLAENGE);
      strcatn(def, p, MAX_ZEILENLAENGE);
    }
    if( (p=parwert(LINKSMENUSIZE, HP_BROWSER_LEVEL)) )
    { strcatn(def, "&" LINKSMENUSIZE "=", MAX_ZEILENLAENGE);
      strcatn(def, p, MAX_ZEILENLAENGE);
    }
  }

  LOG(3, "send_submenufile, z: %s.\n", z);

  q = z;
  get_subname(subname, &z, MAX_ZEILENLAENGE);

  if( *z != MENU_TRENN )                                 /* kein ',', dann Hauptmenü   */
  { *subname = '\0';
    z = q;
  }
  else
  { z++;                                                 /* ',' überlesen              */
    skip_blanks(z);
  }

  if( strcmp(subname, subpar) )                          /* subname != subpar          */
    return false;

  scan_to_teil(def2, z, MAX_ZEILENLAENGE);

  if( !(hp = parwert(HAUPTMENUPAR, HP_BROWSER_LEVEL)) )
    hp = "";
  LOG(3, "send_submenufile, hp: %s, def: %s.\n", hp, def);

  if( *def )                                            /* r_url oder o_url angegeben? */
  { if( *hp )
    { strcpyn(lm, clientgetpath, MAX_ZEILENLAENGE);
      code_pars(lm);
      snprintf(def1, MAX_ZEILENLAENGE, SUBMENULASTMENU, def2,
               strchr(def2, '?') ? '&' : '?', hp, lm);
    }
    else
    { hp = clientgetfile;
      snprintf(def1, MAX_ZEILENLAENGE, SUBMENUINTLINK, def2,
               strchr(def2, '?') ? '&' : '?', hp);
    }
    code_link(def2, def1, MAX_ZEILENLAENGE, true);
    return dosendf(SUBMENULINKFILELAST_PARENT, hp, def2, def, b);
  }
  else
  { if( *hp )
    { strcpyn(lm, clientgetpath, MAX_ZEILENLAENGE);
      code_pars(lm);
      return dosendf(SUBMENULINKFILELAST, def2,
                     strchr(def2, '?') ? '&' : '?', hp, lm, b);
    }
    else
      return dosendf(SUBMENULINKFILE, def2,
                     strchr(def2, '?') ? '&' : '?', clientgetpath, b);
  }
}


/***************************************************************************************/
/* int send_zurueckbutton(void)                                                        */
/*     send_zurueckbutton erzeugt den Zurueckbutton                                    */
/***************************************************************************************/
int send_zurueckbutton(void)
{ char lp[MAX_ZEILENLAENGE], *p, *q, *r;

  LOG(1, "send_zurueckbutton.\n");

  if( (p=parwert(LASTMENUPAR, HP_BROWSER_LEVEL)) )
  { strcpyn(lp, p, MAX_ZEILENLAENGE);
    LOG(4, "send_zurueckbutton, lp: %s.\n", lp);
    if( (q = strchr(lp, '!')) )
    { *q++ = '\0';
      p = strstr(q, "$" LASTMENUPAR);
    }
    else
    { p = NULL;
      q = "";
    }
    if( p )
    { *p = '\0';
      p += strlen(LASTMENUPAR) + 2;
      if( dosendf("<form action=\"%s\" target=\"" MENUFRAMELEFTNAME "\">"
                  "<input name=\"" LASTMENUPAR "\" type=\"hidden\" value=\"%s\">"
                  "<input type=\"submit\" value=\"%s\">",
                  lp, p, _("   Back    ")) )
        return true;
      LOG(4, "send_zurueckbutton, p: %s.\n", p);
    }
    else
    { if( dosendf("<form action=\"%s\" target=\"" MENUFRAMELEFTNAME "\">"
                  "<input type=\"submit\" value=\"%s\">", lp, _("   Back    ")) )
        return true;
    }
    LOG(3, "send_zurueckbutton, q: %s.\n", q);
    while( *q )
    { if( (p = strchr(q, '$')) )
        *p++ = '\0';
      else
        p = "";
      if( (r = strchr(q, '=')) )
      { *r++ = '\0';
        if( dosendf("<input name=\"%s\" type=\"hidden\" value=\"%s\">", q, r) )
          return true;
      }
      q = p;
    }
    if( dosend("</form>\n") )
      return true;
  }
  return false;
}


/***************************************************************************************/
/* int send_submenufile_buttons(void)                                                  */
/*     send_submenufile_buttons erzeugt am Ende eines Submenuefiles Ruecksprungbuttons */
/***************************************************************************************/
int send_submenufile_buttons(void)
{ char *p;

  LOG(1, "send_submenu_buttons.\n");

  if( (p=parwert(HAUPTMENUPAR, HP_BROWSER_LEVEL)) )
  { LOG(4, "send_submenu_buttons, hauptmenupar: %s.\n", p);
    if( send_zurueckbutton() )
      return true;

    return dosendf("<form action=\"%s\" target=\"" MENUFRAMELEFTNAME "\">"
                   "<input name=\"" MENUNAME "\" type=\"hidden\" value=\"" MENUON "\">"
                   "<input type=\"submit\" value=\"%s\"></form>\n",
                   p, _("Main Menu"));
  }
  return false;
}


/***************************************************************************************/
/* int send_subfile(char *zeile, char *target)                                         */
/*                  char *zeile:  eingelesene Zeile                                    */
/*                  char *target: Target des Hyperlinks                                */
/*     send_subfile erstellt Seite aus Datei in neuem oder gesamtem Fenster            */
/***************************************************************************************/
int send_subfile(char *zeile, char *target)
{ char o[MAX_ZEILENLAENGE];
  char z[MAX_ZEILENLAENGE];
  char *b, *q, *subpar;
  char *t;

  if( !(subpar = parwert(SUBMENUPAR, HP_BROWSER_LEVEL)) )
    subpar = "";

  if( *zeile == PAR_PAR_CHAR )
  { zeile++;
    t = zeile;
    skip_to_blank(zeile);
    if( *zeile )
    { *zeile++ = '\0';
      skip_blanks(zeile);
    }
  }
  else
    t = target;

  LOG(1, "send_subfile, zeile: %s, subpar: %s, target: %s.\n", zeile, subpar, t);

  if( NULL != (b = strchr(zeile, MENU_HEAD_TRENN)) )
  { *b++ = '\0';
    skip_blanks(b);
    scan_teil(b, MAX_ZEILENLAENGE-(b-zeile));
  }

  q = zeile;

  strcpyn_str(z, &q, " \t,", MAX_ZEILENLAENGE);          /* subpar Teil                */
  skip_blanks(q);
  if( *q != MENU_TRENN )                                 /* kein ',', dann Hauptmenü   */
  { q = zeile;
    z[0] = '\0';
  }
  else
  { q++;                                                 /* ',' überlesen              */
    skip_blanks(q);
    scan_teil(z, MAX_ZEILENLAENGE);
  }

  if( strcmp(z, subpar) )                                /* subname != subpar          */
    return false;

  scan_to_teil(o, q, MAX_ZEILENLAENGE);
  code_link(z, o, MAX_ZEILENLAENGE, false);
  if( b == NULL )
    b = z;
  if( *t )                                               /* target?                    */
  { if( css_defs[CSS_LINK] )
      return dosendf(TARGETLINK_CLASS LINEEND, z, conv_target(t), css_defs[CSS_LINK], b);
    else
      return dosendf(TARGETLINK LINEEND, z, conv_target(t), b);
  }
  else if( css_defs[CSS_LINK] )
    return dosendf(LINK_CLASS LINEEND, z, css_defs[CSS_LINK], b);
  else
    return dosendf(LINK LINEEND, z, b);
}
#endif  /* #ifndef COMPILER */
