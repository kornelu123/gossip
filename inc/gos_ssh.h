#ifndef SSH_H
#define SSH_H

#include <libssh/libssh.h>

int verify_knownhost(ssh_session session);

int connect_gos();

#endif
