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
	$(top_srcdir)/media/special/postscript.eps \
	$(top_srcdir)/media/special/images.list \
	$(top_srcdir)/media/special/dir.desc \
	$(top_srcdir)/media/special/collated.list \
	$(top_srcdir)/media/special/loadable.desc \
	$(top_srcdir)/media/special/0_8bit.pcx.bin \
	$(top_srcdir)/media/special/0_gray.tiff \
	$(top_srcdir)/media/special/0_rgba.tiff \
	$(top_srcdir)/media/special/0_monochrome_white_polarity.tiff \
	$(top_srcdir)/media/special/0_16bps_msb.tiff \
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
FIM_USE_ASCII_ART_DEFAULT=ca
else
if ENABLE_AA
FIM_USE_ASCII_ART_DEFAULT=aa
endif
endif

.PHONY: fbtests
fbtests: all
if WITH_EXPECT
	$(TIMEOUT_SMALL) \
		$(EXPECT) $(top_srcdir)/scripts/tests/spawn_and_quit.exp $(FIM_EXE) $(FIMNORCOPTS) -o fb $(top_srcdir)/media/
endif
	$(TIMEOUT_SMALL) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o fb $(FIMVIDEOTESTOPTS) 
	if test "$(TERM)" != screen.linux && which screen; then \
		test `screen -q $(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o fb=S $(FIMVIDEOTESTOPTS) -c 'stdout "fail"' | wc -l` == 0; \
	fi # screen does not propagate commands's return code, therefore the stdout-based trick
	if test "$(TERM)" == screen.linux ; then \
		$(TIMEOUT_SMALL) \
			$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o fb=S $(FIMVIDEOTESTOPTS); test $$? = 255; \
	fi
.PHONY: aatests
aatests: all
if WITH_EXPECT
	$(NO_ASAN) $(TIMEOUT_SMALL) \
		$(EXPECT) $(top_srcdir)/scripts/tests/spawn_and_quit.exp $(FIM_EXE) $(FIMNORCOPTS) -o aa $(top_srcdir)/media/
endif
	$(TIMEOUT_LARGE) $(FIM_EXE) $(FIMNORCOPTS) -o aa --sanity-check # differently than -o dumb, this stresses DebugConsole's grow() mechanism
	$(FIM_EXE) $(top_srcdir)/media/ -t -c 'console_scroll_up;console_scroll_down;console_scroll_reset;quit' # placeholder for a better test
	$(NO_ASAN) $(TIMEOUT_SMALL) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o aa   $(FIMVIDEOTESTOPTS)
	if test -n "$(DISPLAY)"; then \
		$(NO_ASAN) $(TIMEOUT_LARGE) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o aa=w $(FIMVIDEOTESTOPTS); \
	fi
	test `SSH_TTY=1       $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/                     -c 'stderr _device_string' -kq   2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #
	test `(unset SSH_TTY; $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -t --no-framebuffer -c 'stderr _device_string' -kq ) 2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #
	test `(      SSH_TTY= $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -t --no-framebuffer -c 'stderr _device_string' -kq ) 2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #

.PHONY: cacatests
cacatests: all
if WITH_EXPECT
	$(TIMEOUT_SMALL) \
		$(EXPECT) $(top_srcdir)/scripts/tests/spawn_and_quit.exp $(FIM_EXE) $(FIMNORCOPTS) -o ca $(top_srcdir)/media/
endif
	$(TIMEOUT_SMALL) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o ca  $(FIMVIDEOTESTOPTS)
	if test -n "$(DISPLAY)"; then \
		$(NO_ASAN) $(TIMEOUT_LARGE) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o ca=w $(FIMVIDEOTESTOPTS); \
	fi
	test `SSH_TTY=1       $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/                     -c 'stderr _device_string' -kq   2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #
	test `(unset SSH_TTY; $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -t --no-framebuffer -c 'stderr _device_string' -kq ) 2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #
	test `(      SSH_TTY= $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -t --no-framebuffer -c 'stderr _device_string' -kq ) 2>&1 > /dev/null` == $(FIM_USE_ASCII_ART_DEFAULT) #


.PHONY: sdltests
sdltests: all
	$(NO_ASAN) $(TIMEOUT_LARGE) \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o sdl=h  $(FIMVIDEOTESTOPTS);
if FIM_WANT_HARDCODED_CONSOLEFONT
	$(NO_ASAN) FBFONT=       \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o sdl -c quit # TODO: this is only a coverage test
	$(NO_ASAN) FBFONT=fim:// \
		$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o sdl -c quit # TODO: this is only a coverage test
endif

# this rule is deprecated, too: testing should be performed with 'make tests' (FIXME)
.PHONY: sanity-check
sanity-check:	all
	@$(FIM_EXE) $(FIMNORCOPTS) -E $(top_srcdir)/scripts/tests/sanity.fim -t

.PHONY: check
check:	tests

# this is the official way of testing fim
.PHONY: tests
tests:	all
	$(TIMEOUT_LARGE) $(FIM_EXE) $(FIMNORCOPTS) -o dumb --sanity-check
	ulimit -c unlimited || true
	ulimit -v 32000 || true
	ulimit -t 1000 || true
	@ ! grep '\<'$(USER)'\>' doc/*man* # against namespace leaks
if ENABLE_PCX
endif
if ENABLE_PCX
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/numbers_pcx/1.pcx -c 'a=_filelistlen;limit "~="; list;b=_filelistlen; stdout a.".".b;quit'` = 2.1
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/../media/numbers_pcx/1.pcx -c 'a=_filelistlen;limit "~="; list;b=_filelistlen; stdout a.".".b;quit'` = 2.2
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/numbers_pcx/1.pcx -c 'a=_filelistlen;limit "~^"; list;b=_filelistlen; stdout a.".".b;quit'` = 2.1
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/../media/numbers_pcx/1.pcx -c 'a=_filelistlen;limit "~^"; list;b=_filelistlen; stdout a.".".b;quit'` = 2.1
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/../media/numbers_pcx/1.pcx -c 'a=_filelistlen;limit "~$$"; list;b=_filelistlen; stdout a.".".b;quit'` = 2.1
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/../media/numbers_pcx/1.pcx -c 'a=_filelistlen;limit "~!"; list;b=_filelistlen; stdout a.".".b;quit'` = 2.0
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/1.pcx $(top_srcdir)/media/../media/numbers_pcx/2.pcx -c 'a=_filelistlen;limit "~!"; list;b=_filelistlen; stdout a.".".b;quit'` = 2.2
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_filelistlen;limit "~i" "4"; b=_filelistlen; limit "~i" "4-4"; c=_filelistlen; limit "~i" "4-5"; d=_filelistlen; stdout a.".".b.".".c.".".d;quit'` == 10.1.1.2
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_filelistlen;limit "~z" "0-900"; b=_filelistlen; limit "~z" "0-909"; c=_filelistlen; stdout a.".".b.".".c;quit'` == 10.0.7 # needs stat
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_filelistlen;limit "!"; b=_filelistlen; stdout a.".".b;quit'` == 10.0 # needs stat
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_filelistlen;limit "~d" "01/01/1970-01/01/2007"; b=_filelistlen; limit "~d" "01/01/2007-19/01/2038"; c=_filelistlen; stdout a.".".b.".".c;quit'` == 10.0.10
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_filelistlen;limit "prime" "yes"; b=_filelistlen; limit "prime" "no" ; c=_filelistlen; limit "prime" "zzz" ; d=_filelistlen; stdout a.".".b.".".c.".".d;quit' --load-image-descriptions-file $(top_srcdir)/media/special/images.desc` == 10.3.2.0
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_filelistlen;limit "uno"; b=_filelistlen;limit "due"; c=_filelistlen; limit "boh"; d=_filelistlen; stdout a.".".b.".".c.".".d;quit' --load-image-descriptions-file $(top_srcdir)/media/special/images.desc` = 10.1.1.0
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'limit "-list";stdout _last_cmd_output;limit "-listall";stdout _last_cmd_output; quit;' --load-image-descriptions-file $(top_srcdir)/media/special/images.desc | grep there.are.*variable > /dev/null # TODO: need better test here
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'limit "-list" "prime";stdout _last_cmd_output;limit "-listall" "prime";stdout _last_cmd_output; quit;' --load-image-descriptions-file $(top_srcdir)/media/special/images.desc | grep there.are.*value > /dev/null # TODO: need better test here
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/special/0_8bit.pcx.bin -C 'stdout "Unsupported PCX file test.."' -c 'quit !(i:width==0)' # named so to not break dir-loading tests
endif
if ENABLE_PCX
if ENABLE_PNG
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "-/5/";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.8.12
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "-/B";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.12.12
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "+/b";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.0.12
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "+/B";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.2.12
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "+/s";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.2.12
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "+/d";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.3.12
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.png $(top_srcdir)/media/fim.png $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "+/u";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit' | grep 0.0.0.12
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort";a=_fileindex; goto;b=_fileindex; goto "+50%";c=_fileindex; stdout a.".".b.".".c."."._filelistlen;quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c 'list "sort"; a=_fileindex; goto "+prime"; b=_fileindex;goto "+prime"; c=_fileindex; goto "+prime"; d=_fileindex; goto "+prime"; e=_fileindex; stdout a.".".b.".".c.".".d.".".e; quit' | grep 0.2.5.6.1
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c 'list "sort_fsize"; goto 0; reload; stdout i:_filename; quit' | grep 1.pcx
endif
if ENABLE_PCX
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort"; list;l1=_last_cmd_output;list "random_shuffle";list;l2=_last_cmd_output;if(l1==l2)quit 255; quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx -c 'list "sort"; next;f=i:_filename;list "swap";     reload;if(f==i:_filename)quit 255; quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx $(top_srcdir)/media/numbers_pcx/*.pcx --sort -c 'list "pop"; if(_filelistlen!= 9) quit 255;quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*.pcx $(top_srcdir)/media/numbers_pcx/*.pcx        -c 'list "pop"; if(_filelistlen!=19) quit 255;quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'a=1' -c 'a=a+1;' -c 'quit (1-(a==2))'
endif
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdir"  _pwd;stdout l;if(l==_filelistlen)quit 255; quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdirr" _pwd;stdout l;if(l==_filelistlen)quit 255; quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdir"      ;stdout l;if(l==_filelistlen)quit 255; quit'
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(top_srcdir)/media/"' -c 'stdout _pwd;l=_filelistlen;list "pushdirr"     ;stdout l;if(l==_filelistlen)quit 255; quit'
if ENABLE_RAW_BITS_RENDERING
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --as-text -c quit
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --as-text   -c 'stdout _last_file_loader;quit'` == Text  # mostly coverage
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --binary=1  -c 'stdout _last_file_loader;quit'` == Bit1  # mostly coverage
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --binary=24 -c 'stdout _last_file_loader;quit'` == Bit24 # mostly coverage
if WANT_READ_STDIN_IMAGE
	test `echo -n U | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-from-stdin --binary=1  -c 'magnify;stdout i:height."."._last_file_loader;quit'` == 1.Bit1  # coverage of tiny case; TODO: remove necessity of magnify
	test `echo -n U | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-from-stdin --binary=24 -c 'magnify;stdout i:height."."._last_file_loader;quit'` == 1.Bit24 # coverage of tiny case; TODO: remove necessity of magnify
endif
endif
	$(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o dumb -c 'window;stdout _last_cmd_output;quit;' grep disabled > /dev/null
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ --dump-reference-help | wc -l` -gt 100
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=m | wc -c` -gt 28000 # man dump
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=m | grep fB | wc -l` -gt 40 # lots of troff
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=l | grep fB | wc -l` -eq 0  # no troff left (hopefully)
	test `$(FIM_EXE) $(FIMNORCOPTS) --NON-existent-OPTIONNN | wc -l` -gt 70 # help function
	$(FIM_EXE) $(FIMNORCOPTS)       --NON-existent-OPTIONNN; test $$? == 240 # help function failing
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
	cat $(top_srcdir)/media/numbers_pcx/1.pcx | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-from-stdin -c 'quit !(i:width>0 && i:height>0)'
	echo | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-from-stdin -kq
	echo | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --read-from-stdin ; test $$? = 248
	echo $(top_srcdir)/media/numbers_pcx/* | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --read-from-stdin --read-from-stdin-elds ' ' -c 'quit !(_filelistlen==9)'
	echo 'quit 33' | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --script-from-stdin              ; test $$? = 33
	echo 'quit 33' | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --script-from-stdin -Kq          ; test $$? = 33
	echo 'quit 33' | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --script-from-stdin -kq          ; test $$? = 33
	echo 'quit 33' | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --script-from-stdin -C 'quit 22' ; test $$? = 22
	echo 'quit 33' | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --script-from-stdin -c 'quit 22' ; test $$? = 22
	echo 'quit 33' | $(FIM_EXE) $(FIMNORCOPTS) -o dumb --script-from-stdin -F 'quit 22' ; test $$? = 22
endif
if ENABLE_ARCHIVE
	tar czf archive.tgz $(top_srcdir)/media/numbers_pcx/ && $(TIMEOUT_SMALL) $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb archive.tgz -c 'stdout i:pages." pages";if(i:pages==1)quit 255;quit' && rm -f archive.tgz
	echo 'not an archive' > archive.tgz; $(TIMEOUT_SMALL) $(abs_builddir)/$(FIM_EXE) $(FIMNORCOPTS) -o dumb archive.tgz -c 'stdout i:pages." pages";if(i:pages!=0)quit 255;quit' && rm -f archive.tgz
endif
if WANT_READ_STDIN_IMAGE
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'pread "cat $(top_srcdir)/media/numbers_pcx/1.pcx"; reload' -c 'quit !(i:width>0 && i:height>0)'
endif
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'cd "$(abs_srcdir)/media/numbers_pcx/1.pcx"; pwd;stdout _last_cmd_output;  cd "-"; pwd;stdout _last_cmd_output;  cd "$(abs_srcdir)/media/numbers_pcx/";pwd;stdout _last_cmd_output; quit' | tr -d '\n'` == "$(abs_srcdir)/media/numbers_pcx""$(abs_srcdir)""$(abs_srcdir)/media/numbers_pcx"
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -C '_file_loader="ppm"' $(top_srcdir)/media/numbers_pcx/1.pcx -c 'stdout i:width; stdout (i:width!=0 && i:height!=0);quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -C '_file_loader="pcx"' $(top_srcdir)/media/numbers_pcx/1.pcx -c 'stdout i:width; stdout (i:width==0 && i:height==0);quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --mark-from-image-descriptions-file $(top_srcdir)/media/special/images.desc -c 'l=_filelistlen;toggleLimitMarked;if(_filelistlen!=l/2)quit 255; quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-descriptions-file-separator $$'\t' $(top_srcdir)/media/special/loadable.desc $(top_srcdir)/media/numbers_pcx/ -c '/2.pcx;if(i:_comment=~"due")quit;quit 255'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb --image-descriptions-file-separator '?'    $(top_srcdir)/media/special/loadable.desc $(top_srcdir)/media/numbers_pcx/ -c '/2.pcx;if(i:_comment=~"due")quit 255;quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/special/dir.desc -c 'quit !(_filelistlen==2)'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --image-descriptions-file-separator $$'\t' --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/2.pcx;if(i:_comment=~"due")quit;quit 255'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --image-descriptions-file-separator '?'    --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/2.pcx;if(i:_comment=~"due")quit 255;quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/1.pcx;if(i:prime=="no")quit 255;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/4.pcx;if(i:prime=="yes")quit 255;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/2.pcx;if(i:_comment!="due")quit 255;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/4.pcx;quit (i:_comment)'; test $$? == 44 
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c '/0.pcx;quit (i:_comment)'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-image-descriptions-file $(top_srcdir)/media/special/images.desc -c 'list "vars"; if(! _last_cmd_output =~ "vals.*files") quit 255;;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ -c 'desc "load" "$(top_srcdir)/media/special/images.desc"; desc "reload" "$(top_srcdir)/media/special/images.desc"; desc "save" "tmp.desc"; quit;' && test `wc -w < tmp.desc` == 15 && rm tmp.desc # need proper 'reload' test
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ -c 'desc "load" "$(top_srcdir)/media/special/images.desc" "\t"; desc "reload" "$(top_srcdir)/media/special/images.desc" "\t"; desc "save" "tmp.desc" "X"; quit;' && test `grep X tmp.desc |  wc -w` == 10 && rm tmp.desc # need proper 'load' with separator test
	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-shadow-directory $(top_srcdir)/media/../ -c 'scale "shadow"; if (_last_cmd_output =~ "image substituted")quit 0; else quit 255;'; fi
	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/ --load-shadow-directory $(top_srcdir)/var/  -c 'scale "shadow"; if (_last_cmd_output =~ "image substituted")quit 0; else quit 255;'; test $$? = 255; fi
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*  -/  media/numbers_pcx/4.pcx -c 'stdout _fileindex;if(_fileindex!=0){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/*  -/                    4.pcx -c 'stdout _fileindex;if(_fileindex!=5){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --/  media/numbers_pcx/4.pcx -c 'stdout _fileindex;if(_fileindex!=0){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --/                    4.pcx -c 'stdout _fileindex;if(_fileindex!=5){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --// media/numbers_pcx/4.___ -c 'stdout _fileindex;if(_fileindex!=0){quit 255}quit'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/numbers_pcx/* --// media/numbers_pcx/4.pcx -c 'stdout _fileindex;if(_fileindex!=5){quit 255}quit'
endif
if ENABLE_JPEG
if WANT_READ_STDIN_IMAGE
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'stdout i:width.i:height; eval "quit"' -i < $(top_srcdir)/media/image.jpg | tail -1` != "00" # mostly coverage
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/image.jpg -K++++ -c 'flip;mirror;pan_left;pan_right;pan_up;pan_down;pan_ne;pan_sw;pan_se;pan_nw "100%"' -c 'popen "echo quit"' # mostly coverage
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
	@if test -n "$(DISPLAY)"; then make sdltests; fi
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
	if test -n "$(DISPLAY)"; then $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) -o sdl=17x17 -P $(top_srcdir)/media/numbers_pcx/1.pcx  -q -c 'quit (_autotop!=1) || (i:swidth!=_screen_width)'; fi # --text-reading
	if test -n "$(DISPLAY)"; then $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) -o sdl=17x17    $(top_srcdir)/media/numbers_pcx/1.pcx  -q -c 'quit (_autotop!=1) || (i:swidth!=_screen_width)'; test $$? = 1; fi # --text-reading
	if test -n "$(DISPLAY)"; then $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) -o sdl=17:16 -q -P $(top_srcdir)/media/numbers_pcx/?.pcx -c 'align "info";a1=_last_cmd_output; scroll "forward";align "info";a2=_last_cmd_output; next; align "info";a3= _last_cmd_output; goto _lastfileindex; align "info";a4=_last_cmd_output; quit !(a1==a3 && a2==a4 && a1!=a2)' -kq ; fi # --text-reading
	if test -n "$(DISPLAY)"; then $(NO_ASAN) $(FIM_EXE) $(FIMNORCOPTS) -o sdl=17:16 -q    $(top_srcdir)/media/numbers_pcx/?.pcx -c 'align "info";a1=_last_cmd_output; scroll "forward";align "info";a2=_last_cmd_output; next; align "info";a3= _last_cmd_output; goto _lastfileindex; align "info";a4=_last_cmd_output; quit !(a1==a3 && a2==a4 && a1==a2)' -kq ; fi # --text-reading
endif
endif
if ENABLE_GIF
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep gif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/icon_smile.gif -c 'rotate 10' -c quit
endif
	top_srcdir=$(top_srcdir) FIMNORCOPTS="$(FIMNORCOPTS)" $(SHELL) $(top_srcdir)/scripts/tests/version.sh
	top_srcdir=$(top_srcdir) FIMNORCOPTS="$(FIMNORCOPTS)" $(SHELL) $(top_srcdir)/scripts/tests/font.sh
	test "`$(FIM_EXE) $(FIMNORCOPTS) -o dumb -R media/ -k 1 -k n -k 1 -k n -k q -F 'stdout _fileindex'`" == 3 # 3 and not 11
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media -c '_caption_over_image=3;display;quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'quit;'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -E $(top_srcdir)/scripts/example/oneline.fim -c quit 
if ENABLE_PNG
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep png
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/image.png -c 'next'   -c quit # leak fixed
if WITH_INKSCAPE
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb    $(top_srcdir)/media/sample.svg -c 'quit !(i:width>0 && i:height>0)' #
endif
endif
if ENABLE_TIFF
	if $(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep tiff && which convert ; then \
		convert $(top_srcdir)/media/image.png $(top_srcdir)/image.tiff && \
		$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/image.tiff -c 'next' -c quit; fi # leak fixed
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/ -c 'echo i:*' -k Tab -c quit # bogus test: should rather exercise MiniConsole
	if which convert ; then \
		convert $(top_srcdir)/media/numbers_pcx/*.pcx $(top_srcdir)/image.tiff && \
		$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/image.tiff -c 'quit !(i:pages == 10)'; fi # multipage code in Image and load
	rm $(top_srcdir)/image.tiff
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/special/0_gray.tiff                      -c 'i:width;quit !(i:width!=0)' # convert media/numbers/0.gif -grayscale average media/special/0_gray.tiff
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/special/0_rgba.tiff                      -c 'i:width;quit !(i:width!=0)' # convert media/numbers/0.gif -alpha transparent -depth 4 media/special/0_rgba.tiff
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/special/0_monochrome_white_polarity.tiff -c 'i:width;quit !(i:width!=0)' # convert media/numbers/0.gif -monochrome -define quantum:polarity=min-is-white -depth 1 media/special/0_monochrome_white_polarity.tiff
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/special/0_16bps_msb.tiff -c 'i:width;quit !(i:width!=0)' # convert media/numbers/0.gif -depth 16 -colorspace srgb -define tiff:endian=msb media/special/0_16bps_msb.tiff
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c 'info;stdout _last_cmd_output;quit' | grep 'media.*KB' # cover info
if ENABLE_JPEG
	$(FIM_EXE) $(FIMNORCOPTS)         -V | grep upported.file.formats | grep jpeg
	if which wrjpgcom; then wrjpgcom -replace -comment 'add JPEG comment' $(top_srcdir)/media/image.jpg > image_commented.jpg && $(FIM_EXE) $(FIMNORCOPTS) -o dumb -X image_commented.jpg -c quit && rm -f image_commented.jpg; fi # leak fixed
	if which convert && which exiftool; then \
		convert $(top_srcdir)/media/icon_smile.gif $(top_srcdir)/media/icon_smile.jpg && \
		exiftool -exif:iso=42 -overwrite_original_in_place $(top_srcdir)/media/icon_smile.jpg && \
	      	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.jpg -c quit && \
	       	rm -f $(top_srcdir)/media/icon_smile.jpg ; \
	fi # only cover test exif
	cp $(top_srcdir)/media/image.jpg $(abs_srcdir)/media/upc_abs.JPG && $(FIM_EXE) $(FIMNORCOPTS) -o dumb --no-stat-push $(abs_srcdir)/media/upc_abs.JPG -c 'quit !(i:width>0 && i:height>0)' && rm -f $(abs_srcdir)/media/upc_abs.JPG # not really jpg-specific, exploits regexps & co
endif
if ENABLE_PS
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/special/postscript.eps -c 'quit !(i:width>0 && i:height>0)'
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -C '_downscale_huge_at_load=-1;_preferred_rendering_dpi=100' $(top_srcdir)/media/special/postscript.eps -c 'quit !(i:width>0 && i:height>0)'
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -C '_downscale_huge_at_load=-1;_preferred_rendering_dpi=100' $(top_srcdir)/media/special/postscript.eps -c 'stdout i:width;quit'` -lt `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -C '_downscale_huge_at_load=-1;_preferred_rendering_dpi=200' $(top_srcdir)/media/special/postscript.eps -c 'stdout i:width;quit'`
endif
if ENABLE_PNG
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X $(top_srcdir)/media/image.png -c '/*.jpg' -c quit # parser leak fixed
endif
	$(TIMEOUT_SMALL) $(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'identifier /*path...' -k q # parser leak fixed with destructor lines in yacc.ypp
	$(FIM_EXE) $(FIMNORCOPTS) -hs -h | grep h.s -q
	test `$(FIM_EXE) $(FIMNORCOPTS) --help   | wc -c` -eq `$(FIM_EXE) $(FIMNORCOPTS) --help=s | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=s | wc -l` -eq `$(FIM_EXE) $(FIMNORCOPTS) --help=d | wc -l`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=s | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=d | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=d | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=l | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=l | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=m | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=l | wc -l` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=m | wc -l`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help _last_cmd_output | wc -l` == 5
	test `$(FIM_EXE) $(FIMNORCOPTS) --help _LAST_CMD_OUTPUT | wc -l` == 2
	test `$(FIM_EXE) $(FIMNORCOPTS) -hs -h | wc -l` = 2
	test `$(FIM_EXE) $(FIMNORCOPTS) -hd -h | wc -l` = 2
	test `$(FIM_EXE) $(FIMNORCOPTS) -hl -h | wc -l` = 3
	test `$(FIM_EXE) $(FIMNORCOPTS) -hm -h | wc -l` = 3
	test `$(FIM_EXE) $(FIMNORCOPTS) -hs -h | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) -hd -h | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS)  -h      -R          | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS)  -h      -R       -R | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS)  -hs     -R          | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS)  -hd     -R          | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS)  -hd     -R          | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS)  -hl     -R          | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS)  -hl     -R          | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS)  -hm     -R          | wc -c`
if WANT_READ_STDIN_IMAGE
	test `$(FIM_EXE) $(FIMNORCOPTS) --help   --read-from-stdin | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help   --read-from-stdin --read-from-stdin | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=s --read-from-stdin | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=d --read-from-stdin | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=d --read-from-stdin | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=l --read-from-stdin | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=l --read-from-stdin | wc -c` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=m --read-from-stdin | wc -c`
	test `$(FIM_EXE) $(FIMNORCOPTS) --help=l --read-from-stdin | wc -l` -lt `$(FIM_EXE) $(FIMNORCOPTS) --help=m --read-from-stdin | wc -l`
endif
#	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb --background-recursive -B . -kK -kq -F 'if(_filelistlen==1)quit 255'; fi # -kK triggers a yield (often, not always)
#	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb --background-recursive -B .     -kq -F 'if(_filelistlen!=1)quit 255'; fi # no -kK -> no background images in batch mode (often, not always)
	if $(CXX11TEST); then $(FIM_EXE) $(FIMNORCOPTS) -o dumb --background-recursive -B .     -kq -F 'if(_filelistlen< 1)quit 255'; fi # difficult to write a reliable test for -B
	test `$(FIM_EXE) $(FIMNORCOPTS) --help /a /q | wc -c` -gt 900
	test `$(FIM_EXE) $(FIMNORCOPTS) --help help magnify _TERM jfngjfngjfn | wc -c` -gt 500
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'dump_key_codes; stdout _last_cmd_output; quit' | wc -c` -gt 1000
	test `$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help     | wc -c` -gt 30000
	test `$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help     | wc -c` -lt 60000
	test `$(FIM_EXE) $(FIMNORCOPTS) --dump-reference-help=man | wc -c` -gt 60000
if WANT_DEFAULT_FIMRC
	test `$(FIM_EXE) $(FIMNORCOPTS) --dump-default-fimrc      | wc -c` -gt 15000
endif
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'exec "src/fimrc"; echo; stdout; pwd; status "status" unalias' -c quit #; cover test misc corner cases (note: no functionality check here)
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'autocmd; autocmd "PostDisplay";autocmd "PostDisplay" "pattern";autocmd "1" "2" "toomuch"' -c quit #; cover test misc corner cases (note: no functionality check here)
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'autocmd_del; autocmd_del "PostDisplay";autocmd_del "PostDisplay" "pattern";autocmd_del "1" "2" "3";autocmd_del "1" "2" "3" "4"' -c quit #; cover test misc corner cases (note: no functionality check here)
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'alias "a" "quit 11"; unalias "a"; alias "a" "quit 23"; a; quit 13' -c quit ; test $$? == 23
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'alias "a" "quit 23"; unalias "-a"; a; quit 13' -c quit ; test $$? == 13
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'unalias; stdout _last_cmd_output; unalias "NotExistent"; stdout _last_cmd_output;quit' | sed 's/unalias.*$$/Un/g' | tr -d '\n'` == UnUn
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c "bind 'z' 'stdout 1';_max_iterated_commands=2;" -k 10z -k q | tr -d '\n'` == 11 # test binding repeat
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c "bind 'z' 'stdout 1';_max_iterated_commands=1;" -k 10z -k q | tr -d '\n'` == 1 # test binding repeat
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c "bind 'z' 'stdout 1';_max_iterated_commands=0;" -k 10z -k q | tr -d '\n'` == 1111111111 # test binding repeat
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.pgm $(top_srcdir)/media/icon_smile.ppm -K /smile.ppm -K '' -K q -F "quit !(_fileindex==2);" # test slash search
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/icon_smile.pgm $(top_srcdir)/media/icon_smile.ppm -K /smole.ppm -K '' -K q -F "quit !(_fileindex==0);" # test slash search
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c 'v=1; bind "_" "recording;stdout _last_cmd_output;"; bind "S" "recording \"start\""' -K _S_rQ_E_q -F 'stdout i:_orientation' | wc -w ` -gt 70
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c 'v=1; bind "_" "recording;                        "; bind "S" "recording \"start\""' -K _S_rQ_E_q -F 'stdout i:_orientation' | wc -w ` -lt 70
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c 'v=1; bind "_" "recording;stdout _last_cmd_output;"; bind "S" "recording \"start\""' -K _S_rQ_E_q -F 'stdout i:_orientation' | grep ^.$$ | tr -d '\n'` == 2
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -C 'v=1; bind "_" "recording;stdout _last_cmd_output;"; bind "S" "recording \"start\""' -K _S_rQ_E_q -F 'stdout i:_orientation' | grep ^.$$ | tr -d '\n'` == 1
	test "`$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c 'stderr; stdout; quit' 2>&1 > /dev/null | grep echo`" == "echo command" # stdout vs stderr
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -c 'stderr "out:due";stdout "out:uno";quit' 2>&1 > /dev/null | grep out` == "out:due" # stdout vs stderr
if WANT_READLINE
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media -C set_commandline_mode -K 'stdout "cli"; quit;' -K ''` = cli
endif
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'set; stdout _last_cmd_output; quit' | wc -w` -ge 50 # test set
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'set "_TERM"; stdout _last_cmd_output; quit' | wc -w` -eq 1 # test set
	test `$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'set "_TERM" "WhatWhat"; stdout _last_cmd_output; set 1 2 3 ; stdout _last_cmd_output; quit' | sed "s/set.*$$/Set/g" | tr -d '\n'` == WhatWhatSetSetSet
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -c 'unbind; unbind "?"; bind "~"; bind "n"; bind "n" ""' -c quit
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb $(top_srcdir)/media/ -c 'color "desaturate";color "negate";color "identity";color "colorblind" "daltonize";color "deuteranopia" "daltonize";color "protanopia" "daltonize";color "tritanopia" "daltonize";color "colorblind"' -c quit # only cover test
	if which timeout; then if ! timeout 2 $(FIM_EXE) $(FIMNORCOPTS) $(top_srcdir)/media/ -o dumb -c '/media' -c 'quit' ; then echo 'Seems like -c "/<filename>" is broken!'; false; fi; fi
	$(FIM_EXE) $(FIMNORCOPTS) -o dumb -X -c 'quit 14' ; if test $$? == 14 ; then echo '[*] Error code return test PASSED' ; else echo 'Error code return test FAILED'; false ; fi
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

