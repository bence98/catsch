#!/usr/bin/make -f

TARGET = catsch
OBJS = cat.o filelist.o rng.o rng/libc.o util.o
OBJS += main.o

.PHONY: all
all: ${TARGET}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJS}

${TARGET}: ${OBJS}
	${CC} -o $@ $^
