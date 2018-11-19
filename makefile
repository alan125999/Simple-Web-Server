CC := gcc
CFLAGS := -Wall

DIR_SRC := ./src
DIR_OBJ := ./obj
DIR_BIN := ./bin

SRC = $(wildcard ${DIR_SRC}/*.c)  
OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = server_fork server_select
BIN_TARGET = $(patsubst %,${DIR_BIN}/%,${TARGET})

all: ${BIN_TARGET}

${DIR_BIN}/server_select:${OBJ}
	$(CC) $(subst server_fork,,${OBJ}) -o ${BIN_TARGET}

${DIR_BIN}/server_fork:${OBJ}
	$(CC) $(subst server_select,,${OBJ}) -o ${BIN_TARGET}

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
    $(CC) $(CFLAGS) -c  $< -o $@

clean: 
	rm -rf ${DIR_OBJ}

distclean: clean
	rm -rf ${DIR_BIN}

.PHONY: all clean distclean