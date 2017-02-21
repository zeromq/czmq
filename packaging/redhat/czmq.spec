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
Name:           czmq
Version:        4.0.3
Release:        1
Summary:        the high-level c binding for 0mq
License:        MPLv2
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
BuildRequires:  systemd-devel
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
Requires:       systemd-devel

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
%{_datadir}/zproject/czmq/*.api

%prep
%setup -q

%build
sh autogen.sh
%{configure} --enable-drafts=%{DRAFTS}
make %{_smp_mflags}

%install
make install DESTDIR=%{buildroot} %{?_smp_mflags}

# remove static libraries
find %{buildroot} -name '*.a' | xargs rm -f
find %{buildroot} -name '*.la' | xargs rm -f

%files
%defattr(-,root,root)
%doc README.md
%doc README.txt
%{_bindir}/zmakecert
%{_mandir}/man1/zmakecert*

%changelog
