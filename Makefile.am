# $Id$

# no-dependencies was due to problems with conditional sources
# nostdinc disables the standard -I. include which breaks a correct string.h include
AUTOMAKE_OPTIONS = nostdinc no-dependencies # dejagnu
SUBDIRS = src doc distros scripts

dist_doc_DATA=$(top_srcdir)/README $(top_srcdir)/README.FIRST $(top_srcdir)/TODO $(top_srcdir)/VERSION $(top_srcdir)/BUGS  $(top_srcdir)/FAQ.TXT $(top_srcdir)/AUTHORS $(top_srcdir)/ChangeLog $(top_srcdir)/NEWS
# no INSTALL INSTALL.TXT is necessary in rules
EXTRA_DIST=  COPYING  \
	Makefile.am configure.ac configure \
	 aclocal.m4 config.h.in \
	autogen.sh \
	$(top_srcdir)/media/image.jpg \
	$(top_srcdir)/media/image.png \
	$(top_srcdir)/media/fim.png \
	$(top_srcdir)/media/numbers/0.gif \
	$(top_srcdir)/media/numbers/1.gif \
	$(top_srcdir)/media/numbers/2.gif \
	$(top_srcdir)/media/numbers/3.gif \
	$(top_srcdir)/media/numbers/4.gif \
	$(top_srcdir)/media/numbers/5.gif \
	$(top_srcdir)/media/numbers/6.gif \
	$(top_srcdir)/media/numbers/7.gif \
	$(top_srcdir)/media/numbers/8.gif \
	$(top_srcdir)/media/numbers/9.gif \
	$(top_srcdir)/media/numbers_pcx/0.pcx \
	$(top_srcdir)/media/numbers_pcx/1.pcx \
	$(top_srcdir)/media/numbers_pcx/2.pcx \
	$(top_srcdir)/media/numbers_pcx/3.pcx \
	$(top_srcdir)/media/numbers_pcx/4.pcx \
	$(top_srcdir)/media/numbers_pcx/5.pcx \
	$(top_srcdir)/media/numbers_pcx/6.pcx \
	$(top_srcdir)/media/numbers_pcx/7.pcx \
	$(top_srcdir)/media/numbers_pcx/8.pcx \
	$(top_srcdir)/media/numbers_pcx/9.pcx \
	$(top_srcdir)/media/multipage/sample.tex \
	$(top_srcdir)/media/multipage/Makefile \
	$(top_srcdir)/var/fonts/Lat15-Terminus16.psf \
	depcomp missing install-sh
FIM_EXE=./src/fim
FIMGS_EXE=./src/fimgs


# mhmhmhm 
# CLEANFILES = autom4te.cache

srcdir = src

# still unfinished
.PHONY: signed-tgzdist
signed-tgzdist: dist
	gpg -sbv -u 0xE0E669C8EF1258B8 $(distdir).tar.gz
	ls -l $(distdir).tar.gz  $(distdir).tar.gz.sig

# still unfinished
.PHONY: signed-dist
signed-dist: dist
	make dist-bzip2 && gpg -sbv -u 0xE0E669C8EF1258B8 $(distdir).tar.bz2
	ls -l $(distdir).tar.bz2 $(distdir).tar.bz2.sig
	gpg -sbv -u 0xE0E669C8EF1258B8 $(distdir).tar.gz
	ls -l $(distdir).tar.gz  $(distdir).tar.gz.sig

#%.sig: %
#	        gpg -sbv -u 0xE0E669C8EF1258B8 $@ $<
#	        gpg -sbav -u 0xE0E669C8EF1258B8 $@ $<


FIMNORCOPTS=-N --no-etc-rc-file

# this rule is deprecated, too: testing should be performed with 'make tests' (FIXME)
.PHONY: sanity-check
sanity-check:	all
	@$(FIM_EXE) $(FIMNORCOPTS) -E $(top_srcdir)/scripts/tests/sanity.fim -t

.PHONY: check
check:	tests

# this is the official way of testing fim
.PHONY: tests
tests:	all
	ulimit -c unlimited || true
	ulimit -v 32000 || true
	ulimit -t 1000 || true
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep gif
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep png
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep jpeg
	@top_srcdir=$(top_srcdir) $(SHELL) $(top_srcdir)/scripts/tests/version.sh
	@top_srcdir=$(top_srcdir) $(SHELL) $(top_srcdir)/scripts/tests/font.sh
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media -c '_caption_over_image=3;display;quit;'
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'quit;'
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -E $(top_srcdir)/scripts/example/oneline.fim -c quit 
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/image.png -c 'next'   -c quit # leak fixed
	@if $(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep tiff && which convert ; then \
		convert $(top_srcdir)/media/image.png $(top_srcdir)/image.tiff && \
		$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/image.tiff -c 'next' -c quit; fi # leak fixed
	if which wrjpgcom; then wrjpgcom -replace -comment 'add JPEG comment' $(top_srcdir)/media/image.jpg > image_commented.jpg && $(FIM_EXE) $(FIMNORCOPTS) -o dumb -X image_commented.jpg -c quit && rm -f image_commented.jpg; fi # leak fixed
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/image.png -c '/*.jpg' -c quit # leak fixed
	if which timeout; then if ! timeout 2 $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/image.png -o dumb -c '/media' -c 'quit' ; then echo 'Seems like -c "/<filename>" is broken!'; false; fi; fi
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'quit 14' ; if test $$? == 14 ; then echo '[*] Error code return test PASSED' ; else echo 'Error code return test FAILED'; false ; fi
	make ltests

.PHONY: ltests
ltests:
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep jpeg
	if test $(abs_builddir) = $(abs_srcdir) ; then make -C src/testdir ; fi

FIMSCRIPTSDIR=`pwd`/scripts/maintenance/

# new target, still not working
.PHONY: tests-all
tests-all: tests
	cd src && which cppcheck && cppcheck *cpp *.h || true
	$(SHELL) $(FIMSCRIPTSDIR)/configure-brute-check.sh
	
#news-dump:
#	$(AWK)  -- 'v=0;($$1 == "Version" && $$2=="'`cat VERSION`'"){while((input=getline) && $$1!="Version"){print}};'  NEWS > f

# this rule is deprecated, too
.PHONY: test
test:	all
	@$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/* #~/M*s/*g

# this rule is deprecated, too
.PHONY: aatest
aatest:	all
	@$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/* -o aa #~/M*s/*g

# this rule is old and deprecated
#tgz:	clean
#	tar -czf ../fim.`date +%Y%m%d%H%M`.tgz ../fim/*
#	ls -l ../fim*.tgz  -v

# this rule is encouraged
.PHONY: report
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
.PHONY: exec
exec:   test

# this rule is smart, isn't it ? I love it !
.PHONY: edit
edit:
	$(EDITOR) $(srcdir)/fim.cpp +':split $(srcdir)/fim.h' # Vim ! :)

.PHONY:
	@true

# this rule is informative and for acting like a peacock 
.PHONY: wc
wc:
	wc $(srcdir)/*.cpp $(srcdir)/*.h $(srcdir)/yacc.ypp $(srcdir)/lex.lex
	@#wc $(FIM) # missing headers..

.PHONY: devhelp
devhelp:
	@echo -----------------------------------------------
	@echo '			short Makefile dev help'
	@echo -----------------------------------------------
	@echo
	@echo 'make uptrunk' will upload trunk tarballs to 
	@echo   http://download.savannah.nongnu.org/releases/fbi-improved/
	@echo
	@make help

.PHONY: help
help:
	@echo -----------------------------------------------
	@echo '			short Makefile help'
	@echo -----------------------------------------------
	@echo
	@echo 'Please read the documentation file before complaining!'
	@echo
	@echo 'The first documentation file you should read is README'
	@echo
	@echo 'If you are experiencing problems, please contact the code author.'
	@echo
	@echo 'His mail box resides as dezperado, then a dot, then autistici dot org.'
	@echo
	@echo 'You are encouraged attaching the output of `make report` in your bug reports.'
	@echo 'Thanks for your collaboration.'
	@echo

.PHONY: ai
ai: $(distdir).tar.gz.sig  $(distdir).tar.gz $(distdir).tar.bz2.sig $(distdir).tar.bz2
	make site up -C var/

.PHONY: up
up: upload news

.PHONY: news
news: freshmeat mail-announce

.PHONY: dox
dox: doc/fim.man doc/fimrc.man doc/fimgs.man

MANSUBST = sed -e 's,@sysconfdir\@,$(sysconfdir),g' -e 's,@docdir\@,$(docdir),g' -e 's,-,\\-,g'

doc/fim.man.in doc/fimrc.man.in: $(FIM_EXE)
	$(FIM_EXE) $(FIMNORCOPTS) --help=m                    > doc/fim.man.in
	$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help=man   > doc/fimrc.man.in

doc/fimgs.man.in: $(FIM_EXE)
if HAVE_GS
	$(FIMGS_EXE) -m -- $(FIMNORCOPTS)                     > doc/fimgs.man.in
endif

doc/fim.man doc/fimrc.man doc/fimgs.man: doc/fim.man.in doc/fimrc.man.in doc/fimgs.man.in
	cat doc/fim.man.in   |$(MANSUBST) > doc/fim.man
	cat doc/fimrc.man.in |$(MANSUBST) > doc/fimrc.man
	cat doc/fimgs.man.in |$(MANSUBST) > doc/fimgs.man

# fixme : missing integration with ChangeLog file
.PHONY: mail-announce
mail-announce:
	@ figlet fim | mutt -s "fim-`cat VERSION` release" "fbi-improved-devel@nongnu.org"  -i -

.PHONY: log
log: changelog

# from trunk, we should call svn2cl .. to get the full fim changelog history 
.PHONY: changelog
changelog:
	svn2cl -i ..

.PHONY: release
release: upload freshmeat

.PHONY: upload
upload: savannah 

.PHONY: uptrunk
uptrunk:
	read && scp $(distdir).tar.gz.sig  $(distdir).tar.gz $(distdir).tar.bz2.sig $(distdir).tar.bz2 dezperado@dl.sv.nongnu.org:/releases/fbi-improved/

# echo -en "cd /releases/fbi-improved\nput $(distdir).tar.gz.sig\n put fim-${fim_cv_version}.tar.gz\n put fim-${fim_cv_version}.tar.bz2\n put fim-${fim_cv_version}.tar.bz2.sig"  | sftp "dezperado@dl.sv.gnu.org" -b 
.PHONY: savannah_
savannah_: signed-dist
	grep trunk VERSION || echo do
	lynx -dump dl.sv.gnu.org/releases/fbi-improved

.PHONY: savannah-lookup
savannah-lookup:
	lynx -dump dl.sv.gnu.org/releases/fbi-improved

# fixme : missing integration with ChangeLog file and such information
.PHONY: freshmeat_old
freshmeat_old: 
	@ $(AWK)  -- 'v=0;($$1 == "Version" && $$2=="'`cat VERSION`'"){while((input=getline) && $$1!="Version"){print}};'  NEWS 
	@echo freshmeat-submit -v "fbi-`cat VERSION`" \
	--project fbi-improved \
	--license GPL \
	--mailing-list-url      $$(cat README | grep "^Mailing List :"|sed s/^.*\ :.//g) \
	--home-page-url         $$(cat README | grep "^Web Page  "|sed s/^.*\ :.//g) \
	--cvs-url               $$(cat README | grep "^Repository"|sed s/^.*\ :.//g) \
	--gzipped-tar-url       $$(cat README | grep "^Releases  "|sed s/^.*\ :.//g)/"fim-`cat VERSION`".tar.gz \
	--mirror-site-url       $$(cat README | grep "^Off.*rror : "|sed s/^.*\ :.//g) \
	--changelog-url         $$(cat README | grep "^ChangeLog  "|sed s/^.*\ :.//g)
	@ echo "are you sure ? (any key to abort, yes to continue)"  && read yes && [[ "$${yes}" == "yes" ]] && \
	$(AWK)  -- 'v=0;($$1 == "Version" && $$2=="'`cat VERSION`'"){while((input=getline) && $$1!="Version"){print}};'  NEWS | \
	 freshmeat-submit -v "fbi-`cat VERSION`" \
	--project fbi-improved \
	--license GPL \
	--mailing-list-url      $$(cat README | grep "^Mailing List :"|sed s/^.*\ :.//g) \
	--home-page-url         $$(cat README | grep "^Web Page  "|sed s/^.*\ :.//g) \
	--cvs-url               $$(cat README | grep "^Repository"|sed s/^.*\ :.//g) \
	--gzipped-tar-url       $$(cat README | grep "^Releases  "|sed s/^.*\ :.//g)/"fim-`cat VERSION`".tar.gz \
	--mirror-site-url       $$(cat README | grep "^Off.*rror : "|sed s/^.*\ :.//g) \
	--changelog-url         $$(cat README | grep "^ChangeLog  "|sed s/^.*\ :.//g)
#	--url-demo         $$(cat README | grep "^Web Page  "|sed s/^.*:.//g)
#	--bzipped-tar-url  $$(cat README | grep "^Web Page  "|sed s/^.*:.//g)

.PHONY: freshmeat-submit
freshmeat-submit:
	@echo Project: fim
	@echo Version: `cat VERSION`
	@echo Release-Focus: Major feature enhancements
	@echo Hide: N
	@echo Home-Page-URL: $$(cat README | grep "^Web Page  "|sed s/^.*\ :.//g)
	@echo Mailing-List-URL: $$(cat README | grep "^Mailing List :"|sed s/^.*\ :.//g)
	@echo Gzipped-Tar-URL: $$(cat README | grep "^Releases  "|sed s/^.*\ :.//g)/"fim-`cat VERSION`".tar.gz
	@echo Bzipped-Tar-URL: $$(cat README | grep "^Releases  "|sed s/^.*\ :.//g)/"fim-`cat VERSION`".tar.bz2
	@echo License: GPL
	@echo Mirror-Site-URL: $$(cat README | grep "^Off.*rror : "|sed s/^.*\ :.//g)
	@echo ChangeLog-URL: $$(cat README | grep "^ChangeLog  "|sed s/^.*\ :.//g)
	@echo CVS-URL: $$(cat README | grep "^Repository"|sed s/^.*\ :.//g)
	@echo 
	@ $(AWK)  -- 'v=0;($$1 == "Version" && $$2=="'`cat VERSION`'"){while((input=getline) && $$1!="Version"){print}};'  NEWS 

.PHONY: freshmeat
freshmeat:
	@ echo "are you sure you want to freshmeat-submit ? (any key to abort, yes to continue)"  && read yes && [[ "$${yes}" == "yes" ]] && \
	make freshmeat-submit | grep -v '^make\[' | freshmeat-submit

.PHONY: commit
commit: log
	svn commit

# This awaits completion.
#rpm: dist
#        rpmbuild -ta @PACKAGE@-@VERSION@.tar.gz

