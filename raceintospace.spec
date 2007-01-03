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


%build
make rpm_all

%install
rm -rf $RPM_BUILD_ROOT
make rpm_install DESTDIR=$RPM_BUILD_ROOT


%clean
echo rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
/usr/bin/raceintospace
/usr/share/man/man1/raceintospace.1.gz
/usr/share/doc/raceintospace/README
/usr/share/raceintospace/music/ASSEMBLY.ogg
/usr/share/raceintospace/music/ASTTRNG.ogg
/usr/share/raceintospace/music/BADNEWS.ogg
/usr/share/raceintospace/music/DRUMSM.ogg
/usr/share/raceintospace/music/ELEPHANT.ogg
/usr/share/raceintospace/music/FILLER.ogg
/usr/share/raceintospace/music/FUTURE.ogg
/usr/share/raceintospace/music/GOOD.ogg
/usr/share/raceintospace/music/HARDWARE.ogg
/usr/share/raceintospace/music/HISTORY.ogg
/usr/share/raceintospace/music/INTELLEG.ogg
/usr/share/raceintospace/music/INTEL.ogg
/usr/share/raceintospace/music/INTERLUD.ogg
/usr/share/raceintospace/music/LIFTOFF.ogg
/usr/share/raceintospace/music/MISSPLAN.ogg
/usr/share/raceintospace/music/NEW1950.ogg
/usr/share/raceintospace/music/NEW1970.ogg
/usr/share/raceintospace/music/PRES.ogg
/usr/share/raceintospace/music/PRGMTRG.ogg
/usr/share/raceintospace/music/R&D.ogg
/usr/share/raceintospace/music/SCCSETUP.ogg
/usr/share/raceintospace/music/SOVTYP.ogg
/usr/share/raceintospace/music/SUCCESS.ogg
/usr/share/raceintospace/music/SVFUN.ogg
/usr/share/raceintospace/music/SVLOGO.ogg
/usr/share/raceintospace/music/SVPORT1.ogg
/usr/share/raceintospace/music/SVPORT2.ogg
/usr/share/raceintospace/music/THEME.ogg
/usr/share/raceintospace/music/UNSUCC.ogg
/usr/share/raceintospace/music/USFUN.ogg
/usr/share/raceintospace/music/USMIL.ogg
/usr/share/raceintospace/music/USPORT1.ogg
/usr/share/raceintospace/music/USPORT2.ogg
/usr/share/raceintospace/music/USSRMIL.ogg
/usr/share/raceintospace/music/VICTORY.ogg


%changelog
