Format: 3.0 (quilt)
Binary: czmq
Source: czmq
Version: 4.2.2-0.1
Maintainer: czmq Developers <zeromq-dev@lists.zeromq.org>
Architecture: any
Build-Depends: debhelper-compat (= 12),
    pkgconf | pkg-config,
    libzmq3-dev,
    uuid-dev,
    libsystemd-dev (>= 200.0.0),
    liblz4-dev,
    libcurl4-gnutls-dev (>= 7.28.0),
    libnss3-dev,
    libmicrohttpd-dev,
    dh-python <!nopython>,
    python3-all-dev <!nopython>, python3-cffi <!nopython>, python3-setuptools <!nopython>,
    asciidoc-base <!nodoc>, xmlto <!nodoc>,

Files:
 7697688bf65a35bc33ae2db51ebb0e3b 818110 czmq.tar.gz
