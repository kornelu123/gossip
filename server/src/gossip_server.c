#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "gossip_server.h"

#define SERVPORT "8080"
#define QUEQUELEN 10
#define CLIENTCOUNT 1024
#define BUFLEN CLIENTCOUNT

struct client_table cli_tab;
pthread_mutex_t cli_tab_mut;

int main(){
  struct addrinfo hints;
  struct addrinfo *servinfo;
  int status, sock_fd;
  pthread_t tid[2];

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

  if(( status = fcntl(sock_fd, F_SETFL, O_NONBLOCK)) == -1){
    fprintf(stderr, "fcntl error : %s \n", gai_strerror(status));
    exit(1);
  }

  pthread_create(&tid[0], NULL, listener, &cli_tab);
  pthread_create(&tid[1], NULL, talker, &cli_tab);

  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
  printf("Shutting down server \n");
  close(cli_tab.sock_fd);
  freeaddrinfo(servinfo);
  return 0;
}

void *listener(void *ptr){
    struct client_table *cli_tab = (struct client_table *)ptr;
    int res;
    struct sockaddr sock_addr;
    int  sock_fd;
    socklen_t socklen;
    pthread_mutex_lock(&cli_tab_mut);
    sock_fd = cli_tab->sock_fd;
    pthread_mutex_unlock(&cli_tab_mut);

    while(1){
      if((res = listen(sock_fd, QUEQUELEN)) == -1){
        fprintf(stderr, "listen error : %s \n", gai_strerror(res));
        exit(1); 
      }
      socklen = sizeof ( sock_addr );
      int res = accept(sock_fd, NULL, NULL);
      if((res == -1) && (errno & (EAGAIN | EWOULDBLOCK) )){
        if(errno & (EAGAIN | EWOULDBLOCK)) continue;
        fprintf(stderr, "accept error : %s \n", gai_strerror(res));
        exit(1);
      }
      printf("connected from fd : %d", res);
      pthread_mutex_lock(&cli_tab_mut);
      cli_tab->client_fd[cli_tab->cur_client].fd = res;
      cli_tab->client_fd[cli_tab->cur_client++].events = POLLIN ;
      pthread_mutex_unlock(&cli_tab_mut);
    }
  }

void *talker(void *ptr){
  char in_buf[BUFLEN];
  struct client_table *cli_tab = (struct client_table*)ptr;
  int sock_fd;
  int curr_count, status;
  struct pollfd *poll_tab;
  poll_tab = (struct pollfd *)calloc(1, sizeof(struct pollfd));
  pthread_mutex_lock(&cli_tab_mut);
  sock_fd = cli_tab->sock_fd;
  pthread_mutex_unlock(&cli_tab_mut);
 
  while(1){
    pthread_mutex_lock(&cli_tab_mut);
    curr_count = cli_tab->cur_client;
    poll_tab = realloc(poll_tab, curr_count*(sizeof (struct pollfd)));
    for(int i=0;i<curr_count;i++){
      poll_tab[i] = cli_tab->client_fd[curr_count];
    }
    pthread_mutex_unlock(&cli_tab_mut);

    status = poll(poll_tab, curr_count, 25);
    if(status == -1){
      fprintf(stderr, "poll error : %s \n", gai_strerror(status));
      exit(1);
    }
    if(status == 0){
      break;
    }
    int rec_st;
    for(int i=0 ;i < curr_count ;i++){ 
      if(poll_tab[i].revents & POLLIN){
        status--;
        rec_st = recv(poll_tab[i].fd, in_buf, BUFLEN, 0);
        if(!rec_st){
          pthread_mutex_lock(&cli_tab_mut);
          for(int n = i; n<curr_count; n++){
            cli_tab->client_fd[curr_count] = cli_tab->client_fd[curr_count+1];
          }
          cli_tab->cur_client--;
          pthread_mutex_unlock(&cli_tab_mut);
        }
        printf("in buf : %s \n", in_buf);
        if(!status) break;
      }
    }
  }
}
