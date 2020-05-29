/** @file

  Dump whole MEBX_BPF and serial out.

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

#include "BiosExtensionLoader.h"

/**
  Dump MEBx BIOS Params

  @param[in] MebxBiosParams       MEBx BIOS params

  @retval None
**/
VOID
DxeMebxBiosParamsDebugDump (
  IN MEBX_BPF                     *MebxBiosParams
  )
{
  DEBUG ((EFI_D_INFO, "\n------------------------ MebxBiosParams Dump Begin -----------------\n"));
  DEBUG ((EFI_D_INFO, " BpfVersion : 0x%x\n", MebxBiosParams->BpfVersion));
  DEBUG ((EFI_D_INFO, " CpuReplacementTimeout : 0x%x\n", MebxBiosParams->CpuReplacementTimeout));
  DEBUG ((EFI_D_INFO, " ActiveRemoteAssistanceProcess : 0x%x\n", MebxBiosParams->ActiveRemoteAssistanceProcess));
  DEBUG ((EFI_D_INFO, " CiraTimeout : 0x%x\n", MebxBiosParams->CiraTimeout));
  DEBUG ((EFI_D_INFO, " OemFlags : 0x%x\n", MebxBiosParams->OemFlags));
  DEBUG ((EFI_D_INFO, "MebxDebugFlags ---\n"));
  DEBUG ((EFI_D_INFO, " Port80 : 0x%x\n", MebxBiosParams->MebxDebugFlags.Port80));
  DEBUG ((EFI_D_INFO, " MeBiosSyncDataPtr : 0x%x\n", MebxBiosParams->MeBiosSyncDataPtr));
  DEBUG ((EFI_D_INFO, " UsbKeyDataStructurePtr : 0x%x\n", MebxBiosParams->UsbKeyDataStructurePtr));
  DEBUG ((EFI_D_INFO, "OemResolutionSettings ---\n"));
  DEBUG ((EFI_D_INFO, " MebxNonUiTextMode : 0x%x\n", MebxBiosParams->OemResolutionSettings.MebxNonUiTextMode));
  DEBUG ((EFI_D_INFO, " MebxUiTextMode : 0x%x\n", MebxBiosParams->OemResolutionSettings.MebxUiTextMode));
  DEBUG ((EFI_D_INFO, " MebxUiTextMode : 0x%x\n", MebxBiosParams->OemResolutionSettings.MebxGraphicsMode));
  DEBUG ((EFI_D_INFO, "\n------------------------ MebxBiosParams Dump End -------------------\n"));
}

