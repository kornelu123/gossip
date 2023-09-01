#include "string_packer.h"
#include <stdlib.h>
#include <string.h>

char pop(struct ring_buf *ring){
  if(ring->last == ring->first){
    return BUFFER_UNDERFLOW;
  }
  char res = ring->val[ring->first];
  ring->val[ring->first++] = ' ';
  ring->first %= ring->counter;
  return res;
}

void ring_clean(struct ring_buf *ring){
  ring->last = 0;
  memset(ring->val, " ", ring->counter);
}

int push(struct ring_buf *ring, char val){
  if(ring->first + 1 == ring->last){
    return BUFFER_OVERFLOW;
  }
  ring->val[ring->last++ % ring->counter] = val;
  return SUCCESS;
}

struct ring_buf *buffer_text(char *text, int delta_x){
  struct ring_buf *buff_text;
  int count = 1;
  struct ring_buf *cur_buf = malloc(count * (sizeof struct ring_buf);
  cur_buf->counter = delta_x;
  for(int i=0;text[i]!= '\0'){
    buff_text->buf
    if(text%delta_x == delta_x-1){
      count++;
      realloc(buff_text, count *(sizeof struct buf_node);
    }
  }
}
