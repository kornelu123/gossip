#ifndef GOSSIP_SERVER_H
#define GOSSIP_SERVER_H

#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct client_table{
  int sock_fd;
  uint32_t cur_client;
  struct pollfd client_fd[1024];
}client_table;

void *listener(void *ptr);

void *talker(void *ptr);

int main();

#endif
