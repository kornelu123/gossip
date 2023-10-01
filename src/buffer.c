#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

void init_buff(struct buff *buf){

    buf = malloc(sizeof (buf));
    buf->first = malloc(sizeof(buf->first));
    buf->last =  malloc(sizeof(buf->last));
}

int pop(struct buff *buf, void *content){

    if(buf->first == NULL || buf->last == NULL)
        return -1;

    memset(content, 0, buf->first->size);
    memcpy(content, buf->first->content, buf->first->size);
    if(buf->first == buf->last){

        buf->first = NULL;
        buf->last = NULL;
        return 0;
    }

    buf->first->next = buf->first->next->next;
    buf->first = buf->first->next;
    return 0;
}

void push(struct buff *buf, void *content, int size){

    struct node *nd = malloc(sizeof(nd));
    nd->content = malloc(size);
    memcpy(nd->content, content, size);
    nd->size = size;
    nd->next = NULL;
    if(buf->first == NULL || buf->last == NULL){

        buf->first = nd;
        buf->last = buf->first;
        buf->last->next = NULL;
        return;
    }

    buf->last->next = nd;
    buf->last = nd;
}
