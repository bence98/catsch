#!/usr/bin/make -f

TARGET = catsch
OBJS = main.o

.PHONY: all
all: ${TARGET}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJS}

${TARGET}: ${OBJS}
	${CC} -o $@ $<
