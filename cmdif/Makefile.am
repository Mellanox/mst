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

# Makefile.am -- Process this file with automake to produce Makefile.in

USER_DIR = $(top_srcdir)
MTCR_DIR = $(USER_DIR)/include/mtcr_ul
TOOLS_LAYOUTS_DIR = $(USER_DIR)/tools_layouts
AM_CPPFLAGS = -I. -I../common -I../tools_layouts  -I${MTCR_DIR} -I.. -I$(USER_DIR)/${MTCR_CONF_DIR}
CCMDIF_SO = ccmdif.so

AM_CFLAGS = -W -Wall -g -MP -MD $(COMPILER_FPIC) -DCMDIF_EXPORTS
CMDIF_VERSION = 1
lib_LTLIBRARIES = libcmdif.la

cmdifincludedir = $(includedir)/mstflint/cmdif/

cmdifinclude_HEADERS = icmd_cif_common.h icmd_cif_open.h

libcmdif_la_SOURCES = tools_cif.c tools_cif.h icmd_cif_common.c icmd_cif_common.h icmd_cif_open.c icmd_cif_open.h

cmdif_pylibdir = $(libdir)/mstflint/python_tools/
cmdif_pylib_DATA = ${CCMDIF_SO} cmdif.py
${CCMDIF_SO}: libcmdif.la
	$(CC) -g -Wall -pthread -shared ${CFLAGS} *.o -o ${CCMDIF_SO} \
	-L$(USER_DIR)/tools_layouts/.libs -ltools_layouts -L$(USER_DIR)/${MTCR_CONF_DIR} -lmtcr_ul

CLEANFILES = ${CCMDIF_SO}
