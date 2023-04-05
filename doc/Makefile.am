# $LastChangedDate$
#
HTMLDOCS=
if WITH_PERL
HTMLDOCS+=FIM.html
endif
if WITH_MAN2HTML
if WITH_PERL
HTMLDOCS+=fim.man.html fimgs.man.html fimrc.man.html
endif
endif

CLEANFILES= doctags$(EXEEXT) *.o  tags $(HTMLDOCS)

EXTRA_DIST = FIM.TXT fim.man doctags.c fim-stylesheet.css ./vim2html.pl fimgs.man fimrc.man fim.man.in fimgs.man.in fimrc.man.in
#MAN2HTML=man2html
MAN2HTML=$(MAN) --html=cat
MAN2HTMLS = sed 's/&minus;/-/g'
MAN2TXT=$(MAN) --ascii # when having e.g. a minimal groff installation

man1_MANS = fim.man fimgs.man
man5_MANS = fimrc.man

# html stuff is installed in the documentation directories, too
htmldir = $(docdir)
dist_html_DATA = $(HTMLDOCS)
dist_doc_DATA = FIM.TXT

.PHONY: all
all:	$(HTMLDOCS) $(EXTRA_DIST)

FIM.html fim-stylesheet.css: doctags$(EXEEXT) FIM.TXT
	./doctags$(EXEEXT) $(srcdir)/FIM.TXT > tags ; $(srcdir)/vim2html.pl tags $(srcdir)/FIM.TXT

fim.man:
	make doc/fim.man -C $(abs_top_builddir)/

fim.man.html: fim.man
	$(MAN2HTML) ./$< | $(MAN2HTMLS) > $@
	if test ! -s $@ ; then $(MAN2TXT) ./$< > $<.txt && $(srcdir)/vim2html.pl tags $<.txt && mv $<.txt.html $@ ; fi

fimrc.man:
	make doc/fimrc.man -C $(abs_top_builddir)/

fimrc.man.html: fimrc.man
	$(MAN2HTML) ./$< | $(MAN2HTMLS) > $@
	if test ! -s $@ ; then $(MAN2TXT) ./$< > $<.txt && $(srcdir)/vim2html.pl tags $<.txt && mv $<.txt.html $@ ; fi

fimgs.man:
	make doc/fimgs.man -C $(abs_top_builddir)/

fimgs.man.html: fimgs.man
	$(MAN2HTML) ./$< | $(MAN2HTMLS) > $@
	if test ! -s $@ ; then $(MAN2TXT) ./$< > $<.txt && $(srcdir)/vim2html.pl tags $<.txt && mv $<.txt.html $@ ; fi

doctags$(EXEEXT): doctags.c
	        $(CC) -o $@ $<

#clean:
#	        $(RM) -f  doctags.o doctags$(EXEEXT) FIM.html tags fim-stylesheet.css fim.man.html fimgs.man.html

