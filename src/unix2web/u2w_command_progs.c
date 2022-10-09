/**************************************/
/* File: u2w_command_progs.c          */
/* Funktionen aus Array command_progs */
/* ausser mysql-Funktionen            */
/* timestamp: 2017-02-12 17:57:29     */
/**************************************/


#include "u2w.h"


/***************************************************************************************/
/* short u2w_list(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],  */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_list speichert mehrere Parameter aus Liste                                  */
/***************************************************************************************/
short u2w_list(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_list, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_list, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_list, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], NONEW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    change_parwert(parnames[i], "", NONEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_lista(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_lista speichert mehrere Parameter aus Liste                                 */
/***************************************************************************************/
short u2w_lista(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_lista, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_lista, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_lista, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], NONEW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    add_parwert(parnames[i], "", NONEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_listi(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_listi speichert mehrere Parameter aus Liste                                 */
/***************************************************************************************/
short u2w_listi(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_listi, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_listi, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_listi, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], LONGW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    change_parwert(parnames[i], "", LONGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_listai(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_listai speichert mehrere Parameter aus Liste                                */
/***************************************************************************************/
short u2w_listai(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_listai, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_listai, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_listai, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], LONGW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    add_parwert(parnames[i], "", LONGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_listf(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_listf speichert mehrere Parameter aus Liste                                 */
/***************************************************************************************/
short u2w_listf(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_listf, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_listf, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_listf, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], DOUBLEW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    change_parwert(parnames[i], "", DOUBLEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_listaf(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_listaf speichert mehrere Parameter aus Liste                                */
/***************************************************************************************/
short u2w_listaf(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_listaf, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_listaf, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_listaf, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], DOUBLEW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    add_parwert(parnames[i], "", DOUBLEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_lists(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_lists speichert mehrere Parameter aus Liste                                 */
/***************************************************************************************/
short u2w_lists(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_lists, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_lists, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_lists, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], STRINGW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    change_parwert(parnames[i], "", STRINGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_listas(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_listas speichert mehrere Parameter aus Liste                                */
/***************************************************************************************/
short u2w_listas(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_listas, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_listas, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_listas, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], STRINGW, HP_GLOBAL_LEVEL);
  for( ; i < lens[0]; i++ )                       /* alle weiteren Vars auf ""         */
    add_parwert(parnames[i], "", STRINGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_llist(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llist speichert mehrere lokale Parameter aus Liste                          */
/***************************************************************************************/
short u2w_llist(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llist, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llist, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llist, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], NONEW, include_counter);
  for( ; i < lens[0]; i++ )
    change_parwert(parnames[i], "", NONEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llista(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llista speichert mehrere lokale Parameter aus Liste                         */
/***************************************************************************************/
short u2w_llista(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llista, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llista, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llista, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], NONEW, include_counter);
  for( ; i < lens[0]; i++ )
    add_parwert(parnames[i], "", NONEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llisti(int pa, int lens[2],char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llisti speichert mehrere lokale Parameter aus Liste                         */
/***************************************************************************************/
short u2w_llisti(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llisti, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llisti, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llisti, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], LONGW, include_counter);
  for( ; i < lens[0]; i++ )
    change_parwert(parnames[i], "", LONGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llistai(int pa, int lens[2],char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llistai speichert mehrere lokale Parameter aus Liste                        */
/***************************************************************************************/
short u2w_llistai(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llistsai, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llistai, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llistai, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], LONGW, include_counter);
  for( ; i < lens[0]; i++ )
    add_parwert(parnames[i], "", LONGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llistf(int pa, int lens[2],char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llistf speichert mehrere lokale Parameter aus Liste                         */
/***************************************************************************************/
short u2w_llistf(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llistf, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llistf, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llistf, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], DOUBLEW, include_counter);
  for( ; i < lens[0]; i++ )
    change_parwert(parnames[i], "", DOUBLEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llistaf(int pa, int lens[2],char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llistaf speichert mehrere lokale Parameter aus Liste                        */
/***************************************************************************************/
short u2w_llistaf(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llistsaf, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llistaf, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llistaf, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], DOUBLEW, include_counter);
  for( ; i < lens[0]; i++ )
    add_parwert(parnames[i], "", DOUBLEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llists(int pa, int lens[2],char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN], */
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llists speichert mehrere lokale Parameter aus Liste                         */
/***************************************************************************************/
short u2w_llists(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llists, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llists, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llists, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    change_parwert(parnames[i], list_pars[i], STRINGW, include_counter);
  for( ; i < lens[0]; i++ )
    change_parwert(parnames[i], "", STRINGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llistas(int pa, int lens[2],char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],*/
/*              char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])                       */
/*             int lens: Anzahl der Elemente in parnames und list_pars                 */
/*             char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN]: Parameternamen           */
/*             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS]: Werte in Liste         */
/*             return: true bei Fehler                                                 */
/*     u2w_llistas speichert mehrere lokale Parameter aus Liste                        */
/***************************************************************************************/
short u2w_llistas(int pa, int lens[2], char parnames[MAX_LIST_LEN][MAX_PAR_NAME_LEN],
             char list_pars[MAX_LIST_LEN][MAX_LEN_LIST_PARS])
{ int i;

  LOG(3, "u2w_llistsas, lens[0]: %d, lens[1]: %d.\n", lens[0], lens[1]);
  LOG(7, "u2w_llistas, parnames[0]: %s, list_pars[0]: %s.\n", parnames[0], list_pars[0]);
  LOG(17, "u2w_llistas, parnames[1]: %s, list_pars[1]: %s.\n", parnames[1], list_pars[1]);

  for( i = 0; i < lens[0] && i < lens[1]; i++ )
    add_parwert(parnames[i], list_pars[i], STRINGW, include_counter);
  for( ; i < lens[0]; i++ )
    add_parwert(parnames[i], "", STRINGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_let(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])            */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_let speichert einen neuen Parameter in pars                                 */
/***************************************************************************************/
short u2w_let(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], NONEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_let_plus(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_let speichert einen neuen Parameter in pars                                 */
/***************************************************************************************/
short u2w_let_plus(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], NONEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_leti(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])           */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_leti speichert einen neuen Integer Parameter in pars                        */
/***************************************************************************************/
short u2w_leti(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], LONGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_letf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])           */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_letf speichert einen neuen Double Parameter in pars                         */
/***************************************************************************************/
short u2w_letf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], DOUBLEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_lets(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])           */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_lets speichert einen neuen String Parameter in pars                         */
/***************************************************************************************/
short u2w_lets(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], STRINGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_let_plusi(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_let speichert einen neuen Parameter in pars                                 */
/***************************************************************************************/
short u2w_let_plusi(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], LONGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_let_plusf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_let speichert einen neuen Parameter in pars                                 */
/***************************************************************************************/
short u2w_let_plusf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], DOUBLEW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_let_pluss(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_let_pluss speichert einen zsätzlichen String Parameter in pars              */
/***************************************************************************************/
short u2w_let_pluss(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], STRINGW, HP_GLOBAL_LEVEL);
  return false;
}


/***************************************************************************************/
/* short u2w_llet(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])           */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_llet speichert einen neuen lokalen Parameter in pars                        */
/***************************************************************************************/
short u2w_llet(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], NONEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llet_plus(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_llet_plus speichert einen weitern lokalen Wert im Parameter pars            */
/***************************************************************************************/
short u2w_llet_plus(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], NONEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_lleti(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_lleti speichert einen neuen lokalen Integer Parameter in pars               */
/***************************************************************************************/
short u2w_lleti(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], LONGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_lletf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_lletf speichert einen neuen lokalen Double Parameter in pars                */
/***************************************************************************************/
short u2w_lletf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], DOUBLEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llets(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_llets speichert einen neuen lokalen String Parameter in pars                */
/***************************************************************************************/
short u2w_llets(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  change_parwert(prg_pars[0], prg_pars[1], STRINGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llet_plusi(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_llet_plusi speichert einen weiteren lokalen Integer Wert zum Parameter pars */
/***************************************************************************************/
short u2w_llet_plusi(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], LONGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llet_plusf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_llet_plusf speichert einen weiteren lokalen Float Wert zum Parameter pars   */
/***************************************************************************************/
short u2w_llet_plusf(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], DOUBLEW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_llet_pluss(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_llet_pluss speichert einen weiteren lokalen String Wert zum Parameter pars  */
/***************************************************************************************/
short u2w_llet_pluss(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  add_parwert(prg_pars[0], prg_pars[1], STRINGW, include_counter);
  return false;
}


/***************************************************************************************/
/* short u2w_eval(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])           */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_evel führt u2w-Kommand %eval aus                                            */
/***************************************************************************************/
short u2w_eval(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  return false;
}


/***************************************************************************************/
/* short u2w_isset(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_isset prüft, ob die Variable, auf die **s zeigt, gesetzt ist                */
/***************************************************************************************/
short u2w_isset(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( prg_pars[0][0] )
  { return NULL == parwert(prg_pars[0], HP_BROWSER_LEVEL);
  }
  return true;
}


/***************************************************************************************/
/* short u2w_setquote(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_setquote speichert zu quotenedes Zeichen für Parameter                      */
/***************************************************************************************/
short u2w_setquote(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa == P1 )
    strcpyn(globalquote, prg_pars[0], MAX_LEN_QUOTE);
  else
    return set_par_quote(prg_pars[1], prg_pars[0]);

  return false;
}


/***************************************************************************************/
/* short store_par2file(char *parname, int level, FILE *ptr)                           */
/*              char parname: zu sichernder Parametername                              */
/*              int level: -2: System, -1: Browser/CMD, 0: global, x: level <= x       */
/*              int h: Filehandle, in das gesichert wird                               */
/*              return: true bei Fehler                                                */
/*     store_par2file schreibt Daten aus parname in Datei mit Stream ptr               */
/***************************************************************************************/
short store_par2file(char *parname, int level, FILE *ptr)
{ wertetype *w;
  char *p, *q;
  size_t nb;
  char zahl[128];

  LOG(1, "store_par2file, parname: %s.\n", parname);

  if( NULL != (w = firstwert(parname, level)) )
  { switch( w->wert.type )
    { case EMPTYW:  nb = 0;
                    break;
      case BINARYW: nb = w->wert.binlen;
                    p = w->wert.s;
                    break;
      case STRINGW: p = w->wert.s;
                    nb = strlen(p);
                    break;
      case DOUBLEW: snprintf(zahl, 128, "%f", w->wert.w.d);
#ifdef WITH_GETTEXT
                    if( NULL != (p = strchr(zahl, *dezimalpunkt)) )
#else
                    if( NULL != (p = strchr(zahl, '.')) )
#endif
                    { q = p + strlen(p) - 1;
                      while( *q == '0' && q-p > 1 )
                        *q-- = '\0';
                    }
                    p = zahl;
                    nb = strlen(p);
                    break;
       case LONGW:  snprintf(zahl, 128, LONGFORMAT, w->wert.w.l);
                    p = zahl;
                    nb = strlen(p);
                    break;
       default:     nb = 0;
    }
    if( nb > 0 )
    { if( nb < fwrite(p, 1, nb, ptr) )
        return true;
    }
    return false;
  }
  return false;
}


/***************************************************************************************/
/* short u2w_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_store speichert Variableninhalt in Datei                                    */
/***************************************************************************************/
short u2w_store(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ FILE *ptr;
  int ret;

  LOG(1, "u2w_store, parname: %s, file: %s.\n", prg_pars[0], prg_pars[1]);

  if( NULL != (ptr = fopen(prg_pars[1], "w")) )
  { ret = store_par2file(prg_pars[0], include_counter, ptr);
    fclose(ptr);
    return ret;
  }
  return true;
}


/***************************************************************************************/
/* short u2w_bstore(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_bstore speichert Browser-Variableninhalt in Datei                           */
/***************************************************************************************/
short u2w_bstore(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ FILE *ptr;
  int ret;

  LOG(1, "u2w_bstore, parname: %s, file: %s.\n", prg_pars[0], prg_pars[1]);

  if( NULL != (ptr = fopen(prg_pars[1], "w")) )
  { ret = store_par2file(prg_pars[0], HP_BROWSER_LEVEL, ptr);
    fclose(ptr);
    return ret;
  }
  return true;
}


/***************************************************************************************/
/* short u2w_delete(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_delete Datei löschen                                                        */
/***************************************************************************************/
short u2w_delete(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  return unlink(prg_pars[0]) != 0;
}


/***************************************************************************************/
/* short u2w_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_write Parameterinhalt in Datei schreiben                                    */
/***************************************************************************************/
short u2w_write(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  if( pa & P2 )
    i = atoi(prg_pars[1]);
#ifdef INTERPRETER
  else
    i = 1;
#else
#ifdef HAS_DAEMON
  else if( !daemonflag )
    i = 1;
#endif
  else
    return true;
#endif

  if( 0 <= i && i < MAX_ANZ_OPEN_FILES && file_ptrs[i] )
    return store_par2file(prg_pars[0], include_counter, file_ptrs[i]);
  else
    return true;
}


/***************************************************************************************/
/* short u2w_gwrite(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_gwrite Global-Parameterinhalt in Datei schreiben                            */
/***************************************************************************************/
short u2w_gwrite(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  if( pa & P2 )
    i = atoi(prg_pars[1]);
#ifdef INTERPRETER
  else
    i = 1;
#else
#ifdef HAS_DAEMON
  else if( !daemonflag )
    i = 1;
#endif
  else
    return true;
#endif

  if( 0 <= i && i < MAX_ANZ_OPEN_FILES && file_ptrs[i] )
    return store_par2file(prg_pars[0], 0, file_ptrs[i]);
  else
    return true;
}


/***************************************************************************************/
/* short u2w_bwrite(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_bwrite Browser-Parameterinhalt in Datei schreiben                           */
/***************************************************************************************/
short u2w_bwrite(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  if( pa & P2 )
    i = atoi(prg_pars[1]);
#ifdef INTERPRETER
  else
    i = 1;
#else
#ifdef HAS_DAEMON
  else if( !daemonflag )
    i = 1;
#endif
  else
    return true;
#endif

  if( 0 <= i && i < MAX_ANZ_OPEN_FILES && file_ptrs[i] )
    return store_par2file(prg_pars[0], -1, file_ptrs[i]);
  else
    return true;
}


/***************************************************************************************/
/* short u2w_print(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_print Daten in Datei schreiben                                              */
/***************************************************************************************/
short u2w_print(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  if( pa & P2 )
    i = atoi(prg_pars[1]);
#ifdef INTERPRETER
  else
    i = 1;
#else
#ifdef HAS_DAEMON
  else if( !daemonflag )
    i = 1;
#endif
  else
    return true;
#endif

  if( 0 <= i && i < MAX_ANZ_OPEN_FILES && file_ptrs[i] )
    return EOF ==  fputs(prg_pars[0], file_ptrs[i]);
  else
    return true;
}


/***************************************************************************************/
/* short u2w_close(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_delete Datei löschen                                                        */
/***************************************************************************************/
short u2w_close(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;
  int ret;

  i = atoi(prg_pars[0]);
  if( 0 <= i && i < MAX_ANZ_OPEN_FILES && file_ptrs[i] != NULL )
  {  ret = fclose(file_ptrs[i]);
     file_ptrs[i] = NULL;
     return ret != 0;
  }
  return true;
}


/***************************************************************************************/
/* short u2w_rename(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_rename Datei umbenennen                                                     */
/***************************************************************************************/
short u2w_rename(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  return rename(prg_pars[0], prg_pars[1]);
}


/***************************************************************************************/
/* short u2w_eof(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])            */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_eof Datei auf Dateiende testen                                              */
/***************************************************************************************/
short u2w_eof(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  if( isdigit(prg_pars[0][0]) )
  { i = atoi(prg_pars[0]);
    if( 0 <= i && i < MAX_ANZ_OPEN_FILES && file_ptrs[i] )
      return !feof(file_ptrs[i]);
  }
  return true;
}


/***************************************************************************************/
/* short u2w_errorout(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_errorout auf stderr schreiben                                               */
/***************************************************************************************/
short u2w_errorout(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
#ifndef INTERPRETER
#ifdef HAS_DAEMON
  if( !daemonflag )
#endif
#endif
    fputs(prg_pars[0], stderr);
  return false;
}


/***************************************************************************************/
/* short u2w_setenv(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_setenv Environment-Variable setzen                                          */
/***************************************************************************************/
short u2w_setenv(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(1, "u2w_setenv, p1: %s.\n", prg_pars[0]);

  if( pa & P2 )
    return setenv(prg_pars[0], prg_pars[1], 1);
  else
   return unsetenv(prg_pars[0]);
}


/***************************************************************************************/
/* short u2w_set(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])            */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_set System-Variable setzen                                                  */
/***************************************************************************************/
short u2w_set(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(1, "u2w_set, p1: %s.\n", prg_pars[0]);

  if( pa == P2 )
    return NULL == store_parwert(prg_pars[0], prg_pars[1], HP_SYSTEM_LEVEL);
  else
  { switch( prg_pars[0][1] )
    { case '2': if( !strcmp(prg_pars[0], SET_S2WMODE) )
                  u2w_mode = S2W_MODE;
                else if( !strcmp(prg_pars[0], SET_U2WMODE) )
                  u2w_mode = U2W_MODE;
      case '3': if( !strcmp(prg_pars[0], SET_S3WMODE) )
                  u2w_mode = S3W_MODE;
                else if( !strcmp(prg_pars[0], SET_U3WMODE) )
                  u2w_mode = U3W_MODE;
      case '4': if( !strcmp(prg_pars[0], SET_S4WMODE) )
                  u2w_mode = S4W_MODE;
                else if( !strcmp(prg_pars[0], SET_U4WMODE) )
                  u2w_mode = U4W_MODE;
      case '5': if( !strcmp(prg_pars[0], SET_S5WMODE) )
                  u2w_mode = S5W_MODE;
                else if( !strcmp(prg_pars[0], SET_U5WMODE) )
                  u2w_mode = U5W_MODE;
    }
    LOG(3, "/u2w_set, u2w_mode: %d.\n", u2w_mode);
    return 0;
  }
}


#ifdef WITH_GETTEXT
/***************************************************************************************/
/* short u2w_setlang(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_setlang Environment-Variable LANG setzen und setlocale aufrufen             */
/***************************************************************************************/
short u2w_setlang(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ struct lconv *l;
  char *p;

  LOG(1, "u2w_setlang, p1: %s.\n", prg_pars[0]);

  if( (p = strchr(prg_pars[0], '.')) )
  { if( utf8flag )
      strcpyn(p, ".UTF-8", MAX_LEN_PRG_PARS-(p-prg_pars[0]));
    else
      *p = '\0';
  }
  else
  { if( utf8flag )
      strcatn(prg_pars[0], ".UTF-8", MAX_LEN_PRG_PARS);
  }

  if( !setenv("LANG", prg_pars[0], 1) )
  { setlocale(LC_ALL, "");
    if( NULL != (l = localeconv()) && l->decimal_point[0] )
      dezimalpunkt = l->decimal_point;

    return 0;
  }
  else
   return true;
}


/***************************************************************************************/
/* short u2w_bindtextdomain(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS]) */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bindtextdomain Textdomain setzen und mit Directory verbinden                */
/***************************************************************************************/
short u2w_bindtextdomain(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  u2wtextdomain = store_str(prg_pars[1]);
  if( pa & P2 )
    bindtextdomain(prg_pars[1], prg_pars[2]);
  else
    bindtextdomain(u2wtextdomain, u2wtextdomaindir);
  return false;
}
#endif  /* WITH_GETTEXT */


/***************************************************************************************/
/* short u2w_logging(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_logout in Loggingdatei schreiben                                            */
/***************************************************************************************/
short u2w_logging(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ if( pa & P2 )
  { if( pa & P3 )
    { if( pa & P4 )
      { if( pa & P5 )
        { if( pa & P6 )
          { if( pa & P7 )
            { if( pa & P8 )
                logging(prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3], prg_pars[4],
                        prg_pars[5], prg_pars[6], prg_pars[7]);
               else
                logging(prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3], prg_pars[4],
                        prg_pars[5], prg_pars[6]);
            }
            else
              logging(prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3], prg_pars[4],
                      prg_pars[5]);
          }
          else
            logging(prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3], prg_pars[4]);
        }
        else
          logging(prg_pars[0], prg_pars[1], prg_pars[2], prg_pars[3]);
      }
      else
        logging(prg_pars[0], prg_pars[1], prg_pars[2]);
    }
    else
      logging(prg_pars[0], prg_pars[1]);
  }
  else
    logging("%s\n", prg_pars[0]);
  return false;
}


/***************************************************************************************/
/* short u2w_sleep(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])          */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_sleep angegebene Sekunden schlafen                                          */
/***************************************************************************************/
short u2w_sleep(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  if( 0 < (i = atoi(prg_pars[0])) )
    sleep(i);
  return false;
}


#ifndef OLDUNIX
/***************************************************************************************/
/* short u2w_usleep(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])         */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_sleep angegebene MikroSekunden schlafen                                     */
/***************************************************************************************/
short u2w_usleep(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ long l;

  if( 0 < (l = atol(prg_pars[0])) )
    usleep(l);
  return false;
}
#endif


/***************************************************************************************/
/* short u2w_partrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_partrim Parameter trimmen                                                   */
/***************************************************************************************/
short u2w_partrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], include_counter, 3);
  else
    trim_parwert_blanks(prg_pars[0], include_counter, 3);
  return false;
}


/***************************************************************************************/
/* short u2w_parltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_parltrim Parameter trimmen                                                  */
/***************************************************************************************/
short u2w_parltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], include_counter, 1);
  else
    trim_parwert_blanks(prg_pars[0], include_counter, 1);
  return false;
}


/***************************************************************************************/
/* short u2w_parrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_parrtrim Parameter trimmen                                                  */
/***************************************************************************************/
short u2w_parrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], include_counter, 2);
  else
    trim_parwert_blanks(prg_pars[0], include_counter, 2);
  return false;
}


/***************************************************************************************/
/* short u2w_gpartrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_gpartrim Parameter trimmen                                                  */
/***************************************************************************************/
short u2w_gpartrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_GLOBAL_LEVEL, 3);
  else
    trim_parwert_blanks(prg_pars[0], HP_GLOBAL_LEVEL, 3);
  return false;
}


/***************************************************************************************/
/* short u2w_gparltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_gparltrim Parameter trimmen                                                 */
/***************************************************************************************/
short u2w_gparltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_GLOBAL_LEVEL, 1);
  else
    trim_parwert_blanks(prg_pars[0], HP_GLOBAL_LEVEL, 1);
  return false;
}


/***************************************************************************************/
/* short u2w_gparrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_gparrtrim Parameter trimmen                                                 */
/***************************************************************************************/
short u2w_gparrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_GLOBAL_LEVEL, 2);
  else
    trim_parwert_blanks(prg_pars[0], HP_GLOBAL_LEVEL, 2);
  return false;
}


/***************************************************************************************/
/* short u2w_bpartrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bpartrim Parameter trimmen                                                  */
/***************************************************************************************/
short u2w_bpartrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_BROWSER_LEVEL, 3);
  else
    trim_parwert_blanks(prg_pars[0], HP_BROWSER_LEVEL, 3);
  return false;
}


/***************************************************************************************/
/* short u2w_bparltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bparltrim Parameter trimmen                                                 */
/***************************************************************************************/
short u2w_bparltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_BROWSER_LEVEL, 1);
  else
    trim_parwert_blanks(prg_pars[0], HP_BROWSER_LEVEL, 1);
  return false;
}


/***************************************************************************************/
/* short u2w_bparrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bparrtrim Parameter trimmen                                                 */
/***************************************************************************************/
short u2w_bparrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_BROWSER_LEVEL, 2);
  else
    trim_parwert_blanks(prg_pars[0], HP_BROWSER_LEVEL, 2);
  return false;
}


/***************************************************************************************/
/* short u2w_cpartrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])       */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_cpartrim Parameter trimmen                                                  */
/***************************************************************************************/
short u2w_cpartrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_COOKIE_LEVEL, 3);
  else
    trim_parwert_blanks(prg_pars[0], HP_COOKIE_LEVEL, 3);
  return false;
}


/***************************************************************************************/
/* short u2w_cparltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_cparltrim Parameter trimmen                                                 */
/***************************************************************************************/
short u2w_cparltrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_COOKIE_LEVEL, 1);
  else
    trim_parwert_blanks(prg_pars[0], HP_COOKIE_LEVEL, 1);
  return false;
}


/***************************************************************************************/
/* short u2w_cparrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bcparrtrim Parameter trimmen                                                */
/***************************************************************************************/
short u2w_cparrtrim(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  if( pa & P2 )
    trim_parwert(prg_pars[0], prg_pars[1], HP_COOKIE_LEVEL, 2);
  else
    trim_parwert_blanks(prg_pars[0], HP_COOKIE_LEVEL, 2);
  return false;
}


/***************************************************************************************/
/* short u2w_bglobal(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bglobal Browser/CMD-Line Parameter zu globalen kopieren optional trimmen    */
/***************************************************************************************/
/* steht in u2w_par.c, weil inline Funktion genutzt wird.                              */
/***************************************************************************************/


/***************************************************************************************/
/* short u2w_bpardef(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*             int pa: Anzahl Parameter in prg_pars                                    */
/*             char prg_pars: übergebene Funktionsparameter                            */
/*             return: true bei Fehler                                                 */
/*     u2w_bpardef Browser/CMD-Line Parameter default-Wert festlegen und optional      */
/*                 zu globalen kopieren und trimmen                                    */
/***************************************************************************************/
/* steht in u2w_par.c, weil inline Funktion genutzt wird.                              */
/***************************************************************************************/


#ifdef WEBSERVER
/***************************************************************************************/
/* short u2w_check_cred(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])     */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_check_cred Testen, ob User und Kennwort zusammenpassen (Unix-Anmeldung/PAM) */
/***************************************************************************************/
short u2w_check_cred(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  return !test_pwd(prg_pars[0], prg_pars[1]);
}


/***************************************************************************************/
/* short u2w_timeout(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])        */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_timeout Timeout setzen                                                      */
/***************************************************************************************/
short u2w_timeout(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ unsigned int i;

  i = (atol(prg_pars[0])+2) / 3;
  if( 0 < i && i < MAX_TIMEOUT_SECS )
  {  timeout_secs = i;
     return false;
  }
  return true;
}


/***************************************************************************************/
/* short u2w_css(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])            */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_css set default classes                                                     */
/***************************************************************************************/
short u2w_css(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{ int i;

  LOG(1, "u2w_css, element: %s, class: %s.\n", prg_pars[0], prg_pars[1]);
  for( i = 0; i < NUM_CSS_DEF_ELEMENTS; i++ )
  { if( !strcasecmp(css_elementtypes[i], prg_pars[0]) )
    { if( pa & P2 && *prg_pars[1] )
      { css_defs[i] = store_str(prg_pars[1]);
        LOG(23, "u2w_css, i: %d, css: %s.\n", i, prg_pars[1]);
      }
      else
        css_defs[i] = NULL;
      return false;
    }
  }
  return false;
}


/***************************************************************************************/
/* short u2w_delonexit(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])      */
/*              int pa: Anzahl Parameter in prg_pars                                   */
/*              char prg_pars: übergebene Funktionsparameter                           */
/*              return: true bei Fehler                                                */
/*     u2w_delonexit Datei zum Löschen beim Beenden eintragen                          */
/***************************************************************************************/
short u2w_delonexit(int pa, char prg_pars[MAX_ANZ_PRG_PARS][MAX_LEN_PRG_PARS])
{
  LOG(1, "u2w_delonexit, p1: %s.\n", prg_pars[0]);

  if( anz_auto_delete_files < MAX_ANZ_AUTO_DELETE_FILES )
  { strcpyn(auto_delete_files[anz_auto_delete_files++], prg_pars[0], MAX_PATH_LEN);
    return 0;
  }
  else
    return 1;
}
#endif  /* #ifdef WEBSERVER */
