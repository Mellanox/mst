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
AM_CPPFLAGS = -I$(top_srcdir) -I$(srcdir)/../crd_lib -I$(top_srcdir)/include/mtcr_ul $(COMPILER_FPIC)
AM_CFLAGS = -MD -pipe -g -Wall -W $(COMPILER_FPIC)

bin_PROGRAMS = mstregdump

mstregdump_SOURCES = mstdump.c
mstregdump_LDADD = ../crd_lib/libcrdump.a ../../dev_mgt/libdev_mgt.la ../../reg_access/libreg_access.la ../../tools_layouts/libtools_layouts.la \
			../../${MTCR_CONF_DIR}/libmtcr_ul.a  -lm ${LDL}

mstregdump_CFLAGS = -DMSTDUMP_NAME=\"mstregdump\" -DDEV_EXAMPLE=\"0b:00.0\"

