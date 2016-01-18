# Copyright 1999-2007 Gentoo Foundation

# Fim custom ebuild by dezperado
# Distributed under the terms of the GNU General Public License v2

# $Id$

# Please note that this file is still experimental !
# 
# if you wonder how to use this ebuild :

# su
# FIM=fim-0.1
# wget http://www.autistici.org/dezperado/fim/${FIM}.tar.gz -O /usr/portage/distfiles/${FIM}.tgz
# mkdir -p /usr/local/portage/media-gfx/fim
# tar xvzf   /usr/portage/distfiles/${FIM}.tar.gz ${FIM}/distros/gentoo/media-gfx/fim/${FIM}.ebuild
# cd /usr/local/portage/media-gfx/fim
# ebuild  ${FIM}.ebuild digest
# echo PORTDIR_OVERLAY=/usr/local/portage >> /etc/make.conf
# emerge fim

# but read first any suggestions in the README file shipped with the Fim package!




# the description 
DESCRIPTION="FIM : Fbi IMproved is a framebuffer image viewer based on Fbi and inspired from Vim"

# the official web page
HOMEPAGE="http://www.autistici.org/dezperado/fim"

# the official mirror
SRC_URI="http://www.autistici.org/dezperado/fim/${P}.tar.gz"

# for testing
#SRC_URI="ftp://127.0.0.1/${P}.tar.gz"
#SRC_URI="http://code.autistici.org/svn/fim"


LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86"
IUSE=""
IUSE="png jpeg gif tiff fbcon pdf"

DEPEND="media-libs/libpng
	media-libs/jpeg
	media-libs/tiff
	media-libs/giflib
	sys-libs/readline
	sys-devel/flex
	sys-devel/bison"

# fim depends optionally on graphics libraries
RDEPEND="jpeg? ( >=media-libs/jpeg-6b )
	png? ( media-libs/libpng )
	gif? ( media-libs/giflib )
	pdf? ( virtual/ghostscript media-libs/tiff )
	tiff? ( media-libs/tiff )
	sys-libs/readline
	sys-devel/flex
	sys-devel/bison"
# but it is constrained on flex and bison !

RDEPEND=""

src_unpack() {
	unpack ${A}
	cd ${S}
}

src_compile() {
	# Let autoconf do its job and then fix things to build fbida
	# according to our specifications

	CONFIGURE_FLAGS=
	if use jpeg; then
		CONFIGURE_FLAGS="jpeg $CONFIGURE_FLAGS"
	else
		CONFIGURE_FLAGS="--disable-jpeg $CONFIGURE_FLAGS"
	fi

	if use png; then
		CONFIGURE_FLAGS="--with-png $CONFIGURE_FLAGS"
	else
		CONFIGURE_FLAGS="--disable-png $CONFIGURE_FLAGS"
	fi

	if use tiff ; then
		CONFIGURE_FLAGS="--with-tiff $CONFIGURE_FLAGS"
	else
		CONFIGURE_FLAGS="--disable-tiff $CONFIGURE_FLAGS"
	fi

	if use gif ; then
		CONFIGURE_FLAGS="--with-gif $CONFIGURE_FLAGS"
	else
		CONFIGURE_FLAGS="--disable-gif $CONFIGURE_FLAGS"
	fi

	econf $CONFIGURE_FLAGS ||  die "econf failed for ${P}"
	emake || die "emake failed for ${P}"
}

src_install() {
# this is wrong : gonna discover why
#	make install || die

# this is good : gonna discover why
        make \
                DESTDIR=${D} \
                prefix=/usr \
                install || die

#	if ! use pdf; then
#		rm -f ${D}/usr/bin/fimgs ${D}/usr/share/man/man1/fimgs.1
#	fi
}

# more docs for lamers :P :
# www.gentoo.org/devel/handbook.xml
# http://linuxreviews.org/gentoo/ebuilds
# man 5 ebuild
# 

