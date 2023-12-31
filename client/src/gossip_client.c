#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
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
   }
  }
}

void *con_send(void *ptr){
  set_terminal_properties();
  int *sock_id = (int*)ptr; 
  char input = 0;  
  char out_buf[OUT_BUF_LENGTH];
  while(input != 'q'){
    input = fgetc(stdin);
    switch(input){
      case 'i':
	int i;
	printf("Please enter your message :\n");
	for(i=0 ; i<OUT_BUF_LENGTH ; i++){
	  out_buf[i] = fgetc(stdin);
	  if(out_buf[i] == '\n')break;
	}
	send(*sock_id, out_buf,sizeof(out_buf), 0); 
	break;
      default:
	break;
    }
  }
  exit(0);
}

int main(){
  printf("Enter username : \n");
  fgets( &uname[0], MAX_UNAME_LEN, stdin);
  printf("Enter password : \n");
  fgets( &passwd[0], MAX_PASSWD_LEN, stdin);
  printf("%s %s", uname, passwd);
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
