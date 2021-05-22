#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "aquarium.h"
#include "server_parser.h"
#include "utils.h"
#include "view.h"
#include "fish.h"
#include "mobility.h"


struct aquarium aquarium;
int aquariums_loaded = 0;

void aquarium_initialise(int w, int h)
{
  aquarium.dim.width = w;
  aquarium.dim.height = h;
  initialise_views();
  initialise_fishes();
  initialise_mobility(&aquarium);
  aquariums_loaded = 1;
}

int aquarium_is_loaded(){
  return aquariums_loaded;
}


int aquarium_load(char *aquarium_name)
{
  if (aquariums_loaded) {
    fprintf(stderr, "An aquarium is already loaded!\n");
    return 0;
  }

  char path[PATH_MAX];

  if (getcwd(path, sizeof(path)) == NULL) {
    perror("getcwd");
    return 0;
  }

  strncat(path, "/", sizeof(path) - 1);
  strncat(path, aquarium_name, sizeof(path) - 1);
  const int success = parse_aquarium_file(path);

  return success;
}


int aquarium_save(char *aquarium_name)
{
  if (!aquariums_loaded) {
    fputs("No aquarium loaded\n", stderr);
    return 0;
  }
  char path[PATH_MAX];

  if (getcwd(path, sizeof(path)) == NULL) {
    perror("getcwd() error");
    return 0;
  }

  strncat(path, "/", sizeof(path)-1);
  strncat(path, aquarium_name, sizeof(path)-1);
  FILE *F = fopen(path, "w");

  fprintf(F, "%dx%d\n", aquarium.dim.width, aquarium.dim.height);
  save_views(F);
  fclose(F);

  return 1;
}


void aquarium_show()
{
  if (!aquariums_loaded) {
    fputs("No aquarium loaded\n", stderr);
    return;
  }

  printf("%dx%d\n", aquarium.dim.width, aquarium.dim.height);
  show_views();
}


void aquarium_delete()
{
  if (!aquariums_loaded) {
    return;
  }

  delete_fishes();
  delete_views();
  delete_mobility();
  aquariums_loaded = 0;
}

struct dimensions aquarium_dim()
{
  return aquarium.dim;
}
