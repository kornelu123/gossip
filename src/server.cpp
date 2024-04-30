#include <thread>

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdint>

#include <netdb.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

#include <unistd.h>

#define DEFAULT_PORT    31337

#define MAX_USER_COUNT  64

#include "logger.cpp"
#include "cip.cpp"

logger logg;

class
server
{
  private:
    struct pollfd                       pfds[MAX_USER_COUNT + 1];
    int                                 fd_count;

    struct sockaddr_storage             last_remote;
    socklen_t                           addrlen;

    uint16_t                            port;

  public:

     server(uint16_t port)
    {
      this->fd_count = 0;
      this->port = port;

      add_pfd(get_listener_socket(), POLLIN, 0);
    }

    void
      run(void)
    {
      const int listener = pfds[0].fd;
      for(;;){
        int poll_count = poll(pfds, this->fd_count, -1);

        if(poll_count == -1){
          herror("poll");
          exit(1);
        }

        logg.log("Got packet", LOG_INFO);

        for(int i = 0; i<this->fd_count; i++){
          if(pfds[i].revents & POLLIN){
            if(pfds[i].fd == listener){
              int newfd = accept(listener, (struct sockaddr *)&(this->last_remote), &(this->addrlen));

              if(newfd == -1)
                herror("accept");

              else
               add_pfd(newfd, POLLIN, 0);

              logg.log("User connected", LOG_INFO);

            } else {
              uint8_t content[MAX_PACK_LEN];
              int recv_bytes = recv(pfds[i].fd, content, MAX_PACK_LEN, 0);

              if(recv_bytes <= 0){
                if(recv_bytes == 0)
                  logg.log("User disconnected", LOG_INFO);
                else
                  herror("recv");

                close(pfds[i].fd);

                del_pfd(i);
              } else {
              
                cip_pack::recved_pack *packet = new cip_pack::recved_pack(recv_bytes, content);
                cip_pack::recved_pack::print_pack(*packet);
              }
            }
          }
        }
      }
    }

  private:
    inline int
      get_listener_socket(void)
    {
      int listener, ret;
      socklen_t size;

      struct sockaddr_in servaddr;

      listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      bzero((char *)&servaddr, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_addr.s_addr = INADDR_ANY;
      servaddr.sin_port = htons(this->port);

      if(bind(listener, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        herror("bind");
        exit(1);
      }

      logg.log("Bind succesful", LOG_INFO);

      if(listen(listener, 5) == -1){
        herror("listen");
        exit(1);
      }

      logg.log("Listen succesful", LOG_INFO);

      return listener;
    }

    inline int
      add_pfd(int fd, int event, int revent)
    {
      if(this->fd_count == MAX_USER_COUNT + 1)
        return -1;

      struct pollfd server_fd;

      if(event)
        server_fd.events = event;

      if(revent)
        server_fd.revents = revent;

      server_fd.fd = fd;

      this->pfds[this->fd_count++] = server_fd;

      return 0;
    }

    inline int
      del_pfd(int index)
    {
      if(this->fd_count == 1)
        return -1;

      if(index >= this->fd_count)
        return -1;

      for(int i=index; i < --this->fd_count; i++){
        this->pfds[i] = this->pfds[i + 1];
      }

      return 0;
    }
};

  int
main(int argc, char *argv[])
{
  server *serv;

  if(argc <= 1){
    serv = new server(DEFAULT_PORT);

    char msg[64];
    snprintf(msg, 64, "No port argument, running with default port %d", DEFAULT_PORT);
    logg.log(msg, LOG_WARN);
  }else{
    uint16_t port_input;
    if(sscanf(argv[1], "%hi", &port_input) == 1){
      serv = new server(port_input);

      char msg[64];
      snprintf(msg, 64, "Starting server with port %d", port_input);
      logg.log(msg, LOG_INFO);
    } else {
      serv = new server(DEFAULT_PORT);

      char msg[64];
      snprintf(msg, 64, "Bad port argument, running with default port %d", DEFAULT_PORT);
      logg.log("Bad port argument, running with default port 1337", LOG_WARN);
    }
  }

  serv->run();
  return 0;
}
