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
#include <errno.h>
#include <unistd.h>
 
#include "proto_cipa.h"
#include "buffer.h"
#include "gossip_server.h"
#include "debug.h"

#define PORT "44444"

struct buff sort_buf;

pthread_mutex_t buf_mut;
pthread_mutex_t  fb_mut;

struct user_list *ulist;

pthread_t handle_thread;

uint8_t add_more_threads = 0;

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
    (*pfds)[*fd_count].events = POLLIN;

    (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count){
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

void add_sort_buff( void *content, int user_fd){
    struct cipa_pack pack;
    uint8_t *head = content;
    uint16_t *size = content + sizeof(uint8_t) ;
    uint8_t  *bytes = content + sizeof(uint8_t)  + sizeof(uint16_t);

    pack.head = *head;
    pack.size = *size;
    pack.content = malloc(pack.size);
    memcpy(pack.content, bytes, pack.size);

    struct intern_pack sort = make_intern_pack(pack, user_fd);

    pthread_mutex_lock(&buf_mut);
        push(&sort_buf, &sort);
    pthread_mutex_unlock(&buf_mut);
}

void init_buffers(){
    init_buff(&sort_buf);
}

void *handle_task(){
    struct intern_pack *pack = NULL;
    while(1){
        pthread_mutex_lock(&buf_mut);
        if(pop(&sort_buf, &pack)){
            pthread_mutex_unlock(&buf_mut);
            continue;
        }
        clock_t clk = clock();

        pthread_mutex_unlock(&buf_mut);

        if((clk - pack->queued_clk)/(CLOCKS_PER_SEC << 6) > 1){
            pthread_mutex_lock(&fb_mut);
            add_more_threads = 1;
            pthread_mutex_unlock(&fb_mut);
        }

        switch(pack->packet.head){
            case H_LOG:{
                if(search_db(pack->packet.content, NOT_DEL)){
                    if(add_active_user(&ulist, pack->user_fd, pack->packet.content)){
                        make_and_send_pack(pack->user_fd, R_FAIL_LOG, NULL);
                        break;
                    }
                    make_and_send_pack(pack->user_fd, R_SUCC_LOG, NULL);
                    pack_ulist(ulist, &pack->packet);
                    make_and_send_pack(pack->user_fd, R_USER_LIST, (void *)ulist);
                    break;
                }

                make_and_send_pack(pack->user_fd, R_FAIL_LOG, NULL);
                break;
            }
            case H_REG:{
                if(!search_db(pack->packet.content, NOT_DEL)){
                    make_and_send_pack(pack->user_fd, R_SUCC_REG, NULL);
                    add_user(pack->packet.content);
                    break;
                }

                make_and_send_pack(pack->user_fd, R_FAIL_REG, NULL);
                break;
            }
            case H_DEL:{
                if(search_db(pack->packet.content, DEL_USER)){
                    make_and_send_pack(pack->user_fd, R_SUCC_DEL, NULL);
                    break;
                }

                make_and_send_pack(pack->user_fd, R_FAIL_DEL, NULL);
                break;
            }
            case H_ULIST:{
                
            }
        }
    }
}

int main(){
    ulist_init(&ulist);

    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = malloc(sizeof *pfds *fd_size);

    int listener = get_listener_socket();

    if(listener == -1){
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;

    pthread_create(&handle_thread, NULL, handle_task, NULL);

    for(;;){
        int poll_count = poll(pfds, fd_count, -1);
        if(poll_count == -1){

            perror("poll");
            exit(1);
        }
        for(int i=0; i<fd_count; i++){
            
            if(pfds[i].revents & POLLIN){
                
                if(pfds[i].fd == listener){
                    
                    addrlen = sizeof remoteaddr;
                    int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
                    if(newfd == -1){

                        perror("accept");
                    } else {

                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
                    }
                } else {
                    
                    void *content = malloc(MAX_CIPA_PACK_LEN);
                    int nbytes = recv(pfds[i].fd, content, MAX_CIPA_PACK_LEN, 0);
                    if(nbytes <= 0){
                        if(nbytes == 0){

                            del_from_pfds(pfds, i, &fd_count);
                            close(pfds[i].fd);
                            break;
                        }else{

                            perror("recv");
                        }
                    } else {

                        content = realloc(content, nbytes);
                        add_sort_buff(content, pfds[i].fd);
                    }
                }
            }
        }
    }

    return 0;
}
