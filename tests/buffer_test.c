#include "buffer_test.h"
#include "buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEST_MESS "TEST_MESSAGE"

int test_buf(){
    
    struct buff *buf = malloc(sizeof(struct buff));
    init_buff(buf);

    char mess[12] = TEST_MESS;
    char org_mess[12] = TEST_MESS;

    push(buf, (void *)mess, sizeof(TEST_MESS));
    push(buf, (void *)mess, sizeof(TEST_MESS));
    push(buf, (void *)mess, sizeof(TEST_MESS));

    memset(mess, 0, sizeof(TEST_MESS));


    while(!pop(buf, mess)){
        if(!strcmp(mess, org_mess)) return -1;
    }

    return 0;
}
