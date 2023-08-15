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
#include "proto_cipa.h"

#define SIZE 1024
#define OUT_BUF_LENGTH 1024
#define IN_BUF_LENGTH 1024

char uname[MAX_UNAME_LEN];
char passwd[MAX_PASSWD_LEN];

void set_terminal_properties(){
  static struct termios oldt, newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void *con_recv(void *ptr){
  int *sock_id = (int *)ptr;
  char in_buf[IN_BUF_LENGTH];
  while(1){
   if(recv(*sock_id, &in_buf, IN_BUF_LENGTH, 0) > 0){
     printf("%s", in_buf);
     memset(&in_buf, 0, IN_BUF_LENGTH);
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
        for(i=0 ; i<OUT_BUF_LENGTH ; i++){
           pack.content[i] = fgetc(stdin);
           if(pack.content[i] == '\n')break;
        }
        pack.header = H_MESS;
        send(*sock_id, &pack,sizeof(pack), 0); 
        break;
      case 'r':
        pack = register_pack(uname, passwd);
        if(res = send(*sock_id, &pack, sizeof(pack), 0) < 0 ){
           fprintf(stderr, "send error : %s \n", gai_strerror(res));
           exit(1);
        }
	     break;
      case 'l':
        pack = login_pack(uname, passwd); 
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
        for(int i=0;i<1023;i++){
          printf("%c",pack.content[i]);
        }
        if(res = send(*sock_id, &pack, sizeof(pack), 0) < 0){
           fprintf(stderr, "send error : %s \n", gai_strerror(res));
           exit(1);
        }
        break;
      default:
	      break;
    }
  }
  exit(0);
}

int main(){
  char inp = 0;
  int count =0;
  printf("Enter username : ");
  do{
    inp = fgetc(stdin);
    uname[count++] = inp;
  }while(inp != '\n');
  count = 0;
  printf("\nEnter password : ");
  do{
    inp = fgetc(stdin);
    passwd[count++] = inp;
  }while(inp != '\n');
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
