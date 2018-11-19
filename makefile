CC := gcc
CFLAGS := -Wall

DIR_SRC := ./src
DIR_OBJ := ./obj
DIR_BIN := ./bin
DIR_CREATE := ${DIR_OBJ} ${DIR_BIN}
SRC = $(wildcard ${DIR_SRC}/*.c)  
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = server_fork server_select
BIN_TARGET = $(patsubst %,${DIR_BIN}/%,${TARGET})

all: ${DIR_CREATE} ${BIN_TARGET} 

${DIR_CREATE}:
	mkdir $@

${DIR_BIN}/server_fork:${OBJ}
	$(CC) $(subst ${DIR_OBJ}/server_select.o,,${OBJ}) -o $@

${DIR_BIN}/server_select:${OBJ}
	$(CC) $(subst ${DIR_OBJ}/server_fork.o,,${OBJ}) -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

clean: 
	rm -rf ${DIR_OBJ}

distclean: clean
	rm -rf ${DIR_BIN}

.PHONY: all clean distclean