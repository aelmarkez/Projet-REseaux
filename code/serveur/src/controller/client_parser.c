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




void parse_hello(char *buffer, struct client *cl)
{
  if (strncmp(buffer, "hello in as ", 12) == 0) {
    if (set_available_view(buffer + 12, 0)) {
      strcpy(cl->name, buffer + 12);
    }
     else {
      const char *name = find_available_view();
      if (name != NULL) {
        strcpy(cl->name, name);
      }
    }
  }
  else if (strcmp(buffer, "hello") == 0) {
    const char *name = find_available_view();
    if (name != NULL) {
      strcpy(cl->name, name);
    }
  }

  if (cl->name[0] == '\0') {
    send(cl->socket, "no greeting\n", 12, 0);
    }else{
    char reply[MAX_BUFFER_SIZE];
    int reply_length;
    reply_length = snprintf(reply, MAX_BUFFER_SIZE,"greeting %s\n", cl->name);
    send(cl->socket, reply, reply_length, 0);
    reply[reply_length - 1] = '\0';
  }
}


void parse_add_fish(char *buffer, struct client *cl)
{ 
  char name[MAX_BUFFER_SIZE];
  int x, y, w, h;
  char m[MAX_BUFFER_SIZE];
  if (sscanf(buffer + 8, "%s at %dx%d, %dx%d, %s", name, &x, &y,
	     &w, &h, m) == 6) {

    struct view *v = find_view(cl->name);

    if (add_fish(name, v->origin.x + v->dim.width * x / 100, v->origin.y + v->dim.height * y / 100, v->dim.width * w / 100, v->dim.height * h / 100, m)) {
      send(cl->socket, "OK\n", 3, 0);

    }
    else {

      send(cl->socket, "NOK\n", 4, 0);
  }
}
}

void get_fishes(struct client *client)
{
  char *buffer;
  buffer = fishes_info(find_view(client->name));
  send(client->socket, "list\n", 5, 0);
  send(client->socket, buffer, strlen(buffer), 0);
  buffer[strlen(buffer) - 1] = '\0';
  free(buffer);
}


void parse_client_command(char *buffer, struct client *client)
{
  
  if (!strncmp(buffer, "ping ",5)) {
    char reply[MAX_BUFFER_SIZE];
    const int reply_length = snprintf(reply, MAX_BUFFER_SIZE - 1, "pong %s\n", buffer + 5);
    send(client->socket, reply, reply_length, 0);
    client->time_of_last_action = time(NULL);
  }
   else if (!strcmp(buffer, "log out")) {
    send(client->socket, "bye\n", 4, 0);
    disconnect_client(client);
    pthread_exit(NULL);
  }
  else {
    if (!aquarium_is_loaded()) {
      send(client->socket, "Aquarium is not loaded", 22, 0);
      return;
    }

    if (!strncmp(buffer, "hello", 5)) {
      parse_hello(buffer, client);
    }
    else if (!strcmp(buffer, "getFishes")) {
      get_fishes(client);
    }
     else if (!strcmp(buffer, "getFishesContinuously")) {
      get_fishes(client);
      client->updates = 1;
    }
     else if (!strncmp(buffer, "addFish ", 8)) {
      parse_add_fish(buffer, client);
    }
    else if (!strncmp(buffer, "delFish ", 8)) {
      char name[MAX_BUFFER_SIZE];
      if (sscanf(buffer + 8, "%s", name) == 1) {
	       if (delete_fish(name)) {
	          send(client->socket, "OK\n", 3, 0);
	        }
          else {
        	  send(client->socket, "NOK\n", 4, 0);
	      }
     }
    }
    else if (!strncmp(buffer, "startFish ", 10) ) {
      char name[MAX_BUFFER_SIZE];
      if (sscanf(buffer + 10, "%s", name) == 1) {

	       if (start_fish(name)) {

	          send(client->socket, "OK\n", 3, 0);
	         }
         else {

	          send(client->socket, "NOK\n", 4, 0);
	         }
        }
     }
  }
}
