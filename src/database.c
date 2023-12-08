#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "database.h"

const char *db_file = "users.txt";

int save_message(char *uname, char *mess){

    char name[strlen(uname) + strlen(".txt")];
    snprintf(name, sizeof(name), "%s.txt", uname);
    FILE *file = fopen(name, 'a');
    if(file == NULL){
        fprintf(stderr, "fopen %s", strerror(errno));
    }
    fprintf(file, "%s\n", mess);
    fclose(file);
    return 0;
}

int search_db(uint8_t *content, uint8_t del_flag){
  char *uname = (char *)malloc(MAX_UNAME_LEN);
  char *passwd = (char *)malloc(MAX_PASSWD_LEN);
 
  int i=0;
  int j=0;
 
  do{
    uname[i] = content[i];
    i++;
  }while(uname[i - 1] != '\n');
  uname[i - 1] = '\0';
 
  do{
    passwd[j] = content[i];
    i++;
    j++;
  }while(passwd[j - 1] != '\n');
  passwd[j - 1] = '\0';
 
 
  FILE* db_read = fopen(db_file,"r");
  if(db_read == NULL){
    fprintf(stderr, "fopen : %s", gai_strerror(db_file));
    exit(1);
  }
  char* lookup = malloc(( sizeof (char))*1024);
  sprintf(lookup, "user:%s;passwd:%s", uname, passwd);
  char *lineptr = NULL;
  size_t size = 0;
  ssize_t chars;
  int count = 0;
 
  int res = 0;
  while((chars = getline(&lineptr, &size, db_read)) >= 0){
    int i;
    int is_valid = 0;
    for(i=0; lineptr[i] != '\n'; i++){
        if(lineptr[i] != lookup[i]){
            is_valid = 1;
        }
    } 
 
    if(is_valid == 1){
        continue;
    }
 
    if(del_flag == DEL_USER){
    FILE* temp_file = fopen("temp", "w");
    while((chars = getline(&lineptr, &size, db_read) >= 0)){
      if(i != count){
          continue;
      }
 
      fprintf(temp_file, lineptr);
      count++;
    }
 
    fclose(temp_file);
    fclose(db_read);
    remove(db_file);
    rename("temp", db_file);
 
    }
    return 0;
  }
  fclose(db_read);
  return -1;
}

void add_user(uint8_t *content){
  char *uname = (char *)malloc(MAX_UNAME_LEN);
  char *passwd = (char *)malloc(MAX_PASSWD_LEN);

  int i=0;
  int j=0;

  do{
    uname[i] = content[i];
    i++;
  }while(uname[i - 1] != '\n');
  uname[i - 1] = '\0';

  do{
    passwd[j] = content[i];
    i++;
    j++;
  }while(passwd[j - 1] != '\n');
  passwd[j - 1] = '\0';

  FILE *db_app = fopen(db_file, "a");
  fprintf(db_app,"user:%s;passwd:%s\n",uname,passwd);
  fclose(db_app);
}
