#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "database.h"

const char *db_file = "users.txt";

int search_db(uint8_t *content){
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
  while((chars = getline(&lineptr, &size, db_read)) >= 0){
    if(!(strcmp(lookup,lineptr))){
      fclose(db_read);
      return 0;
    }
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
  printf("user:%s;passwd:%s\n",uname,passwd);
  fprintf(db_app,"user:%s;passwd:%s\n",uname,passwd);
  fclose(db_app);
}
