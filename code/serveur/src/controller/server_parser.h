#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include "utils.h"

void create_server_parser();
void parse_command();
int parse_config_file(const char *filename, struct controller_config *config);
int parse_aquarium_file(const char *filename);
void save_aquarium();
void show_aquarium();
void add_view();
void del_view();



#endif
