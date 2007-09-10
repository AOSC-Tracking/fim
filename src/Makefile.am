# This file should be processed with automake to generate Makefile.in
#
# $Id$
#
# inspired by mutt's Makefile.am 

subdir = src

AUTOMAKE_OPTIONS = nostdinc no-dependencies
SUBDIRS = 

bin_PROGRAMS = fim

bin_SCRIPTS = fimgs.sh

doc_DATA = fimrc

lex.yy.o  : lex.yy.cc
#lex.yy.cc: lex.lex

lex.yy.cc: lex.lex
	$(LEX) $(LFLAGS) lex.lex

LFLAGS = -+
YFLAGS = -v -d

yacc.tab.cpp: yacc.ypp lex.lex
	$(YACC) $(YFLAGS) yacc.ypp -o yacc.tab.cpp

FIM_LIB_OBJECTS = yacc.tab.o lex.yy.o

#lex.yy.cc : lex.lex

#lex.yy.o : lex.yy.cc yacc.tab.cpp

#		lex.lex 

conf.h:       fimrc
	echo 'char * FIM_DEFAULT_CONFIG_FILE_CONTENTS =' > conf.h
	sed  's/"/\\\"/g;s/^/"/g;s/$$/\\n"/g;' fimrc >> conf.h
	echo '"";' >> conf.h

# the following sources are built at make time
BUILT_SOURCES = lex.yy.cc \
	yacc.tab.cpp conf.h yacc.tab.hpp 

# this hook triggers on 'make dist'
dist-hook:
	-for file in $(BUILT_SOURCES) ; do rm -f $(distdir)/$$file ; done

EXTRA_fim_SOURCES = 
EXTRA_fim_SOURCES += fbi_src/png.c
EXTRA_fim_SOURCES += fbi_src/jpeg.c
EXTRA_fim_SOURCES += fbi_src/tiff.c
EXTRA_fim_SOURCES += fbi_src/gif.c


CLEANFILES=$(BUILT_SOURCES) yacc.output 


fim_SOURCES  = $(BUILT_SOURCES) \
		fim.cpp \
	      fim.h \
	      Arg.h \
	Browser.h \
	Cache.h \
	Command.h \
	CommandConsole.h \
	Image.h \
	Var.h \
	Viewport.h \
	Window.h \
	common.h \
	conf.h \
	extern.h \
	fim.h \
	fim_stream.h \
	fim_stream.cpp \
	fimgs.sh \
	lex.h \
	string.h \
	Arg.cpp \
	Browser.cpp \
	Cache.cpp \
	Command.cpp \
	CommandConsole.cpp \
	Image.cpp \
	Var.cpp \
	Viewport.cpp \
	Window.cpp \
	common.cpp \
	interpreter.cpp \
	string.cpp \
	fbi_src/bmp.c \
	fbi_src/dither.c \
	fbi_src/fb-gui.c \
	fbi_src/fbi.c \
	fbi_src/fbi.spec \
	fbi_src/fbtools.c \
	fbi_src/filter.c \
	fbi_src/loader.c \
	fbi_src/op.c \
	fbi_src/ppm.c \
	fbi_src/fs.c \
	fbi_src/fs.h \
	fbi_src/config.h \
	fbi_src/dither.h \
	fbi_src/fb-gui.h \
	fbi_src/fbtools.h \
	fbi_src/filter.h \
	fbi_src/lirc.h \
	fbi_src/list.h \
	fbi_src/loader.h \
	fbi_src/misc.h \
	fbi_src/op.h

#fim_SOURCES += fbi_src/tiff.c
#fim_SOURCES += fbi_src/gif.c
#fim_SOURCES += fbi_src/jpeg.c

# jpegtools.h

# regex.c is work in progress and awaits for integration
EXTRA_DIST = lex.lex yacc.ypp fimrc defaultConfiguration.cpp fimgs.sh \
	regex.c \
	fbi_src/tiff.c \
	fbi_src/gif.c \
	fbi_src/jpeg.c \
	fbi_src/png.c

#	fbi_src/jpegtools.c \
#	fbi_src/exiftran.c 
#	fbi_src/xwd.c
#	fbi_src/pcd.c 
#	fbi_src/lirc.c 
#
#fim_cpp_CPP_FLAGS=-DFIM_VERSION -DFIM_AUTHOR
#CPP_FLAGS="$CPP_FLAGS -D X_DISPLAY_MISSING"

 DEFINES  =
# The following defines already migrated outta here...
#DEFINES  += -D FIM_DEFAULT_KEY_CONFIG  # with a builtin default key mapping (advised)
DEFINES  += -D FIM_DEFAULT_CONFIG      # with a builtin default (minimal) key binding
#DEFINES  += -D FIM_DEFAULT_CONFIGURATION # with a builtin default alias, autocommand, and binding set (complete)

# The following defines should migrate outta here!
#DEFINES  += -D FIM_NOFIMRC             # with ~/.fimrc loading disabled
#DEFINES  += -D FIM_NOSCRIPTING         # any script loading disabled
 DEFINES  += -D FIM_NOFB                # disable the framebuffer (for debug mostly)
#DEFINES  += -D FIM_AUTOCMDS            # autocommands : they are enabled externally now
 DEFINES  += -D FIM_RECORDING           # command recording
 DEFINES  += -D FIM_SWITCH_FIXUP        # still bugful
 DEFINES  += -D FIM_CHECK_DUPLICATES    # if enabled, no duplicates will be allowed in the filename list
 DEFINES  += -D FIM_CHECK_FILE_EXISTENCE # when a filename is added in the list, a verification occurs
 DEFINES  += -D FIM_REMOVE_FAILED       # a file which failed loading is removed from the list
 DEFINES  += -D FIM_AUTOSKIP_FAILED     # if (FIM_REMOVE_FAILED) after the file is removed, the next is tried
 DEFINES  += -D FIM_COMMAND_AUTOCOMPLETION  # An evil feature, right now
#DEFINES  += -D FIM_WINDOWS		# Fim own's window system. EXPERIMENTAL!
#DEFINES  += -D FIM_BOZ_PATCH		 # added from configure..
#DEFINES  += -D FIM_NO_SYSTEM           # An evil feature for the unware
DEFINES  += -D FIM_READ_STDIN           # Stdin as a source of input filenames
#DEFINES  += -D FIM_PROFILING           # Turns off compiler optimizations
#DEFINES  += -D FIM_IS_SLOWER_THAN_FBI     # If off, turns speedup patches to original Fbi code (experimental)
 FIM_VERSION= $(VERSION)
 FIM_AUTHOR = '"dezperado _CUT_ autistici _CUT_ org"'

DEFINES += -D X_DISPLAY_MISSING @DEFS@ @EXTRADEFS@

AM_CPPFLAGS=${DEFINES} ${FIM_CFLAGS}
AM_LDFLAGS=${FIM_CFLAGS}

fim_DEPENDENCIES = @FIM_LIB_OBJECTS@ @LIBOBJS@ $(INTLDEPS)

#FIMLIBS+=-ljpeg
#FIMLIBS+=-lreadline
FIMLIBS+= -lm  -lfl
#FIMLIBS+=-lpng
#FIMLIBS+=-lz
#FIMLIBS+=-ltiff
#FIMLIBS+=-lgpm
FIMLIBS+=-L/lib
#FIMLIBS+=-lungif
#FIMLIBS+=-lgif


#O=-O3 -funroll-loops

 
fim_LDADD = @FIM_LIB_OBJECTS@ @LIBOBJS@ $(LIBIMAP) $(FIMLIBS) \
        $(INTLLIBS) $(LIBICONV)  $(LIBGPGME_LIBS)

#AM_CPPFLAGS=''

