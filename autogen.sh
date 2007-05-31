#!/bin/sh
# $Id$

# This file is still not complete.

# this should create configure.scan
autoscan || { echo "no autoscan ?" ; exit 1 ; }

[[ -f "configure.scan" ]] || { echo "no configure.scan ?" ; exit 1 ; }

# this should move configure.scan to configure.in
mv "configure.scan" "configure.in" || { echo "problems moving configure.scan to configure.in" ; exit 1 ; }

# we produce a configure script
autoconf "configure.in" > configure || { echo "no autoscan ?" ; exit 1 ; }

[[ -f "configure" ]] || { echo "no configure ?" ; exit 1 ; }

# configure should be able to generate Makefile's
chmod -c +x ./configure || { echo "no touchable configure ?" ; exit 1 ; }
