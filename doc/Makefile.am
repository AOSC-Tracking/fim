
# $Id$

CLEANFILES= doctags *.o  tags

EXTRA_DIST = FIM.html FIM.TXT fim.man.html fim.man doctags.c fim-stylesheet.css ./vim2html.pl fimgs.man

man1_MANS = fim.man fimgs.man

# html stuff is installed in the documentation directories, too
htmldir = $(docdir)
html_DATA = fim.man.html FIM.html
doc_DATA = FIM.TXT

all:	docs

docs:        fim.man.html FIM.html

FIM.html: doctags
	./doctags FIM.TXT > tags ; ./vim2html.pl tags FIM.TXT

fim.man.html:
	man2html fim.man >  fim.man.html

doctags:
	        $(CC) -o doctags doctags.c

clean:
	        rm -f  doctags.o doctags FIM.html tags fim-stylesheet.css fim.man.html

