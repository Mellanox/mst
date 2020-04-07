# Copyright (C) Jan 2020 Mellanox Technologies Ltd. All rights reserved.   
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
# --                                                                        


#######################################################
# 
# MenuSegment.py
# Python implementation of the Class MenuSegment
# Generated by Enterprise Architect
# Created on:      14-Aug-2019 10:11:57 AM
# Original author: talve
# 
#######################################################
from segments.Segment import Segment
from segments.MenuRecord import MenuRecord
from segments.SegmentFactory import SegmentFactory
from utils import constants as cs
from utils.Exceptions import DumpNotSupported


class MenuSegment(Segment):
    """This class is responsible for holding menu segment data.
    """
    RECORD_BITS_SIZE = 416
    RECORD_DWORDS_SIZE = 13

    def __init__(self, data):
        """Initialize the class by setting the class data.
        """
        self.raw_data = data
        self.type = cs.RESOURCE_DUMP_SEGMENT_TYPE_MENU
        self.size = '{:0b}'.format(data[cs.SEGMENT_SIZE_DWORD_LOCATION]).zfill(32)[cs.SEGMENT_SIZE_START: cs.
                                                                                   SEGMENT_SIZE_END]
        self.num_of_records = int('{:0b}'.format(data[cs.MENU_SEGMENT_NUM_OF_RECORDS_DWORD_LOCATION]
                                                 ).zfill(32)[cs.MENU_SEGMENT_NUM_OF_RECORDS_START: cs.
                                                             MENU_SEGMENT_NUM_OF_RECORDS_END], 2)
        self.records = []
        for i in range(self.num_of_records):
            rec = MenuRecord(data[(2 + i * self.RECORD_DWORDS_SIZE): (2 + (i + 1) * self.RECORD_DWORDS_SIZE)])
            self.records.append(rec)

    def get_records(self):
        """get all the menu segment records.
        """
        return self.records

    def get_printable_records(self):
        """get all the menu segment records in a printable format.
        """
        recs = []
        for rec in self.records:
            recs.append(rec.convert_record_obj_to_printable_list())
        return recs

    def get_data(self):
        """get the menu segment data.
        """
        return self.raw_data

    def get_type(self):
        """get the menu segment type.
        """
        return cs.RESOURCE_DUMP_SEGMENT_TYPE_MENU

    def get_segment_type_by_segment_name(self, segment_type):
        """get the menu segment type by a given segment name.
           if not found, method will return the given segment type
        """
        for rec in self.records:
            if MenuRecord.bin_list_to_ascii(rec.segment_name) == segment_type:
                segment_type = str(hex(int(rec.segment_type, 16)))
                break
        return segment_type

    def is_supported(self, **kwargs):
        """check if the arguments matches with the menu data.
        """
        dump_type = kwargs["segment"]
        index1 = kwargs["index1"]
        index2 = kwargs["index2"]
        num_of_objs_1 = kwargs["numOfObj1"]
        num_of_objs_2 = kwargs["numOfObj2"]
        match_rec = None

        # Check whether dump type supported
        try:
            for rec in self.records:
                if rec.segment_type == dump_type or MenuRecord.bin_list_to_ascii(rec.segment_name) == dump_type:
                    match_rec = rec
                    break

            if not match_rec:
                raise DumpNotSupported("Dump type: {0} is not supported".format(dump_type))

            # Check index1 attribute
            if not index1 and index1 is not 0 and match_rec.must_have_index1:
                raise DumpNotSupported(
                    "Dump type: {0} must have index1 attribute, and it wasn't provided".format(dump_type))

            if not index2 and index2 is not 0 and match_rec.must_have_index2:
                raise DumpNotSupported(
                    "Dump type: {0} must have index2 attribute, and it wasn't provided".format(dump_type))

            if index1 and not match_rec.supports_index1:
                raise DumpNotSupported(
                    "Dump type: {0} does not support index1 attribute, and it was provided".format(dump_type))

            if index2 and not match_rec.supports_index2:
                raise DumpNotSupported(
                    "Dump type: {0} does not support index2 attribute, and it was provided".format(dump_type))

            if not num_of_objs_1 and match_rec.must_have_num_of_obj1:
                raise DumpNotSupported(
                    "Dump type: {0} must have numOfObj1 attribute, and it wasn't provided".format(dump_type))

            if not num_of_objs_2 and match_rec.must_have_num_of_obj2:
                raise DumpNotSupported(
                    "Dump type: {0} must have numOfObj2 attribute, and it wasn't provided".format(dump_type))

            if num_of_objs_1 is not None and not match_rec.supports_num_of_obj1:
                raise DumpNotSupported(
                    "Dump type: {0} does not support numOfObj1 attribute, and it was provided".format(dump_type))

            if num_of_objs_2 is not None and not match_rec.supports_num_of_obj2:
                raise DumpNotSupported(
                    "Dump type: {0} does not support numOfObj2 attribute, and it was provided".format(dump_type))

            if num_of_objs_1 == "all" and not match_rec.supports_all_num_of_obj1:
                raise DumpNotSupported(
                    "Dump type: {0} does not support 'all' as numOfObj1 attribute".format(dump_type))

            if num_of_objs_2 == "all" and not match_rec.supports_all_num_of_obj2:
                raise DumpNotSupported(
                    "Dump type: {0} does not support 'all' as numOfObj2 attribute".format(dump_type))

            if num_of_objs_1 == "active" and not match_rec.supports_active_num_of_obj1:
                raise DumpNotSupported(
                    "Dump type: {0} does not support 'active' as numOfObj1 attribute".format(dump_type))

            if num_of_objs_2 == "active" and not match_rec.supports_active_num_of_obj2:
                raise DumpNotSupported(
                    "Dump type: {0} does not support 'active' as numOfObj2 attribute".format(dump_type))

        except DumpNotSupported as e:
            print(e)
            return False

        return True


SegmentFactory.register(cs.RESOURCE_DUMP_SEGMENT_TYPE_MENU, MenuSegment)
