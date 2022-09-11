/****************************************************************/
/* File: u2w_hosts.c                                            */
/* Funktionen zur .hosts Datei                                  */
/* timestamp: 2016-03-13 00:14:30                               */
/****************************************************************/


#include "u2w.h"

#ifdef WEBSERVER

/***************************************************************************************/
/* short host_in_net(uint32_t hip, char *net)                                          */
/*                 uint32_t hip: Host-IP host-order                                    */
/*                 char *net: Netz in der Form: a.b.c.d[/n]                            */
/*                 return: true, host-ip ist im Netz net                               */
/*     host_in_net vergleicht die IP-Adresse eines hosts mit dem eines Netzes und      */
/*                 liefert true, wenn die IP-Adresse host im Netzsegment net ist.      */
/***************************************************************************************/
short host_in_net(uint32_t hip, char *net)
{ unsigned long a, b, c, d;
  int n, i;
  unsigned long nw;

  if( strchr(net, ':') )
    return false;

  i = sscanf(net, "%lu.%lu.%lu.%lu/%d", &a, &b, &c, &d, &n);
  if( i < 4 )
    return false;
  if( i < 5 )
    n = 0;
  else
    n = 32 - n;
  nw = (a << 24) | (b << 16) | (c << 8) | d;

  return (hip >> n) == (nw >> n);
}


/***************************************************************************************/
/* short host_in_list(uint32_t hip, char *list)                                        */
/*                  uint32_t hip: Host-IP host-order                                   */
/*                  char *list: IP-Liste mit Leerzeichen getrennt                      */
/*                  return: true, host ist in Liste enthalten                          */
/*     host_in_list prüft, ob die IP-des hosts in der Liste angegben ist.              */
/***************************************************************************************/
short host_in_list(uint32_t hip, char *list)
{ char *p, *q;

  q = list;
  while( *q )
  { skip_blanks(q);
    p = q;
    skip_to_blank(q);
    if( *q )
      *q++ = '\0';
    if( host_in_net(hip, p) )
      return true;
  }
  return false;
}


/***************************************************************************************/
/* auth_type test_hosts_file(char *hd, uint32_t hip,                                   */
/*                           int *r_flag, int *w_flag, int *a_flag)                    */
/*                     char *hd   : Pfad der .hosts-Datei                              */
/*                     int *r_flag: Lesen im Verzeichnis erlaubt                       */
/*                     int *w_flag: Schreiben im Verzeichnis erlaubt                   */
/*                     int *a_flag: Access auf .passwd und .hosts Datei erlaubt        */
/*                     return: true bei Zugriff, false keine Berechtigung              */
/*     test_hosts_file bestimmt anhand der Datei .hosts die Zugriffsrechte             */
/***************************************************************************************/
auth_type test_hosts_file(char *hd, uint32_t hip, int *r_flag, int *w_flag, int *a_flag)
{ FILE *ptr;                                     /* ptr auf .hosts-Datei               */
  char *z;                                       /* eingelesene Zeilen aus hosts-Datei */
  struct stat stat_buf;                          /* Zum Test, ob hosts-Datei vorhanden */
  char h[MAX_IP_LEN];                            /* IP-Adresse aus hosts-Datei         */
  char r[MAX_ACC_LEN];                           /* access-Zeichen in hosts-Datei      */

  LOG(1, "test_hosts_file, hostsfile: %s.\n", hd);
  if( !stat(hd, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
  { LOG(11, "test_hosts_file, nach stat.\n");
    if( NULL != (ptr = fopen(hd, "r")) )         /* hosts-Datei oeffnen                */
    { while( !feof(ptr) )
      { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )   /* Zeile einlesen          */
          break;
        z = zeile;
        strcpyn_l(r, &z, MAX_ACC_LEN);           /* Accessflags nach r                 */
        strcpyn_l(h, &z, MAX_IP_LEN);            /* IP-Adresse nach h                  */
        if( !strcmp(h, GLOB_IP_AUTH) )
        { if( (hosts_file_mode == GLOBAL || hosts_file_mode == GLOBAL_X)
              && hd != hosts_path )
          { if( test_hosts_file(hosts_path, hip, r_flag, w_flag, a_flag) == AUTH )
              return AUTH;
          }
          continue;
        }
        LOG(3, "test_hosts_file, r: %s, h: %s, zeile: %s.\n", r, h, zeile);
        if( !strcmp(h, NO_IP_AUTH)               /* alle IPs erlaubt?                  */
            || host_in_net(hip, h) )             /* mit host vergleichen               */
        { fclose(ptr);                           /* gefunden, ACCESS-Datei schliessen  */
          *r_flag = *r_flag && (strchr(r, 'r') != NULL);
          *w_flag = *w_flag && (strchr(r, 'w') != NULL);
          *a_flag = *a_flag && (strchr(r, 'a') != NULL);
          LOG(4, "/test_hosts_file, return true.\n");
          return AUTH;                           /* Zugriff gewaehrt zurueck           */
        }
      }
      fclose(ptr);
    }
    return BAD;
  }
  return NONE;
}


#ifdef WITH_IPV6

typedef uint32_t ip6addr[4];

/***************************************************************************************/
/* short host6_in_net(uint32_t hip6[], char *net)                                      */
/*                  uint32_t hip6[]: Host-IP host-order                                */
/*                  char *net: Netz in der Text-Form                                   */
/*                  return: true, host-ip ist im Netz net                              */
/*     host6_in_net vergleicht die IP-Adresse eines hosts mit dem eines Netzes und     */
/*                  liefert true, wenn die IP-Adresse host im Netzsegment net ist.     */
/***************************************************************************************/
short host6_in_net(uint32_t hip6[], char *net)
{ char *p;
  ip6addr ip6;
  unsigned int n;

  if( strchr(net, ':') )
  {
    if( (p = strchr(net, '/')) )
    { *p = '\0';
      n = atoi(p+1);
      if( n >= 128 )
        n = 0;
      else
        n = 128 - n;
    }
    else
      n = 0;
  
    if( !inet_pton(PF_INET6, net, ip6) )
      return false;
  
    ip6[0] = ntohl(ip6[0]);
    ip6[1] = ntohl(ip6[1]);
    ip6[2] = ntohl(ip6[2]);
    ip6[3] = ntohl(ip6[3]);
  
    if( n == 0 )
      return ip6[0] == hip6[0] && ip6[1] == hip6[1] && ip6[2] == hip6[2] && ip6[3] == hip6[3];
    else if( n < 32 )
      return ip6[0] == hip6[0] && ip6[1] == hip6[1] && ip6[2] == hip6[2] && (ip6[3] >> n) == (hip6[3] >> n);
    else if( n < 64 )
      return ip6[0] == hip6[0] && ip6[1] == hip6[1] && (ip6[2] >> (n-32)) == (hip6[2] >> (n-32));
    else if( n < 96 )
      return ip6[0] == hip6[0] && (ip6[1] >> (n-64)) == (hip6[1] >> (n-64));
    else
      return (ip6[0] >> (n-96)) == (hip6[0] >> (n-96));
  }
  else if( hip6[0] == 0 && hip6[1] == 0 && hip6[2] == 0xffff )
    return host_in_net(hip6[3], net);
  else
    return false;
}


/***************************************************************************************/
/* short host6_in_list(uint32_t hip6[], char *list)                                    */
/*                   uint32_t hip6[]: Host-IP host-order                               */
/*                   char *list: IP-Liste mit Leerzeichen getrennt                     */
/*                   return: true, host ist in Liste enthalten                         */
/*     host6_in_list prüft, ob die IP-des hosts in der Liste angegben ist.             */
/***************************************************************************************/
short host6_in_list(uint32_t hip6[], char *list)
{ char *p, *q;

  q = list;
  while( *q )
  { skip_blanks(q);
    p = q;
    skip_to_blank(q);
    if( *q )
      *q++ = '\0';
    if( host6_in_net(hip6, p) )
      return true;
  }
  return false;
}


/***************************************************************************************/
/* auth_type test_hosts6_file(char *hd, uint32_t hip6[],                               */
/*                            int *r_flag, int *w_flag, int *a_flag)                   */
/*                     char *hd   : Pfad der .hosts-Datei                              */
/*                     uint32_t hip6[]: Host-IP host-order                             */
/*                     int *r_flag: Lesen im Verzeichnis erlaubt                       */
/*                     int *w_flag: Schreiben im Verzeichnis erlaubt                   */
/*                     int *a_flag: Access auf .passwd und .hosts Datei erlaubt        */
/*                     return: true bei Zugriff, false keine Berechtigung              */
/*     test_hosts6_file bestimmt anhand der Datei .hosts die Zugriffsrechte            */
/***************************************************************************************/
auth_type test_hosts6_file(char *hd, uint32_t hip6[],
                           int *r_flag, int *w_flag, int *a_flag)
{ FILE *ptr;                                     /* ptr auf .hosts-Datei               */
  char *z;                                       /* eingelesene Zeilen aus hosts-Datei */
  struct stat stat_buf;                          /* Zum Test, ob hosts-Datei vorhanden */
  char h[MAX_IP_LEN];                            /* IP-Adresse aus hosts-Datei         */
  char r[MAX_ACC_LEN];                           /* access-Zeichen in hosts-Datei      */

  LOG(1, "test_hosts6_file, hostsfile: %s.\n", hd);
  if( !stat(hd, &stat_buf) && (S_IFREG & stat_buf.st_mode) == S_IFREG )
  { LOG(11, "test_hosts6_file, nach stat.\n");
    if( NULL != (ptr = fopen(hd, "r")) )         /* hosts-Datei oeffnen                */
    { while( !feof(ptr) )
      { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) )   /* Zeile einlesen          */
          break;
        z = zeile;
        strcpyn_l(r, &z, MAX_ACC_LEN);           /* Accessflags nach r                 */
        strcpyn_l(h, &z, MAX_IP_LEN);            /* IP-Adresse nach h                  */
        if( !strcmp(h, GLOB_IP_AUTH) )
        { if( (hosts_file_mode == GLOBAL || hosts_file_mode == GLOBAL_X)
              && hd != hosts_path )
          { if( test_hosts6_file(hosts_path, hip6, r_flag, w_flag, a_flag) == AUTH )
              return AUTH;
          }
          continue;
        }
        LOG(3, "test_hosts6_file, r: %s, h: %s, zeile: %s.\n", r, h, zeile);
        if( !strcmp(h, NO_IP_AUTH)               /* alle IPs erlaubt?                  */
            || host6_in_net(hip6, h) )           /* mit host vergleichen               */
        { fclose(ptr);                           /* gefunden, ACCESS-Datei schliessen  */
          *r_flag = *r_flag && (strchr(r, 'r') != NULL);
          *w_flag = *w_flag && (strchr(r, 'w') != NULL);
          *a_flag = *a_flag && (strchr(r, 'a') != NULL);
          LOG(4, "/test_hosts6_file, return true.\n");
          return AUTH;                           /* Zugriff gewaehrt zurueck           */
        }
      }
      fclose(ptr);
    }
    return BAD;
  }
  return NONE;
}
#endif


/***************************************************************************************/
/* auth_type test_hosts(char *pfad, int *r_flag, int *w_flag, int *a_flag)             */
/*                      char *pfad : Pfad der angeforderten Seite                      */
/*                      int *r_flag: Lesen im Verzeichnis erlaubt                      */
/*                      int *w_flag: Schreiben im Verzeichnis erlaubt                  */
/*                      int *a_flag: Access auf .passwd und .hosts Datei erlaubt       */
/*                      return: true bei Zugriff, false keine Berechtigung             */
/*           test_hosts bestimmt anhand der Datei .hosts die Zugriffsrechte            */
/***************************************************************************************/
auth_type test_hosts(char *pfad, int *r_flag, int *w_flag, int *a_flag)
{ char *p;
  char hostsdat[MAX_DIR_LEN];                    /* Aufbau des host-Pfades+Datei       */
  auth_type ret;
  uint32_t hip;

  LOG(1, "test_hosts, pfad: %s, file_mode: %d.\n", pfad, hosts_file_mode);

  if( hosts_file_mode == NOACCESSFILE )
  { LOG(3, "/test_hosts, ret: NONE.\n");
    return NONE;
  }

#ifdef WITH_IPV6
  if( clientip6struct )
  { uint32_t hip6[4];
    hip6[0] = (clientip6struct->s6_addr[0] << 24) | (clientip6struct->s6_addr[1] << 16) | (clientip6struct->s6_addr[2] << 8) | clientip6struct->s6_addr[3];
    hip6[1] = (clientip6struct->s6_addr[4] << 24) | (clientip6struct->s6_addr[5] << 16) | (clientip6struct->s6_addr[6] << 8) | clientip6struct->s6_addr[7];
    hip6[2] = (clientip6struct->s6_addr[8] << 24) | (clientip6struct->s6_addr[9] << 16) | (clientip6struct->s6_addr[10] << 8) | clientip6struct->s6_addr[11];
    hip6[3] = (clientip6struct->s6_addr[12] << 24) | (clientip6struct->s6_addr[13] << 16) | (clientip6struct->s6_addr[14] << 8) | clientip6struct->s6_addr[15];

    if( hosts_file_mode == PATH )
      ret = test_hosts6_file(hosts_path, hip6, r_flag, w_flag, a_flag);
    else
    { strcpyn(hostsdat, pfad, MAX_DIR_LEN);        /* Pfad, der angefordert wird         */

      if( NULL == (p = strrchr(hostsdat, '/')) )   /* letzten '/'                        */
        return BAD;

      strcpyn(p+1, HOSTS_FILE, MAX_DIR_LEN+hostsdat-p-1);   /* Pfad+Name der HOSTS-Datei */

      ret = test_hosts6_file(hostsdat, hip6, r_flag, w_flag, a_flag);

      if( (ret != AUTH && hosts_file_mode == GLOBAL_X)
          || (ret == NONE && hosts_file_mode == GLOBAL) )
        ret = test_hosts6_file(hosts_path, hip6, r_flag, w_flag, a_flag);
    }
  }
  else
  {
#endif
    hip = ntohl(clientipstruct->s_addr);

    if( hosts_file_mode == PATH )
      ret = test_hosts_file(hosts_path, hip, r_flag, w_flag, a_flag);
    else
    { strcpyn(hostsdat, pfad, MAX_DIR_LEN);        /* Pfad, der angefordert wird         */

      if( NULL == (p = strrchr(hostsdat, '/')) )   /* letzten '/'                        */
        return BAD;

      strcpyn(p+1, HOSTS_FILE, MAX_DIR_LEN+hostsdat-p-1);   /* Pfad+Name der HOSTS-Datei */

      ret = test_hosts_file(hostsdat, hip, r_flag, w_flag, a_flag);

      if( (ret != AUTH && hosts_file_mode == GLOBAL_X)
          || (ret == NONE && hosts_file_mode == GLOBAL) )
        ret = test_hosts_file(hosts_path, hip, r_flag, w_flag, a_flag);
    }
#ifdef WITH_IPV6
  }
#endif

  if( ret == AUTH )
  { LOG(2, "/test_hosts, ret: AUTH.\n");
    return AUTH;
  }
  else if( ret == NONE )
  { *r_flag = *r_flag && ( hosts_file_mode == NORMAL);
    *w_flag = *w_flag && ( hosts_file_mode == NORMAL);
    *a_flag = *a_flag && ( hosts_file_mode == NORMAL);
    LOG(2, "/test_hosts, ret: %s.\n", password_file_mode == NORMAL ? "NONE" : "BAD");
    return hosts_file_mode == NORMAL ? NONE : BAD;     /* keine .hosts                 */
  }
  else
  { *r_flag = *w_flag = *a_flag = false;
    LOG(1, "/test_hosts, ret: BAD.\n");
    return BAD;                                        /* kein Zugriff                 */
  }
}


/***************************************************************************************/
/* short test_host_in_list(char *list)                                                 */
/*                  char *list: IP-Liste mit Leerzeichen getrennt                      */
/*                      return: true bei Zugriff, false keine Berechtigung             */
/*      test_host_in_list prüft, ob Client IP in *list enthalten ist                   */
/***************************************************************************************/
short test_host_in_list(char *list)
{ uint32_t hip;

  LOG(1, "test_host_in_list, list: %s.\n", list);

#ifdef WITH_IPV6
  if( clientip6struct )
  { uint32_t hip6[4];
    hip6[0] = (clientip6struct->s6_addr[0] << 24) | (clientip6struct->s6_addr[1] << 16) | (clientip6struct->s6_addr[2] << 8) | clientip6struct->s6_addr[3];
    hip6[1] = (clientip6struct->s6_addr[4] << 24) | (clientip6struct->s6_addr[5] << 16) | (clientip6struct->s6_addr[6] << 8) | clientip6struct->s6_addr[7];
    hip6[2] = (clientip6struct->s6_addr[8] << 24) | (clientip6struct->s6_addr[9] << 16) | (clientip6struct->s6_addr[10] << 8) | clientip6struct->s6_addr[11];
    hip6[3] = (clientip6struct->s6_addr[12] << 24) | (clientip6struct->s6_addr[13] << 16) | (clientip6struct->s6_addr[14] << 8) | clientip6struct->s6_addr[15];

    return host6_in_list(hip6, list);
  }
  else
  {
#endif
    hip = ntohl(clientipstruct->s_addr);

    return host_in_list(hip, list);
#ifdef WITH_IPV6
  }
#endif
}


/***************************************************************************************/
/* short edit_hosts(char *hostsdat, int globflag)                                      */
/*                char *hostsdat: Pfad der .hosts-Datei                                */
/*                int globflag: true, wenn globale .hosts-Datei bearbeitet wird        */
/*               return: true bei Fehler                                               */
/*     edit_hosts Erzeugt Formular zum Ändern der .hosts-Datei                         */
/***************************************************************************************/
short edit_hosts(char *hostsdat, int globflag)
{ FILE *ptr;                                     /* ptr auf .hosts-Datei               */
  char *z;                                       /* eingelesen Zeilen aus .hosts-Dat.  */
  char ip[MAX_IP_LEN];                           /* IP-Adresse aus .hosts-Datei        */
  char r[MAX_ACC_LEN];                           /* access-Zeichen in passwd-Datei     */
  int a_flag, r_flag, w_flag;

  LOG(1, "edit_hosts, hostsdat: %s.\n", hostsdat);

  if( hosts_file_mode == NOACCESSFILE )
  {                                                  /* Fehlerseite ausgeben           */
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  if( hosts_start_form(globflag) )               /* Formularanfang ausgeben            */
    return true;

  if( NULL != (ptr = fopen(hostsdat, "r")) )     /* .hosts-Datei öffnen                */
  { while( !feof(ptr) )
    { if( NULL == fgets(zeile, MAX_ZEILENLAENGE, ptr) ) /* Zeile einlesen              */
        break;
      z = zeile;
      strcpyn_l(r, &z, MAX_ACC_LEN);             /* Access Zeichen nach r              */
      strcpyn_l(ip, &z, MAX_AUTH_LEN);           /* IP- String nach ip                 */
      a_flag = strchr(r, 'a') != NULL;           /* Access erlaubt?                    */
      r_flag = strchr(r, 'r') != NULL;           /* Read erlaubt?                      */
      w_flag = strchr(r, 'w') != NULL;           /* Write erlaubt?                     */
      if( *r && *ip )
      { if( hosts_eintrag_form(r_flag, w_flag, a_flag, ip, false) )  /* Formularzeile  */
        { fclose(ptr);                           /* bei Fehler abbruch                 */
          return true;
        }
      }
    }
    fclose(ptr);
  }
  if( hosts_eintrag_form(false, false, false, "NEU", true) )  /* Neuer Eintrag         */
    return true;
  return hosts_end_form();                       /* Formularende ausgeben              */
}


/***************************************************************************************/
/* short save_hosts(char *hostsdat, int globflag)                                      */
/*                char *hostsdat: Pfad der .hosts-Datei                                */
/*                return: true bei Fehler                                              */
/*     save_hosts Speichert neue Access-Rechte aus Formularergebnis                    */
/***************************************************************************************/
short save_hosts(char *hostsdat, int globflag)
{ FILE *ptr;                                     /* ptr auf ACCESS-Datei               */
  char ip[MAX_IP_LEN];                           /* IP-Adresse für hosts-Datei         */
  wertetype *w;                                  /* Zeigt auf die einzelnen Werte      */
  char parname[MAX_AUTH_LEN];                    /* Zum Aufbauen des Parameternamen    */
  int r_flag, w_flag, a_flag;                    /* read, write, access_flag           */
  char *p;

  LOG(1, "save_hosts, hostsdat: %s.\n", hostsdat);

  if( hosts_file_mode == NOACCESSFILE )
  {                                                  /* Fehlerseite ausgeben           */
    send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                    SEITE_NICHT_GEFUNDEN_TEXT);
    return true;
  }

  if( NULL != (ptr = fopen(hostsdat, "w")) )     /* hosts-Datei öffnen                 */
  { w = firstwert("N", HP_BROWSER_LEVEL);        /* ersten Wert zum Feld N (hidden)    */
    while( w != NULL )                           /* noch Wert vorhanden?               */
    { strcpy(parname, "L_");                     /* Löschbuttonname                    */
      strcatn(parname, wert2string(w->wert), MAX_AUTH_LEN);   /* zusammensetzen        */
      if( !eqbpar(parname, "on") )               /* nicht angeklickt, dann             */
      { parname[0] = 'R';                        /* Readbuttonname zusammensetzen      */
        r_flag = eqbpar(parname, "on");
        parname[0] = 'W';                        /* Writebuttonname zusammensetzen     */
        w_flag = eqbpar(parname, "on");
        parname[0] = 'A';                        /* Accessbuttonname zusammensetzen    */
        a_flag = eqbpar(parname, "on");
        parname[0] = 'I';
        if( (p=parwert(parname, HP_BROWSER_LEVEL)) && *p )
        { strcpyn(ip, p, MAX_IP_LEN);
          snprintf(zeile, MAX_ZEILENLAENGE, "%s%s%s%s %s\n",
                   r_flag ? "r" : "", w_flag ? "w" : "", a_flag ? "a" : "",
                   (r_flag || w_flag || a_flag) ? "" : ".",
                   ip);
          fputs(zeile, ptr);
          LOG(3, "save_hosts, zeile: %s", zeile);
        }
      }
      w = w->next;
    }
    fclose(ptr);
    return edit_hosts(hostsdat, globflag);
  }
                                                     /* Fehlerseite ausgeben           */
  send_error_page(SEITE_NICHT_GEFUNDEN_NUM, SEITE_NICHT_GEFUNDEN_DESC, "",
                  SEITE_NICHT_GEFUNDEN_TEXT);
  return true;
}


/***************************************************************************************/
/* short hosts_start_form(int globflag)                                                */
/*                      int globflag: true, wenn globale .hosts-Datei bearbeitet wird  */
/*     hosts_start_form Gibt Anfang des Editierfensters für die .hosts-Datei aus       */
/***************************************************************************************/
short hosts_start_form(int globflag)
{ if( send_http_header("text/html", "", true, 0, u2w_charset) )
    return 1;
  http_head_flag = 3;
  return dosendf(PAGE_HEADER "<title>PASSWD</title>" PAGE_HEADER_ENDE, u2w_charset)
        || send_body_start()
#ifdef DOS
        || send_headline(_("edit hosts.uaw file"))
#else
        || send_headline(_("edit .hosts file"))
#endif
        || start_formular(false, false, globflag ? SAVE_GLOBAL_ACC : clientgetfile)
        || dosend(globflag
                  ?  "<input name=\"" HOSTSEDITMODE "\" type=\"hidden\" value=\"" HOSTSGLOBSAVE "\">\n"
                  :  "<input name=\"" HOSTSEDITMODE "\" type=\"hidden\" value=\"" HOSTSSAVE "\">\n"
                 )
        || dosend("<table>\n")
        || dosendf("<tr><td align=center>R</td><td align=center>W</td><td align=center>A</td><td>%s</td><td>%s</td></tr>\n",
                   _("IP-address"), _("delete"));
}


/***************************************************************************************/
/* short hosts_end_form(void)                                                          */
/*     hosts_end_form gibt Ende des Editierfensters für die .hosts-Datei mit Buttons   */
/*                    aus                                                              */
/***************************************************************************************/
short hosts_end_form(void)
{
  if( dosend("</table>\n") || end_formular(BUTTONS, false) || dosend(PAGE_END) )
    return true;
  else
    return false;
}


/***************************************************************************************/
/* short hosts_eintrag_form(int r_flag, int w_flag, int a_flag, char *ip, int newflag) */
/*     hosts_eintrag_form gibt eine Zeile der .hosts-Datei aus                         */
/***************************************************************************************/
short hosts_eintrag_form(int r_flag, int w_flag, int a_flag, char *ip, int newflag)
{
  if( dosendf("<tr><td><input name=\"N\" type=\"hidden\" value=\"%s\">"
              "<input name=\"R_%s\" type=\"checkbox\"%s></td>"
              "<td><input name=\"W_%s\" type=\"checkbox\"%s></td>"
              "<td><input name=\"A_%s\" type=\"checkbox\"%s></td>"
              "<td><input name=\"I_%s\" type=\"text\" size=15 value=\"%s\"></td>",
              ip,
              ip, r_flag ? " CHECKED" : "",
              ip, w_flag ? " CHECKED" : "",
              ip, a_flag ? " CHECKED" : "",
              ip, newflag ? "" : ip) )
    return true;
  if( newflag )
    return dosendf("<td>%s</td></tr>\n", _("new entry"));
  else
    return dosendf("<td><input name=\"L_%s\" type=\"checkbox\"></td></tr>\n", ip);
}
#endif  /* #ifdef WEBSERVER */
