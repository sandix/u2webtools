/***********************************************/
/* File: u2w_menu.c                            */
/* Funktionen zur Erzeugung von Menueseiten    */
/* timestamp: 2013-10-27 11:07:42              */
/***********************************************/


#include "u2w.h"

#ifndef COMPILER

/***************************************************************************************/
/* int send_frame(char *rows, char *cols, char *def1, char *def2)                      */
/*                char *rows: Wert für rows= oder ""                                   */
/*                char *cols: Wert für cols= oder ""                                   */
/*                char *def1: Defaultseite1                                            */
/*                char *def2: Defaultseite2                                            */
/*     send_frame erstellt ein frameset                                                */
/***************************************************************************************/
int send_frame(char *rows, char *cols, char *def1, char *def2)
{ char link[MAX_ZEILENLAENGE];

  LOG(1, "send_frame, rows: %s, cols: %s, def1: %s, def2: %s.\n", rows, cols, def1, def2);

  if( *rows )
  { if( dosendf("<frameset rows=\"%s,*\"><frame name=\"" FRAMETOPNAME  "\" src=\"", rows) )
      return true;
  }
  else if( *cols )
  { if( dosendf("<frameset cols=\"%s, *\"><fram name=\"" FRAMELEFTNAME  "\"e src=\"", cols) )
      return true;
  }
  else
  { if( dosend("<frameset rows=\"50%, *\"><fram name=\"" FRAMETOPNAME  "\"e src=\"") )
      return true;
  }

  if( *def1 )
  { scan_to(zeile, &def1, MAX_ZEILENLAENGE, 0, "", '\0');
    code_link(link, zeile, MAX_ZEILENLAENGE, false);
    if( dosend(link) )
      return true;
  }
  else
  { if( dosend(clientgetfile) || dosend("?" FRAMENAME "=" FRAME1) )
      return true;
  }

  if( dosendf("\"><frame name=\"%s\" src=\"", *rows || !*cols ? FRAMEBOTTOMNAME : FRAMERIGHTNAME) )
    return true;

  if( *def2 )
  { scan_to(zeile, &def2, MAX_ZEILENLAENGE, 0, "", '\0');
    code_link(link, zeile, MAX_ZEILENLAENGE, false);
    if( dosend(link) )
      return true;
  }
  else
  { if( dosend(clientgetfile) || dosend("?" FRAMENAME "=" FRAME2 ))
      return true;
  }

  return dosend("\"></frameset><noframes><body>Diese Seite verwendet Frames, der Browser unterstützt keine Frames!</body></noframes></frameset>\n</HTML>\n");
}
#endif  /* #ifndef COMPILER */
