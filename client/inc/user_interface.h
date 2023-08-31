#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

struct box_coord{
  int x_start, x_end, y_start, y_end;
};


int init_screen();

void print_background();

void draw_border(struct box_coord);

int print_text(char *text, struct box_coord coord);

void print_input_credentials();

#endif
