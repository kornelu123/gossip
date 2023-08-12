#ifndef _PROTO_CIPA_H
#define _PROTO_CIPA_H
#include <stdint.h>
#include <stdio.h>

#define MAX_UNAME_LEN 127
#define MAX_PASSWD_LEN 894

#define H_LOGIN 1
#define H_REG 2
#define H_MESS 3
#define H_LOGOUT 4


typedef struct cipa_packet{
  uint8_t header;
  char content[1023];
}cipa_packet;

typedef struct user{
  char uname[MAX_UNAME_LEN];
  int  user_fd;
}user;

struct cipa_packet register_pack(char *uname, char *passwd);

struct cipa_packet login_pack(char *uname, char *passwd);

void parse_packet(struct cipa_packet *pack, int user_fd);

int search_db(char *uname, char *passwd);

#endif
