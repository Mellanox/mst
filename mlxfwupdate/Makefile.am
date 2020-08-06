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
MTCR_DIR = $(USER_DIR)/${MTCR_CONFIG_DIR}

bin_PROGRAMS = mstfwmanager

docdir=$(INSTALL_BASEDIR)/etc/mstflint
dist_doc_DATA=certificate/ca-bundle.crt

MTCR_UL_DIR = $(USER_DIR)/${MTCR_CONF_DIR}
MTCR_UL_LIB = $(MTCR_UL_DIR)/libmtcr_ul.a

XML_FLAGS = -DUSE_XML -DLIBXML_STATIC
XML_LIBS = -lxml2 $(ZLIB_LIB)

CURL_LIBS = -lcurl  -lssl -lcrypto -lrt

CURL_FLAGS = -DCURL_STATICLIB

common_CPPFLAGS = -I$(USER_DIR)/common\
            -I$(USER_DIR)/\
            -I$(USER_DIR)/include/mtcr_ul\
            -I$(USER_DIR)/mflash\
            -I$(USER_DIR)/cmdparser\
            -I$(USER_DIR)/libmfa\
            $(INIPARSER_CFLAGS)\
            $(JSON_CFLAGS)\
            -I$(USER_DIR)/mlxfwops/lib\
            -I$(USER_DIR)/pldmlib\
            -I$(CURL_INC_DIR)\
            -I$(USER_DIR)/mft_utils\
            -I$(USER_DIR)/dev_mgt

AM_CXXFLAGS = -g -MP -MD -Wall -W $(BOOST_CXX_IGNORES)


common_SOURCEES =  mlxfwmanager_common.cpp mlxfwmanager_common.h\
                        menu.cpp menu.h\
                        mlxfwmanager.cpp mlxfwmanager.h\
                        mlnx_dev.cpp mlnx_dev.h\
                        cmd_line_parser.cpp cmd_line_parser.h\
                        cmd_line_params.cpp cmd_line_params.h\
                        psid_query_item.cpp psid_query_item.h\
                        image_access.cpp image_access.h\
                        server_request.cpp server_request.h\
                        output_fmts.cpp output_fmts.h\
                        psid_lookup_db.cpp psid_lookup_db.h\
                        img_version.cpp img_version.h\
                        err_msgs.h\
						fw_version_old_clp.cpp fw_version_old_clp.h\
                        fw_version_with_sub_build.cpp fw_version_with_sub_build.h
                        
common_LDADD  = $(USER_DIR)/dev_mgt/libdev_mgt.la\
                        $(USER_DIR)/mvpd/libmvpd.la\
                        $(USER_DIR)/mlxfwops/lib/libmlxfwops.a\
                        $(USER_DIR)/pldmlib/libpldm.la\
                        $(USER_DIR)/mlxconfig/libmlxcfg.la\
                        $(USER_DIR)/libmfa/libmfa.la\
                        $(USER_DIR)/ext_libs/minixz/libminixz.a\
                        $(USER_DIR)/cmdparser/libcmdparser.a\
                        $(USER_DIR)/mflash/libmflash.la\
                        $(USER_DIR)/tools_res_mgmt/libtools_res_mgmt.a\
                        $(JSON_LIBS)\
                        $(INIPARSER_LIBS)\
                        $(MUPARSER_LIBS)\
                        $(SQLITE_LIBS)\
                        $(USER_DIR)/mft_utils/libmftutils.a\
                        $(USER_DIR)/cmdif/libcmdif.la\
                        $(USER_DIR)/reg_access/libreg_access.la\
                        $(USER_DIR)/tools_layouts/libtools_layouts.la\
                        $(USER_DIR)/fw_comps_mgr/libfw_comps_mgr.a\
                        $(XML_LIBS)\
                        $(CURL_LIBS)\
                        -lm $(LIBSTD_CPP) ${LDL}
                        
if ENABLE_OPENSSL
common_LDADD += $(top_srcdir)/mlxsign_lib/libmlxsign.a -lcrypto
else
endif

if ENABLE_CS
common_LDADD += $(top_srcdir)/tools_crypto/libtools_crypto.a -lcrypto
else
endif

if ENABLE_INBAND
common_LDADD += $(top_srcdir)/mad_ifc/libmad_ifc.a
else
endif

mstfwmanager_CXXFLAGS =  -DMSTFLINT -DUSE_CURL $(AM_CXXFLAGS) $(CURL_FLAGS) $(XML_FLAGS) -I$(MTCR_UL_DIR) $(common_CPPFLAGS)
mstfwmanager_SOURCES  = $(common_SOURCEES)
mstfwmanager_LDADD    = $(common_LDADD) $(MTCR_UL_LIB) -lz
