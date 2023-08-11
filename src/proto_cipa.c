#include "proto_cipa.h"
#include <string.h>
#include <stdio.h>

char *db_file = "users.txt";

struct cipa_packet register_pack(char *uname, char *passwd){
  struct cipa_packet pack;
  memset(&pack, 0, 1024);
  pack.header = H_REG;

  int i;
  for(i=0; uname[i] != '\0'; i++){
    pack.content[i] = uname[i];
  }

  for(int k=0; passwd[k] != '\0'; i++,k++){
    pack.content[i] = passwd[k];
  }
  pack.content[i++] = '\0';

  return pack;
}

void parse_packet(struct cipa_packet *pack){
  uint8_t header = pack->header;
  switch(header){
    case H_REG:
      printf("Got register header\n");
      int i = 0;
      FILE* db_app  = fopen(db_file,"a+");
      FILE* db_read = fopen(db_file,"r");
      char uname[MAX_UNAME_LEN];
      char passwd[MAX_PASSWD_LEN];
      while(pack->content[i] != '\n'){
        uname[i] = pack->content[i++];
      }
      i++;
      int k=0;
      while(pack->content[i] != '\0'){
        passwd[k++] = pack->content[i++];
      }
      printf("cwel\n");
      fprintf(db_app,"user:%s;passwd:%s",uname,passwd);
      fflush(db_app);
      memset(&pack, 0 ,1024);
      break;
    default:
      return;
  }
}
