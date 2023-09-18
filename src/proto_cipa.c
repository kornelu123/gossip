#include "proto_cipa.h"
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "database.h"

#define SUCCESFUL_LOG 0
#define ALREADY_LOGGED 1

#define SUCCESFUL_CONN 0
#define CONN_ERROR 1
#define USER_NOT_FOUND 2

struct user_list u_list;

void ulist_init(){
  u_list.cur_count = 0;
  for(int i=0;i<1024;i++){
    u_list.users[i].talker_fd = 0;
  }
}

void parse_packet(struct cipa_packet *pack, int user_fd){
  uint8_t header = pack->header;
  char uname[MAX_UNAME_LEN];
  char passwd[MAX_PASSWD_LEN];
  int i ;
  int k ;
  switch(header){
    case H_REG:
      memset(uname, 0, MAX_UNAME_LEN);
      memset(passwd, 0, MAX_PASSWD_LEN);
      i=0;
      k=0;
      while(pack->content[i] != '\0'){
        uname[i] = pack->content[i];
        i++;
      }
      i++;
      while(pack->content[i] != '\0'){
        passwd[k] = pack->content[i];
        k++;i++;
      }
      if(search_db(uname, passwd) == 1){
        struct cipa_packet r_pack;
        add_user(uname, passwd);
        memset(&r_pack, 0, sizeof(r_pack));
        r_pack = success_pack("Registered succesfully \n");
        send(user_fd,&r_pack, sizeof(r_pack), 0);
      }else{
        struct cipa_packet r_pack;
        memset(&r_pack, 0, sizeof(r_pack));
        r_pack = failed_pack("This user already exists \n");
        send(user_fd, &r_pack , sizeof(r_pack), 0); 
      }
      break;
    case H_LOGIN:
      i=0;
      k=0;
      while(pack->content[i] != '\0'){
        uname[i] = pack->content[i];
        i++;
      }
      uname[i++] = '\0';
      while(pack->content[i] != '\0'){
        passwd[k] = pack->content[i];
        k++;i++;
      }
      passwd[++k]  = '\0';
      if(search_db(uname, passwd) != 0){
        struct cipa_packet r_pack;
        memset(&r_pack, 0, sizeof(r_pack));
        r_pack = failed_pack("Username or password is incorrect ");
        send(user_fd, &r_pack, sizeof(r_pack), 0);
      }else{
        if(userlist_add(user_fd, uname) == ALREADY_LOGGED){
          struct cipa_packet r_pack;
          memset(&r_pack, 0, sizeof(r_pack));
          r_pack = failed_pack("User already logged ");
          send(user_fd,&r_pack, sizeof(r_pack) , 0);
        }else{
          struct cipa_packet r_pack;
          memset(&r_pack, 0, sizeof(r_pack));
          r_pack = success_pack("Succesfully logged in ");
          send(user_fd,&r_pack, sizeof(r_pack) , 0);
        }
      }
      break;
    case H_CONN:
      while(pack->content[i] != '\n'){
        uname[i] = pack->content[i];
        i++;
      }
      int res;
      res = handle_conn(user_fd, uname);
      if(res == SUCCESFUL_CONN){
        send(user_fd, "Connection succesfull \n", 23, 0); 
      }
      if(res == USER_NOT_FOUND){
        send(user_fd, "User not found \n", 16, 0);
      }
      break;
    case H_MESS:
      handle_mess(user_fd, pack->content);
      break;
    case H_DISCONN:
      handle_disconn(user_fd);
      break;
    case H_REQ_ACT:
      printf("%s", "Handling\n");
      int count = 0;
      do{
        *pack = active_u_pack(u_list, &count);
        send(user_fd, &pack, sizeof(pack), 0);
      }while(pack->header != H_ANS_ACT_EN);
      break;
    default:
      break;
  }
  memset(pack, 0, 1024);
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
  int i;
  for(i=0; i<u_list.cur_count;i++){
    if(u_list.users[i].user_fd == user_fd) break;
  }
  u_list.users[i] = u_list.users[--u_list.cur_count];
}

int handle_disconn(int user_fd){
  int i;
  for(i=0;i<u_list.cur_count;i++){
    if(u_list.users[i].user_fd == user_fd) break;
  }
  if(i == u_list.cur_count) return -1;

  int j;
  for( j=0;j<u_list.cur_count;j++){
    if(u_list.users[i].talker_fd == u_list.users[j].user_fd) break;
  }

  if(j == u_list.cur_count) return -1;
  u_list.users[i].talker_fd =  0;
  memset(u_list.users[i].tname, 0, MAX_UNAME_LEN);
  u_list.users[j].talker_fd =  0;
  memset(u_list.users[j].tname, 0, MAX_UNAME_LEN);

  return 0;
}

int handle_conn(int user_fd, char*uname){
  int i;
  for(i=0;i<u_list.cur_count;i++){
    if(u_list.users[i].user_fd == user_fd) break;
  }
  for(int j=0;j<u_list.cur_count;j++){
    if(!strcmp(uname, u_list.users[j].uname)){
      u_list.users[i].talker_fd = u_list.users[j].user_fd;
      u_list.users[j].talker_fd = u_list.users[i].user_fd;
      char mess[1024];
      sprintf(mess, "Connected from : %s", u_list.users[i].uname);
      send(u_list.users[i].talker_fd, mess, sizeof(mess), 0);
      return SUCCESFUL_CONN;
    }
  }
  return USER_NOT_FOUND;
}

int handle_mess(int user_fd, char* mess){
  for(int i=0;i<u_list.cur_count;i++){
    if(u_list.users[i].user_fd == user_fd){
      if(u_list.users[i].talker_fd == 0){
        send(u_list.users[i].talker_fd, "You're not connected to talker, please connect with 'c' to proceed \n", 68, 0);
        send(u_list.users[i].user_fd, "You're not connected to talker, please connect with 'c' to proceed \n", 68, 0);
        return 1;
      }
      printf("user_fd : %d\n talker_fd : %d\n",u_list.users[i].user_fd, u_list.users[i].talker_fd);
      send(u_list.users[i].talker_fd, mess, 1024, 0);
      return 0;
    }
  }
  return 2;
}

struct cipa_packet register_pack(char *uname, char *passwd){
  struct cipa_packet pack;
  memset(&pack, 0, 1024);
  pack.header = H_REG;

  int i;
  for(i=0; uname[i] != '\0'; i++){
    pack.content[i] = uname[i];
  }
  pack.content[i++] = '\0';

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
  pack.content[i++] = '\0';

  for(int k=0; passwd[k] != '\0'; i++,k++){
    pack.content[i] = passwd[k];
  }

  pack.content[i++] = '\0';

  return pack;
}

struct cipa_packet connect_pack(char *uname){
  struct cipa_packet pack;
  memset(&pack, 0 , 1024);
  pack.header = H_CONN;

  int i=0;
  do{
    pack.content[i] = uname[i];
  }while(uname[i++] !=  '\n');

  return pack;
}

struct cipa_packet disconn_pack(){
  struct cipa_packet pack;
  memset(&pack, 0, 1024);
  pack.header = H_DISCONN;

  return pack;
}

struct cipa_packet mess_pack(char *mess){
  struct cipa_packet pack;
  memset(&pack, 0, 1024);
  pack.header = H_MESS;

  int i=0;
  do{
    pack.content[i] = mess[i];
    i++;
  }while(pack.content[i] != '\0');
  pack.content[i++] = '\0';

  return pack;
}

struct cipa_packet success_pack(char *mess){
  struct cipa_packet pack;
  pack.header = SUCCESSFUL;
  int i=0;
  do{
    pack.content[i] = mess[i];
    i++;
  }while(pack.content[i] != '\0');

  return pack;
}

struct cipa_packet failed_pack(char *mess){
  struct cipa_packet pack;
  pack.header = FAILED;
  int i=0;
  do{
    pack.content[i] = mess[i];
    i++;
  }while(pack.content[i] != '\0');

  return pack;
}

struct cipa_packet active_u_pack(struct user_list ulist, int *count){
  struct cipa_packet pack;
  int j =0;
  for(int i=*count;i<ulist.cur_count;i++){
    int ulen = strlen(ulist.users[i].uname);
    if((j + ulen +1)> 1023 ) {
      pack.content[j++] ='\0';
      *count = i;
      pack.header = H_ANS_ACT_NE;
      return pack;
    }
    for(int k=0;k< ulen && j< 1023;k++, j++){
      pack.content[j] = ulist.users[i].uname[k];
    }
    pack.content[j++] = '\0';
  }
  pack.header = H_ANS_ACT_EN;
  *count =0;
  return pack;
}

struct cipa_packet request_u_pack(){
  struct cipa_packet pack;
  pack.header = H_REQ_ACT;
  return pack;
}

int handle_active_u_pack(struct user_list *ulist, int server_fd){
    struct cipa_packet pack = request_u_pack();
    send(server_fd, &pack, sizeof(pack), 0); 
    char uname_temp[MAX_UNAME_LEN];
    int k=0;
    int i;
    do{
      recv(server_fd, &pack, sizeof(pack), 0);
      i=0;
      while(i < 1023){
        uname_temp[k++] = pack.content[i];
        if(pack.content[i++] == '\0'){
           k=0;
           strcpy(uname_temp, ulist->users[ulist->cur_count++].uname);
        }
      }
    }while(pack.header != H_ANS_ACT_EN);
    return ulist->cur_count;
}
