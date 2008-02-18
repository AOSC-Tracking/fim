# This file should be processed with automake..
#
# $Id$
#
# inspired by mutt's  Makefile.am 
#
# yet not optimal

# no-dependencies was due to problems with conditional sources
# nostdinc disables the standard -I. include which breaks a correct string.h include
AUTOMAKE_OPTIONS = nostdinc no-dependencies
SUBDIRS = src doc distros

EXTRA_DIST=VERSION BUGS  COPYING  FAQ.TXT  INSTALL  README README.FIRST TODO \
	Makefile.am configure.ac configure \
	AUTHORS ChangeLog NEWS aclocal.m4 config.h.in \
	autogen.sh \
	scripts \
	depcomp missing install-sh

# mhmhmhm 
# CLEANFILES = autom4te.cache

srcdir = src

# still unfinished
signed-dist: $(distdir).tar.gz.sig

%.sig: %
	        gpg -sbv -u EF1258B8 $@ $<
#	        gpg -sbav -u EF1258B8 $@ $<


# this rule is deprecated, too
sanity-check:	all
	@./src/fim -E scripts/tests/sanity.fim

# this rule is deprecated, too
test:	all
	@./src/fim media/* #~/M*s/*g

# this rule is old and deprecated
tgz:	clean
	tar -czf ../fim.`date +%Y%m%d%H%M`.tgz ../fim/*
	ls -l ../fim*.tgz  -v

# this rule is encouraged
report:
	@cat VERSION
	@echo '-'
	@$(LEX) -V   2>&1
	@echo '-'
	@$(YACC) -V  2>&1
	@echo '-'
	@$(CC) -v    2>&1
	@echo '-'
	@echo 'now please report the bug with this information to the author via email' ;

# this rule is redundant
exec:   test

# this rule is smart, isn't it ? I love it !
edit:
	$(EDITOR) $(srcdir)/fim.cpp +':split $(srcdir)/fim.h' # Vim ! :)

.PHONY:
	@true

# this rule is informative and for acting like a peacock 
wc:
	wc $(srcdir)/*.cpp $(srcdir)/*.h $(srcdir)/yacc.ypp $(srcdir)/lex.lex
	@#wc $(FIM) # missing headers..

help:
	@echo -----------------------------------------------
	@echo '			short Makefile help'
	@echo -----------------------------------------------
	@echo
	@echo 'Please read the doc/FIM.TXT file before complaining!'
	@echo
	@echo '..or README, or INSTALL, or some other file around here. There is plenty!'
	@echo '...'
	@echo
	@echo 'If you are experiencing problems, please write at dezperado, the author of fim.'
	@echo
	@echo 'His mail box resides as dezperado , on the server autistici dot org.'
	@echo
	@echo 'You are encouraged to read the documentation before, and emailing the output of `make report`!'
	@echo 'Thanks for your collaboration'
	@echo
	@echo

up: upload news

news: freshmeat mail-announce

# fixme : missing integration with ChangeLog file
mail-announce:
	@ figlet fim | mutt -s "fim-`cat VERSION` release" "fbi-improved-devel@nongnu.org"  -i -

release: upload freshmeat

upload: savannah 

savannah: signed-dist dist
	echo -en "cd /releases/fbi-improved\nput $(distdir).tar.gz.sig\n put fim-${fim_cv_version}.tar.gz"  | sftp "dezperado@dl.sv.gnu.org" -b 
	lynx -dump dl.sv.gnu.org/releases/fbi-improved

savannah-lookup:
	lynx -dump dl.sv.gnu.org/releases/fbi-improved

# fixme : missing integration with ChangeLog file and such information
freshmeat: savannah
	@ echo freshmeat-announce -v "fbi-`cat VERSION`" \
	--project fbi-improved \
	--license GPL \
	--mailing-list-url      $$(cat README | grep "^Mailing List :"|sed s/^.*\ :.//g) \
	--home-page-url         $$(cat README | grep "^Web Page  "|sed s/^.*\ :.//g) \
	--cvs-url               $$(cat README | grep "^Repository"|sed s/^.*\ :.//g) \
	--gzipped-tar-url       $$(cat README | grep "^Releases  "|sed s/^.*\ :.//g)/"fbi-`cat VERSION`".tar.gz \
	--mirror-site-url       $$(cat README | grep "^Off.rror : "|sed s/^.*\ :.//g)
#	--url-demo         $$(cat README | grep "^Web Page  "|sed s/^.*:.//g)
#	--bzipped-tar-url  $$(cat README | grep "^Web Page  "|sed s/^.*:.//g)
#	--changelog-url    $$(cat README | grep "^Web Page  "|sed s/^.*\ :.//g)/ChangeLog

