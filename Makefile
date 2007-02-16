
# All of this is in balance as an elephant on thin ice..
# it's horrible.

GCC=gcc
GPP=g++ -fpermissive -fno-default-inline -g
LD=ld

##########################################################
# add our flags + libs
#
srcdir	= ./src
#VPATH	= $(srcdir)
-include Make.config
#include $(srcdir)/mk/Variables.mk

#The following flags, if set, enable certain optional features.
DEFINES=-D FIM_DEFAULT_CONFIG -D FIM_DEFAULT_KEY_CONFIG #-D FIM_NOFB

CFLAGS	+= -DVERSION='"$(VERSION)"' $(DEFINES)
CPPFLAGS +=  $(CFLAGS)  $(DEFINES)
LDLIBS	+= -lreadline -lm -lfl -ltiff -ljpeg -lpng -lgif -lFS -lz

OBJS_FBI := \
	src/fbi.o src/fbtools.o src/fs.o src/fb-gui.o \
	src/dither.o src/loader.o src/filter.o src/op.o \
	src/ppm.o src/bmp.o

OBJS_FIM := \
	Arg.o Browser.o Command.o CommandConsole.o Image.o Var.o \
	common.o \
	fim.o interpreter.o \
	lex.yy.o \
	string.o yacc.tab.o

all:	fim
#################################################################
# poor man's autoconf ;-)

include $(srcdir)/mk/Autoconf.mk

#
# A partial automake
define make-config
HAVE_ENDIAN_H	:= $(call ac_header,endian.h)
HAVE_X11	:= $(call ac_header,X11/X.h)
HAVE_LIBJPEG	:= $(call ac_lib,jpeg_start_compress,jpeg)
HAVE_LIBUNGIF	:= $(call ac_lib,DGifOpenFileName,ungif)
HAVE_LIBPNG	:= $(call ac_lib,png_read_info,png,-lz)
HAVE_LIBTIFF	:= $(call ac_lib,TIFFOpen,tiff)
HAVE_LIBFS	:= $(call ac_lib,FSOpenServer,FS)
HAVE_LIBFOO	:= $(call ac_lib,FSOpenServer,foo)
endef


########################################################################

ifeq ($(HAVE_X11),yes)
CFLAGS	+= -I/usr/X11R6/include -I/usr/X11R6/include/X11/fonts
LDFLAGS	+= -L/usr/X11R6/$(LIB)
else
CFLAGS	+= -DX_DISPLAY_MISSING=1
endif

ifeq ($(HAVE_LIBTIFF),no)
$(error sorry, you should install libtiff first!)
endif

ifeq ($(HAVE_LIBFS),no)
$(error sorry, you should install libFS first!)
endif

ifeq ($(HAVE_LIBPNG),no)
$(error sorry, you should install libpng first!)
endif

ifeq ($(HAVE_LIBJPEG),no)
$(error sorry, you should install libjpeg first!)
endif

#We had some rudimental library existence check ..

ifeq ($(HAVE_LIBFOO),yes)
$(error "I can't believe it !, you have libFOO :) !")
endif

########################################################################


includes	= ENDIAN_H
#libraries	= PCD JPEG UNGIF PNG TIFF LIRC
libraries	= JPEG UNGIF PNG TIFF 

#PCD_LDLIBS	:= -lpcd
JPEG_LDLIBS	:= -ljpeg
UNGIF_LDLIBS	:= -lungif
PNG_LDLIBS	:= -lpng -lz
TIFF_LDLIBS	:= -ltiff
#LIRC_LDLIBS	:= -llirc_client

#PCD_OBJS	:= pcd.o
JPEG_OBJS	:= jpeg.o
UNGIF_OBJS	:= gif.o
PNG_OBJS	:= png.o
TIFF_OBJS	:= tiff.o
#LIRC_OBJS	:= lirc.o


inc_cflags	:= $(call ac_inc_cflags,$(includes))
lib_cflags	:= $(call ac_lib_cflags,$(libraries))
O=-O3
CFLAGS		+= $(inc_cflags) $(lib_cflags) $(O)


install: all
	#$(INSTALL_DIR) $(bindir)
	#$(INSTALL_BINARY) $(TARGETS) $(bindir)
	#$(INSTALL_SCRIPT) fbgs.sh $(bindir)
	#$(INSTALL_DIR) $(mandir)/man1
	#$(INSTALL_DATA) fim.man $(mandir)/man1/fbi.1
	#$(INSTALL_DATA) fbgs.sh.man $(mandir)/man1/fbgs.1

clean:
	rm -f $(OBJS_FBI) $(depfiles)  $(OBJS_FIM)  *.o fim -f src/*.o src/*/*.o yacc.output yacc.tab.cpp yacc.tab.hpp lex.yy.cc Make.config md5sums.md5
	@rm -f md5sums.md5 ; md5sum * */* */*/* */*/*/* > md5sums.md5 2> /dev/null || true

#realclean distclean:
#	@ rm -f Make.config
#	@ cd src ; rm -f $(TARGETS) *~ xpm/*~ *.bak
#	@ make md5
#	make clean

md5:
	@rm -f md5sums.md5 ; md5sum * */* */*/* */*/*/* > md5sums.md5 2> /dev/null || true

fim:	flex $(OBJS_FBI) $(OBJS_FIM)
	$(GPP) -o fim $(OBJS_FBI) $(OBJS_FIM) $(LDLIBS)
	@ strip fim

flex:
	flex -+ lex.lex
	bison -v  -d yacc.ypp
	$(GPP) -c lex.yy.cc
	$(GPP) -c yacc.tab.cpp

test:
	@./fim #~/M*s/*g

#all:  $(OBJS_FIM)
#fim: $(OBJS_FBI)  $(OBJS_FIM)

tgz:	clean
	#rm -f ../fim*.tgz && 
	tar -czf ../fim.`date +%Y%m%d%H%M`.tgz ../fim/*
	ls -l ../fim*.tgz  -v
	@#tar -tzf ../fim*.tgz  -v


report:
	@flex -V  
	@echo '-' ; 
	@bison -V
	@echo '-' ; 
	@gcc -v
	@echo '-' ; 
	@echo 'now please report the bug with this information to the author via email' ; 

exec:	test

edit:
	vim fim.cpp +':split fim.h'

.PHONY:
	@true

wc:
	wc $(FIM) 	# missing headers..

#clean:
#	@ make md5

