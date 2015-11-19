Format:         1.0
Source:         czmq
Version:        3.0.3-1
Binary:         czmq, libczmq3
Architecture:   any all
Maintainer:     John Doe <John.Doe@example.com>
Standards-Version: 3.9.5
Build-Depends: bison, debhelper (>= 8),
    pkg-config,
    automake,
    autoconf,
    libtool,
    libzmq4-dev,
    uuid-dev,
    dh-autoreconf

Package-List:
 czmq dev net optional arch-any
 libczmq3 dev net optional arch-any

