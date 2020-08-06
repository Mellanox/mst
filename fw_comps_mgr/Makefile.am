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
USER_DIR = $(top_srcdir)
MTCR_DIR = $(USER_DIR)/${MTCR_CONF_DIR}
MTCR_INC_DIR = $(top_srcdir)/include/mtcr_ul
CMDIF_DIR = $(USER_DIR)/cmdif
REG_ACCESS_DIR = $(USER_DIR)/reg_access
COMMON_DIR = $(USER_DIR)/common
LAYOUTS_DIR = $(USER_DIR)/tools_layouts
UTILS_DIR = $(USER_DIR)/mft_utils
MAD_IFC_DIR = $(USER_DIR)/mad_ifc
TOOLS_RES_MGMT_DIR = $(USER_DIR)/tools_res_mgmt

AM_CPPFLAGS = -I. -I$(USER_DIR) -I$(MTCR_INC_DIR) -I$(MTCR_DIR) -I$(REG_ACCESS_DIR) -I$(CMDIF_DIR) \
           -I$(COMMON_DIR) -I $(LAYOUTS_DIR) -I $(UTILS_DIR) -I$(TOOLS_RES_MGMT_DIR) 

FWCOMPS_VERSION = 1

AM_CFLAGS = -MD -pipe -Wall -W $(COMPILER_FPIC)

AM_CXXFLAGS = -Wall -W -g -MP -MD -pipe $(COMPILER_FPIC)

noinst_LIBRARIES = libfw_comps_mgr.a

libfw_comps_mgr_a_SOURCES =  fw_comps_mgr.cpp fw_comps_mgr.h fw_comps_mgr_dma_access.cpp fw_comps_mgr_dma_access.h fw_comps_mgr_direct_access.cpp fw_comps_mgr_direct_access.h fw_comps_mgr_abstract_access.cpp fw_comps_mgr_abstract_access.h
                  

libfw_comps_mgr_a_LIBADD = ../reg_access/libreg_access.la $(MTCR_DIR)/libmtcr_ul.a $(LAYOUTS_LIB) $(TOOLS_RES_MGMT_DIR)/libtools_res_mgmt.a

if ENABLE_INBAND
libfw_comps_mgr_a_LIBADD +=  $(top_srcdir)/mad_ifc/libmad_ifc.a
endif
