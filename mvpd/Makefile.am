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
MTCR_DIR = $(top_srcdir)/${MTCR_CONF_DIR}
COMMON_DIR = $(top_srcdir)/common
LAYOUTS_DIR = $(top_srcdir)/tools_layouts
USER_DIR = $(top_srcdir)

AM_CPPFLAGS = -I. -I$(USER_DIR) -I$(srcdir) -I$(LAYOUTS_DIR) -I$(USER_DIR)/include/mtcr_ul
AM_CFLAGS = -W -Wall -g -MP -MD $(COMPILER_FPIC)

noinst_LTLIBRARIES = libmvpd.la

libmvpd_la_SOURCES = mvpd.c mvpd.h

libmvpd_la_DEPENDENCIES = $(MTCR_DIR)/libmtcr_ul.a
libmvpd_la_LIBADD = $(libmvpd_la_DEPENDENCIES)

RMVPD_SO = rmvpd.so

		  
LDADD= ../${MTCR_CONF_DIR}/libmtcr_ul.a ${LDL}


noinst_HEADERS = mvpd.h

${RMVPD_SO}: libmvpd.la
	$(CC) -g -Wall -pthread -shared ${CFLAGS} *.o -o ${RMVPD_SO} \
	 -L$(MTCR_DIR) -lmtcr_ul
	
CLEANFILES = ${RMVPD_SO}
