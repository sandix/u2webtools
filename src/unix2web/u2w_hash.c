/*********************************************/
/* File: u2w_hash.c                          */
/* Funktionen zur Erstellung und Nutzung der */
/* u2w-Kommando Hashtabelle                  */
/* timestamp: 2014-04-16 21:34:12            */
/*********************************************/


#include "u2w.h"


/***************************************************************************************/
/* unsigned int u2w_hash(char *s)                                                      */
/*                 char *s: String                                                     */
/*                 return : Hashwert zum String s                                      */
/*              u2w_hash berechnet Hashwert                                            */
/***************************************************************************************/
unsigned int u2w_hash(char *s)
{ unsigned short c;
  unsigned int h;

  h = U2W_HASH_INIT;
  while( (c = *s++) )
    h = h<<U2W_HASH_SHIFT ^ c;
  return h % U2W_HASH_TABLE_SIZE;
}


/***************************************************************************************/
/* void add_u2w_hash(char *cmd, unsigned int ttoken, unsigned int i)                   */
/*                   char *cmd: Name des Kommandos ohne %                              */
/*                   unsigned int ttoken: ID für Funktionstyp                          */
/*                   unsigend int i: ID in der Tabelle                                 */
/*      add_u2w_hash fügt ein Kommando in Hash-Tabelle ein                             */
/***************************************************************************************/
void add_u2w_hash(char *cmd, unsigned int ttoken, unsigned int i)
{ unsigned int h;

  LOG(99, "add_u2w_hash, cmd: %s, anz_u2wcmds: %d.\n", cmd, anz_u2wcmds);

  if( anz_u2wcmds < U2W_COMMAND_HASH_SIZE )
  { h = u2w_hash(cmd);
    u2wcmds[anz_u2wcmds].command = cmd;
    u2wcmds[anz_u2wcmds].tblid = ttoken;
    u2wcmds[anz_u2wcmds].id = i;
    u2wcmds[anz_u2wcmds].next = u2whash[h];
    u2whash[h] = &u2wcmds[anz_u2wcmds++];
  }
  else
  { fprintf(stderr, "Error no space left for u2wcmds in hash table!\n");
    exit(11);
  }
}


/***************************************************************************************/
/* int get_u2w_hash(char *cmd, unsigned int attoken, unsigned int *ttoken,             */
/*                  unsigned int *i)                                                   */
/*                  char *cmd: Name des Kommandos ohne %                               */
/*                  unsigned char attoken: erlaubte IDs für Funktionstyp               */
/*                  unsigned char ttoken: ID für Funktionstyp                          */
/*                  unsigend char i: ID in der Tabelle                                 */
/*                  return 0: cmd gefunden und ttoken und i gefüllt                    */
/*     get_u2w_hash sucht cmd in Hash-Array und füllt ttoken und i                     */
/***************************************************************************************/
int get_u2w_hash(char *cmd, unsigned int attoken, unsigned int *ttoken, unsigned int *i)
{ unsigned int h;
  hu2wtype *ht;

  LOG(49, "get_u2w_hash, cmd: %s, attoken: %u.\n", cmd, attoken);

  h = u2w_hash(cmd);
  ht = u2whash[h];
  while( ht )
  { if( 0 == strcmp(cmd, ht->command) && (ht->tblid & attoken) )
    { *ttoken = ht->tblid;
      *i = ht->id;
      LOG(69, "/get_u2w_hash, return 0.\n");
      return 0;
    }
    else
      ht = ht->next;
  }
  LOG(69, "/get_u2w_hash, return 1.\n");
  return 1;
}


/***************************************************************************************/
/* void init_u2w_hash(void)                                                            */
/*      init_u2w_hash initialisiert die Hash-Tabellen für U2W-Kommandos                */
/***************************************************************************************/
void init_u2w_hash(void)
{ command_list_prog_type *clp;
  command_prog_type *cp;
  scan_list_prog_type *slp;
  scan_prog_type *sp;
  scan_prog_send_type *sps;
  u2w_put_command_type *upc;
  format_command_type *fc;
  unsigned int i;
#ifdef WEBSERVER
  command_prog_token_type *cpt;
#endif

  clp = command_list_progs;
  for(i = 0; (clp+i)->command[0]; i++)
    add_u2w_hash((clp+i)->command, T_COMMAND_LIST_PROGS, i);

  cp = command_progs;
  for(i = 0; (cp+i)->command[0]; i++)
    add_u2w_hash((cp+i)->command, T_COMMAND_PROGS, i);

#ifdef WEBSERVER
  cpt = u2w_command_progs;
  for(i = 0; (cpt+i)->command[0]; i++)
    add_u2w_hash((cpt+i)->command, T_U2W_COMMAND_PROGS, i);

  cp = u2w_table_progs;
  for(i = 0; (cp+i)->command[0]; i++)
    add_u2w_hash((cp+i)->command, T_U2W_TABLE_PROGS, i);
#endif

  slp = scan_list_progs;
  for(i = 0; (slp+i)->command[0]; i++)
    add_u2w_hash((slp+i)->command, T_SCAN_LIST_PROGS, i);

  sp = scan_progs;
  for(i = 0; (sp+i)->command[0]; i++)
    add_u2w_hash((sp+i)->command, T_SCAN_PROGS, i);

  sp = scan_send_progs;
  for(i = 0; (sp+i)->command[0]; i++)
    add_u2w_hash((sp+i)->command, T_SCAN_SEND_PROGS, i);

  sp = scan_send_progs_out;
  for(i = 0; (sp+i)->command[0]; i++)
    add_u2w_hash((sp+i)->command, T_SCAN_SEND_PROGS_OUT, i);

  sps = scan_only_send_progs;
  for(i = 0; (sps+i)->command[0]; i++)
    add_u2w_hash((sps+i)->command, T_SCAN_ONLY_SEND_PROGS, i);

  upc = u2w_put_pre_http_commands;
  for(i = 0; (upc+i)->command[0]; i++)
    add_u2w_hash((upc+i)->command, T_U2W_PUT_PRE_HEADER, i);

  upc = u2w_put_header_commands;
  for(i = 0; (upc+i)->command[0]; i++)
    add_u2w_hash((upc+i)->command, T_U2W_PUT_HEADER, i);

  upc = u2w_put_http_header_commands;
  for(i = 0; (upc+i)->command[0]; i++)
    add_u2w_hash((upc+i)->command, T_U2W_PUT_HTTP_HEADER, i);

  upc = u2w_put_commands;
  for(i = 0; (upc+i)->command[0]; i++)
    add_u2w_hash((upc+i)->command, T_U2W_PUT, i);

  fc = format_commands;
  for(i = 0; (fc+i)->command[0]; i++)
    add_u2w_hash((fc+i)->command, T_FORMAT, i);

}
