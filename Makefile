#!/usr/bin/make -f

TARGET = catsch
OBJS = cat.o filelist.o rng.o util.o
OBJS += rng/libc.o rng/urandom.o
OBJS += rng/mod.o
OBJS += main.o

.PHONY: all
all: ${TARGET}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJS}

${TARGET}: ${OBJS}
	${CC} -o $@ $^

rng/prng_%.so: rng/%.o
	${CC} --shared -o $@ $^
