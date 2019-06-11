/*
 * Copyright (C) Jan 2019 Mellanox Technologies Ltd. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

 *
 */

#include "mlxlink_commander.h"

using namespace mlxreg;

MlxlinkCommander::MlxlinkCommander() : RegAccessParser("", "", NULL, 0),_userInput()

{
    _device = "";
    _mf = NULL;
    _extAdbFile = "";
    _localPort = 0;
    _depth = 0;
    _pcieIndex = 0;
    _node = 0;
    _numOfLanes = 4;
    _numOfLanesPcie = 0;
    _writeGvmi = false;
    _linkUP = false;
    _splitted = false;
    _plugged = false;
    _linkModeForce = false;
    _prbsTestMode = false;
    _speedForce = "";
    _anDisable = 0;
    _cableAtten12G = 0;
    _cableLen = 0;
    _cablePN = "N/A";
    _cableSN = "N/A";
    _moduleTemp = "N/A";
    _cableMediaType = 0;
    _uniqueCmds = 0;
    _networkCmds = 0;
    _mlxLinkLib = NULL;
    _mlxlinkLogger = NULL;
    _activeSpeed = 0;
    _activeSpeedEx = 0;
    _protoCapability = 0;
    _protoCapabilityEx = 0;
    _protoAdmin = 0;
    _protoAdminEx = 0;
    _speedBerCsv = 0;
    _fecActive = 0;
    _protoActive = 0;
    _productTechnology = 0;
    _allUnhandledErrors = "";
    _mlxlinkMaps = MlxlinkMaps::getInstance();
}

MlxlinkCommander::~MlxlinkCommander()
{
    if (_writeGvmi) {
        writeGvmi(0);
    }
    if (_mf) {
        mclose(_mf);
    }
    if (_mlxLinkLib) {
        delete _mlxLinkLib;
    }
}

u_int32_t MlxlinkCommander::getFieldValue(const string &field_name, std::vector<u_int32_t>& buff)
{
    u_int32_t field_Val = RegAccessParser::getFieldValue(field_name, buff);
    DEBUG_LOG(_mlxlinkLogger,"%-15s: %-30s\tVALUE: 0x%08x (%d)\n","GET_FIELD",
            (char*)field_name.c_str(),field_Val,field_Val) ;
    return field_Val;
}

void MlxlinkCommander::genBuffSendRegister(const string &regName, maccess_reg_method_t method)
{
    DEBUG_LOG(_mlxlinkLogger, "%-15s: %s\n", "ACCESS_METHOD",
            method == MACCESS_REG_METHOD_GET ? "GET": "SET");
    if (_writeGvmi) {
        writeGvmi(1);
    }
    _buffer = genBuffUnknown();
    _mlxLinkLib->sendRegister(regName, method, _buffer);

    if (_writeGvmi) {
        writeGvmi(0);
    }
}

void MlxlinkCommander::updateField(const string &field_name, u_int32_t value)
{
    DEBUG_LOG(_mlxlinkLogger, "%-15s: %-30s\tVALUE: 0x%08x (%d)\n",
            "UPDATE_FIELD", (char*)field_name.c_str(),value,value);
    RegAccessParser::updateField(field_name, value);
}

void MlxlinkCommander::resetParser(const string &regName)
{
    DEBUG_LOG(_mlxlinkLogger, "%s: %-30s\n","OPERATION ON REGISTER",
            (char*)regName.c_str());
    _regNode = _mlxLinkLib->findAdbNode(regName);
    _data = "";
    _indexes = "";
    _len = 0;
    setParseMethod();
    _buffer.resize(_len);
    for (std::vector<u_int32_t>::size_type j = 0; j < _len; j++) {
        _buffer[j] = 0;
    }
}

void MlxlinkCommander::setParseMethod()
{
    if (!_regNode) {
        if (_len > MAX_REG_SIZE) {
            throw MlxRegException("Register length: 0x%08x is too large", _len);
        }
        _parseMode = Pm_Unknown;
        _len = (_len + 3) / 4;
    } else {
        _parseMode = Pm_Known;
        _len = (_regNode->size) >> 5;
    }
}

void MlxlinkCommander::checkRegCmd()
{
    if(_userInput._pcie){
        return;
    }
    string regName = "PAOS";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("swid", SWID);
    try {
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    } catch (MlxRegException& exp) {
        throw MlxRegException("Problem accessing the device. Please verify that driver is up");
    }
    throw MlxRegException("FW Version " + _fwVersion + " is not supported. Please update FW.");
}

void MlxlinkCommander::checkValidFW()
{
    try {
        struct connectib_icmd_get_fw_info fw_info;
        memset(&fw_info, 0, sizeof(fw_info));
        gcif_get_fw_info(_mf, &fw_info);
        _fwVersion = to_string(fw_info.fw_version.MAJOR) + "."
                     + to_string(fw_info.fw_version.MINOR) + "."
                     + to_string(fw_info.fw_version.SUBMINOR);
        if(_userInput._pcie){ //PDDR is not relevant to pcie
            return;
        }
        string regName = "PDDR";
        resetParser(regName);
        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_OPERATIONAL_INFO_PAGE);

        try {

            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (MlxRegException& exp) {
            if (exp.what_s() != "Please Provide Valid gvmi Address (--gvmi_address <address>)") {
                checkRegCmd();
            } else {
                throw MlxRegException(exp);
            }
        }

        u_int32_t phyMngrFsmState = getFieldValue("phy_mngr_fsm_state", _buffer);

        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_TROUBLESHOOTING_INFO_PAGE);
        updateField("group_opcode", ADVANCED_OPCODE);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);


        u_int32_t statusOpcode = getFieldValue("advanced_opcode", _buffer);

        if (phyMngrFsmState == 0 && statusOpcode == 0) {
            checkAllPortsStatus();
        }
    } catch (const std::exception &exc) {
        throw MlxRegException(string(
                "Checking valid firmware raised the following exception: ") +
                string(exc.what()) + string("\n"));
    }
}

int MlxlinkCommander::getProductTechnology()
{
    try {
        string regName = "SLRG";
        resetParser(regName);
        updateField("local_port", _localPort);
        updateField("pnat", (_userInput._pcie) ? PNAT_PCIE : PNAT_LOCAL);
        updateField("lane", 0);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        int version = getVersion(getFieldValue("version", _buffer));
        return version;
    } catch (MlxRegException &exc) {
        throw MlxRegException(
                "Unable to get product technology: %s", exc.what_s().c_str());
    }
}

u_int32_t MlxlinkCommander::maxLocalPort()
{
    switch (_devID) {
    case DeviceSpectrum:
        return MAX_LOCAL_PORT_ETH;

    case DeviceSwitchIB:
    case DeviceSwitchIB2:
        return MAX_LOCAL_PORT_IB;

    case DeviceQuantum:
        return MAX_LOCAL_PORT_QUANTUM;

    case DeviceSpectrum2:
        return MAX_LOCAL_PORT_SPECTRUM2;
    default:
        return 0;
    }
    return 0;
}

bool MlxlinkCommander::checkPortStatus(u_int32_t localPort)
{
    string regName = "PDDR";
    resetParser(regName);

    updateField("local_port", localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("page_select", PDDR_OPERATIONAL_INFO_PAGE);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    u_int32_t phyMngrFsmState = getFieldValue("phy_mngr_fsm_state", _buffer);

    resetParser(regName);

    updateField("local_port", localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("page_select", PDDR_TROUBLESHOOTING_INFO_PAGE);
    updateField("group_opcode", ADVANCED_OPCODE);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    u_int32_t statusOpcode = getFieldValue("advanced_opcode", _buffer);

    if (!(phyMngrFsmState == 0 && statusOpcode == 0)) {
        return true;
    }
    return false;
}

void MlxlinkCommander::checkAllPortsStatus()
{
    if (dm_dev_is_hca(_devID)) {
        checkRegCmd();
    }
    if (_devID == DeviceSpectrum) {
        string regName = "PMLP";
        for (u_int32_t localPort = 1; localPort <= maxLocalPort();
             localPort++) {
            resetParser(regName);
            updateField("local_port", localPort);

            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
            if (getFieldValue("width", _buffer) == 0) {
                continue;
            }
            if (checkPortStatus(localPort)) {
                return;
            }
        }
        throw MlxRegException("FW Version " + _fwVersion + " is not supported. Please update FW.");
    } else if (_devID == DeviceSwitchIB || _devID == DeviceSwitchIB2
               || _devID == DeviceQuantum) {
        string regName = "PLIB";
        for (u_int32_t localPort = 1; localPort <= maxLocalPort();
             localPort++) {
            resetParser(regName);
            updateField("local_port", localPort);

            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
            if (getFieldValue("ib_port", _buffer) == 0) {
                continue;
            }
            if (checkPortStatus(localPort)) {
                return;
            }
        }
        throw MlxRegException("FW Version " + _fwVersion + " is not supported. Please update FW.");
    }
}

void MlxlinkCommander::labelToLocalPort() {
    if (_userInput._pcie) {
        _pcieIndex = _userInput._pcieIndex;
        _depth = _userInput._depth;
        _node = _userInput._node;
        _localPort = _pcieIndex;
        if (_depth > 0) {
            _localPort = _node + DBN_TO_LOCAL_PORT_BASE;
        }
        /* TODO:
         * Once MPIR reg updated (local port field will be added), we have to
         * add support to find the fields of DPN by local port.
         */
        return;
    }
    if (dm_dev_is_hca(_devID)) {
        labelToHCALocalPort();
        return;
    } else if(_userInput._pcie) {
        throw MlxRegException("No PCIE in Switch!");
    }

    if (_devID == DeviceSpectrum || _devID == DeviceSpectrum2) {
        labelToSpectLocalPort();
    } else if (_devID == DeviceSwitchIB || _devID == DeviceSwitchIB2
            || _devID == DeviceQuantum) {
        labelToIBLocalPort();
    }
}

void MlxlinkCommander::labelToHCALocalPort()
{
    u_int32_t mtype = 0;
    mget_mdevs_type(_mf, &mtype);
    if (_splitted) {
        throw MlxRegException("No Splits in HCA!");
    } else if (_userInput._labelPort > 1
            && !(mtype & (MST_USB | MST_USB_DIMAX))) {
        throw MlxRegException(
                "Please Provide the Physical Device of Port "
                        + to_string(_userInput._labelPort));
    } else if (_userInput._labelPort > 1
            && (mtype & (MST_USB | MST_USB_DIMAX))) {
        if (_userInput._labelPort == 2) {
            if (_userInput._gvmiAddress == 0) {
                throw MlxRegException(
                        "Please Provide Valid gvmi Address (--gvmi_address <address>)");
            }
            _writeGvmi = true;
        } else {
            throw MlxRegException("Please Provide Valid Port Number (1/2)");
        }
    }
    _localPort = 1;
}

void MlxlinkCommander::labelToSpectLocalPort()
{
    string regName = "PMLP";
    string splitStr = to_string(
            (_userInput._splitPort > 0) ?
                    _userInput._splitPort - 1 : _userInput._splitPort);
    for (u_int32_t localPort = 1; localPort <= maxLocalPort(); localPort++) {
        resetParser(regName);
        updateField("local_port", localPort);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        _numOfLanes = getFieldValue("width", _buffer);
        if (_numOfLanes == 0) {
            continue;
        }
        if ((getFieldValue("lane0_module_mapping", _buffer) & 0xFF) + 1
                == _userInput._labelPort) {
            checkWidthSplit(localPort);
            _localPort =
                    (_splitted) ?
                            (_numOfLanes == 2 && _devID == DeviceSpectrum2) ?
                                localPort + _userInput._splitPort :
                                localPort + _userInput._splitPort -1 :
                            localPort;
            updateField("local_port", _localPort);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
            if ((getFieldValue("lane0_module_mapping", _buffer) & 0xFF) + 1
                    == _userInput._labelPort) {
                return;
            }
        }
    }
    throw MlxRegException(
                "Failed to find Local Port, please provide valid Port Number");
}

void MlxlinkCommander::checkWidthSplit(u_int32_t localPort)
{
    if (!_splitted) {
        checkUnSplit(localPort);
        return;
    }
    switch (_userInput._splitPort) {
    case 1:
        throw MlxRegException("Invalid split number!");
    case 2:
        if (_numOfLanes < 4) {
            return;
        }
        break;

    case 3:
    case 4:
        if (_numOfLanes == 1) {
            return;
        }
        break;

    default:
        break;
    }
    throw MlxRegException(
              "Port " + to_string(_userInput._labelPort) + "/"
              + to_string(_userInput._splitPort)
              + " does not exist!");
}

void MlxlinkCommander::checkUnSplit(u_int32_t localPort)
{
    if (localPort) {
    }
    return;
    /*if (_numOfLanes == 4) {
       return;
       } else if (_numOfLanes == 2) {
       throw MlxRegException("Port is Splitted! please provide label port and lane, ex: " + convertToString(_labelPort) + "/2");
       } else if (_numOfLanes == 1) {
       //TODO - 4x-1x
       if (localPort) {}
       //throw MlxRegException("Port is Splitted! please provide label port and lane, ex: " + convertToString(_labelPort) + "/4");
       }*/
}

void MlxlinkCommander::labelToIBLocalPort()
{
    u_int32_t labelPort = _userInput._labelPort;
    if (_splitted && _devID != DeviceQuantum) {
        throw MlxRegException("No split in IB!");
    }
    bool ibSplitReady = isIBSplitReady();
    if (_devID == DeviceQuantum) {
        toIBLabelPort(labelPort, ibSplitReady);
    } else if (labelPort > maxLocalPort()) {
        throw MlxRegException("Invalid port number!");
    }

    string regName = "PLIB";
    for (u_int32_t localPort = 1; localPort <= maxLocalPort(); localPort++) {
        resetParser(regName);
        updateField("local_port", localPort);
        try {
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (MlxRegException & exp) {
            continue; // no reason to fail when one of the ports is not supported, such as in split mode.
        }
        if (getFieldValue("ib_port", _buffer) == labelPort) {
            _localPort = localPort;
            return;
        }
    }
    if (_devID == DeviceQuantum && ibSplitReady) {
        throw MlxRegException("Port %d is not splitted physically from switch "
                "side, Use this command to split it physically:\n"
                "interface ib <port/ports range> module-type qsfp-split-2",
                _userInput._labelPort);
    }
}

bool MlxlinkCommander::isIBSplitReady() {
    string regName = "PLIB";
    u_int32_t max_port = maxLocalPort();
    for (u_int32_t localPort = 1; localPort <= max_port; localPort++) {
        resetParser(regName);
        updateField("local_port", localPort);
        try {
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (MlxRegException & exp) {
            continue; // no reason to fail when one of the ports is not supported, such as in split mode.
        }
        if (getFieldValue("ib_port", _buffer) == max_port/2) {
            return true;
        }
    }
    return false;
}

void MlxlinkCommander::toIBLabelPort(u_int32_t &labelPort,bool ibSplitReady)
{
    if (labelPort > (maxLocalPort()-2)/2) {
        throw MlxRegException("Invalid port number!");
    }
    if (ibSplitReady) {
        if (!_splitted) {
            labelPort = (_userInput._labelPort*2) - 1;
        } else {
            switch(_userInput._splitPort){
            case 2:
                labelPort = (_userInput._labelPort*2) - (2-_userInput._splitPort);
                break;
            default:
                throw MlxRegException("Invalid split number!");
            }
        }
    } else if (_splitted) {
        throw MlxRegException("Split mode is not ready!"
                "\nThis command is used to set the split mode ready from switch "
                "side:\nsystem profile ib split-ready");
    }
}

string MlxlinkCommander::getAscii(const string & name, u_int32_t size)
{
    string value = "";
    char c;
    u_int32_t name_tmp;

    /*if (dm_dev_is_hca(_devID)) {
        for (u_int32_t i = 0; i < size; i++) {
            c = (char) getFieldValue(name + "[" + convertToString(i) + "]", _buffer);
            if ((int)c != 0 && (int)c != 32) {
                value.push_back(c);
            }
        }
       } else {*/
    for (u_int32_t i = 0; i < size / 4; i++) {
        name_tmp = getFieldValue(name + "[" + to_string(i) + "]", _buffer);
        for (int k = 24; k > -1; k -= 8) {
            c = (char)(name_tmp >> k);
            if ((int)c != 0 && (int)c != 32) {
                value.push_back(c);
            }
        }
    }
    //}
    return (value != "") ? value : "N/A";
}

string MlxlinkCommander::getFieldStr(const string &field)
{
    return to_string(getFieldValue(field, _buffer));
}

string MlxlinkCommander::getRxTxCDRState(u_int32_t state)
{
    string stateMask = "";
    u_int32_t mask = 0001;
    for (u_int32_t i = 0; i < _numOfLanes; i++) {
        if (state & mask) {
            stateMask += "ON,";
        } else {
            stateMask += "OFF,";
        }
        mask = mask << 1;
    }
    return deleteLastComma(stateMask);
}

void MlxlinkCommander::getActualNumOfLanesIB()
{
    string regName = "PTYS";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("proto_mask", _protoActive);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    _numOfLanes = getFieldValue("ib_link_width_oper", _buffer);
}

void MlxlinkCommander::getActualNumOfLanesETH(u_int32_t linkSpeedActive)
{
    if (_linkUP) {
        _numOfLanes = _mlxlinkMaps->_ETHSpeed2Lanes[linkSpeedActive];
    } else {
        string regName = "PMLP";
        resetParser(regName);
        updateField("local_port", _localPort);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        _numOfLanes = getFieldValue("width", _buffer);
    }
}

void MlxlinkCommander::getActualNumOfLanes(u_int32_t linkSpeedActive)
{
    return (_protoActive == IB) ?
           getActualNumOfLanesIB() : getActualNumOfLanesETH(linkSpeedActive);
}

u_int32_t MlxlinkCommander::activeSpeed2gNum(u_int32_t mask, bool extended)
{
    if(extended) {
        return _mlxlinkMaps->_EthExtSpeed2gNum[mask];
    }
    return (_protoActive == IB) ? _mlxlinkMaps->_IBSpeed2gNum[mask] : _mlxlinkMaps->_ETHSpeed2gNum[mask];
}

string MlxlinkCommander::activeSpeed2Str(u_int32_t mask, bool extended)
{
    if(extended) {
        return _mlxlinkMaps->_EthExtSpeed2Str[mask];
    }
    return (_protoActive == IB) ? _mlxlinkMaps->_IBSpeed2Str[mask] : _mlxlinkMaps->_ETHSpeed2Str[mask];
}

void MlxlinkCommander::getCableParams()
{
    try {
        string regName = "PDDR";
        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_MODULE_INFO_PAGE);

        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

        _cableMediaType = getFieldValue("cable_type", _buffer);
        _plugged = (_cableMediaType != UNPLUGGED);
        _cableAtten12G = (_cableMediaType == PASSIVE) ? getFieldValue("cable_attenuation_12g", _buffer) : 0;
        _cableLen = getFieldValue("cable_length", _buffer);
        _cablePN = getAscii("vendor_pn", 16);
        _cableSN = getAscii("vendor_sn", 16);
        _moduleTemp = getTemp(getFieldValue("temperature", _buffer));
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Getting cable parameters via PDDR raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

bool MlxlinkCommander::inPrbsTestMode()
{
    try {
        bool res = checkPaosDown() && checkPpaosTestMode();
        return res;
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Checking in PRBS test mode raised the following exception: ") + string(exc.what()) + string("\n");
    }
    return false;
}

bool MlxlinkCommander::checkPaosDown()
{
    string regName = "PAOS";
    resetParser(regName);

    updateField("swid", SWID);
    updateField("local_port", _localPort);
    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    u_int32_t paosOperStatus = getFieldValue("oper_status", _buffer);
    if (paosOperStatus == PAOS_DOWN) {
        return true;
    }
    return false;
}

bool MlxlinkCommander::checkPpaosTestMode()
{
    string regName = "PPAOS";
    resetParser(regName);

    updateField("swid", SWID);
    updateField("local_port", _localPort);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    u_int32_t ppaosPhyTestModeStatus = getFieldValue("phy_test_mode_status",
                                                     _buffer);
    if (ppaosPhyTestModeStatus == PHY_TEST_MODE_STATUS) {
        return true;
    }
    return false;
}

u_int32_t MlxlinkCommander::getPtysCap()
{
    string regName = "PTYS";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("proto_mask", _protoActive);
    updateField("an_disable_admin", 0);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    string proto = (_protoActive == IB) ? "ib" : "eth";
    return getFieldValue(proto + "_proto_capability", _buffer);
}

void MlxlinkCommander::getSltpParamsFromVector(std::vector<string> sltpParams)
{
    int startPoint = POLARITY;
    if (_productTechnology == PRODUCT_16NM) {
        startPoint = PRE_2_TAP;
    }
    for (u_int32_t i = startPoint; i < sltpParams.size(); i++) {
        strToUint32((char*) sltpParams[i].c_str(), _userInput._sltpParams[i]);
    }
}

std::vector<string> MlxlinkCommander::parseParamsFromLine(const string & ParamsLine)
{
    std::vector<string> paramVector;
    string param;
    stringstream stream(ParamsLine);
    while (getline(stream, param, ',')) {
        if (param == "") {
            throw  MlxRegException("Wrong input format");
        }
        paramVector.push_back(param.c_str());
    }
    return paramVector;
}

void MlxlinkCommander::setPrintVal(MlxlinkCmdPrint &mlxlinkCmdPrint, int index, string key,
            string value, string color, bool print, bool valid)
{
    mlxlinkCmdPrint.mlxlinkRecords[index].key = key;
    mlxlinkCmdPrint.mlxlinkRecords[index].val = valid? value: "N/A";
    mlxlinkCmdPrint.mlxlinkRecords[index].color = color;
    mlxlinkCmdPrint.mlxlinkRecords[index].visible = print;
}

void MlxlinkCommander::setPrintTitle(MlxlinkCmdPrint &mlxlinkCmdPrint, string title
        , u_int32_t size, bool print)
{
    mlxlinkCmdPrint.title = title;
    mlxlinkCmdPrint.visible = print;
    mlxlinkCmdPrint.initRecords(size);
}

void MlxlinkCommander::writeGvmi(u_int32_t data)
{
    mwrite4(_mf, _userInput._gvmiAddress, data);
}

// Query functions

void MlxlinkCommander::showModuleInfo()
{
    try {
        string regName = "PMLP";
        resetParser(regName);
        updateField("local_port", _localPort);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        u_int32_t module_number = (getFieldValue("lane0_module_mapping", _buffer) & 0xFF);

        regName = "PMAOS";
        resetParser(regName);
        updateField("module", module_number);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        u_int32_t oper_status = getFieldValue("oper_status", _buffer);

        regName = "PDDR";
        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_MODULE_INFO_PAGE);

        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

        string temp, tempHighTH, tempLowTH;
        u_int32_t tempNum;
        float voltage, voltageHighTH, voltageLowTH;
        float rxPowerHighTH, rxPowerLowTH, txPowerHighTH, txPowerLowTH;
        float biasHighTH, biasLowTH;
        std::vector<float> rxPowerLane, txPowerLane, biasCurrentLane;

        tempNum    = getFieldValue("temperature", _buffer);
        temp       = getTemp(tempNum);
        tempHighTH = getTemp(getFieldValue("temperature_high_th", _buffer));
        tempLowTH  = getTemp(getFieldValue("temperature_low_th", _buffer));

        voltage       = getFieldValue("voltage", _buffer) / 10.0;
        voltageHighTH = getFieldValue("voltage_high_th", _buffer) / 10.0;
        voltageLowTH  = getFieldValue("voltage_low_th", _buffer) / 10.0;

        rxPowerHighTH = getPower(getFieldValue("rx_power_high_th", _buffer));
        rxPowerLowTH  = getPower(getFieldValue("rx_power_low_th", _buffer));
        txPowerHighTH = getPower(getFieldValue("tx_power_high_th", _buffer));
        txPowerLowTH  = getPower(getFieldValue("tx_power_low_th", _buffer));
        u_int32_t cableType = getFieldValue("cable_type", _buffer);
        bool Identified = (cableType != UNIDENTIFIED);
        bool plugged = (cableType != UNPLUGGED);
        bool passive = (cableType == PASSIVE);
        u_int32_t cableIdentifier = getFieldValue("cable_identifier", _buffer);
        string cableAttenuation = "N/A";
        if (cableIdentifier != IDENTIFIER_SFP) {
            if (passive) {
                cableAttenuation = getFieldStr("cable_attenuation_5g") + ","
                        + getFieldStr("cable_attenuation_7g") + ","
                        + getFieldStr("cable_attenuation_12g");
                }
        } else {
            cableAttenuation = "Reserved for SFP";
        }
        for (u_int32_t lane = 0; lane < _numOfLanes; lane++) {
            string laneStr = to_string(lane);
            rxPowerLane.push_back(getPower(getFieldValue("rx_power_lane" + laneStr, _buffer)));
            txPowerLane.push_back(getPower(getFieldValue("tx_power_lane" + laneStr, _buffer)));
            biasCurrentLane.push_back(getFieldValue("tx_bias_lane" + laneStr, _buffer) / 500.0);
        }
        biasLowTH = getFieldValue("tx_bias_low_th", _buffer) / 500.0;
        biasHighTH = getFieldValue("tx_bias_high_th", _buffer) / 500.0;
        bool DDMI = (_numOfLanes
                     && (tempNum || voltage || rxPowerLane[0] || txPowerLane[0]
                         || biasCurrentLane[0]));
        bool qsfp = getQsfp(cableIdentifier);
        u_int32_t cableVendor = getFieldValue("cable_vendor", _buffer);
        u_int32_t extCompliance = getFieldValue("ext_ethernet_compliance_code", _buffer);
        u_int32_t compliance = getFieldValue("ethernet_compliance_code", _buffer);
        string complianceLabel = getComplianceLabel(compliance, extCompliance,
                cableIdentifier, cableVendor != MELLANOX);
        u_int32_t cableTech = getFieldValue("cable_technology", _buffer);
        string cableTechnology = (qsfp) ? _mlxlinkMaps->_cableTechnologyQsfp[(cableTech & 240) >> 4] :
                                          _mlxlinkMaps->_cableTechnologySfp[(cableTech & 15)];
        string oui = getOui(cableVendor);

        setPrintTitle(_moduleInfoCmd,"Module Info", MODULE_INFO_LAST);

        setPrintVal(_moduleInfoCmd,MODULE_INFO_IDENTIFIER , "Identifier",
                plugged ? getCableIdentifier(cableIdentifier) : "N/A",
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_COMPLIANCE , "Compliance",
                complianceLabel, ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_CABLE_TECHNOLOGY , "Cable Technology",
                plugged ? cableTechnology : "N/A" , ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_CABLE_TYPE , "Cable Type",
                getCableType(cableType), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_OUI , "OUI",
                plugged ? oui : "N/A", ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_VENDOR_NAME , "Vendor Name",
                getAscii("vendor_name", 16), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_VENDOR_PART_NUMBER , "Vendor Part Number",
                getAscii("vendor_pn", 16), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_VENDOR_SERIAL_NUMBER , "Vendor Serial Number",
                getAscii("vendor_sn", 16), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_REV , "Rev",
                getVendorRev("vendor_rev"), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_ATTENUATION_5G_7G_12G_DB , "Attenuation (5g,7g,12g) [dB]",
                cableAttenuation, ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_WAVELENGTH_NM , "Wavelength [nm]",
                passive ? "N/A" : to_string(getFieldValue("wavelength", _buffer)),
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_TRANSFER_DISTANCE_M , "Transfer Distance [m]",
                getFieldStr("cable_length"), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_DIGITAL_DIAGNOSTIC_MONITORING , "Digital Diagnostic Monitoring",
                DDMI ? "Yes" : "No", ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_POWER_CLASS , "Power Class",
                plugged ? getPowerClass(getFieldValue("cable_power_class", _buffer)) : "N/A",
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_CDR_RX , "CDR RX",
                plugged ? getRxTxCDRState(getFieldValue("rx_cdr_state", _buffer)) : "N/A",
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_CDR_TX , "CDR TX",
                plugged ? getRxTxCDRState(getFieldValue("tx_cdr_state", _buffer)) : "N/A",
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_LOS_ALARM , "LOS Alarm",
                getLosAlarm(), ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_TEMPERATURE_C , "Temperature [C]",
                DDMI ? getValueAndThresholdsStr<string, string>(temp, tempLowTH, tempHighTH) : "N/A",
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_VOLTAGE , "Voltage [mV]",
                DDMI ? getValueAndThresholdsStr<float, float>(voltage, voltageLowTH, voltageHighTH) : "N/A",
                        ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_BIAS_CURRENT_mA , "Bias Current [mA]",
                DDMI ? getValueAndThresholdsStr<string, float>(getStringFromVector(biasCurrentLane),
                biasLowTH, biasHighTH) : "N/A",  ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_RX_POWER_CURRENT_dBm , "Rx Power Current [dBm]",
                DDMI ? getValueAndThresholdsStr<string, float>(getStringFromVector(rxPowerLane),
                rxPowerLowTH, rxPowerHighTH) : "N/A", ANSI_COLOR_RESET, true, Identified && plugged);
        setPrintVal(_moduleInfoCmd,MODULE_INFO_TX_POWER_CURRENT_dBm , "Tx Power Current [dBm]",
                DDMI ? getValueAndThresholdsStr<string, float>(getStringFromVector(txPowerLane),
                txPowerLowTH, txPowerHighTH) : "N/A", ANSI_COLOR_RESET, true, Identified && plugged);

        cout << _moduleInfoCmd;

        if (oper_status != 1) {
            MlxlinkRecord::printWar("Warning: Cannot get module EEPROM information, module operation status is not 'plugged and enabled'");
        }
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing Module Info via PDDR raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

string MlxlinkCommander::getCompliance(u_int32_t compliance, std::map<u_int32_t,
        std::string> complianceMap, bool bitMasked)
{
    string compliance_str = "";
    static const string separator = ", ";
    static const size_t separator_size = separator.size();
    std::map<u_int32_t, std::string>::iterator it;
    if (!compliance) {
        return complianceMap.begin()->second;
    }
    if (bitMasked) {
        for (it=complianceMap.begin(); it!=complianceMap.end(); ++it) {
            u_int32_t comp_bit = it->first;
            string curr_compliance = it->second;
            if (compliance & comp_bit) {
                if (curr_compliance != "" &&
                        (comp_bit != QSFP_ETHERNET_COMPLIANCE_CODE_EXT)) {
                    compliance_str += (curr_compliance + separator);
                }
            }
        }
        size_t str_size = compliance_str.size();
        if(str_size && !(compliance & QSFP_ETHERNET_COMPLIANCE_CODE_EXT)){
            compliance_str.erase(str_size - separator_size, separator_size);
        }
    } else {
        it = complianceMap.find(compliance);
        if (it == complianceMap.end()) {
            compliance_str = "Unknown compliance " + to_string(compliance);
        } else {
            compliance_str =  it->second;
        }
    }
    return compliance_str;
}

string MlxlinkCommander::getComplianceLabel(u_int32_t compliance,
        u_int32_t ext_compliance, u_int32_t cable_identifier,
        bool ignoreExtBitChk)
{
    string compliance_label = "N/A";
    if (_protoActive == ETH) {
        bool qsfp = getQsfp(cable_identifier);
        if (qsfp) {
            if (compliance) {
                compliance_label =
                        getCompliance(compliance, _mlxlinkMaps->_cableComplianceQsfp, true);
                if (ignoreExtBitChk ||
                        (compliance & QSFP_ETHERNET_COMPLIANCE_CODE_EXT)) {
                    compliance_label +=
                            getCompliance(ext_compliance, _mlxlinkMaps->_cableComplianceExt);
                }
            } else if (ignoreExtBitChk) {
                compliance_label =
                        getCompliance(ext_compliance, _mlxlinkMaps->_cableComplianceExt);
            }
        } else if (cable_identifier == IDENTIFIER_SFP) {
            compliance_label =
                    getCompliance(compliance, _mlxlinkMaps->_cableComplianceSfp, true);
        }
    }
    return compliance_label;
}

string MlxlinkCommander::getLosAlarm()
{
    return "N/A";
}

void MlxlinkCommander::prepareSltp28_40nm(std::vector<std::vector<string> > &sltpLanes,
        u_int32_t laneNumber)
{
    sltpLanes[laneNumber].push_back(getFieldStr("polarity"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_tap0"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_tap1"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_tap2"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_bias"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_preemp_mode"));
    if (_userInput._advancedMode) {
        sltpLanes[laneNumber].push_back(getFieldStr("ob_reg"));
        sltpLanes[laneNumber].push_back(getFieldStr("ob_leva"));
    }
}

void MlxlinkCommander::prepareSltp16nm(std::vector<std::vector<string> > &sltpLanes,
        u_int32_t laneNumber)
{
    sltpLanes[laneNumber].push_back(getFieldStr("pre_2_tap"));
    sltpLanes[laneNumber].push_back(getFieldStr("pre_tap"));
    sltpLanes[laneNumber].push_back(getFieldStr("main_tap"));
    sltpLanes[laneNumber].push_back(getFieldStr("post_tap"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_m2lp"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_amp"));
    sltpLanes[laneNumber].push_back(getFieldStr("ob_alev_out"));

}

template<typename T, typename Q>
string MlxlinkCommander::getValueAndThresholdsStr(T value, Q lowTH, Q highTH)
{
    stringstream out;
    out << value << " [" << lowTH << ".." << highTH << "]";
    return out.str();
}

void MlxlinkCommander::showPddr()
{
    try {
        bool print = !_prbsTestMode;
        string regName = "PDDR";
        resetParser(regName);
        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_OPERATIONAL_INFO_PAGE);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        _protoActive = getFieldValue("proto_active", _buffer);
        _fecActive = getFieldValue("fec_mode_active", _buffer);
        u_int32_t phyMngrFsmState = getFieldValue("phy_mngr_fsm_state", _buffer);
        u_int32_t loopbackMode = PHY_NO_LOOPBACK;
        if (phyMngrFsmState != PHY_MNGR_DISABLED) {
            loopbackMode = getFieldValue("loopback_mode", _buffer);
        }
        _linkUP = (phyMngrFsmState == PHY_MNGR_ACTIVE_LINKUP || phyMngrFsmState == PHY_MNGR_PHYSICAL_LINKUP);
        if(_protoActive == IB) {
            _activeSpeed = getFieldValue("link_speed_active", _buffer);
            _protoCapability = getFieldValue("cable_proto_cap", _buffer);
            _protoAdmin = (phyMngrFsmState != 0) ?
                    getFieldValue("core_to_phy_link_proto_enabled", _buffer):0;
        }
        getPtys();
        getActualNumOfLanes(_activeSpeed);

        resetParser(regName);
        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_OPERATIONAL_INFO_PAGE);

        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

        setPrintTitle(_operatingInfoCmd,"Operational Info",
                PDDR_OPERATIONAL_INFO_LAST,print);
        bool extended = (_activeSpeedEx != 0 && _protoAdminEx != 0);
        u_int32_t speed_val = extended ? _activeSpeedEx :_activeSpeed;

        _speedBerCsv = activeSpeed2gNum(speed_val, extended);
        _speedStrG = activeSpeed2Str(speed_val, extended);
        string color = MlxlinkRecord::state2Color(phyMngrFsmState);
        setPrintVal(_operatingInfoCmd,PDDR_STATE ,"State",
                _mlxlinkMaps->_pmFsmState[phyMngrFsmState], color,true,print);
        string physical_state =
                _mlxlinkMaps->_ethANFsmState[getFieldValue("eth_an_fsm_state", _buffer)];
        setPrintVal(_operatingInfoCmd,PDDR_PHYSICAL_STATE , "Physical state",
                physical_state, color, print);
        setPrintVal(_operatingInfoCmd,PDDR_SPEED,"Speed", _speedStrG, color,
                print,_linkUP);
        setPrintVal(_operatingInfoCmd,PDDR_WIDTH ,"Width",
                to_string(_numOfLanes) + "x", color, print,_linkUP);

        u_int32_t fec_mode = getFieldValue("fec_mode_active", _buffer);
        string fec_mode_str = "Unsupported FEC MODE";
        string msg_color = MlxlinkRecord::state2Color(0);
        if(fec_mode<FEC_MODE_END) {
            fec_mode_str = _mlxlinkMaps->_fecModeActive[fec_mode];
            msg_color = color;
        }
        setPrintVal(_operatingInfoCmd,PDDR_FEC , "FEC", fec_mode_str,
                msg_color,print, _linkUP);
        setPrintVal(_operatingInfoCmd,PDDR_LOOPBACK_MODE , "Loopback Mode",
                _mlxlinkMaps->_loopbackModeList[loopbackMode],
                getLoopbackColor(loopbackMode), true, print);
        setPrintVal(_operatingInfoCmd,PDDR_AUTO_NEGOTIATION , "Auto Negotiation",
                _mlxlinkMaps->_anDisableList[_anDisable] + _speedForce,
                getAnDisableColor(_anDisable), true, print);
        setPrintTitle(_supportedInfoCmd,"Supported Info",
                PDDR_SUPPORTED_INFO_LAST, print);
        u_int32_t speeds_mask = _protoAdminEx ? _protoAdminEx: _protoAdmin;
        string supported_speeds = SupportedSpeeds2Str(_protoActive, speeds_mask,
                (bool)_protoAdminEx);
        color = MlxlinkRecord::supported2Color(supported_speeds);
        stringstream value;
        value << "0x" << std::hex << setfill('0') << setw(8) << speeds_mask \
                << " (" << supported_speeds << ")" << setfill(' ');
        string title = "Enabled Link Speed";
        if(_protoAdminEx) {
            title += " (Ext.)";
        }
        setPrintVal(_supportedInfoCmd,PDDR_ENABLED_LINK_SPEED, title,
                speeds_mask? value.str() : "N/A",color, true, print);

        speeds_mask = _protoCapabilityEx ? _protoCapabilityEx: _protoCapability;
        supported_speeds = SupportedSpeeds2Str(_protoActive, speeds_mask,
                (bool)_protoCapabilityEx);
        color = MlxlinkRecord::supported2Color(supported_speeds);
        value.str("");
        value.clear();
        value << "0x" << std::hex << setfill('0') << setw(8) << speeds_mask \
                << " (" << supported_speeds << ")" << setfill(' ');

        title = "Supported Cable Speed";
        if(_protoCapabilityEx) {
            title += " (Ext.)";
        }
        setPrintVal(_supportedInfoCmd,PDDR_SUPPORTED_LINK_SPEED, title,
                value.str(),color, true, print);

        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        updateField("page_select", PDDR_TROUBLESHOOTING_INFO_PAGE);
        updateField("group_opcode", ADVANCED_OPCODE);

        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

        u_int32_t advancedOpcode = getFieldValue("advanced_opcode", _buffer);
        color = status2Color(advancedOpcode);
        setPrintTitle(_troubInfoCmd,"Troubleshooting Info",
                PDDR_TRUOBLESHOOTING_INFO_LAST, print);

        stringstream statusOp;
        statusOp << std::dec << advancedOpcode;
        setPrintVal(_troubInfoCmd,PDDR_STATUS_OPCODE ,"Status Opcode",
                statusOp.str(),color, true, print);

        string groupOpcode = getGroupStr(advancedOpcode);
        setPrintVal(_troubInfoCmd,PDDR_GROUP_OPCODE ,"Group Opcode",
                groupOpcode,color, true, print);

        string message = "";
        char txt[16], c;
        u_int32_t message_buf;
        bool finalize = false;

        for (int i = 0; i < PDDR_STATUS_MESSAGE_LENGTH_SWITCH; i++) {
            string path = "status_message[";
            sprintf(txt, "%d", i);
            path.append(txt);
            path.append("]");
            message_buf = getFieldValue(path, _buffer);
            for (int k = 24; k > -1; k -= 8) {
                c = (char)(message_buf >> k);
                message.push_back(c);
                if (c == '\0') {
                    finalize = true;
                    break;
                }
            }
            if (finalize == true) {
                break;
            }
        }
        setPrintVal(_troubInfoCmd,PDDR_RECOMMENDATION ,"Recommendation",
                message,color, true, print);
        if (_prbsTestMode) {
            showTestMode();
        }

        std::cout << _operatingInfoCmd;
        std::cout << _supportedInfoCmd;
        std::cout << _troubInfoCmd;

    } catch (const std::exception &exc) {
        throw MlxRegException(string("Showing PDDR raised the following exception: ") + string(exc.what()));
    }
}

void MlxlinkCommander::getPtys()
{
    string regName = "PTYS";
    resetParser(regName);
    updateField("local_port", _localPort);
    updateField("proto_mask", _protoActive);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    if(_protoActive == ETH) {
        _activeSpeedEx = getFieldValue("ext_eth_proto_oper", _buffer);
        _activeSpeed = getFieldValue("eth_proto_oper", _buffer);
        _protoCapabilityEx = getFieldValue("ext_eth_proto_capability", _buffer);
        _protoCapability = getFieldValue("eth_proto_capability", _buffer);
        _protoAdmin = getFieldValue("eth_proto_admin", _buffer);
        _protoAdminEx = getFieldValue("ext_eth_proto_admin", _buffer);
    }

    _anDisable = getFieldValue("an_disable_admin", _buffer);
    if (_anDisable) {
        u_int32_t val = _protoAdminEx ? _protoAdminEx : _protoAdmin;
        _speedForce = " - " + SupportedSpeeds2Str(_protoActive, val,
                (bool)_protoAdminEx);
    }
}

void MlxlinkCommander::showTestMode()
{
    std::map<std::string, std::string> pprtMap = getPprt();
    std::map<std::string, std::string> ppttMap = getPptt();

    setPrintTitle(_testModeInfoCmd,"Test Mode Info", TEST_MODE_INFO_LAST);
    setPrintVal(_testModeInfoCmd,TEST_MODE_PPRT_PRBS_MODE ,"RX PRBS Mode",
            pprtMap["pprtPrbsMode"]);
    setPrintVal(_testModeInfoCmd,TEST_MODE_PPTT_PRBS_MODE ,"TX PRBS Mode",
            ppttMap["ppttPrbsMode"]);
    setPrintVal(_testModeInfoCmd,TEST_MODE_PPRT_LANE_RATE ,"RX Lane Rate",
            pprtMap["pprtLaneRate"]);
    setPrintVal(_testModeInfoCmd,TEST_MODE_PPTT_LANE_RATE ,"TX Lane Rate",
            ppttMap["ppttLaneRate"]);
    setPrintVal(_testModeInfoCmd,TEST_MODE_PPRT_TUNING_STATUS ,"Tuning Status",
            pprtMap["pprtTuningStatus"]);
    setPrintVal(_testModeInfoCmd,TEST_MODE_PPRT_LOCK_STATUS ,"Lock Status",
            pprtMap["pprtLockStatus"]);

    cout << _testModeInfoCmd;
}

std::map<std::string, std::string> MlxlinkCommander::getPprt()
{
    string regName = "PPRT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    std::map<string, string> pprtMap;
    pprtMap["pprtPrbsMode"] = prbsMaskToMode(
        getFieldValue("prbs_mode_admin", _buffer));
    pprtMap["pprtLaneRate"] = prbsMaskToLaneRate(
        getFieldValue("lane_rate_oper", _buffer));
    pprtMap["pprtTuningStatus"] = prbsMaskToTuningStatus(
        getFieldValue("prbs_rx_tuning_status", _buffer));
    bool pcie = _userInput._pcie;
    u_int32_t numOfLanesToUse = (pcie) ? _numOfLanesPcie : _numOfLanes;
    pprtMap["pprtLockStatus"] = prbsMaskToLockStatus(
        getFieldValue("prbs_lock_status", _buffer), numOfLanesToUse);
    return pprtMap;
}

std::map<std::string, std::string> MlxlinkCommander::getPptt()
{
    string regName = "PPTT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    std::map<string, string> ppttMap;
    ppttMap["ppttPrbsMode"] = prbsMaskToMode(
        getFieldValue("prbs_mode_admin", _buffer));
    ppttMap["ppttLaneRate"] = prbsMaskToLaneRate(
        getFieldValue("lane_rate_admin", _buffer));

    return ppttMap;
}

void MlxlinkCommander::showMpcntTimers()
{
    string regName = "MPCNT";
    resetParser(regName);

    updateField("pcie_index", _localPort);
    updateField("grp", MPCNT_TIMERS_GROUPS);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    setPrintTitle(_mpcntTimerInfCmd, "Management PCIe Timers Counters Info",
            MPCNT_TIMER_INFO_LAST);
    setPrintVal(_mpcntTimerInfCmd, MPCNT_DL_DOWN ,"dl down",
            getFieldStr("dl_down"));

    cout << _mpcntTimerInfCmd;
}

void MlxlinkCommander::showBer()
{
    if (_userInput._pcie) {
        showMpcntTimers();
    }
    try {
        if (_prbsTestMode) {
            showTestModeBer();
            return;
        }
        if (_userInput._pcie) {
            showMpcntPerformance();
            return;
        }
        string regName = "PPCNT";
        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("swid", SWID);
        updateField("pnat", PNAT_LOCAL);
        updateField("grp", PPCNT_STATISTICAL_GROUP);
        updateField("clr", 0);
        updateField("prio_tc", 0);

        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

        setPrintTitle(_berInfoCmd,"Physical Counters and BER Info", BER_INFO_LAST);

        char buff[64];
        float val = (float)add32BitTo64(
                getFieldValue("time_since_last_clear_high", _buffer),
                getFieldValue("time_since_last_clear_low", _buffer))/60000.0;
        sprintf(buff, "%.01f", val);
        setPrintVal(_berInfoCmd,BER_TIME_SINCE_LAST_CLEAR ,"Time Since Last Clear [Min]",
                buff, ANSI_COLOR_RESET,true,_linkUP);
        setPrintVal(_berInfoCmd,BER_EFFECTIVE_PHYSICAL_ERRORS, "Effective Physical Errors",
                to_string(add32BitTo64(getFieldValue("phy_symbol_errors_high", _buffer),
                        getFieldValue("phy_symbol_errors_low", _buffer))), ANSI_COLOR_RESET,true,_linkUP);

        std::vector<string> rawErrorsPerLane;
        string phy_raw_err = "";
        for (u_int32_t lane = 0; lane < _numOfLanes; lane++) {
            string laneStr = to_string(lane);
            phy_raw_err = to_string(add32BitTo64(getFieldValue("phy_raw_errors_lane" + laneStr + "_high", _buffer),
                                                 getFieldValue("phy_raw_errors_lane" + laneStr + "_low", _buffer)));
            rawErrorsPerLane.push_back(phy_raw_err);
        }
        setPrintVal(_berInfoCmd,BER_RAW_PHYSICAL_ERRORS_PER_LANE , "Raw Physical Errors Per Lane",
                getStringFromVector(rawErrorsPerLane), ANSI_COLOR_RESET,true,_linkUP);
        setPrintVal(_berInfoCmd,BER_EFFECTIVE_PHYSICAL_BER ,"Effective Physical BER",
                getFieldStr("effective_ber_coef") + "E-" + getFieldStr("effective_ber_magnitude"), ANSI_COLOR_RESET,true,_linkUP);
        setPrintVal(_berInfoCmd,BER_RAW_PHYSICAL_BER , "Raw Physical BER",
                getFieldStr("raw_ber_coef") + "E-" + getFieldStr("raw_ber_magnitude"), ANSI_COLOR_RESET,true,_linkUP);

        cout << _berInfoCmd;
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing BER via PPCNT raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showTestModeBer()
{
    string regName = "PPCNT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("swid", SWID);
    updateField("pnat", PNAT_LOCAL);
    updateField("grp", PPCNT_STATISTICAL_GROUP);
    updateField("clr", 0);
    updateField("prio_tc", 0);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    std::vector<string> errors;
    for (u_int32_t lane = 0; lane < _numOfLanes; lane++) {
        errors.push_back(to_string(add32BitTo64(getFieldValue("phy_raw_errors_lane" + to_string(lane) + "_high", _buffer),
                                                getFieldValue("phy_raw_errors_lane" + to_string(lane) + "_low", _buffer))));
    }

    char buff[64];
    float val = (float)add32BitTo64(
            getFieldValue("time_since_last_clear_high", _buffer),
            getFieldValue("time_since_last_clear_low", _buffer))/60000.0;
    sprintf(buff, "%.01f", val);

    setPrintTitle(_testModeBerInfoCmd,
            "Physical Counters and BER Info (PRBS)", TEST_MODE_BER_INFO_LAST);
    setPrintVal(_testModeBerInfoCmd,TEST_MODE_BER_TIME_SINCE_LAST_CLEAR ,
            "Time Since Last Clear [Min]",buff);
    setPrintVal(_testModeBerInfoCmd,TEST_MODE_BER_PRBS_ERRORS ,
            "PRBS Errors",getStringFromVector(errors));
    setPrintVal(_testModeBerInfoCmd,TEST_MODE_BER ,  "PRBS BER",
            getFieldStr("raw_ber_coef") + "E-" + getFieldStr("raw_ber_magnitude"));

    cout <<_testModeBerInfoCmd;
}

void MlxlinkCommander::showMpcntPerformance()
{
    string regName = "MPCNT";
    resetParser(regName);

    updateField("pcie_index", _localPort);
    updateField("grp", MPCNT_PERFORMANCE_GROUP);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    setPrintTitle(_mpcntPerfInfCmd,
            "Management PCIe Performance Counters Info", MPCNT_PERFORMANCE_INFO_LAST);
    setPrintVal(_mpcntPerfInfCmd,MPCNT_RX_ERRORS ,
            "RX Errors",getFieldStr("rx_errors"));
    setPrintVal(_mpcntPerfInfCmd,MPCNT_TX_ERRORS ,
            "TX Errors",getFieldStr("tx_errors"));
    setPrintVal(_mpcntPerfInfCmd,MPCNT_CRC_ERROR_DLLP ,
            "CRC Error dllp",getFieldStr("crc_error_dllp"));
    setPrintVal(_mpcntPerfInfCmd,MPCNT_CRC_ERROR_TLP ,
            "CRC Error tlp",getFieldStr("crc_error_tlp"));

    cout << _mpcntPerfInfCmd;
}

void MlxlinkCommander::showEye()
{
    try {
        string regName = "SLRG";
        resetParser(regName);

        bool pcie = _userInput._pcie;
        std::vector<string> physicalGrades;
        std::vector<string> heightLengths;
        std::vector<string> phaseWidths;
        u_int32_t numOfLanesToUse = (pcie) ? _numOfLanesPcie : _numOfLanes;

        for (u_int32_t i = 0; i < numOfLanesToUse; i++) {
            updateField("local_port", _localPort);
            updateField("pnat", (pcie) ? PNAT_PCIE : PNAT_LOCAL);
            updateField("lane", i);

            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

            physicalGrades.push_back(MlxlinkRecord::addSpaceForSlrg(
                    to_string((getFieldValue("grade", _buffer)))));
            int version = getVersion(getFieldValue("version", _buffer));
            string height_eo_pos("height_eo_pos");
            string height_eo_neg("height_eo_neg");
            string phase_eo_pos("phase_eo_pos");
            string phase_eo_neg("phase_eo_neg");
            if (version == PRODUCT_16NM) {
                height_eo_pos.append("_mid");
                height_eo_neg.append("_mid");
                phase_eo_pos.append("_mid");
                phase_eo_neg.append("_mid");
            }
            int offsetEOPos = getHeight(getFieldValue(height_eo_pos, _buffer));
            int offsetEONeg = getHeight(getFieldValue(height_eo_neg, _buffer));
            string offsetEONegStr = to_string(offsetEOPos + offsetEONeg);
            heightLengths.push_back(MlxlinkRecord::addSpaceForSlrg(offsetEONegStr));
            int phaseEOPos = getPhase(getFieldValue(phase_eo_pos, _buffer));
            int phaseEONeg = getPhase(getFieldValue(phase_eo_neg, _buffer));
            string phaseEONegStr = to_string(phaseEOPos + phaseEONeg);
            phaseWidths.push_back(MlxlinkRecord::addSpaceForSlrg(phaseEONegStr));
            resetParser(regName);
        }

        string showEyeTitle = "EYE Opening Info";
        if (pcie) {
            showEyeTitle += " (PCIe)";
        }

        setPrintTitle(_eyeOpeningInfoCmd, showEyeTitle, EYE_OPENING_INFO_LAST);
        setPrintVal(_eyeOpeningInfoCmd, EYE_PHYSICAL_GRADE,
                "Physical Grade", getStringFromVector(physicalGrades),
                ANSI_COLOR_RESET,true,_linkUP || _prbsTestMode || _numOfLanesPcie);
        setPrintVal(_eyeOpeningInfoCmd, EYE_HEIGHT_EYE_OPENING,
                "Height Eye Opening [mV]", getStringFromVector(heightLengths),
                ANSI_COLOR_RESET,true,_linkUP || _prbsTestMode || _numOfLanesPcie);
        setPrintVal(_eyeOpeningInfoCmd, EYE_PHASE_EYE_OPENING,
                "Phase  Eye Opening [psec]", getStringFromVector(phaseWidths),
                ANSI_COLOR_RESET,true,_linkUP || _prbsTestMode || _numOfLanesPcie);

        cout << _eyeOpeningInfoCmd;

    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing Eye via SLRG raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showFEC()
{
    try {
        /*string regName = "PPLM";
           //std::vector<u_int32_t> _buffer;
           //RegAccessParser parser = getRegAccessParser(regName);
           //resetParser(regName);

           updateField("local_port", _localPort);

           genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
           u_int32_t FECSupported100G = getFieldValue("fec_override_cap_100g", _buffer);
           u_int32_t FECSupported25G50G = getFieldValue("fec_override_cap_50g", _buffer);
           u_int32_t FECAdmin100G = getFieldValue("fec_override_admin_100g", _buffer);
           u_int32_t FECAdmin25G50G = getFieldValue("fec_override_admin_50g", _buffer);

           u_int32_t FEC100G = (FECAdmin100G) ? FECAdmin100G : FECSupported100G;
           u_int32_t FEC25G50G = (FECAdmin25G50G) ? FECAdmin25G50G : FECSupported25G50G;

           std::vector<string> showFecValues;
           showFecValues.push_back("0x" + convertIntToHexString(FEC100G) + " (" + FEC2Str100G(FEC100G) + ")");
           showFecValues.push_back("0x" + convertIntToHexString(FEC25G50G) + " (" + FEC2Str50G25G(FEC25G50G) + ")");
           showFecValues.push_back("0x" + convertIntToHexString(_fecModeRequest) + " (" + FECReq2Str(_fecModeRequest, _linkUP) + ")");
           printCmdOutput(_showFecTitle, _showFecLines, showFecValues);*/
        string fecCap100,fecCap50,fecCap40,fecCap25,fecCap10;
        std::vector<string> showFecValues;
        if (_protoActive == IB || _prbsTestMode) {
            fecCap100 = "0x0 (N/A)";
            fecCap50 = "0x0 (N/A)";
            fecCap40 = "0x0 (N/A)";
            fecCap25 = "0x0 (N/A)";
            fecCap10 = "0x0 (N/A)";
        } else {
            string supportedSpeeds = EthSupportedSpeeds2Str(getPtysCap());
            if (supportedSpeeds.find("100G") != string::npos) {
                if (_devID == DeviceConnectX4) {
                    fecCap100 = "0x4 (RS-FEC(528,514))";
                } else {
                    fecCap100 = "0x5 (No-FEC,RS-FEC(528,514))";
                }
            } else {
                showFecValues.push_back("0x0 (N/A)");
            }
            if (supportedSpeeds.find("50G") != string::npos) {
                fecCap50 = "0x7 (No-FEC,FireCode FEC,RS-FEC(528,514))";
            } else {
                fecCap50 = "0x0 (N/A)";
            }
            if (supportedSpeeds.find("40G") != string::npos) {
                fecCap40 = "0x3 (No-FEC,FireCode FEC)";
            } else {
                fecCap40 = "0x0 (N/A)";
            }
            if (supportedSpeeds.find("25G") != string::npos) {
                fecCap25 = "0x7 (No-FEC,FireCode FEC,RS-FEC(528,514))";
            } else {
                fecCap25 = "0x0 (N/A)";
            }
            if (supportedSpeeds.find("10G") != string::npos) {
                fecCap10 = "0x3 (No-FEC,FireCode FEC)";
            } else {
                fecCap10 = "0x0 (N/A)";
            }
        }

        setPrintTitle(_fecCapInfoCmd, "FEC Capability Info", FEC_CAP_INFO_LASE);
        setPrintVal(_fecCapInfoCmd, FEC_CAP_100G, "FEC Capability 100GbE",fecCap100);
        setPrintVal(_fecCapInfoCmd, FEC_CAP_50G, "FEC Capability 50GbE", fecCap50);
        setPrintVal(_fecCapInfoCmd, FEC_CAP_40G, "FEC Capability 40GbE", fecCap40);
        setPrintVal(_fecCapInfoCmd, FEC_CAP_25G, "FEC Capability 25GbE", fecCap25);
        setPrintVal(_fecCapInfoCmd, FEC_CAP_10G, "FEC Capability 10GbE", fecCap10);

        cout << _fecCapInfoCmd;

    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing FEC raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showSltp()
{
    try {
        string regName = "SLTP";
        resetParser(regName);

        bool valid = true;
        u_int32_t numOfLanesToUse =
            (_userInput._pcie) ? _numOfLanesPcie : _numOfLanes;
        std::vector<std::vector<string> > sltpLanes(numOfLanesToUse,
                                                    std::vector<string>());
        string showSltpTitle = "Serdes Tuning Transmitter Info";
        if (_userInput._pcie) {
            showSltpTitle += " (PCIe)";
        }
        setPrintTitle(_sltpInfoCmd, showSltpTitle, numOfLanesToUse+1);
        if (_productTechnology == PRODUCT_16NM &&
               !_userInput._advancedMode) {
            throw MlxRegException(
                    "Showing SLTP for 16nm technology is supported only "
                    "in advanced mode");
        }
        if (_productTechnology == PRODUCT_16NM) {
            _mlxlinkMaps->_sltpHeader = "pre2Tap,preTap,mainTap,postTap,m2lp,amp,alevOut";
        } else {
            _mlxlinkMaps->_sltpHeader = "Pol,tap0,tap1,tap2,bias,preemp_mode";
            if (_userInput._advancedMode) {
                _mlxlinkMaps->_sltpHeader += ",reg,leva";
            }
        }
        if (!_linkUP && !_userInput._pcie && !_prbsTestMode) {
            setPrintVal(_sltpInfoCmd,0, "", _mlxlinkMaps->_sltpHeader,ANSI_COLOR_RESET, true,false);
            return;
        }
        setPrintVal(_sltpInfoCmd,0, "", _mlxlinkMaps->_sltpHeader);
        for (u_int32_t i = 0; i < numOfLanesToUse; i++) {
            updateField("local_port", _localPort);
            updateField("pnat", (_userInput._pcie) ? PNAT_PCIE : PNAT_LOCAL);
            updateField("lane", i);
            updateField("c_db", _userInput._db);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
            if (_productTechnology == PRODUCT_16NM) {
                prepareSltp16nm(sltpLanes, i);
            } else {
                prepareSltp28_40nm(sltpLanes, i);
            }
            if (!getFieldValue("status", _buffer)) {
                valid = false;
            }
            resetParser(regName);
            setPrintVal(_sltpInfoCmd,(int)i + 1,
                    "Lane " + to_string(i),getStringFromVector(sltpLanes[i]),ANSI_COLOR_RESET, true,valid);

        }
        cout << _sltpInfoCmd;
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing SLTP raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showDeviceData()
{
    try {
        std::vector<string> showDeviceDataValues;
        string regName = "MSGI";
        bool success = false;
        try {
            resetParser(regName);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
            success = true;
        } catch (...) { }
        setPrintTitle(_showDeviceInfoCmd, "Device Info", DEVICE_INFO_LAST);
        setPrintVal(_showDeviceInfoCmd,DEVICE_PART_NUMBER, "Part Number", success ? getDevicePN() : "N/A");
        setPrintVal(_showDeviceInfoCmd,DEVICE_PART_NAME,"Part Name", success ? getDeviceProductName() : "N/A");
        setPrintVal(_showDeviceInfoCmd,DEVICE_SERIAL_NUMBER, "Serial Number", success ? getDeviceSN() : "N/A");
        setPrintVal(_showDeviceInfoCmd,DEVICE_REVISION, "Revision", success ? getDeviceRev() : "N/A");
        setPrintVal(_showDeviceInfoCmd,DEVICE_FW_VERSION, "FW Version", getDeviceFW());

        cout << _showDeviceInfoCmd;

        if (dm_dev_is_hca(_devID)) {
            MlxlinkRecord::printWar("Note: P/N, Product Name, S/N and Revision are supported only in switches");
        }

    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing Device Data via MSGI raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showBerMonitorInfo()
{

    try {
        if (dm_dev_is_hca(_devID)) {
            throw MlxRegException("\"--" BER_MONITOR_INFO_FLAG "\" option is not supported for HCA");
        }
        string regName = "PPBMC";
        string monitor_state = "N/A";
        string monitor_type = "N/A";
        string alarm_th = "N/A";
        string warning_th = "N/A";
        string normal_th = "N/A";
        resetParser(regName);
        updateField("local_port", _localPort);
        updateField("pnat", PNAT_LOCAL);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

        monitor_state = _mlxlinkMaps->_ppbmcBerMonitorState[getFieldValue("monitor_state", _buffer)];
        u_int32_t monitor_type_int = getFieldValue("monitor_type", _buffer);
        monitor_type = _mlxlinkMaps->_ppbmcBerMonitorType[monitor_type_int];

        /* TODO - To Remove comment for next version
         * After alarm_th_mantissa, alarm_th_exp,
         * warning_th_mantissa, warning_th_exp,
         * normal_th_mantissa and normal_th_exp will be available in adabe in PPBMP
         */
        /*
           regName = "PPBMP";
           resetParser(regName);
           updateField("local_port", _localPort);
           updateField("pnat", PNAT_LOCAL);
           updateField("monitor_group", monitor_type_int);
           genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

           alarm_th    = convertToString(getFieldValue("alarm_th_mantissa", _buffer));
           alarm_th   += "E-" + convertToString(getFieldValue("alarm_th_exp", _buffer));
           warning_th  = convertToString(getFieldValue("warning_th_mantissa", _buffer));
           warning_th += "E-" + convertToString(getFieldValue("warning_th_exp", _buffer));
           normal_th   = convertToString(getFieldValue("normal_th_mantissa", _buffer));
           normal_th  += "E-" + convertToString(getFieldValue("normal_th_exp", _buffer));
         */
        setPrintTitle(_showBerMonitorInfo, "BER Monitor Info", BER_MONITOR_INFO_LAST);
        setPrintVal(_showBerMonitorInfo,BER_MONITOR_STATE, "BER Monitor State", monitor_state);
        setPrintVal(_showBerMonitorInfo,BER_MONITOR_TYPE, "BER Monitor Type", monitor_type);

        /* TODO - To Remove comment for next version
         * After alarm_th_mantissa, alarm_th_exp,
         * warning_th_mantissa, warning_th_exp,
         * normal_th_mantissa and normal_th_exp will be available in adabe in PPBMP
         */
        //showBerMonitorValues.push_back(alarm_th);
        //showBerMonitorValues.push_back(warning_th);
        //showBerMonitorValues.push_back(normal_th);
        cout << _showBerMonitorInfo;
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing BER Monitor via PPBMC raised the following exception: ") + string(exc.what()) + string("\n");
        /* TODO - To replace _allUnhandledErrors assignment with the below assignment
         * After alarm_th_mantissa, alarm_th_exp,
         * warning_th_mantissa, warning_th_exp,
         * normal_th_mantissa and normal_th_exp will be available in adabe in PPBMP
         */
        //_allUnhandledErrors += string("Showing BER Monitor via PPBMC and PPBMP raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showExternalPhy()
{
    try {
        if (dm_dev_is_hca(_devID)) {
                throw MlxRegException("\"--" PEPC_SHOW_FLAG "\" option is not supported for HCA");
        }
        string regName = "PEPC";
        string twisted_pair_an = "N/A";
        string twisted_pair_force_mode = "N/A";
        string twisted_pair_an_mode = "N/A";
        resetParser(regName);
        updateField("local_port", _localPort);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        twisted_pair_force_mode = _mlxlinkMaps->_pepcTwistedPairForceMode[getFieldValue("twisted_pair_force_mode", _buffer)];
        setPrintTitle(_extPhyInfoCmd, "External PHY Info", EXT_PHY_INFO_INFO_LAST);
        setPrintVal(_extPhyInfoCmd,EXT_PHY_TWISTED_PAIR_FORCE_MODE,
                "Twisted Pair Force Mode", twisted_pair_force_mode);
        cout << _extPhyInfoCmd;
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing External PHY via PEPC raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showPcie()
{
    try {
        showPcieState();
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Showing Pcie via MPEIN raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::showPcieState()
{
    string regName = "MPEIN";
    resetParser(regName);

    updateField("pcie_index", _localPort);
    updateField("depth", _depth);
    updateField("node", _node);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    string linkSpeedEnabled = " ("
                              + pcieSpeedStr(getFieldValue("link_speed_enabled", _buffer)) + ")";
    string linkWidthEnabled = " ("
                              + to_string((getFieldValue("link_width_enabled", _buffer)))
                              + "X)";
    _numOfLanesPcie = getFieldValue("link_width_active", _buffer);

    setPrintTitle(_pcieInfoCmd,"PCIe Operational (Enabled) Info" , PCIE_INFO_LAST);
    setPrintVal(_pcieInfoCmd, PCIE_LINK_SPEED_ACTIVE_ENABLED,
            "Link Speed Active (Enabled)",
            pcieSpeedStr(getFieldValue("link_speed_active", _buffer))+ linkSpeedEnabled);
    setPrintVal(_pcieInfoCmd, LINK_WIDTH_ACTIVE_ENABLED,
            "Link Width Active (Enabled)",
            to_string(_numOfLanesPcie) + "X" + linkWidthEnabled);
    setPrintVal(_pcieInfoCmd, DEVICE_STATUS, "Device Status",
            getFieldValue("device_status", _buffer)?pcieDeviceStatusStr(getFieldValue("device_status", _buffer)):"N/A");

    cout << _pcieInfoCmd;
}

void MlxlinkCommander::collectBER()
{
    try {
        const char *fileName = _userInput._csvBer.c_str();
        ifstream ifile(fileName);
        ofstream berFile;

        if (!ifile) {
            berFile.open(fileName, std::ofstream::app);
            berFile << _mlxlinkMaps->_berCollectTitle << endl;
        } else {
            berFile.open(fileName, std::ofstream::app);
        }

        cout << endl << "Collecting BER and producing report to "
             << _userInput._csvBer << " ..." << endl;

        string active_fec = _prbsTestMode ? "N/A" : _mlxlinkMaps->_fecModeActive[_fecActive];
        findAndReplace(active_fec, ",", " ");
        string devicePN = getDevicePN(true);
        string deviceFW = getDeviceFW();
        getCableParams();
        u_int32_t linkDown = getLinkDown();
        std::map<std::string, float>  errorsVector = getRawEffectiveErrors();
        float rawBerLimit = getRawBERLimit();
        string result =
            (getResult(errorsVector, rawBerLimit, linkDown)) ?
            "Pass" : "Fail";
        string protocol = _prbsTestMode ? getPrbsModeRX() : _speedStrG;
        u_int32_t speed = _prbsTestMode ? getPrbsRateRX() : _speedBerCsv;
        if (_protoActive == IB) {
            speed *= _numOfLanes / MAX_LANES_NUMBER;
        }
        berFile << _userInput._testMode << ',' << protocol << ','
                << speed << ',' << active_fec << ',' << _userInput._iteration << ','
                << devicePN << ',' << deviceFW << ',' << '1' << ','
                << _userInput._labelPort << ','
                << getCableMedia(_cableMediaType) << ','
                << _cablePN << ','
                << _cableLen << ','
                << _cableAtten12G << ','
                << errorsVector["time_since_last_clear_sec"] / 60 << ','
                << linkDown
                << ',' << errorsVector["raw_ber"] << ',' << rawBerLimit << ','
                << errorsVector["eff_errors_counter"] << ','
                << errorsVector["eff_ber"] << ',' << result << ','
                << "N/A,N/A,N/A," << _moduleTemp << ',' << _moduleTemp << ",N/A,N/A," << _cableSN << ",N/A";
        berFile << endl;
        berFile.close();
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Collecting BER and producing report raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

int MlxlinkCommander::getLinkDown()
{
    string regName = "PPCNT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("swid", SWID);
    updateField("pnat", PNAT_LOCAL);
    updateField("grp", PPCNT_PHY_GROUP);
    updateField("clr", 0);
    updateField("prio_tc", 0);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    return getFieldValue("link_down_events", _buffer);
}

std::map<std::string, float>  MlxlinkCommander::getRawEffectiveErrorsinTestMode()
{
    string regName = "PPCNT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("swid", SWID);
    updateField("pnat", PNAT_LOCAL);
    updateField("grp", PPCNT_PHY_GROUP);
    updateField("clr", 0);
    updateField("prio_tc", 0);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    std::map<string, float> errorsVector;
    errorsVector["phy_corrected_bits_lane0"] = add32BitTo64(getFieldValue(
                                                                "edpl_bip_errors_lane0_high", _buffer),
                                                            getFieldValue("edpl_bip_errors_lane0_low", _buffer));
    errorsVector["phy_corrected_bits_lane1"] = add32BitTo64(getFieldValue(
                                                                "edpl_bip_errors_lane1_high", _buffer),
                                                            getFieldValue("edpl_bip_errors_lane1_low", _buffer));
    errorsVector["phy_corrected_bits_lane2"] = add32BitTo64(getFieldValue(
                                                                "edpl_bip_errors_lane2_high", _buffer),
                                                            getFieldValue("edpl_bip_errors_lane2_low", _buffer));
    errorsVector["phy_corrected_bits_lane3"] = add32BitTo64(getFieldValue(
                                                                "edpl_bip_errors_lane3_high", _buffer),
                                                            getFieldValue("edpl_bip_errors_lane3_low", _buffer));
    errorsVector["time_since_last_clear_sec"] = (add32BitTo64(getFieldValue(
                                                                  "time_since_last_clear_high", _buffer),
                                                              getFieldValue("time_since_last_clear_low", _buffer))) / 1000;

    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("swid", SWID);
    updateField("pnat", PNAT_LOCAL);
    updateField("grp", PPCNT_STATISTICAL_GROUP);
    updateField("clr", 0);
    updateField("prio_tc", 0);

    float effBerCoef = 0, effBerMag = 0, rawBerCoef = 0, rawBerMag = 0;
    float raw_ber = 0, eff_ber = 0;
    int raw_errors_counter = 0, eff_errors_counter = 0;
    eff_errors_counter = add32BitTo64(getFieldValue("phy_symbol_errors_high", _buffer),
                                      getFieldValue("phy_symbol_errors_low", _buffer));
    raw_errors_counter = add32BitTo64(getFieldValue("phy_corrected_bits_high", _buffer),
                                      getFieldValue("phy_corrected_bits_low", _buffer));
    rawBerCoef = getFieldValue("raw_ber_coef", _buffer);
    rawBerMag = getFieldValue("raw_ber_magnitude", _buffer);
    effBerCoef = getFieldValue("effective_ber_coef", _buffer);
    effBerMag = getFieldValue("effective_ber_magnitude", _buffer);
    raw_ber = rawBerCoef * std::pow(10, -rawBerMag);
    eff_ber = effBerCoef * std::pow(10, -effBerMag);

    errorsVector["raw_errors_counter"] = raw_errors_counter;
    errorsVector["raw_ber"] = raw_ber;
    errorsVector["eff_errors_counter"] = eff_errors_counter;
    errorsVector["eff_ber"] = eff_ber;

    return errorsVector;
}

std::map<string, float> MlxlinkCommander::getRawEffectiveErrors()
{
    if (_prbsTestMode) {
        return getRawEffectiveErrorsinTestMode();
    }
    string regName = "PPCNT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("swid", SWID);
    updateField("pnat", PNAT_LOCAL);
    updateField("grp", PPCNT_STATISTICAL_GROUP);
    updateField("clr", 0);
    updateField("prio_tc", 0);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    std::map<string, float> errorsVector;
    errorsVector["time_since_last_clear_sec"] = (add32BitTo64(getFieldValue(
                                                                  "time_since_last_clear_high", _buffer),
                                                              getFieldValue("time_since_last_clear_low", _buffer))) / 1000;

    float effBerCoef = 0, effBerMag = 0, rawBerCoef = 0, rawBerMag = 0;
    float raw_ber = 0, eff_ber = 0;
    int raw_errors_counter = 0, eff_errors_counter = 0;
    eff_errors_counter = add32BitTo64(getFieldValue("phy_symbol_errors_high", _buffer),
                                      getFieldValue("phy_symbol_errors_low", _buffer));
    raw_errors_counter = add32BitTo64(getFieldValue("phy_corrected_bits_high", _buffer),
                                      getFieldValue("phy_corrected_bits_low", _buffer));
    rawBerCoef = getFieldValue("raw_ber_coef", _buffer);
    rawBerMag = getFieldValue("raw_ber_magnitude", _buffer);
    effBerCoef = getFieldValue("effective_ber_coef", _buffer);
    effBerMag = getFieldValue("effective_ber_magnitude", _buffer);
    raw_ber = rawBerCoef * std::pow(10, -rawBerMag);
    eff_ber = effBerCoef * std::pow(10, -effBerMag);

    errorsVector["raw_errors_counter"] = raw_errors_counter;
    errorsVector["raw_ber"] = raw_ber;
    errorsVector["eff_errors_counter"] = eff_errors_counter;
    errorsVector["eff_ber"] = eff_ber;

    return errorsVector;
}

string MlxlinkCommander::getPrbsModeRX()
{
    string regName = "PPRT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    return prbsMaskToMode(getFieldValue("prbs_mode_admin", _buffer));
}

u_int32_t MlxlinkCommander::getPrbsRateRX()
{
    string regName = "PPRT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    return prbsMaskToRateNum(getFieldValue("lane_rate_oper", _buffer));
}

float MlxlinkCommander::getRawBERLimit()
{
    float limit;
    if (_userInput._testMode == "NOMINAL"
        || _userInput._testMode == "Nominal") {
        switch (_speedBerCsv) {
        case 25:
        case 50:
        case 100:
            if (_mlxlinkMaps->_fecModeActive[_fecActive] == _mlxlinkMaps->_fecModeActive[0]) {
                limit = pow(10.0, NOMINAL_25G_NO_FEC);
            } else if (_cableAtten12G <= 15) {
                limit = pow(10.0, NOMINAL_25G_LOW_ATTN);
            } else {
                limit = pow(10.0, NOMINAL_25G_HIGH_ATTN);
            }
            break;

        case 10:
            limit = pow(10.0, NOMINAL_10G);
            break;

        case 56:
        case 40:
            limit = pow(10.0, NOMINAL_56G_40G);
            break;

        default:
            limit = pow(10.0, NOMINAL_DEFAULT);
        }
    } else {
        switch (_speedBerCsv) {
        case 25:
        case 50:
        case 100:
            if (_mlxlinkMaps->_fecModeActive[_fecActive] == _mlxlinkMaps->_fecModeActive[0]) {
                limit = 3.0 * pow(10.0, CORNER_25G_NO_FEC);
            } else if (_cableAtten12G <= 15) {
                limit = pow(10.0, CORNER_25G_LOW_ATTN);
            } else {
                limit = pow(10.0, CORNER_25G_HIGH_ATTN);
            }
            break;

        case 10:
            limit = pow(10.0, CORNER_10G);
            break;

        case 56:
        case 40:
            limit = pow(10.0, CORNER_56G_40G);
            break;

        default:
            limit = pow(10.0, CORNER_DEFAULT);
        }
    }
    return limit;
}

bool MlxlinkCommander::getResult(std::map<std::string, float>  errorsVector, float rawBerLimit, int linkDown)
{
    float timeSinceLastClearSec = errorsVector["time_since_last_clear_sec"];
    if (timeSinceLastClearSec == 0
        || 1 / (timeSinceLastClearSec * _speedBerCsv * pow(10.0, 9))
        > getRawBERLimit()) {
        return false;
    }
    if (errorsVector["raw_ber"] < rawBerLimit
        && errorsVector["eff_ber"] < rawBerLimit && linkDown == 0) {
        return true;
    }
    return false;
}

string MlxlinkCommander::getDevicePN(bool queryMSGI)
{
    if (queryMSGI) {
        string regName = "MSGI";
        try {
            resetParser(regName);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (...) {
            return "N/A";
        }
    }
    return getAscii("part_number", 20);
}

string MlxlinkCommander::getDeviceSN(bool queryMSGI)
{
    if (queryMSGI) {
        string regName = "MSGI";
        try {
            resetParser(regName);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (...) {
            return "N/A";
        }
    }
    return getAscii("serial_number", 24);
}

string MlxlinkCommander::getDeviceProductName(bool queryMSGI)
{
    if (queryMSGI) {
        string regName = "MSGI";
        try {
            resetParser(regName);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (...) {
            return "N/A";
        }
    }
    return getAscii("product_name", 64);
}

string MlxlinkCommander::getDeviceRev(bool queryMSGI)
{
    if (queryMSGI) {
        string regName = "MSGI";
        try {
            resetParser(regName);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        } catch (...) {
            return "N/A";
        }
    }
    return getVendorRev("revision");
}

string MlxlinkCommander::getDeviceFW()
{
    return _fwVersion;
}

string MlxlinkCommander::getVendorRev(const string & name)
{
    string value = "";
    u_int32_t rev = getFieldValue(name, _buffer);
    u_int32_t shift = 0xFF000000;
    for (u_int32_t i = 0; i < 4; i++) {
        char c = (char) ((rev & shift) >> (3 - i) * 8);
        if (c != 0 && c != 32) {
            value.push_back(c);
        }
        shift = shift >> 8;
    }
    return (value != "") ? value : "N/A";
}

// Config functions

void MlxlinkCommander::clearCounters()
{
    try {
        MlxlinkRecord::printCmdLine("Clearing Counters");

        string regName = "PPCNT";
        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("swid", SWID);
        updateField("pnat", PNAT_LOCAL);
        updateField("grp", PPCNT_STATISTICAL_GROUP);
        updateField("prio_tc", 0);
        updateField("clr", 1);

        genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Clearing counters via PPCNT raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::sendPaosCmd(PAOS_ADMIN adminStatus)
{
    string regName = "PAOS";
    resetParser(regName);

    updateField("swid", SWID);
    updateField("local_port", _localPort);
    updateField("admin_status", adminStatus);
    updateField("ase", 1);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
}

void MlxlinkCommander::sendPaos()
{
    try {
        PAOS_CMD paosCmd = paos_to_int(_userInput._paosCmd);
        switch (paosCmd) {
        case UP:
            sendPaosUP();
            return;

        case DN:
            sendPaosDown();
            return;

        case TG:
            sendPaosToggle();
            return;

        case NO:
        default:
            return;
        }
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Sending PAOS raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::sendPaosDown()
{
    MlxlinkRecord::printCmdLine("Configuring Port State (Down)");
    sendPaosCmd(PAOS_DOWN);
    if (!checkPaosDown()) {
        if (dm_dev_is_hca(_devID)) {
            string protocol = (_protoActive == IB) ? "IB" : "ETH";
            throw MlxRegException("Port is not Down, please run mstconfig -d " + _device + " set KEEP_" + protocol + "_LINK_UP_P<port_number>=0.\nIn case of multi-host please verify all hosts are not holding the port up.");
        } else {
            throw MlxRegException("Port is not Down, Aborting...");
        }
    }
}

void MlxlinkCommander::sendPaosUP()
{
    MlxlinkRecord::printCmdLine("Configuring Port State (Up)");
    sendPaosCmd(PAOS_UP);
}

void MlxlinkCommander::sendPaosToggle()
{
    sendPaosDown();
    sendPaosUP();
}

void MlxlinkCommander::handlePrbs()
{
    try {
        if (_userInput._prbsMode == "EN") {
            if (!_plugged) {
                throw MlxRegException("Cannot Enter Physical Test Mode when Cable is Unplugged");
            }
            checkPprtCap();
            checkPpttCap();
            if (_prbsTestMode) {
                sendPrbsPpaos(false);
            }
            sendPaosDown();
            sendPprt();
            sendPptt();
            sendPrbsPpaos(true);
            MlxlinkRecord::printCmdLine("Configuring Port to Physical Test Mode");
        } else if (_userInput._prbsMode == "DS") {
            MlxlinkRecord::printCmdLine("Configuring Port to Regular Operation");
            if (_prbsTestMode) {
                sendPrbsPpaos(false);
            }
            sendPaosToggle();
        } else if (_userInput._prbsMode == "TU") {
            if (_prbsTestMode) {
                MlxlinkRecord::printCmdLine("Performing Tuning");
                startTuning();
            } else {
                throw MlxRegException(
                          "Port is not configured for physical test mode. Please see help");
            }
        }
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Handling PRBS raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::checkPprtCap()
{
    string regName = "PPRT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    if (!(prbsLaneRateCapToMask(_userInput._pprtRate) & getFieldValue("lane_rate_cap", _buffer))) {
        throw MlxRegException("Device does not support lane rate " + _userInput._pprtRate + " in physical test mode.");
    }
    if (!(prbsModeCapToMask(_userInput._pprtMode) & getFieldValue("prbs_modes_cap", _buffer))) {
        throw MlxRegException("Device does not support " + _userInput._pprtMode + " in physical test mode.");
    }
}

void MlxlinkCommander::checkPpttCap()
{
    string regName = "PPTT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    if (!(prbsLaneRateCapToMask(_userInput._ppttRate) & getFieldValue("lane_rate_cap", _buffer))) {
        throw MlxRegException("Device does not support lane rate " + _userInput._ppttRate + " in physical test mode.");
    }
    if (!(prbsModeCapToMask(_userInput._ppttMode) & getFieldValue("prbs_modes_cap", _buffer))) {
        throw MlxRegException("Device does not support " + _userInput._ppttMode + " in physical test mode.");
    }
}

void MlxlinkCommander::sendPrbsPpaos(bool testMode)
{
    string regName = "PPAOS";
    resetParser(regName);

    updateField("swid", SWID);
    updateField("local_port", _localPort);
    updateField("phy_test_mode_admin",
                (testMode ? PPAOS_PHY_TEST_MODE : PPAOS_REGULAR_OPERATION));

    genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
}

void MlxlinkCommander::startTuning()
{
    string regName = "PPRT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);
    updateField("s", PPRT_PPTT_START_TUNING);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
}

void MlxlinkCommander::sendPprt()
{
    string regName = "PPRT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);
    updateField("lane_rate_oper", prbsLaneRateToMask(_userInput._pprtRate));
    updateField("prbs_mode_admin", prbsModeToMask(_userInput._pprtMode));

    genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
}

void MlxlinkCommander::sendPptt()
{
    string regName = "PPTT";
    resetParser(regName);

    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("e", PPRT_PPTT_ENABLE);
    updateField("lane_rate_admin", prbsLaneRateToMask(_userInput._ppttRate));
    updateField("prbs_mode_admin", prbsModeToMask(_userInput._ppttMode));
    genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
}

void MlxlinkCommander::sendPtys()
{
    try {
        if (_prbsTestMode) {
            throw MlxRegException("Cannot Configure Port Speeds in Physical Test Mode. Please Disable First");
        }
        MlxlinkRecord::printCmdLine("Configuring Port Speeds");
        u_int32_t protoCap = getPtysCap();
        string regName = "PTYS";
        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("proto_mask", _protoActive);
        u_int32_t ptysMask = 0x0;
        u_int32_t ptysExtMask = 0x0;
        string proto = (_protoActive == IB) ? "ib" : "eth";
        for (u_int32_t i = 0; i < _ptysSpeeds.size(); i++) {
            u_int32_t ext_mask = ptysSpeedToExtMask(_ptysSpeeds[i]);
            if(ext_mask)
            {
                ptysExtMask |= ext_mask;
                continue;
            }
            ptysMask |= ptysSpeedToMask(_ptysSpeeds[i], protoCap);
        }
        if (_protoActive == IB) {
            ptysMask |= 0x1;
            updateField("ib_link_width_admin", 7);
        }
        else {
            updateField("ext_eth_proto_admin", ptysExtMask);
        }
        if (_linkModeForce == true) {
            updateField("an_disable_admin", 1);
            if(ptysExtMask) {
                updateField("force_tx_aba_param", 0);
            }
        }
        updateField(proto + "_proto_admin", ptysMask);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Sending PTYS (Configuring port speeds) raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

u_int32_t MlxlinkCommander::ptysSpeedToExtMask(const string & speed)
{
    if(_protoActive == IB)
        return 0;
    return ptysSpeedToExtMaskETH(speed);
}

u_int32_t MlxlinkCommander::ptysSpeedToMask(const string & speed, u_int32_t protoCap)
{
    checkSupportedSpeed(speed, protoCap);
    return (_protoActive == IB) ?
           ptysSpeedToMaskIB(speed) : ptysSpeedToMaskETH(speed);
}

void MlxlinkCommander::checkSupportedSpeed(const string & speed, u_int32_t protoCap)
{
    u_int32_t mask =
        (_protoActive == IB) ?
        ptysSpeedToMaskIB(speed) : ptysSpeedToMaskETH(speed);
    u_int32_t diffProto =
        (_protoActive == IB) ?
        ptysSpeedToMaskETH(speed) : ptysSpeedToMaskIB(speed);
    if (diffProto) {
        string errStr = "Supported Speeds Are: ";
        errStr += SupportedSpeeds2Str(_protoActive, protoCap);
        throw MlxRegException(speed + " is not supported by Protocol!\n" + errStr);
    }
    if (!(mask & protoCap)) {
        string errStr = "Supported Speeds Are: ";
        errStr += SupportedSpeeds2Str(_protoActive, protoCap);
        throw MlxRegException(speed + " is not supported by Device!\n" + errStr);
    }
}

void MlxlinkCommander::sendPplm()
{
    try {
        if (_prbsTestMode) {
            throw MlxRegException("Cannot Configure FEC in Physical Test Mode. Please Disable First.");
        }
        MlxlinkRecord::printCmdLine("Configuring Port FEC");
        if (_protoActive == IB) {
            throw MlxRegException("FEC Configuration is Valid for ETHERNET only!");
        }
        if (!_linkUP && _userInput._speedFec == "") {
            throw MlxRegException("When Port is Not Active, You Must Specify the Speed to Configure FEC (--fec_speed <speed>)");
        }

        checkPplmCap();

        string regName = "PPLM";
        resetParser(regName);

        updateField("local_port", _localPort);
        string speedStrG = (_linkUP && _userInput._speedFec == "") ?
                speedToStr(_speedStrG) : _userInput._speedFec + "g";
        if (speedStrG != "100g") {
            updateField("fec_override_admin_100g", 0);
        }
        if (speedStrG != "50g" && speedStrG != "25G") {
            updateField("fec_override_admin_50g", 0);
            updateField("fec_override_admin_25g", 0);
        }
        if (speedStrG != "10g" && speedStrG != "40g") {
            updateField("fec_override_admin_10g_40g", 0);
        }
        if (speedStrG == "10g" || speedStrG == "40g") {
            speedStrG = "10g_40g";
        }
        if (speedStrG != "56g") {
            updateField("fec_override_admin_56g", 0);
        }
        string fieldToUpdate = "fec_override_admin_" + speedStrG;
        updateField(fieldToUpdate, fecToBit(_userInput._pplmFec));
        if (speedStrG == "50g") {
            updateField("fec_override_admin_25g", fecToBit(_userInput._pplmFec));
        } else if (speedStrG == "25g") {
            updateField("fec_override_admin_50g", fecToBit(_userInput._pplmFec));
        }
        genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Sending PPLM (Configuring port FEC) raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::checkPplmCap()
{
    if (_userInput._pplmFec == "AU") {
        return;
    }
    string regName = "PPLM";
    resetParser(regName);
    updateField("local_port", _localPort);
    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
    string speedStrG = (_linkUP && _userInput._speedFec == "") ? speedToStr(_speedStrG) : deleteLastComma(_userInput._speedFec) + "g";
    if (speedStrG == "10g" || speedStrG == "40g") {
        speedStrG = "10g_40g";
    }
    if (!(fecToBit(_userInput._pplmFec) & getFieldValue("fec_override_cap_" + speedStrG, _buffer))) {
        string speedExpception = (_linkUP) ? _speedStrG : _userInput._speedFec;
        throw MlxRegException(FEC2Str(_userInput._pplmFec) + " is not supported in " + speedExpception);
    }
}

void MlxlinkCommander::getSltpRegAndLeva(u_int32_t lane)
{
    string regName = "SLTP";

    resetParser(regName);
    updateField("local_port", _localPort);
    updateField("pnat", PNAT_LOCAL);
    updateField("lane_speed", prbsLaneRateToMask(_speedStrG));
    updateField("lane", lane);
    updateField("c_db", _userInput._db);

    genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);

    _userInput._sltpParams[OB_REG] = getFieldValue("ob_reg", _buffer);
    _userInput._sltpParams[OB_LEVA] = getFieldValue("ob_leva", _buffer);
}

void MlxlinkCommander::checkSltpParamsSize()
{
    u_int32_t sltpParamsSize = OB_PREEMP_MODE;
    if (_userInput._advancedMode) {
        if (_productTechnology == PRODUCT_16NM) {
            sltpParamsSize = OB_ALEV_OUT;
        } else {
            sltpParamsSize = OB_LEVA;
        }
    } else if (_productTechnology == PRODUCT_16NM) {
        throw MlxRegException(
                "Configuring SLTP for 16nm technology is supported only "
                "in advanced mode");
    }
    if (_userInput._sltpParams.size() != sltpParamsSize+1) {
        throw MlxRegException("Invalid set of Transmitter Parameters");
    }
}

void MlxlinkCommander::updateSltp28_40nmFields()
{
    updateField("polarity", _userInput._sltpParams[POLARITY]);
    updateField("ob_tap0", _userInput._sltpParams[OB_TAP0]);
    updateField("ob_tap1", _userInput._sltpParams[OB_TAP1]);
    updateField("ob_tap2", _userInput._sltpParams[OB_TAP2]);
    updateField("ob_bias", _userInput._sltpParams[OB_BIAS]);
    updateField("ob_preemp_mode", _userInput._sltpParams[OB_PREEMP_MODE]);
    updateField("ob_reg", _userInput._sltpParams[OB_REG]);
}

void MlxlinkCommander::updateSltp16nmFields()
{
    updateField("pre_2_tap", _userInput._sltpParams[PRE_2_TAP]);
    updateField("pre_tap", _userInput._sltpParams[PRE_TAP]);
    updateField("main_tap", _userInput._sltpParams[MAIN_TAP]);
    updateField("post_tap", _userInput._sltpParams[POST_TAP]);
    updateField("ob_m2lp", _userInput._sltpParams[OB_M2LP]);
    updateField("ob_amp", _userInput._sltpParams[OB_AMP]);
    updateField("ob_alev_out", _userInput._sltpParams[OB_ALEV_OUT]);
}

void MlxlinkCommander::sendSltp()
{
    MlxlinkRecord::printCmdLine("Configuring Port Transmitter Parameters");
    checkSltpParamsSize();
    string regName = "SLTP";
    if (_userInput._sltpLane && _userInput._lane >= _numOfLanes) {
        throw MlxRegException(
                  "Could Not find Lane " + to_string(_userInput._lane));
    }
    try {
        for (u_int32_t i = 0; i < _numOfLanes; i++) {
            if (_userInput._sltpLane && _userInput._lane != i) {
                continue;
            }
            if (_userInput._db && i != 0) {
                break;
            }
            if (_productTechnology != PRODUCT_16NM && !_userInput._advancedMode) {
                getSltpRegAndLeva(i);
            }
            resetParser(regName);
            updateField("local_port", _localPort);
            updateField("pnat", PNAT_LOCAL);
            updateField("lane_speed", prbsLaneRateToMask(_speedStrG));
            updateField("lane", i);
            if (_productTechnology == PRODUCT_16NM) {
                updateSltp16nmFields();
            } else {
                updateSltp28_40nmFields();
            }
            updateField("c_db", _userInput._db);
            genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
        }
    } catch (MlxRegException &exc) {
        u_int32_t ob_bad_stat = getFieldValue("ob_bad_stat", _buffer);
        _allUnhandledErrors += string("Configuring Port "
                "Transmitter Parameters raised the following exception: "
                "Failed to send parameter set") +
                string( _productTechnology == PRODUCT_16NM ?
                        _mlxlinkMaps->_SLTP16BadSetStatus2Str[ob_bad_stat].c_str() :
                        _mlxlinkMaps->_SLTPBadSetStatus2Str[ob_bad_stat].c_str())+"\n";
    }
}

u_int32_t MlxlinkCommander::getLoopbackMode(const string &lb)
{
    if (lb == "NO") {
        return PHY_NO_LOOPBACK;
    }
    if (lb == "PH") {
        return PHY_LOCAL_LOOPBACK;
    }
    if (lb == "EX") {
        return EXTERNAL_LOCAL_LOOPBACK;
    }
    throw MlxRegException(
            "Invalid loopback option: %s, see tool usage \"%s --help\"",
            lb.c_str(), MLXLINK_EXEC);
}

void MlxlinkCommander::sendPplr()
{
    try {
        MlxlinkRecord::printCmdLine("Configuring Port Loopback");
        string regName = "PPLR";
        resetParser(regName);

        updateField("local_port", _localPort);
        updateField("lb_en", getLoopbackMode(_userInput._pplrLB));

        genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Sending PPLR (Configuring port loopback) raised the following exception: ") + string(exc.what()) + string("\n");
    }
}

void MlxlinkCommander::sendPepc()
{
    try {
        MlxlinkRecord::printCmdLine("Configuring External PHY");
        string regName = "PEPC";

        // Get old values of PEPC fields
        resetParser(regName);
        updateField("local_port", _localPort);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_GET);
        u_int32_t forceMode_int = getFieldValue("twisted_pair_force_mode", _buffer);
        u_int32_t anMode_int = getFieldValue("twisted_pair_an_mode", _buffer);

        // Set new values of PEPC fields
        resetParser(regName);
        updateField("local_port", _localPort);
        if (_userInput._sendPepcForceMode) {
            updateField("twisted_pair_force_mode", pepc_force_mode_to_int(_userInput._forceMode));
        } else {
            updateField("twisted_pair_force_mode", forceMode_int);
        }
        updateField("twisted_pair_an_mode", anMode_int);
        genBuffSendRegister(regName, MACCESS_REG_METHOD_SET);
    } catch (const std::exception &exc) {
        _allUnhandledErrors += string("Sending PEPC (Configuring External PHY parameters) raised the following exception: ") + string(exc.what()) + string("\n");
    }
}
