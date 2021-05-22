#ifndef CLIENT_H
#define CLIENT_H
#include "utils.h"
#include "queue.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "queue.h"

struct client{
  int socket;
  char name[256];
  time_t time_of_last_action;
  pthread_t thread;
  int connected;
  int updates;

  LIST_ENTRY(client) entries;
};


void add_client(int socket);

void disconnect_client(struct client *client);

void delete_client();

void *create_client(void *p);


#endif
