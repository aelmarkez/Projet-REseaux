#ifndef FISH_H
#define FISH_H

#include "utils.h"
#include "queue.h"

#define MAX_BUFFER_SIZE 256

struct view;


struct fish {
	char name[MAX_BUFFER_SIZE];
	struct coordinates pos;
	struct dimensions dim;
	int is_started;
	char mobility[MAX_BUFFER_SIZE];
  void* (*mobility_func)(struct fish *);
  void *arg;
  TAILQ_ENTRY(fish) entries;
};

int add_fish(char *name, int x, int y, int w, int h, char* m) ;

int delete_fish(char* name);

int start_fish(char* name);

void update_fish();

void update_fishes();

int fish_inside_view(struct fish *fish, struct view *view);

char *fish_info(struct view *view, struct fish *fish);

char *fishes_info(struct view *view);


struct fish* find_fish(char* name);

void set_fishes();

void initialise_fishes();

void delete_fishes();

#endif
