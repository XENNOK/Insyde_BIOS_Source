/** @file
  This function offers an interface to modify DXE_CPU_PLATFORM_POLICY_PROTOCOL Data before the system
  installs DXE_CPU_PLATFORM_POLICY_PROTOCOL.
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

#include <Library/DxeOemSvcChipsetLib.h>
typedef UINT16  STRING_REF;
#include <Protocol/CpuPlatformPolicy.h>
/**
 This function offers an interface to modify DXE_CPU_PLATFORM_POLICY_PROTOCOL Data before the system
 installs DXE_CPU_PLATFORM_POLICY_PROTOCOL.

 @param[in, out]    *CPUPlatformPolicy  On entry, points to DXE_CPU_PLATFORM_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_CPU_PLATFORM_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxeCpuPlatformPolicy (
  IN OUT VOID                           *CPUPlatformPolicy
  )
{
  DXE_CPU_PLATFORM_POLICY_PROTOCOL      *mCPUPlatformPolicy;

  mCPUPlatformPolicy = CPUPlatformPolicy;
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
