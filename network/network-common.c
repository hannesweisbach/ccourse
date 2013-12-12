#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "network-common.h"

void server_usage() {
  fprintf(stdout, "--port | -p <port>  start listening on port <port>.\n");
  fprintf(stdout, "                    The default port is 8053.\n");
  fprintf(stdout, "--iface | -i <ip>   start listening on ip <ip>.\n");
  fprintf(stdout,
          "                    The default is to listen on all interfaces.\n");
  fprintf(stdout, "--size | -s <size>  Message size in bytes (1 to 65535).\n");
  fprintf(stdout, "                    The default messages size is 1024 bytes.\n");
}

const char *hostname(struct sockaddr *sa, socklen_t length) {
  static char hbuf[NI_MAXHOST];
  //  static char sbuf[NI_MAXSERV];

  int err =
      getnameinfo(sa, length, hbuf, sizeof(hbuf), NULL, 0, NI_NUMERICHOST);
  if (err) {
    errstring(gai_strerror(err));
    hbuf[0] = 0;
  }

  return hbuf;
}

const char *service(struct sockaddr *sa, socklen_t length) {
  static char sbuf[NI_MAXSERV];

  int err =
      getnameinfo(sa, length, NULL, 0, sbuf, sizeof(sbuf), NI_NUMERICSERV);
  if (err) {
    errstring(gai_strerror(err));
    sbuf[0] = 0;
  }

  return sbuf;
}

int count_addrs(struct addrinfo *ais) {
  int num = 0;
  for (struct addrinfo *ai = ais; ai; ai = ai->ai_next)
    num++;
  return num;
}

enum CONFIG {
  IS_CLIENT = 0,
  IS_SERVER = 1,
};

static struct addrinfo *get_server_addr(const char *host, const char *port,
                                 enum CONFIG listen) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_ADDRCONFIG;
  if (listen == IS_SERVER)
    hints.ai_flags |= AI_PASSIVE;
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  struct addrinfo *ais;

  int err = getaddrinfo(host, port, &hints, &ais);
  if (err) {
    fprintf(stderr, "%s(%d): getaddrinfo failed: %s\n", __func__, __LINE__,
            gai_strerror(err));
    return NULL;
  }

  return ais;
}

struct addrinfo * get_listen_addr(const char * host, const char * port) {
  return get_server_addr(host, port, IS_SERVER);
}

struct addrinfo *get_connect_addr(const char *host, const char *port) {
  return get_server_addr(host, port, IS_CLIENT);
}

int get_accept_fds(const char *host, const char *port, int **fds) {
  int ret = -1;
  assert(fds);
  assert(host || port);

  struct addrinfo *ais = get_listen_addr(host, port);
  int nfds = count_addrs(ais);

  *fds = malloc(sizeof(int) * nfds);
  int *const fds_ = *fds;
  if (fds_ == NULL) {
    err("malloc fds");
    goto err_malloc;
  }

  nfds = 0;
  for (struct addrinfo *ai = ais; ai; ai = ai->ai_next) {
    int s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (s < 0) {
      err2("%s: ", hostname(ai->ai_addr, ai->ai_addrlen));
      continue;
    }

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(s, ai->ai_addr, ai->ai_addrlen)) {
      err("bind");
      close(s);
      continue;
    } else {
      if (listen(s, SOMAXCONN)) {
        err("listen");
      } else {
        fprintf(stdout, "Listening on %s:%s\n",
                hostname(ai->ai_addr, ai->ai_addrlen),
                service(ai->ai_addr, ai->ai_addrlen));
        fds_[nfds++] = s;
      }
    }
  }

  freeaddrinfo(ais);
  return nfds;

err_malloc:
  freeaddrinfo(ais);

  return ret;
}

int open_socket(const char *host, const char *port) {
  int s = -1;
  struct addrinfo *ais = get_connect_addr(host, port);
  for (struct addrinfo *ai = ais; ai; ai = ai->ai_next) {
    s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (s < 0) {
      err("socket");
      continue;
    }

    if (connect(s, ai->ai_addr, ai->ai_addrlen)) {
      err2("%s", hostname(ai->ai_addr, ai->ai_addrlen));
      close(s);
      s = -1;
      continue;
    } else {
      break;
    }
  }

  freeaddrinfo(ais);

  return s;
}

int init_fdset(const int *const fds, size_t nfds, fd_set *fdset) {
  int maxfd = INT_MIN;
  FD_ZERO(fdset);

  for (size_t i = 0; i < nfds; i++) {
    int fd = fds[i];
    FD_SET(fd, fdset);
    if (fd > maxfd)
      maxfd = fd;
  }

  return maxfd;
}


