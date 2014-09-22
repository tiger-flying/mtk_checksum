CFLAGS=-std=c99
CC=gcc

HEADERS=my_getopt.h my_linklist.h my_config.h
SRCS=my_getopt.c my_linklist.c my_config.c calculate_sum.c gen_sum.c
OBJS=$(patsubst %.c, %.o, $(notdir $(SRCS)))
TARGET=mtkchecksum
DEPS=$(HEADERS)

all: $(TARGET)

$(OBJS): $(DEPS) $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY:
	clean distclean
clean:
	rm -f *.o *.gch

distclean: clean
	rm -f $(TARGET)
