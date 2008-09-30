
# $Id$

CLEANFILES= doctags *.o  tags

EXTRA_DIST = FIM.TXT fim.man doctags.c fim-stylesheet.css ./vim2html.pl fimgs.man $(HTMLDOCS)

man1_MANS = fim.man fimgs.man

# html stuff is installed in the documentation directories, too
htmldir = $(docdir)
html_DATA = $(HTMLDOCS)
doc_DATA = FIM.TXT

all:	docs

docs:        $(HTMLDOCS)

FIM.html: doctags FIM.TXT
	./doctags FIM.TXT > tags ; ./vim2html.pl tags FIM.TXT

fim.man.html: fim.man
	man2html fim.man >  fim.man.html

fimgs.man.html: fimgs.man
	man2html fimgs.man >  fimgs.man.html

doctags:
	        $(CC) -o doctags doctags.c

clean:
	        rm -f  doctags.o doctags FIM.html tags fim-stylesheet.css fim.man.html fimgs.man.html

