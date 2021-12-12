/******************************************/
/* File: u2w_u2w_command_progs.c          */
/* Funktionen aus Array u2w_command_progs */
/* ausser mysql-Funktionen                */
/* timestamp: 2017-02-25 18:52:45         */
/******************************************/


#include "u2w.h"


#ifdef WEBSERVER
/***************************************************************************************/
/* int u2w_par_var(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*                  int pa: Bitflag mit gesetzten Parameterfeldern                     */
/*                  char prg_pars: Parameter                                           */
/*     u2w_par_var %var: hidden Parameter                                              */
/***************************************************************************************/
int u2w_par_var(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], char token)
{
  if( tablelevel )
    return newhiddenvar(prg_pars[0], prg_pars[1]);
  else
    return dosendf("<input name=\"%s\" type=\"hidden\" value=\"%s\">",
                   prg_pars[0], prg_pars[1]);
}


/***************************************************************************************/
/* int u2w_par_varif(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*                  int pa: Bitflag mit gesetzten Parameterfeldern                     */
/*                  char prg_pars: Parameter                                           */
/*     u2w_par_varif %varif: hidden Parameter, wenn Wert gesetzt                       */
/***************************************************************************************/
int u2w_par_varif(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], char token)
{
  if( *prg_pars[1] )
  { if( tablelevel )
      return newhiddenvar(prg_pars[0], prg_pars[1]);
    else
      return dosendf("<input name=\"%s\" type=\"hidden\" value=\"%s\">",
                     prg_pars[0], prg_pars[1]);
  }
  else
    return false;
}


/***************************************************************************************/
/* int u2w_hiddenvars(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*                  int pa: Bitflag mit gesetzten Parameterfeldern                     */
/*                  char prg_pars: Parameter                                           */
/*     u2w_hiddenvars %hiddenvars: hidden Parameter nach Liste                         */
/***************************************************************************************/
/* steht in u2w_par.c, weil inline Funktion genutzt wird.                              */
/***************************************************************************************/
#endif


/***************************************************************************************/
/* int u2w_button(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS],         */
/*                char token)                                                          */
/*                  int pars: Bitflag mit gesetzten Parameterfeldern                   */
/*                  char prg_pars: Parameter                                           */
/*                  char token: wird hier ignoriert                                    */
/*     u2w_button liest Block hinter %button                                           */
/***************************************************************************************/
int u2w_button(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], char token)
{ if( pars & P5 )
  { snprintf(zeile, MAX_ZEILENLAENGE, " target=\"%s\"", conv_target(prg_pars[4]));
    return send_link_button(prg_pars[0], prg_pars[2], zeile, "");
  }
  else
    return send_link_button(prg_pars[0], prg_pars[2], "", "");
}


/***************************************************************************************/
/* int u2w_colspan(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*                  int pars: Bitflag mit gesetzten Parameterfeldern                   */
/*                  char prg_pars: Parameter                                           */
/*     u2w_colspan TD mit colspan                                                      */
/***************************************************************************************/
int u2w_colspan(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;
  char z[MAX_ZEILENLAENGE], *p;

  i = atoi(prg_pars[1]);
  p = z;
  if( pars & P3 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", prg_pars[2]);
  else if( css_defs[CSS_TD] )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", css_defs[CSS_TD]);
  if( pars & P4 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " %s", prg_pars[3]);
  *p = '\0';
  if( akt_tablecols )
  { akt_tablecols += i;
    return dosendf("<td colspan=%d%s>%s</td>\n",
                   i, z, *prg_pars[0] ? prg_pars[0] : "&nbsp;");
  }
  else
  { akt_tablecols += i;
    if( css_defs[CSS_TR] )
      return dosendf("<tr class=\"%s\"><td colspan=%d%s>%s</td>\n", css_defs[CSS_TR],
                     i, z, *prg_pars[0] ? prg_pars[0] : "&nbsp;");
    else
      return dosendf("<tr><td colspan=%d%s>%s</td>\n",
                     i, z, *prg_pars[0] ? prg_pars[0] : "&nbsp;");
  }
}


/***************************************************************************************/
/* int u2w_rowspan(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*                  int pars: Bitflag mit gesetzten Parameterfeldern                   */
/*                  char prg_pars: Parameter                                           */
/*     u2w_rowspan TD mit rowspan                                                      */
/***************************************************************************************/
int u2w_rowspan(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;
  char z[MAX_ZEILENLAENGE], *p;

  i = atoi(prg_pars[1]);
  p = z;
  if( pars & P3 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", prg_pars[2]);
  else if( css_defs[CSS_TD])
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " class=\"%s\"", css_defs[CSS_TD]);
  if( pars & P4 )
    p += snprintf(p, MAX_ZEILENLAENGE-(p-z), " %s", prg_pars[3]);
  *p = '\0';
  if( akt_tablecols++ )
    return dosendf("<td rowspan=%d%s>%s</td>\n",
                   i, z, *prg_pars[0] ? prg_pars[0] : "&nbsp;");
  else if( css_defs[CSS_TR] )
    return dosendf("<tr class=\"%s\"><td rowspan=%d%s>%s</td>\n", css_defs[CSS_TR],
                   i, z, *prg_pars[0] ? prg_pars[0] : "&nbsp;");
  else
    return dosendf("<tr><td rowspan=%d%s>%s</td>\n",
                   i, z, *prg_pars[0] ? prg_pars[0] : "&nbsp;");
}


/***************************************************************************************/
/* int u2w_par(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], char token)*/
/*            int pars: Bitflag mit gesetzten Parameterfeldern                         */
/*            char prg_pars: Parameter                                                 */
/*            char token: Art des Formularfeldes                                       */
/*     u2w_par liest Block hinter Formularfeldern                                      */
/***************************************************************************************/
int u2w_par(int pars, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS], char token)
{ int i1, i2;
  int rda_flag;                                    /* Bit 1: read-only                 */
                                                   /* Bit 2: disabled                  */
                                                   /* Bit 4: autofocus                 */

  LOG(1, "u2w_par, p7: %s, token: %c.\n", (pars & P7) ? prg_pars[6] : "", token);

  i1 = i2 = 0;
  if( (pars & P5) && isalnum(prg_pars[4][0]) )
    i1 = atoi(prg_pars[4]);
  if( (pars & P6) && isalnum(prg_pars[5][0]) )
    i2 = atoi(prg_pars[5]);
  rda_flag = ((pars & P7) && strchr(prg_pars[6], 'r')) ? 1 : 0;
  rda_flag |= ((pars & P7) && strchr(prg_pars[6], 'd')) ? 2 : 0;
  rda_flag |= ((pars & P7) && strchr(prg_pars[6], 'a')) ? 4 : 0;

  return send_par(token, prg_pars[0], (pars & P4) ? prg_pars[3] : "",
                  (pars & P3) ? prg_pars[2] : NULL, (pars & P2) ? prg_pars[1] : "",
                  i1, i2, rda_flag, (pars & P8) ? prg_pars[7] : NULL,
                  (pars & P9) ? prg_pars[8] : NULL, (pars & PA) ? prg_pars[9] : NULL);
}
