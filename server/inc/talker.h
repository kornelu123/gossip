#ifndef TALKER_H
#define TALKER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
  int sock_id,
  int client_id,
  sockaddr_in serv_addr,
  sockaddr_in cli_addr
}talker;

#endif
