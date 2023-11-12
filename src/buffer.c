#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

void init_buff(struct buff *buf){
    buf = (struct buff *)malloc(sizeof(struct buff));
    buf->first = NULL;
    buf->last =  NULL;
}

int pop(struct buff *buf, struct intern_pack **pack){

    if(buf->first == NULL || buf->last == NULL)
        return -1;
    
    *pack = realloc(*pack, sizeof(*buf->first->pack));
    memcpy(*pack, buf->first->pack, sizeof(*buf->first->pack));

    if(buf->first == buf->last){

        buf->first = NULL;
        buf->last = NULL;
        return 0;
    }

    buf->first = buf->first->next;
    return 0;
}

void push(struct buff *buf, struct intern_pack *pack){
    struct node *nd = malloc(sizeof(struct node));
    nd->pack = malloc(sizeof(*pack));
    memcpy(nd->pack, pack, sizeof(*pack));
    nd->next = NULL;

    if(buf->first == NULL || buf->last == NULL){

        buf->first = nd;
        buf->last = nd;
        return;
    }

    buf->last->next = nd;
    buf->last = nd;
}
