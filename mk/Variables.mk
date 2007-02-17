# common variables ...
########################################################################

# directories
DESTDIR	=
srcdir	?= .
prefix	?= /usr/local
bindir	=  $(DESTDIR)$(prefix)/bin
#mandir	=  $(DESTDIR)$(prefix)/share/man
locdir  =  $(DESTDIR)$(prefix)/share/locale

# package + version
#empty	:=
#space	:= $(empty) $(empty)
#PWD	:= $(shell cd $(srcdir); pwd)
#DIR	:= $(patsubst $(dir $(PWD))%,%,$(PWD))
#PACKAGE	:= $(word 1,$(subst -,$(space),$(DIR)))
#VERSION	:= $(word 2,$(subst -,$(space),$(DIR)))
#TARBALL	:= $(PACKAGE)_$(VERSION).tar.gz

# programs
CC		?= gcc
CXX		?= g++
MOC             ?= $(if $(QTDIR),$(QTDIR)/bin/moc,moc)
INSTALL		?= install
INSTALL_BINARY  := $(INSTALL) -s
INSTALL_SCRIPT  := $(INSTALL)
INSTALL_DATA	:= $(INSTALL) -m 644
INSTALL_DIR	:= $(INSTALL) -d

# cflags
#CFLAGS	?= -g -O2
#CFLAGS	+= -Wall -Wmissing-prototypes -Wstrict-prototypes \
#	   -Wpointer-arith -Wunused

# add /usr/local to the search path if something is in there ...
#ifneq ($(wildcard /usr/local/include/*.h),)
#  CFLAGS  += -I/usr/local/include
#  LDFLAGS += -L/usr/local/$(LIB)
#endif

## fixup include path for $(srcdir) != "."
#ifneq ($(srcdir),.)
#  CFLAGS  += -I. -I$(srcdir)
#endif

