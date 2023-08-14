#include "proto_cipa.h"
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "database.h"

#define SUCCESFUL_LOG 0
#define ALREADY_LOGGED 1

struct user_list u_list;

void ulist_init(){
  u_list.cur_count = 0;
}

void parse_packet(struct cipa_packet *pack, int user_fd){
  uint8_t header = pack->header;
  char uname[MAX_UNAME_LEN];
  char passwd[MAX_PASSWD_LEN];
  int i;
  int k;
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
        add_user(uname, passwd);
        send(user_fd, "Registered succesfully \n", 25, 0);
      }else{
        send(user_fd, "This user already exists \n", 28, 0); 
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
        send(user_fd, "Username or password is incorrect \n",35, 0);
      }else{
        if(userlist_add(user_fd, uname) == ALREADY_LOGGED){
           send(user_fd, "User already logged \n", 21, 0);
        }else{
           send(user_fd, "Succesfully logged in \n", 23, 0);
        }
      }
      memset(&pack, 0 ,1024);
      break;
    default:
      break;
  }
}

int userlist_add(int user_fd, char *uname){
  for(int i=0; i < u_list.cur_count; i++){
    if(!(strcmp(uname, u_list.users[i].uname))){
      return ALREADY_LOGGED;
    }
  }
  strcpy(u_list.users[u_list.cur_count].uname, uname);
  u_list.users[u_list.cur_count++].user_fd = user_fd;
  return SUCCESFUL_LOG;
}

void userlist_remove(int user_fd){
  for(int i=0
}

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
