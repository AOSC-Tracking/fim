# config
srcdir	= .
VPATH	= $(srcdir)
-include Make.config
include $(srcdir)/mk/Variables.mk

# add our flags + libs
CFLAGS	+= -DVERSION='"$(VERSION)"'
LDLIBS	+= -lm

# build
TARGETS	:= fbi exiftran

OBJS_FBI := \
	fbi.o fbtools.o fs.o fb-gui.o \
	jpegtools.o jpeg/transupp.o \
	dither.o loader.o filter.o op.o \
	ppm.o bmp.o
OBJS_EXIFTRAN := \
	exiftran.o jpegtools.o jpeg/transupp.o

# default target
all: build


#################################################################
# poor man's autoconf ;-)

include $(srcdir)/mk/Autoconf.mk

define make-config
LIB		:= $(LIB)
HAVE_ENDIAN_H	:= $(call ac_header,endian.h)
HAVE_X11	:= $(call ac_header,X11/X.h)
HAVE_LIBPCD	:= $(call ac_lib,pcd_open,pcd)
HAVE_LIBJPEG	:= $(call ac_lib,jpeg_start_compress,jpeg)
HAVE_LIBUNGIF	:= $(call ac_lib,DGifOpenFileName,ungif)
HAVE_LIBPNG	:= $(call ac_lib,png_read_info,png,-lz)
HAVE_LIBTIFF	:= $(call ac_lib,TIFFOpen,tiff)
HAVE_LIBEXIF	:= $(call ac_lib,exif_data_new_from_file,exif,-lm)
HAVE_LIBCURL	:= $(call ac_lib,curl_easy_init,curl)
HAVE_LIBLIRC	:= $(call ac_lib,lirc_init,lirc_client)
endef


########################################################################
# conditional stuff

ifeq ($(HAVE_X11),yes)
CFLAGS	+= -I/usr/X11R6/include -I/usr/X11R6/include/X11/fonts
LDFLAGS	+= -L/usr/X11R6/$(LIB)
LDLIBS	+= -lFS
OBJS	+= xwd.o
else
CFLAGS	+= -DX_DISPLAY_MISSING=1
endif

includes	= ENDIAN_H
libraries	= PCD JPEG UNGIF PNG TIFF EXIF CURL LIRC

PCD_LDLIBS	:= -lpcd
JPEG_LDLIBS	:= -ljpeg
UNGIF_LDLIBS	:= -lungif
PNG_LDLIBS	:= -lpng -lz
TIFF_LDLIBS	:= -ltiff
EXIF_LDLIBS	:= -lexif
CURL_LDLIBS	:= -lcurl
LIRC_LDLIBS	:= -llirc_client

PCD_OBJS	:= pcd.o
JPEG_OBJS	:= jpeg.o
UNGIF_OBJS	:= gif.o
PNG_OBJS	:= png.o
TIFF_OBJS	:= tiff.o
CURL_OBJS	:= curl.o
LIRC_OBJS	:= lirc.o

inc_cflags	:= $(call ac_inc_cflags,$(includes))
lib_cflags	:= $(call ac_lib_cflags,$(libraries))
CFLAGS		+= $(inc_cflags) $(lib_cflags)

OBJS_FBI	+= $(call ac_lib_mkvar,$(libraries),OBJS)
OBJS_EXIFTRAN	+= $(call ac_lib_mkvar,CURL,OBJS)

fbi      : LDLIBS += $(call ac_lib_mkvar,$(libraries),LDLIBS)
exiftran : LDLIBS += $(call ac_lib_mkvar,JPEG EXIF CURL,LDLIBS)

# catch fopen calls
ifeq ($(HAVE_LIBCURL),yes)
  CFLAGS	+= -D_GNU_SOURCE
  LDFLAGS	+= -Wl,--wrap=fopen
endif


########################################################################
# rules

build: $(TARGETS)

fbi: $(OBJS_FBI)
exiftran: $(OBJS_EXIFTRAN)

install: all
	$(INSTALL_DIR) $(bindir)
	$(INSTALL_BINARY) $(TARGETS) $(bindir)
	$(INSTALL_SCRIPT) fbgs $(bindir)
	$(INSTALL_DIR) $(mandir)/man1
	$(INSTALL_DATA) fbi.man $(mandir)/man1/fbi.1
	$(INSTALL_DATA) fbgs.man $(mandir)/man1/fbgs.1
	$(INSTALL_DATA) exiftran.man $(mandir)/man1/exiftran.1

clean:
	-rm -f $(OBJS_FBI) $(OBJS_EXIFTRAN) $(depfiles)

realclean distclean: clean
	-rm -f Make.config
	-rm -f $(TARGETS) *~ xpm/*~ *.bak


include $(srcdir)/mk/Compile.mk
include $(srcdir)/mk/Maintainer.mk
-include $(depfiles)


########################################################################
# maintainer stuff

COPY1 = loader.h loader.c filter.c filter.h op.c op.h misc.h \
	ppm.c xwd.c pcd.c bmp.c jpeg.c tiff.c png.c gif.c curl.h curl.c \
	jpegtools.c jpegtools.h exiftran.c exiftran.man
COPY2 = fbtools.c fbtools.h fs.c fs.h

sync::
	rm -rf $(COPY1) $(COPY2)
	set -x; for file in $(COPY1); do		\
		cp ../ida-*/$$file .;		\
	done
	for file in $(COPY2); do		\
		cp ../xawtv-4*/console/$$file .; \
	done
	chmod 444 $(COPY1) $(COPY2)
