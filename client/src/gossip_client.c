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
#include "gossip_client.h"
#include "database.h"

#define IN_BUF_LENGTH 1024


struct credentials credent;

void get_credent(struct credentials *credent){
    printf("Type in username: \n");
    fgets(credent->uname, MAX_UNAME_LEN, stdin);
    printf("Type in password: \n");
    fgets(credent->passwd, MAX_PASSWD_LEN, stdin);
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
  ssize_t res;
   if((res = recv(*sock_id, &in_buf, IN_BUF_LENGTH, 0)) > 0){
     for(int i=0; i<res; i++){
        printf("%02X:", in_buf[i]);
     }
     printf("\n");
     memset(in_buf, 0, IN_BUF_LENGTH);
   }
  }
}

void *con_send(void *ptr){
    set_terminal_properties();
    int *sock_id = (int*)ptr; 
    char input = 0;  
    while(input != 'q'){
        input = fgetc(stdin);
        switch(input){
            case 'r':{
                uint8_t head = H_REG;
                make_and_send_pack(*sock_id, head, (void *)&credent);
    	        break;
            }
            case 'l':{
                uint8_t head = H_LOG;
                make_and_send_pack(*sock_id, head, (void *)&credent);
                break;
            }
            case 'd':{
                uint8_t head = H_DEL;
                make_and_send_pack(*sock_id, head, (void *)&credent);
                break;
            }
            default:
                break;
        }
    }
    exit(0);
}

int main(){
  get_credent(&credent);
  char* addr = "127.0.0.1";
  int sock_id = socket(AF_INET ,SOCK_STREAM , IPPROTO_TCP);
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(44444); 
  serv_addr.sin_addr.s_addr = inet_addr(addr);

  if(connect(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr))){
    return -1;
  }
   
  
  pthread_t transmiter;
  pthread_t receiver;

  int iret1 = pthread_create(&transmiter, NULL, con_send, (void *)&sock_id);
  int iret2 = pthread_create(&receiver,  NULL, con_recv, (void *)&sock_id);

  pthread_join(transmiter, NULL);
  pthread_join(receiver,   NULL);
  close(sock_id);
  return 0;
}
