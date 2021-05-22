#ifndef VIEW_H
#define VIEW_H
#include "utils.h"
#include "queue.h"
#define MAX_VIEWS 40



struct view {
  char name[256];
  int id;
	struct dimensions dim;
	struct coordinates origin;
	int is_available;

  TAILQ_ENTRY(view) entries;

};




void initialise_views();

void add_view(char *name, int x, int y, int w, int h);


void save_view(FILE *F, struct view *v);

void show_view(struct view *v);

void delete_view(char *name);

struct view* find_view(char *name);

char *find_available_view();

int set_available_view(char *name, int a);

void delete_views();

void show_views();

void save_views(FILE *F);

int view_number();

#endif
