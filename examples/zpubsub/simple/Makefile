CC=gcc
CFLAGS=-pthread -std=gnu99 -pedantic -Werror -Wall -Wc++-compat -D_GNU_SOURCE -DLINUX -D_REENTRANT -DTHREAD_SAFE
LDFLAGS=-pthread
LDLIBS=-lzmq -lczmq
ZPUBSUB_SRC=../../../labs/zpubsub.c ../../../labs/zpubsub_option.c ../../../labs/zpubsub_filter.c
ZPUBSUB_OBJ=zpubsub.o zpubsub_option.o zpubsub_filter.o
PUB_SRC=test_publisher.c
PUB_OBJ=$(ZPUBSUB_OBJ) $(PUB_SRC:.c=.o)
SUB_SRC=test_subscriber.c
SUB_OBJ=$(ZPUBSUB_OBJ) $(SUB_SRC:.c=.o)
TARGETS=test_publisher test_subscriber

default: zpubsub
	make $(TARGETS)

zpubsub: $(ZPUBSUB_SRC)
	$(CC) $(CFLAGS) -c $^

test_publisher: $(PUB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(PUB_OBJ) $(LDLIBS)

test_subscriber: $(SUB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(SUB_OBJ) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)
