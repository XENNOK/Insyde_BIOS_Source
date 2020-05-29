/** @file
 PEI IPMI LPC library implement code - Program LPC.

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


#include <Library/PeiIpmiLpcLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Base.h>
#include <PchRegs.h>
#include <PchAccess.h>
#include <PciAccess.h>


UINT8  IoDecodeRegisterTable[] = {
  R_PCH_LPC_GEN1_DEC,
  R_PCH_LPC_GEN2_DEC,
  R_PCH_LPC_GEN3_DEC,
  R_PCH_LPC_GEN4_DEC
  };


/**
 Programming PCH LPC Generic I/O Decode Range Register for BMC use.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiSmmBaseAddress  Value from FixedPcdGet16 (PcdIpmiSmmBaseAddress).
 @param[in]         IpmiSmmRegOffset    Value from FixedPcdGet16 (PcdIpmiSmmRegOffset).
 @param[in]         IpmiPostBaseAddress Value from FixedPcdGet16 (PcdIpmiPostBaseAddress).
 @param[in]         IpmiPostRegOffset   Value from FixedPcdGet16 (PcdIpmiPostRegOffset).
 @param[in]         IpmiOsBaseAddress   Value from FixedPcdGet16 (PcdIpmiOsBaseAddress).
 @param[in]         IpmiOsRegOffset     Value from FixedPcdGet16 (PcdIpmiOsRegOffset).

 @retval EFI_SUCCESS                    Programming success.
 @retval EFI_OUT_OF_RESOURCES           No unused Generic Decode Range Register can decode.
 @retval EFI_UNSUPPORTED                Does not implement this function.
*/
EFI_STATUS
IpmiProgramLpc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINT16                       IpmiSmmBaseAddress,
  IN       UINT16                       IpmiSmmRegOffset,
  IN       UINT16                       IpmiPostBaseAddress,
  IN       UINT16                       IpmiPostRegOffset,
  IN       UINT16                       IpmiOsBaseAddress,
  IN       UINT16                       IpmiOsRegOffset
  )
{
  UINT8                                 Index;
  BOOLEAN                               DecodeSuccess;
  UINT16                                DecodeBaseAddr;
  UINT16                                DecodeAddrEnd;

  //
  // Check if the Address Range in the IoDecodeTable is already programmed.
  //
  Index = 0;
  DecodeSuccess = FALSE;
  while (Index < (sizeof (IoDecodeRegisterTable) / sizeof (UINT8))) {
    if (PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) != 0) {
      DecodeBaseAddr = (PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) & (UINT16)(~BIT0));
      DecodeAddrEnd = DecodeBaseAddr + (UINT16)((PchLpcPciCfg32 (IoDecodeRegisterTable[Index]) >> 16) + 0x03);
      if ((DecodeBaseAddr <= (IpmiPostBaseAddress & 0xFFF0)) &&
          ((IpmiPostBaseAddress | 0x000F) <= DecodeAddrEnd)) {
        DecodeSuccess = TRUE;
        break;
      }
    }
    Index++;
  }

  //
  // Program PCH LPC I/F Generic decode address
  //
  if (DecodeSuccess == FALSE) {
    Index = 0;
    while (Index < (sizeof (IoDecodeRegisterTable) / sizeof (UINT8))) {
      if (PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) == 0) {
        MmioWrite32 (
          MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, IoDecodeRegisterTable[Index++]),
          (UINT32)((UINT32)(((~0xFFF0) & 0xFC) << 16) + (UINT32)((IpmiPostBaseAddress & 0xFFF0) + 1))
          );
        DecodeSuccess = TRUE;
        break;
      }
      Index++;
    }
  }

  if (DecodeSuccess == FALSE) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;

}

