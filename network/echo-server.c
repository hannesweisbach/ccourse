#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <getopt.h>
#include <stddef.h>

#include "network-common.h"

void strrev(const unsigned char *in, unsigned char *out, size_t len) {
  /* reverse string, but leave an (optional) trailing 0 and/or \n as last
   * character(s) */
  size_t trail = 0;
  for (const unsigned char *endp = &in [len - 1]; *endp == 0 || *endp == '\n';
       endp--)
    trail++;
  size_t end = len - trail;
  for (size_t i = 0; i < end; i++) {
    out[end - i - 1] = in[i];
  }
  memcpy(&out[end], &in[end], trail);
}

/* receive up to n bytes or until '0' is encountered */
ssize_t echo_recv(int fd, unsigned char * buf, size_t bytes) {
  size_t total = 0;
  while(total < bytes) {
    ssize_t r = read(fd, &buf[total], bytes - total);
    if(r <= 0) {
      err("read");
      return -1;
    } else {
      total += r;
      for(size_t i = total; i > total - r; i--)
        if(buf[i - 1] == 0)
          return i;
    }
  }
  buf[total - 1] = 0;
  return total;
}


void handle(int fd, size_t msg_size) {
  unsigned char recv_buf[msg_size];
  unsigned char send_buf[msg_size];

  struct sockaddr_storage addr;
  socklen_t addrlen = sizeof(addr);

  int client_socket = accept(fd, (struct sockaddr *)&addr, &addrlen);
  if (client_socket < 0) {
    err("accept");
    return;
  }

  fprintf(stdout, "Connection request from %s\n",
          hostname((struct sockaddr *)&addr));

  ssize_t recv_len = echo_recv(client_socket, recv_buf, sizeof(recv_buf));
  if (recv_len < 0)
    goto out;

  fprintf(stdout, "Recv'd %zd bytes: '", recv_len);
  for (ssize_t i = 0; i < recv_len; i++)
    if (recv_buf[i] != '\n')
      fprintf(stdout, "%c", recv_buf[i]);
  fprintf(stdout, "' ");
  for (ssize_t i = 0; i < recv_len; i++)
    fprintf(stdout, "%02x ", recv_buf[i]);
  fprintf(stdout, "\n");

  strrev(recv_buf, send_buf, recv_len);

  ssize_t send_len = write(client_socket, send_buf, recv_len);
  if (send_len < 0)
    err("write");

out:
  close(client_socket);
}

int main(int argc, char *argv[]) {
  const char *host = NULL;
  const char *port = "8053";
  ssize_t msg_size = 1024;

  static struct option longopts[] = { { "port", required_argument, NULL, 'p' },
                                      { "iface", required_argument, NULL, 'i' },
                                      { "size", required_argument, NULL, 's' },
                                      { NULL, 0, NULL, 0 } };

  int opt;
  while ((opt = getopt_long(argc, argv, "p:i:s:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'p':
      port = optarg;
      break;
    case 'i':
      host = optarg;
      break;
    case 's':
      if(sscanf(optarg, "%zd", &msg_size) == 1 && msg_size > 0)
        break;
    case ':':
    case '?':
    default:
      server_usage();
      return EXIT_FAILURE;
    }
  }
  
  fprintf(stdout, "Message size %zd\n", msg_size);

  sig_t action = signal(SIGPIPE, SIG_IGN);
  if (action == SIG_ERR) {
    err("signal");
    fprintf(stderr, "Might get SIGPIPE.\n");
  }

  int *fds;
  int nfds = get_accept_fds(host, port, &fds);

  for (;;) {
    fd_set sockets;
    int maxfd = init_fdset(fds, nfds, &sockets);

    int ret = select(maxfd + 1, &sockets, NULL, NULL, NULL);
    if (ret < 0) {
      err("select");
    } else if (ret == 0) {
      err("timeout");
    } else {
      for (int i = 0; i < nfds; i++) {
        int fd = fds[i];
        if (FD_ISSET(fd, &sockets)) {
          handle(fd, msg_size);
        }
      }
    }
  }
}

