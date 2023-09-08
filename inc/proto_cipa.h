#ifndef _PROTO_CIPA_H
#define _PROTO_CIPA_H
#include <stdint.h>
#include <stdio.h>

#define MAX_UNAME_LEN 127
#define MAX_PASSWD_LEN 894

#define SUCCESSFUL 0
#define FAILED 1

#define H_LOGIN 1
#define H_REG 2
#define H_MESS 3
#define H_LOGOUT 4
#define H_CONN 5
#define H_DISCONN 6


typedef struct cipa_packet{
  uint8_t header;
  char content[1023];
}cipa_packet;

typedef struct user{
  char uname[MAX_UNAME_LEN];
  char tname[MAX_UNAME_LEN];
  int  user_fd;
  int  talker_fd;
}user;

typedef struct user_list{
  user users[1024];
  int cur_count;
}user_list;

void ulist_init();

int userlist_add(int user_fd, char *uname);

void userlist_remove(int user_fd);

int handle_conn(int user_fd, char *uname);

struct cipa_packet register_pack(char *uname, char *passwd);

int handle_mess(int user_fd, char *mess);

struct cipa_packet login_pack(char *uname, char *passwd);

struct cipa_packet disconn_pack();

struct cipa_packet connect_pack(char *uname);

struct cipa_packet mess_pack(char *mess);

void parse_packet(struct cipa_packet *pack, int user_fd);

int search_db(char *uname, char *passwd);

int handle_disconn(int user_fd);

struct cipa_packet success_pack(char *mess);

struct cipa_packet failed_pack(char *mess);

#endif
