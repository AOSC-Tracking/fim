#!/bin/sh
# $Id$

# This file is still not complete.

# this should create configure.scan

aclocal  || { echo "no aclocal  ?" ; exit 1 ; }

# we produce a configure script
autoconf || { echo "no autoconf ?" ; exit 1 ; }

# we produce a brand new Makefile
automake --add-missing || { echo "no automake ?" ; exit 1 ; }

