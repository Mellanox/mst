#--
# Copyright (C) Jan 2013 Mellanox Technologies Ltd. All rights reserved.
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

PYTHON_WRAPPER_SCRIPT = ../common/python_wrapper

AM_CPPFLAGS= -I$(top_srcdir)/include/mtcr_ul -I$(top_srcdir)/common -I$(top_srcdir)/mvpd $(COMPILER_FPIC)
AM_CFLAGS= -I$(top_srcdir)/include/mtcr_ul -I$(top_srcdir)/common -I$(top_srcdir)/mvpd $(COMPILER_FPIC)

LDADD= $(top_srcdir)/${MTCR_CONF_DIR}/libmtcr_ul.a ${LDL}

bin_PROGRAMS = mstmread \
					mstmwrite \
					mstmcra \
					mstmtserver \
					mstvpd \
					mstcongestion
					
bin_SCRIPTS = mstfwreset

mstmread_SOURCES = mread.c

mstmwrite_SOURCES = mwrite.c

mstvpd_SOURCES = vpd.c
mstvpd_LDADD = $(top_srcdir)/mvpd/libmvpd.la $(top_srcdir)/${MTCR_CONF_DIR}/libmtcr_ul.a ${LDL}

mstcongestion_SOURCES = congestion.cpp
mstcongestion_LDADD = $(top_srcdir)/cmdif/libcmdif.la \
                      $(top_srcdir)/dev_mgt/libdev_mgt.la \
                      $(top_srcdir)/reg_access/libreg_access.la \
                      $(top_srcdir)/tools_layouts/libtools_layouts.la \
                      $(top_srcdir)/cmdparser/libcmdparser.a \
                      $(top_srcdir)/${MTCR_CONF_DIR}/libmtcr_ul.a ${LDL}

mstmcra_SOURCES  = mcra.c

mstmtserver_SOURCES = mtserver.c tcp.c tcp.h
mstmtserver_CFLAGS = -DMST_UL

SUBDIRS = mlxfwresetlib
MSTFWRESET_PYTHON_WRAPPER=mstfwreset
${MSTFWRESET_PYTHON_WRAPPER}: $(PYTHON_WRAPPER_SCRIPT)
	cp $(PYTHON_WRAPPER_SCRIPT) $@
mstfwresetlibdir=$(libdir)/mstflint/python_tools/$(MSTFWRESET_PYTHON_WRAPPER)

mlxpcilibdir=$(libdir)/mstflint/python_tools/mlxpci
mlxpcilib_DATA = binary_file.py mlxpci_lib.py

mstfwresetlib_SCRIPTS = $(MSTFWRESET_PYTHON_WRAPPER).py

