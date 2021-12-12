/**********************************************/
/* Headerdatei fuer unix2webd mit Tokentablle */
/* timestamp: 2016-07-24 10:23:39             */
/**********************************************/

/***************************************************************************************/
/* TOKENTABELLE */

#define T_COMMAND_PROGS           1
#define T_COMMAND_LIST_PROGS      2
#define T_U2W_COMMAND_PROGS       4
#define T_SCAN_PROGS              8
#define T_SCAN_LIST_PROGS        16
#define T_SCAN_SEND_PROGS        32
#define T_SCAN_SEND_PROGS_OUT    64
#define T_SCAN_ONLY_SEND_PROGS  128
#define T_U2W_PUT_PRE_HEADER    256
#define T_U2W_PUT_HEADER        512
#define T_U2W_PUT_HTTP_HEADER  1024
#define T_U2W_PUT              2048
#define T_FORMAT               4096
#define T_CALC                 8192
#define T_ATOM                16384
#define T_U2W_TABLE_PROGS     32568

#define T_U2W_PUT_HEADER_S_ELEMENTS 4  /* die ersten 4 Elemente in T_U2W_PUT_HEADER    */
                                       /* muessen auch bei s?w ausgewertet werden,     */
                                       /* damit %head und %heads uebersprungen werden  */
#define T_U2W_COMMAND_PROGS_IGNORE_BLANKS 4  /* bei den ersten 4 Elementen in          */
                                             /* T_U2W_COMMAND_PROGS werden Leerzeichen */
                                             /* am Zeilenanfang ingoriert              */

/* u2w_command_progs */

#define T_VAR               '0'
#define T_VARIF             '1'
#define T_HIDDENVARS        '2'
#define T_DATALIST          '3'
#define T_PAR_CHECK         'a'
#define T_PAR_PBUTTON       'b'
#define T_PAR_OKBUTTON      'c'
#define T_PAR_RADIO         'd'
#define T_PAR_CLOSEBUTTON   'e'
#define T_PAR_TEXTAREA      'f'
#define T_PAR_BUTTON        'g'
#define T_PAR_CHECKBOX      'h'
#define T_PAR_PARBUTTON     'i'
#define T_PAR_FILE          'j'
#define T_PAR_MAINMENU      'k'
#define T_PAR_LINKLIST      'l'
#define T_PAR_OPTION        'm'
#define T_PAR_PWD           'n'
#define T_PAR_RADIO_BUTTON  'o'
#define T_PAR_SELECT        'p'
#define T_PAR_TEXT          'q'
#define T_PAR_BACKBUTTON    's'
#define T_PAR_COLOR         't'
#define T_PAR_DATE          'u'
#define T_PAR_DATETIMETZ    'v'
#define T_PAR_DATETIME      'w'
#define T_PAR_EMAIL         'x'
#define T_PAR_MONTH         'y'
#define T_PAR_NUMBER        'z'
#define T_PAR_RANGE         'A'
#define T_PAR_SEARCH        'B'
#define T_PAR_TEL           'C'
#define T_PAR_TIME          'D'
#define T_PAR_URL           'E'
#define T_PAR_WEEK          'F'
#define T_PAR_CHECKID       'H'


/* calc */

#define T_PLUS                '+'
#define T_MINUS               '-'
#define T_MATCH               '~'
#define T_STERN               '*'
#define T_GETEILT             '/'
#define T_PROZENT             '%'
#define T_GLEICH              '='
#define T_UNGELICH            '!'
#define T_KLEINER             '<'
#define T_GROESSER            '>'
#define T_UND                 '&'
#define T_ODER                '|'
#define T_FRAGEZEICHEN        '?'
#define T_DOPPELPUNKT         ':'
#define T_KLAMMERAUF          '('
#define T_KLAMMERZU           ')'
#define T_UNDUND              'a'
#define T_NEG                 'b'
#define T_KONKAT              'c'
#define T_UMINUS              'd'
#define T_GROESSERGLEICH      'g'
#define T_KLEINERGLEICHABS    'h'
#define T_KLEINERABS          'i'
#define T_GROESSERGLEICHABS   'j'
#define T_GROESSERABS         'k'
#define T_KLEINERGLEICH       'l'
#define T_MATCHICASE          'm'
#define T_NOT                 'n'
#define T_ODERODER            'o'
#define T_SUBSTRING           's'
#define T_SUBSTRINGICASE      't'
#define T_GLEICHGLEICH        'G'
#define T_GLEICHGLEICHICASE   'H'
#define T_GLEICHABS           'I'
#define T_UNGLEICHABS         'J'
#define T_UNGLEICHGLEICHICASE 'K'
#define T_LINKSSHIFT          'L'
#define T_NOTMATCH            'M'
#define T_NOTMATCHICASE       'N'
#define T_RECHTSSHIFT         'R'
#define T_NOTSUBSTRING        'S'
#define T_NOTSUBSTRINGICASE   'T'
#define T_UNGLEICHGLEICH      'U'
