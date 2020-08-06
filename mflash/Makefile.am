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
AM_CPPFLAGS = -I. -I$(top_srcdir)/include/mtcr_ul -I$(top_srcdir)/common -I$(top_srcdir)/tools_layouts -I$(top_srcdir)/reg_access \
          -I$(top_srcdir)/cmdif -I$(top_srcdir)/tools_res_mgmt $(COMPILER_FPIC)

AM_CFLAGS = -MD -pipe -Wall -W -DMST_UL -g ${MFLASH_INBAND_FLAG} $(COMPILER_FPIC)

noinst_LTLIBRARIES = libmflash.la

libmflash_la_SOURCES = mflash.c mflash.h\
			mflash_dev_capability.c mflash_dev_capability.h \
			mflash_pack_layer.c mflash_pack_layer.h mflash_access_layer.c mflash_access_layer.h\
			mflash_types.h mlash_common_structs.h

noinst_HEADERS = mflash.h mflash_types.h mflash_common_structs.h
