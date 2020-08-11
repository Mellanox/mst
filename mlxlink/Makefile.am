#--
# 
# Copyright (C) Jan 2019 Mellanox Technologies Ltd. All rights reserved.
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

SUBDIRS = modules

USER_DIR = $(top_srcdir)
COMMON_DIR = $(top_srcdir)/common
MTCR_DIR = $(top_srcdir)/${MTCR_CONF_DIR}
MTCR_INC_DIR = $(top_srcdir)/include/mtcr_ul
UTILS_DIR = $(top_srcdir)/mft_utils
MLXREG_DIR = $(top_srcdir)/mlxreg
LAYOUTS_DIR = $(top_srcdir)/tools_layouts
MLXLINK_MODULES_DIR = $(top_srcdir)/mlxlink

AM_CXXFLAGS = -Wall -W -DMST_UL -g -MP -MD -pipe -Werror

bin_PROGRAMS = mstlink

mstlink_CXXFLAGS = -I. -I$(top_srcdir) -I$(MTCR_DIR) -I$(MFT_EXT_LIBS_INC_DIR) -I$(UTILS_DIR) -I$(MLXREG_DIR) -I$(MLXLINK_MODULES_DIR) -I$(MTCR_INC_DIR) $(JSON_CFLAGS) -DEXTERNAL
mstlink_SOURCES = mlxlink_main.cpp

mstlink_DEPENDENCIES = modules/libmodules_lib.a \
                       modules/printutil/libprint_util_lib.a \
                      $(USER_DIR)/mlxreg/libmstreg_lib.a \
                      $(USER_DIR)/cmdparser/libcmdparser.a \
                      $(UTILS_DIR)/libmftutils.a \
                      $(MTCR_DIR)/libmtcr_ul.a \
                      $(USER_DIR)/adb_parser/libadb_parser.a \
                      $(USER_DIR)/cmdif/libcmdif.a \
                      $(USER_DIR)/dev_mgt/libdev_mgt.a \
                      $(USER_DIR)/reg_access/libreg_access.a \
                      $(LAYOUTS_DIR)/libtools_layouts.a \
                      $(USER_DIR)/xz_utils/libxz_utils.a \
                      $(USER_DIR)/ext_libs/minixz/libminixz.a \
                      -lboost_regex -lboost_filesystem -lboost_system \
                      -llzma $(LIBSTD_CPP) ${LDL} -lexpat \
                      $(JSON_LIBS)

mstlink_LDADD = $(mstlink_DEPENDENCIES)

