// example of monitoring a sysfs entry with a zloop reactor
//
// sysfs has a way of notifying a user process that the content of a sysfs
// file has changed - the kernel function sysfs_notify() makes a
// sysfs entry pollable.
//
// sysfs_notify() will make a poll(2) return with the POLLPRI and POLLERR
// event bits set. However, by default czmq will interpret POLLERR
// on a file descriptor as fatal and disable the zloop callback, resulting
// in a once-only notification on a sysfs entry.
//
// the new ZMQ_IGNERR flag can be set in zmq_pollitem_t.events to suppress
// this behaviour.
//
// see also: http://lwn.net/Articles/174660/ for background on sysfs_notify()
//



#include "czmq.h"


static char *sysfs_name;
static int sysfs_fd;
static void *ctx;
static zloop_t *loop;

int s_handle_sysfs(zloop_t *loop, zmq_pollitem_t *poller, void *arg)
{
    char buffer[256], *endptr;
    unsigned eventmask;
    int retval;

    printf("s_handle_sysfs revents=0x%x\n", poller->revents);

    // with a sysfs device, poll returns POLLPRI|POLLERR which is mapped
    // to ZMQ_POLLERR by libzmq; same for select() version of libzmq.

    if (poller->revents & ZMQ_POLLERR) {
	lseek(poller->fd, 0, SEEK_SET);
	retval = read(poller->fd, buffer, sizeof(buffer));
	if (retval > 0) {
	    printf("s_handle_sysfs read(%d): '%.*s'\n", retval, retval, buffer);
	} else {
	    // pretty bad, but not much to do about it
	    printf("reading sysfs entry %s - %s : disabling sysfs notifications\n",
		   sysfs_name, strerror(retval));
	    zloop_poller_end (loop, poller);
	}
    }
    return 0;
}

int main (int argc, char *argv[])
{
    int i, retval;

    assert(argc > 1);
    sysfs_name = argv[1];

    ctx = zctx_new ();
    assert(ctx != NULL);

    sysfs_fd = open(sysfs_name, O_RDONLY);
    assert(sysfs_fd >= 0);

    loop = zloop_new();
    assert (loop);
    zloop_set_verbose (loop, 1);

    zmq_pollitem_t sysfs_poller =     { 0, sysfs_fd,  ZMQ_POLLERR };
    if (argc > 2) {
#ifdef  ZMQ_IGNERR
	sysfs_poller.events |= ZMQ_IGNERR;
	printf("ZMQ_IGNERR set on sysfs_poller\n");
#else
	printf("ZMQ_IGNERR not available in this czmq version.\n");
#endif
    }
    zloop_poller (loop, &sysfs_poller, s_handle_sysfs, 0);
    zloop_start(loop);
    exit(0);
}
