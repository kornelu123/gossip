#ifndef USER_INTERFACE_H_
#define USER_INTERFACE_H_

#include <stdint.h>
#include "proto_cipa.h"

struct credentials{
  char uname[MAX_UNAME_LEN];
  char passwd[MAX_PASSWD_LEN];
};

struct box_coord{
  int x_start, x_end, y_start, y_end;
};

struct string_buf{
  int size_x;
  int size_y;
  int start_x;
  int start_y;
  char **buff;
  int count;
};

struct cursor{
  int x, y;
};

int init_screen();

void print_background();

char handle_login_screen(struct credentials *credent);

void draw_border(struct box_coord);

void print_fb_window(char *mess);

void print_home_screen();

struct string_buf *create_buff(struct box_coord coord, char *text);

void draw_message_box(struct box_coord coord, char *text);

void print_text(struct string_buf *text);

int count_lines(char *text, int size_x);

void free_buff(struct string_buf *buff);

void print_act_users(struct user_list *ulist, struct box_coord coord);

void draw_user_list(struct user_list *ulist, struct box_coord coord);

void draw_users_box(struct box_coord users_coord, struct user_list *ulist);

#endif
