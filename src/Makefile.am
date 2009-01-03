# This file should be processed with automake to generate Makefile.in
#
# $Id$
#
# inspired by mutt's Makefile.am 

subdir = src

AUTOMAKE_OPTIONS = nostdinc no-dependencies
SUBDIRS = 

bin_PROGRAMS = fim

bin_SCRIPTS = fimgs

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
	echo 'const char * FIM_DEFAULT_CONFIG_FILE_CONTENTS =' > conf.h
	sed  's/"/\\\"/g;s/^/"/g;s/$$/\\n"/g;' fimrc >> conf.h
	echo '"";' >> conf.h

fim.cpp  :	version.h
version.h:	../config.h
	grep define ../config.h | grep FIM | sort | sed 's/^#//g;s/define //g;s/^\([A-Za-z0-9_]\+\).*$$/+\1  /g;s/^/\"/g;s/$$/\"/g' > version.h
	echo "\"\n\"" >> version.h
	grep define ../config.h | grep FIM.*'"' | sort | sed 's/^#//g;s/define //g;s/^\([A-Za-z0-9_]\+\) "\(.*\)"$$/"\1 = \\"\2\\"\\n"/g'   >> version.h
	grep undef ../config.h | grep FIM | sort | sed 's/^#//g;s/.*undef //g;s/^\([A-Za-z0-9_]\+\).\+$$/-\1  /g;s/^/\"/g;s/$$/\"/g' >> version.h
	echo  '"\n'CXXFLAGS=$(CXXFLAGS)'\n"' >> version.h


# the following sources are built at make time
BUILT_SOURCES = lex.yy.cc \
	yacc.tab.cpp conf.h yacc.tab.hpp 

# this hook triggers on 'make dist'
dist-hook:
	-for file in $(BUILT_SOURCES) ; do rm -f $(distdir)/$$file ; done

EXTRA_fim_SOURCES = 
# The following files are indicated as source files by configure.ac
# ( they are optional and are built via @FIM_LIB_OBJECTS@ )
#
EXTRA_fim_SOURCES += FbiStuffGif.cpp
EXTRA_fim_SOURCES += FbiStuffJpeg.cpp
EXTRA_fim_SOURCES += FbiStuffDjvu.cpp
EXTRA_fim_SOURCES += FbiStuffPdf.cpp
EXTRA_fim_SOURCES += FbiStuffTiff.cpp
EXTRA_fim_SOURCES += FbiStuffPng.cpp

CLEANFILES=$(BUILT_SOURCES) yacc.output 


fim_SOURCES  = $(BUILT_SOURCES) \
	fim.cpp \
	fim.h \
	AADevice.h \
	CACADevice.h \
	SDLDevice.h \
	Arg.h \
	Browser.h \
	Cache.h \
	Command.h \
	CommandConsole.h \
	DebugConsole.h \
	DisplayDevice.h \
	DummyDisplayDevice.h \
	Image.h \
	FbiStuff.h \
	FbiStuffList.h \
	FbiStuffFbtools.h \
	FbiStuffLoader.h \
	FontServer.h \
	FramebufferDevice.h \
	Namespace.h \
	Var.h \
	Viewport.h \
	Window.h \
	common.h \
	conf.h \
	version.h \
	fim.h \
	fim_stream.h \
	fim_stream.cpp \
	fimgs \
	lex.h \
	readline.h \
	readline.cpp \
	string.h \
	AADevice.cpp \
	CACADevice.cpp \
	SDLDevice.cpp \
	Arg.cpp \
	Browser.cpp \
	Cache.cpp \
	Command.cpp \
	CommandConsole.cpp \
	DebugConsole.cpp \
	DisplayDevice.cpp \
	FbiStuff.cpp \
	FbiStuffFbtools.cpp \
	FbiStuffLoader.cpp \
	FbiStuffPpm.cpp \
	FbiStuffBmp.cpp \
	FbiStuffBit1.cpp \
	FbiStuffBit24.cpp \
	FontServer.cpp \
	FramebufferDevice.cpp \
	Image.cpp \
	Namespace.cpp \
	Var.cpp \
	Viewport.cpp \
	Window.cpp \
	common.cpp \
	interpreter.cpp \
	string.cpp

CommandConsole.o: conf.h defaultConfiguration.o
fim.o: version.h

# regex.c is work in progress and awaits for integration
EXTRA_DIST = lex.lex yacc.ypp fimrc defaultConfiguration.cpp fimgs \
	regex.c

#fim_cpp_CPP_FLAGS=-DFIM_VERSION -DFIM_AUTHOR
#CPP_FLAGS="$CPP_FLAGS -D X_DISPLAY_MISSING"

 DEFINES  =
# The following defines already migrated outta here...

# The following defines should migrate outta here!
#DEFINES  += -D FIM_NOFIMRC             # with ~/.fimrc loading disabled
#DEFINES  += -D FIM_NOSCRIPTING         # any script loading disabled
#DEFINES  += -D FIM_AUTOCMDS            # autocommands : they are enabled externally now
#DEFINES  += -D FIM_ALIASES_WITHOUT_ARGUMENTS	# if defined, aliases are much more prosaic
#DEFINES  += -D FIM_WINDOWS		# Fim own's window system. EXPERIMENTAL!
#DEFINES  += -D FIM_BOZ_PATCH		 # added from configure..
#DEFINES  += -D FIM_NO_SYSTEM           # An evil feature for the unware
#DEFINES  += -D FIM_PROFILING           # Turns off compiler optimizations
#DEFINES  += -D FIM_IS_SLOWER_THAN_FBI     # If off, turns speedup patches to original Fbi code (experimental)
#DEFINES  += -D FIM_NO_UNSAFE_CMDS		# Some unsafe commands are temporarily hidden
#DEFINES  += -D FIM_FASTER_CLEARLINES	# We save mmapped memset() calls in this way (a very small gain, though)
#DEFINES  += -D FIM_DEFAULT_KEY_CONFIG  # with a builtin default key mapping (advised)
#DEFINES  += -D FIM_DEFAULT_CONFIGURATION # with a builtin default alias, autocommand, and binding set (complete)

# Moved to configure.ac (and thus to config.h) :
#DEFINES  += -D FIM_DEFAULT_CONFIG      # with a builtin default (minimal) key binding
#DEFINES  += -D FIM_NOFB                # disable the framebuffer (for debug mostly)
#DEFINES  += -D FIM_RECORDING           # command recording
#DEFINES  += -D FIM_SWITCH_FIXUP        # still bugful
#DEFINES  += -D FIM_CHECK_DUPLICATES    # if enabled, no duplicates will be allowed in the filename list
#DEFINES  += -D FIM_CHECK_FILE_EXISTENCE # when a filename is added in the list, a verification occurs
#DEFINES  += -D FIM_REMOVE_FAILED       # a file which failed loading is removed from the list
#DEFINES  += -D FIM_AUTOSKIP_FAILED     # if (FIM_REMOVE_FAILED) after the file is removed, the next is tried
#DEFINES  += -D FIM_COMMAND_AUTOCOMPLETION  # An evil feature, right now
#DEFINES  += -D FIM_NAMESPACES		# Each object among {Image,Browser,Viewport,Window} will also be a namespace
#DEFINES  += -D FIM_READ_STDIN           # Stdin as a source of input filenames
#DEFINES  += -D FIM_RANDOM           # 'random' is a fake variable
#DEFINES  += -D FIM_CACHE_DEBUG		# Debug info on cache available
#DEFINES  += -D FIM_BUGGED_CACHE	# Sometimes something breaks and we want to isolate mechanisms
#DEFINES  += -D X_DISPLAY_MISSING

# DEFINES  += @DEFS@ 

#FIM_VERSION= $(VERSION)
# FIM_AUTHOR = '"Michele Martone <dezperado _CUT_ autistici _CUT_ org>"'

#DEFINES  += -D FIM_VERSION="\"$(FIM_VERSION)\""
# DEFINES  += -D FIM_AUTHOR=$(FIM_AUTHOR)

#AM_CPPFLAGS=${DEFINES} # ${FIM_CFLAGS} @FIM_DEFS@
AM_CPPFLAGS=
#AM_CXXFLAGS=-I . -I ..
AM_CXXFLAGS=$(FIM_CXXFLAGS)
#AM_LDFLAGS=${FIM_CFLAGS}

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

