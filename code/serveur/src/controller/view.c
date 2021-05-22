#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "view.h"
#include "utils.h"


TAILQ_HEAD(view_queue, view);

struct view_queue* views = NULL;
static pthread_mutex_t mutex_views;

int nbr_views = 0;

void initialise_views()
{
  views = malloc(sizeof(struct view_queue));
  TAILQ_INIT(views);
}

void add_view(char *name, int x, int y, int w, int h)
{
  struct view *v = malloc(sizeof(struct view));
  strcpy(v->name, name);
  v->dim.height = h;
  v->dim.width = w;
  v->origin.x = x;
  v->origin.y = y;
  v->id = nbr_views++;
  v->is_available = 1;
  TAILQ_INSERT_TAIL(views, v, entries);
}


void save_view(FILE *F, struct view *v)
{
  if (v != NULL) {
    fprintf(F, "%s %dx%d+%d+%d\n", v->name, v->origin.x, v->origin.y, v->dim.width, v->dim.height);
  }
}

void show_view(struct view *v) {
  if (v != NULL) {
    printf("%s %dx%d+%d+%d\n", v->name, v->origin.x, v->origin.y, v->dim.width, v->dim.height);
  }
}

void delete_view(char *name)
{
  struct view* v = NULL;
  TAILQ_FOREACH(v, views, entries) {
    if (strcmp(v->name, name) == 0) {
      TAILQ_REMOVE(views, v, entries);
      nbr_views--;
      free(v);
      return;
    }
  }
}

struct view* find_view(char *name) {
  
  
  struct view* v = NULL;
  TAILQ_FOREACH(v, views, entries) {

    if (strcmp(v->name, name) == 0) {
        
        break;
    }
  }
  //pthread_mutex_unlock(&mutex_views);
  
  return v;
}


char *find_available_view()
{
  //pthread_mutex_lock(&mutex_views);
  struct view *v = NULL;
  TAILQ_FOREACH(v, views, entries) {
    if (v->is_available) {
      v->is_available = 0;
      return v->name;
    }
  }
  //pthread_mutex_unlock(&mutex_views);
  return 0;
}


int set_available_view(char *name, int a)
{
 // pthread_mutex_lock(&mutex_views);
  struct view *v = NULL;
  TAILQ_FOREACH(v, views, entries) {
    if (strcmp(v->name, name) == 0) {
      if (v->is_available != a) {
	        v->is_available = a;
	        return 1;
      }
      break;
    }
  }
  //pthread_mutex_unlock(&mutex_views);
  return 0;
}

void delete_views()
{
  if (views == NULL) {
    return;
  }
 // pthread_mutex_lock(&mutex_views);

  while (!TAILQ_EMPTY(views)) {
    struct view * v = TAILQ_FIRST(views);
    TAILQ_REMOVE(views, v, entries);
    nbr_views--;
    free(v);
  }
 // pthread_mutex_unlock(&mutex_views);

  free(views);
}

void show_views()
{
 // pthread_mutex_lock(&mutex_views);

  if (TAILQ_EMPTY(views) == 0) {
    struct view* v = TAILQ_FIRST(views);
    for (; v != TAILQ_LAST(views, view_queue); v = TAILQ_NEXT(v, entries)) {
      show_view(v);
    }
    show_view(TAILQ_LAST(views, view_queue));
  //pthread_mutex_unlock(&mutex_views);

  }
}

void save_views(FILE *F)
{
  //pthread_mutex_lock(&mutex_views);

  if (!TAILQ_EMPTY(views)) {

    struct view* v = TAILQ_FIRST(views);
    for (; v != TAILQ_LAST(views, view_queue); v = TAILQ_NEXT(v, entries)) {
      save_view(F, v);
    }
    save_view(F, TAILQ_LAST(views, view_queue));
  }
  //pthread_mutex_unlock(&mutex_views);

}

int view_number()
{
  return nbr_views;
}
