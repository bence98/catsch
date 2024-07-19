#!/usr/bin/make -f

TARGET = catsch
OBJS = filelist.o rng.o rng/libc.o main.o

.PHONY: all
all: ${TARGET}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJS}

${TARGET}: ${OBJS}
	${CC} -o $@ $^
