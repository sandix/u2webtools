/*********************************************/
/* File: u2w_u2w.h                           */
/* U2W Funktionen / Befehle                  */
/* timestamp: 2017-02-12 14:49:25            */
/*********************************************/


/***************************************************************************************/
/* Sonderzeichen                                                                       */
/***************************************************************************************/
#define ESCAPE_CHAR       '\\'        /* '\' Quotet folgende Sonderzeichen             */
#define CHARS_TO_ESCAPE   "\\%$#&| "  /* Zeichen, die mit '\' gequotet werden          */
#define ATTRIB_CHAR       '%'         /* mit '%' beginnt z. B. Authentifizierung       */
#define ATTRIB_STR        "%"         /* wie ATTRIB_CHAR, um Strings zu basteln        */
#define DEFAULT_WERT_CHAR ','         /* ab hier steht der Defaultwert des Parameters  */
#define LET_WERT_CHAR     '='         /* ab hier steht der Wert bei %let               */
#define BESCHREIBUNG_CHAR ';'         /* ab hier steht die Beschreibung des Parameters */
#define OPT_VALUE_CHAR    ':'         /* ab hier steht bei Optionsfeldern der Wert     */
#define PAR_CHAR          '$'         /* mit '$' beginnen Parameter                    */
                                      /* mit '$%' beginnen System parameter (%spar)    */
#define SHELL_ESCAPE      '#'         /* Kommandos in einer Zeile, CR/LF wird beachtet */
#define SHELL_INLINE      '`'         /* Shellergebnis in einer Zeile, CR/LF -> ' '    */


/***************************************************************************************/
/* Systemfunktionen                                                                    */
/***************************************************************************************/
#define MYPORT       "myport"         /* %myport ist der Port des u2w-Servers          */
#define PRINTUSER    "user"           /* %user ist angemeldeter User                   */
#define PRINTPWD     "pwd"            /* %pwd ist das Passwort zum User                */
#define METHOD       "method"         /* %method ist die HTTP-Methode (GET/POST...)    */
#define PUTDATA      "putdata"        /* Pfad zu den PUT Daten                         */
#define CLIENTIP     "clientip"       /* %clientip ist die Connect-IP-Adresse          */
                                      /* bei Zugriff über Proxy, dessen Adresse        */
#define CLIENTIP6    "clientip6"      /* IP-V4 Adressen als IP-V6 Adressen liefern     */
#define THISFILE     "this"           /* %this ist die aktuelle u2w-Datei              */
#define MYHOST       "thishost"       /* %thishost ist mein Hostname                   */
#define MYPAGE       "thispage"       /* %thispage ist die komplette Seitenanforderung */
#define ALLVARS      "allvars"        /* %allvars: alle Variablen                      */
#define ALLPARS      "allpars"        /* %allpars: alle Parameter vom Browser/CMD      */
#define REFERER      "referer"        /* %referer: Referer vom Browswer                */
#define USERAGENT    "useragent"      /* %useragent: User-Agent vom Browser            */
#define USERAGENTTYPE "uatype"        /* B(rowser), T(ablet), M(obile), C(onsole)      */
#define GETHTTPS     "https"          /* %https liefert "https" oder "http"            */
#define GETHTTP      "http"           /* %http liefert "https" oder "http"             */
#define HTTP_ACCEPT_LANGUAGE "http_accept_language" /* %http_accept_language, wie vom  */
                                      /* Browser im header                             */
#define PREFLANG     "preflang"       /* %preflang([<langlist>]): bevorzugte Sprache   */
                                      /* des Browsers [aus <langlist>] oder die erste  */
#define PAR          "par"            /* %par(name[,[ sep][, [brdr][, [max], [flgs]]]])*/
                                      /*   Variable name ausgeben,                     */
                                      /*   zwischen einzelnen Werten [sep] (def ' ')   */
                                      /*   jeden Wert mit [brdr] umschließen           */
                                      /*   maximal [max] Zeichen, (nicht implementiert)*/
                                      /*   [flgs]: S: allow shrinking (nicht implemen.)*/
                                      /*           L: trim left  (nicht implementiert) */
                                      /*           R: trim right (nicht implementerit) */
                                      /*           T: trim both  (nicht implementerit) */
#define GPAR         "gpar"           /* %gpar(name[, ...]) globale Variable name ausg.*/
#define BPAR         "bpar"           /* %bpar(name[, ...]) Parameter vom Browser ausg.*/
                                      /*   bzw. bei u2w_interpreter von der CMD-Line   */
#define BFPAR        "bfpar"          /* %bfpar(name[, ...]) File Params bei Multipart */
                                      /* <name>_error, _file, _path, _store, _mime     */
#define HPAR         "hpar"           /* %hpar(name[, ...]) Elemente des HTTP-Headers  */
#define CPAR         "cpar"           /* %cpar(name[, ...]) cookies                    */
#define BGLOBAL      "bglobal"        /* %bglobal('parlist ...'[,[ sep][, trimflag]])  */
                                      /* kopiert Browser pars zu globalen Pars         */
                                      /* parlist ist mit sep (def. ' ') getrennt       */
#define BPARDEF      "bpardef"        /* %bpardef() Def. fuer Browser Vars             */
                         /* %bpardef(<varname>, <defvalue>[, <GLOBAL>[, trimflag]])    */
                         /* wenn <varname> nicht vom Browser geliefert, auf <defvalue> */
                         /* setzen. <GLOBAL> != 0, <varname> zu global Pars kopieren   */
#define SPAR         "spar"           /* %spar(name) Systemparameter lesen             */
#define IPAR         "ipar"           /* %ipar(name) Initparameter lesen statt $%<name>*/
#define COUNTPAR     "countpar"       /* %countpar(name) Anzahl der Werte              */
#define GCOUNTPAR    "gcountpar"      /* %gcountpar(name) Anzahl Werte global          */
#define BCOUNTPAR    "bcountpar"      /* %bcountpar(name) Anzahl Werte Browser         */
#define CCOUNTPAR    "ccountpar"      /* %ccountpar(name) Anzahl Werte CMD ==bcountpar */
#define GETENV       "getenv"         /* %getenv(<name>) Environment-Variable          */
#define SETENV       "setenv"         /* %setenv(<name>, <wert>) Environment setzen    */
#define SETLANG      "setlang"        /* %setlang(<lang>) Sprache setzen, setzt Envir. */
                                      /* LANG=<lang> und ruft setlocale(LC_ALL, "") auf*/
#define GETPID       "pid"            /* %pid PID des Prozesses                        */
#define GETPPID      "ppid"           /* %ppid PID des Parent Prozesses                */
#define ISREGEXP     "isregexp"       /* %isregexp auf Regexp prüfen                   */

/***************************************************************************************/
/* Befehle, die mit % beginnen                                                         */
/***************************************************************************************/
#define TITLE        "title"          /* %title <Title> Titel des Headers festlegen    */
#define AUTHORIZE    "authorize"      /* %authorize verlangt Authentifizierung         */
#define LOGIN        "login"          /* %login, 401 Authorize senden                  */
#define CHECK_CRED   "check_cred"     /* %check_cred(<user>, <pwd>) test User und PWD  */
                                      /* beim Betriebssystem UNIX / PAM                */
#define PAGE_NOT_FOUND "page_not_found" /* %page_not_found Server sendt page not found */
#define SETUSER      "setuser"        /* %setuser <username> User ändern               */
#define NO_SETUSER_USER      "-"      /* %setuser -, User nicht aendern                */
#define NO_SETUSER_USER_CHAR '-'      /* %setuser -, User nicht aendern Zeichen        */
#define HTTPNUM      "httpnum"        /* %httpnum <n>, setzt HTTP-Responsecode auf <n> */
#define HTTPDESC     "httpdesc"       /* %httpdesc <desc>, setzt HTTP-Resp.description */
#define ERRORNUM     "errornum"       /* %errornum <n>, setzt HTTP-Errorcode auf <n>   */
                                      /*   alias zu %httpnum                           */
#define ERRORDESC    "errordesc"      /* %errordesc <desc>, setzt HTTP-Errordescription*/
                                      /*    alias zu %httpdesc                         */
#define MIME         "mime"           /* %mime <type>, setzt den Mime-Type bei s?w     */
#define SAVENAME     "savename"       /* %savename <dateiname>, Belegung für save as   */
#define CHARSET      "charset"        /* %charset <characterset>,                      */
                                      /*   setzt charset bei content-type              */
#define REFRESH      "refresh"        /* %refresh <sek> URL, setzt Meta refresh        */
#define REDIRECT     "redirect"       /* %redirect URL, 301 Moved Perm senden          */
#define SETCOOKIE    "setcookie"      /* %setcookie("name=value"[, <path>, <...>]])    */
#define PARAMETER    "parameter"      /* %parameter Start des Parameterblocks          */
#define PARAMETER_GET "parameter_get" /* %parameter_get, GET nicht POST verwenden      */
#define PARAMETER_MULTIPART "parameter_multipart"  /* %parameter_multipart, POST mit   */
                                      /*  multipart/form-data                          */
#define RESULTPAGE   "resultpage"     /* %resultpage[:target] URL                      */
#define PARAMETER_EXT "parameter_ext" /* %parameter_ext('<ext like css=".." id="..">') */
#define SETRECHTS    "setrechts"      /* %setrechts URL -> Menüseite rechts ändern     */
#define SETRIGHT     "setright"       /* %setright URL -> Menüseite rechts ändern      */
#define SETOBEN      "setoben"        /* %setoben URL -> Menüseite oben ändern         */
#define SETTOP       "settop"         /* %settop URL -> Menüseite oben ändern          */
#define SETLINKS     "setlinks"       /* %setlinks URL -> Menüseite links ändern       */
#define SETLEFT      "setleft"        /* %setleft URL -> Menüseite links ändern        */
#define FSETTOP      "fsettop"        /* %fsettop URL -> Frame oben ändern             */
#define FSETBOTTOM   "fsetbottom"     /* %fsetbottom URL -> Frame unten ändern         */
#define FSETLEFT     "fsetleft"       /* %fsetleft URL -> FRame links ändern           */
#define FSETRIGHT    "fsetright"      /* %setright URL -> Menüseite rechts ändern      */
#define SETFRAME     "setframe"       /* %setframe name URL -> Frame setzen            */
#define ENDBLOCK     "end"            /* %end, Ende eines Blockes (Parameterblock)     */
#define ENDPAR       "endpar"         /* %endpar, wie %end, es wird aber weitergelesen */
#define ENDBLOCK_NB  "nb"             /* %end nb, wie %end, nur ohne Buttons           */
#define END_OK       "ok"             /* %end ok, OK Button                            */
#define END_AB       "ab"             /* %end ab, Abbruch Button                       */
#define END_BA       "back"           /* %end back, Zurück Button                      */
#define END_CLEAR    "clear"          /* %end clear, Clear Button                      */
#define END_CLOSE    "close"          /* %end close, Close Button                      */
#define BACKGROUND   "background"     /* %background <bild> gibt Hintergrundbild an    */
#define BODYPARS     "bodypars"       /* %bodypars .... Zusatz für <BODY ...>          */
#define LEFT         "left"           /* das Formular wird linksbuendig dargestellt    */
#define CENTER       "center"         /* der Text wird zentriert dargestellt           */
#define AUS          "/"              /* Zum Ausschalten einiger Styles                */
#define BF           "b"              /* %b Ausdruck Fett                              */
#define TT           "tt"             /* %tt Ausdruck in Courier New                   */
#define KBD          "kbd"            /* % Tag <kbd>                                   */
#define SAMP         "samp"           /* % Tag <samp>                                  */
#define CODE         "code"           /* % Tag <code>                                  */
#define PRESAMP      "psamp"          /* % Tag <pre><samp>                             */
#define PRECODE      "pcode"          /* % Tag <pre><code>                             */
#define UNDERLINE    "u"              /* %u Unterstreichen ein                         */
#define HL           "h"              /* %h<n> Headline in Größe <n>                   */
#define NL           "n"              /* %n Absatzmarke einfügen                       */
#define NL_CHAR      'n'              /* %n als Zeichen                                */
#define NL_CRLF      "crlf"           /* %crlf sendet CR LF                            */
#define RED          "red"            /* rote Schrift                                  */
#define GREEN        "green"          /* grüne Schrift                                 */
#define YELLOW       "yellow"         /* gelbe Schrift                                 */
#define BLUE         "blue"           /* blaue Schrift                                 */
#define MAGENTA      "magenta"        /* magenta Schrift                               */
#define AQUA         "aqua"           /* hellblaue Schrift                             */
#define BLACK        "black"          /* schwarze Schrift                              */
#define WHITE        "white"          /* weisse Schrift                                */
#define REM_CHAR     '%'              /* Zeilen, die mit %% beginnen, sind Kommentare  */
#define IF           "if"             /* %if Bedingung, bedingte Verarbeitung          */
#define ELSE         "else"           /* %else, ab hier else Fall                      */
#define ELSEIF       "elif"           /* %elif, elseif Bedingung                       */
#define FI           "fi"             /* Ende else/then Fall                           */
#define CALC         '{'              /* Bloecke zwischen %{ ... } werden berechnet    */
#define CALC_ENDE    '}'
#define CALCSTR      "calc"           /* %calc($a), es wird der Inhalt von $a berechnet*/
                                      /* %{$a} ergibt den String $a                    */
#define ISSET        "isset"          /* %isset($a) ist true, wenn $a definiert ist    */
#define IMAGE        "image"          /* %image(<url>) Image einfügen                  */
#define OBJECT       "object"         /* %object(data, width, heigth[, type[, pars]])  */
                                      /* <object ...></object>                         */
#define SOBJECT      "obj"            /* %obj(data, width, heigth[, type[, pars]])     */
                                      /* nur <object ...>                              */
#define EOBJECT      "/obj"           /* %/obj -> </object>                            */
#define TABLE_START  "table"          /* ab hier wird eine Tabelle begonnen            */
#define TABLE_AUTOCOL "autocol"       /* bei Tabellen leere Zellen anhängen, wenn      */
                                      /* in den Zeilen voher mehr Spalten waren        */
#define TABLE_AUTOCOL_OFF "/autocol"  /* Autocol ausschalten                           */
#define TABLE_BORDER "border"         /* %table border: Tabelle mit Rahmen             */
#define TABLE_HEAD   "tablehead"      /* %tablehead ... Kopfzeile der Tabelle          */
#define TABLE_END    "/table"         /* Ende der Tabelle, mit %end wird die Tabelle   */
                                      /* automatisch beendet.                          */
#define COLSPAN      "colspan"        /* %colspan(<text>, <num>[, class[, add]])       */
#define ROWSPAN      "rowspan"        /* %rowspan(<text>, <num>[, class[, add]])       */
#define TABLE_SEP    '|'              /* Spaltentrenner für Tabellen                   */
#define TABLE_LEFT    'l'             /* Formatzeichen für linksbündig                 */
#define TABLE_CENTER  'c'             /* Formatzeichen für zentriert                   */
#define TABLE_RIGHT   'r'             /* Formatzeichen für rechtsbündig                */
#define TABLE_JUSTIFY 'j'             /* Formatzeichen für Blocksatz                   */
#define INCLUDE      "include"        /* %include <datei> es wird Datei eingefuegt     */
#define MAX_ANZ_INCLUDE 10            /* 10 verschachtelte %include erlauben           */
#define SYSTEMCMD    "system"         /* %system(<cmd>) / %system <cmd>                */
#define INPUT        "input"          /* %input Shell-Kommando                         */
#define STORE        "store"          /* %store(parameter, <Dateiname>) sichert Daten  */
#define BSTORE       "bstore"         /* %bstore(parameter, <Dateiname>) Browser Daten */
#define DELETEFILE   "delete"         /* %delete(<Dateiname>) Datei löschen            */
#define DELONEXIT    "delonexit"      /* %delonexit(<Dateiname>) Datei löschen, wenn   */
                                      /*   Ausgabe beendet ist                         */
#define READDATEI    "read"           /* %read(<Dateiname>[, <parameter>]) Datei lesen */
#define SKIP_EMPTY   "skip_empty"     /* %skip_empty: Leere Parameter werden nicht an  */
                                      /* Programme übergeben                           */
#define NOT_SKIP_EMPTY "/skip_empty"  /* %/skip_empty: alle Parameter beachten         */
#define EVAL           "eval"         /* %eval: Kommando ohne Ausgabe ausführen        */
#define LET            "let"          /* %let trägt Parameter sofort ein               */
#define LET_PLUS       "leta"         /* %leta trägt weiteren Wert zum Parameter ein   */
#define LETI           "leti"         /* %leti trägt Integer-Wert ein                  */
#define LETF           "letf"         /* %letf trägt double-Wert ein                   */
#define LETS           "lets"         /* %lets trägt String-Wert ein                   */
#define LET_PLUSI      "letai"        /* %letai trägt weiteren Integer Wert ein        */
#define LET_PLUSF      "letaf"        /* %letaf trägt weiteren Float Wert ein          */
#define LET_PLUSS      "letas"        /* %letas trägt weiteren String Wert ein         */
#define LIST           "list"         /* %list durch Komma getrennte Vars belegen      */
#define LISTA          "lista"        /* %lista trägt weitere Werte bei Vars ein       */
#define LISTI          "listi"        /* %listi Integer Werte an Vars zuweisen         */
#define LISTAI         "listai"       /* %listai trägt weitere Integer Werte ein       */
#define LISTF          "listf"        /* %listf float Werte an Vars zuweisen           */
#define LISTAF         "listaf"       /* %listaf trägt weitere Float Werte ein         */
#define LISTS          "lists"        /* %lists String Werte an Vars zuweisen          */
#define LISTAS         "listas"       /* %listas trägt weitere String Werte ein        */
#define LVARS          "lvars"        /* %lvars varliste oder %lvar(varliste ...)      */
                                      /*   Vars der varliste sind auf aktuelle         */
                                      /*   %include-Datei beschränkt                   */
                                      /* %llet* lokale Parameter belegen               */
                                      /*   übersteuert mit %lvars definierte Vars      */
#define LLET           "llet"         /* %llet trägt lokalen Parameter sofort ein      */
#define LLET_PLUS      "lleta"        /* %lleta weiterer lokalen Wert zum Parameter    */
#define LLETI          "lleti"        /* %lleti lokaler Integer-Wert                   */
#define LLETF          "lletf"        /* %lletf lokaler double-Wert                    */
#define LLETS          "llets"        /* %llets lokaler String-Wert                    */
#define LLET_PLUSI     "lletai"       /* %lletai weiterer lokalen Integer Wert         */
#define LLET_PLUSF     "lletaf"       /* %lletaf weiterer lokalen Float Wert           */
#define LLET_PLUSS     "lletas"       /* %lletas weiterer lokalen String Wert          */
#define LLIST          "llist"        /* %llist durch Komma getrennte lokale Vars      */
#define LLISTA         "llista"       /* %llista weitere lokalen Werte bei Vars        */
#define LLISTI         "llisti"       /* %llisti Integer Werte an Vars zuweisen        */
#define LLISTAI        "llistai"      /* %llistai trägt weitere Integer Werte ein      */
#define LLISTF         "llistf"       /* %llistf float Werte an Vars zuweisen          */
#define LLISTAF        "llistaf"      /* %llistaf trägt weitere Float Werte ein        */
#define LLISTS         "llists"       /* %llists String Werte an Vars zuweisen         */
#define LLISTAS        "llistas"      /* %llistas trägt weitere String Werte ein       */
#define SETQUOTE       "setquote"     /* %setquote(c, v) c wird in v gequotet          */
                                      /* %setquote(c) c wird in allen Pars gequotet    */
#define LOOP_END_CHAR  '/'            /* %/<l> beendet Schleifen (%/for, %/while, ...) */
#define LOOP_END_STR   "/"            /* LOOP_END_CHAR als String                      */
#define LOOP_STRINGS   "for while"    /* Moeglichkeiten von Schleifen (fuer break)     */
#define FOR            "for"          /* %for <name> werte ...                         */
#define FOREACH        "foreach"      /* %foreach <name> <parname>                     */
#define END_FOR     LOOP_END_STR FOR  /* %/for beendet die for oder foreach Schleife   */
#define WHILE          "while"        /* %wile <bedingung>                             */
#define END_WHILE  LOOP_END_STR WHILE /* %/while  beendet die while Schleife           */
#define BREAK          "break"        /* %break, verlassen einer for Schleife          */
#define CONTINUE       "continue"     /* %continue [for|while]                         */
#define SWITCH         "switch"       /* %switch bedingte Ausführung (c-Style)         */
#define CASE           "case"         /* %case WERT                                    */
#define DEFAULT        "default"      /* %default passt immer                          */
#define END_SWITCH LOOP_END_STR SWITCH /* %/switch - Ende des Switch                   */
#define SPLIT          "split"        /* %split(<w>, <c>[, <nv>[, <nb>]]) <w> zerlegen */
                                      /* nach Zeichen <c> Ausgabe <nv>te bis <nb>te    */
#define SUBSTR         "substr"       /* %substr(<string>, <start>[, <length>])        */
                                      /* ist <start> < 0, wird vom Ende gezählt        */
                                      /* ist <length> < 0, wird vom Ende abgeschnitten */
#define STRPOS         "strpos"       /* %strpos(<string>, <suchstr>[, <pos>])         */
#define STRCPOS        "strcpos"      /* %strcpos(<string>, <chrs>[, <pos|])           */
                                      /*   sucht ein Zeichen aus <chrs>                */
#define GETPAR         "getpar"       /* %getpar(<string>[, <pos>])                    */
                                      /*   liefert Liste: Wert,Position,Flag           */
                                      /*   Wert: <string> ab <post> bis ',' oder ')'   */
                                      /*   Position: Pos des Zeichens nach ',' oder ')'*/
                                      /*   Flag: 0 bei ')', 1 bei ','                  */
#define GETNAME        "getname"      /* %getname(<string>[, <pos>])                   */
                                      /*   liefert Liste: Name,Ende,nächstesZeichen    */
                                      /*     Name: eingelesener Name                   */
                                      /*     Ende: Position nach Name                  */
                                      /*     nächstes Zeichen: Zeichen bei Pos. Ende   */
                                      /*       oder '}' bei ${<name>}                  */
#define EXIT           "exit"         /* %exit sofort beenden                          */
#define RETURN         "return"       /* %return include-Datei verlassen               */
#define SLEEP          "sleep"        /* %sleep(<sec>)                                 */
#define USLEEP         "usleep"       /* %usleep(<mysec>)                              */
#define ISODATE        "isodate"      /* Datum ins ISO-Format wandeln                  */
                                      /* Eingabe: y-m-d oder d.m.y                     */
#define DATE           "date"         /* Ausgabe des Datums:                           */
                                      /* %date([<outformat>][,<date>])                 */
                                      /* Aktuelles Datum oder vorgegebenes Datum       */
                                      /* formatiert ausgeben                           */
#define TRIM           "trim"         /* %trim(<var>[, <cs>]) Zeichen aus <cs>/Blanks  */
                                      /*   am Anfang und Ende entfernen und ausgeben   */
#define LTRIM          "ltrim"        /* %ltrim(<var>[, <cs>]) Zeichen aus <cs>/Blanks */
                                      /*   am Anfang entfernen und ausgeben            */
#define RTRIM          "rtrim"        /* %rtrim(<var>[, <cs>]) Zeichen aus <cs>/Blanks */
                                      /*   am Ende entfernen und ausgeben              */
#define PARTRIM        "partrim"      /* %partrim(<v>[, <cs>]) Variable <v> trimmen    */
#define PARLTRIM       "parltrim"     /* %parltrim(<v>[, <cs>]) Variable <v> trimmen   */
#define PARRTRIM       "parrtrim"     /* %parrtrim(<v>[, <cs>]) Variable <v> trimmen   */
#define GPARTRIM       "gpartrim"     /* %gpartrim(<v>[, <cs>]) globale <v> trimmen    */
#define GPARLTRIM      "gparltrim"    /* %gparltrim(<v>[, <cs>]) globale <v> trimmen   */
#define GPARRTRIM      "gparrtrim"    /* %gparrtrim(<v>[, <cs>]) globale <v> trimmen   */
#define BPARTRIM       "bpartrim"     /* %bpartrim(<v>[, <cs>]) Browser <v> trimmen    */
#define BPARLTRIM      "bparltrim"    /* %bparltrim(<v>[, <cs>]) Browser <v> trimmen   */
#define BPARRTRIM      "bparrtrim"    /* %bparrtrim(<v>[, <cs>]) Browser <v> trimmen   */
#define CPARTRIM       "cpartrim"     /* %cpartrim(<v>[, <cs>]) CMD <v> trimmen        */
#define CPARLTRIM      "cparltrim"    /* %cparltrim(<v>[, <cs>]) CMD <v> trimmen       */
#define CPARRTRIM      "cparrtrim"    /* %cparrtrim(<v>[, <cs>]) CMD <v> trimmen       */
#define REPLACE        "replace"      /* %replace(<r>, <s>, <t>) alle Vorkommen von    */
                                      /* <r> durch <s> in <t> ersetzen.                */
#define REPLACEQUOTED  "replacequoted" /* %replacequoted(<s>, <q>) in <s> gequotete    */
                                      /* Zeichen durch binärwerte 0x1 bis 0xn ersetzen */
                                      /* z. B. <q> = '\$@%' -->>                       */
                                      /* \\ -> 0x1, \$ -> 0x2, \@ -> 0x3, % -> 0x4     */
#define UNREPLACE      "unreplace"    /* %unreplace(<s>, <q>)                          */
                                      /* %replacequoted rückgängig machen und          */
                                      /* Quotierung entfernen                          */
                                      /* z. B. <q> = '\$@%' -->>                       */
                                      /* 0x1 -> \, 0x2 -> $, 0x3 -> @, 0x4 -> %        */
#define COUNTCHAR      "countchar"    /* %countchar(<s>, <c>) Vorkommen von <c> in <s> */
                                      /*  zählen                                       */
#define RANDOM         "rand"         /* %rand([von],[bis]) Zufallszahl generieren     */
                                      /* wenn von und bis Integer, wird Integer,       */
                                      /* sonst Float generiert                         */
#define FLUSH          "flush"        /* %flush: chunk-Puffer senden                   */
#define TIMEOUT        "timeout"      /* %timeout <sekunden> Timeout in Sekunden setzen*/
#define BINDTEXTDOMAIN "bindtextdomain" /* %bindtextdomain(<domain>[,<domaindir>])     */
#define GETTEXT        '"'            /* %"Text" gettext("Text"), dann u2w             */
#define GETTEXTCONST   '\''           /* %'Text' nur gettext                           */
#define GETTEXTFKT     '('            /* %(Text) Text auswerten, dann gettext(Text)    */
#define GETTEXTFKTENDE ')'
#define GETTEXTFKTENDESTR ")"
#define DECIMALPOINT   "decimalpoint" /* %decimalpoint: Dezimalpunkt der locale        */

#define HLINK          "hlink"        /* %hlink(url, Text                              */
                                      /*       [, target[, class[, add[, urladd]]]])   */
                                      /* urladd wird ungequotet an url angehängt, so   */
                                      /* kann auch '&' gesendet werden                 */
#define IFRAME         "iframe"       /* %iframe(url[, name[, class[, add[, urladd]]]])*/
#define URL            "url"          /* %url(url[, ?/&-Flag]), ist ?/&-Flag gesetzt,  */
                                      /*   dann werden auch ? und & konvertiert        */

/***************************************************************************************/
/* Formularseiten                                                                      */
/***************************************************************************************/
#define PAR_TEXT         "text"      /* %text erzeugt Textfeld                         */
#define PAR_PWD          "password"  /* %password erzeugt Textfeld fuer Passwoerter    */
#define PAR_TEXT_SIZE    16          /* Groesse des Text und PWD-Feldes                */
#define PAR_CHECKBOX     "checkbox"  /* %checkbox, Checkbox                            */
#define PAR_CHECK        "check"     /* %check, mehrere Checkboxen                     */
#define PAR_CHECKID      "checkid"   /* %checkid Checkbox mit Value "ID"               */
#define CHECKPARS_TRENN_CHAR ':'     /* Trennzeichen für Checkboxen über %check        */
#define CHECKPARS_TRENN_CHAR_S ":"   /* Trennzeichen für Checkboxen über %check        */
#define PAR_OPTION       "option"    /* %option, Optionsfeld                           */
#define GET_CHECKS       "checkvars" /* %checkvars(<checkname>) -> markierte Boxen     */
#define PAR_RADIO_BUTTON "radiobutton"  /* %radio, Radiobuttons                        */
#define PAR_RADIO        "radio"     /* %radio, Radiobuttons                           */
#define PAR_SELECT       "select"    /* Select Feld fuer Mehrfachauswahl               */
#define PAR_COLOR        "pcolor"    /* %pcolor, Farbe                                 */
#define PAR_DATE         "pdate"     /* %pdate, Datum                                  */
#define PAR_DATETIMETZ   "pdatetimetz" /* %pdatetimetz Date Time with timezone         */
#define PAR_DATETIME     "pdatetime" /* %pdatetime Date Time without timezone          */
#define PAR_EMAIL        "pemail"    /* %pemail E-Mail-Address                         */
#define PAR_MONTH        "pmonth"    /* %pmonth select month and year                  */
#define PAR_NUMBER       "pnumber"   /* %pnumber select number                         */
#define PAR_RANGE        "prange"    /* %prange select range                           */
#define PAR_SEARCH       "psearch"   /* %psearch search field                          */
#define PAR_TEL          "ptel"      /* %ptel telephone number                         */
#define PAR_TIME         "ptime"     /* %ptime time without timezone                   */
#define PAR_URL          "purl"      /* %purl url                                      */
#define PAR_WEEK         "pweek"     /* %pweek week and year                           */

#define PAR_TEXTAREA     "textarea"  /* %textarea, Eingabefeld mit mehreren Zeilen     */
#define PAR_TEXTAREA_COLS  50        /* 50 Zeichen breit                               */
#define PAR_TEXTAREA_ROWS   4        /* vier Zeilen                                    */

#define VAR              "var"       /* %var: hidden Textfeld zur Parameteruebergabe   */
#define VARIF            "varif"     /* %varif: hidden Textfeld, wenn Wert gesetzt     */
#define HIDDENVARS       "hiddenvars"/* %hiddenvars('<v1> ...<vn>'[, <sep>[, <all>]])  */
#define QUERYVARS        "queryvars" /* %queryvars('<v1> ...<vn>'[, <sep>[, <all>]])   */
                                     /* string mit var1=$var1&var2=$var2... aufbauen   */
                                     /* <flag> nicht gesetzt, nur belegte <vars>       */

#define PAR_FILE         "file"      /* %file, Dateiname auswaehlen                    */
#define ERRORNAME        "_error"    /* Dateiname um _error erweitert ist Fehlermeld.  */
                                     /* Variable beim Speichern von Dateien            */
#define CLIENTFILENAME   "_file"     /* Dateiname des Clients bei multipart POST       */
#define CLIENTPATHNAME   "_path"     /* Pfad der Datei bei multipart POST              */
#define STOREPATHNAME    "_store"    /* Speicherort der Datei                          */
#define CLIENTMIME       "_mime"     /* Mime-Type der Datei                            */

#define PAR_LINKLIST     "linklist"  /* %linklist für eine Liste von Links             */
#define PAR_BUTTON       "button"    /* %button[:target] ref; Text                     */
#define PAR_PARBUTTON    "parbutton" /* %parbutton ref; Text      (altlast)            */
#define PAR_PBUTTON      "pbutton"   /* %pbutton name; Value -> Button mit name und W. */
#define PAR_OKBUTTON     "okbutton"  /* %okbutton [Value][; Text]                      */
#define VAR_OK           "ok"        /* %ok -> Wert des OK-Buttons                     */
#define VAR_CONTENTTYPE  "contenttype" /* %contenttype -> String content-type          */
#define VAR_CONTENT      "content"   /* %content -> Name der Datei mit dem content     */
#define PAR_BACKBUTTON   "backbutton"  /* %backbutton erzeugt Button "Zurück"          */
#define PAR_HAUPTMENUE   "hauptmenu" /* %hauptmenu erzeugt Button Hauptmenü            */
#define PAR_MAINMENU     "mainmenu"  /* %mainmenu erzeugt Button Main menu             */
#define PAR_CLOSEBUTTON  "closebutton" /* %closebutton -> Button "Schließen"/"Close"   */
#define DATALIST         "datalist"  /* %datalist <ID> "<val1>" "<val2>" ...           */

#define NO_NEWLINE       "nonl"      /* %nonl, Parameterfelder werden nicht durch      */
                                     /* Zeilenumbrüche beendet                         */
#define NOT_NO_NEWLINE   "/nonl"     /* %/nonl, Parameterfelder werden durch           */
                                     /* Zeilenumbrüche beendet (default)               */

#define READONLY         'ro'        /* %feld:::ro erzeugt readonly Feld               */
 
#define PAR_PAR_CHAR     ':'         /* Trennzeichen vor Parametern z. B.:             */
                                     /* %linklist:2[::ro] oder %textarea:60:8[:ro]     */
                                     /* Allgemein: %feld[:<breite>[:<hoehe>[:ro]]]     */

#define FKT_PAR_CHAR     ','         /* Trennzeichen in Funktionen: %mysql(p1,p2..)    */


/***************************************************************************************/
/* Menueseiten                                                                         */
/***************************************************************************************/
#define SYSVARSTART  "u645342"        /* Systemvariablen beginnen damit                */
#define SYSVARSTARTLEN 7              /* Länge von SYSVARSTART                         */
#define MENU         "menu"           /* %menu leitet eine Menueseite ein              */
#define OKBUTTONNAME SYSVARSTART "ok" /* Name des OK-Buttons                           */
#define MENUNAME     SYSVARSTART "me" /* Name der Steuervariable fürs Menüsystem       */
#define FORMMAGIC    SYSVARSTART "fm" /* FORMMAGIC ist 1, wenn Formular ausgefüllt     */
#define MENUON       "on847629"       /* Wert von MENUNAME, wenn im linken Frame       */
#define MENUSUBFILE  "sf947629"       /* Wert von MENUNAME, wenn in Subfile            */
#define MENUTOP      "top47629"       /* Wert von MENUNAME, wenn im oberen Frame       */
#define MENURECHTS   "mr847629"       /* Wert von MENUNAME, wenn im rechten Frame      */
#define MENUSHELL    "ms823528"       /* Wert, von MENUNAME, wenn %shell-Aufruf        */
#define RECHTSMENU   SYSVARSTART "rm" /* Var, wenn rechtes Frame vorgegeben            */
#define LINKSMENU    SYSVARSTART "li" /* Var, wenn linkes Frame vorgegeben             */
#define LINKSMENUSIZE SYSVARSTART "lg"/* Var, wenn Größe linker Frame vorgegeben       */
#define OBENMENU     SYSVARSTART "om" /* Var, wenn oberes Frame vorgegeben             */
#define OBENMENUSIZE SYSVARSTART "og" /* Var, wenn Größe für oberen Frame              */
#define MENUHEAD     "menuhead"       /* %menuhead, Anfang Kopfseite                   */
#define SUBMENU      "sub"            /* Untermenue                                    */
#define SUBNAME      "subname"        /* %subname enthält Name des Untermenüs          */
#define SUBMENUPAR   SYSVARSTART "sb" /* Parameter für Untermenüs                      */
#define SUBFILE      "subfile"        /* Untermenue aus Datei                          */
#define SHELLMENU    "shell"          /* direkter Shellaufruf                          */
#define EXITMENU     "exitmenu"       /* Menueseite verlassen                          */
#define EXITTARGET   "_parent"
#define MENULINK     "link"           /* Hyperlink mit Target mright                   */
#define NEWWIND      "newwind"        /* Neues Fenster oeffnen                         */
#define NEWTARGET    "_blank"
#define NEWMENU      "newmenu"        /* gesamtes Fenster benutzen                     */
#define TOPTARGET    "_top"
#define HAUPTMENUPAR SYSVARSTART "hm" /* Variable um Hauptmenu zu finden               */
#define LASTMENUPAR  SYSVARSTART "lm" /* Variable des letzten Submenues                */
#define MENUSUBTRENN '.'              /* trennt die einzelnen Submenuebenen            */
#define MENU_TRENN ','                /* Hier endet Submenuname                        */
#define MENU_HEAD_TRENN ';'           /* ab ; alternative Ueberschrift                 */



/***************************************************************************************/
/* Frames                                                                              */
/***************************************************************************************/
#define FRAME        "frame"          /* %frame:rows:cols:def1:def2                    */
#define FRAMENAME    SYSVARSTART "fr" /* Name der Steuervariable fürs Menüsystem       */
#define FRAME1       "frame4729"      /* Wert von FRAMENAME, wenn im ersten Frame      */
#define FRAME2       "frame4829"      /* Wert von FRAMENAME, wenn im zweiten Frame     */
#define FRAME_END    "/frame"         /* %/frame, ab hier zweite Frame                 */


#ifdef MAYDBCLIENT
/***************************************************************************************/
/* MySQL Befehle                                                                       */
/***************************************************************************************/
#define MYSQLPORT      "mysqlport"      /* MySQL-Port ändern Std.: 3306                */
#define MYSQLCONNECT   "mysqlconnect"   /* %mysqlconnect(server,user,pwd,db)           */
#define MYSQLQUERY     "mysqlquery"     /* %mysqlquery(query)                          */
#define MYSQLWRITE     "mysqlwrite"     /* %mysqlwrite(query) (für update und insert)  */
#define MYSQLGETLINE   "mysqlgetline"   /* %mysqlgetline                               */
#define MYSQLREADWRITELINE "mysqlreadline" /* %mysqlreadline([cn],[ssep])              */
#define MYSQLWRITELINE "mysqlline"      /* %mysqlline([cn],[ssep])                     */
#define MYSQLNUMFIELDS "mysqlnumfields" /* %mysqlnumfields                             */
#define MYSQLVALUE     "mysqlvalue"     /* %mysqlvalue(0..n)                           */
#define MYSQLRAWVALUE  "mysqlrawvalue"  /* %mysqlrawvalue(0..n)                        */
#define MYSQLISVALUE   "mysqlisvalue"   /* %mysqlisvalue                               */
#define MYSQLREADVALUE "mysqlreadvalue" /* %mysqlreadvalue                             */
#define MYSQLID        "mysqlid"        /* %mysqlid                                    */
#define MYSQLNUMROWS   "mysqlrows"      /* %mysqlrows                                  */
#define MYSQLREAD      "mysqlread"      /* %mysqlread(query[, ssep[, zsep]])           */
#define MYSQLREADRAW   "mysqlreadraw"   /* %mysqlreadraw(query[, ssep[, zsep]])        */
#define MYSQLWERT      "mysqlwert"      /* %mysqlwert(tabelle, spalte, id)             */
#define MYSQLIDVALUE   "mysqlidvalue"   /* %mysqlidvalue(tabelle, spalte, id)          */
#define MYSQLINS       "mysqlins"       /* %mysqlins(tabelle, "f1='w1', f2='w2'...")   */
#define MYSQLENUMS     "mysqlenums"     /* %mysqlenums(tabelle, spalte)                */
#define MYSQLSETS      "mysqlsets"      /* %mysqlsets(tabelle, spalte)                 */
#define MYSQLSTORE     "mysqlstore"     /* %mysqlstore(datei, tabelle, spalte, id)     */
#define MYSQLSTOREINS  "mysqlstoreins"  /* %mysqlstoreins(datei, tabelle, spalte)      */
#define MYSQLSTOREV    "mysqlstorev"    /* %mysqlstorev(datei, varname)                */
#define MYSQLTEST      "mysqltest"      /* %mysqltest(query) liefert nur Errorcode     */
#define MYSQLOUT       "mysqlout"       /* %mysqlout(query, datei[, ssep[,zsep]])      */
                                        /*   Ergebnis in Datei schreiben               */

#endif  /* MAYDBCLIENT */

#ifdef MAXDBCLIENT
/***************************************************************************************/
/* MAXDB Befehle                                                                       */
/***************************************************************************************/
#define MAXDBPORT      "maxdbport"      /* MAXDB-Port ändern Std.: 7210                */
#define MAXDBCONNECT   "maxdbconnect"   /* %maxdbconnect(server,user,pwd,db)           */
#define MAXDBQUERY     "maxdbquery"     /* %maxdbquery(query)                          */
#define MAXDBWRITE     "maxdbwrite"     /* %maxdbwrite(query) (für update und insert)  */
#define MAXDBGETLINE   "maxdbgetline"   /* %maxdbgetline                               */
#define MAXDBREADWRITELINE "maxdbreadline" /* %maxdbreadline([cn],[ssep])              */
#define MAXDBWRITELINE "maxdbline"      /* %maxdbline([cn],[ssep])                     */
#define MAXDBNUMFIELDS "maxdbnumfields" /* %maxdbnumfields                             */
#define MAXDBVALUE     "maxdbvalue"     /* %maxdbvalue(0..n)                           */
#define MAXDBISVALUE   "maxdbisvalue"   /* %maxdbisvalue                               */
#define MAXDBREADVALUE "maxdbreadvalue" /* %maxdbreadvalue                             */
#define MAXDBID        "maxdbid"        /* %maxdbid                                    */
#define MAXDBNUMROWS   "maxdbrows"      /* %maxdbrows                                  */
#define MAXDBREAD      "maxdbread"      /* %maxdbread(query[, ssep[, zsep]])           */
#define MAXDBIDVALUE   "maxdbidvalue"   /* %maxdbidvalue(tabelle, spalte, id)          */
#define MAXDBINS       "maxdbins"       /* %maxdbins(tabelle, "f1='w1', f2='w2'...")   */
#define MAXDBENUMS     "maxdbenums"     /* %maxdbenums(tabelle, spalte)                */
#define MAXDBSETS      "maxdbsets"      /* %maxdbsets(tabelle, spalte)                 */
#define MAXDBSTORE     "maxdbstore"     /* %maxdbstore(datei, tabelle, spalte, id)     */
#define MAXDBSTOREINS  "maxdbstoreins"  /* %maxdbstoreins(datei, tabelle, spalte)      */
#define MAXDBTEST      "maxdbtest"      /* %maxdbtest(query) liefert nur Errorcode     */
#define MAXDBOUT       "maxdbout"       /* %maxdbout(query, datei[, ssep[,zsep]])      */
                                        /*   Ergebnis in Datei schreiben               */

#endif  /* MAXDBCLIENT */

#ifdef POSTGRESQLCLIENT
/***************************************************************************************/
/* PostgreSQL Befehle                                                                  */
/***************************************************************************************/
#define PGSQLPORT      "pgsqlport"      /* PGSQL-Port ändern Std.: 5432                */
#define PGSQLCONNECT   "pgsqlconnect"   /* %pgsqlconnect(server,user,pwd,db)           */
#define PGSQLQUERY     "pgsqlquery"     /* %pgsqlquery(query)                          */
#define PGSQLWRITE     "pgsqlwrite"     /* %pgsqlwrite(query) (für update und insert)  */
#define PGSQLGETLINE   "pgsqlgetline"   /* %pgsqlgetline                               */
#define PGSQLREADWRITELINE "pgsqlreadline" /* %pgsqlreadline([cn],[ssep])              */
#define PGSQLWRITELINE "pgsqlline"      /* %pgsqlline([cn],[ssep])                     */
#define PGSQLNUMFIELDS "pgsqlnumfields" /* %pgsqlnumfields                             */
#define PGSQLVALUE     "pgsqlvalue"     /* %pgsqlvalue(0..n)                           */
#define PGSQLISVALUE   "pgsqlisvalue"   /* %pgsqlisvalue                               */
#define PGSQLREADVALUE "pgsqlreadvalue" /* %pgsqlreadvalue                             */
#define PGSQLID        "pgsqlid"        /* %pgsqlid                                    */
#define PGSQLNUMROWS   "pgsqlrows"      /* %pgsqlrows                                  */
#define PGSQLREAD      "pgsqlread"      /* %pgsqlread(query[, ssep[, zsep]])           */
#define PGSQLIDVALUE   "pgsqlidvalue"   /* %pgsqlidvalue(tabelle, spalte, id)          */
#define PGSQLINS       "pgsqlins"       /* %pgsqlins(tabelle, "f1='w1', f2='w2'...")   */
#define PGSQLSTORE     "pgsqlstore"     /* %pgsqlstore(datei, tabelle, spalte, id)     */
#define PGSQLTEST      "pgsqltest"      /* %pgsqltest(query) liefert nur Errorcode     */
#define PGSQLOUT       "pgsqlout"       /* %pgsqlout(query, datei[, ssep[,zsep]])      */
                                        /*   Ergebnis in Datei schreiben               */

#endif  /* POSTGRESQLCLIENT */

#ifdef SQLITE3
/***************************************************************************************/
/* SQLITE3 Befehle                                                                     */
/***************************************************************************************/
#define SQL3OPEN      "sqliteopen"      /* %sqliteopen(dbpath)                         */
#define SQL3QUERY     "sqlitequery"     /* %sqlitequery(query)                         */
#define SQL3WRITE     "sqlitewrite"     /* %sqlitewrite(query) (für update und insert) */
#define SQL3GETLINE   "sqlitegetline"   /* %sqlitegetline                              */
#define SQL3READWRITELINE "sqlitereadline" /* %sqlitereadline([cn],[ssep])             */
#define SQL3WRITELINE "sqliteline"      /* %sqliteline([cn],[ssep])                    */
#define SQL3NUMFIELDS "sqlitenumfields" /* %sqlitenumfields                            */
#define SQL3VALUE     "sqlitevalue"     /* %sqlitevalue(0..n)                          */
#define SQL3RAWVALUE  "sqliterawvalue"  /* %sqliterawvalue(0..n)                       */
#define SQL3ISVALUE   "sqliteisvalue"   /* %sqliteisvalue                              */
#define SQL3READVALUE "sqlitereadvalue" /* %sqlitereadvalue                            */
#define SQL3ID        "sqliteid"        /* %sqliteid                                   */
#define SQL3CHANGES   "sqlitechanges"   /* %sqlitechanges                              */
#define SQL3READ      "sqliteread"      /* %sqliteread(query[, ssep[, zsep]])          */
#define SQL3READRAW   "sqlitereadraw"   /* %sqlitereadraw(query[, ssep[, zsep]])       */
#define SQL3IDVALUE   "sqliteidvalue"   /* %sqliteidvalue(tabelle, spalte, id)         */
#define SQL3INS       "sqliteins"       /* %sqliteins(tabelle, "f1='w1', f2='w2'...")  */
#define SQL3STORE     "sqlitestore"     /* %sqlitestore(datei, tabelle, spalte, id)    */
#define SQL3STOREINS  "sqlitestoreins"  /* %sqlitestoreins(datei, tabelle, spalte)     */
#define SQL3STOREV    "sqlitestorev"    /* %sqlitestorev(datei, varname)               */
#define SQL3TEST      "sqlitetest"      /* %sqlitetest(query) liefert nur Errorcode    */
#define SQL3OUT       "sqliteout"       /* %sqliteout(query, datei[, ssep[,zsep]])     */
                                        /*   Ergebnis in Datei schreiben               */

#endif  /* SQLITE3 */

#ifdef DBCLIENT
/***************************************************************************************/
/* DatabaseSQL Befehle                                                                 */
/***************************************************************************************/
#define SQLCONNECT     "sqlconnect"     /* %let db=%sqlconnect(DB, server,user,pwd,db) */
                                        /*                     DB: MySQL, PGSQL, ...   */
#define SQLCLOSE       "sqlclose"       /* %sqlclose(db)                               */
#define SQLQUERY       "sqlquery"       /* %let res=%sqlquery(db, query)               */
#define SQLWRITE       "sqlwrite"       /* %sqlwrite(db, query) (für update und insert)*/
#define SQLGETLINE     "sqlgetline"     /* %sqlgetline(res)                            */
#define SQLREADWRITELINE "sqlreadline"  /* %sqlreadline(res, [ssep])                   */
#define SQLWRITELINE   "sqlline"        /* %sqlline(res, [ssep])                       */
#define SQLNUMFIELDS   "sqlnumfields"   /* %sqlnumfields(res)                          */
#define SQLVALUE       "sqlvalue"       /* %sqlvalue(res, 0..n)                        */
#define SQLISVALUE     "sqlisvalue"     /* %sqlisvalue(res)                            */
#define SQLREADVALUE   "sqlreadvalue"   /* %sqlreadvalue(res)                          */
#define SQLID          "sqlid"          /* %sqlid(db)                                  */
#define SQLNUMROWS     "sqlrows"        /* %sqlrows(res)                               */
#define SQLREAD        "sqlread"        /* %sqlread(db, query[, ssep[, zsep]])         */
#define SQLWERT        "sqlwert"        /* %sqlwert(db, tabelle, spalte, id)           */
#define SQLINS         "sqlins"         /* %sqlins(db, tabelle, "f1='w1', f2='w2'...") */
#define SQLENUMS       "sqlenums"       /* %sqlenums(db, tabelle, spalte)              */
#define SQLSETS        "sqlsets"        /* %sqlsets(db, tabelle, spalte)               */
#define SQLSTORE       "sqlstore"       /* %sqlstore(db, datei, tabelle, spalte, id)   */
#define SQLSTOREINS    "sqlstoreins"    /* %sqlstoreins(db, datei, tabelle, spalte)    */
#define SQLTEST        "sqltest"        /* %sqltest(db, query) liefert nur Errorcode   */
#define SQLOUT         "sqlout"         /* %sqlout(db, query, datei[, ssep[,zsep]])    */
                                        /*   Ergebnis in Datei schreiben               */

#endif  /* DBCLIENT */

/***************************************************************************************/
/* u2w, /u2w, u4w und /u4w funktionieren noch nicht, weil der Scanner abbricht         */
/* entfällt eigentlich, da %set('u2w'), ... moeglich ist.                              */
#define U2W_ON         "u2w"          /* u2w ab hier u2w Mode                          */
#define U2W_OFF        "%/u2w"        /* u2w ab hier kein u2w Mode                     */
#define U4W_ON         "%u4w"         /* u2w ein                                       */
#define U4W_OFF        "/u4w"         /* u4w aus                                       */
/***************************************************************************************/

#define HTML_FKT       "html"         /* %html(Text) Text direkt senden, es werden nur */
                                      /* Funktionen ausgewertet, keine Sonderzeichen   */
#define HTML_ON        "html"         /* %html ab hier steht HTML-Code                 */
#define HTML_OFF       "/html"        /* %/html nun wieder x2w-Code                    */
#define HTML_HEAD_ON   "head"         /* %head ab hier Code für den <head>             */
#define HTML_HEADS_ON  "heads"        /* %heads ab hier Code für den <head> (s2w-Mode) */
#define HTML_HEAD_OFF  "/head"        /* %/head Ende <head>                            */
#define HTML_HEADS_OFF "/heads"       /* %/heads Ende <head>                           */
#define HTTP_HEAD_ON   "httphead"     /* %httphead ab hier Code für den HTTP-Header    */
#define HTTP_HEADS_ON  "httpheads"    /* %httpheads ab hier HTTP-Header im (s2w-Mode)  */
#define HTTP_HEAD_OFF  "/httphead"    /* %/httphead Ende HTTP-Header                   */
#define HTTP_HEADS_OFF "/httpheads"   /* %/httpheads Ende HTTP-Header                  */
#define PRE_ON         "pre"          /* %pre -> <pre> und s2w mode                    */
#define PRE_OFF        "/pre"         /* Ende <pre> und s2w mode                       */

#define CSSLINK        "csslink"      /* %csslink <url>                                */
#define CSS            "css"          /* %css(<element>, <class>)                      */
                                      /* <element>: link, table, tablehead, par, body, */
                                      /*            text, textarea, h1, h2, h3, h4,    */
                                      /*            password, checkbox, option, radio, */
                                      /*            select, file, linklist, button,    */
#define FILEOPEN       "open"         /* Datei öffnen                                  */
#define FILEAPPEND     "append"       /* Datei zum Anhängen öffnen                     */
#define FILECREATE     "create"       /* Neue Datei anlegen, alte Datei löschen        */
#define READLINE       "readline"     /* %readline([<Dateipointer>]) aus Datei oder    */
                                      /* stdin lesen                                   */
#define PRINT          "print"        /* %print(Text[, <Dateiptr>]) Datei schreiben    */
#define WRITE          "write"        /* %write(parname[, <Dateiptr>]) Par schreiben   */
#define CLOSE          "close"        /* %close(<Dateipointer>) Datei schliessen       */
#define RENAME         "rename"       /* %rename(<Dateivon>, <Dateinach>) umbenennen   */
#define FILESIZE       "filesize"     /* %filesize(<Dateiname>) größe der Datei        */
#define FILE_EOF       "eof"          /* %eof(<Dateipointer>) auf EOF testen           */
#define ERROROUT       "error"        /* %error(Text) auf stderr schreiben             */
#define LOGGING        "log"          /* %log(Text) in Loggingdatei schreiben          */
#define DIRLIST        "dirlist"      /* %dirlist(<dir>[,<pat>[,f|d]]) Directory liste */
                                      /* <dir> nach <pat> suchen f: Files, d: dirs     */
                                      /* %dirlist("/tmp", "uls.*", "f")                */


/***************************************************************************************/
/* Variablennamen, die mit %set gesetzt werden                                         */
/***************************************************************************************/
#define SET                  "set"               /* %set('varname', 'wert') Systemvars */
                                                 /* setzen. Möglich sind folgende:     */
#define S2W_NEWLINE          "newline"           /* Zeilenwechsel                      */
#define S2W_NEWPAGE          "newpage"           /* Seitenwechsel                      */
#define S2W_PAGE_HEADER      "page_header"       /* Seitenkopf                         */
#define S2W_PAGE_FOOTER      "page_footer"       /* Seitenfuss                         */
#define S2W_IMAGE            "image"             /* Bild einfügen, sollte %s enthalten */
#define S2W_OBJECT           "object"            /* Objekt einfügen, sollte %s enth.   */
#define S2W_BACKGROUND       "background"        /* Bild als Hintergrund, %s!          */
#define S2W_TABLE_START      "table_start"       /* Tabellenstart                      */
#define S2W_TABLE_END        "table_end"         /* Tabellenende                       */
#define S2W_TABLE_ROW_START  "table_row_start"   /* Start Tabellenzeile                */
#define S2W_TABLE_ROW_END    "table_row_end"     /* Ende Tabellenzeile                 */
#define S2W_TABLE_CELL_START "table_cell_start"  /* Start Tabellenzelle                */
#define S2W_TABLE_CELL_END   "table_cell_end"    /* Ende Tabellenzelle                 */
#define S2W_HYPERLINK        "hyperlink"         /* 1. %s Text, 2. %s Linkziel???      */
#define S2W_BOLD_ON          "bold_on"           /* Fett an                            */
#define S2W_BOLD_OFF         "bold_off"          /* Fett aus                           */
#define S2W_UNDERLINE_ON     "underline_on"      /* Unterstreichen an                  */
#define S2W_UNDERLINE_OFF    "underline_off"     /* Unterstreichen aus                 */
#define S2W_FIXED_FONT_ON    "fixed_font_on"     /* Fixed Font an                      */
#define S2W_FIXED_FONT_OFF   "fixed_font_off"    /* Fixed Font aus                     */
#define S2W_FONT_SIZE        "font_size"         /* Font Size setzen                   */
                                                 /*  %set('font_size', '\\size{%d}')   */

#define SET_U2WMODE          "u2w"               /* %set('u2w') in U2W-Mode wechseln   */
#define SET_U3WMODE          "u3w"               /* %set('u3w') in U3W-Mode wechseln   */
#define SET_U4WMODE          "u4w"               /* %set('u4w') in U4W-Mode wechseln   */
#define SET_U5WMODE          "u5w"               /* %set('u5w') in U5W-Mode wechseln   */
#define SET_S2WMODE          "s2w"               /* %set('s2w') in S2W-Mode wechseln   */
#define SET_S3WMODE          "s3w"               /* %set('s3w') in S3W-Mode wechseln   */
#define SET_S4WMODE          "s4w"               /* %set('s4w') in S4W-Mode wechseln   */
#define SET_S5WMODE          "s5w"               /* %set('s5w') in S5W-Mode wechseln   */
