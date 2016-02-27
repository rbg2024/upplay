Summary:        Linux UPnP audio control point
Name:           upplay
Version:        1.2.4
Release:        1%{?dist}
Group:          Applications/Multimedia
License:        GPLv2+
URL:            http://www.lesbonscomptes.com/upplay
Source0:        http://www.lesbonscomptes.com/upplay/upplay-%{version}.tar.gz
BuildRequires:  qt-devel
BuildRequires:  qtwebkit-devel
BuildRequires:  libupnpp
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%description
upplay is a simple audio control point which will let you play music on
your UPnP devices. It supports gapless transitions using either the
standard UPnP or OpenHome Playlist services.

%prep
%setup -q 

%build
qmake-qt4
make %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install INSTALL_ROOT=%{buildroot} STRIP=/bin/true INSTALL='install -p'
desktop-file-install --delete-original \
  --dir=%{buildroot}/%{_datadir}/applications \
  %{buildroot}/%{_datadir}/applications/%{name}.desktop

%clean
%{__rm} -rf %{buildroot}

%files
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/upplay.desktop
%{_datadir}/pixmaps/upplay.png
%{_datadir}/icons/hicolor/48x48/apps/upplay.png

%changelog
* Fri Feb 26 2016 Jean-Francois Dockes <jf@dockes.org> - 1.2.4
- Fixes to the AVTransport interface
* Fri Feb 05 2016 Jean-Francois Dockes <jf@dockes.org> - 1.2.3
- Small fixes. Needed update for upmpdcli 1.1.0
* Wed Dec 23 2015 Jean-Francois Dockes <jf@dockes.org> - 1.2.2
- Better reactions to openhome renderer source changes
* Fri Dec 18 2015 Jean-Francois Dockes <jf@dockes.org> - 1.2.1
- Small fixes for Songcast tool.
* Mon Dec 14 2015 Jean-Francois Dockes <jf@dockes.org> - 1.2.0
- OpenHome Radio service. Songcast tool.
* Sat Nov 14 2015 Jean-Francois Dockes <jf@dockes.org> - 1.1.0
- Random play, more preferences.
* Sun Feb 22 2015 Jean-Francois Dockes <jf@dockes.org> - 1.0.0
- Minor improvements (sorting, volume control)
* Sun Feb 22 2015 Jean-Francois Dockes <jf@dockes.org> - 0.10.0
- Initial
