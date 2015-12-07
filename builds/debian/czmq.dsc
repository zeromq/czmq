Format:         1.0
Source:         CZMQ
Version:        3.0.3-1
Binary:         libczmq3, CZMQ-dev
Architecture:   any all
Maintainer:     John Doe <John.Doe@example.com>
Standards-Version: 3.9.5
Build-Depends: bison, debhelper (>= 8),
    pkg-config,
    automake,
    autoconf,
    libtool,
    libzmq4-dev,
    libuuid-dev,
    dh-autoreconf

Package-List:
 libczmq3 deb net optional arch=any
 CZMQ-dev deb libdevel optional arch=any

