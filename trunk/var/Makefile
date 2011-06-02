
# $Id$

FWP=http://www.autistici.org/dezperado/fim
INDEXIN=index.html.in
FIMDIR=..
FIMSITE=html-dezperado/fim
SITEDIR=./www
FTPSCRIPT=./script.lftp
FTPURL=ftp://dezperado@ftp5.autistici.org
SRCDIR=./
MIRROR=$$(cat $(FIMDIR)/README | grep "^Releases  "|sed s/^.*\ :.//g)
MAILINGLIST=$$(cat $(FIMDIR)/README | grep "^Mailing List "|sed s/^.*\ :.//g)
URL=$(MIRROR)/"fim-`cat $(FIMDIR)/VERSION`".tar.gz

TGZ="fim-`cat $(FIMDIR)/VERSION`".tar.gz
SIG=$(TGZ).sig
REPOSITORY=`cat $(FIMDIR)/README | grep "^Repository"|sed s/^.*\ :.//g`
FVERSION=`cat $(FIMDIR)/VERSION`
HOMEPAGE=$$(cat $(FIMDIR)/README | grep "^Web Page  "|sed s/^.*\ :.//g)
PACKAGE=fim
FIMHTMLDOCS= FIM.html fimrc.man.html fim.man.html fimgs.man.html
PAGES= $(SITEDIR)/index.html $(FIMHTMLDOCS)
CP=cp -p

help:
	@echo
	@echo HELP : 'make site | make up | make all'
	@echo

#tgz: $(SITEDIR)/$(TGZ)
tgz:
	cd $(FIMDIR) && make doc && make signed-tgzdist && cd -
	
all:	site up

$(FIMHTMLDOCS):
	cd $(FIMDIR)/doc && make $(FIMHTMLDOCS)

$(SITEDIR)/index.html:
	m4 -DVERSION="$(FVERSION)"  \
	-D REPOSITORY=$(REPOSITORY) \
	-D MIRROR=$(MIRROR) \
	-D TGZ=$(TGZ) \
	-D SIG=$(SIG) \
	-D URL=$(URL) \
	-D FWP=$(FWP) \
	-D NEWSCONTENTS="$$( cat $(FIMDIR)/NEWS  )" \
	-D DATE="`date +'%d/%m/%Y'`" \
	-D SIZE="$$( du -sb $(FIMDIR)/$(TGZ) | cut -f 1 )" \
	-D HOMEPAGE=$(HOMEPAGE) \
	-D MAILINGLIST=$(MAILINGLIST) \
	$(SRCDIR)/$(INDEXIN) \
	 > $(SITEDIR)/index.html 


#$(SITEDIR)/index.html: $(INDEXIN) $(FIMDIR)/VERSION

#site:	$(PAGES) $(SITEDIR)/index.html

site:	$(PAGES) $(SITEDIR)/index.html
	make tgz
	#sed 's/$$VERSION./'`cat $(FIMDIR)/VERSION`'/g' $(SRCDIR)/$(INDEXIN) > $(SITEDIR)/index.html
	mkdir -p $(SITEDIR)
	#cd $(FIMDIR) && ./autogen.sh && ./configure && make doc && make dist && cd -
	cd $(FIMDIR)/doc && make htmldocs
	for i in $(FIMHTMLDOCS) ; do cp $(FIMDIR)/doc/$$i  $(SITEDIR) ; done
	txt2html $(FIMDIR)/README > $(SITEDIR)/README.html
	txt2html $(FIMDIR)/NEWS > $(SITEDIR)/NEWS.html
	txt2html $(FIMDIR)/BUGS > $(SITEDIR)/BUGS.html
	rm -f $(SITEDIR)/index.html
	make $(SITEDIR)/index.html
	$(CP) $(FIMDIR)/README  $(SITEDIR)/README
	$(CP) $(FIMDIR)/BUGS  $(SITEDIR)/BUGS
	$(CP) $(FIMDIR)/NEWS  $(SITEDIR)/NEWS
	$(CP) $(FIMDIR)/doc/FIM.TXT  $(SITEDIR)/FIM.TXT
	$(CP) ../$(TGZ) $(SITEDIR)
	$(CP) ../$(SIG) $(SITEDIR)
	#webcheck -a $(SITEDIR)/index.html 
	ls -ltr $(SITEDIR) 

# lftp :~> help mirror
# Usage: mirror [OPTS] [remote [local]]
#
# Mirror specified remote directory to local directory
#  ...
#  -n, --only-newer       download only newer files (-c won't work)
#  -R, --reverse          reverse mirror (put files)
#  -v, --verbose[=N]      verbose operation
#      --log=FILE         write lftp commands being executed to FILE
# ...
# When using -R, the first directory is local and the second is remote.
# If the second directory is omitted, basename of first directory is used.
# If both directories are omitted, current local and remote directories are used.
 
up:
	echo "# generated automatically ftp session to $(FTPURL)" > $(FTPSCRIPT)
	echo "set ftp:ssl-force true" > $(FTPSCRIPT)
	echo "set ftp:passive-mode true" > $(FTPSCRIPT)
	echo "open $(FTPURL)" >> $(FTPSCRIPT)
	echo "ls" www/ >> $(FTPSCRIPT)
	echo "mirror -v --log=./ftp.log -R -n --exclude '.svn/*'  $(SITEDIR) $(FIMSITE)" >> $(FTPSCRIPT)
	echo "ls" www/ >> $(FTPSCRIPT)
	lftp -f $(FTPSCRIPT)
	cat ftp.log


