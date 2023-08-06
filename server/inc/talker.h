#ifndef TALKER_H
#define TALKER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

void *reader(void* ptr);

void *writer(void* ptr);

#endif
