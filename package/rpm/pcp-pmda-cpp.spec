#
# RPM Spec file for the PMDA++ project.
#

Summary: PMDA++ Library
Name: pcp-pmda-cpp
Version: 0.3.1
Release: 1%{?dist}
License: Boost Software License
Group: Development/Libraries
Source: pcp-pmda-cpp-%{version}.tar.gz
URL: https://github.com/pcolby/pcp-pmda-cpp

BuildRequires: boost >= 1.32
BuildRequires: cmake >= 2.6
BuildRequires: pcp-libs-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
This library is header-only, so the top-level pcp-pmda-cpp won't be built,
and this message entirely ignored (yet required) by rpmbuild.

%prep
%setup -c -q

%build
%{__rm} -rf %{buildroot}
%{__mkdir_p} build/devel
pushd build/devel
%cmake ../../pcp-pmda-cpp-%{version}/include
%{__make} %{?_smp_mflags}
popd
%{__mkdir_p} build/examples
pushd build/examples
%cmake ../../pcp-pmda-cpp-%{version}/example
%{__make} %{?_smp_mflags}
popd

%install
%{__make} install DESTDIR=%{buildroot} -C build/devel
%{__make} install DESTDIR=%{buildroot} -C build/examples

%clean
%{__rm} -rf %{buildroot}

%package devel
Summary: Development headers for the PMDA++ library.
Group: Development/Libraries
Requires: pcp-libs-devel
%if 0%{?rhel} >= 6
BuildArch: noarch
%endif

%description devel
PMDA++ is a header-only library that allows developers to write Performance
Metrics Domain Agents (PMDAs) for Performance Co-Pilot (PCP) in C++.

%package examples
Summary: Examples for the PMDA++ library.
Group: Development/Libraries
Requires: boost-program-options
Requires: pcp-libs

%description examples
Examples from the PMDA++ project.

%files devel
%{_includedir}/pcp-cpp

%files examples
%{_bindir}/pmda*
%{_datadir}/doc/pcp-cpp/examples/

%changelog
* Fri Feb 14 2014 Paul Colby <git@colby.id.au> - 0.3.1-1
- initial %{name} spec file.
