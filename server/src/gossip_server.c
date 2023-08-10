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
#define CLIENTCOUNT 1024

struct client_table cli_tab;
pthread_mutex_t cli_tab_mut;

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

  if(( status = pthread_mutex_init(&cli_tab_mut, NULL)) != 0){
    fprintf(stderr, "bind error : %s \n", gai_strerror(status));
    exit(1);
  } 

  freeaddrinfo(servinfo);
  return 0;
}

void *listener(void *ptr){
    struct client_table *cli_tab = (struct client_table *)ptr;
    int res;
    struct sockaddr sock_addr;
    int cli_fd, sock_fd;
    pthread_mutex_lock(&cli_tab_mut);
    sock_fd = cli_tab->sock_fd;
    pthread_mutex_unlock(&cli_tab_mut);

    while(1){
      if((res = listen(sock_fd, QUEQUELEN)) == -1){
        fprintf(stderr, "listen error : %s \n", gai_strerror(res));
        exit(1); 
      }
      int res = accept(sock_fd, (struct sockaddr*) &sock_addr, (socklen_t *)sizeof sock_addr);
      if(res == -1){
	fprintf(stderr, "accept error : %s \n", gai_strerror(res));
	exit(1);
      }
      pthread_mutex_lock(&cli_tab_mut);
      cli_tab->client_fd[cli_tab->cur_client].fd = res;
      cli_tab->client_fd[cli_tab->cur_client++].event = POLLIN ;
      pthread_mutex_unlock(&cli_tab_mut);
    }
  }

void *talker(void *ptr){
  struct client_table *cli_tab = (struct client_table*)ptr;
  int sock_fd;
  int curr_count, status;
  struct client_table cli_tab_cpy;
  pollfd *poll_tab;
  poll_tab = (pollfd *)calloc(1, sizeof(pollfd));
  pthread_mutex_lock(&cli_tab_mut);
  sock_fd = cli_tab->sock_fd;
  pthread_mutex_unlock(&cli_tab_mut);
 
  while(1){
    pthread_mutex_lock(&cli_tab_mut);
    curr_count = cli_tab->cur_client;
    poll_tab = realloc(poll_tab, curr_count*(sizeof pollfd));
    for(int i=0;i<curr_count;i++){
      poll_tab[i] = cli_tab->client_fd[curr_count];
    }
    pthread_mutex_unlock(&cli_tab_mut);

    status = poll(poll_tab, 1, 25);
    if(status == -1){
      fprintf(stderr, "poll error : %s \n", gai_strerror(res));
      exit(1);
    }
    if(status == 0){
      break;
    }

    for(int i=0 ;i < status ;i++){ 
      
    }

  }


  poll(
}
