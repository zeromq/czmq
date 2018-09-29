AC_DEFUN([AX_PROJECT_LOCAL_HOOK], [
    AC_CHECK_HEADERS(pthread.h)
    AC_CHECK_LIB([pthread], [pthread_create],
        [CFLAGS="${CFLAGS} -pthread"],
        [AC_MSG_WARN([cannot link with -pthread.])]
    )

    AC_CACHE_CHECK([whether SOCK_CLOEXEC is supported], [czmq_cv_sock_cloexec],
        [AC_TRY_RUN([/* SOCK_CLOEXEC test */
#include <sys/types.h>
#include <sys/socket.h>

int main (int argc, char *argv [])
{
    int s = socket (PF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    return (s == -1);
}
        ],
        [czmq_cv_sock_cloexec="yes"],
        [czmq_cv_sock_cloexec="no"],
        [czmq_cv_sock_cloexec="not during cross-compile"]
        )]
    )
    AS_IF([test "x$czmq_cv_sock_cloexec" = "xyes"],
        AC_DEFINE([CZMQ_HAVE_SOCK_CLOEXEC],
            [1],
            [Whether SOCK_CLOEXEC is defined and functioning.])
    )
])
