CZMQ_FLAGS := $(shell pkg-config libczmq libzmq --cflags --libs)

all: zloop-sysfs zloop-namedpipe sysfsexample.ko


zloop-namedpipe: zloop-namedpipe.c
	gcc -g $(CZMQ_FLAGS) -o $@ $^



zloop-sysfs: zloop-sysfs.c
	gcc -g $(CZMQ_FLAGS) -o $@ $^

obj-m := sysfsexample.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

sysfsexample.ko: sysfsexample.c
	make M=$(PWD) -C $(KDIR)

clean:
	rm -f zloop-sysfs zloop-namedpipe sysfsexample.ko *.o modules.order  Module.symvers
