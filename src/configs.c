#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "configs.h"

const char *help_message = "\n"
    "Usage:\n"
    "\tserver [-p port] [-s site_path] [-d [-l log_path]]\n"
    "\tserver [-h]\n"
    "\n"
    "\t-p: Change port number (default 80)\n"
    "\t-s: Change site path (default '/var/www/html)\n"
    "\t-d: Run as daemon\n"
    "\t-l: Change log path (default '/var/log/myhttpd.log')\n"
    "\t-h: Show this message\n";

void init_config(struct myconfig_t *conf){
    conf->is_daemon = 0;
    conf->port = 80;
    conf->log_path = "/var/log/myhttpd.log";
    conf->site_path = "/var/www/html";
}

int args_to_config(int argc, char *argv[], struct myconfig_t *conf){
    int i;
    init_config(conf);
    for(i = 1; i < argc; i++){
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
            i++;
            continue;
        }
        if(strcmp(argv[i], "-s") == 0) {
            if(argc == i + 1) {
                fprintf(stderr, "-s: missing site path");
                return -1;
            }
            conf->site_path = argv[i+1];
            i++;
            continue;
        }
        if(strcmp(argv[i], "-l") == 0) {
            if(argc == i + 1) {
                fprintf(stderr, "-l: missing log path");
                return -1;
            }
            conf->log_path = argv[i+1];
            i++;
            continue;
        }
        if(strcmp(argv[i], "-h") == 0) {
            fprintf(stderr, "%s", help_message);
            return -1;
        }
        fprintf(stderr, "Unexpected argument: %s", argv[i]);
        return -1;
    }
    return config(conf);
}

int config(struct myconfig_t *conf){
    pid_t pid;
    if(conf->is_daemon) {
        pid = fork();
        if(pid < 0) {
            perror("daemon:fork");
            return -1;
        }
        if(pid > 0) exit(1);
        if(make_daemon(conf) < 0) return -1;
    }
    else {
        if(chdir(conf->site_path) < 0){
            fprintf(stderr, "chdir to site %s : ", conf->site_path);
            perror("");
            return -1;
        }
    }
    return 0;
}

int make_daemon(struct myconfig_t *conf){
    int log_fd, i;
	pid_t pid;
    
    // open log file
    log_fd = open(conf->log_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if(log_fd < 0) {
        fprintf(stderr, "make_daemon:open log %s : ", conf->log_path);
        perror("");
        return -1;    
    }
	pid = fork();
    if(pid < 0) {
        perror("make_daemon:fork");
        return -1;
    }
	if(pid > 0) exit(1);

    // Create new process group
	if (setsid() < 0) {
        perror("make_daemon:setsid");
        return -1;
    }

    umask(0);

    // Create new process but not session leader
    pid = fork();
	if(pid < 0) {
        perror("make_daemon:fork 2");
        return -1;
    }
	if(pid > 0) exit(1);

    // Change Working Directory to site_path
	if(chdir(conf->site_path) < 0){
        fprintf(stderr, "chdir to site %s : ", conf->site_path);
        perror("");
        return -1;
    }

	// close off file descriptors
	for (i = 0; i < 3; i++)
		close(i);

	// redirect stdin to /dev/null 
	if (open("/dev/null", O_RDONLY) != 0) {
        perror("make_daemon:redirect stdin");
    }

    // redirect stdout, stderr to log file
    if (dup(log_fd) != 1) {
        perror("make_daemon:redirect stdout");
    }
	if (dup(log_fd) != 2) {
        perror("make_daemon:redirect stderr");
    }
    close(log_fd);
	return 0;
}