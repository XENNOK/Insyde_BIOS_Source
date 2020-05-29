/** @file
  Provide OEM to define SLP string which is based on OEM specification.

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

#include <Library/BaseMemoryLib.h>
#include <Library/DxeOemSvcKernelLib.h>

//
// Maximum length of OemSLPString is 16 bytes.
//
#define OEM_SLP_STRING_MAX_LENGTH 16

//
// module variables
//
CHAR8        OemSLPString[OEM_SLP_STRING_MAX_LENGTH] = {'P','L','A','T','F','O','R','M','-','C','R','B','0','0','0','0'};
UINTN        SlpDedicatedAddress = 0xFE810;

/**
  This OemService provides OEM to define SLP string which based on OEM specification. 
  This service only supports the legacy OS (XP system).

  @param[out]  *SlpStringLength      The length of SLP string.
  @param[out]  *SlpStringAddress     A pointer to the address of SLP string. 

  @retval      EFI_SUCCESS           Set SLP string to specific memory address successfully.
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallLegacyBiosOemSlp (
  OUT UINTN                                 *SlpLength,
  OUT UINTN                                 *SlpAddress
  )
{
  *SlpLength = OEM_SLP_STRING_MAX_LENGTH;  
  *SlpAddress = SlpDedicatedAddress;
  CopyMem  ((VOID *)SlpDedicatedAddress, (CHAR8 *)OemSLPString, *SlpLength);
 
  return EFI_SUCCESS;
}
