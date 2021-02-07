# Copyright (c) 2004-2010 Mellanox Technologies LTD. All rights reserved.
# 
# This software is available to you under a choice of one of two
# licenses.  You may choose to be licensed under the terms of the GNU
# General Public License (GPL) Version 2, available from the file
# COPYING in the main directory of this source tree, or the
# OpenIB.org BSD license below:
#
#     Redistribution and use in source and binary forms, with or
#     without modification, are permitted provided that the following
#     conditions are met:
#
#      - Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#
#      - Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials
#        provided with the distribution.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#--

dnl Process this file with autoconf to produce a configure script.

AC_INIT(mstflint, 4.16.0, eranj@mellanox.co.il)

AC_DEFINE_UNQUOTED([PROJECT], ["mstflint"], [Define the project name.])
AC_SUBST([PROJECT])

AC_DEFINE_UNQUOTED([VERSION], ["4.16.0"], [Define the project version.])
AC_SUBST([VERSION])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([README])
AM_INIT_AUTOMAKE([foreign])

dnl Checks for programs
AC_PROG_CC
AC_PROG_CXX
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_LIBTOOL
AC_CONFIG_HEADERS( config.h )

OS=$(uname -s)

MTCR_CONF_DIR=""
LDL=""
default_en_inband=""

LDFLAGS="${LDFLAGS} -L/usr/lib64 -L/usr/local/lib -L/usr/local/lib64"
CXXFLAGS="${CXXFLAGS} -isystem /usr/local/include" 
if test "x$OS" = "xFreeBSD"; then
    AC_MSG_NOTICE(FreeBSD Build)
    MTCR_CONF_DIR="mtcr_freebsd"
    default_en_inband="no"
else
    MTCR_CONF_DIR="mtcr_ul"
    LDL="-ldl"
    default_en_inband="yes"
fi
AC_SUBST(MTCR_CONF_DIR)
AC_SUBST(LDL)
AC_SUBST(default_en_inband)

dnl Checks for headers
AC_CHECK_HEADER(termios.h,[CXXFLAGS="${CXXFLAGS} -DHAVE_TERMIOS_H"])
TOOLS_CRYPTO=""
MAD_IFC=""
FW_MGR_TOOLS=""
XZ_UTILS_DIR=""
ADABE_TOOLS=""
ADABE_DBS=""
ADABE_DBS_EXTRA_DIST=""

COMPILER_FPIC="-fPIC"
AC_SUBST(COMPILER_FPIC)

AC_MSG_CHECKING(--enable-dc argument)
AC_ARG_ENABLE(dc,
		    [  --disable-dc            Disable mstflint "dc" command. Eliminates zlib dependency],
		    [enable_dc=$enableval],
		    [enable_dc="yes"])
AC_MSG_RESULT($enable_dc)
if test "$enable_dc" = "yes"; then
  AC_CHECK_HEADER(zlib.h,,AC_MSG_ERROR([cannot find zlib.h . Use --disable-dc to remove this dependaency]))
  AC_CHECK_LIB(z, uncompress,, AC_MSG_ERROR([cannot find zlib uncompress() function. Use --disable-dc to remove this dependaency]))
fi

# FWMANAGER
AC_MSG_CHECKING(--enable-fw-mgr argument)
AC_ARG_ENABLE(fw-mgr,
             [  --enable-fw-mgr        Enable compiling mstfwmanager tool and features],
             [enable_fw_mgr="yes"],
             [enable_fw_mgr="no"])
AC_MSG_RESULT($enable_fw_mgr)
if test "x$enable_fw_mgr" = "xyes"; then
    AC_CHECK_HEADER(curl/curl.h,,AC_MSG_ERROR([cannot find curl/curl.h . this header is needed for compiling fw manager tool]))
    AC_CHECK_HEADER(zlib.h,,AC_MSG_ERROR([cannot find zlib.h . this header is needed for compiling fw manager tool]))
    AC_CHECK_LIB(z, uncompress,, AC_MSG_ERROR([cannot find zlib uncompress() function.]))
    AC_CHECK_HEADER(lzma.h,,AC_MSG_ERROR([Cannot find lzma.h.]))
    AC_CHECK_LIB(boost_regex, regcompA,, AC_MSG_ERROR([cannot find boost_regex regcompA() function. Try re-installing the library...]))
    CURL_INC_DIR=/usr/include/curl/
fi

# Adabe
AC_MSG_CHECKING(--enable-adb-generic-tools argument)
AC_ARG_ENABLE(adb-generic-tools,
             [  --enable-adb-generic-tools        Enable compiling the following tools which depends on ADABE: mstreg and mstlink],
             [enable_adb_generic_tools="yes"],
             [enable_adb_generic_tools="no"])
AC_MSG_RESULT($enable_adb_generic_tools)
if test "x$enable_adb_generic_tools" = "xyes"; then
    AC_LANG_PUSH([C++])
        AC_CHECK_LIB([expat], XML_ParserCreate_MM,
                [AC_CHECK_HEADERS(expat.h, have_expat=true, have_expat=false)],
                have_expat=false)
        if ! $have_expat; then
        AC_MSG_ERROR([Expat library is missing, please install expat xml parser to continue: expat-devel.]
                [http://expat.sourceforge.net/])
        fi
        AC_CHECK_HEADERS([boost/foreach.hpp], [],
            [AC_MSG_ERROR([Boost libraries are missing, please install boost to continue.])])
    AC_LANG_POP([C++])
fi

AC_SUBST(CURL_INC_DIR)

AM_CONDITIONAL(ENABLE_FWMGR, [test "x$enable_fw_mgr" = "xyes" ])
AC_SUBST(ENABLE_FWMGR)

AC_SUBST(ENABLE_FWMGR)
# XML2
AC_MSG_CHECKING(--enable-xml2 argument)
AC_ARG_ENABLE(xml2,
            [  --enable-xml2           Enable mstflint libxml2 dependent features],
            [enable_xml2="yes"],
            [enable_xml2="no"])
AC_MSG_RESULT($enable_xml2)
if test [ "x$enable_xml2" = "xyes" ] || [ test "x$enable_fw_mgr" = "xyes" ]; then
    # XML2 checks
    AC_LANG_PUSH([C++])
        if test "x$OS" = "xFreeBSD"; then
            CXXFLAGS="${CXXFLAGS} -isystem /usr/local/include/libxml2"
        else
            CXXFLAGS="${CXXFLAGS} -isystem /usr/include/libxml2"
        fi
        AC_CHECK_HEADER([libxml2/libxml/xpath.h],[],[AC_MSG_ERROR([libxml2: xpath.h is not found in the system PATH. make sure libxml2 headers are installed.])])
        AC_CHECK_LIB(xml2, xmlDocDumpFormatMemoryEnc,, AC_MSG_ERROR([cannot find xml2 xmlDocDumpFormatMemoryEnc() function. Try re-installing the library...]))
    AC_LANG_POP([C++])
fi

AM_CONDITIONAL(DISABLE_XML2, [test  "x$enable_xml2" = "xno"])
AC_SUBST(DISABLE_XML2)

AM_CONDITIONAL(ENABLE_DC, [test  "x$enable_dc" = "xyes"])
AC_SUBST(ENABLE_DC)

AC_MSG_CHECKING(--enable-inband argument)
AC_ARG_ENABLE(inband,
		    [  --disable-inband        Disable inabnd access. Prevents FW updtes for Mellanox SwitchX and ConnectIB devices. Eliminates infiniband/mad.h dependency],
		    [enable_inband=$enableval],
		    [enable_inband=$default_en_inband])        
AC_MSG_RESULT($enable_inband)
if test "x$enable_inband" = "xyes"; then
  AC_CHECK_HEADER(infiniband/mad.h,,AC_MSG_ERROR([cannot find infiniband/mad.h . Use --disable-inband to remove this dependaency]))
  MAD_IFC="mad_ifc"
else
  CXXFLAGS="$CXXFLAGS -DNO_INBAND"
  CFLAGS="$CFLAGS -DNO_INBAND"
fi
AC_SUBST(MAD_IFC)

AM_CONDITIONAL(ENABLE_INBAND, [test  "x$enable_inband" = "xyes"])

AC_MSG_CHECKING(--enable-cs argument)
AC_ARG_ENABLE(cs,
            [  --enable-cs             Enable mstflint "checksum" command, dependant of openssl library],
            [enable_cs="yes"],
            [enable_cs="no"])
AC_MSG_RESULT($enable_cs)
if test "x$enable_cs" = "xyes"; then
  AC_CHECK_HEADER(openssl/md5.h,,AC_MSG_ERROR([cannot find openssl/md5.h . remove --enable-cs to remove this dependaency or install openssl]))
  TOOLS_CRYPTO="tools_crypto"
fi

AC_MSG_CHECKING(openssl argument)
AC_ARG_ENABLE(openssl,
            [  --disable-openssl       Disable all openssl features, dependant of openssl library],
            [enable_openssl=$enableval],
            [enable_openssl="yes"])
AC_MSG_RESULT($enable_openssl)
if test "x$enable_openssl" = "xyes"; then
  AC_CHECK_HEADER(openssl/sha.h,,AC_MSG_ERROR([cannot find openssl/sha.h . remove --enable-openssl to remove this dependaency or install openssl]))
  AC_CHECK_HEADER(openssl/rsa.h,,AC_MSG_ERROR([cannot find openssl/rsa.h . remove --enable-openssl to remove this dependaency or install openssl]))
  AC_CHECK_HEADER(openssl/pem.h,,AC_MSG_ERROR([cannot find openssl/pem.h . remove --enable-openssl to remove this dependaency or install openssl]))
  AC_CHECK_HEADER(openssl/bio.h,,AC_MSG_ERROR([cannot find openssl/bio.h . remove --enable-openssl to remove this dependaency or install openssl]))
  AC_CHECK_HEADER(openssl/md5.h,,AC_MSG_ERROR([cannot find openssl/md5.h . remove --enable-openssl to remove this dependaency or install openssl]))
  TOOLS_CRYPTO="tools_crypto mlxsign_lib"
fi

AC_MSG_CHECKING(--enable-all-static argument)
AC_ARG_ENABLE([all_static],
    AS_HELP_STRING([--enable-all-static], [Enable creating none dynamic executables]),
    [enable_all_static="yes"],
    [enable_all_static="no"])
AC_MSG_RESULT($enable_all_static)

AS_IF([test "x$enable_all_static" = "xyes"], [
  if test "x$enable_dc" = "xyes"; then
    AC_MSG_ERROR([cannot enable all static with enable dc . add --disable-dc to remove dependaency with dynamic zlib])
  fi
  if test "x$enable_openssl" = "xyes"; then
    AC_MSG_ERROR([cannot enable all static with enable openssl . add --disable-openssl to remove dependaency with dynamic openssl])
  fi
  if test "x$enable_inband" = "xyes"; then
    AC_MSG_ERROR([cannot enable all static with enable inband . add --disable-inband to remove dependaency with dynamic inband])
  fi
  LDFLAGS="$LDFLAGS -all-static"
  CFLAGS="$CFLAGS -DSQLITE_OMIT_LOAD_EXTENSION=0 -DIBVSMAD_DLOPEN"
])

AC_SUBST(TOOLS_CRYPTO)
AM_CONDITIONAL(ENABLE_OPENSSL, [test "x$enable_openssl" = "xyes" ])
AM_CONDITIONAL(ENABLE_CS, [test  "x$enable_cs" = "xyes" || test "x$enable_openssl" = "xyes" ])

AC_ARG_ENABLE([static_libstdcpp],
    AS_HELP_STRING([--enable-static-libstdcpp], [Enable link static to libstdc++]))

AS_IF([test "x$enable_static_libstdcpp" = "xyes" -o "x$enable_all_static" = "xyes" ], [
  CXXFLAGS="$CXXFLAGS -static-libstdc++ -static-libgcc"
])

AC_ARG_ENABLE([dynamic_ld],
    AS_HELP_STRING([--enable-dynamic-ld], [Enable dynamic link to ld64.so]))

AS_IF([test "x$enable_dynamic_ld" = "xyes"], [
  LDFLAGS="$LDFLAGS -Wl,--dynamic-linker=/lib64/ld64.so.2"
])

AC_CHECK_HEADERS(iniparser.h, INIPARSER_SYSTEM_AVAILABLE="yes", INIPARSER_SYSTEM_AVAILABLE="no")

if test "$INIPARSER_SYSTEM_AVAILABLE" = "yes"; then
  AC_SEARCH_LIBS([iniparser_load], [iniparser], [INIPARSER_SYSTEM_AVAILABLE="yes"],[
    INIPARSER_SYSTEM_AVAILABLE="no"
  ])
fi

if test "$INIPARSER_SYSTEM_AVAILABLE" = "no"; then
    INIPARSER_CFLAGS='-I$(top_srcdir)/ext_libs/iniParser'
    AC_SUBST(INIPARSER_CFLAGS)
    INIPARSER_LIBS='$(top_builddir)/ext_libs/iniParser/libiniparser.a'
    AC_SUBST(INIPARSER_LIBS)
fi

AM_CONDITIONAL([USE_LOCAL_INIPARSER], [test "$INIPARSER_SYSTEM_AVAILABLE" = no])
AS_IF([test "x$INIPARSER_SYSTEM_AVAILABLE" = "xyes" ], [
    CXXFLAGS="$CXXFLAGS -DHAVE_INI_PARSER"
])


PKG_CHECK_MODULES([JSON], [jsoncpp], [JSON_SYSTEM_AVAILABLE="yes"],[
    JSON_SYSTEM_AVAILABLE="no"
    JSON_CFLAGS='-I$(top_srcdir)/ext_libs/json'
    AC_SUBST(JSON_CFLAGS)
    JSON_LIBS='$(top_builddir)/ext_libs/json/libjson.a'
    AC_SUBST(JSON_LIBS)
])
AM_CONDITIONAL([USE_LOCAL_JSON], [test "$JSON_SYSTEM_AVAILABLE" = no])

PKG_CHECK_MODULES([MUPARSER], [muparser], [MUPARSER_SYSTEM_AVAILABLE="yes"],[
    MUPARSER_SYSTEM_AVAILABLE="no"
    MUPARSER_CFLAGS='-I$(top_srcdir)/ext_libs/muparser'
    AC_SUBST(MUPARSER_CFLAGS)
    MUPARSER_LIBS='$(top_builddir)/ext_libs/muparser/libmuparser.a'
    AC_SUBST(MUPARSER_LIBS)
])
AM_CONDITIONAL([USE_LOCAL_MUPARSER], [test "$MUPARSER_SYSTEM_AVAILABLE" = no])

PKG_CHECK_MODULES([SQLITE], [sqlite3], [SQLITE_SYSTEM_AVAILABLE="yes"],[
    SQLITE_SYSTEM_AVAILABLE="no"
    SQLITE_CFLAGS='-I$(top_srcdir)/ext_libs/sqlite'
    AC_SUBST(SQLITE_CFLAGS)
    SQLITE_LIBS='$(top_builddir)/ext_libs/sqlite/libsqlite3.a'
    AC_SUBST(SQLITE_LIBS)
])
AM_CONDITIONAL([USE_LOCAL_SQLITE], [test "$SQLITE_SYSTEM_AVAILABLE" = no])

CFLAGS="$CFLAGS -DMST_UL"
CXXFLAGS="$CXXFLAGS -DMST_UL"

AM_CFLAGS="-Wall"
AM_CXXFLAGS="-Wall"

AC_CONFIG_FILES( mstflint.spec )

# We save and restore CFLAGS, after adding Wall and Werror
# because AX_CHECK_COMPILE_FLAG doesn't work properly without it

# C related ignores
OLD_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall -Werror"
AX_CHECK_COMPILE_FLAG([-Wno-format-overflow], [CFLAGS="$CFLAGS -Wno-format-overflow"], [], [])
AX_CHECK_COMPILE_FLAG([-Wno-implicit-fallthrough], [CFLAGS="$CFLAGS -Wno-implicit-fallthrough"], [], [])
AX_CHECK_COMPILE_FLAG([-Wno-cast-function-type], [CFLAGS="$CFLAGS -Wno-cast-function-type"], [], [])
CFLAGS="$OLD_CFLAGS"

# C++ related ignores
OLD_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -Wall -Werror"
AC_LANG_PUSH([C++])
AX_CHECK_COMPILE_FLAG([-Wno-implicit-fallthrough], [CXXFLAGS="$CXXFLAGS -Wno-implicit-fallthrough"], [], [])
AX_CHECK_COMPILE_FLAG([-Wno-format-overflow], [CXXFLAGS="$CXXFLAGS -Wno-format-overflow"], [], [])
AX_CHECK_COMPILE_FLAG([-Wno-cast-function-type], [CXXFLAGS="$CXXFLAGS -Wno-cast-function-type"], [], [])
AX_CHECK_COMPILE_FLAG([-Wno-class-memaccess], [CXXFLAGS="$CXXFLAGS -Wno-class-memaccess"], [], [])
AC_LANG_POP([C++])
CXXFLAGS="$OLD_CXXFLAGS"

TOOLS_BUILD_TIME=`date  +"%b %d %Y\, %H:%M:%S"`
if test "x$SOURCE_DATE_EPOCH" != "x"; then
    TOOLS_BUILD_TIME=`LC_ALL=C date -u -d @$SOURCE_DATE_EPOCH +"%b %d %Y\, %H:%M:%S"`
fi
AC_SUBST(TOOLS_BUILD_TIME)

AC_ARG_VAR(MSTFLINT_VERSION_STR, The MSTFLINT version)
if test x"$MSTFLINT_VERSION_STR" == x; then
    MSTFLINT_VERSION_STR="$PACKAGE_STRING"
fi

AC_SUBST(MSTFLINT_VERSION_STR)
AC_DEFINE([MSTFLINT_VERSION_STR], [], [The MSTFLINT version])
AC_DEFINE_UNQUOTED(MSTFLINT_VERSION_STR, "$MSTFLINT_VERSION_STR")


AC_ARG_VAR(TOOLS_GIT_SHA, The MSTFLINT tree git sha hash version)
if test x"$TOOLS_GIT_SHA" == x; then
    TOOLS_GIT_SHA="N/A"
fi
AC_SUBST(TOOLS_GIT_SHA)
AC_DEFINE([TOOLS_GIT_SHA], [], [The MSTFLINT tree git sha hash version])
AC_DEFINE_UNQUOTED(TOOLS_GIT_SHA, "$TOOLS_GIT_SHA")

AS_IF([test "x$enable_cs" = "xyes" || test "x$enable_openssl" = "xyes"], [
        AC_CONFIG_FILES(tools_crypto/Makefile)
    ])
    
AS_IF([test "x$enable_openssl" = "xyes"], [
        AC_CONFIG_FILES(mlxsign_lib/Makefile)
    ])

if [ test "x$enable_openssl" = "xno" ] && \
    [ test "x$enable_fw_mgr" = "xyes" ] ; then
    AC_MSG_ERROR([--enable-fw-mgr cannot be used with --disabled-openssl.])
fi

AS_IF([test "x$enable_fw_mgr" = "xyes"], [
        AC_CONFIG_FILES(mlxfwupdate/Makefile)
        AC_CONFIG_FILES(ext_libs/iniParser/Makefile)
        AC_CONFIG_FILES(mlxarchive/Makefile)
        FW_MGR_TOOLS="mlxfwupdate mlxarchive"
    ])
    
AC_SUBST(FW_MGR_TOOLS)

AS_IF([test "x$enable_adb_generic_tools" = "xyes"], [
        AC_CONFIG_FILES(adb_parser/Makefile)
        AC_CONFIG_FILES(tools_layouts/adb/Makefile)
        AC_CONFIG_FILES(tools_layouts/adb/prm/Makefile)
        AC_CONFIG_FILES(tools_layouts/adb/prm/hca/Makefile)
        AC_CONFIG_FILES(tools_layouts/adb/prm/hca/ext/Makefile)
        AC_CONFIG_FILES(tools_layouts/adb/prm/switch/Makefile)
        AC_CONFIG_FILES(tools_layouts/adb/prm/switch/ext/Makefile)
        AC_CONFIG_FILES(mlxreg/Makefile)
        AC_CONFIG_FILES(mlxlink/Makefile)
        AC_CONFIG_FILES(mlxlink/modules/Makefile)
        AC_CONFIG_FILES(mlxlink/modules/printutil/Makefile)
        ADABE_TOOLS="adb_parser mlxreg mlxlink"
        ADABE_DBS="adb"
        ADABE_DBS_EXTRA_DIST="adb/prm/hca/ext/*.adb adb/prm/switch/ext/*.adb"
    ])

AS_IF([test "x$enable_fw_mgr" = "xyes" || test "x$enable_adb_generic_tools" = "xyes"], [
        AC_CONFIG_FILES(xz_utils/Makefile)
        XZ_UTILS_DIR="xz_utils"
    ])

AC_SUBST(ADABE_TOOLS)
AC_SUBST(ADABE_DBS)
AC_SUBST(ADABE_DBS_EXTRA_DIST)
AC_SUBST(XZ_UTILS_DIR)

if test "x$OS" = "xFreeBSD"; then
    AC_MSG_NOTICE(FreeBSD MTCR)
    AC_CONFIG_FILES(mtcr_freebsd/Makefile)
else
    AC_MSG_NOTICE(Linux MTCR)
    AC_CONFIG_FILES(mtcr_ul/Makefile)
fi


AC_OUTPUT( Makefile common/Makefile mft_utils/Makefile mft_utils/hsmclient/Makefile mtcr_py/Makefile ext_libs/Makefile ext_libs/sqlite/Makefile ext_libs/muparser/Makefile ext_libs/json/Makefile ext_libs/minixz/Makefile dev_mgt/Makefile \
        tools_layouts/Makefile reg_access/Makefile cmdif/Makefile libmfa/Makefile pldmlib/Makefile tools_res_mgmt/Makefile mlxconfig/Makefile mlxconfig/mlxconfig_dbs/Makefile mflash/Makefile mlxfwops/Makefile mlxfwops/lib/Makefile cmdparser/Makefile \
        flint/Makefile small_utils/Makefile small_utils/mlxfwresetlib/Makefile mstdump/Makefile mstdump/crd_lib/Makefile mstdump/crd_main/Makefile mstdump/mstdump_dbs/Makefile mvpd/Makefile \
        fw_comps_mgr/Makefile mad_ifc/Makefile tracers/Makefile tracers/fwtrace/Makefile resourcedump/Makefile resourceparse/Makefile )

