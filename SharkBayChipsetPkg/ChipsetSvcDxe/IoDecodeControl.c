/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcIoDecodeControl().
 The function DxeCsSvcIoDecodeControl() use chipset services to set
 IO decode region.
	
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Protocol/H2ODxeChipsetServices.h>
#include <Library/IoLib.h>
#include <PchAccess.h>
#include <SaAccess.h>

typedef struct {
  IO_DECODE_TYPE      IoType;
  IO_DECODE_ADDRESS   IoAddress;
  UINT16              IoDecMask;
  UINT16              IoDecValue;
  UINT16              IoDecEn;
} IO_DECODE_CONTROL_TABLE;

IO_DECODE_CONTROL_TABLE mIoDecConTable[] = {
  // {IoType, IoAddress, IoDecMask, IoDecValue, IoDecEn}
  {IoDecodeFdd, AddressFdd3F0, B_PCH_LPC_FDD_DEC, 0x0000, B_PCH_LPC_ENABLES_FDD_EN},
  {IoDecodeFdd, AddressFdd370, B_PCH_LPC_FDD_DEC, B_PCH_LPC_FDD_DEC, B_PCH_LPC_ENABLES_FDD_EN},
  {IoDecodeLpt, AddressLpt378, B_PCH_LPC_LPT_DEC, 0x0000, B_PCH_LPC_ENABLES_LPT_EN},
  {IoDecodeLpt, AddressLpt278, B_PCH_LPC_LPT_DEC, 0x0100, B_PCH_LPC_ENABLES_LPT_EN},
  {IoDecodeLpt, AddressLpt3BC, B_PCH_LPC_LPT_DEC, 0x0200, B_PCH_LPC_ENABLES_LPT_EN},
  {IoDecodeComA, AddressCom3F8, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_3F8, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom2F8, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_2F8, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom220, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_220, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom228, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_228, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom238, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_238, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom2E8, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_2E8, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom338, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_338, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComA, AddressCom3E8, B_PCH_LPC_COMA_DEC, V_PCH_LPC_COMA_3E8, B_PCH_LPC_ENABLES_COMA_EN},
  {IoDecodeComB, AddressCom3F8, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_3F8, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom2F8, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_2F8, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom220, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_220, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom228, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_228, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom238, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_238, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom2E8, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_2E8, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom338, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_338, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeComB, AddressCom3E8, B_PCH_LPC_COMB_DEC, V_PCH_LPC_COMB_3E8, B_PCH_LPC_ENABLES_COMB_EN},
  {IoDecodeCnf2, AddressCnf24E4F, 0, 0, B_PCH_LPC_ENABLES_CNF2_EN},
  {IoDecodeCnf1, AddressCnf12E2F, 0, 0, B_PCH_LPC_ENABLES_CNF1_EN},
  {IoDecodeMc, AddressMc6266, 0, 0, B_PCH_LPC_ENABLES_MC_EN},
  {IoDecodeKbc, AddressKbc6064, 0, 0, B_PCH_LPC_ENABLES_KBC_EN},
  {IoDecodeGameh, AddressGameh208, 0, 0, B_PCH_LPC_ENABLES_GAMEH_EN},
  {IoDecodeGamel, AddressGamel200, 0, 0, B_PCH_LPC_ENABLES_GAMEL_EN},
};

/**
 Allocate Generic I/O.
 
 @param[in]         Address             Decode address
 @param[in]         Length              Decode length
                    
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER One of the parameters has an invalid value
                                          1. Address big then 0xFFFF
                                          2. Length need to be 4-byte aligned
*/
STATIC
EFI_STATUS
AllocateGenericIo (
  IN UINTN              Address,
  IN UINT16             Length
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  UINT8                 LpcGenDecRegs[] = {R_PCH_LPC_GEN1_DEC, R_PCH_LPC_GEN2_DEC, R_PCH_LPC_GEN3_DEC, R_PCH_LPC_GEN4_DEC};
  UINT32                LpcGenDesRegValue;
  UINTN                 DecodeBaseAddress;
  UINTN                 DecodeRangeAddress;

  Status = EFI_INVALID_PARAMETER;

  // The PCH Does not provide decode down to the word or byte level
  if (((Length & 0x0003) != 0) || (Address > 0xFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  // Update current decode settings
  for (Index = 0; Index < sizeof (LpcGenDecRegs) / sizeof (UINT8); Index++) {
    LpcGenDesRegValue = PchLpcPciCfg32 (LpcGenDecRegs[Index]);
    DecodeBaseAddress = (UINTN)(LpcGenDesRegValue & B_PCH_LPC_GEN1_DEC_IOBAR);
    if (DecodeBaseAddress == Address) {
      if (Length == 0) {
        // Disable IO Decode
        MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, LpcGenDecRegs[Index]) = 0;
      } else {
        DecodeRangeAddress = (((UINTN)(Length & B_PCH_LPC_GEN1_DEC_IOBAR))<<16);
        MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, LpcGenDecRegs[Index]) &= (~B_PCH_LPC_GEN1_DEC_IODRA);
        MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, LpcGenDecRegs[Index]) |= (DecodeRangeAddress + B_PCH_LPC_GEN1_DEC_EN);
      }
      Status = EFI_SUCCESS;
    }
  }

  // Add new decode settings
  if ((Status != EFI_SUCCESS) && (Length != 0)) {
    Status = EFI_OUT_OF_RESOURCES;
    for (Index = 0; Index < sizeof (LpcGenDecRegs) / sizeof (UINT8); Index++) {
      LpcGenDesRegValue = PchLpcPciCfg32 (LpcGenDecRegs[Index]);
      if (LpcGenDesRegValue == 0) {
        DecodeBaseAddress = (Address & B_PCH_LPC_GEN1_DEC_IOBAR);
        DecodeRangeAddress = (((UINTN)(Length & B_PCH_LPC_GEN1_DEC_IOBAR))<<16);
        MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, LpcGenDecRegs[Index]) = (UINT32)(DecodeRangeAddress + DecodeBaseAddress + B_PCH_LPC_GEN1_DEC_EN);
        Status = EFI_SUCCESS;
        break;
      }
    }
  }

  return Status;
}
/**
 IO address decode

 @param[in]         Type                Decode type
 @param[in]         Address             Decode address
 @param[in]         Length              Decode length
                                
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER One of the parameters has an invalid value
                                          1. Type has invalid value
                                          2. Address big then 0xFFFF
                                          3. Length need to be 4-byte aligned
 @retval            EFI_OUT_OF_RESOURCES  There are not enough resources available to set IO decode
*/
EFI_STATUS
IoDecodeControl (
  IN IO_DECODE_TYPE     Type,
  IN IO_DECODE_ADDRESS  Address,
  IN UINT16             Length
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  UINT16                IoDecodeRangesReg,IoDecodeRangesEnable;

  Status = EFI_INVALID_PARAMETER;

  if (Type == IoDecodeGenericIo) {
    Status = AllocateGenericIo ((UINTN)Address, Length);
  } else {
    if ((Type < IoDecodeTypeMaximum) && (Address < IoDecodeAddressMaximum)) {
      IoDecodeRangesReg = PchLpcPciCfg16 (R_PCH_LPC_IO_DEC);
      IoDecodeRangesEnable = PchLpcPciCfg16 (R_PCH_LPC_ENABLES);
      for (Index = 0; Index < sizeof (mIoDecConTable) / sizeof (IO_DECODE_CONTROL_TABLE); Index++) {
        if ((Type == mIoDecConTable[Index].IoType) && (Address == mIoDecConTable[Index].IoAddress)) {
          IoDecodeRangesReg &= (~(mIoDecConTable[Index].IoDecMask));
          IoDecodeRangesReg |= (mIoDecConTable[Index].IoDecValue);
          if (Length == 0) {
            IoDecodeRangesEnable &= (~mIoDecConTable[Index].IoDecEn);
          } else {
            IoDecodeRangesEnable |= mIoDecConTable[Index].IoDecEn;
          }
          Status = EFI_SUCCESS;
          break;
        }
      }
      MmPci16 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_IO_DEC) = IoDecodeRangesReg;
      MmPci16 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_ENABLES) = IoDecodeRangesEnable;
    }
  }

  return Status;
}