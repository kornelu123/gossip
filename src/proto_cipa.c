#include "proto_cipa.h"
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>

char *db_file = "users.txt";

struct user users[1024];

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

struct cipa_packet login_pack(char *uname, char *passwd){
  struct cipa_packet pack;
  memset(&pack, 0, 1024);
  pack.header = H_LOGIN;

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

void parse_packet(struct cipa_packet *pack, int user_fd){
  uint8_t header = pack->header;
  char uname[MAX_UNAME_LEN];
  char passwd[MAX_PASSWD_LEN];
  int i;
  int k;
  FILE* db_app  = fopen(db_file,"a+");
  switch(header){
    case H_REG:
      i=0;
      k=0;
      printf("Got register header\n");
      while(pack->content[i] != '\n'){
        uname[i] = pack->content[i];
        i++;
      }
      i++;
      while(pack->content[i] != '\0'){
        passwd[k] = pack->content[i];
        k++;i++;
      }
      if(search_db(uname, passwd)){
        fprintf(db_app,"user:%s;passwd:%s",uname,passwd);
        fflush(db_app);
      }else{
        send(user_fd, "This user already exists\n", 26, 0); 
      }
      memset(&pack, 0 ,1024);
      break;
    case H_LOGIN:
      i=0;
      k=0;
      printf("Got login header\n");
      while(pack->content[i] != '\n'){
        uname[i] = pack->content[i];
        i++;
      }
      i++;
      while(pack->content[i] != '\0'){
        passwd[k] = pack->content[i];
        k++;i++;
      }
      if(search_db(uname, passwd)){
        send(user_fd, "Username or password is incorrect",33, 0);
      }else{
        
      }
      memset(&pack, 0 ,1024);
      
    default:
      break;
  }
}

int search_db(char *uname, char *passwd){
  FILE* db_read = fopen(db_file,"r");
  if(db_read == NULL){
    fprintf(stderr, "fopen : %s", gai_strerror(db_file));
    exit(1);
  }
  char* lookup = malloc(( sizeof (char))*1024);
  sprintf(lookup, "user:%s;passwd:%s", uname, passwd);
  char *lineptr = NULL;
  size_t size = 0;
  ssize_t chars;
  while((chars = getline(&lineptr, &size, db_read)) >= 0){
    if(!(strcmp(lookup,lineptr))){
      return 0;
    }
  }
  return 1;
}
