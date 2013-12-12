#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define emsg(s, d)                                                             \
  fprintf(stderr, "%s(%d): %s %s\n", __func__, __LINE__, s, strerror(d))
#define err(s) emsg(s, errno)
#define err2(fmt, ...)                                                         \
  do {                                                                         \
    int errno_tmp__ = errno;                                                   \
    fprintf(stderr, "%s(%d): %s " fmt "\n", __func__, __LINE__,                \
            strerror(errno_tmp__), __VA_ARGS__);                               \
  } while (0)
#define errstring(s) fprintf(stderr, "%s(%d): %s\n", __func__, __LINE__, s)


void server_usage(void);
int get_accept_fds(const char *host, const char *port, int **fds);
int init_fdset(const int *const fds, size_t nfds, fd_set *fdset);
struct addrinfo *get_connect_addr(const char *host, const char *port);
int open_socket(const char *host, const char *port);
const char * hostname(struct sockaddr * sa, socklen_t length);
