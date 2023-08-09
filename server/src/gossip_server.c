#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gossip_server.h"

#define SERVPORT "8080"
#define QUEQUELEN 10

void *listener(void *ptr);

struct client_table cli_tab;

int main(){
  struct addrinfo hints;
  struct addrinfo *servinfo;
  int status, sock_fd;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, SERVPORT, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo error: %s \n", gai_strerror(status));
    exit(1);
  }

  if((sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
    fprintf(stderr, "socket error : %s \n",gai_strerror(sock_fd));
    exit(1);
  }

  if(( status = bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen)) == -1){
    fprintf(stderr, "bind error : %s \n", gai_strerror(status));
    exit(1);
  }
  cli_tab.sock_fd = sock_fd;
  cli_tab.cur_client = 0;

  freeaddrinfo(servinfo);
  return 0;
}

void *listener(void *ptr){
    struct client_table *cli_tab = (struct client_table *)ptr;
    int res;

    while(1){
      if((res = listen(cli_tab->sock_fd, QUEQUELEN)) == -1){
        fprintf(stderr, "listen error : %s \n", gai_strerror(res));
        exit(1); 
      }
    }
  }
