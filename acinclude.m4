AC_DEFUN([AX_PROJECT_LOCAL_HOOK], [
    AC_CHECK_HEADERS(pthread.h)
    AC_CHECK_LIB([pthread], [pthread_create],
        [CFLAGS="${CFLAGS} -pthread"],
        [AC_MSG_WARN([cannot link with -pthread.])]
    )
])
