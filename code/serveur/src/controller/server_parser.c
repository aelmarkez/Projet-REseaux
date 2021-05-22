#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "aquarium.h"
#include "client.h"
#include "client_parser.h"
#include "server_parser.h"
#include "view.h"
#include "queue.h"

#define MAX_BUFFER_SIZE 256

int terminate_program;
pthread_t client_th;



void parse_command(char *buffer)
{
  if (strncmp(buffer, "load ", 5) == 0) {
    if (aquarium_load(buffer + 5)) {
      printf("    -> aquarium loaded (%d display view)!\n", view_number());
    }
  } else if (strcmp(buffer, "show aquarium") == 0) {
    aquarium_show();
  } else if (strncmp(buffer, "save ", 5) == 0) {
    if (aquarium_save(buffer + 5)) {
      printf("    -> aquarium saved! (%d display view)\n", view_number());
    }
  } else if (strncmp(buffer, "add view ", 9) == 0) {
    char name[MAX_BUFFER_SIZE];
    int x, y, w, h;

    if (sscanf(buffer + 9, "%s %dx%d+%d+%d", name, &x, &y, &w, &h) == 5) {
      add_view(name, x, y, w, h);
      printf("    -> view added.");
    } else {
      printf("Invalid view.\n");
    }
  } else if (strncmp(buffer, "del view ", 9) == 0) {
    delete_view(buffer + 9);
    printf("    -> view %s deleted.\n", buffer + 9);
  } else {
    printf("Unknown command.\n");
  }
}


void create_server_parser()
{
  terminate_program = 0;
  pthread_create(&client_th, NULL, create_client, NULL);
  char buffer[MAX_BUFFER_SIZE];

  printf("$ ");
  while (fgets(buffer, MAX_BUFFER_SIZE, stdin) != NULL && !terminate_program) {
    char *p = strchr(buffer, '\n');
    if (p == NULL) {
      buffer[MAX_BUFFER_SIZE] = '\0';

    } else {
      *p = '\0';
    }
    parse_command(buffer);
    printf("$ ");
  }
}



int parse_config_file(const char *filename, struct controller_config *config)
{
  FILE *F = NULL;
  size_t size = 0;
  char *line = NULL;

  if ((F = fopen(filename, "r")) == NULL) {
    perror("fopen");
    return 0;
  }

  while (getline(&line, &size, F) != -1) {
    if (line[0] == '#') {
      continue;
    }

    char name[MAX_BUFFER_SIZE];
    char value[MAX_BUFFER_SIZE];

    if (sscanf(line, "%s = %s", name, value) == 2) {
      if (strcmp(name, "controller-port") == 0) {
	config->port = atoi(value);
      } else if (strcmp(name, "display-timeout-value") == 0) {
	config->display_timeout_value = atoi(value);
      } else if (strcmp(name, "fish-update-interval") == 0) {
	config->fish_update_interval = atoi(value);
      }
    }
  }


  free(line);
  fclose(F);

  return 1;
}


int parse_aquarium_file(const char *filename)
{
  FILE *F = fopen(filename, "r");
  if (F == NULL) {
    perror("fopen");
    return 0;
  }

  char *line = NULL;
  size_t size = 0;
  if (getline(&line, &size, F) == -1) {
    return 0;
  }

  int w, h;
  if (sscanf(line, "%dx%d", &w, &h) != 2) {
    perror("sscanf");
    return 0;
  }
  aquarium_initialise(w, h);
  while (getline(&line, &size, F) != -1) {
    char name[MAX_BUFFER_SIZE];
    int x, y, width, height;
    if (sscanf(line, "%s %dx%d+%d+%d", name, &x, &y, &width, &height) != 5) {
      perror("sscanf");
      continue;
    }
  // check if view dimensions are vaalid
    if (x < 0 || x + width > w || y < 0 || y + height > h) {
      fprintf(stderr, "View %s not loaded because dimensions are invalid.\n", name);
    }
    else{
      add_view(name, x, y, width, height);
    }
  }

  free(line);
  fclose(F);

  return 1;
}
