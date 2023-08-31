#ifndef GOSSIP_CLIENT_H
#define GOSSIP_CLIENT_H
#include "proto_cipa.h"

struct credentials{
  char uname[MAX_UNAME_LEN];
  char passwd[MAX_PASSWD_LEN];
};

void get_credent(struct credentials *credent);

void set_terminal_properties();

void *con_recv(void *ptr);

void *con_send(void *ptr);

int main();

#endif
