#ifndef UTILS_H
#define UTILS_H



extern int terminate_program;
extern int sock;


struct controller_config
{
  int port;
  int display_timeout_value;
  int fish_update_interval;
};

struct coordinates {
	int x;
	int y;
};

struct dimensions {
	int width;
	int height;
};


#endif
