#ifndef GOSSIP_SERVER_H
#define GOSSIP_SERVER_H

#include "cipa_packet.h"
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct client_table{
  int sock_fd;
  uint32_t cur_client;
  struct pollfd client_fd[1024];
}client_table;

typedef struct pack_info{
    struct cipa_packet pack;
    int talker_fd, user_fd;
}

typedef struct pack_fifo{
};



void *get_in_addr(struct sockaddr *sa);

void add_to_pfds(struct poll *pfds[], int newfd, int *fd_count, int *fd_size);

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

int get_listener_socket(void);

void *listener(void *ptr);

void *talker(void *ptr);

int main();

#endif
