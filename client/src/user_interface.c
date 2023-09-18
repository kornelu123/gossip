#include <ncurses.h>
#include <stdlib.h>
#include <menu.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include "user_interface.h"
#include "proto_cipa.h"

#define RED_BLUE 1

char *credential_mess = "Please insert your login credentials\nLogin : \nPassword :";

struct cipa_packet cipack;

struct box_coord win_coord;
struct box_coord log_coord;
struct box_coord users_coord;
struct box_coord fb_coord;
struct box_coord header_coord;

char uname[MAX_UNAME_LEN];

int init_screen(){
  struct winsize ws;
  initscr();
  ioctl(0, TIOCGWINSZ, &ws);
  win_coord.x_end = ws.ws_col;
  win_coord.y_end = ws.ws_row;
  fb_coord.x_start = win_coord.x_start + 1;
  fb_coord.x_end = win_coord.x_end;
  fb_coord.y_start = win_coord.y_end - 4;
  fb_coord.y_end = win_coord.y_end;
  log_coord.x_start = (win_coord.x_end/2) - (win_coord.x_end/4);
  log_coord.x_end = (win_coord.x_end/2) + (win_coord.x_end/4);
  log_coord.y_start = (win_coord.y_end/2) - (win_coord.y_end/6);
  log_coord.y_end = (win_coord.y_end/2) + (win_coord.y_end/6);
  users_coord.x_start = (win_coord.x_end/5);
  users_coord.y_start = win_coord.y_start + 4;
  users_coord.y_end = win_coord.y_end;
  users_coord.x_end = win_coord.x_end;
  header_coord.x_start = win_coord.x_start;
  header_coord.x_end   = win_coord.x_end;
  header_coord.y_start = win_coord.y_start;
  header_coord.y_end   = win_coord.y_start + 4;
  return 0;
}

void print_fb_window(char *mess){
  struct string_buf *buff = create_buff(fb_coord, mess);
  draw_message_box(fb_coord, mess);
  print_text(buff);
  free_buff(buff);
}

char handle_login_screen(struct credentials * credent){
  start_color();
  init_pair(RED_BLUE, COLOR_BLACK, COLOR_RED);
  clear();
  print_background();
  draw_message_box(log_coord ,credential_mess);
  refresh();
  mvprintw(log_coord.y_end -2, log_coord.x_start + 1, "Log in <l>");
  mvprintw(log_coord.y_end -2, log_coord.x_start + 12 + strlen("Log in <l>"), "Register <r>");
  move(log_coord.y_start + 2, log_coord.x_start + strlen("Login : "));
  getnstr(credent->uname, MAX_UNAME_LEN);
  strcpy(uname, credent->uname);
  move(log_coord.y_start + 3, log_coord.x_start + strlen("Password : "));
  getnstr(credent->passwd, MAX_PASSWD_LEN);
  char input;
  noecho();
  input = wgetch(stdscr);
  echo();
  return input;
}

void print_home_screen(char *uname,struct user_list *ulist){
  print_background();

  char *mess;
  mess = malloc(sizeof ( char ) * strlen(uname) + strlen("Your nickname : "));
  sprintf(mess, "Your nickname : %s", uname);
  draw_message_box(header_coord, mess);
  draw_users_box(users_coord, ulist);
  wgetch(stdscr);
  free(mess);
}

void draw_border(struct box_coord coord){
  attron(COLOR_PAIR(RED_BLUE));
  for(int i=coord.x_start;i<coord.x_end;i++){
    mvprintw(coord.y_start, i, "=");
    mvprintw(coord.y_end -1, i, "=");
  }
  for(int i=coord.y_start;i<coord.y_end;i++){
    mvprintw(i, coord.x_start, "=");
    mvprintw(i, coord.x_end -1, "=");
  }
  attroff(COLOR_PAIR(RED_BLUE));
}

void print_background(){
  draw_border(win_coord);
}

void draw_message_box(struct box_coord coord, char *text){
  draw_border(coord);
  struct string_buf *buff = create_buff(coord, text);
  print_text(buff);
  free_buff(buff);
}

void free_buff(struct string_buf *buff){
  for(int i=0;i<buff->count;i++){
    free(buff->buff[i]);
  }
  free(buff);
}

struct string_buf *create_buff(struct box_coord coord, char *text){
  struct string_buf *text_buff = malloc(sizeof(struct string_buf));
  int size_x = coord.x_end - coord.x_start - 2;
  int size_y = coord.y_end - coord.y_start - 2;
  text_buff->size_x = size_x;
  text_buff->size_y = size_y;
  text_buff->start_x = coord.x_start +1;
  text_buff->start_y = coord.y_start +1;

  int count = count_lines(text, size_x);
  if(count < size_y){
    size_y = count;
    text_buff->size_y = size_y;
  }
  
  char **buff = malloc(count * sizeof(char *));
  text_buff->count = count;

  for(int i=0;i<count;i++){
    buff[i] = malloc((size_x+1) * sizeof(char));
    buff[i][size_x] = '\0';
  }

  text_buff->buff = buff;
  int k=0; 
  int j=0;
  int len = strlen(text);
  for(int i=0;i<len;j++, i++){
    if(text[i] == '\n'){
      buff[k++][j] = '\0';
      j = -1;
      continue;
    }
    if((j % size_x +1) == size_x){
      buff[k++][j] = '\0';
      j = 0;
    }
    buff[k][j] = text[i];
  }
  return text_buff;
}
void print_act_users(struct user_list *ulist, struct box_coord coord){
  for(int i=0;i<ulist->cur_count && i < (coord.y_end - coord.y_start -2);i++){
    mvprintw(coord.y_start + i, coord.x_start +1, ulist->users[i].uname);
  }
}

int count_lines(char *text, int size_x){
  int k=0;
  int count = 1;
  for(int i=0;text[i] != '\0';k++, i++){
    if((k%size_x == size_x -1) || text[i] == '\n'){
      count++;
      k = 0;
    }
  }
  return count;
}

void print_text(struct string_buf *text){
  for(int i=0;i< text->size_y;i++){
    mvprintw(text->start_y + i, text->start_x, text->buff[i]);
  }
}
void draw_users_box(struct box_coord users_coord, struct user_list *ulist){
  draw_border(users_coord);
  print_act_users(ulist, users_coord);
}
