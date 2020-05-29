/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_CONFIG_H_
#define _SETUP_CONFIG_H_
#ifndef VFRCOMPILE
#include <Uefi/UefiInternalFormRepresentation.h>
#endif
#include <ChipsetSetupConfig.h>

#pragma pack(1)

//
//  Setup Utility Structure
//
//-----------------------------------------------------------------------------------------------------------------
// Important!!! The following setup utility structure should be syncronize with OperationRegion MBOX in mailbox.asi.
// If you do NOT follow it, you may face on unexpected issue. The total size are 1200bytes.
// (Common 146bytes + Kernel 84bytes + Chipset 300bytes + OEM 70bytes + ODM 100bytes + Platform 500bytes)
//-----------------------------------------------------------------------------------------------------------------

//[-start-130709-IB05160465-modify]//
typedef struct {
  //
  // Kernel system configuration (offset 0~229, total 230 bytes)
  //
  #define _IMPORT_KERNEL_SETUP_
  #include <KernelSetupData.h>
  #undef _IMPORT_KERNEL_SETUP_

  //
  // Chipset system configuration (offset 230~529, total 300 bytes)
  //
  #define _IMPORT_CHIPSET_COMMON_SETUP_
  #include <ChipsetCommonSetupData.h>
  #undef _IMPORT_CHIPSET_COMMON_SETUP_

  //
  // Chipset system configuration (offset 530~1099, total 570 bytes)
  //
  #define _IMPORT_CHIPSET_SPECIFIC_SETUP_
  #include <ChipsetSpecificSetupData.h>
  #undef _IMPORT_CHIPSET_SPECIFIC_SETUP_

//OEM_Start
//Offset(1100);                 // This area must sync to SetupConfig.h
  UINT8         OEMRSV[100];    // Dummy area. Reserve for OEM team, really structure will fill in SetupConfig.h
//OEM_End

//OEM_Start
//Offset(1200);                 // This are must sync to SetupConfig.h
  UINT8         ODMRSV[100];   // Dummy area. Reserve for ODM, really structure will fill in SetupConfig.h
//OEM_End

//=======================================================================================
// Note: If change sizeof(SYSTEM_CONFIGURATION) in SetupConfig.h, 
//         must update really structure size to (PcdSetupConfigSize) in Project.dsc!!!
//=======================================================================================

} SYSTEM_CONFIGURATION;
//[-end-130709-IB05160465-modify]//
#pragma pack()

#endif
