#include "czmq.h"

static char *pipe_name;
static int pipe_fd;
static void *ctx;
static zloop_t *loop;


int s_handle_pipe(zloop_t *loop, zmq_pollitem_t *poller, void *arg)
{
    char buffer[20], *endptr;
    unsigned eventmask;
    int retval;

    printf("s_handle_pipe revents=0x%x\n", poller->revents);

    // if 'echo 123 >pipe' is executed, this handler is called twice:
    // - first time when there's actually something to read
    // - a second time when the pipe write side is closed

    // depending on how libzmq is configured, revents may be subtly different
    // on pipe write side close:
    // if configured with --with-poller=poll|epoll, a ZMQ_POLLERR is signaled
    // if configured with --with-poller=select, a ZMQ_POLLIN  is signaled

    if (poller->revents & (ZMQ_POLLIN|ZMQ_POLLERR)) {
	// check if pipe readable
	retval = read(poller->fd, buffer, sizeof(buffer));
	if (retval > 0) {
	    printf("s_handle_pipe read(%d): '%.*s'\n", retval, retval, buffer);
	}
	if ((retval == 0) || (poller->revents & ZMQ_POLLERR)) {
	    printf("s_handle_pipe: pipe closed - re-registering poller\n");

	    // the write side was closed.
	    zloop_poller_end(loop, poller);
	    // close & reopen pipe,
	    close(poller->fd);
	    poller->fd = pipe_fd = open(pipe_name, O_RDONLY|O_NONBLOCK);
	    if (poller->fd < 0) {
		// real bad - pipe removed?
		printf("ERROR: reopening pipe %s : %s - disabling pipe notifications\n",
		       pipe_name,
		       strerror (errno));
		return 0;
	    }
	    // reestablish zloop poller.
	    zloop_poller(loop, poller, s_handle_pipe, NULL);

	} else if (retval < 0) {
	    // 'should not happen'
	    printf("ERROR: read(%s): %s - disabling pipe notifications\n",
		   pipe_name,
		   strerror (errno));
	}
    }
    return 0;
}


int main (int argc, char *argv[])
{
    int i, retval;

    pipe_name = argv[1];

    ctx = zctx_new ();
    assert(ctx != NULL);

    pipe_fd = open(pipe_name, O_RDONLY|O_NONBLOCK);
    assert(pipe_fd >= 0);

    loop = zloop_new();
    assert (loop);
    zloop_set_verbose (loop, 1);

    zmq_pollitem_t pipe_poller =      { 0, pipe_fd,   ZMQ_POLLIN };

    zloop_poller (loop, &pipe_poller, s_handle_pipe, 0);
    zloop_start(loop);
    exit(0);
}
