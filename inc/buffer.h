#ifndef BUFFER_H_
#define BUFFER_H_

#include "proto_cipa.h"

struct node{
    struct intern_pack *pack;
    struct node        *next;
};

struct buff{
    struct node       *first;
    struct node        *last;
};


void init_buff(struct buff *buf);

int pop(struct buff *buf, struct intern_pack **pack);

void push(struct buff *buf, struct intern_pack *pack);

#endif
