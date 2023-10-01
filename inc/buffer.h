#ifndef BUFFER_H_
#define BUFFER_H_

struct node{
    void *content;
    struct node *next;
    int size;
};

struct buff{
    struct node *first;
    struct node *last;
};


void init_buff(struct buff *buf);

int pop(struct buff *buf, void *content);

void push(struct buff *buf, void *content, int size);

#endif
