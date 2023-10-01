#ifndef GOSSIP_SERVER_H
#define GOSSIP_SERVER_H

#include "proto_cipa.h"

int get_listener_socket();

void *receive_task();

int main();

void add_sort_buff(struct cipa_packet pack);

#endif
