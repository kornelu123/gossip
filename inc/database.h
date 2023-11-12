#ifndef DATABASE_H_
#define DATABASE_H_

#define MAX_UNAME_LEN 64
#define MAX_PASSWD_LEN MAX_UNAME_LEN

struct user_credent{
    char    uname[MAX_UNAME_LEN];
    char  passwd[MAX_PASSWD_LEN];
};

void add_user(uint8_t *content);

int search_db(uint8_t *content);

#endif
