#ifndef GOSSIP_CLIENT_H
#define GOSSIP_CLIENT_H

void set_terminal_properties();

void *con_recv(void *ptr);

oid *con_send(void *ptr);

int main();

#endif
