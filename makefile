all:
	gcc -o server_fork server_fork.c
	gcc -o server_select server_select.c

distclean:
	rm server_fork.c server_select.c
