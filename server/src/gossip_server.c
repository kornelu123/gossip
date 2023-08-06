#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <poll.h>

#define SERV_SIZE 1024

int *create_socket();
 
void *pthread_routine(void* ptr);

char *msg = "Kurwa \n";

int client_sock[11];

int main(){
  int server_socket = create_socket();

  
  close(server_socket);
  return 0;
}

int *create_socket(){
  int socket_id;
  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(8080);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  if((socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
    perror("Socket error ");
    exit(1);
  }
  
  if(bind(socket_id, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
    perror("Bind error ");
    exit(1);
  }

  if(listen(socket_id, 5) == -1){
    perror("Listen error ");
    exit(1);
  }
  printf("Server started \n"); 
  return &socket_id;
}

void *poll_clients(void* ptr){
  int *new_socket_id = (int *)ptr;
  free(ptr);

  struct pollfd pollfds[11];
  pollfds[0].fd = *new_socket_id;
  pollfds[0].events = POLLIN | POLLPRI;
  int used_clients = 0;

  while(1){
     int pool_res = poll(pollfds, used_clients +1, 500);
     if(pool_res > 0){
       if(pollfds[0].revents & POLLIN){
	 struct sockaddr_in cliaddr;
	 int addrlen = sizeof(cliaddr);
	 int client_socket = 
  	}

  close(new_socket_id);
  return NULL;
}
