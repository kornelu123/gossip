#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

const char *db_file = "users.txt";

int search_db(char *uname, char *passwd){
  FILE* db_read = fopen(db_file,"r");
  if(db_read == NULL){
    fprintf(stderr, "fopen : %s", gai_strerror(db_file));
    exit(1);
  }
  char* lookup = malloc(( sizeof (char))*1024);
  sprintf(lookup, "user:%s;passwd:%s\n", uname, passwd);
  char *lineptr = NULL;
  size_t size = 0;
  ssize_t chars;
  while((chars = getline(&lineptr, &size, db_read)) >= 0){
    if(!(strcmp(lookup,lineptr))){
    fclose(db_read);
      return 0;
    }
  }
  sprintf(lookup, "user:%s;passwd:", uname);
  while((chars = getline(&lineptr, &size, db_read)) >= 0){
    if(!(strcmp(lookup,lineptr))){
      fclose(db_read);
      return 2;
    }
  }
  fclose(db_read);
  return 1;
}

void add_user(char *uname, char *passwd){
  FILE *db_app = fopen(db_file, "a");
  fprintf(db_app,"user:%s;passwd:%s\n",uname,passwd);
  fclose(db_app);
}
