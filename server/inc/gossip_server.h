#ifndef GOSSIP_SERVER_H
#define GOSSIP_SERVER_H

#include "proto_cipa.h"

int get_listener_socket();

void *handle_task();

int main();

void add_sort_buff(void *content, int user_fd);

#endif
