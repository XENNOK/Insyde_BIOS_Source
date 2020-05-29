/** @file
  This file defines the PCH S3 support Protocol.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2008 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchS3Support.h

@brief
  This file defines the PCH S3 support Protocol.

**/
#ifndef _PCH_S3_SUPPORT_PROTOCOL_H_
#define _PCH_S3_SUPPORT_PROTOCOL_H_

///
/// Define the PCH S3 Support protocol GUID
///
#define EFI_PCH_S3_SUPPORT_PROTOCOL_GUID \
  { 0x2224aee3, 0x8d0b, 0x480a, { 0xb2, 0x72, 0x2a, 0xbe, 0x92, 0xcd, 0x4e, 0x78 } }

#include "Protocol/PchPlatformPolicy.h"

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                             gEfiPchS3SupportProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_PCH_S3_SUPPORT_PROTOCOL EFI_PCH_S3_SUPPORT_PROTOCOL;

typedef enum {
  PchS3ItemTypeSendCodecCommand,
  PchS3ItemTypeInitPcieRootPortDownstream,
  PchS3ItemTypePcieSetPm,
  PchS3ItemTypeProgramIobp,
  PchS3ItemTypeMax
} EFI_PCH_S3_DISPATCH_ITEM_TYPE;

///
/// It's better not to use pointer here because the size of pointer in DXE is 8, but it's 4 in PEI
/// plug 4 to ParameterSize in PEIM if you really need it
///
typedef struct {
  UINT32                        HdaBar;
  UINT32                        CodecCmdData;
} EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND;

typedef struct {
  UINT8                         RootPortBus;
  UINT8                         RootPortDevice;
  UINT8                         RootPortFunc;
  UINT8                         TempBusNumberMin;
  UINT8                         TempBusNumberMax;
} EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM;

typedef struct {
  UINT8                                RootPortBus;
  UINT8                                RootPortDevice;
  UINT8                                RootPortFunc;
  PCH_PCI_EXPRESS_ASPM_CONTROL         RootPortAspm;
  UINT8                                NumOfDevAspmOverride;
  UINT32                               DevAspmOverrideAddr;
  UINT8                                TempBusNumberMin;
  UINT8                                TempBusNumberMax;
  UINT8                                NumOfDevLtrOverride;
  UINT32                               DevLtrOverrideAddr;
  UINT32                               PchPwrOptPcie;
  PCH_PCIE_EXPRESS_L1SUBSTATES_CONTROL L1SubstatesConfig;
  UINT8                                PolicyRevision;
  BOOLEAN                              FirstRPToSetPm;
  BOOLEAN                              L1SupportedInAllEnabledPorts;
  BOOLEAN                              ClkreqSupportedInAllEnabledPorts;
} EFI_PCH_S3_PARAMETER_PCIE_SET_PM;

typedef struct {
  UINT32                        RootComplexBar;
  UINT32                        Address;
  UINT32                        AndMask;
  UINT32                        OrMask;
} EFI_PCH_S3_PARAMETER_PROG_IOBP;

typedef struct {
  EFI_PCH_S3_DISPATCH_ITEM_TYPE Type;
  VOID                          *Parameter;
} EFI_PCH_S3_DISPATCH_ITEM;

//
// Member functions
//
/**
  Set an item to be dispatched at S3 resume time. At the same time, the entry point
  of the PCH S3 support image is returned to be used in subsequent boot script save
  call

  @param[in] This                       Pointer to the protocol instance.
  @param[in] DispatchItem               The item to be dispatched.
  @param[out] S3DispatchEntryPoint      The entry point of the PCH S3 support image.

  @retval EFI_STATUS                    Successfully completed.
  @retval EFI_OUT_OF_RESOURCES          Out of resources.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_PCH_S3_SUPPORT_SET_S3_DISPATCH_ITEM) (
  IN     EFI_PCH_S3_SUPPORT_PROTOCOL   * This,
  IN     EFI_PCH_S3_DISPATCH_ITEM      * DispatchItem,
  OUT    EFI_PHYSICAL_ADDRESS          * S3DispatchEntryPoint
  );

///
/// Protocol definition
///
/// This is Protocol is used to set an item to be dispatched at S3 resume time.
/// At the same time, the entry point of the PCH S3 support image is returned to
/// be used in subsequent boot script save call.
///
struct _EFI_PCH_S3_SUPPORT_PROTOCOL {
  EFI_PCH_S3_SUPPORT_SET_S3_DISPATCH_ITEM SetDispatchItem;  ///< Set the item to be dispatched at S3 resume time.
};

#endif
