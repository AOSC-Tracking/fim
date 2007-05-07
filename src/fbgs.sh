#!/bin/bash
# $Id$

# This is a modified version of the original fbgs script,by Gerd Knorr
# and is capable of displaying pdf, eps, ps, and dvi files.
# Now enhanced for viewing .cbr and .cbz files :)!
#
#20060917	modified by dez
#20061229	added .cbr and .cbz support (on file extension basis, not detection!)
#20070307	added .tar,.tgz support
# tmp dir
DIR="${TMPDIR-/var/tmp}/fbps-$$"
mkdir -p $DIR	|| exit 1
trap "rm -rf $DIR" EXIT

declare -f 

export -f

# as advised in man which, for mysterious reasons
which ()
{
	( alias; declare -f ; ) | /usr/bin/which --tty-only --read-alias --read-functions --show-tilde --show-dot $@
}

#export -f which

function check_in_path()
{
	local cmd="$1";
	[[ -z "$cmd" ]] && return 0
	[[ -z $(which "$cmd" 2>/dev/null) ]] && return -1
	return 0
}

function info()  { echo '  [+]' "$@" 1>&2 ;  }
function einfo() { echo '  [-]' "$@" 1>&2 ;  }
function die() { einfo "ERROR : "$@ 1>&2 ; exit -1 ; }

check_in_path fim || die "no fim in \$PATH"

#FBI=/usr/bin/fbi
#FBI=~/fim/fim
#FBI=./fim
FBI=fim
#FBI='/usr/local/bin/fbi -a '
#FBI='fbi'

# parse options
fbiopts="-w"
gsopts="-q"
passwd=""
opt=1

# in this way odd filenames are kept intact, be WARNED
while test "$opt" = "1"
#set -- `getopt "" "$@"`
#while [ ! -z "$1" ]
do
	case "$1" in
#		-l)	gsopts="$gsopts -r100x100"
#			shift
#			;;
#		-xl)	gsopts="$gsopts -r120x120"
#			shift
#			;;
#		-xxl)	gsopts="$gsopts -r150x150"
#			shift
#			;;
#		-q | -a)
#			fbiopts="$fbiopts $1"
#			shift
#			;;
#		-d | -m | -t | -g | -f)
#			fbiopts="$fbiopts $1 $2"
#			shift; shift
#			;;
		-p)	password="$2"
			shift; shift
			;;
		-h)	help text
			exit 1
			;;
	        *)	
			fbiopts="$fbiopts $1";
			break;;
#		-*)	echo "unknown option: $1"
#			exit 1
#			;;
#		*)	opt=0
#			;;
	esac
#	shift
done
###############################################################################
##		HANDLING OF CBR AND CBZ ARCHIVES
###############################################################################
UNCBZ="unzip" ; ZOPT="-x '*/*'" # suits for Info-ZIP implementation
UNCBR="rar x"                   # suits for Alexander Roshal's RAR 3.51
UNTAR="tar xf"
UNTGZ="tar xzf"
#while [[ "$1" != "" ]]
#while [[ "$#" -gt "0" ]]
while test "$opt" = "1"
#while shift
do
	f="$1";
	shift;
	[[ "$f" =~ '^http://' ]] && { fn="$DIR"/"`basename $f`" && wget "$f" -O "$fn" && trap "rm $fn" EXIT ; f="$fn"; }
	[[ -z "$f" ]] && break ;
	#while [[ ! -f "$f" ]] ; do shift ; [[ "$#" -lt 0 ]] && break 2 ; done
###############################################################################
##		HANDLING OF DVI,PS AND PDF FILES
###############################################################################
#[[ "$f" =~ \\.dvi$ ]] && f="/tmp/$$.ps" && dvips -q "$f" -o "$f" &&  trap "rm $f ; rm -fRd $DIR" EXIT


[[ "$f" =~ \\.dvi$ ]] && fbiopts=" -P $fbiopts"
#[[ "$f" =~ \\.pdf$ ]] && fbiopts=" -o $fbiopts"
[[ "$f" =~ \\.pdf$ ]] && fbiopts=" -P $fbiopts"
[[ "$f" =~ \\.ps$  ]] && fbiopts=" -P $fbiopts"
# run ghostscript
#echo
#echo "### rendering pages, please wait ... ###"
#echo

if ( [[ "$f" =~ \\.ps$  ]] || [[ "$f" =~ \\.pdf$  ]] )
then 

BDIR=''
if check_in_path md5sum
then
	DIRS=$(echo "$f" | md5sum )
	DIRS=${DIRS// /}
	DIRS=${DIRS//-/}
	BDIR="$DIR/$DIRS"
else
	BDIR="$DIR/"$(basename "$f")
fi
mkdir -p $BDIR # 	|| exit 1
info "rendering to $BDIR"
#this is a workaround for gs's .. bug ..
[[ "$f" =~ "^\.\."  ]] && f="$PWD/$f"

gs	-dSAVER -dNOPAUSE -dBATCH			\
	-sDEVICE=png256 -r240x240 -sOutputFile=$BDIR/ps%03d.png	\
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
#pages=`ls $DIR/ 2>/dev/null | wc -l`
#if test "$pages" -eq "0"; then
#	echo
#	echo "Oops: ghostscript wrote no pages ($pages)?"
#	echo
#	exit 1
else

[[ "$f" =~ \\.cbr$    ]] && ( $UNCBR "$f"    "$DIR"  ) 
[[ "$f" =~ \\.rar$    ]] && ( $UNCBR "$f"    "$DIR"  ) 
[[ "$f" =~ \\.cbz$    ]] && ( $UNCBZ "$f" $ZOPT -d "$DIR" ) 
[[ "$f" =~ \\.zip$    ]] && ( $UNCBZ "$f" $ZOPT -d "$DIR" ) 
[[ "$f" =~ \\.tgz$    ]] && ( $UNTGZ "$f" -C "$DIR" ) 
[[ "$f" =~ \\.tar.gz$ ]] && ( $UNTGZ "$f" -C "$DIR" ) 
[[ "$f" =~ \\.tar$    ]] && ( $UNTAR "$f" -C "$DIR" ) 

# lone file handling..
#[[ "$?" == 0 ]] || echo "this script is not suited for file $f" 
fi
#fi
done
# show pages
#fbiopts=
#$FBI $fbiopts -P -- $DIR/* $DIR/*/* $DIR/*/*/* $DIR/*/*/*/* $DIR/*/*/*/*/* $DIR/*/*/*/*/*/* $DIR/*/*/*/*/*/*/*
find $DIR/  | $FBI $fbiopts  -- -
#cacaview $DIR/*
#$FBI $fbiopts -P  `find $DIR -iname '*.png' -or -iname '*.jpg' -or -iname '*.gif' -or -iname '*.jpeg' -or -iname '*.tiff' -or -iname '*.bmp'`
#$FBI $fbiopts -P -- $files
#fbi $fbiopts -P $DIR/ps*.tiff


