/*
 * Copyright (C) Jan 2013 Mellanox Technologies Ltd. All rights reserved.
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

#ifndef _MTCR_COM_DEFS_H
#define _MTCR_COM_DEFS_H

#ifdef __WIN__

#include<winsock2.h>
#include <windows.h>

#ifdef MTCR_EXPORTS
#define MTCR_API __declspec(dllexport)
#else
#define MTCR_API __declspec(dllimport)
#endif

#if defined(_MSC_VER)
    #include <stdint.h>
#else
    typedef __int8           int8_t;
    typedef __int16          int16_t;
    typedef __int32          int32_t;
    typedef __int64          int64_t;
#endif // !_MSC_VER
    typedef unsigned __int8  u_int8_t;
    typedef unsigned __int16 u_int16_t;
    typedef unsigned __int32 u_int32_t;
    typedef unsigned __int64 u_int64_t;

#if defined(_WIN64)
    typedef __int64 MT_long_ptr_t;
    typedef unsigned __int64 MT_ulong_ptr_t;
#else
    typedef _W64 long MT_long_ptr_t;
    typedef _W64 unsigned long MT_ulong_ptr_t;
#endif

#elif defined(__DJGPP__)

    typedef unsigned char    u_int8_t;
    typedef char             int8_t;
    typedef unsigned short   u_int16_t;
    typedef short            int16_t;
    typedef unsigned int     u_int32_t;
    typedef long             int32_t;
    typedef unsigned long long u_int64_t;
    typedef long long        int64_t;

#define MTCR_API

#else  /* UNIX */

#include <sys/types.h>
#define MTCR_API

#endif

//#ifndef USE_IB_MGT
typedef struct mib_private_t {
        int dummy;
} MIB_Private;
//#else
//#include "mtcr_ib_private.h"
//#endif

typedef enum MError {
    ME_OK = 0,
    ME_ERROR,
    ME_BAD_PARAMS,
    ME_CR_ERROR,
    ME_NOT_IMPLEMENTED,
    ME_SEM_LOCKED,
    ME_MEM_ERROR,
    ME_TIMEOUT,

    ME_MAD_SEND_FAILED,
    ME_UNKOWN_ACCESS_TYPE,
    ME_UNSUPPORTED_DEVICE,
    ME_REG_NOT_SUPPORTED,

    ME_PCI_READ_ERROR,
    ME_PCI_WRITE_ERROR,
    ME_PCI_SPACE_NOT_SUPPORTED,
    ME_PCI_IFC_TOUT,

    ME_UNSUPPORTED_OPERATION,
    ME_UNSUPPORTED_ACCESS_TYPE,

    // errors regarding REG_ACCESS
    ME_REG_ACCESS_OK = 0,
    ME_REG_ACCESS_BAD_STATUS_ERR = 0x100,
    ME_REG_ACCESS_BAD_METHOD,
    ME_REG_ACCESS_NOT_SUPPORTED,
    ME_REG_ACCESS_DEV_BUSY,
    ME_REG_ACCESS_VER_NOT_SUPP,
    ME_REG_ACCESS_UNKNOWN_TLV,
    ME_REG_ACCESS_REG_NOT_SUPP,
    ME_REG_ACCESS_CLASS_NOT_SUPP,
    ME_REG_ACCESS_METHOD_NOT_SUPP,
    ME_REG_ACCESS_BAD_PARAM,
    ME_REG_ACCESS_RES_NOT_AVLBL,
    ME_REG_ACCESS_MSG_RECPT_ACK,
    ME_REG_ACCESS_UNKNOWN_ERR,
    ME_REG_ACCESS_SIZE_EXCCEEDS_LIMIT,
    ME_REG_ACCESS_CONF_CORRUPT,
    ME_REG_ACCESS_LEN_TOO_SMALL,
    ME_REG_ACCESS_BAD_CONFIG,
    ME_REG_ACCESS_ERASE_EXEEDED,
    ME_REG_ACCESS_INTERNAL_ERROR,

    // errors regarding ICMD
    ME_ICMD_STATUS_CR_FAIL = 0x200,       // cr-space access failure
    ME_ICMD_INVALID_OPCODE,
    ME_ICMD_INVALID_CMD,
    ME_ICMD_OPERATIONAL_ERROR,
    ME_ICMD_BAD_PARAM,
    ME_ICMD_BUSY,
    ME_ICMD_INIT_FAILED,
    ME_ICMD_NOT_SUPPORTED,
    ME_ICMD_STATUS_SEMAPHORE_TO,          // timed out while trying to take semaphore
    ME_ICMD_STATUS_EXECUTE_TO,            // timed out while waiting for command to execute
    ME_ICMD_STATUS_IFC_BUSY,
    ME_ICMD_STATUS_ICMD_NOT_READY,
    ME_ICMD_UNSUPPORTED_ICMD_VERSION,
    ME_ICMD_UNKNOWN_STATUS,
    ME_ICMD_ICM_NOT_AVAIL,
    ME_ICMD_WRITE_PROTECT,
    ME_ICMD_SIZE_EXCEEDS_LIMIT,

    //errors regarding Tools CMDIF
    ME_CMDIF_BUSY = 0x300,
    ME_CMDIF_TOUT,
    ME_CMDIF_BAD_STATUS,
    ME_CMDIF_BAD_OP,
    ME_CMDIF_NOT_SUPP,
    ME_CMDIF_BAD_SYS,
    ME_CMDIF_UNKN_TLV,
    ME_CMDIF_RES_STATE,
    ME_CMDIF_UNKN_STATUS,

    //errors regarding MAD IF
    ME_MAD_BUSY = 0x400,
    ME_MAD_REDIRECT,
    ME_MAD_BAD_VER,
    ME_MAD_METHOD_NOT_SUPP,
    ME_MAD_METHOD_ATTR_COMB_NOT_SUPP,
    ME_MAD_BAD_DATA,
    ME_MAD_GENERAL_ERR,

    ME_LAST
} MError;

// typedefs for UEFI
#ifdef UEFI_BUILD
#include <mft_uefi_common.h>
#endif
typedef enum MType_t {
    MST_ERROR = 0x0,
    MST_FPGA = 0x1,     // Unsupported
    MST_MLNXOS = 0x2,
    MST_LPC = 0x4,
    MST_PCI = 0x8,
    MST_PCICONF = 0x10,
    /*MST_CALBR,*/
    MST_USB = 0x20,
    MST_IB = 0x40,
    MST_IF = 0x80,
    MST_PPC = 0x100,
    MST_USB_DIMAX = 0x200,
    MST_FWCTX = 0x400,
    MST_REMOTE = 0x800,
#ifdef ENABLE_MST_DEV_I2C
    MST_DEV_I2C = 0x1000,
#endif
    // 0x2000 reserved
    MST_FPGA_ICMD = 0x4000,
    MST_CABLE = 0x8000,
    MST_FPGA_DRIVER = 0x10000,
    MST_DRIVER_CONF = 0x40000,
    MST_DRIVER_CR = 0x80000,
    MST_DEFAULT = 0xffffffff & ~MST_CABLE & ~MST_FPGA & ~MST_FPGA_ICMD & ~MST_FPGA_DRIVER
} MType;

typedef enum DType_t {
    MST_GAMLA, MST_TAVOR, MST_DIMM, MST_NOADDR
} DType;
#define MST_ANAFA2 MST_TAVOR
#define MST_EEPROM MST_GAMLA
typedef enum Mdevs_t {
    MDEVS_GAMLA = 0x01, /*  Each device that actually is a Gamla */
    MDEVS_I2CM = 0x02, /*  Each device that can work as I2C master */
    MDEVS_MEM = 0x04, /*  Each device that is a memory driver (vtop) */
    MDEVS_TAVOR_DDR = 0x08, /*  Each device that maps to DDR */
    MDEVS_TAVOR_UAR = 0x10, /*  Each device that maps to UAR */
    MDEVS_TAVOR_CR = 0x20, /*  Each device that maps to CR */
    MDEVS_IF = 0x40, /*  Standard device  interface */
    MDEVS_REM = 0x80, /*  Remote devices */
    MDEVS_PPC = 0x100, /*  PPC devices */
    MDEVS_DEV_I2C = 0x200, /* Generic linux kernel i2c device */
    MDEVS_IB = 0x400, /* Cr access over IB Mads */
    MDEVS_MLNX_OS = 0x800, /* access by CmdIf in MlnxOS */
    MDEVS_FWCTX = 0x900, /* access by func/context (like UEFI) */
    MDEVS_LPC = 0x1000,/* Access LPC region */
    MDEVS_FPGA = 0x2000,/* Access LPC region */
    MDEVS_FPGA_NEWTON = 0x4000,/* Access LPC region */
    MDEVS_CABLE = 0x8000,
    MDEVS_TAVOR = (MDEVS_TAVOR_DDR | MDEVS_TAVOR_UAR | MDEVS_TAVOR_CR), MDEVS_ALL = 0xffffffff
} Mdevs;

typedef enum {
    MACCESS_REG_METHOD_GET = 1,
    MACCESS_REG_METHOD_SET = 2
} maccess_reg_method_t;

typedef enum {
    AS_ICMD = 3, AS_CR_SPACE = 2, AS_SEMAPHORE = 0xa
} address_space_t;

typedef struct vf_info_t {
    char dev_name[512];
    u_int16_t domain;
    u_int8_t bus;
    u_int8_t dev;
    u_int8_t func;
    char** net_devs;      // Null terminated array
    char** ib_devs;       // Null terminated array
} vf_info;

typedef struct dev_info_t {
        Mdevs type;
        char dev_name[512];
        int ul_mode;

        union {
                struct {
                        u_int16_t domain;
                        u_int8_t bus;
                        u_int8_t dev;
                        u_int8_t func;

                        u_int16_t dev_id;
                        u_int16_t vend_id;
                        u_int32_t class_id;
                        u_int16_t subsys_id;
                        u_int16_t subsys_vend_id;

                        char cr_dev[512];
                        char conf_dev[512];
                        char** net_devs;      // Null terminated array
                        char** ib_devs;       // Null terminated array
                        char numa_node[4096];     //
                        vf_info * virtfn_arr;
                        u_int16_t virtfn_count;
                } pci;

                struct {
                        u_int32_t mtusb_serial;
                        u_int32_t TBD;
                } usb;

                struct {
                        u_int32_t TBD;
                } ib;

                struct {
                        u_int32_t TBD;
                } remote;
        };
} dev_info;


typedef enum {
    RA_MFPA=0x9010,
    RA_MFBA=0x9011,
    RA_MFBE=0x9012,
} reg_access_t;

typedef struct mfile_t mfile;

#endif
