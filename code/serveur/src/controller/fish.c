#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "fish.h"
#include "view.h"
#include "queue.h"
#include "utils.h"
#include "mobility.h"
#include "string.h"


TAILQ_HEAD(fish_queue, fish);
//pthread_mutex_t mutex_fish;
//
//pthread_mutexattr_t attr;

struct fish_queue* fishes;


void initialise_fishes()
{
  fishes = malloc(sizeof(struct fish_queue));
  TAILQ_INIT(fishes);
  //pthread_mutexattr_init(&attr);
  //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  //pthread_mutex_init(&mutex_fish, &attr);
  printf("Fishes initialisation\n");
}

int add_fish(char *name, int x, int y, int w, int h, char* m)
{
  if (find_fish(name) != NULL) {
    return 0;
  }

  struct fish *fish = malloc(sizeof(struct fish));
  strcpy(fish->name, name);
	fish->pos.x = x;
	fish->pos.y = y;
	fish->dim.width = w;
	fish->dim.height = h;
  fish->arg = NULL;
	fish->is_started = 0;
  if (set_mobility(fish, m) == 0) {
    return 0;
}
  //pthread_mutex_lock(&mutex_fish);
  TAILQ_INSERT_TAIL(fishes, fish, entries);
  //pthread_mutex_unlock(&mutex_fish);
  return 1;
}

int delete_fish(char *name)
{
  struct fish *fish = find_fish(name);
  if (fish == NULL) {
    return 0;
  }
  //pthread_mutex_lock(&mutex_fish);
  TAILQ_REMOVE(fishes, fish, entries);
  //pthread_mutex_unlock(&mutex_fish);
  free(fish);
  free(fish->arg);
  return 1;
}

int start_fish(char *name)
{
  //pthread_mutex_lock(&mutex_fish);
  struct fish *f = find_fish(name);
  if (f == NULL || f->is_started == 1) {
    return 0;
  }
  f->is_started= 1;
  //pthread_mutex_unlock(&mutex_fish);
  return 1;
}

struct fish* find_fish(char *name)
{
  //pthread_mutex_lock(&mutex_fish);
  if (TAILQ_EMPTY(fishes) == 0) {
    struct fish *f = TAILQ_FIRST(fishes);
    for (; f != TAILQ_LAST(fishes, fish_queue); f = TAILQ_NEXT(f, entries)) {
      if (strcmp(f->name,name) == 0) {
	        return f;
      }
    }
    if (strcmp(f->name,name) == 0) {
      return f;
    }
  }
  //pthread_mutex_lock(&mutex_fish);
  return NULL;
}

void update_fishes()
{
    //pthread_mutex_lock(&mutex_fish);
    if (!TAILQ_EMPTY(fishes)) {
      struct fish* f;
      for (f = TAILQ_FIRST(fishes); f != TAILQ_LAST(fishes, fish_queue); f = TAILQ_NEXT(f, entries)) {
	       call_mobility_function(f);
      }
      call_mobility_function(f);
    }
    //pthread_mutex_unlock(&mutex_fish);
}




void delete_fishes()
{
  if (fishes == NULL) {
    return;
  }
  while (TAILQ_EMPTY(fishes) == 0) {
    struct fish* f = TAILQ_FIRST(fishes);
    delete_fish(f->name);
  }
  free(fishes);
  fishes = NULL;
  //pthread_mutex_destroy(&mutex_fish);
  //pthread_mutexattr_destroy(&attr);
}


int fish_inside_view(struct fish *fish, struct view *view)
{
  return (fish->pos.x >= view->origin.x && fish->pos.y >= view->origin.y
	  && fish->pos.x <= view->origin.x + view->dim.width
	  && fish->pos.y <= view->origin.y + view->dim.height)
    || (fish->pos.x + fish->dim.width >= view->origin.x
	&& fish->pos.x + fish->dim.width <= view->origin.x + view->dim.width
	&& fish->pos.y >= view->origin.y && fish->pos.y <= view->origin.y + view->dim.height)
    || (fish->pos.x >= view->origin.x && fish->pos.x <= view->origin.x + view->dim.width
	&& fish->pos.y + fish->dim.height <= view->origin.y + view->dim.height
	&& fish->pos.y + fish->dim.height >= view->origin.y)
    || (fish->pos.x + fish->dim.width <= view->origin.x + view->dim.width
	&& fish->pos.x + fish->dim.width >= view->origin.x
	&& fish->pos.y + fish->dim.height <= view->origin.y + view->dim.height
	&& fish->pos.y + fish->dim.height >= view->origin.y);
}

char *fish_info(struct view *view, struct fish *fish)
{
    char *info = malloc(MAX_BUFFER_SIZE * sizeof(char));
  snprintf(info, 350, "[%s at %dx%d, %dx%d, 0]", fish->name,
	   (fish->pos.x - view->origin.x) * 100 / view->dim.width,
	   (fish->pos.y - view->origin.y) * 100 / view->dim.height,
	   fish->dim.width * 100 / view->dim.width,
	   fish->dim.height * 100 / view->dim.height);
  return info;
}


char *fishes_info(struct view *view)
{
  //pthread_mutex_lock(&mutex_fish);
  struct fish *fish = NULL;
  char *info = malloc(MAX_BUFFER_SIZE * sizeof(char));
  strcpy(info, "list");
  TAILQ_FOREACH(fish, fishes, entries) {
    if (fish_inside_view(fish, view)) {
      char *fi = fish_info(view, fish);
      strcat(info, " ");
      strcat(info, fi);
    }
  }
  //pthread_mutex_unlock(&mutex_fish);
  strcat(info, "\n");
  return info;
}
