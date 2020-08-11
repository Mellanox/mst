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

#include "mlxlink_ui.h"

MlxlinkUi::MlxlinkUi() :
    CommandLineRequester(MLXLINK_EXEC " OPTIONS"),
    _cmdParser(MLXLINK_EXEC)
{
}

MlxlinkUi::~MlxlinkUi()
{
    if (_mlxlinkCommander) {
         delete _mlxlinkCommander;
    }
}

void MlxlinkUi::createMlxlinkCommander()
{
    _mlxlinkCommander = new MlxlinkCommander();
}

void MlxlinkUi::printSynopsisHeader()
{
    printf(
        IDENT "NAME:\n"
        IDENT2 MLXLINK_EXEC "\n\n"
        IDENT "SYNOPSIS:\n"
        IDENT2 MLXLINK_EXEC " [OPTIONS]\n\n"
        IDENT "DESCRIPTION:\n"
        IDENT2 "The mstlink tool is used to check and debug link status and issues related to them.\n"
        IDENT2 "The tool can be used on different links and cables (passive, active, transceiver and backplane).\n");
    printf("\n");
    printf(IDENT "OPTIONS:\n");
    MlxlinkRecord::printFlagLine(HELP_FLAG_SHORT, HELP_FLAG, "", "Display help message.");
    MlxlinkRecord::printFlagLine(VERSION_FLAG_SHORT, VERSION_FLAG, "",
                  "Display version info.");
    MlxlinkRecord::printFlagLine(DEVICE_FLAG_SHORT, DEVICE_FLAG, "device",
                  "Perform operation for a specified mst device");
    MlxlinkRecord::printFlagLine(LABEL_PORT_FLAG_SHORT, LABEL_PORT_FLAG, "port_number",
                  "Port Number");
    MlxlinkRecord::printFlagLine(PORT_TYPE_FLAG_SHORT, PORT_TYPE_FLAG, "port_type", "Port Type [NETWORK(Default)/PCIE]");
    MlxlinkRecord::printFlagLine(DEPTH_FLAG_SHORT, DEPTH_FLAG, "depth", "depth level of the DUT of some hierarchy (PCIE only)");
    MlxlinkRecord::printFlagLine(PCIE_INDEX_FLAG_SHORT, PCIE_INDEX_FLAG, "pcie_index", "PCIe index number (Internal domain index) (PCIE only)");
    MlxlinkRecord::printFlagLine(NODE_FLAG_SHORT, NODE_FLAG, "node", "the node within each depth (PCIE only)");
    MlxlinkRecord::printFlagLine(PRINT_JSON_OUTPUT_FLAG_SHORT, PRINT_JSON_OUTPUT_FLAG, "", "Print the output in json format");
}

void MlxlinkUi::printSynopsisQueries()
{
    printf(IDENT "QUERIES:\n");
    MlxlinkRecord::printFlagLine(PCIE_LINKS_FLAG_SHORT, PCIE_LINKS_FLAG, "",
                  "Show valid PCIe links (PCIE only)");
    MlxlinkRecord::printFlagLine(MODULE_INFO_FLAG_SHORT, MODULE_INFO_FLAG, "",
                  "Show Module Info");
    MlxlinkRecord::printFlagLine(BER_FLAG_SHORT, BER_FLAG, "",
                  "Show Physical Counters and BER Info");
    MlxlinkRecord::printFlagLine(EYE_OPENING_FLAG_SHORT, EYE_OPENING_FLAG, "",
                  "Show Eye Opening Info");
    MlxlinkRecord::printFlagLine(FEC_DATA_FLAG_SHORT, FEC_DATA_FLAG, "", "Show FEC Capabilities");
    MlxlinkRecord::printFlagLine(SLTP_SHOW_FLAG_SHORT, SLTP_SHOW_FLAG, "", "Show Transmitter Info");

    MlxlinkRecord::printFlagLine(DEVICE_DATA_FLAG_SHORT, DEVICE_DATA_FLAG, "", "General Device Info");
    MlxlinkRecord::printFlagLine(BER_MONITOR_INFO_FLAG_SHORT, BER_MONITOR_INFO_FLAG, "", "Show BER Monitor Info (not supported for HCA)");
    MlxlinkRecord::printFlagLine(PEPC_SHOW_FLAG_SHORT, PEPC_SHOW_FLAG, "", "Show External PHY Info (for Ethernet switches only)");
}

void MlxlinkUi::printSynopsisCommands()
{
    printf(IDENT "COMMANDS:\n");
    MlxlinkRecord::printFlagLine(PAOS_FLAG_SHORT, PAOS_FLAG, "port_state",
                  "Configure Port State [UP(up)/DN(down)/TG(toggle)]");
    MlxlinkRecord::printFlagLine(PTYS_FLAG_SHORT, PTYS_FLAG, "speeds",
                  "Configure Speeds [speed1,speed2,...]");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PTYS_LINK_MODE_FORCE_FLAG_SHORT, PTYS_LINK_MODE_FORCE_FLAG, "",
                  "Configure Link Mode Force (Disable AN)");
    MlxlinkRecord::printFlagLine(PPLR_FLAG_SHORT, PPLR_FLAG, "loopback",
                  "Configure Loopback Mode [NO(no loopback)/PH(phy loopback)/EX(external loopback)]");
    MlxlinkRecord::printFlagLine(PPLM_FLAG_SHORT, PPLM_FLAG, "fec_override",
                  "Configure FEC [AU(Auto)/NF(No-FEC)/FC(FireCode FEC)/RS(RS-FEC)]");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(FEC_SPEED_FLAG_SHORT, FEC_SPEED_FLAG, "fec_speed",
                  "Speed to Configure FEC [100G/50G/25G/...] (Default is Active Speed)");
    MlxlinkRecord::printFlagLine(SLTP_SET_FLAG_SHORT, SLTP_SET_FLAG, "params",
                  "Configure Transmitter Parameters [polarity,ob_tap0,...]");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(LANE_FLAG_SHORT, LANE_FLAG, "transmitter_lane",
                  "Transmitter Lane to Set (Optional - Default All Lanes)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(DATABASE_FLAG_SHORT, DATABASE_FLAG, "", "Save Transmitter Configuration for Current Speed Permanently (Optional)");
    MlxlinkRecord::printFlagLine(PRBS_MODE_FLAG_SHORT, PRBS_MODE_FLAG, "prbs_mode",
                  "Physical Test Mode Configuration [EN(enable)/DS(disable)/TU(perform tuning)]");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PPRT_PRBS_FLAG_SHORT, PPRT_PRBS_FLAG, "rx_prbs_mode",
                  "RX PRBS Mode [PRBS31(Default)/PRBS7/...] (Optional - Default PRBS31)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PPTT_PRBS_FLAG_SHORT, PPTT_PRBS_FLAG, "tx_prbs_mode",
                  "TX PRBS Mode [PRBS31(Default)/PRBS7/...] (Optional - Default PRBS31)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PPRT_RATE_FLAG_SHORT, PPRT_RATE_FLAG, "rx_lane_rate",
                  "RX Lane Rate [EDR(Default)/25G/10G/...]  (Optional - Default 25G)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PPTT_RATE_FLAG_SHORT, PPTT_RATE_FLAG, "tx_lane_rate",
                  "TX Lane Rate [EDR(Default)/25G/10G/...]  (Optional - Default 25G)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PRBS_LANES_FLAG_SHORT, PRBS_LANES_FLAG, "lanes",
                  "PRBS lanes to set (one or more lane separated by comma)[0,1,2,...] (Optional - Default all lanes)");
    MlxlinkRecord::printFlagLine(BER_COLLECT_FLAG_SHORT, BER_COLLECT_FLAG, "csv_file",
                  "Port Extended Information Collection [CSV File]");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(BER_LIMIT_FLAG_SHORT, BER_LIMIT_FLAG, "limit_criteria",
                  "BER Limit Criteria [Nominal(Default)/Corner/Drift] (Optional - Default Nominal)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(ITERATION_FLAG_SHORT, ITERATION_FLAG, "iteration", "Iteration Number of BER Collection");
    MlxlinkRecord::printFlagLine(PPCNT_CLEAR_FLAG_SHORT, PPCNT_CLEAR_FLAG, "",
                  "Clear Counters");
    MlxlinkRecord::printFlagLine(PEPC_SET_FLAG_SHORT, PEPC_SET_FLAG, "",
                  "Set External PHY (for Ethernet switches only)");
    printf(IDENT);
    MlxlinkRecord::printFlagLine(PEPC_FORCE_MODE_FLAG_SHORT, PEPC_FORCE_MODE_FLAG, "twisted_pair_force_mode",
                  "Twisted Pair Force Mode [MA(Master)/SL(Slave)]");
}

void MlxlinkUi::printSynopsis()
{
    printSynopsisHeader();
    printf("\n");
    printSynopsisQueries();
    printf("\n");
    printSynopsisCommands();
}

void MlxlinkUi::printHelp()
{
    printSynopsis();
    printf("\n");
    printf(IDENT "Examples:\n");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "get info of <device>, <port_number>",
           MLXLINK_EXEC " -d <device> -p <port_number>");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n",
           "get info of <device>, <port_number> and BER Counters",
           MLXLINK_EXEC " -d <device> -p <port_number> -c");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n",
           "get info of <device>, <port_number> and Transmitter Parameters",
           MLXLINK_EXEC " -d <device> -p <port_number> --show_serdes_tx");

    printf("\n");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Configure Port State",
           MLXLINK_EXEC " -d <device> -p <port_number> --port_state UP");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Configure Port Speeds",
           MLXLINK_EXEC " -d <device> -p <port_number> --speeds 25G,50G,100G");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Configure FEC",
           MLXLINK_EXEC " -d <device> -p <port_number> --fec RS");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Configure Port for Physical Test Mode",
           MLXLINK_EXEC " -d <device> -p <port_number> --test_mode EN (--rx_prbs PRBS31 --rx_rate 25G --tx_prbs PRBS7 --tx_rate 10G)");
    printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Perform PRBS Tuning",
           MLXLINK_EXEC " -d <device> -p <port_number> --test_mode TU");
    if (_mlxlinkCommander->_userInput._advancedMode) {
        printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Configure Transmitter Parameters (on lane, to database)",
               MLXLINK_EXEC " -d <device> -p <port_number> --serdes_tx <polarity>,<ob_tap0>,<ob_tap1>,<ob_tap2>,<ob_bias>,<ob_preemp_mode>,<ob_reg>,<ob_leva> (--serdes_tx_lane <lane number>) (--database)");
    } else {
        printf(IDENT2 "%-40s: \n" IDENT3 "%s\n", "Configure Transmitter Parameters (on lane, to database)",
               MLXLINK_EXEC " -d <device> -p <port_number> --serdes_tx <polarity>,<ob_tap0>,<ob_tap1>,<ob_tap2>,<ob_bias>,<ob_preemp_mode> (--serdes_tx_lane <lane number>) (--database)");
    }
    printf("\n");
}

void MlxlinkUi::validateMandatoryParams()
{
    if (_mlxlinkCommander->_device == "") {
        throw MlxRegException("Please provide a device name");
    }
    if (_mlxlinkCommander->_userInput._portType != "NETWORK" && _mlxlinkCommander->_userInput._portType != "PCIE") {
        throw MlxRegException(
                  "Please provide a valid Port Type [NETWORK(Default)/PCIE]");
    }
    if (_mlxlinkCommander->_userInput._links &&
            _mlxlinkCommander->_userInput._portType != "PCIE") {
        throw MlxRegException("The --" PCIE_LINKS_FLAG " option is valid only with --port_type PCIE");
    }
    if (_mlxlinkCommander->_userInput._links &&
            (_mlxlinkCommander->_userInput._showEyeInfo || _mlxlinkCommander->_userInput._showSltp ||
                    _mlxlinkCommander->_userInput._showCounters ||  _mlxlinkCommander->_userInput._showSlrp )) {
        throw MlxRegException("No options allowed to use while querying --" PCIE_LINKS_FLAG);
    }
    if ((_mlxlinkCommander->_userInput._labelPort == 0) && (_mlxlinkCommander->_userInput._portType != "PCIE")) {
        throw MlxRegException("Please provide a valid port number");
    }
}

void MlxlinkUi::validatePCIeParams()
{
    if (_mlxlinkCommander->_userInput._portType == "PCIE") {
        _mlxlinkCommander->_userInput._pcie = true;
        if (_mlxlinkCommander->_uniqueCmds) {
            throw MlxRegException("Command flags are not available for PCIE");
        }
        if (_mlxlinkCommander->_networkCmds) {
            throw MlxRegException("FEC and Module Info flags are not available for PCIE");
        }
        _sendRegFuncMap[SHOW_PCIE] = SHOW_PCIE;
        _sendRegFuncMap[SHOW_PDDR] = 0;
        if (_mlxlinkCommander->_userInput._sendNode ||
                _mlxlinkCommander->_userInput._sendDepth ||
                _mlxlinkCommander->_userInput._sendPcieIndex) {
            if (!(_mlxlinkCommander->_userInput._sendNode &&
                    _mlxlinkCommander->_userInput._sendDepth &&
                    _mlxlinkCommander->_userInput._sendPcieIndex)) {
                throw MlxRegException("The --depth, --pcie_index and --node must be specified for PCIE");
           } else {
               _mlxlinkCommander->_userInput._sendDpn = true;
           }
        }
        if (_mlxlinkCommander->_userInput._specifiedPort) {
            if (_mlxlinkCommander->_userInput._sendNode ||
                    _mlxlinkCommander->_userInput._sendDepth ||
                    _mlxlinkCommander->_userInput._sendPcieIndex) {
                throw MlxRegException("For PCIE port, either port flag or depth, pcie_index and node flags should be specified");
           }
        }
    } else if (_mlxlinkCommander->_userInput._sendNode ||
            _mlxlinkCommander->_userInput._sendDepth ||
            _mlxlinkCommander->_userInput._sendPcieIndex) {
        throw MlxRegException("The --depth, --node and --pcie_index flags are valid only with --port_type PCIE");
    }
}

void MlxlinkUi::validateGeneralCmdsParams()
{
    if (_mlxlinkCommander->_uniqueCmds > 1) {
        throw MlxRegException("Commands are mutually exclusive!");
    }
    if (_sendRegFuncMap[SEND_PAOS] == SEND_PAOS && !checkPaosCmd(_mlxlinkCommander->_userInput._paosCmd)) {
        throw MlxRegException(
                  "Please provide a valid paos command [UP(up)/DN(down)/TG(toggle)]");
    }
    if (_sendRegFuncMap[SEND_PPLM] == SEND_PPLM && !checkPplmCmd(_mlxlinkCommander->_userInput._pplmFec)) {
        throw MlxRegException(
                  "Please provide a valid FEC [AU(Auto)/NF(No-Fec)/FC(FireCode FEC)/RS(RS FEC)]");
    }
    if (_sendRegFuncMap[SEND_PPLM] != SEND_PPLM && _mlxlinkCommander->_userInput._speedFec != "") {
        throw MlxRegException(
                  "The --fec_speed flag is valid only with --fec flag");
    }
    if (_sendRegFuncMap[SEND_PPLM] == SEND_PPLM) {
        if (_mlxlinkCommander->_userInput._speedFec != "" &&
                _mlxlinkCommander->_userInput._speedFec != "100G" &&
                _mlxlinkCommander->_userInput._speedFec != "56G" &&
                _mlxlinkCommander->_userInput._speedFec != "50G" &&
                _mlxlinkCommander->_userInput._speedFec != "40G" &&
                _mlxlinkCommander->_userInput._speedFec != "25G" &&
                _mlxlinkCommander->_userInput._speedFec != "10G") {
            throw MlxRegException("Please Provide a Valid Speed to Configure FEC (100G/56G/50G/40G/25G/10G)");
        }
    }
    if (_sendRegFuncMap[SEND_SLTP] == SEND_SLTP) {
        if (_mlxlinkCommander->_userInput._sltpLane && _mlxlinkCommander->_userInput._db) {
            throw MlxRegException(
                      "Lane and Database flags are mutually exclusive");
        }
    } else if (_mlxlinkCommander->_userInput._sltpLane || _mlxlinkCommander->_userInput._db) {
        throw MlxRegException(
                  "Transmitter Lane or DataBase flags are valid only with Configure Transmitter Parameters flag (--st_set)");
    }
}

void MlxlinkUi::validatePRBSParams()
{
    bool prbsFlags = _mlxlinkCommander->_userInput._sendPprt ||
                    _mlxlinkCommander->_userInput._sendPptt ||
                    _mlxlinkCommander->_userInput._pprtRate != "" ||
                    _mlxlinkCommander->_userInput._pprtRate != "";
    if (_sendRegFuncMap[SEND_PRBS] == SEND_PRBS) {
        if (!checkPrbsCmd(_mlxlinkCommander->_userInput._prbsMode)) {
            throw MlxRegException(
                      "you must provide a valid PRBS test mode [DS/EN/TU]");
        }
        if (_mlxlinkCommander->_userInput._prbsMode == "DS" ||
                _mlxlinkCommander->_userInput._prbsMode == "TU") {
            if (prbsFlags) {
                throw MlxRegException(
                          "PRBS parameters flags valid only with PRBS Enable flag (--test_mode EN)");
            }
        }
        if (_mlxlinkCommander->_userInput._prbsMode == "EN"
            && (!prbsLaneRateCheck(_mlxlinkCommander->_userInput._pprtRate)
                || !prbsLaneRateCheck(_mlxlinkCommander->_userInput._ppttRate))) {
            string errStr = "Valid PRBS Lane Rates Are:";
            for (u_int32_t i = 0; i < _mlxlinkCommander->_mlxlinkMaps->_prbsLaneRateList.size(); i++) {
                errStr += _mlxlinkCommander->_mlxlinkMaps->_prbsLaneRateList[i] + ",";
            }
            errStr = deleteLastComma(errStr);
            errStr += "\nDefault PRBS Lane Rate is EDR / 25GE / 50GE / 100GE (25.78125 Gb/s)";
            throw MlxRegException(errStr);
        }
    } else if (prbsFlags) { // add check for lanes flag to work with PRBS and eye scan only
        throw MlxRegException(
                  "PRBS parameters flags valid only with PRBS Enable flag (--test_mode EN)");
    }
}

void MlxlinkUi::validateSpeedAndCSVBerParams()
{
    if (_sendRegFuncMap[SEND_BER_COLLECT] == SEND_BER_COLLECT) {
        if (!endsWith(_mlxlinkCommander->_userInput._csvBer, ".csv")) {
            throw MlxRegException("you must provide a valid .csv file");
        }
        if (!checkTestMode(_mlxlinkCommander->_userInput._testMode)) {
            throw MlxRegException(
                      "Please provide a valid test mode [Nominal(Default)/Corner/Drift]");
        }
    } else if (_mlxlinkCommander->_userInput._testMode != "Nominal") {
        throw MlxRegException(
                  "BER Limit Criteria flag is valid only with Port Information Collection flag (--bc)");
    }
    if (_sendRegFuncMap[SEND_PEPC] == SEND_PEPC) {
        if (!_mlxlinkCommander->_userInput._sendPepcForceMode) {
            throw MlxRegException(
                      "Please provide --twisted_pair_force_mode");
        }
        if (_mlxlinkCommander->_userInput._sendPepcForceMode &&
                !checkPepcForceMode(_mlxlinkCommander->_userInput._forceMode)) {
            throw MlxRegException(
                      "Please provide a valid twisted pair force mode [MA(Master)/SL(Slave)]");
        }
    } else {
        if (_mlxlinkCommander->_userInput._sendPepcForceMode) {
            throw MlxRegException(
                      "Please provide --set_external_phy option to configure the twisted pair force mode");
        }
    }
    if (_mlxlinkCommander->_linkModeForce && _mlxlinkCommander->_ptysSpeeds.size() != 1) {
        throw MlxRegException(
                  "--link_mode_force should receive exactly one speed using --speeds");
    }
}

void MlxlinkUi::paramValidate()
{
    validateMandatoryParams();
    validatePCIeParams();
    validateGeneralCmdsParams();
    validatePRBSParams();
    validateSpeedAndCSVBerParams();
}

void MlxlinkUi::initCmdParser()
{
    for (u_int32_t it = SHOW_PDDR; it <= FUNCTION_LAST; it++) {
        _sendRegFuncMap.push_back(0);
    }
    AddOptions(DEVICE_FLAG, DEVICE_FLAG_SHORT, "MstDevice",
               "Mellanox mst device name");
    AddOptions(HELP_FLAG, HELP_FLAG_SHORT, "", "Show help message and exit");
    AddOptions(VERSION_FLAG, VERSION_FLAG_SHORT, "", "Show version and exit");
    AddOptions(PORT_TYPE_FLAG, PORT_TYPE_FLAG_SHORT, "port_type", "Port Type");
    AddOptions(DEPTH_FLAG, DEPTH_FLAG_SHORT, "depth", "depth");
    AddOptions(PCIE_INDEX_FLAG, PCIE_INDEX_FLAG_SHORT, "pcie_index", "PCIe Index");
    AddOptions(NODE_FLAG, NODE_FLAG_SHORT, "node", "node");
    AddOptions(LABEL_PORT_FLAG, LABEL_PORT_FLAG_SHORT, "LabelPort",
               "Label Port");
    AddOptions(PCIE_LINKS_FLAG, PCIE_LINKS_FLAG_SHORT, "", "Show valid PCIe links");
    AddOptions(BER_FLAG, BER_FLAG_SHORT, "", "Show BER Info");
    AddOptions(EYE_OPENING_FLAG, EYE_OPENING_FLAG_SHORT, "",
               "Show Eye Opening Info");
    AddOptions(MODULE_INFO_FLAG, MODULE_INFO_FLAG_SHORT, "",
               "Show Module Info");
    AddOptions(PPCNT_CLEAR_FLAG, PPCNT_CLEAR_FLAG_SHORT, "",
               "Clear PPCNT Counters");
    AddOptions(DEVICE_DATA_FLAG, DEVICE_DATA_FLAG_SHORT, "", "Device Info");
    AddOptions(BER_MONITOR_INFO_FLAG, BER_MONITOR_INFO_FLAG_SHORT, "", "Show BER Monitor Info");
    AddOptions(PEPC_SHOW_FLAG, PEPC_SHOW_FLAG_SHORT, "", "Show External PHY Info");
    AddOptions(PRINT_JSON_OUTPUT_FLAG, PRINT_JSON_OUTPUT_FLAG_SHORT, "", "Print the output in json format");

    AddOptions(FEC_DATA_FLAG, FEC_DATA_FLAG_SHORT, "", "FEC Data");
    AddOptions(PAOS_FLAG, PAOS_FLAG_SHORT, "PAOS", "Send PAOS");
    AddOptions(PTYS_FLAG, PTYS_FLAG_SHORT, "PTYS", "Send PTYS");
    AddOptions(PTYS_LINK_MODE_FORCE_FLAG, PTYS_LINK_MODE_FORCE_FLAG_SHORT, "", "Set Link Mode Force");
    AddOptions(PPLM_FLAG, PPLM_FLAG_SHORT, "PPLM", "Send PPLM");
    AddOptions(FEC_SPEED_FLAG, FEC_SPEED_FLAG_SHORT, "FECSPEED", "Send PPLM with Speed");
    AddOptions(PPLR_FLAG, PPLR_FLAG_SHORT, "PPLR", "Send PPLR");
    AddOptions(BER_COLLECT_FLAG, BER_COLLECT_FLAG_SHORT, "BERCollectFile",
               "BER Collection csv file");
    AddOptions(BER_LIMIT_FLAG, BER_LIMIT_FLAG_SHORT, "Mode",
               "Test Mode of Ber Collect (Nominal/Corner/Drift)");
    AddOptions(ITERATION_FLAG, ITERATION_FLAG_SHORT, "Iteration", "Iteration of BER Collect");
    AddOptions(PRBS_MODE_FLAG, PRBS_MODE_FLAG_SHORT, "PRBS",
               "Enable/Disable PRBS Test Mode");
    AddOptions(PPRT_PRBS_FLAG, PPRT_PRBS_FLAG_SHORT, "PPRT", "PPRT Mode");
    AddOptions(PPTT_PRBS_FLAG, PPTT_PRBS_FLAG_SHORT, "PPTT", "PPTT Mode");
    AddOptions(PPRT_RATE_FLAG, PPRT_RATE_FLAG_SHORT, "PPRT_RATE",
               "PPRT Lane Rate");
    AddOptions(PPTT_RATE_FLAG, PPTT_RATE_FLAG_SHORT, "PPTT_RATE",
               "PPTT Lane Rate");
    AddOptions(PRBS_LANES_FLAG, PRBS_LANES_FLAG_SHORT, "lanes",
               "PRBS lanes to set");

    AddOptions(SLTP_SHOW_FLAG, SLTP_SHOW_FLAG_SHORT, "", "get SLTP");
    AddOptions(SLTP_SET_FLAG, SLTP_SET_FLAG_SHORT, "set", "set SLTP");
    AddOptions(SLTP_SET_ADVANCED_FLAG, SLTP_SET_ADVANCED_FLAG_SHORT, "", "set SLTP");
    AddOptions(LANE_FLAG, LANE_FLAG_SHORT, "lane", "Lane");
    AddOptions(DATABASE_FLAG, DATABASE_FLAG_SHORT, "", "DB");
    AddOptions(GVMI_ADDRESS_FLAG, GVMI_ADDRESS_FLAG_SHORT, "gvmi", "GVMI");
    AddOptions(PEPC_SET_FLAG, PEPC_SET_FLAG_SHORT, "", "set External PHY");
    AddOptions(PEPC_FORCE_MODE_FLAG, PEPC_FORCE_MODE_FLAG_SHORT, "Twisted_Pair_Force_Mode", "Twisted Pair Force Mode");
    AddOptions(LOG_FLAG,LOG_FLAG_SHORT,"Log file path","Mlxlink Logger");

    _cmdParser.AddRequester(this);
}

void MlxlinkUi::commandsCaller()
{
    for (u_int32_t it = SHOW_PDDR; it < FUNCTION_LAST; it++) {
        if (_sendRegFuncMap[it]) {
            switch(_sendRegFuncMap[it]) {
            case SHOW_PDDR:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showPddr()");
                _mlxlinkCommander->showPddr();
               break;
            case SHOW_PCIE:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showPcie()");
               _mlxlinkCommander->showPcie();
               break;
            case SHOW_MODULE:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showModuleInfo() : \"Show Module Info\"");
               _mlxlinkCommander->showModuleInfo();
               break;
            case SHOW_BER:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showBer() : \"Show Physical Counters and BER Info\"");
                _mlxlinkCommander->showBer();
               break;
            case SHOW_EYE:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showEye() : \"Show Eye Opening Info\"");
               _mlxlinkCommander->showEye();
               break;
            case SHOW_FEC:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showFEC() : \"Show FEC Capabilities\"");
               _mlxlinkCommander->showFEC();
               break;
            case SHOW_SLTP:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showSltp() : \"Show Transmitter Info\"");
               _mlxlinkCommander->showSltp();
               break;
            case SHOW_DEVICE:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showDeviceData() : \"General Device Info\"");
                _mlxlinkCommander->showDeviceData();
                break;
            case SHOW_BER_MONITOR:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showBerMonitorInfo() : \"Show Physical Counters and BER Info\"");
                _mlxlinkCommander->showBerMonitorInfo();
                break;
            case SHOW_EXTERNAL_PHY:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showExternalPhy() : \"Show External PHY Info\"");
                _mlxlinkCommander->showExternalPhy();
                break;
            case SHOW_PCIE_LINKS:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> showPcieLinks() : \"Show Valid PCIe Links\"");
                _mlxlinkCommander->showPcieLinks();
                break;
            case SEND_BER_COLLECT:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> collectBER() :  \"Port Extended Information Collection\"");
                _mlxlinkCommander->collectBER();
                break;
            case SEND_PAOS:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> sendPaos() : \"Configure Port State\"");
                _mlxlinkCommander->sendPaos();
                break;
            case SEND_PTYS:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> sendPtys() : \"Configure Speeds\"");
                _mlxlinkCommander->sendPtys();
                break;
            case SEND_PPLM:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> sendPplm() : \"Configure FEC\"");
                _mlxlinkCommander->sendPplm();
                break;
            case SEND_PPLR:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> sendPplr() : \"Configure Loopback Mode\"");
                _mlxlinkCommander->sendPplr();
                break;
            case SEND_PRBS:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> handlePrbs() : \"Physical Test Mode Configuration\"");
                _mlxlinkCommander->handlePrbs();
                break;
            case SEND_SLTP:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> sendSltp() : \"Configure Transmitter Parameters\"");
                _mlxlinkCommander->sendSltp();
                break;
            case SEND_CLEAR_COUNTERS:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger, "-> clearCounters() : \"Clear Counters\"");
                _mlxlinkCommander->clearCounters();
                break;
            case SEND_PEPC:
                PRINT_LOG(_mlxlinkCommander->_mlxlinkLogger,"-> sendPepc() : \"Set External PHY\"");
                _mlxlinkCommander->sendPepc();
                break;
            }
        }
    }
}

ParseStatus MlxlinkUi::HandleOption(string name, string value)
{
    _sendRegFuncMap[SHOW_PDDR] = SHOW_PDDR;
    if (name == PRINT_JSON_OUTPUT_FLAG) {
        MlxlinkRecord::jsonFormat = true;
        return PARSE_OK;
    } else if (name == HELP_FLAG) {
        printHelp();
        return PARSE_OK_WITH_EXIT;
    } else if (name == VERSION_FLAG) {
        print_version_string(MLXLINK_EXEC, NULL);
        return PARSE_OK_WITH_EXIT;
    } else if (name == DEVICE_FLAG) {
        _mlxlinkCommander->_device = value;
        return PARSE_OK;
    } else if (name == MODULE_INFO_FLAG) {
        _sendRegFuncMap[SHOW_MODULE] =SHOW_MODULE;
        _mlxlinkCommander->_networkCmds++;
        return PARSE_OK;
    } else if (name == BER_FLAG) {
        _sendRegFuncMap[SHOW_BER] = SHOW_BER;
        _mlxlinkCommander->_userInput._showCounters = true;
        return PARSE_OK;
    } else if (name == PPCNT_CLEAR_FLAG) {
        _sendRegFuncMap[SEND_CLEAR_COUNTERS] = SEND_CLEAR_COUNTERS;
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == EYE_OPENING_FLAG) {
        _sendRegFuncMap[SHOW_EYE] = SHOW_EYE;
        _mlxlinkCommander->_userInput._showEyeInfo = true;
        return PARSE_OK;
    } else if (name == SLTP_SHOW_FLAG) {
        _sendRegFuncMap[SHOW_SLTP] = SHOW_SLTP;
        _mlxlinkCommander->_userInput._showSltp = true;
        return PARSE_OK;
    } else if (name == SLTP_SET_FLAG) {
        string sltpParamsLine = toUpperCase(value);
        std::vector<string> sltpParams = _mlxlinkCommander->parseParamsFromLine(sltpParamsLine);
        _mlxlinkCommander->getSltpParamsFromVector(sltpParams);
        _sendRegFuncMap[SEND_SLTP] = SEND_SLTP;
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == LANE_FLAG) {
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._lane);
        _mlxlinkCommander->_userInput._sltpLane = true;
        return PARSE_OK;
    } else if (name == DATABASE_FLAG) {
        _mlxlinkCommander->_userInput._db = true;
        return PARSE_OK;
    } else if (name == GVMI_ADDRESS_FLAG) {
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._gvmiAddress);
        return PARSE_OK;
    } else if (name == SLTP_SET_ADVANCED_FLAG) {
        _mlxlinkCommander->_userInput._advancedMode = true;
        return PARSE_OK;
    } else if (name == PORT_TYPE_FLAG) {
        _mlxlinkCommander->_userInput._portType = toUpperCase(value);
        return PARSE_OK;
    } else if (name == LABEL_PORT_FLAG) {
        if (endsWith(value.c_str(), "/1") || endsWith(value.c_str(), "/2")
            || endsWith(value.c_str(), "/3")
            || endsWith(value.c_str(), "/4")) {
            _mlxlinkCommander->_userInput._splitPort = value[value.length() - 1] - '0';
            value.erase(value.size() - 2);
            _mlxlinkCommander->_splitted = true;
        }
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._labelPort);
        _mlxlinkCommander->_userInput._specifiedPort = true;
        return PARSE_OK;
    } else if (name == DEPTH_FLAG) {
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._depth);
        _mlxlinkCommander->_userInput._sendDepth = true;
        return PARSE_OK;
    } else if (name == PCIE_INDEX_FLAG) {
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._pcieIndex);
        _mlxlinkCommander->_userInput._sendPcieIndex = true;
        return PARSE_OK;
    } else if (name == NODE_FLAG) {
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._node);
        _mlxlinkCommander->_userInput._sendNode = true;
        return PARSE_OK;
    } else if (name == PCIE_LINKS_FLAG) {
        _sendRegFuncMap[SHOW_PCIE_LINKS] = SHOW_PCIE_LINKS;
        _mlxlinkCommander->_userInput._links = true;
        return PARSE_OK;
    } else if (name == DEVICE_DATA_FLAG) {
        _sendRegFuncMap[SHOW_DEVICE] = SHOW_DEVICE;
        return PARSE_OK;
    } else if (name == FEC_DATA_FLAG) {
        _sendRegFuncMap[SHOW_FEC] = SHOW_FEC;
        _mlxlinkCommander->_networkCmds++;
        return PARSE_OK;
    } else if (name == PAOS_FLAG) {
        _sendRegFuncMap[SEND_PAOS] = SEND_PAOS;
        _mlxlinkCommander->_userInput._paosCmd = toUpperCase(value);
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == PTYS_FLAG) {
        _sendRegFuncMap[SEND_PTYS] = SEND_PTYS;
        _mlxlinkCommander->_ptysSpeeds = _mlxlinkCommander->parseParamsFromLine(toUpperCase(value));
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == PPLM_FLAG) {
        _sendRegFuncMap[SEND_PPLM] = SEND_PPLM;
        _mlxlinkCommander->_userInput._pplmFec = toUpperCase(value);
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == FEC_SPEED_FLAG) {
        _mlxlinkCommander->_userInput._speedFec = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PPLR_FLAG) {
        _sendRegFuncMap[SEND_PPLR] = SEND_PPLR;
        _mlxlinkCommander->_userInput._pplrLB = toUpperCase(value);
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == PRBS_MODE_FLAG) {
        _sendRegFuncMap[SEND_PRBS] = SEND_PRBS;
        _mlxlinkCommander->_userInput._prbsMode = toUpperCase(value);
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == PPRT_PRBS_FLAG) {
        _mlxlinkCommander->_userInput._sendPprt = true;
        _mlxlinkCommander->_userInput._pprtMode = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PPTT_PRBS_FLAG) {
        _mlxlinkCommander->_userInput._sendPptt = true;
        _mlxlinkCommander->_userInput._ppttMode = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PPRT_RATE_FLAG) {
        _mlxlinkCommander->_userInput._pprtRate = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PPTT_RATE_FLAG) {
        _mlxlinkCommander->_userInput._ppttRate = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PRBS_LANES_FLAG) {
        std::vector<string> prbsLanesParams = _mlxlinkCommander->parseParamsFromLine(value);
        _mlxlinkCommander->getprbsLanesFromParams(prbsLanesParams);
        return PARSE_OK;
    } else if (name == BER_COLLECT_FLAG) {
        _sendRegFuncMap[SEND_BER_COLLECT] = SEND_BER_COLLECT;
        _mlxlinkCommander->_userInput._csvBer = value;
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == BER_LIMIT_FLAG) {
        _mlxlinkCommander->_userInput._testMode = toUpperCase(value);
        return PARSE_OK;
    } else if (name == ITERATION_FLAG) {
        _mlxlinkCommander->strToUint32((char*) value.c_str(), _mlxlinkCommander->_userInput._iteration);
        return PARSE_OK;
    } else if (name == BER_MONITOR_INFO_FLAG) {
        _sendRegFuncMap[SHOW_BER_MONITOR] = SHOW_BER_MONITOR;
        return PARSE_OK;
    } else if (name == PEPC_SHOW_FLAG) {
        _sendRegFuncMap[SHOW_EXTERNAL_PHY] = SHOW_EXTERNAL_PHY;
        return PARSE_OK;
    } else if (name == PEPC_FORCE_MODE_FLAG) {
        _mlxlinkCommander->_userInput._sendPepcForceMode = true;
        _mlxlinkCommander->_userInput._forceMode = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PEPC_AN_MODE_FLAG) {
        _mlxlinkCommander->_userInput._sendPepcANMode = true;
        _mlxlinkCommander->_userInput._anMode = toUpperCase(value);
        return PARSE_OK;
    } else if (name == PEPC_SET_FLAG) {
        _sendRegFuncMap[SEND_PEPC] = SEND_PEPC;
        _mlxlinkCommander->_uniqueCmds++;
        return PARSE_OK;
    } else if (name == PTYS_LINK_MODE_FORCE_FLAG) {
        _mlxlinkCommander->_linkModeForce = true;
        return PARSE_OK;
    } else if (name == LOG_FLAG) {
        _mlxlinkCommander->_userInput._logFilePath = value;
        return PARSE_OK;
    }
    return PARSE_ERROR;
}

int MlxlinkUi::run(int argc, char **argv)
{
    int exit_code = 0;
    createMlxlinkCommander();
    initCmdParser();
    ParseStatus rc = _cmdParser.ParseOptions(argc, argv);
    if (rc == PARSE_OK_WITH_EXIT) {
        return exit_code;
    } else if ((rc == PARSE_ERROR) || (rc == PARSE_ERROR_SHOW_USAGE)) {
        throw MlxRegException(
                  "failed to parse arguments. "
                  + string(_cmdParser.GetErrDesc()));
    }
    paramValidate();
    _mlxlinkCommander->_mf = mopen(_mlxlinkCommander->_device.c_str());
    if (!_mlxlinkCommander->_mf) {
        throw MlxRegException(
                  "Failed to open device: \"" + _mlxlinkCommander->_device + "\", "
                  + strerror(errno));
    }
    if (!MlxRegLib::isDeviceSupported(_mlxlinkCommander->_mf)) {
        throw MlxRegException("Device is not supported");
    }
    MlxRegLib::isAccessRegisterSupported(_mlxlinkCommander->_mf);
    _mlxlinkCommander->_mlxLinkLib = new MlxRegLib(_mlxlinkCommander->_mf,
            _mlxlinkCommander->_extAdbFile, _mlxlinkCommander->_useExtAdb);
    if (_mlxlinkCommander->_mlxLinkLib->isIBDevice() &&
            !_mlxlinkCommander->_mlxLinkLib->isAccessRegisterGMPSupported(
                    MACCESS_REG_METHOD_GET)) {
        MlxlinkRecord::printWar("Warning: AccessRegisterGMP Get() method is not supported.\n"
                 "         mlxlink has limited functionality", _mlxlinkCommander->_jsonRoot);
    }
    _mlxlinkCommander->_devID = _mlxlinkCommander->_mlxLinkLib->getDevId();
    _mlxlinkCommander->_isHCA = dm_dev_is_hca(_mlxlinkCommander->_devID);
    _mlxlinkCommander->labelToLocalPort();
    _mlxlinkCommander->checkValidFW();
    if (!_mlxlinkCommander->_userInput._pcie) {
        _mlxlinkCommander->_prbsTestMode = _mlxlinkCommander->inPrbsTestMode();
        _mlxlinkCommander->getCableParams();
    } else if (!_mlxlinkCommander->_userInput._sendDpn) {
        _mlxlinkCommander->initValidDPNList();
    }
    if (!(_mlxlinkCommander->_userInput._pcie &&
            _mlxlinkCommander->_validDpns.size() > 1)) {
        _mlxlinkCommander->_productTechnology = _mlxlinkCommander->getProductTechnology();
    }
    if (_mlxlinkCommander->_userInput._logFilePath != "") {
        _mlxlinkCommander->_mlxlinkLogger = new MlxlinkLogger(
                _mlxlinkCommander->_userInput._logFilePath );
    }
    commandsCaller();
    if (_mlxlinkCommander->_allUnhandledErrors != "") {
        exit_code = 1;
        if (!MlxlinkRecord::jsonFormat) {
            MlxlinkRecord::printErrorsSection(
                    _mlxlinkCommander->_mlxlinkMaps->_showErrorsTitle,
                    _mlxlinkCommander->_allUnhandledErrors);
        }
    }

    if (MlxlinkRecord::jsonFormat) {
        _mlxlinkCommander->prepareJsonOut();
        cout << _mlxlinkCommander->_jsonRoot.toStyledString();
    }

    cout << endl;
    return exit_code;
}
