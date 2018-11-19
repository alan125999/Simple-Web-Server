#include <string.h>
#include <stdio.h>

#include "configs.h"

const char *help_message = "\n"
    "Usage:\n"
    "\tserver [-p port] [-d] [-l log_path] [-s site_path]"
    "\tserver [-h]"
    "\t-p: Change port number (default 80)"
    "\t-d: Run as daemon"
    "\t-l: Change log path (default '/var/log/myhttpd.log')"
    "\t-s: Change site path (default '/var/www/html)"
    "\t-h: Show this message";

void init_config(struct myconfig_t *conf){
    conf->is_daemon = 0;
    conf->port = 80;
    conf->log_path = "/var/log/myhttpd.log";
    conf->site_path = "/var/www/html";
}

int args_to_config(int argc, char *argv[], struct myconfig_t *conf){
    int i;
    for(i = 0; i < argc; i++){
        if(strcmp(argv[i], "-d") == 0) {
            conf->is_daemon = 1;
            continue;
        }
        if(strcmp(argv[i], "-p") == 0) {
            if(argc == i + 1) {
                fprintf(stderr, "-p: missing port number");
                return -1;
            }
            conf->port = atoi(argv[i+1]);
        }
        if(strcmp(argv[i], "-s") == 0) {
            if(argc == i + 1) {
                fprintf(stderr, "-s: missing site path");
                return -1;
            }
            conf->site_path = argv[i+1];
        }
        if(strcmp(argv[i], "-l") == 0) {
            if(argc == i + 1) {
                fprintf(stderr, "-s: missing log path");
                return -1;
            }
            conf->log_path = argv[i+1];
        }
        if(strcmp(argv[i], "-h") == 0) {
            fprintf(stderr, "-s: missing log path");
        }

    }
}