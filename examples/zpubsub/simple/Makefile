CC=gcc
CFLAGS=-pthread -std=gnu99 -pedantic -Werror -Wall -Wc++-compat -D_GNU_SOURCE -DLINUX -D_REENTRANT -DTHREAD_SAFE
LDFLAGS=-pthread
LDLIBS=-lzmq -lczmq_labs -lczmq 
PUB_SRC=test_publisher.c
PUB_OBJ=$(PUB_SRC:.c=.o)

SUB_SRC=test_subscriber.c
SUB_OBJ=$(SUB_SRC:.c=.o)

TARGETS=test_publisher test_subscriber

default: 
	make $(TARGETS)

test_publisher: $(PUB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(PUB_OBJ) $(LDLIBS)

test_subscriber: $(SUB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(SUB_OBJ) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)
