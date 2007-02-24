Name:           raceintospace
Version:        @VER@
Release:        1%{?dist}
Summary:        A strategy game

Group:          Games
License:        GPL
URL:            http://raceintospace.sourceforge.net/
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
A strategy game for going to the moon

%prep
%setup -q
configure --prefix=/usr


%build
make rpm_all

%install
rm -rf $RPM_BUILD_ROOT
make rpm_install DESTDIR=$RPM_BUILD_ROOT


%clean
echo rm -rf $RPM_BUILD_ROOT


%files
%defattr(755,root,root,-)
/usr/bin/raceintospace

%defattr(644,root,root,-)
/usr/man/man6/raceintospace.6.gz
/usr/share/doc/raceintospace/README

# the mkrpm script will append all of the data files to the
# end of this file, so nothing else can appear after here


