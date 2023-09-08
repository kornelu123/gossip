#ifndef GOSSIP_CLIENT_H
#define GOSSIP_CLIENT_H
#include "proto_cipa.h"

void set_terminal_properties();

void *con_recv(void *ptr);

void *con_send(void *ptr);

int main();

#endif
