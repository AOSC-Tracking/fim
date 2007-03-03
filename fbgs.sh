#!/bin/bash
# $Id$

# This is a modified version of the original fbgs script,by Gerd Knorr
# and is capable of displaying pdf, eps, ps, and dvi files.
# Now enhanced for viewing .cbr and .cbz files :)!
#
#20060917	modified by dez
#20061229	added .cbr and .cbz support (on file extension basis, not detection!)

# tmp dir
DIR="${TMPDIR-/var/tmp}/fbps-$$"
mkdir -p $DIR	|| exit 1
trap "rm -rf $DIR" EXIT
FBI=/usr/bin/fbi
#FBI=~/fim/fim
FBI=./fim
#FBI='/usr/local/bin/fbi -a '
#FBI='fbi'

# parse options
fbiopts="-a"
gsopts="-q"
passwd=""
opt=1
while test "$opt" = "1"; do
	case "$1" in
		-l)	gsopts="$gsopts -r100x100"
			shift
			;;
		-xl)	gsopts="$gsopts -r120x120"
			shift
			;;
		-xxl)	gsopts="$gsopts -r150x150"
			shift
			;;
		-q | -a)
			fbiopts="$fbiopts $1"
			shift
			;;
		-d | -m | -t | -g | -f)
			fbiopts="$fbiopts $1 $2"
			shift; shift
			;;
		-p)	password="$2"
			shift; shift
			;;
		-h)	echo fixme: help text
			exit 1
			;;
		-*)	echo "unknown option: $1"
			exit 1
			;;
		*)	opt=0
			;;
	esac
done
###############################################################################
##		HANDLING OF CBR AND CBZ ARCHIVES
###############################################################################
UNCBZ="unzip" ; ZOPT="-x '*/*'" # suits for Info-ZIP implementation
UNCBR="rar x"                   # suits for Alexander Roshal's RAR 3.51

while [[ "$1" != "" ]];do
	f="$1";shift
[[ "$f" =~ \\.cbr$ ]] && ( $UNCBR "$f"    "$DIR"  ) 
[[ "$f" =~ \\.cbz$ ]] && ( $UNCBZ "$f" $ZOPT -d "$DIR" ) 
###############################################################################
##		HANDLING OF DVI,PS AND PDF FILES
###############################################################################
[[ "$f" =~ \\.dvi$ ]] && f="/tmp/$$.ps" && dvips -q "$f" -o "$f" &&  trap "rm $f ; rm -fRd $DIR" EXIT
#[[ "$f" =~ \\.dvi$ ]] && fbiopts=" -o $fbiopts"
#[[ "$f" =~ \\.pdf$ ]] && fbiopts=" -o $fbiopts"
#[[ "$f" =~ \\.ps$  ]] && fbiopts=" -o $fbiopts"
# run ghostscript
#echo
#echo "### rendering pages, please wait ... ###"
#echo
if ( [[ "$f" =~ \\.ps$  ]] || [[ "$f" =~ \\.pdf$  ]] ) ; then 
gs	-dSAVER -dNOPAUSE -dBATCH			\
	-sDEVICE=png256 -r200x200 -sOutputFile=$DIR/ps%03d.png	\
	$gsopts						\
	"$f"
#	-sPDFPassword="$password"			\
#-sDEVICE=tiffpack -sOutputFile=$DIR/ps%03d.tiff	\

# tell the user we are done :-)
#echo -ne "\\007"
#echo stuff : `ls -l $DIR`
#echo "contents of $DIR:"
#$FBI $fbiopts -P $DIR/ps*.png
#$FBI $fbiopts -P $DIR/ps*.png 2>/dev/null
#fbi $fbiopts -P $DIR/ps*.tiff
# sanity check
pages=`ls $DIR/ 2>/dev/null | wc -l`
if test "$pages" -eq "0"; then
	echo
	echo "Oops: ghostscript wrote no pages ($pages)?"
	echo
	exit 1
fi
fi
done
$FBI $fbiopts -P $DIR/*
# show pages
#fbi $fbiopts -P $DIR/ps*.tiff


