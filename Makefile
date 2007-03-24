# $Id$

# All of this is in balance as an elephant on thin ice..
# it's horrible.
# 20070227 ..but i am fixing this :)
# 20070307 no more reference to libFS nor X11 !
#
##########################################################

#WARNINGS := -Wall -pedantic #-Wextra -pedantic
GCC=gcc
#GPP=g++ -fpermissive -fno-default-inline -g
GPP=g++ -fno-default-inline #-g $(WARNINGS)
LD=ld

#################################################################
# add our flags + libs
#
#srcdir	= ./src
srcdir	= ./
#VPATH	= $(srcdir)
-include Make.config
include $(srcdir)/mk/Variables.mk

#################################################################
# The following flags, if set, enable certain optional features at compile time.
# Remember to issue a 'make clean' before changing one of these and recompiling..

 DEFINES  :=
 DEFINES  += -D FIM_DEFAULT_KEY_CONFIG	# with a builtin default key mapping (advised)
 DEFINES  += -D FIM_DEFAULT_CONFIG	# with a builtin default (minimal) key binding
 DEFINES  += -D FIM_DEFAULT_CONFIGURATION # with a builtin default alias, autocommand, and binding set (complete)
 DEFINES  += -D FIM_NOFIMRC             # with ~/.fimrc loading disabled
#DEFINES  += -D FIM_NOSCRIPTING         # any script loading disabled
#DEFINES  += -D FIM_NOFB		# disable the framebuffer (for debug mostly)
 DEFINES  += -D FIM_AUTOCMDS		# autocommands
 DEFINES  += -D FIM_RECORDING		# command recording
#DEFINES  += -D FIM_SWITCH_FIXUP	# still bugful 
 DEFINES  += -D FIM_CKECK_DUPLICATES	# if enabled, no duplicates will be allowed in the filename list
 DEFINES  += -D FIM_CHECK_FILE_EXISTENCE # when a filename is added in the list, a verification occurs
 DEFINES  += -D FIM_REMOVE_FAILED	# a file which failed loading is removed from the list
 DEFINES  += -D FIM_AUTOSKIP_FAILED	# if (FIM_REMOVE_FAILED) after the file is removed, the next is tried
 DEFINES  += -D FIM_COMMAND_AUTOCOMPLETION  # An evil feature, right now

#################################################################

CFLAGS	 += -DVERSION='"$(VERSION)"' $(DEFINES)
CXXFLAGS +=  $(CFLAGS)  $(DEFINES) $(WARNINGS)
LDLIBS	 += -lreadline -lm -lfl -ltiff -ljpeg -lpng -lgif -lz
#-lrt (realtime library ) cannot be used :(  (causes harm!)

OBJS_FBI := \
	src/fbi.o src/fbtools.o src/fs.o src/fb-gui.o \
	src/dither.o src/loader.o src/filter.o src/op.o \
	src/ppm.o src/bmp.o

OBJS_FIM := \
	lex.yy.o \
	yacc.tab.o \
	Arg.o Browser.o Command.o CommandConsole.o Image.o Var.o \
	common.o \
	fim.o interpreter.o \
	string.o

FLEX   := flex
BISON  := bison
LEX    := $(FLEX)
YACC   := $(BISON)
LFLAGS := -+
YFLAGS := -v -d

all: fim


#
# The dependencies in the lexer/parser subsystem are tricky :
# 
# lex.yy.o   :  lex.yy.cc
# lex.yy.cc  :  lex.lex
# yacc.tab.o :  yacc.ypp lex.lex
#

#
# The canonical (procedural) commands for the lexer/parser:
#
#	$(LEX) $(LFLAGS) lex.lex
#	$(YACC) $(YFLAGS) yacc.ypp
#

# The goal-oriented way : 

%.yy.cc: %.lex
	$(LEX) $(LFLAGS) $<

%.tab.cpp: %.ypp lex.lex
	$(YACC) $(YFLAGS) $<

lex.yy.o : lex.yy.cc yacc.tab.cpp

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
#HAVE_LIBFS	:= $(call ac_lib,FSOpenServer,FS)
endef
##HAVE_LIBFOO	:= $(call ac_lib,FSOpenServer,foo)

HAVE_X11	:= # Now there is no more X11 nor libFS dependency!! :)
HAVE_LIBFS	:= 

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

#ifeq ($(HAVE_LIBFS),no)
#$(error sorry, you should install libFS first!)
#endif

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

ifneq ($(findstring FIM_DEFAULT_CONFIGURATION,$(DEFINES)),)
CommandConsole.o : conf.h
endif

ifneq ($(findstring FIM_DEFAULT_CONFIG,$(DEFINES)),)
CommandConsole.o : defaultConfiguration.cpp
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
JPEG_OBJS	:= src/jpeg.o
UNGIF_OBJS	:= src/gif.o
PNG_OBJS	:= src/png.o
TIFF_OBJS	:= src/tiff.o
#LIRC_OBJS	:= lirc.o


inc_cflags	:= $(call ac_inc_cflags,$(includes))
lib_cflags	:= $(call ac_lib_cflags,$(libraries))
O=-O3
CFLAGS		+= $(inc_cflags) $(lib_cflags) $(O)

TARGETS	= fim


OBJS_FBI	+= $(call ac_lib_mkvar,$(libraries),OBJS)

clean:
	rm -f $(OBJS_FBI) $(depfiles)  $(OBJS_FIM)  *.o fim -f src/*.o src/*/*.o yacc.output yacc.tab.cpp yacc.tab.hpp lex.yy.cc Make.config md5sums.md5
	@ rm -f md5sums.md5 ; md5sum * */* */*/* */*/*/* > md5sums.md5 2> /dev/null || true
	@ rm -f conf.h

#realclean distclean:
#	@ rm -f Make.config
#	@ cd src ; rm -f $(TARGETS) *~ xpm/*~ *.bak
#	@ make md5
#	make clean

md5:
	@rm -f md5sums.md5 ; md5sum * */* */*/* */*/*/* > md5sums.md5 2> /dev/null || true


conf.h:	fimrc
	echo 'char * FIM_DEFAULT_CONFIG_FILE_CONTENTS =' > conf.h   
	sed  's/"/\\\"/g;s/^/"/g;s/$$/\\n"/g;' fimrc >> conf.h
	echo '"";' >> conf.h

fim:    $(OBJS_FIM) $(OBJS_FBI)
	$(GPP) -o fim $(OBJS_FBI) $(OBJS_FIM) $(LDLIBS)
	@ strip fim

flex:
	flex  $(LFLAGS) lex.lex
	bison $(YFLAGS) yacc.ypp
	$(GPP) -c lex.yy.cc
	$(GPP) -c yacc.tab.cpp

install:	all
	$(INSTALL_DIR) $(bindir)
	$(INSTALL_BINARY) $(TARGETS) $(bindir)
	$(INSTALL_SCRIPT) fbgs.sh $(bindir)/fimgs
	#$(INSTALL_DIR) $(mandir)/man1
	#$(INSTALL_DATA) fim.man $(mandir)/man1/fbi.1
	#$(INSTALL_DATA) fbgs.sh.man $(mandir)/man1/fbgs.1

test:	fim
	@./fim media/* #~/M*s/*g

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
	$(EDITOR) fim.cpp +':split fim.h' # Vim ! :)

.PHONY:
	@true

wc:
	wc *.cpp *.h yacc.ypp lex.lex
	@#wc $(FIM) # missing headers..

#clean:
#	@ make md5
include $(srcdir)/mk/Compile.mk
include $(srcdir)/mk/Maintainer.mk
#-include $(depfiles)

#################################################################
