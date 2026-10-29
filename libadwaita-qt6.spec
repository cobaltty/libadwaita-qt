Name:           libadwaita-qt6
Version:        1.4.50
Release:        1%{?dist}
Summary:        Adwaita style plugin for Qt6
License:        LGPLv2+
URL:            https://github.com/trialuser02/adwaita-qt
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  cmake >= 3.17
BuildRequires:  ninja-build
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel >= 6.2.0
BuildRequires:  qt6-qtbase-private-devel
BuildRequires:  pkgconfig(Qt6DBus)

Provides:       adwaita-qt6 = %{version}-%{release}
Obsoletes:      adwaita-qt6 < 1.4.50

%description
A Qt6 style plugin that implements the GNOME Adwaita design language.
Provides flat, modern widget styling matching libadwaita for Qt applications.
Includes the style plugin (libadwaita.so) and shared libraries
(libadwaita6.so, libadwaita6priv.so).

%prep
%autosetup -n %{name}-%{version}

%build
%cmake -G Ninja \
    -DUSE_QT6=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLE=OFF
%cmake_build

%install
%cmake_install

# Remove .la files and static libs
find %{buildroot} -name "*.la" -delete 2>/dev/null || true

# Fix libtool archive
find %{buildroot} -name "*.a" -delete 2>/dev/null || true

%post
/sbin/ldconfig || :

%postun
/sbin/ldconfig || :

%files
%license LICENSE.LGPL2
%doc README.md
%{_libdir}/libadwaita6.so*
%{_libdir}/libadwaita6priv.so*
%{_qt6_plugindir}/styles/libadwaita.so
%{_libdir}/pkgconfig/libadwaita6.pc
%{_libdir}/cmake/AdwaitaQt6/
%{_includedir}/Adwaita/
