#!/bin/sh
# $Id$

# This creates buildbot slaves for fim to be used in the gcc compile farm.
# To set a fim bot, it is necessary to set up a master bot server name and port,
# so have the slave bot name and password.
# beware: this script should be used on a single-user system.

#host=
#port=9999
#passwd=

fail() { echo "[!] usage: host=some_hostname port=some_port passwd=some_passwd $0"; exit -1 ; }

[ -z "$host"   ] && fail
[ -z "$port"   ] && fail
[ -z "$passwd" ] && fail

# see http://gcc.gnu.org/wiki/CompileFarm

D=".fsffrance.org"
for h in gcc12.fsffrance.org gcc41 gcc40 gcc54
# gcc30.fsffrance.org gcc31.fsffrance.org
#for h in gcc12.fsffrance.org gcc30.fsffrance.org gcc31.fsffrance.org gcc41 gcc40 gcc54
#for h in gcc12$D gcc40$D

do
	un=fimbot-`echo $h | sed 's/\..*$//g'`
	bd=fim-build-slave-$un
	m=$host:$port
	echo $un
	up=$passwd
	e=''
	bb=/usr/bin/buildbot
	kb="$bb stop $bd"
	rd="rm -fR $bd"
	cl="if test -d $bd ; then $kb ; $rd ; fi"
	cs="$bb create-slave $bd $m $un $up"
	sb="$bb start $bd"
	wi="echo $USER@`hostname` ($h) > $bd/info/admin"
	wi="$wi && uname -a > $bd/info/host"
	$e ssh $h "$cl ; $cs && $wi &&$sb "
done

