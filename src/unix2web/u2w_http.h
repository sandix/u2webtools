/*********************************************/
/* File: u2w_http.h                          */
/* HTTP Makros                               */
/* timestamp: 2017-02-12 14:49:25            */
/*********************************************/


/***************************************************************************************/
/* HTTP Texte                                                                          */
/***************************************************************************************/
#define CR           '\xd'
#define LF           '\xa'
#define CRLF         "\xd\xa"

#define NOT_MODIFIED "HTTP/1.1 304 Not Modified" CRLF "Date: %s" CRLF "Server: unix2web" CRLF "Connection: %s" CRLF CRLF

#define AUTHORIZE_SEITE_NUM  "401"
#define AUTHORIZE_SEITE_DESC "Authorization Required"
#define AUTHORIZE_SEITE_HTTP "WWW-Authenticate: Basic realm=\"%s\"" CRLF
#define STD_REALM            "WWW Syko"
#define AUTHORIZE_DIGEST_HTTP "WWW-Authenticate: Digest realm=\"%s\", nonce=\"%s\"" CRLF
#define AUTHORIZE_DIGEST_STALE_HTTP "WWW-Authenticate: Digest realm=\"%s\", nonce=\"%s\", stale=\"true\"" CRLF
#define AUTHORIZE_SEITE_TEXT "You must be authenticated to use this service"

#define MAX_LEN_DIGEST_NONCE 60

#define BAD_AUTHORIZE_NUM  "410"
#define BAD_AUTHORIZE_DESC "Bad Authorization"
#define BAD_AUTHORIZE_TEXT _("Yout must be authenticated to use this service!")

#define SEITE_NICHT_GEFUNDEN_NUM  "404"
#define SEITE_NICHT_GEFUNDEN_DESC "Page not Found"
#define SEITE_NICHT_GEFUNDEN_TEXT _("Page not found.")

#define PAGE_MOVED_PERMANENTLY_NUM  "301"
#define PAGE_MOVED_PERMANENTLY_DESC "Moved Permanently"
#define PAGE_MOVED_PERMANENTLY_LOC  "Location: %s" CRLF
#define PAGE_MOVED_PERMANENTLY_TXT  "The document has moved: <a href=\"%s\">here</A>"
#define PAGE_MOVED_PERMANENTLY_HTTP "Location: http://%s%s%s" CRLF
#define PAGE_MOVED_PERMANENTLY_HTTP_TEXT "The document has moved: <a href=\"http://%s%s%s\">here</A>"
#define PAGE_MOVED_PERMANENTLY_HTTPS "Location: https://%s%s%s" CRLF
#define PAGE_MOVED_PERMANENTLY_HTTPS_TEXT "The document has moved: <a href=\"https://%s%s%s\">here</A>"

#define METHOD_NOT_ALLOWED_NUM  "405"
#define METHOD_NOT_ALLOWED_DESC "Method not allowed"
#define METHOD_NOT_ALLOWED_TEXT _("method not allowed.")

#define FORBIDDEN_NUM  "403"
#define FORBIDDEN_DESC "Forbidden"
#define FORBIDDEN_TEXT _("Acces of page is not allowed.")

#define REQUEST_TOO_LARGE_NUM "413"
#define REQUEST_TOO_LARGE_DESC "Request Entity Too Large"
#define REQUEST_TOO_LARGE_TEXT _("Request Entity Too Large")

#define CREATED_NUM  "201"
#define CREATED_DESC "Created"
#define CREATED_TEXT _("File was created.")

#define CONFLICT_NUM  "409"
#define CONFLICT_DESC "Conflict"
#define CONFLICT_TEXT _("Unable to write file. Probably file system fulll.")

#define ERROR_HTTP_HEADER "HTTP/1.1 %s %s" CRLF "Server: unix2web %s" CRLF "Connection: %s" CRLF "Content-Length: %d" CRLF "%s" CRLF "%s"

#define OPTIONS_RETURN "HTTP/1.1 200 OK" CRLF "Date: %s" CRLF "Server: unix2web" CRLF "Connection: %s" CRLF "Content-Length: 0" CRLF "Allow: %sOPTIONS%s%s%s" CRLF CRLF

#define DOCTYPE "<!DOCTYPE html>\n"
#define PAGE_HEADER DOCTYPE "<html><head><style>table.u2wborder{border-collapse: collapse;}\n.u2wborder>tbody>tr>th,.u2wborder>tbody>tr>td{padding:1px;border:1px solid black;}\nth{text-align:center;}\n.u2wtaleft{text-align:left;}\n.u2wtacenter{text-align:center;}\n.u2wtaright{text-align:right;}\n</style>\n<meta charset=\"%s\">\n"
#define META_REFRESH "<meta http-equiv=\"refresh\" content=\"%d; url=%s\">\n"
#define PAGE_HEADER_ENDE "</head>\n"

#define PAGE_ERROR_HEADER DOCTYPE "<html><head>\n<title>%s %s</title>\n</head><body>\n<h1>%s</h1>\n"

#define PAGE_BODY_START            "<body>\n"
#define PAGE_BODY_START_PAR        "<body %s>\n"
#define PAGE_BODY_START_BG         "<body background=\"%s\">\n"
#define PAGE_BODY_START_CLASS      "<body class=\"%s\">\n"
#define PAGE_BODY_START_CLASS_PAR  "<body class=\"%s\" %s>\n"
#define PAGE_BODY_START_CLASS_BG   "<body class=\"%s\" background=\"%s\">\n"
#define HEADLINE                 "<h2>"
#define HEADLINE_ENDE            "</h2>\n"
#define PAGE_END                 "</body></html>\n"
#define LINK                     "<a href=\"%s\">%s</a>"
#define LINK_CLASS               "<a href=\"%s\" class=\"%s\">%s</a>"
#define HREF_HTML                "<a href=\"%s\"%s>%s</a>"
#define HREF_HTML_ADD            "<a href=\"%s%s\"%s>%s</a>"
#define IFRAME_HTML              "<iframe src=\"%s\"%s></iframe>"
#define IFRAME_HTML_ADD          "<iframe src=\"%s%s\"%s></iframe>"
#define FRAMERIGHTNAME           "fright"
#define FRAMELEFTNAME            "fleft"
#define FRAMETOPNAME             "ftop"
#define FRAMEBOTTOMNAME          "fbottom"
#define OLDFRAMELINKSNAME        "links"
#define OLDFRAMEOBENNAME         "oben"
#define OLDFRAMERECHTSNAME       "rechts"
#define MENUFRAMERIGHTNAME       "mright"
#define MENUFRAMELEFTNAME        "mleft"
#define MENUFRAMETOPNAME         "mtop"
#define RECHTSLINK               "<a href=\"%s\" target=\"" MENUFRAMERIGHTNAME "\">%s</a>"
#define START_FIXED_FONT         "<pre>"
#define END_FIXED_FONT           "</pre>"
#define LINEEND                  "<br>\n"
#define PAREND                   "<br>\n"
#define TABLE_ROW_START          "<tr>"
#define TABLE_ROW_CLASS_START    "<tr class=\"%s\">"
#define TABLE_ROW_END            "</tr>\n"
#define TABLE_COL_START          "<td>"
#define TABLE_COL_START_LEFT     "<td class=\"u2wtaleft\">"
#define TABLE_COL_START_CENTER   "<td class=\"u2wtacenter\">"
#define TABLE_COL_START_RIGHT    "<td class=\"u2wtaright\">"
#define TABLE_COL_END            "</td>"
#define TABLE_COLUMN_TRENN        TABLE_COL_END TABLE_COL_START
#define TABLE_COLUMN_TRENN_LEFT   TABLE_COL_END TABLE_COL_START_LEFT
#define TABLE_COLUMN_TRENN_CENTER TABLE_COL_END TABLE_COL_START_CENTER
#define TABLE_COLUMN_TRENN_RIGHT  TABLE_COL_END TABLE_COL_START_RIGHT
#define TABLE_COL_CLASS_START    "<td class=\"%s\">"
#define TABLE_COLUMN_CLASS_TRENN TABLE_COL_END TABLE_COL_CLASS_START
#define TABLE_COL_HEAD_START     "<th style=\"text-align: center\">"
#define TABLE_COL_HEAD_CLASS_START "<th class=\"%s\">"
#define TABLE_COL_HEAD_END       "</th>"
#define TABLE_COLUMN_HEAD_TRENN  TABLE_COL_HEAD_END TABLE_COL_HEAD_START
#define TABLE_COLUMN_HEAD_CLASS_TRENN TABLE_COL_HEAD_END TABLE_COL_HEAD_CLASS_START
#define IMAGE_START              "<img src=\""
#define IMAGE_END                "\">"

#define DIRLIST_START       HEADLINE "%s %s" HEADLINE_ENDE START_FIXED_FONT
#define DIRLIST_START_SUCH  HEADLINE "%s %s%s" HEADLINE_ENDE START_FIXED_FONT
#ifdef LONGLONG
#define DIRLIST_ENTRY       LINK " %*s %9llu\n"
#else
#define DIRLIST_ENTRY       LINK " %*s %9lu\n"
#endif
#define DIRLIST_END         END_FIXED_FONT



/* Link mit Target                                                                     */
#define TARGETLINK         "<a href=\"%s\" target=\"%s\">%s</a>"
#define TARGETLINK_CLASS   "<a href=\"%s\" target=\"%s\" class=\"%s\">%s</a>"

/* Submenulink mit Target                                                              */
#define SUBMENULINK  "<a href=\"%s?" SUBMENUPAR "=%s%s\" target=\"%s\">%s</a><br>\n"
/* Submenulink mit Target und Hauptmenü                                                */
#define SUBMENULINKHPTM  "<a href=\"%s?" SUBMENUPAR "=%s&" HAUPTMENUPAR "=%s%s\" target=\"%s\">%s</a><br>\n"

/* Submenulink für %subfile ohne [:r_url:o_url] ohne gesetztes HAUPTMENUPAR            */
#define SUBMENULINKFILE "<a href=\"%s%c" MENUNAME "=" MENUSUBFILE "&" HAUPTMENUPAR "=/%s\" target=\"" MENUFRAMELEFTNAME "\">%s</a><br>\n"

/* Submenulink für %subfile ohne [:r_url:o_url] mit gesetztem HAUPTMENUPAR             */
#define SUBMENULINKFILELAST "<a href=\"%s%c" MENUNAME "=" MENUSUBFILE "&" HAUPTMENUPAR "=%s&" LASTMENUPAR "=%s\" target=\"" MENUFRAMELEFTNAME "\">%s</a><br>\n"

/* Submenulink für %subfile:r_url:o_url mit gesetztem HAUPTMENUPAR                     */
#define SUBMENULASTMENU "%s%c" MENUNAME "=" MENUSUBFILE "&" HAUPTMENUPAR "=%s&" LASTMENUPAR "=%s"

/* Submenulink für %subfile:r_url:o_url ohne gesetztem HAUPTMENUPAR                    */
#define SUBMENUINTLINK "%s%c" MENUNAME "=" MENUSUBFILE "&" HAUPTMENUPAR "=/%s"

/* Submenulink für %subfile:r_url:o_url wird um die obigen zwei defines erweitert      */
#define SUBMENULINKFILELAST_PARENT "<a href=\"%s?" LINKSMENU "=%s%s\" target=\"_parent\">%s</a><br>\n"

/* Submenushell                                                                        */
#define SUBMENUSHELL "<a href=\"%s?" MENUNAME "=" MENUSHELL "&" SUBMENUPAR "=%s&" SHELLMENU "=%s\" target=\"right\">%s</a><br>\n"

#define SETFRAMESCRIPT "<script type=\"text/javascript\">\n<!--\ntop.%s.location.href=\"%s\";\n//-->\n</script>\n"

#define CSSLINKHEADER "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n"

