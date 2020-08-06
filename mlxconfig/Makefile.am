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
MTCR_DIR = $(top_srcdir)/${MTCR_CONF_DIR}
COMMON_DIR = $(top_srcdir)/common
LAYOUTS_DIR = $(top_srcdir)/tools_layouts
UTILS_DIR = $(top_srcdir)/mft_utils
DEV_MGT_DIR = $(top_srcdir)/dev_mgt
LAYOUTS_LIB = $(LAYOUTS_DIR)/libtools_layouts.la
UTILS_LIB = $(USER_DIR)/mft_utils/libmftutils.a
CMDIF_DIR = $(USER_DIR)/cmdif
MLXSIGN_DIR = $(USER_DIR)/mlxsign_lib
MLXSIGN_LIB = $(MLXSIGN_DIR)/libmlxsign.a
COMPS_MGR_DIR = $(USER_DIR)/fw_comps_mgr
TOOLS_RES_MGMT_DIR = $(USER_DIR)/tools_res_mgmt

PYTHON_WRAPPER_SCRIPT = ../common/python_wrapper

SUBDIRS = mlxconfig_dbs

AM_CPPFLAGS = -I. -I$(USER_DIR) -I$(top_srcdir)/include/mtcr_ul -I$(MTCR_DIR) -I$(COMMON_DIR) $(WIN64_INC)\
           -I$(LAYOUTS_DIR) -I$(UTILS_DIR) -I$(DEV_MGT_DIR) -I$(CMDIF_DIR) -I$(TOOLS_RES_MGMT_DIR) $(MUPARSER_CFLAGS) $(SQLITE_CFLAGS) $(COMPILER_FPIC)


AM_CXXFLAGS = -pthread -Wall -W -g -MP -MD -pipe  -Wno-deprecated-declarations  $(COMPILER_FPIC) -DDATA_PATH=\"$(pkgdatadir)\"
bin_PROGRAMS = mstconfig
MLXPRIVHOST_PYTHON_WRAPPER=mstprivhost
bin_SCRIPTS = ${MLXPRIVHOST_PYTHON_WRAPPER}
${MLXPRIVHOST_PYTHON_WRAPPER}: $(PYTHON_WRAPPER_SCRIPT)
	cp $(PYTHON_WRAPPER_SCRIPT) $@

mlxprivhostlibdir=$(libdir)/mstflint/python_tools/$(MLXPRIVHOST_PYTHON_WRAPPER)
mlxprivhostlib_SCRIPTS = $(MLXPRIVHOST_PYTHON_WRAPPER).py

noinst_LTLIBRARIES = libmlxcfg.la

libmlxcfg_la_SOURCES = mlxcfg_status.h mlxcfg_utils.h mlxcfg_utils.cpp mlxcfg_view.h\
        mlxcfg_expression.h mlxcfg_expression.cpp\
mlxcfg_db_items.h mlxcfg_db_items.cpp\
        mlxcfg_db_manager.h mlxcfg_db_manager.cpp mlxcfg_param.h mlxcfg_param.cpp mlxcfg_tlv.h mlxcfg_tlv.cpp\
        mlxcfg_commander.h mlxcfg_commander.cpp mlxcfg_generic_commander.h mlxcfg_generic_commander.cpp\
        mlxcfg_param_lib.h mlxcfg_param_lib.cpp mlxcfg_4thgen_commander.h mlxcfg_4thgen_commander.cpp

libmlxcfg_la_LIBADD = $(UTILS_LIB) $(MUPARSER_LIBS) $(SQLITE_LIBS)\
        $(CMDIF_DIR)/libcmdif.la ../reg_access/libreg_access.la $(LAYOUTS_LIB) $(MTCR_DIR)/libmtcr_ul.a\
        $(DEV_MGT_DIR)/libdev_mgt.la $(COMPS_MGR_DIR)/libfw_comps_mgr.a $(TOOLS_RES_MGMT_DIR)/libtools_res_mgmt.a $(MLNXOS_PPC_LIBS) $(LIBSTD_CPP) ${LDL}

mstconfig_LDADD = libmlxcfg.la $(UTILS_LIB) $(MUPARSER_LIBS) $(SQLITE_LIBS)\
        $(CMDIF_DIR)/libcmdif.la ../reg_access/libreg_access.la $(LAYOUTS_LIB) $(MTCR_DIR)/libmtcr_ul.a\
        $(DEV_MGT_DIR)/libdev_mgt.la $(COMPS_MGR_DIR)/libfw_comps_mgr.a $(TOOLS_RES_MGMT_DIR)/libtools_res_mgmt.a $(MLNXOS_PPC_LIBS) $(LIBSTD_CPP) ${LDL}

if DISABLE_XML2
AM_CXXFLAGS += -DDISABLE_XML2
else
mstconfig_LDADD += -lxml2
libmlxcfg_la_LIBADD += -lxml2
endif

if ENABLE_INBAND
mstconfig_LDADD += $(top_srcdir)/mad_ifc/libmad_ifc.a
libmlxcfg_la_LIBADD += $(top_srcdir)/mad_ifc/libmad_ifc.a
else
endif

if ENABLE_OPENSSL
mstconfig_LDADD +=   $(MLXSIGN_LIB) -lcrypto
libmlxcfg_la_LIBADD += $(MLXSIGN_LIB) -lcrypto
else
AM_CXXFLAGS += -DNO_OPEN_SSL
endif

mstconfig_SOURCES =  mlxcfg_ui.h mlxcfg_parser.cpp mlxcfg_ui.cpp

#get mst device examples and tool name from makefile
AM_CXXFLAGS += -DMLXCFG_NAME=\"mstconfig\"
AM_CXXFLAGS += -DMST_DEV_EXAMPLE=\"04:00.0\" -DMST_DEV_EXAMPLE2=\"05:00.0\"

