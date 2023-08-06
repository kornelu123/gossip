#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

void start_serv();

int main(){
  start_serv();

  return 0;
}

void start_serv(){
  int sock_id ;
  int client_id;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  socklen_t addr_size;
  int cli_len = sizeof(cli_addr);
  char in_buf[1024];
  char ping;

  sock_id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(8080);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  bind(sock_id, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  while(listen(sock_id, 1024));

  client_id = accept(sock_id, (struct sockaddr *)&cli_addr, &cli_len);

  while(1){
    if(read(client_id, &in_buf, 1024)){
      printf("%s",in_buf);
    }
  }
  close(sock_id);
}

