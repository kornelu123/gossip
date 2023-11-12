#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>

#include "proto_cipa.h"
#include "database.h"

struct intern_pack make_intern_pack(struct cipa_pack pack, int user_fd){

    struct intern_pack sort;
    sort.user_fd = user_fd;
    sort.packet  = pack;
    clock_t clk = clock();
    sort.queued_clk = clk;

    return sort;
}

int pack_credent(struct user_credent *credent, struct cipa_pack *pack){
    char uname[MAX_UNAME_LEN];
    char passwd[MAX_PASSWD_LEN];

    int i=0;
    do{
      uname[i] = credent->uname[i];
      i++;
    }while(uname[i-1] != '\n');

    int j=0;
    do{
      passwd[j] = credent->passwd[j];
      j++;
    }while(passwd[j-1] != '\n');

    pack->content = realloc(pack->content, i + j);

    pack->size = i + j;
    memcpy(pack->content, uname, i);
    memcpy(pack->content + i, passwd, j);

    return i+j;
}

int pack_ulist(struct user_list *ulist, struct cipa_pack *pack){
    uint8_t *content = (uint8_t *)malloc(MAX_CIPA_PACK_LEN - 3);
    int i=0;
    struct user *u = ulist->first;

    while(1){
        memcpy(content + i, u->uname, sizeof(u->uname));
        i += sizeof(u->uname);
        if(u->next == NULL) 
            break;
        u = u->next;
    }
    
    pack->size = i;
    pack->content = realloc(pack->content, i);
    memcpy(pack->content, content, i);
    return i;
}

void make_and_send_pack(int server_fd, uint8_t head, void * content){

    struct cipa_pack pack;
    pack.size = 0;
    pack.head = head;
    pack.content = NULL;
    int size;

    switch(head){
        case H_LOG:
        case H_REG:
        case H_DEL:{
            size = pack_credent((struct user_credent *)content, &pack);
            pack.size = size;
            break;
        }
        case R_SUCC_LOG:
        case R_FAIL_LOG:{
            break;
        }
        case R_USER_LIST:{
            size = pack_ulist((struct user_list *)content, &pack);
            pack.size = size;
        }
    }

    send_pack(server_fd, pack);
    free(pack.content);
}

void ulist_init(struct user_list **ulist){
    *ulist = (struct user_list *)malloc(sizeof(struct user_list));
    (*ulist)->first = NULL;
    (*ulist)->last  = NULL;
}

void add_active_user(struct user_list **ulist, int user_fd, uint8_t *content){
    struct user *u = (struct user*) malloc(sizeof (struct user));
    u->uname = (char *) malloc(*content);
    char uname[MAX_UNAME_LEN];
    int i=0;

    do{
        uname[i] = content[i];
        i++;
    }while(uname[i-1] != 0x0A);
    uname[i-1] = '\0';

    u->uname = realloc(u->uname, i + 2);
    memcpy(u->uname, uname, i);
    add_to_user_list(ulist, &u);
}
void add_to_user_list(struct user_list **ulist, struct user **u){


    if(((*ulist)->first == NULL) || ((*ulist)->first == NULL)){

        (*ulist)->first = *u;
        (*ulist)->last  = *u;
        (*u)->prev       = NULL;
        (*u)->next       = NULL;
        return;
    }

    (*ulist)->last->next = *u;
    (*u)->prev = (*ulist)->last;
    (*u)->next = NULL;
    (*ulist)->last       = *u;
}

int del_from_user_list(struct user_list **ulist, int user_fd){

    struct user *u = malloc(sizeof(struct user));
    u = (*ulist)->first;
    do{
        if(u->user_fd == user_fd){
            u->prev->next = u->next;
            if(u == (*ulist)->last){
                (*ulist)->last = u->prev;
                return USER_DELETED;
            }
            u->next->prev = u->prev;
            return USER_DELETED;
        }
    }while(u->next != NULL);

    return USER_NOT_FOUND;
}

int find_user_by_name(struct user_list ulist,struct user *u ,char *uname){
    u = ulist.first;

    do{
       if(!memcmp(u->uname, uname, MAX_UNAME_LEN)){
            return USER_FOUND;
       }
       u = u->next;
    }while(u != NULL);

    return USER_NOT_FOUND;
}

void send_pack(int server_fd, struct cipa_pack pack){
    int full_pack_size = pack.size + sizeof(pack.size) + sizeof(pack.head);
    void *content = malloc(full_pack_size);

    memcpy(content, &pack.head, sizeof(pack.head));
    memcpy(content + sizeof(pack.head), &pack.size, sizeof(pack.size));
    if(pack.content != NULL){
        memcpy(content + sizeof(pack.head) + sizeof(pack.size), pack.content, pack.size);
    }

    send(server_fd, content, full_pack_size, 0);
    free(content);
}
