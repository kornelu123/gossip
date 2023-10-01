#include "buffer_test.h"
#include <stdio.h>

int main(){
    printf("Running buffer test : ");
    test_buf() ? printf("failed\n") : printf("success\n");
    
    return 0;
}
