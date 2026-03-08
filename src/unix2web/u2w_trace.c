/*********************************/
/* File: u2w_trace.c             */
/* Funktionen zur Methode TRACE  */
/* timestamp: 2021-12-12 20:10:32*/
/*********************************/


#include "u2w.h"

#ifdef WEBSERVER


/***************************************************************************************/
/* int send_trace(char *puffer, long nb)                                               */
/*                char *puffer: Gelesene zeichen vom Browser                           */
/*                long nb: Zeichen in puffer                                           */
/*     send_trace Methode TRACE, gelesene zeichen zurücksenden                         */
/***************************************************************************************/
int send_trace(char *puffer, long nb)
{
  LOG(1, "send_trace, puffer: %.*s.\n", nb, puffer);

  if( send_http_header("message/http", "", true, 0, NULL) )
    return true;
  return dobinsend(puffer, nb);

}
#endif  /* #ifdef WEBSERVER */
