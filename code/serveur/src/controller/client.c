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

#include "aquarium.h"
#include "fish.h"
#include "client_parser.h"
#include "server_parser.h"
#include "queue.h"
#include "view.h"



#define MAX_BUFFER_SIZE 256


int terminate_program;
int sock;
struct controller_config c;


LIST_HEAD(client_queue, client);

struct client_queue *clients;

void *client_thread(void *a);


void add_client(int socket)
{
  struct client *client = malloc(sizeof(struct client));
  client->socket = socket;
  client->name[0] = '\0';
  client->time_of_last_action = time(NULL);
  client->connected = 1;

  LIST_INSERT_HEAD(clients, client, entries);

  pthread_create(&client->thread, NULL, client_thread, client);
}


void disconnect_client(struct client *client)
{
  if (client->name[0] != '\0') {
    set_available_view(client->name, 1);
  }
  client->connected = 0;
  close(client->socket);
  printf("\nClient %d disconnected\n$ ", client->socket);
}


void delete_client()
{
  struct client *client = NULL;
  while (!LIST_EMPTY(clients)) {
    client = LIST_FIRST(clients);
    LIST_REMOVE(client, entries);
    pthread_join(client->thread, NULL);
    if (client->connected) {
      disconnect_client(client);
    }
    free(client);
  }
  free(clients);
}





void *client_thread(void *a)
{
  struct client *client = a;
  char buffer[MAX_BUFFER_SIZE];
  ssize_t nb_bytes;

  while (!terminate_program) {

    if (client->updates) {
      get_fishes(client);
      usleep((c.fish_update_interval)*1000);
    }    

    
    if ((nb_bytes = recv(client->socket, buffer, MAX_BUFFER_SIZE, 0)) == -1) {
      if (errno != EAGAIN) {
	perror("recv");
	if (errno == ECONNRESET) {
	  disconnect_client(client);
	  break;
	}
      }
    } else {
      if (nb_bytes == 0) {
	disconnect_client(client);
	break;
      } else {
	buffer[nb_bytes - 1] = '\0';
	parse_client_command(buffer, client);
      }
      client->time_of_last_action = time(NULL);
    }
  }

  pthread_exit(NULL);
}


void *create_client(void *p)
{
  (void)p;
  parse_config_file("controller.cfg", &c);

  const int port = c.port;
  int opt = 1;
  struct sockaddr_in sin;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		 &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(sock, 5) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  clients = malloc(sizeof(struct client_queue));
  LIST_INIT(clients);

  while (!terminate_program) {
    struct sockaddr_in client_sin;
    socklen_t len = sizeof(client_sin);
    const int client_sock = accept(sock, (struct sockaddr *)&client_sin, &len);
    fprintf(stderr,"%d",client_sock);
    if (client_sock == -1) {
      if (!terminate_program) {
	perror("accept");
      }
    } else {
      fcntl(client_sock, F_SETFL, O_NONBLOCK);
      char *ip = inet_ntoa(client_sin.sin_addr);
      printf("\nConnection established with client %s, id = %d\n$ ", ip, client_sock);
      add_client(client_sock);
    }
  }

  delete_client();
  close(sock);

  pthread_exit(NULL);
}
