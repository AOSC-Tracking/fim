#!/bin/bash
# $Id$

# This is a modified version of the original fbgs script,by Gerd Knorr
# and is capable of displaying pdf, eps, ps, and dvi files.
# Now enhanced for viewing .cbr and .cbz files :)!
#
# Note : it won't probably work on bash, version < 3.0 because of regular expressions.

# this script wants a temporary directory to stores rendered/wget'ed files
# this directory is named after the process ID
DIR="${TMPDIR-/var/tmp}/fbps-$$"

# we must have write permissions in that directory
mkdir -p $DIR	|| exit 1

# we want that directory clean after this script execution, no matter what happens
trap "rm -rf $DIR" EXIT

# useless for now :)
# declare -f 

# useless, too
# export -f

# as advised in man which, for mysterious reasons ( C compatible )
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

# our favourite framebuffer viewing program :)
FBI=fim

check_in_path $FBI  || die "no fim in \$PATH"
check_in_path echo  || die "no echo in \$PATH"
check_in_path find  || die "no find in \$PATH"
check_in_path wget  || die "no wget in \$PATH"
check_in_path gs    || die "no gs (ghostscript) in \$PATH"
check_in_path tar   || die "no tar in \$PATH"
check_in_path basename   || die "no basename in \$PATH"
check_in_path md5sum   || die "no md5sum in \$PATH"


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
		# explicit option passing
		-c)	fbiopts="$fbiopts -c $2"
			shift; shift
			;;
		-p)	password="$2"
			shift; shift
			;;
		-h)	help text
			exit 1
			;;
	        *)	
			[[ "$1" =~ '^-.*' ]] && fbiopts="$fbiopts $1";
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
UNBZ="tar xjf"
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
#[[ -f "$f" ]] || { echo "an option!" ;  fbiopts="$fbiopts $f" ; break ; } # not a file, but an option


[[ "$f" =~ \\.dvi$ ]] && fbiopts=" -P $fbiopts"
[[ "$f" =~ \\.pdf$ ]] && fbiopts=" -P $fbiopts"
[[ "$f" =~ \\.ps$  ]] && fbiopts=" -P $fbiopts"
[[ "$f" =~ \\.eps$  ]] && fbiopts=" -P $fbiopts"

if ( [[ "$f" =~ \\.ps$  ]] || [[ "$f" =~ \\.pdf$  ]] ) || ( [[ "$f" =~ \\.eps$  ]] || [[ "$f" =~ \\.dvi$  ]] )
then 

BDIR=''
if check_in_path md5sum
then
	DIRS=$(echo "$f" | md5sum )
	DIRS=${DIRS// /}
	DIRS=${DIRS//-/}
	BDIR="$DIR/$DIRS"
	# this means that giving duplicate imput files will overwrite the original render directory...
else
	BDIR="$DIR/"$(basename "$f")
fi
mkdir -p $BDIR  	|| die "failed mkdir $BDIR"
info "rendering $f to $BDIR.."

#this is a workaround for gs's .. bug ..

[[ "$f" =~ "^\.\."  ]] && f="$PWD/$f"

# note : we cannot render gs renderable documents with more than 1000 pages...
gs	-dSAVER -dNOPAUSE -dBATCH			\
	-sDEVICE=png256 -r240x240 -sOutputFile=$BDIR/ps%03d.png	\
	$gsopts						\
	"$f" || die "ghostscript failed rendering!"
# still unused options :
#	-sPDFPassword="$password"			\
#	-sDEVICE=tiffpack				\
#	-sOutputFile=$DIR/ps%03d.tiff

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
elif ( ( [[ "$f" =~ \\.cbr$  ]] || [[ "$f" =~ \\.cbz$  ]] ) || ( [[ "$f" =~ \\.rar$  ]] || [[ "$f" =~ \\.zip$  ]] ) || ( [[ "$f" =~ \\.tgz$  ]] || [[ "$f" =~ \\.tar.gz$  ]] ) || ( [[ "$f" =~ \\.tar$  ]] || [[ "$f" =~ \\.tar.bz2$  ]] ) )  && ! [[ "$f" =~ '^http://' ]]
then
# in the case of an archive ...
# an ideal fimgs script would unpack recursively in search for interesting files..

[[ "$f" =~ \\.cbr$    ]] && ( $UNCBR "$f"    "$DIR"  ) 
[[ "$f" =~ \\.rar$    ]] && ( $UNCBR "$f"    "$DIR"  ) 
[[ "$f" =~ \\.cbz$    ]] && ( $UNCBZ "$f" $ZOPT -d "$DIR" ) 
[[ "$f" =~ \\.zip$    ]] && ( $UNCBZ "$f" $ZOPT -d "$DIR" ) 
[[ "$f" =~ \\.tgz$    ]] && ( $UNTGZ "$f" -C "$DIR" ) 
[[ "$f" =~ \\.tar.gz$ ]] && ( $UNTGZ "$f" -C "$DIR" ) 
[[ "$f" =~ \\.tar$    ]] && ( $UNTAR "$f" -C "$DIR" ) 
[[ "$f" =~ \\.tar.bz2$ ]] && ( $UNBZ "$f" -C "$DIR" ) 

# ... but this fimgs script is lazy and it gets satisfied with a bare decompress !

# lone file handling..
#[[ "$?" == 0 ]] || echo "this script is not suited for file $f" 
else
	[[ -f "$DIR/`basename $f`" ]] || cp -- "$f" "$DIR" || die "problems copying $f to $DIR"
fi
#fi
done
# show pages
#fbiopts=
#$FBI $fbiopts -P -- $DIR/* $DIR/*/* $DIR/*/*/* $DIR/*/*/*/* $DIR/*/*/*/*/* $DIR/*/*/*/*/*/* $DIR/*/*/*/*/*/*/*
#echo "options are $fbiopts"

# there should be some filter to avoid feed garbage to fim ... 
find $DIR/ -type f -iname '*.jpg' -or -iname '*.jpeg' -or -iname '*.png' -or -iname '*.gif' -or -iname '*.bmp' -or -iname '*.tiff' | $FBI $fbiopts  -- -

# when no framebuffer device is available, we could invoke another console viewer, couldn't we ?
# cacaview $DIR/*

# the pld syntax
# $FBI $fbiopts -P  `find $DIR -iname '*.png' -or -iname '*.jpg' -or -iname '*.gif' -or -iname '*.jpeg' -or -iname '*.tiff' -or -iname '*.bmp'`

# or the older ..
#$FBI $fbiopts -P -- $files

# or the ancient one
#fbi $fbiopts -P $DIR/ps*.tiff


