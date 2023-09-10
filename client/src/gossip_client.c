#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <termios.h>
#include <ncurses.h>
#include "proto_cipa.h"
#include "gossip_client.h"
#include "user_interface.h"

#define SIZE 1024
#define OUT_BUF_LENGTH 1024
#define IN_BUF_LENGTH 1024
#define MAX_MESS_LENGTH MAX_PASSWD_LEN

struct credentials credent;

void handle_login(int sock_id){
  init_screen();
  char inp;
  int res;
  int err = 0;
  clear();
  do{
    echo();
    inp = handle_login_screen(&credent);
    noecho();
    if(inp == 'r'){
      struct cipa_packet pack = register_pack(credent.uname, credent.passwd);
      if((res = send(sock_id, &pack, sizeof(pack), 0) < 0)){
        fprintf(stderr, "send error : %s \n", gai_strerror(res));
        exit(1);
      }
      memset(&pack, 0 , sizeof(pack));
      recv(sock_id, &pack, IN_BUF_LENGTH, 0);
      clear();
      print_fb_window(pack.content);
      wgetch(stdscr);
      memset(&pack, 0, sizeof(pack));
    }else if(inp == 'l'){
      struct cipa_packet pack = login_pack(credent.uname, credent.passwd); 
      if(res = send(sock_id, &pack, sizeof(pack), 0) < 0 ){
        fprintf(stderr, "send error : %s \n", gai_strerror(res));
        exit(1);
      }
      memset(&pack, 0 , sizeof(pack));
      recv(sock_id, &pack, sizeof(pack), 0);
      clear();
      if(pack.header == SUCCESSFUL){
        clear();
        print_fb_window("Succesfull login");
        refresh();
        wgetch(stdscr);
        break;
      }
      print_fb_window(pack.content);
      wgetch(stdscr);
      memset(&pack, 0, sizeof(pack));
    }else{
      clear();
      print_fb_window("Not a valid input");
      refresh();
      wgetch(stdscr);
    }
  }while(1);
}

void get_credent(struct credentials *credent){
  char uname[MAX_UNAME_LEN];
  char passwd[MAX_PASSWD_LEN];
  printf("Insert username : \n");
  fgets(uname, MAX_UNAME_LEN, stdin);
  printf("Insert password : \n");
  fgets(passwd, MAX_PASSWD_LEN, stdin);
  memcpy(credent->uname, uname, MAX_UNAME_LEN);
  memcpy(credent->passwd, passwd, MAX_UNAME_LEN);
}

void set_terminal_properties(){
  static struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void *con_recv(void *ptr){
  int *sock_id = (int *)ptr;
  while(1){
  char in_buf[IN_BUF_LENGTH];
   if(recv(*sock_id, &in_buf, IN_BUF_LENGTH, 0) > 0){
     printf("%s", in_buf);
     memset(in_buf, 0, IN_BUF_LENGTH);
   }
  }
}

void *con_send(void *ptr){
  set_terminal_properties();
  int *sock_id = (int*)ptr; 
  struct cipa_packet pack;
  char input = 0;  
  int res;
  char out_buf[OUT_BUF_LENGTH];
  while(input != 'q'){
    input = fgetc(stdin);
    switch(input){
      case 'i':
        int i;
        char mess[MAX_MESS_LENGTH];
        fgets(mess, MAX_MESS_LENGTH, stdin);
        printf("%s", mess);
        pack = mess_pack(mess);
        printf("Ended getting input\n");
        send(*sock_id, &pack,sizeof(pack), 0); 
        break;
      case 'r':
        pack = register_pack(credent.uname, credent.passwd);
        if(res = send(*sock_id, &pack, sizeof(pack), 0) < 0 ){
           fprintf(stderr, "send error : %s \n", gai_strerror(res));
           exit(1);
        }
	     break;
      case 'l':
        pack = login_pack(credent.uname, credent.passwd); 
        if(res = send(*sock_id, &pack, sizeof(pack), 0) < 0 ){
           fprintf(stderr, "send error : %s \n", gai_strerror(res));
           exit(1);
        }
        break;
      case 'c':
        printf("Type in user you want to connect to : \n");
        char inp;
        char uname[MAX_UNAME_LEN];
        for(int i=0;i< OUT_BUF_LENGTH;i++){
           inp = fgetc(stdin);
           uname[i] = inp;
           if(inp == '\n') break;
        }
        pack = connect_pack(uname);
        if(res = send(*sock_id, &pack, sizeof(pack), 0) < 0){
           fprintf(stderr, "send error : %s \n", gai_strerror(res));
           exit(1);
        }
        break;
      case 'd':
        pack = disconn_pack();
        if(res = send(*sock_id, &pack, sizeof(pack), 0) < 0){
           fprintf(stderr, "send error : %s \n", gai_strerror(res));
           exit(1);
        }
        break;
      default:
	      break;
    }
    memset(&pack, 0, 1024);
  }
  exit(0);
}

int main(){
  char* addr = "127.0.0.1";
  int sock_id = socket(AF_INET ,SOCK_STREAM , IPPROTO_TCP);
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(8080); 
  serv_addr.sin_addr.s_addr = inet_addr(addr);

  if(connect(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
    return -1;
  }

  handle_login(sock_id);

  pthread_t transmiter;
  pthread_t receiver;
  int iret1, iret2;
  
  iret1 = pthread_create(&transmiter, NULL, con_send, (void *)&sock_id);
  iret2 = pthread_create(&receiver,  NULL, con_recv, (void *)&sock_id);

  pthread_join(transmiter, NULL);
  pthread_join(receiver,   NULL);
  close(sock_id);
  return 0;
}
