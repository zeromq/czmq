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

Name:           czmq
Version:        3.0.3
Release:        1
Summary:        the high-level c binding for 0mq
License:        MIT
URL:            http://example.com/
Source0:        %{name}-%{version}.tar.gz
Group:          System/Libraries
BuildRequires:  automake
BuildRequires:  autoconf
BuildRequires:  libtool
BuildRequires:  pkg-config
BuildRequires:  zeromq-devel
BuildRequires:  uuid-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
czmq the high-level c binding for 0mq.

%package -n libczmq3
Group:          System/Libraries
Summary:        the high-level c binding for 0mq

%description -n libczmq3
czmq the high-level c binding for 0mq.
This package contains shared library.

%post -n libczmq3 -p /sbin/ldconfig
%postun -n libczmq3 -p /sbin/ldconfig

%files -n libczmq3
%defattr(-,root,root)
%doc COPYING
%{_libdir}/libczmq.so.*

%package devel
Summary:        the high-level c binding for 0mq
Group:          System/Libraries
Requires:       libczmq3 = %{version}
Requires:       zeromq-devel
Requires:       uuid-devel

%description devel
czmq the high-level c binding for 0mq.
This package contains development files.

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/libczmq.so
%{_libdir}/pkgconfig/libczmq.pc

%prep
%setup -q

%build
sh autogen.sh
%{configure}
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

%changelog
