/** @file
  Include file for InstallVerbTable PEIM.

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INSTALL_VERB_TABLE_H_
#define _INSTALL_VERB_TABLE_H_

#include <Base.h>
#include <Ppi/Stall.h>
#include <Ppi/VerbTable.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiChipsetSvcLib.h>
//[-start-130319-12360001-add]//
#include <Library/PcdLib.h>
//[-end-130319-12360001-add]//

#define STALL_TIME                      100
#define DETECT_CODEC_STALL_TIME         1
#define HDA_INIT_FAIL_RETRY_STALL_TIME  300
#define TIME_OUT_LOOP_INIT              0x00
#define TIME_OUT_MAX_LOOP               1000
#define DETECT_CODEC_TIME_OUT_MAX_LOOP  10000
#define HDA_MAX_SDIN_NUM                0x0F
#define HDA_MAX_SDIN_FLG                0x7FFF
#define HDA_SDIN_INIT                   0x01
#define HDA_SDIN_NUM                    0x04
#define HDA_SDIN_FLG                    0x0F
#define HDA_NO_CODEC                    0x00

//
// Codec Parameters and Controls Command
//
#define CONTROL_GET_PARAMETER       0xF00
#define PARAMETER_VENDOR_DEVICE_ID  0x00
#define PARAMETER_REVISION_ID       0x02
//[-start-130410-12360005-add]//
#define CLEAN_CODEC_ADDRESS_MASK    (~0xF0000000)
//[-end-130410-12360005-add]//

//
// Register Definition
//
#define HDA_REG_GCAP                0x00
#define HDA_REG_GCTL                0x08
#define HDA_REG_GCTL_BIT_CRST       BIT0
#define HDA_REG_WAKEEN              0x0C
#define HDA_REG_STATESTS            0x0E
#define HDA_REG_STATESTS_BIT_SDIN0  0x01
#define HDA_REG_ICW                 0x60
#define HDA_REG_IRR                 0x64
#define HDA_REG_ICS                 0x68
#define HDA_REG_ICS_BIT_ICB         BIT0
#define HDA_REG_ICS_BIT_IRV         BIT1

//
// Function Definition
//
EFI_STATUS
PeimInstallVerbTableInitEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
InstallVerbTablePpi (
  IN CONST EFI_PEI_SERVICES                      **PeiServices
  );

EFI_STATUS
InstallVerbTable (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *OemVerbTableAddress
  );

EFI_STATUS
HdaInitialize (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  );

EFI_STATUS
HdaReset (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  );

EFI_STATUS
FindOutCodec (
  IN     EFI_PEI_SERVICES                  **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT16                            *SdinWake
  );

EFI_STATUS
DeliverCommandToCodec (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi,
  IN UINT32                                CommandData
  );

EFI_STATUS
ReceiveCodecData (
  IN     EFI_PEI_SERVICES                  **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT32                            *ResponseData
  );

EFI_STATUS
ChipsetHdaInitHook (
  IN UINT32                                HdaBar
  );

//[-start-130410-12360005-add]//
EFI_STATUS
ConfigureVerbTable (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi,
  IN UINT16                                SdinWake,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *VerbTableHeaderDataAddress
  );
//[-end-130410-12360005-add]//

#endif
