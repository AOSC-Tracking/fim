
# $Id$

#FWP=http://www.autistici.org/dezperado/fim
FWP=http://www.nongnu.org/fbi-improved/
VI_ABBR='<ABBR TITLE="Visual Interface">VI</ABBR>'
FIM_ABBR='<ABBR TITLE="Fbi IMproved">FIM</ABBR>'
GNU_ABBR='<ABBR TITLE="GNU is Not Unix">GNU</ABBR>'
SSH_ABBR='<ABBR TITLE="Secure SHell">SSH</ABBR>'
VIM_ABBR='<ABBR TITLE="VI IMproved">VIM</ABBR>'
ISO_ABBR='<ABBR TITLE="International Organization for Standardization">ISO</ABBR>'
JPEG_ABBR='<ABBR TITLE="Joint Photographic Experts Group">JPEG</ABBR>'
BASH_ABBR='<ABBR TITLE="Bourne Again SHell">BASH</ABBR>'
EXIF_ABBR='<ABBR TITLE="Exchangeable image file format">EXIF</ABBR>'
MUTT_ABBR='<ABBR TITLE="The Mutt Mail User Agent">Mutt</ABBR>'
ASCII_ABBR='<ABBR TITLE="American Standard Code for Information Interchange">ASCII</ABBR>'
EMACS_ABBR='<ABBR TITLE="Editing MACroS">Emacs</ABBR>'
INDEXIN=index.html.in
FIMDIR=..
FIMSITE=html-dezperado/fim
SITEDIR=./www
FTPSCRIPT=./script.lftp
FTPURL=ftp://dezperado@ftp5.autistici.org
SIZE="$$( du -sb $(FIMDIR)/$(TGZ) | cut -f 1 )"
SRCDIR=./
MIRROR=$$(cat $(FIMDIR)/README | grep "^Releases  "|sed s/^.*\ :.//g)
COPYRIGHT_NOTICE=$$(cat $(FIMDIR)/README | grep "^FIM.is.Copyright  "|sed s/^.*\ :.//g)
MAILINGLIST=$$(cat $(FIMDIR)/README | grep "^Mailing List "|sed s/^.*\ :.//g)
URL=$(MIRROR)/"fim-`cat $(FIMDIR)/VERSION`".tar.gz

TGZ="fim-`cat $(FIMDIR)/VERSION`".tar.gz
SIG=$(TGZ).sig
REPOSITORY=`cat $(FIMDIR)/README | grep "^Repository"|sed s/^.*\ :.//g`
FVERSION=`cat $(FIMDIR)/VERSION`
HOMEPAGE=$$(cat $(FIMDIR)/README | grep "^Homepage  "|sed s/^.*\ :.//g)
SVNNPAGE=$$(cat $(FIMDIR)/README | grep "^Savannah.Page"|sed s/^.*Page:.//g)
FBIPAGE=https://www.kraxel.org/blog/linux/fbida/
VIMPAGE=https://www.vim.org/
PACKAGE=fim
FIMHTMLDOCS= FIM.html fimrc.man.html fim.man.html fimgs.man.html
PAGES= $(SITEDIR)/index.html $(FIMHTMLDOCS)
CP=cp -p
N2H=he --escape --use-named-refs # in package "node-he"

help:
	@echo
	@echo HELP : 'make pages | make site | make up | make all | make cp'
	@echo        'make pages # generates the HTML files'
	@echo        'make site  # generates also the tarball'
	@echo        'make up    # copies the HTML files to their local dir'
	@echo        'make cp    # copies the tarball to the local mirror dir'
	@echo        'make all   # all of the above'
	@echo

#tgz: $(SITEDIR)/$(TGZ)
# tgz:
#	cd $(FIMDIR) && make doc && make signed-tgzdist && cd - # this directory cares just for the www pages
	
all:	site up cp

$(FIMHTMLDOCS):
	cd $(FIMDIR)/doc && make $(FIMHTMLDOCS)

$(SITEDIR)/index.html: $(INDEXIN) $(FIMDIR)/NEWS
	m4 -DVERSION="$(FVERSION)"  \
	-D REPOSITORY=$(REPOSITORY) \
	-D MIRROR=$(MIRROR) \
	-D COPYRIGHT_NOTICE=$(COPYRIGHT_NOTICE) \
	-D TGZ=$(TGZ) \
	-D SIG=$(SIG) \
	-D URL=$(URL) \
	-D FWP=$(FWP) \
	-D FDP=$(FDP) \
	-D VI_ABBR=$(VI_ABBR) \
	-D FIM_ABBR=$(FIM_ABBR) \
	-D GNU_ABBR=$(GNU_ABBR) \
	-D SSH_ABBR=$(SSH_ABBR) \
	-D VIM_ABBR=$(VIM_ABBR) \
	-D ISO_ABBR=$(ISO_ABBR) \
	-D BASH_ABBR=$(BASH_ABBR) \
	-D JPEG_ABBR=$(JPEG_ABBR) \
	-D EXIF_ABBR=$(EXIF_ABBR) \
	-D MUTT_ABBR=$(MUTT_ABBR) \
	-D ASCII_ABBR=$(ASCII_ABBR) \
	-D EMACS_ABBR=$(EMACS_ABBR) \
	-D NEWSCONTENTS="$$( grep -v '^#' $(FIMDIR)/NEWS | $(N2H) )" \
	-D THANKSCONTENTS="$$( cat $(FIMDIR)/THANKS )" \
	-D MANFIMCONTENTS="$$( cat $(FIMDIR)/doc/fim.man.html | $(FIMDIR)/scripts/maintenance/htmlbody.awk | sed 's/name=\"/name=\"fim.man.html_/g;s/href=\"#/href=\"#fim.man.html_/g' )" \
	-D MANFIMRCCONTENTS="$$( cat $(FIMDIR)/doc/fimrc.man.html | $(FIMDIR)/scripts/maintenance/htmlbody.awk | sed 's/name=\"/name=\"fimrc.man.html_/g;s/href=\"#/href=\"#fimrc.man.html_/g')" \
	-D MANFIMGSCONTENTS="$$( cat $(FIMDIR)/doc/fimgs.man.html | $(FIMDIR)/scripts/maintenance/htmlbody.awk | sed 's/name=\"/name=\"fimgs.man.html_/g;s/href=\"#/href=\"#fimgs.man.html_/g')" \
	-D DATE="`date +'%d/%m/%Y'`" \
	-D SIZE=$(SIZE) \
	-D HOMEPAGE=$(HOMEPAGE) \
	-D SVNNPAGE=$(SVNNPAGE) \
	-D FBIPAGE=$(FBIPAGE) \
	-D VIMPAGE=$(VIMPAGE) \
	-D MAILINGLIST=$(MAILINGLIST) \
	$(SRCDIR)/$(INDEXIN) \
	 > $(SITEDIR)/index.html 


#$(SITEDIR)/index.html: $(INDEXIN) $(FIMDIR)/VERSION

pages:	$(PAGES) $(SITEDIR)/index.html

site:	pages
	test -n "$(SIZE)"
	#make tgz # this directory cares just for the www pages
	#sed 's/$$VERSION./'`cat $(FIMDIR)/VERSION`'/g' $(SRCDIR)/$(INDEXIN) > $(SITEDIR)/index.html
	mkdir -p $(SITEDIR)
	#cd $(FIMDIR) && ./autogen.sh && ./configure && make doc && make dist && cd -
	#cd $(FIMDIR)/doc && make htmldocs
	cd $(FIMDIR) && make dox
	for i in $(FIMHTMLDOCS) ; do cp -v $(FIMDIR)/doc/$$i  $(SITEDIR) ; done
	txt2html $(FIMDIR)/README > $(SITEDIR)/README.html
	grep -v '^#' $(FIMDIR)/NEWS | txt2html -  > $(SITEDIR)/NEWS.html
	txt2html $(FIMDIR)/BUGS > $(SITEDIR)/BUGS.html
	rm -f $(SITEDIR)/index.html
	make $(SITEDIR)/index.html
	$(CP) $(FIMDIR)/README  $(SITEDIR)/README
	$(CP) $(FIMDIR)/BUGS  $(SITEDIR)/BUGS
	grep -v '^#' $(FIMDIR)/NEWS > $(SITEDIR)/NEWS
	$(CP) $(FIMDIR)/doc/FIM.TXT  $(SITEDIR)/FIM.TXT
	#$(CP) ../$(TGZ) $(SITEDIR)
	#$(CP) ../$(SIG) $(SITEDIR)
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
 
up_old:
	echo "# generated automatically ftp session to $(FTPURL)" > $(FTPSCRIPT)
	echo "set ftp:ssl-force true" > $(FTPSCRIPT)
	echo "set ftp:passive-mode true" > $(FTPSCRIPT)
	echo "open $(FTPURL)" >> $(FTPSCRIPT)
	echo "ls" www/ >> $(FTPSCRIPT)
	echo "ls" $(FIMSITE) >> $(FTPSCRIPT)
	echo "mirror -v --log=./ftp.log -R -n --exclude '.svn/*'  $(SITEDIR) $(FIMSITE)" >> $(FTPSCRIPT)
	echo "ls" www/ >> $(FTPSCRIPT)
	echo "ls" $(FIMSITE) >> $(FTPSCRIPT)
	lftp -f $(FTPSCRIPT)
	cat ftp.log

FIMCVSDIR=~/src/fim_cvs_www/fbi-improved/

up:
	cd $(SITEDIR) && cp -v * $(FIMCVSDIR)
	ls -l $(FIMCVSDIR)

cp:
	cp -v ../$(TGZ) ../$(SIG) $(FIMCVSDIR)/
	ls -l $(FIMCVSDIR)/
#	cp -v ../$(TGZ) ../$(SIG) $(FIMCVSDIR)/../mirror
#	ls -l $(FIMCVSDIR)/../mirror

