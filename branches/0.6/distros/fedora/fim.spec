Name:		fim
Version:	0.3
Release:	0.beta%{?dist}
Summary:	Highly customizable and scriptable image viewer
Group:		Applications/Multimedia
License:	GPLv2
URL:		https://www.autistici.org/dezperado/
Source0:	http://download.savannah.nongnu.org/releases/fbi-improved/fim-%{version}-beta.tar.gz
Source1:	https://www.autistici.org/dezperado/fim-%{version}-beta.tar.gz
Patch:		fim-0.3-beta-FontServer.patch
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:	readline-devel giflib-devel libpng-devel libjpeg-devel libtiff-devel
BuildRequires:	flex bison autoconf automake m4
Requires:	ImageMagick gimp inkscape

%description
FIM aims to be a highly customizable and scriptable image
viewer targeted at the users who are comfortable with software
like the Vim text editor or the Mutt mail user agent.

It is based on the Fbi image viewer, by Gerd Hoffmann, and works in the 
Linux framebuffer console ( so, it is not an X program! ).

%prep
%setup -q -n %{name}-%{version}-beta
# #include <climits>
%patch -p1

%build
%configure
make -j1

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/%{name}
mv $RPM_BUILD_ROOT/%{_prefix}/doc/%{name}-%{version}-beta/fimrc \
	$RPM_BUILD_ROOT/%{_datadir}/%{name}/
rm -rf $RPM_BUILD_ROOT/%{_prefix}/doc/%{name}-%{version}-beta/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc AUTHORS BUGS ChangeLog COPYING FAQ.TXT INSTALL NEWS README README.FIRST TODO VERSION
%{_bindir}/*
%{_mandir}/man1/*
%{_datadir}/%{name}

%changelog
* Sat Aug 30 2008 jeff <moe@blagblagblag.org> - 0.3.0-0.beta
- Initial spec

