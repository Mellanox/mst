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

DIST_SUBDIRS = .

USER_DIR = ..
AM_CPPFLAGS = -I$(USER_DIR) -I$(USER_DIR)/common
AM_CXXFLAGS = -Wall -W -g -MP -MD -Werror -pipe $(COMPILER_FPIC)

lib_LTLIBRARIES = libadb_parser.la
libadb_parser_la_SOURCES = adb_parser.h adb_parser.cpp buf_ops.h buf_ops.cpp expr.h expr.cpp adb_expr.h adb_expr.cpp adb_db.h adb_db.cpp
