#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#include "client.h"

void parse_hello(char *buffer, struct client *cl);

void parse_add_fish(char *buffer, struct client *cl);

void get_fishes(struct client *client);

void parse_client_command(char *buffer, struct client *client);














#endif
