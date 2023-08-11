#ifndef _PROTO_CIPA_H
#define _PROTO_CIPA_H
#include <stdint.h>

#define MAX_UNAME_LEN 127
#define MAX_PASSWD_LEN 894

#define H_LOGIN 1
#define H_REG 2
#define H_MESS 3
#define H_LOGOUT 4


typedef struct cipa_packet{
  uint8_t header;
  uint8_t content[1023];
}cipa_packet;

struct cipa_packet register_pack(char *uname, char *passwd);

void parse_packet(cipa_packet pack);

#endif
