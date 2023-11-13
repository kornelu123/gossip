#ifndef _PROTO_CIPA_H
#define _PROTO_CIPA_H

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "database.h"

#define USER_NOT_FOUND  -1
#define USER_FOUND       0
#define USER_DELETED     0


#define MAX_CIPA_PACK_LEN 1400

#define H_LOG   0x00
#define H_REG   0x01
#define H_DEL   0x02
#define H_ULIST 0x03

#define R_USER_LIST 0xFB
#define R_FAIL_REG  0xFC
#define R_SUCC_REG  0xFD
#define R_SUCC_LOG  0xFE
#define R_FAIL_LOG  0xFF

struct __attribute__((packed)) user{
    int                 size;
    int              user_fd;
    char *             uname;
    struct user*        next;
    struct user*        prev;
};

struct user_list{
    struct user *       first;
    struct user *        last;
};

struct __attribute__((packed)) cipa_pack{
    uint8_t             head;
    uint16_t            size;
    uint8_t         *content;
};

struct __attribute__((packed)) intern_pack{
    clock_t       queued_clk;
    int              user_fd;
    struct cipa_pack  packet;
};

struct intern_pack make_intern_pack(struct cipa_pack pack, int user_fd);

void make_and_send_pack(int server_fd, uint8_t head, void *content);

int pack_credent(struct user_credent *credent, struct cipa_pack *pack);

struct cipa_pack make_pack(uint8_t head, void *content);

void send_pack(int server_fd, struct cipa_pack pack);

int pack_ulist(struct user_list *ulist, struct cipa_pack *pack);

void add_to_user_list(struct user_list **ulist, struct user **u);

int del_from_user_list(struct user_list **list, int user_fd);

int find_user_by_name(struct user_list ulist,struct user *u ,char *uname);

int add_active_user(struct user_list **ulist, int user_fd, uint8_t *content);

void ulist_init(struct user_list **ulist);

#endif
