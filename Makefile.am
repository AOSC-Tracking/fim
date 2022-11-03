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
	$(top_srcdir)/media/special/icon_smile_gif_at_7.bin \
	$(top_srcdir)/media/special/images.desc \
	$(top_srcdir)/media/special/images.list \
	$(top_srcdir)/media/special/collated.list \
	$(top_srcdir)/media/special/loadable.desc \
	$(top_srcdir)/var/fonts/Lat15-Terminus16.psf \
	depcomp missing install-sh
FIM_EXE=./src/fim$(EXEEXT)
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


CXX11TEST=test ` $(FIM_EXE) -V | grep standard | head -1  | tr -d -c '[:digit:]'` -gt 200301
FIMNORCOPTS=-N --no-etc-rc-file --no-history
FIMVIDEOTESTOPTS=-K ']]].[[fmfmwsk--++hhlltW' -K ':quit' -K ''
NO_ASAN=ASAN_OPTIONS=detect_leaks=0

if ENABLE_CACA
FIM_USE_ASCII_ART_DEFAULT=caca
else
if ENABLE_AA
FIM_USE_ASCII_ART_DEFAULT=aa
endif
endif

.PHONY: fbtests
fbtests: all
if WITH_EXPECT
	$(TIMEOUT_SMALL) \
		$(EXPECT) $(top_srcdir)/scripts/tests/spawn_and_quit.exp $(FIM_EXE) $(FIMNORCOPTS) -o fbdev $(top_srcdir)/media/
endif
	$(TIMEOUT_SMALL) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o fbdev   $(FIMVIDEOTESTOPTS) 

.PHONY: aatests
aatests: all
if WITH_EXPECT
	$(NO_ASAN) $(TIMEOUT_SMALL) \
		$(EXPECT) $(top_srcdir)/scripts/tests/spawn_and_quit.exp $(FIM_EXE) $(FIMNORCOPTS) -o aa $(top_srcdir)/media/
endif
	$(NO_ASAN) $(TIMEOUT_SMALL) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o aa   $(FIMVIDEOTESTOPTS)
	if test -n "$(DISPLAY)"; then \
		$(NO_ASAN) $(TIMEOUT_LARGE) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o aa=w $(FIMVIDEOTESTOPTS); \
	fi
	test `SSH_TTY=1       $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/                     -c 'stderr _device_string' -kq   2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #
	test `(unset SSH_TTY; $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -t --no-framebuffer -c 'stderr _device_string' -kq ) 2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #

.PHONY: cacatests
cacatests: all
if WITH_EXPECT
	$(TIMEOUT_SMALL) \
		$(EXPECT) $(top_srcdir)/scripts/tests/spawn_and_quit.exp $(FIM_EXE) $(FIMNORCOPTS) -o caca $(top_srcdir)/media/
endif
	$(TIMEOUT_SMALL) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o caca  $(FIMVIDEOTESTOPTS)
	if test -n "$(DISPLAY)"; then \
		$(NO_ASAN) $(TIMEOUT_LARGE) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o caca=w $(FIMVIDEOTESTOPTS); \
	fi
	test `SSH_TTY=1       $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/                     -c 'stderr _device_string' -kq   2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #
	test `(unset SSH_TTY; $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -t --no-framebuffer -c 'stderr _device_string' -kq ) 2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #


.PHONY: sdltests
sdltests: all
	if test -n "$(DISPLAY)"; then \
		$(NO_ASAN) $(TIMEOUT_LARGE) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o sdl=h  $(FIMVIDEOTESTOPTS); \
	fi

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
	@ ! grep '\<'$(USER)'\>' doc/*man* # against namespace leaks
if ENABLE_PCX
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort"; f=i:_filename;list "random_shuffle";reload;if(f==i:_filename)quit 255; quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort"; next;f=i:_filename;list "swap";     reload;if(f==i:_filename)quit 255; quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx $(top_srcdir)/media/numbers_pcx/*.pcx --sort -c 'list "pop"; if(_filelistlen!= 9) quit 255;quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx $(top_srcdir)/media/numbers_pcx/*.pcx        -c 'list "pop"; if(_filelistlen!=19) quit 255;quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'a=1' -c 'a=a+1;' -c 'quit (1-(a==2))'
endif
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdir"  _pwd;stdout l;if(l==_filelistlen)quit 255; quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdirr" _pwd;stdout l;if(l==_filelistlen)quit 255; quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdir"      ;stdout l;if(l==_filelistlen)quit 255; quit'
	@$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdirr"     ;stdout l;if(l==_filelistlen)quit 255; quit'
if ENABLE_RAW_BITS_RENDERING
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --as-text   -c 'stdout _last_file_loader;quit'` == Text  # mostly coverage
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --binary=1  -c 'stdout _last_file_loader;quit'` == Bit1  # mostly coverage
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --binary=24 -c 'stdout _last_file_loader;quit'` == Bit24 # mostly coverage
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --as-text -c quit
endif
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --dump-reference-help | wc -l` -gt 100
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=m | wc -c` -gt 28000 # man dump
	test `$(FIM_EXE) $(FIMNORCOPTS) --NON-existent-OPTIONNN | wc -l` -gt 70 # help function
if ENABLE_BMP
	if which convert; then \
		convert $(top_srcdir)/media/icon_smile.gif -monochrome $(top_srcdir)/media/icon_smile.bmp && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.bmp -c 'stdout _last_file_loader;quit'` == bmp && rm -f $(top_srcdir)/icon_smile.bmp && \
		if which ppmtobmp ; then \
		convert $(top_srcdir)/media/icon_smile.gif ppm:- | ppmtobmp -bpp 8 > $(top_srcdir)/media/icon_smile.bmp && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.bmp -c 'stdout _last_file_loader;quit'` == bmp && rm -f $(top_srcdir)/icon_smile.bmp && \
		convert $(top_srcdir)/media/icon_smile.gif ppm:- | ppmtobmp -bpp 4 > $(top_srcdir)/media/icon_smile.bmp && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.bmp -c 'stdout _last_file_loader;quit'` == bmp && rm -f $(top_srcdir)/icon_smile.bmp ; \
		fi && \
		convert $(top_srcdir)/media/icon_smile.gif -depth 8 -alpha Disassociate $(top_srcdir)/media/icon_smile.bmp && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.bmp -c 'stdout _last_file_loader;quit'` == bmp && rm -f $(top_srcdir)/icon_smile.bmp; \
	fi
endif
	if which convert; then \
		convert $(top_srcdir)/media/icon_smile.gif -depth 8  $(top_srcdir)/media/icon_smile.ppm && \
		sed -i 's/P6$$/P6\n# comment/g'                      $(top_srcdir)/media/icon_smile.ppm && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.ppm -c 'stdout _last_file_loader;quit'` == ppm && rm -f $(top_srcdir)/icon_smile.ppm && \
		convert $(top_srcdir)/media/icon_smile.gif -depth 16 $(top_srcdir)/media/icon_smile.ppm && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.ppm -c 'stdout _last_file_loader;quit'` == ppm && rm -f $(top_srcdir)/icon_smile.ppm && \
		convert $(top_srcdir)/media/icon_smile.gif -depth 8  $(top_srcdir)/media/icon_smile.pgm && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.pgm -c 'stdout _last_file_loader;quit'` == pgm && rm -f $(top_srcdir)/icon_smile.pgm; \
		convert $(top_srcdir)/media/icon_smile.gif -depth 16 $(top_srcdir)/media/icon_smile.pgm && \
		test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.pgm -c 'stdout _last_file_loader;quit'` == pgm && rm -f $(top_srcdir)/icon_smile.pgm; \
	fi # cover-test FbiStuffPpm.cpp
if ENABLE_PCX
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c '2,4 stdout  "{}";q' | wc -l` = 3
	cd $(top_srcdir)/media/numbers_pcx/; $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb                            --read-from-file ../special/images.list   -c 'if(_filelistlen!=2)quit 255; quit'
if WANT_READ_STDIN_IMAGE
	cd $(top_srcdir)/media/numbers_pcx/; $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb --read-from-stdin-elds '!' --read-from-file ../special/images.list   -c 'if(_filelistlen!=0)quit 255; quit'
	cd $(top_srcdir)/media/numbers_pcx/; $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb --read-from-stdin-elds '!' --read-from-file ../special/collated.list -c 'if(_filelistlen!=2)quit 255; quit'
endif
if ENABLE_ARCHIVE
	tar czf archive.tgz $(top_srcdir)/media/numbers_pcx/ && $(TIMEOUT_SMALL) $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb archive.tgz -c 'stdout i:pages." pages";if(i:pages==1)quit 255;quit' && rm -f archive.tgz
	echo 'not an archive' > archive.tgz; $(TIMEOUT_SMALL) $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb archive.tgz -c 'stdout i:pages." pages";if(i:pages!=0)quit 255;quit' && rm -f archive.tgz
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -C '_file_loader="ppm"' $(top_srcdir)/media/numbers_pcx/1.pcx -c 'stdout i:width; stdout (i:width!=0 && i:height!=0);quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -C '_file_loader="pcx"' $(top_srcdir)/media/numbers_pcx/1.pcx -c 'stdout i:width; stdout (i:width==0 && i:height==0);quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --mark-from-image-descriptions-file $(top_srcdir)/media/special/images.desc -c 'l=_filelistlen;toggleLimitMarked;if(_filelistlen!=l/2)quit 255; quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-descriptions-file-separator $$'\t' $(top_srcdir)/media/special/loadable.desc $(top_srcdir)/media/numbers_pcx/ -c '/2.pcx;if(i:_comment=~"due")quit;quit 255'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-descriptions-file-separator '?'    $(top_srcdir)/media/special/loadable.desc $(top_srcdir)/media/numbers_pcx/ -c '/2.pcx;if(i:_comment=~"due")quit 255;quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --image-descriptions-file-separator $$'\t' --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/2.pcx;if(i:_comment=~"due")quit;quit 255'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --image-descriptions-file-separator '?'    --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/2.pcx;if(i:_comment=~"due")quit 255;quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/1.pcx;if(i:prime=="no")quit 255;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/4.pcx;if(i:prime=="yes")quit 255;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/2.pcx;if(i:_comment!="due")quit 255;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/4.pcx;quit (i:_comment)'; test $$? == 44 
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/0.pcx;quit (i:_comment)'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c 'list "vars"; if(! _last_cmd_output =~ "vals.*files") quit 255;;quit;'
	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-shadow $(top_srcdir)/media/../ -c 'scale "shadow"; if (_last_cmd_output =~ "image substituted")quit 0; else quit 255;'; fi
	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-shadow $(top_srcdir)/var/  -c 'scale "shadow"; if (_last_cmd_output =~ "image substituted")quit 0; else quit 255;'; test $$? = 255; fi
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*  -/  media/numbers_pcx/4.pcx -c 'stdout _fileindex;if(_fileindex!=0){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*  -/                    4.pcx -c 'stdout _fileindex;if(_fileindex!=5){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --/  media/numbers_pcx/4.pcx -c 'stdout _fileindex;if(_fileindex!=0){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --/                    4.pcx -c 'stdout _fileindex;if(_fileindex!=5){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --// media/numbers_pcx/4.___ -c 'stdout _fileindex;if(_fileindex!=0){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --// media/numbers_pcx/4.pcx -c 'stdout _fileindex;if(_fileindex!=5){quit 255}quit'
endif
if ENABLE_JPEG
if WANT_READ_STDIN_IMAGE
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'stdout i:width.i:height; quit' -i < $(top_srcdir)/media/image.jpg | tail -1` != "00" # mostly coverage
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.jpg -K++++ -c 'flip;mirror;pan_left;pan_right;pan_up;pan_down;pan_ne;pan_sw;pan_se;pan_nw "100%"' -c q # mostly coverage
endif
if ENABLE_AA
	@make aatests
endif
if ENABLE_CACA
	@make cacatests
endif
if ENABLE_FRAMEBUFFER
	@if test -z "$(DISPLAY)$(SSH_TTY)"; then make fbtests; fi
endif
if ENABLE_SDL
	@make sdltests
endif
if ENABLE_AA
	$(FIM_EXE) $(FIMNORCOPTS) -o aa $(top_srcdir)/media/ -K :$$'\t' -Kq | grep WELCOME 2>&1 | cat > /dev/null # cover what happens hitting Tab in console mode
endif
if HAVE_RUNNABLE_TESTS
if ENABLE_GIF
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 2:6 $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 2:7 $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 11"libungif"
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 2+4 $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 2+5 $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 11"libungif"
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 0 $(top_srcdir)/media/icon_smile.gif -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 11"libungif"
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 6 $(top_srcdir)/media/icon_smile.gif -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 6:1000 $(top_srcdir)/media/icon_smile.gif -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit' | grep -v Reading` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 7 $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 11"libungif"
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 6 $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -C '_seek_magic="gif"' $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit' | tr -d '\n' | tr ' ' _ ` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -C '_seek_magic="GIF"' $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 11"libungif"
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 8 -C '_seek_magic="GIF"' $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 000
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb --offset 6 -C '_seek_magic="GIF"' $(top_srcdir)/media/special/icon_smile_gif_at_7.bin -c 'stdout (i:width!=0).(i:height!=0)._last_file_loader;quit'` == 11"libungif"
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit' --slideshow 0 -1 # if this does not hang is good
	if which timeout; then \
		timeout 2 $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit' --slideshow 0 ; R=$$?; \
		if which reset ; then reset; fi; \
		test $$R = 124; \
	fi;
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit 0; quit 1;' -F 'quit 2; quit 3'; test "$$?" = 2
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -C 'quit 0; quit 1;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit 0; quit 1;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit 3'; test "$$?" = 3
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit 2' -F 'quit 0; quit 1;'
	for r in 0 1 2 254 255 ; do $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c "quit $$r" || test $$? = $$r ; done ;
	for r in 0 1 2 254 255 ; do $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -C "quit $$r" || test $$? = $$r ; done ;
	for r in 0 1 2 254 255 ; do $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'quit' -F "quit $$r" || test $$? = $$r ; done ;
if ENABLE_SDL
if ENABLE_PCX
	if test -n "$(DISPLAY)"; then \
		$(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) -o sdl -r w $(top_srcdir)/media/numbers_pcx/ -R -q -a -c 'display "reinit" "w200:200"' -c 'quit (i:swidth!=_screen_height) || (i:swidth!=_screen_width)'; # pcx needs no library\
	fi
endif
endif
if ENABLE_GIF
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep gif
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/icon_smile.gif -c 'rotate 10' -c quit
endif
	@top_srcdir=$(top_srcdir) FIMNORCOPTS="$(FIMNORCOPTS)" $(SHELL) $(top_srcdir)/scripts/tests/version.sh
	@top_srcdir=$(top_srcdir) FIMNORCOPTS="$(FIMNORCOPTS)" $(SHELL) $(top_srcdir)/scripts/tests/font.sh
	@test "`$(FIM_EXE) $(FIMNORCOPTS) -o dumb -R media/ -k 1 -k n -k 1 -k n -k q -F 'stdout _fileindex'`" == 3 # 3 and not 11
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media -c '_caption_over_image=3;display;quit;'
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'quit;'
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -E $(top_srcdir)/scripts/example/oneline.fim -c quit 
if ENABLE_PNG
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep png
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/image.png -c 'next'   -c quit # leak fixed
endif
if ENABLE_TIFF
	@if $(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep tiff && which convert ; then \
		convert $(top_srcdir)/media/image.png $(top_srcdir)/image.tiff && \
		$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/image.tiff -c 'next' -c quit; fi # leak fixed
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/ -c 'echo i:*' -k Tab -c quit # bogus test: should rather exercise MiniConsole
endif
if ENABLE_JPEG
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep jpeg
	if which wrjpgcom; then wrjpgcom -replace -comment 'add JPEG comment' $(top_srcdir)/media/image.jpg > image_commented.jpg && $(FIM_EXE) $(FIMNORCOPTS) -o dumb -X image_commented.jpg -c quit && rm -f image_commented.jpg; fi # leak fixed
	if which convert && which exiftool; then \
		convert $(top_srcdir)/media/icon_smile.gif $(top_srcdir)/media/icon_smile.jpg && \
		exiftool -exif:iso=42 -overwrite_original_in_place $(top_srcdir)/media/icon_smile.jpg && \
	      	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.jpg -c quit && \
	       	rm -f $(top_srcdir)/media/icon_smile.jpg ; \
	fi # only cover test exif
endif
if ENABLE_PNG
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/image.png -c '/*.jpg' -c quit # parser leak fixed
endif
	@test `$(FIM_EXE) $(FIMNORCOPTS) --help -B | wc -l` -lt `$(FIM_EXE) --help -B -B | wc -l`
	@test `$(FIM_EXE) $(FIMNORCOPTS) --help /a /q | wc -c` -gt 900
	@test `$(FIM_EXE) $(FIMNORCOPTS) --help help magnify _TERM jfngjfngjfn | wc -c` -gt 500
	@test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'dump_key_codes; stdout _last_cmd_output; quit' | wc -c` -gt 1000
	@test `$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help     | wc -c` -gt 30000
	@test `$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help     | wc -c` -lt 60000
	@test `$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help=man | wc -c` -gt 60000
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'exec "src/fimrc"; echo; stdout; pwd; status "status" unalias' -c quit #; cover test misc corner cases (note: no functionality check here)
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'autocmd; autocmd "PostDisplay";autocmd "PostDisplay" "pattern";autocmd "1" "2" "toomuch"' -c quit #; cover test misc corner cases (note: no functionality check here)
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'autocmd_del; autocmd_del "PostDisplay";autocmd_del "PostDisplay" "pattern";autocmd_del "1" "2" "3"' -c quit #; cover test misc corner cases (note: no functionality check here)
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'unbind; unbind "?"; bind "~"; bind "n"; bind "n" ""' -c quit
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'color "desaturate";color "negate";color "identity";color "colorblind" "daltonize";color "deuteranopia" "daltonize";color "protanopia" "daltonize";color "tritanopia" "daltonize";color "colorblind"' -c quit # only cover test
	if which timeout; then if ! timeout 2 $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o dumb -c '/media' -c 'quit' ; then echo 'Seems like -c "/<filename>" is broken!'; false; fi; fi
	@$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'quit 14' ; if test $$? == 14 ; then echo '[*] Error code return test PASSED' ; else echo 'Error code return test FAILED'; false ; fi
	make ltests
if HAVE_VALGRIND
	if ! ldd $(FIM_EXE) | grep libasan.so.5; then \
		make clean -C src/testdir ; \
		VALGRIND='valgrind --tool=memcheck --leak-check=yes --num-callers=15 --exit-on-first-error=yes --error-exitcode=255' \
			make ltests; \
	fi
endif
endif

.PHONY: ltests
ltests:
if ENABLE_JPEG
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep jpeg
	if test $(abs_builddir) = $(abs_srcdir) ; then make -C src/testdir ; fi
endif

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
if HAVE_RUNNABLE_TESTS
	$(FIM_EXE) $(FIMNORCOPTS) --help=m                    > doc/fim.man.in
	$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help=man   > doc/fimrc.man.in
endif

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

