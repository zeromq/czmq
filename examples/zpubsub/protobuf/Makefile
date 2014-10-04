CC = g++
PROTO = protoc
CFLAGS = -g -pthread
LDFLAGS = -g -pthread
LDLIBS = -lzmq -lczmq -lprotobuf -lpthread
ZPUBSUB_SRC=../../../labs/zpubsub.c ../../../labs/zpubsub_option.c ../../../labs/zpubsub_filter.c
ZPUBSUB_OBJ=zpubsub.o zpubsub_option.o zpubsub_filter.o
LIB_SRC = topic.cc subscriber.cc participant.cc
PUB_SRC = $(LIB_SRC) testMessage.pb.cc testPublisher.cc
PUB_OBJ = $(ZPUBSUB_OBJ) $(PUB_SRC:.cc=.o)
SUB_SRC = $(LIB_SRC) testMessage.pb.cc testSubscriber.cc
SUB_OBJ = $(ZPUBSUB_OBJ) $(SUB_SRC:.cc=.o)
TARGETS = testPublisher testSubscriber

default: zpubsub
	make $(TARGETS)

zpubsub: $(ZPUBSUB_SRC)
	$(CC) $(CFLAGS) -c $^

testPublisher: $(PUB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(PUB_OBJ) $(LDLIBS)

testSubscriber: $(SUB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(SUB_OBJ) $(LDLIBS)

%.pb.cc: %.proto
	$(PROTO) --cpp_out=. $<

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.pb.* $(TARGETS)
