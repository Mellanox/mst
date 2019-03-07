/*                  - Mellanox Confidential and Proprietary -
 *
 *  Copyright (C) Jan 2013, Mellanox Technologies Ltd.  ALL RIGHTS RESERVED.
 *
 *  Except as specifically permitted herein, no portion of the information,
 *  including but not limited to object code and source code, may be reproduced,
 *  modified, distributed, republished or otherwise exploited in any form or by
 *  any means for any purpose without the prior written permission of Mellanox
 *  Technologies Ltd. Use of software subject to the terms and conditions
 *  detailed in the file "LICENSE.txt".
 *
 */

#include <sstream>
#include "mlxreg_lib.h"
#include "cmdif/icmd_cif_open.h"
#ifndef MST_UL
#include <tools_layouts/adb_dbs.h>
#endif
#include <tools_layouts/prm_adb_db.h>
#include <common/tools_utils.h>
#include <mlxreg_exception.h>

#define REG_ACCESS_UNION_NODE "access_reg_summary"

using namespace mlxreg;

/**********************************************************
 *             Supported Register Access                  *
 *********************************************************/
const u_int64_t MlxRegLib::_gSupportedRegisters[] = {
    0x5008,         //PPCNT
    0x5006,         //PAOS
    0x5004,         //PTYS
    0x5026,         //SLRP
    0x5027,         //SLTP
    0x5028,         //SLRG
    0x5029,         //PTAS
    0x5018,         //PPLR
    0x5023,         //PPLM
    0x503c,         //PMDR
    0x9050,         //MPEIN
    0x9051,         //MPCNT
    0x5036,         //PPTT
    0x5037,         //PPRT
    0x5040,         //PPAOS
    0x5031,         //PDDR
    0x5002,         //PMLP
    0x500A,         //PLIB
    0x9014,         //MCIA
    0x902b,         //MLCR
    0x0             //Termination - DO NOT DELETE!
};

const int MlxRegLib::RETRIES_COUNT = 3;
const int MlxRegLib::SLEEP_INTERVAL = 100;

/************************************
* Function: MlxRegLib
************************************/
MlxRegLib::MlxRegLib(mfile *mf, string extAdbFile, bool onlyKnown, bool isExternal)
{
    _mf = mf;
    _onlyKnownRegs = onlyKnown;
    _isExternal = isExternal;
    try {
        if (_isExternal) {
            dm_dev_id_t devID = getDevId();
            extAdbFile = PrmAdbDB::getDefaultDBName(dm_dev_is_switch(devID));
        }
        initAdb(extAdbFile);
    } catch (MlxRegException& adbInitExp) {
        if (_adb) {
            delete _adb;
        }
        throw adbInitExp;
    }
    string unionNode = REG_ACCESS_UNION_NODE;
    string rootNode  = unionNode + "_selector";
    if (_isExternal) {
        rootNode  = rootNode + "_ext";
    }

    _regAccessRootNode  = _adb->createLayout(rootNode);
    if (!_regAccessRootNode) {
        throw MlxRegException("No supported access registers found");
    }
    _regAccessUnionNode = _regAccessRootNode->getChildByPath(unionNode);
    if (!_regAccessUnionNode) {
        throw MlxRegException("No supported access registers found");
    }
    if (!_regAccessUnionNode->isUnion()) {
        throw MlxRegException("No supported access registers found");
    }
    try
    {
        _regAccessMap = _regAccessUnionNode->unionSelector->getEnumMap();
        _supportedRegAccessMap = genSuppRegsList(_regAccessMap);
    }
    catch (AdbException& exp)
    {
        throw MlxRegException("Failed to extract registers info. %s", exp.what());
    }
    // Set error map
    std::map<int, std::string> errmap;
    errmap[MRLS_SUCCESS] = "Success";
    errmap[MRLS_GENERAL] = "General error";
    updateErrCodes(errmap);
}

/************************************
* Function: ~MlxRegLib
************************************/
MlxRegLib::~MlxRegLib()
{
    if (_regAccessRootNode) {
        delete _regAccessRootNode;
    }
    if (_adb) {
        delete _adb;
    }
}

dm_dev_id_t MlxRegLib::getDevId()
{
    return getDevId(_mf);
}

dm_dev_id_t MlxRegLib::getDevId(mfile *mf)
{
    dm_dev_id_t devID = DeviceUnknown;
    u_int32_t hwDevID = 0;
    u_int32_t hwChipRev = 0;
    if (dm_get_device_id(mf, &devID, &hwDevID, &hwChipRev)) {
        throw MlxRegException("Failed to read device ID");
    }
    return devID;
}

bool MlxRegLib::isDeviceSupported(mfile *mf)
{
    dm_dev_id_t devID = getDevId(mf);
    return devID != DeviceConnectX2 && devID != DeviceConnectX3 && devID != DeviceConnectX3Pro && devID != DeviceSwitchX;
}

void MlxRegLib::initAdb(string extAdbFile)
{
    _adb = new Adb();
    if (extAdbFile != "") {
        if (!_adb->load(extAdbFile, false, NULL, false)) {
            throw MlxRegException("Failure in loading Adabe file. %s", _adb->getLastError().c_str());
        }
    } else {
        throw MlxRegException("No Adabe was provided, please provide Adabe file to continue");
    }
}

/************************************
* Function: findAdbNode
************************************/
AdbInstance* MlxRegLib::findAdbNode(string name)
{
    if (_supportedRegAccessMap.find(name) == _supportedRegAccessMap.end()) {
        throw MlxRegException("Can't find access register name: %s", name.c_str());
    }
    return _regAccessUnionNode->getUnionSelectedNodeName(name);
}

/************************************
* Function: showRegister
************************************/
MlxRegLibStatus MlxRegLib::showRegister(string regName, std::vector<AdbInstance*> &fields)
{
    AdbInstance *adbNode = findAdbNode(regName);
    fields = adbNode->getLeafFields();
    return MRLS_SUCCESS;
}

/************************************
* Function: showRegisters
************************************/
MlxRegLibStatus MlxRegLib::showRegisters(std::vector<string> &regs)
{
    for (std::map<string, u_int64_t>::iterator it = _supportedRegAccessMap.begin(); it != _supportedRegAccessMap.end(); ++it) {
        regs.push_back(it->first);
    }
    return MRLS_SUCCESS;
}

/************************************
* Function: showRegisters
************************************/
MlxRegLibStatus MlxRegLib::showAllRegisters(std::vector<string> &regs)
{
    for (std::map<string, u_int64_t>::iterator it = _regAccessMap.begin(); it != _regAccessMap.end(); ++it) {
        regs.push_back(it->first);
    }
    return MRLS_SUCCESS;
}

/************************************
* Function: sendMaccessReg
************************************/
int MlxRegLib::sendMaccessReg(u_int16_t regId, int method, std::vector<u_int32_t> &data)
{
    int status;
    int rc;
    std::vector<u_int32_t> temp_data;
    copy(data.begin(), data.end(), back_inserter(temp_data));
    int i = RETRIES_COUNT;
    do {
        rc = maccess_reg(_mf,
                           regId,
                           (maccess_reg_method_t)method,
                           (u_int32_t*)&data[0],
                           (sizeof(u_int32_t) * data.size()),
                           (sizeof(u_int32_t) * data.size()),
                           (sizeof(u_int32_t) * data.size()),
                           &status);
        if ((rc != ME_ICMD_STATUS_IFC_BUSY &&
                status != ME_REG_ACCESS_BAD_PARAM) ||
                !(_mf->flags & MDEVS_REM)) {
            break;
        }
        data.clear();
        copy(temp_data.begin(), temp_data.end(), back_inserter(data));
        msleep(SLEEP_INTERVAL);
    } while(i-->0);
    temp_data.clear();
    return rc;
}

/************************************
* Function: sendRegister
************************************/
MlxRegLibStatus MlxRegLib::sendRegister(string regName, int method, std::vector<u_int32_t> &data)
{
    u_int16_t regId = (u_int16_t)_supportedRegAccessMap.find(regName)->second;
    int rc;
    rc = sendMaccessReg(regId, method, data);
    if (rc) {
        throw MlxRegException("Failed to send access register: %s", m_err2str((MError)rc));
    }
    return MRLS_SUCCESS;
}

/************************************
* Function: sendRegister
************************************/
MlxRegLibStatus MlxRegLib::sendRegister(u_int16_t regId, int method, std::vector<u_int32_t> &data)
{
    int rc;
    rc = sendMaccessReg(regId, method, data);
    if (rc) {
        throw MlxRegException("Failed send access register: %s", m_err2str((MError)rc));
    }
    return MRLS_SUCCESS;
}

/************************************
* Function: getLastErrMsg
************************************/
string MlxRegLib::getLastErrMsg()
{
    std::stringstream sstm;
    int lastErrCode = getLastErrCode();
    string errCodeStr = err2Str(lastErrCode);
    string errStr = err();
    sstm << errCodeStr;
    if (errStr != errCodeStr) {
        sstm << ": " << errStr;
    }
    return sstm.str();
}

/************************************
* Function: genSuppRegsList
************************************/
std::map<string, u_int64_t> MlxRegLib::genSuppRegsList(std::map<string, u_int64_t> availableRegs)
{
    std::map<string, u_int64_t> supportedReg;
    for (std::map<string, u_int64_t>::iterator it = availableRegs.begin(); it != availableRegs.end(); it++) {
        if (_onlyKnownRegs) {
            if (isRegAccessSupported(it->second) && isRegSizeSupported(it->first)) {
                supportedReg.insert(*it);
            }
        } else {
            supportedReg.insert(*it);
        }
    }
    return supportedReg;
}

/************************************
* Function: isRegSizeSupported
************************************/
bool MlxRegLib::isRegSizeSupported(string regName)
{
    AdbInstance *adbNode = _regAccessUnionNode->getUnionSelectedNodeName(regName);
    return (((adbNode->size >> 3) <= (u_int32_t)mget_max_reg_size(_mf, MACCESS_REG_METHOD_SET)) ||
            ((adbNode->size >> 3) <= (u_int32_t)mget_max_reg_size(_mf, MACCESS_REG_METHOD_GET)));
}

/************************************
* Function: isRegAccessSupported
************************************/
bool MlxRegLib::isRegAccessSupported(u_int64_t regID)
{
    u_int64_t *suppRegId = (u_int64_t*)_gSupportedRegisters;
    while (*suppRegId) {
        if (*suppRegId == regID) {
            return true;
        }
        suppRegId++;
    }
    return false;
}

/************************************
* Function: isAccessRegisterSupported
************************************/
void MlxRegLib::isAccessRegisterSupported(mfile *mf)
{
    int status;
    struct connectx4_icmd_query_cap_general icmd_cap;
    int i = RETRIES_COUNT;
    do {
        memset(&icmd_cap, 0, sizeof(icmd_cap));
        status = get_icmd_query_cap(mf, &icmd_cap);
        if(!(status || icmd_cap.allow_icmd_access_reg_on_all_registers == 0))
            break;
        msleep(SLEEP_INTERVAL);
    } while (i-- > 0);

    if (status || icmd_cap.allow_icmd_access_reg_on_all_registers == 0) {
        throw MlxRegException("FW burnt on device does not support generic access register");
    }
}
/************************************
* Function: isAccessRegisterGMPSupported
************************************/
bool MlxRegLib::isAccessRegisterGMPSupported(maccess_reg_method_t reg_method)
{
    return (bool)(supports_reg_access_gmp(_mf, reg_method));
}

/************************************
* Function: isIBDevice
************************************/
bool MlxRegLib::isIBDevice()
{
    return (bool)(_mf->flags & MDEVS_IB);
}
