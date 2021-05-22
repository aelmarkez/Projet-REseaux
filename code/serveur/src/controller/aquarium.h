#ifndef AQUARIUM_H
#define AQUARIUM_H

#include "utils.h"



struct aquarium {
  struct dimensions dim;
};

/*typedef struct aquarium_t {
	struct dimensions dim;
	int nb_fish;
	int nb_views;
	struct fish *fishes[MAX_FISHES];
	struct view *views[MAX_VIEWS];
} aquarium_t;*/




void aquarium_initialise(int w, int h);

int aquarium_load(char *aquarium_name);

int aquarium_save(char *aquarium_name);

void aquarium_show();

void aquarium_delete();

int aquarium_is_loaded();

struct dimensions aquarium_dim();


/***********************************************************/


#endif
