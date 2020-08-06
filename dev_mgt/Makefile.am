#--
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
USER_DIR = ..
AM_CPPFLAGS = -I$(srcdir) -I$(top_srcdir) -I$(top_srcdir)/common  -I$(top_srcdir)/include/mtcr_ul

AM_CFLAGS = -W -Wall -g -MP -MD -Wswitch-enum $(COMPILER_FPIC) -DMTCR_EXPORT
lib_LTLIBRARIES = libdev_mgt.la

libdev_mgt_la_SOURCES = \
						tools_dev_types.c \
						tools_dev_types.h \
						$(top_srcdir)/common/bit_slice.h       \
						$(top_srcdir)/common/compatibility.h   \
						$(top_srcdir)/common/tools_version.h

dev_mgt_pylibdir = $(libdir)/mstflint/python_tools/

dev_mgt_pylib_DATA = c_dev_mgt.so dev_mgt.py
dist_dev_mgt_pylib_DATA = dev_mgt.py

c_dev_mgt.so: libdev_mgt.la
	$(CC) -g -Wall -pthread -shared ${CFLAGS} tools_dev_types.o -o c_dev_mgt.so \
	-L$(USER_DIR)/reg_access/.libs -lreg_access \
	-L$(USER_DIR)/tools_layouts/.libs -ltools_layouts \
	-L$(USER_DIR)/${MTCR_CONF_DIR} -lmtcr_ul

CLEANFILES = c_dev_mgt.so
