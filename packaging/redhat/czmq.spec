#
#    czmq - The high-level C binding for 0MQ
#
#    Copyright (c) the Contributors as noted in the AUTHORS file.
#    This file is part of CZMQ, the high-level C binding for 0MQ:
#    http://czmq.zeromq.org.
#
#    This Source Code Form is subject to the terms of the Mozilla Public
#    License, v. 2.0. If a copy of the MPL was not distributed with this
#    file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

# To build with draft APIs, use "--with drafts" in rpmbuild for local builds or add
#   Macros:
#   %_with_drafts 1
# at the BOTTOM of the OBS prjconf
%bcond_with drafts
%if %{with drafts}
%define DRAFTS yes
%else
%define DRAFTS no
%endif

# build with python_cffi support enabled
%bcond_with python_cffi
%if %{with python_cffi}
%define py2_ver %(python2 -c "import sys; print ('%d.%d' % (sys.version_info.major, sys.version_info.minor))")
%endif

# build with python3_cffi support enabled
%bcond_with python3_cffi
%if %{with python3_cffi}
%define py3_ver %(python3 -c "import sys; print ('%d.%d' % (sys.version_info.major, sys.version_info.minor))")
%endif

Name:           czmq
Version:        4.2.2
Release:        1
Summary:        the high-level c binding for 0mq
License:        MPL-2.0
URL:            https://github.com/zeromq/czmq
Source0:        %{name}-%{version}.tar.gz
Group:          System/Libraries
# Note: ghostscript is required by graphviz which is required by
#       asciidoc. On Fedora 24 the ghostscript dependencies cannot
#       be resolved automatically. Thus add working dependency here!
BuildRequires:  ghostscript
BuildRequires:  asciidoc
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  libtool
BuildRequires:  pkgconfig
BuildRequires:  xmlto
BuildRequires:  zeromq-devel
BuildRequires:  libuuid-devel
BuildRequires:  systemd-devel >= 200.0.0
BuildRequires:  liblz4-devel
BuildRequires:  libcurl-devel >= 7.28.0
BuildRequires:  nss-devel
BuildRequires:  libmicrohttpd-devel
%if %{with python_cffi}
BuildRequires:  python-cffi
BuildRequires:  python-devel
BuildRequires:  python-setuptools
%endif
%if %{with python3_cffi}
BuildRequires:  python3-devel
BuildRequires:  python3-cffi
BuildRequires:  python3-setuptools
%endif
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
czmq the high-level c binding for 0mq.

%package -n libczmq4
Group:          System/Libraries
Summary:        the high-level c binding for 0mq shared library

%description -n libczmq4
This package contains shared library for czmq: the high-level c binding for 0mq

%post -n libczmq4 -p /sbin/ldconfig
%postun -n libczmq4 -p /sbin/ldconfig

%files -n libczmq4
%defattr(-,root,root)
%{_libdir}/libczmq.so.*

%package devel
Summary:        the high-level c binding for 0mq
Group:          System/Libraries
Requires:       libczmq4 = %{version}
Requires:       zeromq-devel
Requires:       libuuid-devel
Requires:       systemd-devel >= 200.0.0
Requires:       liblz4-devel
Requires:       libcurl-devel >= 7.28.0
Requires:       nss-devel
Requires:       libmicrohttpd-devel

%description devel
the high-level c binding for 0mq development tools
This package contains development files for czmq: the high-level c binding for 0mq

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/libczmq.so
%{_libdir}/pkgconfig/libczmq.pc
%{_mandir}/man3/*
%{_mandir}/man7/*
# Install api files into /usr/local/share/zproject
%dir %{_datadir}/zproject/
%dir %{_datadir}/zproject/czmq
%{_datadir}/zproject/czmq/*

%if %{with python_cffi}
%package -n python2-czmq-cffi
Group: Python
Summary: Python CFFI bindings for czmq
Requires: python >= %{py2_ver}.0, python < 3.0.0

%description -n python2-czmq-cffi
This package contains Python CFFI bindings for czmq

%files -n python2-czmq-cffi
%{_libdir}/python%{py2_ver}/site-packages/czmq_cffi/
%{_libdir}/python%{py2_ver}/site-packages/czmq_cffi-*-py%{py2_ver}.egg-info/
%endif

%if %{with python3_cffi}
%package -n python3-czmq-cffi
Group: Python
Summary: Python 3 CFFI bindings for czmq
Requires: python = %{py3_ver}

%description -n python3-czmq-cffi
This package contains Python 3 CFFI bindings for czmq

%files -n python3-czmq-cffi
%{_libdir}/python%{py3_ver}/site-packages/czmq_cffi/
%{_libdir}/python%{py3_ver}/site-packages/czmq_cffi-*-py%{py3_ver}.egg-info/
%endif

%prep
#FIXME: error:... did not worked for me
%if %{with python_cffi}
%if %{without drafts}
echo "FATAL: python_cffi not yet supported w/o drafts"
exit 1
%endif
%endif

%setup -q

%build
sh autogen.sh
%{configure} --enable-drafts=%{DRAFTS} --with-uuid=yes --with-libsystemd=yes --with-liblz4=yes --with-libcurl=yes --with-nss=yes --with-libmicrohttpd=yes
make %{_smp_mflags}
%if %{with python_cffi} || %{with python3_cffi}
# Problem: we need pkg-config points to built and not yet installed copy of czmq
# Solution: chicken-egg problem - let's make "fake" pkg-config file
sed -e "s@^libdir.*@libdir=.libs/@" \
    -e "s@^includedir.*@includedir=include/@" \
    src/libczmq.pc > bindings/python_cffi/libczmq.pc
cd bindings/python_cffi
# This avoids problem with "weird" character quoting between shell and python3
ln -sfr ../../include/ .
ln -sfr ../../src/.libs/ .
export PKG_CONFIG_PATH=`pwd`
%endif
%if %{with python_cffi}
python2 setup.py build
%endif
%if %{with python3_cffi}
python3 setup.py build
%endif

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

# remove static libraries
find %{buildroot} -name '*.a' | xargs rm -f
find %{buildroot} -name '*.la' | xargs rm -f

%if %{with python_cffi} || %{with python3_cffi}
cd bindings/python_cffi
export PKG_CONFIG_PATH=`pwd`
%endif
%if %{with python_cffi}
python2 setup.py install --root=%{buildroot} --skip-build --prefix %{_prefix}
%endif
%if %{with python3_cffi}
python3 setup.py install --root=%{buildroot} --skip-build --prefix %{_prefix}
%endif

%files
%defattr(-,root,root)
%doc README.md
%doc README.txt
%{_bindir}/zmakecert
%{_mandir}/man1/zmakecert*

%changelog
* Wed Dec 31 2014 czmq Developers <zeromq-dev@lists.zeromq.org
- Initial packaging.
