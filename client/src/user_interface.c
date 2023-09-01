#include <ncurses.h>
#include <stdlib.h>
#include <menu.h>
#include <string.h>
#include <sys/ioctl.h>
#include "user_interface.h"
#include "proto_cipa.h"

#define RED_BLUE 1

char *credential_mess = "Please insert your login credentials\0";

struct box_coord win_coord;

int init_screen(){
  struct winsize ws;
  initscr();
  ioctl(0, TIOCGWINSZ, &ws);
  win_coord.x_end = ws.ws_col;
  win_coord.y_end = ws.ws_row;
  start_color();
  init_pair(RED_BLUE, COLOR_BLACK, COLOR_RED);
  clear();
  print_background();
  print_input_credentials();
  getch();
  endwin();
  return 0;
}

void print_background(){
  draw_border(win_coord);
}

int print_text(char *text, struct box_coord coord){
  struct ring_buf line;
  ring_clean(&line);
  int linelen = coord.x_end - coord.x_start -2;
  line.counter = linelen;
  int linecount = coord.y_end - coord.y_start - 2;
  if((linelen)*(coord.y_end - coord.y_start -2) < textlen) return -1;
  
  for(int j=0;j<linecount;j++){
    for(int i=0;i<linelen;i++){
      if((out = text[j*linelen + i]) == '\0') return 0;
      if((out = text[j*linelen + i]) == '\n') break;
      mvprintw(coord.y_start + j+1, coord.x_start + i + 1, &out);
    }
  }
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

void print_input_credentials(){
  struct box_coord coord;
  coord.x_end = win_coord.x_end/2 + 35; 
  coord.x_start   = win_coord.x_end/2 - 35; 
  coord.y_end = win_coord.y_end/2 + 5;
  coord.y_start   = win_coord.y_end/2 - 5;
  attron(COLOR_PAIR(RED_BLUE));
  draw_border(coord);
  coord.y_start++;
  print_text(credential_mess, coord);
  coord.y_start++;
  print_text("Username: ", coord);
  coord.y_start++;
  print_text("Password: ", coord);
}
