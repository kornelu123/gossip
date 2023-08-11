#include "proto_cipa.h"
#include <string.h>

struct cipa_packet register_pack(char *uname, char *passwd){
  struct cipa_packet pack;
  pack.header = H_REG;

  int i;
  for(i=0; uname[i] != '\0'; i++){
    pack.content[i] = uname[i];
  }
  pack.content[i++] = '\0';

  for(; passwd[i] != '\0'; i++){
    pack.content[i] = passwd[i];
  }

  return pack;
}


void parse_packet(cipa_packet pack){
  uint8_t header = pack.header;
  printf("header : %x \n;", pack.header);
  printf("pack_content : %s \n", pack.content);
}
