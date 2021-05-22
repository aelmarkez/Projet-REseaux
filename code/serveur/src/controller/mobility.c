#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "aquarium.h"
#include "utils.h"
#include "mobility.h"
#include "fish.h"




struct path_arg {
  time_ms arrival_time;
  struct coordinates end_point;
};


void *random_path(struct fish* fish);
void *horizontal_path(struct fish* fish);
void *vertical_path(struct fish* fish);

void random_arg(struct fish *fish);
void horizontal_arg(struct fish *fish);
void vertical_arg(struct fish *fish);
void no_mobility_arg(struct fish *fish);


static void *mobility_fun[3] = {random_path, horizontal_path, vertical_path};

static void *mobility_args[4] = {random_arg, horizontal_arg, vertical_arg, no_mobility_arg};
static char *mobility_names[3] = {"RandomWayPoint", "HorizontalPathWay", "VerticalPathWay"};

pthread_t update_thread;

static int mobility_status = 1;

struct dimensions aquarium_dimension;


int mobility_from_name(char *name)
{
  for (enum mobility f = 0; f != NO_MOBILITY; f++) {
    if (strcmp(name, mobility_names[f]) == 0) {
      return f;
    }
  }
  return NO_MOBILITY;
}


void *updater(void *arg)
{
  (void)arg;
  while (mobility_status == 0) {
    update_fishes();
    usleep(DT*1000);
  }
  pthread_exit(NULL);
}


void initialise_mobility(struct aquarium *aquarium)
{
  srand(time(NULL));
  aquarium_dimension = aquarium->dim;
  mobility_status = 0;
  pthread_create(&update_thread, NULL, &updater, NULL);
}


void delete_mobility()
{
  mobility_status = 1;
  pthread_join(update_thread, NULL);
}


time_ms get_time (void)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  time_ms t = ts.tv_nsec / 1000000 + ts.tv_sec*1000;
  return t;
}


void random_point(struct coordinates *p)
{
  p->x = rand() % aquarium_dimension.width;
  p->y = rand() % aquarium_dimension.height;
}


void *no_mobility(struct fish* fish)
{
  (void)fish;
  return NULL;
}



void *random_path(struct fish* fish)
{
  struct path_arg *r = fish->arg;

  if (r->arrival_time <= DT) {
    r->arrival_time = RANDOM_TIME_MAX;
    random_point(&r->end_point);
    return NULL;
  }

  time_ms t =  r->arrival_time / DT;
  fish->pos.x += (r->end_point.x - fish->pos.x) / t;
  fish->pos.y += (r->end_point.y - fish->pos.y) / t;
  r->arrival_time -= DT;
  return NULL;
}


void *horizontal_path(struct fish* fish)
{
  struct path_arg *r = fish->arg;

  if (r->arrival_time <= DT) {
    r->arrival_time = RANDOM_TIME_MAX;
    if(fish->pos.x > aquarium_dim().width / 2){
       r->end_point.x = 1;
     }
      else{
       r->end_point.x = aquarium_dim().width - fish->dim.width - 1;
      }
    return NULL;
  }

  time_ms t =  r->arrival_time / DT;
  fish->pos.x += (r->end_point.x - fish->pos.x) / t;
  r->arrival_time -= DT;
  return NULL;
}

void *vertical_path(struct fish* fish)
{
  struct path_arg *r = fish->arg;

  if (r->arrival_time <= DT) {
    r->arrival_time = RANDOM_TIME_MAX;
    if(fish->pos.y > aquarium_dim().height / 2){
       r->end_point.y = 1;
     }
      else{
       r->end_point.y = aquarium_dim().height - fish->dim.height - 1;
      }
    return NULL;
  }

  time_ms t =  r->arrival_time / DT;
  fish->pos.y += (r->end_point.y - fish->pos.y) / t;
  r->arrival_time -= DT;
  return NULL;
}


void random_arg(struct fish *fish)
{
  struct path_arg *p = malloc(sizeof(struct path_arg));
  p->arrival_time = 0;
  fish->arg = p;
}


void horizontal_arg(struct fish *fish)
{
  struct path_arg *p = malloc(sizeof(struct path_arg));
  p->arrival_time = DT;
  if(fish->pos.x > aquarium_dim().width / 2){
    p->end_point.x = 1;
  }
  else{
    p->end_point.x = aquarium_dim().width - fish->dim.width - 1;
  }
  p->end_point.y = fish->pos.y;
  fish->arg = p;
}

void vertical_arg(struct fish * fish){
  struct path_arg *p = malloc(sizeof(struct path_arg));
  p->arrival_time = DT;
  if(fish->pos.y > aquarium_dim().height / 2){
    p->end_point.y = 1;
  }
  else{
    p->end_point.y = aquarium_dim().height - fish->dim.height - 1;
  }
  p->end_point.x = fish->pos.x;
  fish->arg = p;

}

void no_mobility_arg(struct fish *fish)
{
   (void)fish;
 }


void call_mobility_function(struct fish *fish)
{
  if (fish->is_started) {
    fish->mobility_func(fish);
  }
}


int set_mobility(struct fish* fish, char *mobility)
{
  strcpy(fish->mobility, mobility);
  int f = mobility_from_name(mobility);

  if (f == NO_MOBILITY) {
    fish->mobility_func = &no_mobility;
    no_mobility_arg(fish);
    return 0;
  }

  fish->mobility_func = mobility_fun[f];
  void *(*param_arg)(struct fish*) = mobility_args[f];
  param_arg(fish);
  return 1;
}
