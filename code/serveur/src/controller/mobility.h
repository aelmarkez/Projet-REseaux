#ifndef MOBILITY_H
#define MOBILITY_H
#include <time.h>
#include "utils.h"


//Time in ms between each update
#define DT 100 //in ms
#define RANDOM_TIME_MAX 5000 //in ms
typedef long time_ms;

struct fish;
struct aquarium;

enum mobility {
			RANDOM_WAY_POINT,
			HORIZONTAL_WAY_POINT,
      VERTICAL_WAY_POINT,
			NO_MOBILITY
};

void initialise_mobility(struct aquarium *aquarium);
void delete_mobility();

int mobility_from_name(char *name);
void call_mobility_function(struct fish*);
int set_mobility(struct fish* fish, char *mobility);

#endif
