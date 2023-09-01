#ifndef STRING_PACKER_H_
#define STRING_PACKER_H_

#define BUFFER_OVERFLOW '\0'
#define BUFFER_UNDERFLOW '\0'
#define SUCCESS 0

#include <stdint.h>

struct ring_buf{
  char val[256];
  uint16_t last; 
  uint16_t counter;
  struct ring_buf *next;
};


int push(struct ring_buf *ring, char val);

char pop(struct ring_buf *ring);

void ring_clean(struct ring_buf *ring);

void node_add(struct ring_buf *src, struct ring_buf *dst){
  src->next = NULL;
  dst->next = src;
}

#endif
