#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/epoll.h>

#define SERV_SIZE 1024
#define MAX_EVENTS 10

int socket_id;
struct sockaddr_in serv_addr;
int serv_addr_len;

char in_buf[1024];

int create_socket();
 
char *msg = "Kurwa \n";

int main(){
  int server_socket, conn_sock, nfds, epollfd;
  server_socket = create_socket();
  struct epoll_event ev, events[MAX_EVENTS];
  serv_addr_len = sizeof(serv_addr);

  epollfd = epoll_create1(0);
  if(epollfd == -1){
    perror("epoll_create1");
    exit(1);
  }

  ev.events = EPOLLIN;
  ev.data.fd = server_socket;

  if(epoll_ctl(epollfd, EPOLL_CTL_ADD, server_socket, &ev) == -1){
    perror("epoll_ctl : listen_sock");
    exit(1);
  }

  for(;;){
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if(nfds == -1){
      perror("epoll_wait");
      exit(1);
    }
    for(int n=0; n<nfds; ++n){
      if(events[n].data.fd == server_socket){
        conn_sock = accept(server_socket, ( struct sockaddr *) &serv_addr, 
           (socklen_t *)&serv_addr_len);
        if(conn_sock == -1){
          perror("accept");
          exit(1);
        }

        int status = fcntl(conn_sock, F_SETFL, fcntl(socket_id, F_GETFL, 0) | O_NONBLOCK);
        if(status == -1){
          perror("fctnl");
          exit(1); 
        }
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = conn_sock;
        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
              &ev) == -1){
          perror("epoll_ctl: conn_sock");
          exit(1);
        }
      } else {
	if(read(conn_sock, in_buf, 1024) > 0){
	  write(conn_sock, in_buf, 1024);
	  printf("%d : %s", conn_sock ,in_buf);
	}
      }
    }
  }
  
  close(server_socket);
  return 0;
}

int create_socket(){
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
  return socket_id;
}
