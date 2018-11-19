#include <string.h>
#include <stdio.h>

struct myconfig_t {
    short is_daemon;
    int port;
    char *site_path;
    char *log_path;
};

void init_config(struct myconfig_t *conf);

int args_to_config(int argc, char *argv[], struct myconfig_t *conf);