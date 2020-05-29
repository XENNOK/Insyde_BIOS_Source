/** @file

  Register definitions for PTT HCI (Platform Trust Technology - Host Controller Interface).

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position


;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _PTT_HCI_REGS_H_
#define _PTT_HCI_REGS_H_

#ifdef PTT_FLAG

///
/// FTPM HCI register base address
///
#define R_PTT_HCI_BASE_ADDRESS             0xFED70000

///
/// FTPM HCI Control Area
///
#define R_PTT_HCI_CA_RSVD                  0x00
#define R_PTT_HCI_CA_ERROR                 0x04
#define R_PTT_HCI_CA_CANCEL                0x08
#define R_PTT_HCI_CA_START                 0x0C
#define R_PTT_HCI_CA_INT_RSVD              0x10
#define R_PTT_HCI_CA_CMD_SZ                0x18
#define R_PTT_HCI_CA_CMD                   0x1C
#define R_PTT_HCI_CA_RSP_SZ                0x24
#define R_PTT_HCI_CA_RSP                   0x28

///
/// FTPM HCI Private Area
///
#define R_PTT_HCI_CMD                      0x40
#define R_PTT_HCI_STS                      0x44

///
/// FTPM HCI Command and Response Buffer
///
#define R_PTT_HCI_CRB                      0x80

///
/// R_PTT_HCI_STS Flags
///
#define B_PTT_HCI_STS_ENABLED              0x00000001 ///< BIT0
#define B_PTT_HCI_STS_READY                0x00000002 ///< BIT1
#define B_PTT_HCI_STS_ACM_AS_CRTM          0x00000004 ///< BIT2
#define B_PTT_HCI_STS_STARTUP_EXEC         0x00000008 ///< BIT3

///
/// Value written to R_PTT_HCI_CMD and CA_START
/// to indicate that a command is available for processing
///
#define V_PTT_HCI_COMMAND_AVAILABLE_START  0x00000001
#define V_PTT_HCI_COMMAND_AVAILABLE_CMD    0x00000000
#define V_PTT_HCI_BUFFER_ADDRESS_RDY       0x00000003

///
/// Ignore bit setting mask for WaitRegisterBits
///
#define V_PTT_HCI_IGNORE_BITS              0x00000000

///
/// All bits clear mask for WaitRegisterBits
///
#define V_PTT_HCI_ALL_BITS_CLEAR           0xFFFFFFFF
#define V_PTT_HCI_START_CLEAR              0x00000001

///
/// Max FTPM command/reponse buffer length
///
#define S_PTT_HCI_CRB_LENGTH               3968 ///< 0xFED70080:0xFED70FFF = 3968 Bytes

#endif /// PTT_FLAG
#endif
