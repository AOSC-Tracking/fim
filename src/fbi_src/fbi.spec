%define prefix /usr

Name: fbi
Summary: linux FrameBuffer Imageviewer
Version: 1.26
Release: 0
Source0: fbi_%{version}.tar.gz
Group: Applications/Graphics
License: GPL
BuildRoot: /var/tmp/fbi-%{version}.root

%description
This is a image viewer for linux framebuffer devices.
It has PhotoCD, jpeg, ppm, gif, tiff, xwd, bmp and png
support built-in.  Unknown formats are piped throuth
convert (ImageMagick), which hopefully can handle it...

%prep
%setup -q

%build
export CFLAGS="$RPM_OPT_FLAGS"
make prefix=%{prefix}

%install
if test "%{buildroot}" != ""; then
	rm -rf "%{buildroot}"
fi
make prefix=%{prefix} DESTDIR="%{buildroot}" install

%files
%defattr(-,root,root)
%{prefix}/bin/*
%{prefix}/share/man/man1/*.1.gz
%doc INSTALL README COPYING

%clean
if test "%{buildroot}" != ""; then
	rm -rf %{buildroot}
fi
