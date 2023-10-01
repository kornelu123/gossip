#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>

#include "proto_cipa.h"
#include "buffer.h"

#define PORT "44444"

int listener;
pthread_rwlock_t sort_lock;

struct buff sort_buf;

int get_listener_socket(void){
    int listener, rv;
    int yes = 1;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0){
      fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
      exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next){
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(listener < 0)
            continue;

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if(bind(listener, p->ai_addr, p->ai_addrlen) < 0){
        close(listener);
        continue;
    };

    break;
    }
 freeaddrinfo(ai);

 if(p == NULL){
    return -1;
 }

 if(listen(listener, 10) == -1){
    return -1;
 }

 return listener;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size){
    
    if(*fd_count == *fd_size){
        *fd_size *= 2;

        *pfds = realloc(*pfds, sizeof (**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count++].events = POLLIN;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count){
    pfds[i] = pfds[--*fd_count];
}

void add_sort_buff(struct cipa_packet pack){
    pthread_rwlock_wrlock(&sort_lock);
        push(&sort_buf, (void *)&pack, sizeof(pack));
    pthread_rwlock_unlock(&sort_lock);
}

void *receive_task(){
    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds *fd_size);

    listener = get_listener_socket();

    struct cipa_packet pack;

    if(listener == -1){
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    for(;;){
        int poll_count = poll(pfds, fd_count, -1);
        if(poll_count == -1){

            perror("poll");
            exit(1);
        }
        for(int i=0; i<fd_count; i++){
            
            if(pfds[i].events & POLLIN){
                
                if(pfds[i].fd == listener){
                    
                    addrlen = sizeof remoteaddr;
                    int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
                    if(newfd == -1){

                        perror("accept");
                    } else {

                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
                        printf("connected");
                    }
                } else {

                    int nbytes = recv(pfds[i].fd, &pack, sizeof pack, 0);
                    if(nbytes <= 0){
                        if(nbytes == 0){

                            del_from_pfds(pfds, i, &fd_count);
                            close(pfds[i].fd);
                            break;
                        }else{

                            perror("recv");
                        }
                    } else {
                        printf("got pack");

                        add_sort_buff(pack);
                    }
                }
            }
        }
    }
}

void handle_pack(struct cipa_packet pack){
    printf("%s", pack.content);
}

void init_buffers(){
    init_buff(&sort_buf);
}

void *sort_task(){

    struct cipa_packet pack;

    for(;;){
        pthread_rwlock_rdlock(&sort_lock);
        if(pop(&sort_buf, (void *)&pack))
            pthread_rwlock_unlock(&sort_lock);
            continue;
        pthread_rwlock_unlock(&sort_lock);
        handle_pack(pack);
    }

}

int main(){

    pthread_t receiver;
    pthread_t sorter;
    int res;

    init_buffers();
    if((res = pthread_rwlock_init(&sort_lock, NULL))){

        perror("rwlock");
        exit(1);
    }

    pthread_create(&receiver,
            NULL,
            receive_task,
            NULL);

    pthread_create(&sorter,
            NULL,
            sort_task,
            NULL);



    pthread_join(receiver, NULL);
    pthread_join(sorter, NULL);

    return 0;
}
