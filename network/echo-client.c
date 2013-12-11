#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "network-common.h"

void usage(const char *progname) {
  fprintf(stdout, "Usage: %s [-p <port>] <server> <message>\n", progname);
  fprintf(stdout, "--port | -p <port>  start listening on port <port>.\n");
  fprintf(stdout, "                    The default port is 8053.\n");
}

ssize_t recv_n(int fd, unsigned char * buf, size_t bytes) {
  size_t total = 0;
  while(total < bytes) {
    ssize_t r = read(fd, &buf[total], bytes - total);
    if(r <= 0) {
      err("read");
      return -1;
    } else {
      total += r;
    }
  }
  /* protocol guarantees us, that the recv'd msg is zero-terminated */
  //buf[total - 1] = 0;
  return total;
}

void handle(int fd, const char *msg) {
  size_t size = strlen(msg) + 1;
  unsigned char reply[size];

  write(fd, msg, size);

  ssize_t err = read(fd, reply, size);
  if (err < 0) {
    err("read");
    return;
  }
  fprintf(stdout, "%s\n", reply);
}

int main(int argc, char *argv[]) {
  const char *progname = argv[0];
  const char *host;
  const char *port = "8053";
  const char *msg;

  static struct option longopts[] = { { "port", required_argument, NULL, 'p' },
                                      { NULL, 0, NULL, 0 } };

  int opt;
  while ((opt = getopt_long(argc, argv, "p:", longopts, NULL)) != -1) {
    switch (opt) {
    case 'p':
      port = optarg;
      break;
    case ':':
    case '?':
    default:
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (argc < optind) {
    fprintf(stderr, "%s: missing argument -- <server>\n", progname);
    usage(progname);
  }
  host = argv[optind++];
  if (argc < optind) {
    fprintf(stderr, "%s: missing argument -- <message>\n", progname);
    usage(progname);
  }
  msg = argv[optind];

  int s = open_socket(host, port);
  if (s == -1) {
    fprintf(stderr, "Cannot contact %s:%s\n", host, port);
    return EXIT_FAILURE;
  }

  handle(s, msg);
}
