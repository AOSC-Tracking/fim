dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.54])
AC_INIT([src/fim.h])

AC_MSG_NOTICE([
        Welcome to the Fim configure script!
	This script will create the right Makefile for building Fim on your system.
	But first, it will check if your system has the right configuratin for Fim to build.

	There are options to this script, please run :
	  ./configure --help=short
	for a complete list of these options.
])


AM_CONFIG_HEADER([config.h])
#AC_CONFIG_SRCDIR(pinentry/pinentry.h)

#
#AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
AC_GNU_SOURCE
#AC_MANTAINER_MODE
#AC_CANONICAL_HOST

#missing_dir=`cd $ac_aux_dir && pwd`
#AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
#AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
#AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
#AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
#AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)



AC_PROG_CC
AC_PROG_CPP
AC_PROG_YACC
AC_PROG_LEX

AC_PROG_INSTALL
AC_PROG_RANLIB

AC_REVISION([$Revision : ... $])

AC_PROG_CXX
#AC_PROG_CXXCPP
AC_CHECK_TOOL(AR, ar, ar)
fim_cv_version=`cat VERSION`
#AC_LANG_PUSH([C++])
AM_INIT_AUTOMAKE(fim, $fim_cv_version)
#AM_INIT_AUTOMAKE(fim, 1.0)
AC_SUBST([CONFIG_STATUS_DEPENDENCIES], ['$(top_srcdir)/VERSION'])
CPPFLAGS=''


AC_SUBST(FIM_LIB_OBJECTS)
AC_SUBST(VERSION)
#AC_LANG_POP([C++])
#FIM_LIB_OBJECTS="$FIM_LIB_OBJECTS string.o"

fim="fim"
AC_MSG_CHECKING([for prefix])
if test x$prefix = xNONE; then
  fim_cv_prefix=$ac_default_prefix
else
  fim_cv_prefix=$prefix
fi
AC_MSG_RESULT($fim_cv_prefix)

AC_ISC_POSIX
AM_C_PROTOTYPES
if test "x$U" != "x"; then
  AC_MSG_ERROR(Compiler not ANSI compliant)
fi
#AC_PROG_CPP
#AC_CONFIG_HEADERS(..)
AC_PROG_MAKE_SET

AC_C_INLINE
AC_C_CONST

AC_SYS_LARGEFILE
AC_FUNC_FSEEKO
AC_CHECK_SIZEOF(off_t)



have_default_bindings=yes
have_default_mapping=yes
have_default_config=yes

AC_ARG_ENABLE(default_bindings, AC_HELP_STRING([--disable-default_bindings], [Builtin default minimal key binding (unfinished)]),
[       if test x$enableval = xyes; then
		have_default_bindings=yes
	else
                have_default_bindings=no
        fi
])

AC_ARG_ENABLE(default_mapping, AC_HELP_STRING([--disable-default_mapping], [Builtin default minimal key binding (unfinished)]),
[       if test x$enableval = xyes; then
                have_default_mapping=yes
	else
		have_default_mapping=no
        fi
])

AC_ARG_ENABLE(default_config, AC_HELP_STRING([--disable-default_config], [Builtin default minimal key binding (unfinished)]),
[       if test x$enableval = xyes; then
                have_default_config=yes
	else
		have_default_config=no
        fi
])

	EXTRADEFS=
#		DEFS="$DEFS -D FIM_DEFAULT_CONFIG"
#		EXTRADEFS="$EXTRADEFS -D FIM_DEFAULT_CONFIG"
	if test x$have_default_bindings != xno; then
#		DEFS="$DEFS -D FIM_DEFAULT_CONFIG"
		EXTRADEFS="$EXTRADEFS -D FIM_DEFAULT_CONFIG"
	fi
	if test x$have_default_mapping != xno; then
#		DEFS="$DEFS -D FIM_DEFAULT_KEY_CONFIG"
		EXTRADEFS="$EXTRADEFS -D FIM_DEFAULT_KEY_CONFIG"
	fi
	if test x$have_default_config != xno; then
#		DEFS="$DEFS -D FIM_DEFAULT_CONFIGURATION"
		EXTRADEFS="$EXTRADEFS -D FIM_DEFAULT_CONFIGURATION"
	fi

#CPPFLAGS=$DEPS
AC_SUBST(LIBPNG_CFLAGS)
AC_SUBST(FIM_CFLAGS)
AC_SUBST(LIBPNG_LIBS)


AC_ARG_ENABLE(system, AC_HELP_STRING([--disable-system], [Disable system() and pipe() calls.]),
[       if test x$enableval = xno ; then
                fim_system_calls=no
        fi
])


AC_HEADER_STDC

AC_CHECK_HEADERS(stdarg.h sys/ioctl.h ioctl.h sysexits.h)
AC_CHECK_HEADERS(sys/time.h sys/resource.h)
AC_CHECK_HEADERS(unix.h)

AC_CHECK_FUNCS(setrlimit getsid)

AC_TYPE_SIGNAL



AC_ARG_WITH(regex, AC_HELP_STRING([--with-regex], [Use the GNU regex library]),
        [fim_cv_regex=yes],
        [AC_CHECK_FUNCS(regcomp, fim_cv_regex=no, fim_cv_regex=yes)])

if test $fim_cv_regex = no ; then
AC_CACHE_CHECK([whether your system's regexp library is completely broken],
        [fim_cv_regex_broken],
        AC_TRY_RUN([
#include <unistd.h>
#include <regex.h>
main() { regex_t blah ; regmatch_t p; p.rm_eo = p.rm_eo; return regcomp(&blah, "foo.*bar", REG_NOSUB) || regexec (&blah, "foobar", 0, NULL, 0); }],
        fim_cv_regex_broken=no, fim_cv_regex_broken=yes, fim_cv_regex_broken=yes))
        if test $fim_cv_regex_broken = yes ; then
                echo "Using the included GNU regex instead." >&AC_FD_MSG
                fim_cv_regex=yes
        fi
fi

if test $fim_cv_regex = yes; then
        AC_DEFINE(USE_GNU_REGEX,1,[ Define if you want to use the included regex.c. ])
        AC_LIBOBJ(regex)
fi



AC_MSG_CHECKING(where to put the documentation)
AC_ARG_WITH(docdir, AC_HELP_STRING([--with-docdir=PATH], [Specify where to put the documentation]),
        [fim_cv_docdir=$withval],
        [fim_cv_docdir="${prefix}/doc/fim-${fim_cv_version}"])

AC_MSG_RESULT($fim_cv_docdir)

docdir=$fim_cv_docdir
AC_SUBST(docdir)
AC_SUBST(fim_cv_version)

dnl ============= gif file handling

AC_ARG_ENABLE(gif, AC_HELP_STRING([--disable-gif], [Disable gif support.]),
[       if test x$enableval = xno ; then
		fim_handle_gif=no
        fi
])
if test x$fim_handle_gif != xno ; then
  fim_handle_gif=yes
  have_gif=yes
  if test x$with_libgif != xno && test -z "$LIBGIF"; then
    AC_CHECK_LIB(gif,DGifOpenFileHandle ,
      have_gif=yes,
      have_gif=no
      AC_MSG_WARN(*** GIF loader will not be built (GIF library not found) ***))
    if test "$have_gif" = yes; then
      AC_MSG_CHECKING([for gif.h])
      AC_TRY_CPP(
[#include <stdio.h>
#undef PACKAGE
#undef VERSION
#undef HAVE_STDLIB_H
#include <gif_lib.h>],
        have_gif=yes,
        have_gif=no)
      AC_MSG_RESULT($have_gif)
      if test "$have_gif" = yes; then
        LIBGIF='-lgif'
      else
          AC_MSG_WARN(*** GIF loader will not be built (GIF header file not found) ***)
      fi
    fi
  fi

  if test x$with_libgif != xno ; then
  FIM_LIB_OBJECTS="$FIM_LIB_OBJECTS gif.o"
  FIMLIBS="$FIMLIBS $LIBGIF"
  fi

#  if test x$with_libgif != xno && test -z "$LIBGIF"; then
  #
#  fi
else
  fim_handle_gif=no
fi
AC_SUBST(LIBGIF)
AM_CONDITIONAL(ENABLE_GIF, test x$have_gif = xyes)


dnl ============= tiff file handling

AC_ARG_ENABLE(tiff, AC_HELP_STRING([--disable-tiff], [Disable tiff support.]),
[       if test x$enableval = xno ; then
		fim_handle_tiff=no
        fi
])
if test x$fim_handle_tiff != xno ; then
  fim_handle_tiff=yes
  have_tiff=yes
  if test x$with_libtiff != xno && test -z "$LIBTIFF"; then
    AC_CHECK_LIB(tiff,TIFFOpen ,
      have_tiff=yes,
      have_tiff=no
      AC_MSG_WARN(*** TIFF loader will not be built (TIFF library not found) ***))
    if test x"$have_tiff" = xyes; then
      AC_MSG_CHECKING([for tiff.h])
      AC_TRY_CPP(
[#include <stdio.h>
#undef PACKAGE
#undef VERSION
#undef HAVE_STDLIB_H
#include <tiff.h>],
        have_tiff=yes,
        have_tiff=no)
      AC_MSG_RESULT($have_tiff)
      if test x"$have_tiff" = xyes; then
        LIBTIFF='-ltiff'
      else
          AC_MSG_WARN(*** TIFF loader will not be built (TIFF header file not found) ***)
      fi
    fi
  fi

  if test x$with_libtiff != xno ; then
  FIM_LIB_OBJECTS="$FIM_LIB_OBJECTS tiff.o"
  FIMLIBS="$FIMLIBS $LIBTIFF"
  fi

#  if test x$with_libtiff != xno && test -z "$LIBTIFF"; then
  #
#  fi
else
  fim_handle_tiff=no
fi
AC_SUBST(LIBTIFF)
AM_CONDITIONAL(ENABLE_TIFF, test x$have_tiff = xyes)

dnl ============= GNU readline : essential

    AC_CHECK_LIB(readline, rl_replace_line,
      have_readline=yes,have_readline=no
      AC_MSG_ERROR(*** GNU readline library headers not found !  please install them ***))

FIMLIBS="$FIMLIBS -lreadline"

dnl ============= GNU gpm, inofficial yet

#    AC_CHECK_LIB(gpm,Gpm_Open,
#      ,true,
#      AC_MSG_ERROR(*** gpm lib not found ... no problem ... ***))
#FIMLIBS="$FIMLIBS -lgpm"

dnl ============= jpeg file handling

#AC_ARG_WITH(jpeg, [  --disable-jpeg ], [ disable jpeg file support])
AC_ARG_ENABLE(jpeg, AC_HELP_STRING([--disable-jpeg], [Disable jpeg support.]),
[       if test x$enableval = xno ; then
		fim_handle_jpeg=no
        fi
])
if test x$fim_handle_jpeg != xno ; then
  fim_handle_jpeg=yes
  have_jpeg=yes
  if test x$with_libjpeg != xno && test -z "$LIBJPEG"; then
    AC_CHECK_LIB(jpeg, jpeg_destroy_decompress,
      have_jpeg=yes,
      have_jpeg=no
      AC_MSG_WARN(*** JPEG loader will not be built (JPEG library not found) ***))
    if test x"$have_jpeg" = xyes; then
      AC_MSG_CHECKING([for jpeglib.h])
      AC_TRY_CPP(
[#include <stdio.h>
#undef PACKAGE
#undef VERSION
#undef HAVE_STDLIB_H
#include <jpeglib.h>],
        have_jpeg=yes,
        have_jpeg=no)
      AC_MSG_RESULT($have_jpeg)
      if test x"$have_jpeg" = xyes; then
        LIBJPEG='-ljpeg'
      else
          AC_MSG_WARN(*** JPEG loader will not be built (JPEG header file not found) ***)
      fi
    fi
  fi

  if test x$with_libjpeg != xno ; then
  FIM_LIB_OBJECTS="$FIM_LIB_OBJECTS jpeg.o"
  FIMLIBS="$FIMLIBS $LIBJPEG"
  fi

#  if test x$with_libjpeg != xno && test -z "$LIBJPEG"; then
#     AC_MSG_ERROR([
#     Checks for JPEG loader failed. You can build without it by passing
#    --disable-jpeg to ./configure. ])
#  fi
else
  fim_handle_jpeg=no
fi
AC_SUBST(LIBJPEG)
AM_CONDITIONAL(ENABLE_JPEG, test x$have_jpeg = xyes)

dnl ================================================================================

## png file handling

AC_ARG_WITH(png-prefix, AC_HELP_STRING([--with-png-prefix=PFX], [prefix where libpng is installed (optional)]),
libpng_config_prefix="$withval", libpng_config_prefix="")

fim_handle_png=yes

AC_ARG_ENABLE(png, AC_HELP_STRING([--disable-png], [Disable png support.]),
[       if test x$enableval = xno ; then
		fim_handle_png=no
        fi
])

if test x$fim_handle_png = xyes; then
	# support for custom library configurator path
        if test x$libpng_config_prefix != x; then
                LIBPNG_CONFIG="$libpng_config_prefix/bin/libpng-config"
        else
		# default path configurator
                AC_PATH_PROG(LIBPNG_CONFIG, libpng-config, no)
        fi
        if test "x$LIBPNG_CONFIG" = "xno"; then
                AC_MSG_ERROR([LIBPNG not found])
        else
                LIBPNG_CFLAGS=`$LIBPNG_CONFIG --cflags`
                LIBPNG_LIBS=`$LIBPNG_CONFIG --libs`
		# finish this !
                FIM_LIB_OBJECTS="$FIM_LIB_OBJECTS png.o"
                FIMLIBS="$FIMLIBS -lpng"
                AC_DEFINE(FIM_WITH_LIBPNG, 1, [Defined, if libpng support is enabled])
        fi
fi

#AM_CONDITIONAL(BUILD_PNG, test x$fim_handle_png = xyes)


#AC_DEFINE([FIM_DEFAULT_KEY_CONFIG]   ,1, [Defined, if GPGME support is enabled])
#AC_DEFINE([FIM_DEFAULT_CONFIG], 1, [Defined, if GPGME support is enabled])

#AC_DEFINE([FIM_DEFAULT_CONFIGURATION], 1, [Defined, if GPGME support is enabled])

#AC_MSG_CHECKING(whether struct dirent defines d_ino)

dnl ==================

fim_cv_nofimrc=no
AC_ARG_ENABLE(fimrc, AC_HELP_STRING([--enable-fimrc], [Fim will by default load the ~.fimrc file.]),
[if test x$enableval = xyes; then
        fim_cv_fimrc=yes
fi])

if test x$fim_cv_fimrc = xyes; then
	have_fimrc=yes
else
	EXTRADEFS="$EXTRADEFS -D FIM_NOFIMRC"
fi


dnl ==================

fim_cv_warnings=no
AC_ARG_ENABLE(screen, AC_HELP_STRING([--enable-screen], [Fim will work under screen (EXPERIMENTAL!)]),
[if test x$enableval = yes; then
        fim_cv_screen=yes
fi])

if test x$fim_cv_screen = xyes; then
        # boz patch is on
	have_boz_patch=yes
	EXTRADEFS="$EXTRADEFS -D FIM_BOZ_PATCH"
fi

dnl ==================

fim_cv_warnings=no
AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug], [Turns on debug options (recommended, for debugging)]),
[if test x$enableval = xyes; then
        fim_cv_debug=yes
fi])

dnl ==================

fim_cv_warnings=no
AC_ARG_ENABLE(warnings, AC_HELP_STRING([--enable-warnings], [Turns on compiler warnings (recommended, for debugging)]),
[if test x$enableval = xyes; then
        fim_cv_warnings=yes
fi])

if test x$GCC = xyes ; then
	if test x$fim_cv_debug = xyes; then
        # CFLAGS are resetted
        CFLAGS="-g"
	fi
fi

if test x$GCC = xyes ; then
	if test x$fim_cv_warnings = xyes ; then
        CFLAGS="-Wall -pedantic $CFLAGS"
	fi
fi

dnl ==================

# This code still awaits for integration ...
AC_ARG_WITH(cflags, AC_HELP_STRING([--with-cflags=CFLAGS], 
	[Specify a custom (no spaces!!) single CFLAGS append value.  e.g.: -pg
	 You can't call this argument multiple times.
	 In case you want completely reset CFLAGS use :
	 CFLAGS='<all that you want>' ./configure ..
	]),
        [if test x$withval != xyes; then
                AC_DEFINE_UNQUOTED(CFLAGS, "$withval",
                 [custom CFLAGS])
		FIM_CFLAGS="$CFLAGS $withval"
        fi],
        [AC_DEFINE_UNQUOTED(EXECSHELL, "/bin/sh")])


dnl ==================

# This code still awaits for integration ...
AC_ARG_WITH(exec-shell, AC_HELP_STRING([--with-exec-shell=SHELL], [Specify alternate shell (ONLY if /bin/sh is broken)]),
        [if test x$withval != xyes; then
                AC_DEFINE_UNQUOTED(EXECSHELL, "$withval",
                 [program to use for shell commands])
         else
                AC_DEFINE_UNQUOTED(EXECSHELL, "/bin/sh")
        fi],
        [AC_DEFINE_UNQUOTED(EXECSHELL, "/bin/sh")])

dnl ==================

AC_SUBST(LIBIMAP)
AC_SUBST(LIBIMAPDEPS)

AC_SUBST(FIMLIBS)
AC_SUBST(DEFS)
AC_SUBST(EXTRADEFS)
AC_SUBST(FIM_LIB_OBJECTS)

#AC_CONFIG_FILES([Makefile],[src/Makefile])

AC_OUTPUT(Makefile)
AC_OUTPUT(src/Makefile)
AC_OUTPUT(doc/Makefile)
AC_OUTPUT(distros/Makefile)
#AC_OUTPUT

AC_MSG_NOTICE([
	Fim v${VERSION} has been configured as follows:

	default minimal key mapping         : ${have_default_mapping:-no}
	default minimal key bindings        : ${have_default_bindings:-no}
	default configuration file builtin  : ${have_default_config:-no}
        compile time warnings               : ${fim_cv_warnings:-no} ( ${FIM_CFLAGS} )
        system,popen calls wrappers enabled : ${fim_system_calls:-yes}
	png  file support                   : ${fim_handle_png:-no}
	jpeg file support                   : ${fim_handle_jpeg}
	tiff file support                   : ${fim_handle_tiff}
	gif  file support                   : ${fim_handle_gif}
	patch for running under screen on?  : ${have_boz_patch:-no}
	~/.fimrc file loading on startup    : ${have_fimrc:-no}

	If you are not satisfied with this configuration, please run :

	  ./configure --help=short

	for a complete list of configure options specific to Fim.

	If you are satisfied, run 'make' to build Fim and its documentation.
	Otherwise, 'make fim' to build only Fim.

	To install the program and its documentation, run 'make install' if you are root,
	or run 'su -c '"make install"' if you are not root.

	p.s.: documentation is found in the doc/ directory !
	Please see doc/FIM.TXT, doc/FIM.html, doc/fim.man.

	Enjoy! Please see the BUGS file for bug reports, and the documentation anyway!
])


