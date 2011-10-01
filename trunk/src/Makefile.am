# $Id$

subdir = src
AUTOMAKE_OPTIONS = nostdinc no-dependencies dejagnu
SUBDIRS = 
check_PROGRAMS = fim
bin_PROGRAMS = fim
bin_SCRIPTS = fimgs
doc_DATA = fimrc

LFLAGS = -+ --nounistd
YFLAGS = -v -d

#lex.yy.cc : lex.lex
lex.yy.cc: lex.lex
	$(LEX) $(LFLAGS) lex.lex
	@$(ECHO) "Applying a horrible hack to avoid the flex non-throwing isatty() redeclaration in 'extern "C" int isatty (int );'"
	$(SED) -i 's/^.*extern.*isatty.*int.*$$//g' $@

yacc.tab.cpp: yacc.ypp lex.lex
	$(YACC) $(YFLAGS) yacc.ypp -o yacc.tab.cpp

FIM_LIB_OBJECTS = yacc.tab.o lex.yy.o

conf.h:       fimrc
	$(ECHO) 'const fim_char_t * FIM_DEFAULT_CONFIG_FILE_CONTENTS =' > $@
	$(SED)  's/\\/\\\\/g;s/"/\\\"/g;s/^/"/g;s/$$/\\n"/g;' $< >> $@
	$(ECHO) '"";' >> $@

grammar.h: yacc.tab.cpp yacc.output
	$(ECHO) 'const fim_char_t * FIM_DEFAULT_GRAMMAR_FILE_CONTENTS =' > $@
	$(CAT) yacc.output | $(AWK) -f ../scripts/maintenance/yacc2grammar.awk  | $(CPP) -include ../scripts/maintenance/yacc2grammar.h  | $(GREP) -v '^#' | $(SED) 's/"/\\\"/g;s/^/"/g;s/$$/\\n"/g;' >> $@
	$(ECHO) '"";' >> $@

examples.h: ../scripts/example/oneline.fim
	$(ECHO) 'const fim_char_t * FIM_DEFAULT_EXAMPLE_FILE_CONTENTS =' > $@
	$(CAT) ../scripts/example/oneline.fim | $(SED) 's/"/\\\"/g;s/^/"/g;s/$$/\\n"/g;' >> $@
	$(ECHO) '"";' >> $@

CommandConsole-help.o: CommandConsole-help.cpp help-acm.cpp help.cpp examples.h grammar.h

# in the following : for some freebsd oddity, i was forced to replace ..0-9_]\+ with ..0-9_]*
version.h:	../config.h
	$(GREP) define $< | $(GREP) FIM | $(SORT) | $(SED) 's/^#//g;s/define //g;s/^\([A-Za-z0-9_]*\).*$$/+\1  /g;s/^/\"/g;s/$$/\"/g' > $@
	$(ECHO) "\"\n\"" >> $@
	$(GREP) define $< | $(GREP) FIM.*'"' | $(SORT) | $(SED) 's/^#//g;s/define //g;s/^\([A-Za-z0-9_]*\) "\(.*\)"$$/"\1 = \\"\2\\"\\n"/g'   >> $@
	$(GREP) undef $< | $(GREP) FIM | $(SORT) | $(SED) 's/^#//g;s/.*undef //g;s/^\([A-Za-z0-9_]*\).\+$$/-\1  /g;s/^/\"/g;s/$$/\"/g' >> $@
	$(ECHO)  '"\n'CXXFLAGS=$(CXXFLAGS)'\n"' >> $@

help.cpp: fim.h
	$(ECHO) -e "#ifdef FIM_WANT_INLINE_HELP\n" > $@
	$(GREP)   '^#define FIM_VID' $< | $(SED) 's/^#define //g;s/\(^[A-Z0-9_]\+\)\s\+\("[^ ]*"\)\s*\/\*\(.*\)\*\/$$/fim_var_help_db[\1]=\3;/g' >> $@
	$(ECHO) '#endif' >> $@

help-acm.cpp: fim.h
	$(ECHO) -e "//#ifdef FIM_WANT_INLINE_HELP\n" > $@
	$(ECHO) -e '#define FIM_AUTOCOMMANDS_LIST \' >> $@
	$(GREP)   '^#define FIM_ACM' $< | $(SED) 's/^#define //g;s/\(^[A-Z0-9_]\+\)\s\+\("[^ ]*"\)\s*\/\*\(.*\)\*\/$$/\1", "/g'|tr '\n' ' ' >> $@
	$(ECHO)  ' '>> $@
	$(ECHO) '//#endif' >> $@
	
# the following sources are built at make time
# old bison versions could have problems with this (e.g.: generating yacc.tab.cpp.h instead of yacc.tab.hpp)
BUILT_SOURCES = lex.yy.cc yacc.tab.cpp conf.h yacc.tab.hpp 

# this hook triggers on 'make dist'
dist-hook:
	-for file in $(BUILT_SOURCES) ; do $(RM) -f $(distdir)/$$file ; done

EXTRA_fim_SOURCES = 
# The following files are marked as source files by configure.ac
EXTRA_fim_SOURCES += FbiStuffGif.cpp
EXTRA_fim_SOURCES += FbiStuffJpeg.cpp
EXTRA_fim_SOURCES += FbiStuffTiff.cpp
EXTRA_fim_SOURCES += FbiStuffPng.cpp
EXTRA_fim_SOURCES += FbiStuffDjvu.cpp
EXTRA_fim_SOURCES += FbiStuffMagick.cpp
EXTRA_fim_SOURCES += FbiStuffPdf.cpp
EXTRA_fim_SOURCES += FbiStuffPs.cpp

CLEANFILES=$(BUILT_SOURCES) yacc.output 

#CPP_FLAGS="$CPP_FLAGS -D X_DISPLAY_MISSING"
AM_CPPFLAGS=
fim_DEPENDENCIES = @FIM_LIB_OBJECTS@ @LIBOBJS@ $(INTLDEPS)
fim_LDADD = @FIM_LIB_OBJECTS@ @LIBOBJS@ $(FIMLIBS) $(LEXLIB)
fim_SOURCES  = $(BUILT_SOURCES) \
	fim.cpp \
	fim.h \
	fim_types.h \
	fim_limits.h \
	fim_plugin.h \
	fim_plugin.cpp \
	fim_wrappers.h \
	AADevice.h \
	Benchmarkable.h \
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
	FimWindow.h \
	common.h \
	conf.h \
	help.cpp \
	help-acm.cpp \
	version.h \
	grammar.h \
	examples.h \
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
	CommandConsole-cmd.cpp \
	CommandConsole-init.cpp \
	CommandConsole-help.cpp \
	DebugConsole.cpp \
	DisplayDevice.cpp \
	FbiStuff.cpp \
	FbiStuffFbtools.cpp \
	FbiStuffLoader.cpp \
	FbiStuffPpm.cpp \
	FbiStuffBmp.cpp \
	FbiStuffBit1.cpp \
	FbiStuffBit24.cpp \
	FbiStuffMatrixMarket.cpp \
	FontServer.cpp \
	FramebufferDevice.cpp \
	Image.cpp \
	Namespace.cpp \
	Var.cpp \
	Viewport.cpp \
	FimWindow.cpp \
	common.cpp \
	interpreter.cpp \
	string.cpp

Command.o: Command.h
CommandConsole.o: defaultConfiguration.o
CommandConsole-init.o: conf.h defaultConfiguration.o
help.o: conf.h defaultConfiguration.o
fim.o: version.h help.cpp grammar.h examples.h
Var.o: help.cpp
fim.cpp: version.h help.cpp grammar.h examples.h
lex.yy.o: lex.yy.cc

# regex.c is work in progress and awaits for integration
EXTRA_DIST = lex.lex yacc.ypp fimrc defaultConfiguration.cpp fimgs \
	testdir/*ok testdir/*in testdir/Makefile \
	testsuite/*/* \
	regex.c

RUNTESTDEFAULTFLAGS = --tool ./fim FIM=$$srcdir/./fim  --srcdir $$srcdir/testsuite
#RUNTESTDEFAULTFLAGS = --all --tool ./fim FIM=$$srcdir/./fim --tool_opts=$$srcdir/../media/fim.png --srcdir $$srcdir/testsuite


