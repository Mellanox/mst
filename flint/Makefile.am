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
#        disclaimer in the documentation and/or othint2_oem.er materials
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
MTCR_DIR    = $(top_srcdir)/${MTCR_CONF_DIR}
MTCR_INC_DIR = $(top_srcdir)/include/mtcr_ul
MFLASH_DIR  = $(top_srcdir)/mflash
COMMON_DIR  = $(top_srcdir)/common
LAYOUTS_DIR = $(top_srcdir)/tools_layouts
MFT_UTILS_DIR = $(top_srcdir)/mft_utils
CMDIF_DIR   = $(top_srcdir)/cmdif
MAD_IFC_DIR   = $(top_srcdir)/mad_ifc

AM_CPPFLAGS = -I$(top_srcdir) -I$(srcdir) -I$(MTCR_DIR) -I$(MFLASH_DIR) -I$(COMMON_DIR) \
           -I$(LAYOUTS_DIR) -I$(MFT_UTILS_DIR) -I$(MTCR_INC_DIR)

mstflint_CXXFLAGS = -Wall -W -g -MP -MD -pipe -DEXTERNAL $(COMPILER_FPIC)
bin_PROGRAMS = mstflint

mstflint_SOURCES = flint.cpp flint.h subcommands.cpp subcommands.h\
					flint_params.cpp flint_params.h cmd_line_parser.cpp err_msgs.h

mstflint_LDADD =  ../mlxfwops/lib/libmlxfwops.a  \
                  ../cmdparser/libcmdparser.a    \
                  ../mflash/libmflash.la         \
		  ../tools_res_mgmt/libtools_res_mgmt.a \
                  $(CMDIF_DIR)/libcmdif.la        \
                  ../reg_access/libreg_access.la  \
                  ../dev_mgt/libdev_mgt.la        \
		  ../${MTCR_CONF_DIR}/libmtcr_ul.a        \
                  ../tools_layouts/libtools_layouts.la \
                  ../fw_comps_mgr/libfw_comps_mgr.a \
		  ../mft_utils/libmftutils.a\
		  ${LDL}


if ENABLE_DC
mstflint_LDADD += -lz
else
mstflint_CXXFLAGS += -DNO_ZLIB
endif

if ENABLE_CS
mstflint_LDADD += $(top_srcdir)/tools_crypto/libtools_crypto.a -lcrypto
else
endif

if ENABLE_INBAND
mstflint_LDADD += $(MAD_IFC_DIR)/libmad_ifc.a
else
endif

if ENABLE_OPENSSL
mstflint_LDADD += $(top_srcdir)/mlxsign_lib/libmlxsign.a -lcrypto 
else
endif

if ENABLE_FWMGR
mstflint_LDADD += $(top_srcdir)/libmfa/libmfa.la $(top_srcdir)/ext_libs/minixz/libminixz.a $(top_srcdir)/mlxarchive/libmstarchive.la $(top_srcdir)/xz_utils/libxz_utils.la -llzma -lm
else
mstflint_CXXFLAGS += -DNO_MSTARCHIVE
endif

#get mst device examples and tool name from makefile
mstflint_CXXFLAGS+= -DFLINT_NAME=\"mstflint\" -DFLINT_DISPLAY_NAME=\"MstFlint\"
mstflint_CXXFLAGS += -DMST_DEV_EXAMPLE1=\"03:00.0\" -DMST_DEV_EXAMPLE2=\"mlx4_0\" -DMST_DEV_EXAMPLE3=\"03:00.0\" -DMST_DEV_EXAMPLE4=\"04:00.0\"

