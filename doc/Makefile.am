
# $LastChangedDate$

HTMLDOCS=FIM.html fim.man.html  fimgs.man.html fimrc.man.html

CLEANFILES= doctags *.o  tags $(HTMLDOCS)

EXTRA_DIST = FIM.TXT fim.man doctags.c fim-stylesheet.css ./vim2html.pl fimgs.man fimrc.man
#MAN2HTML=man2html
MAN2HTML=man --html=cat

man1_MANS = fim.man fimgs.man fimrc.man

# html stuff is installed in the documentation directories, too
htmldir = $(docdir)
dist_html_DATA = $(HTMLDOCS)
dist_doc_DATA = FIM.TXT

all:	htmldocs  $(EXTRA_DIST)

htmldocs:        $(HTMLDOCS)
#docs:        $(HTMLDOCS)

FIM.html fim-stylesheet.css: doctags FIM.TXT
	./doctags FIM.TXT > tags ; ./vim2html.pl tags FIM.TXT

fim.man.html: fim.man
	$(MAN2HTML) ./fim.man >  fim.man.html || echo "$(ERRMSG) $(MAN2HTML) ./fim.man >  fim.man.html"

fimrc.man.html: fimrc.man
	$(MAN2HTML) ./fimrc.man >  fimrc.man.html || echo "$(ERRMSG) $(MAN2HTML) ./fimrc.man >  fimrc.man.html"

ERRMSG="no $(MAN2HTML) at build time : please install $(MAN2HTML) and reissue :"

fimgs.man.html: fimgs.man
	$(MAN2HTML) ./fimgs.man >  fimgs.man.html || echo "$(ERRMSG) $(MAN2HTML) ./fimgs.man >  fimgs.man.html"

doctags:
	        $(CC) -o doctags doctags.c

#clean:
#	        rm -f  doctags.o doctags FIM.html tags fim-stylesheet.css fim.man.html fimgs.man.html

