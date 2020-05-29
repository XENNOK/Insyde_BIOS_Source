/** @file
  This file offers interface to get OemInt15VbiosFunctionHook array and do additional 
  VbiosHookCallBack function that are list on OemInt15VbiosFunctionHook array.
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

#include <Library/SmmOemSvcChipsetLib.h>
#if 0 // Sample Implementation

#define INT15_HOOK_OOOO           0x5F55
//
// INT15 Service Function HOOK 
// This array stores OEM requested INT15 Hook Function Numbers that will
// be installed (registered) later. Detain meanings of such Function Numbers
// can be found in VBIOS Spec. Ex: 0x5F14 is "Get/Update System BIOS State"
//
#define OemINT15_VBIOS_FUNCTION_HOOK_LIST \
          INT15_HOOK_OOOO

STATIC UINT16 OemInt15VbiosFunctionHook[] = { OemINT15_VBIOS_FUNCTION_HOOK_LIST };
#endif
/**
 This function provides an interface to get OemInt15VbiosFunctionHook array.
 Caller will use this to install additional VbiosHookCallBack function.

 @param[out]        *OemInt15VbiosFunctionlist  Point to OemInt15VbiosFunctionHook array.
 @param[out]        *Size               The number of entries in OemInt15VbiosFunctionHook array.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
GetOemInt15VbiosFunctionlist (
  IN OUT UINT16                         **OemInt15VbiosFunctionlist,
  OUT UINT16                            *Size
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
  *OemInt15VbiosFunctionlist = OemInt15VbiosFunctionHook;
  *Size = sizeof (OemInt15VbiosFunctionHook) / sizeof (UINT16);
  return EFI_MEDIA_CHANGED;
#endif
  return EFI_UNSUPPORTED;
}
/**
 This function provides an interface to do additional VbiosHookCallBack function that are list on 
 OemInt15VbiosFunctionHook array.

 @param[in]         Int15FunNum         Int15 function number.
 @param[in out]     CpuRegs             The structure containing CPU Registers (AX, BX, CX, DX etc.).
 @param[in]         Context             Context.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcVbiosHookCallBack (
  IN UINT32                             Int15FunNum,
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs,
  IN VOID                               *Context
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
  switch (Int15FunNum) {
    case INT15_HOOK_OOOO :
      CpuRegs->X.CX |= 0;
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F; // Function supported and successful 
      break;

    default:
      break;
  }
#endif
  return EFI_UNSUPPORTED;
}
