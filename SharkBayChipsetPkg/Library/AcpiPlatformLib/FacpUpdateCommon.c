/** @file

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

#include <Library/AcpiPlatformCommon.h>
#include <SmiTable.h>

typedef enum {
 SMM_RESET_ADDR,
 X_PM1A_EVT_BLK,
 X_PM1B_EVT_BLK,
 X_PM1A_CNT_BLK,
 X_PM1B_CNT_BLK,
 X_PM2_CNT_BLK,
 X_PM_TMR_BLK,
 X_GPE0_BLK,
 X_GPE1_BLK,
 MAX_FACP_GAS_DATA_NUM
} FACP_GAS_DATA_NUM;

EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE  AddrOfAllGASData[MAX_FACP_GAS_DATA_NUM] = {0};

EFI_STATUS
Acpi30FacpUpdateForGASData (
  IN EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE   **AddrOfFadtPointer3
  )
{
  UINTN     Index;

  for (Index = 0; Index < MAX_FACP_GAS_DATA_NUM; Index++) {
    //
    // To update some items about GAS definition.
    //
    switch (Index) {
      //
      // To Update SpaceId, BitWidth and Address
      //
    case SMM_RESET_ADDR :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO;
      AddrOfAllGASData[Index].RegisterBitWidth  = 0x08;               // 8 Bits
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = SW_SMI_PORT;
      break;
      
    case X_PM1A_EVT_BLK :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO;
      AddrOfAllGASData[Index].RegisterBitWidth  = ((*AddrOfFadtPointer3)->Pm1EvtLen) * 8;  // Translate Byte to Bit
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = (*AddrOfFadtPointer3)->Pm1aEvtBlk;
      break;
      
    case X_PM1A_CNT_BLK :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO;
      AddrOfAllGASData[Index].RegisterBitWidth  = ((*AddrOfFadtPointer3)->Pm1CntLen) * 8;  // Translate Byte to Bit
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = (*AddrOfFadtPointer3)->Pm1aCntBlk;
      break;

    case X_PM2_CNT_BLK :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO;
      AddrOfAllGASData[Index].RegisterBitWidth  = ((*AddrOfFadtPointer3)->Pm2CntLen) * 8;  // Translate Byte to Bit
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = (*AddrOfFadtPointer3)->Pm2CntBlk;
      break;

    case X_PM_TMR_BLK :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO;
      AddrOfAllGASData[Index].RegisterBitWidth  = ((*AddrOfFadtPointer3)->PmTmrLen) * 8;   // Translate Byte to Bit
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = (*AddrOfFadtPointer3)->PmTmrBlk;
      break;

    case X_GPE0_BLK :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO;
      AddrOfAllGASData[Index].RegisterBitWidth  = ((*AddrOfFadtPointer3)->Gpe0BlkLen) * 8; // Translate Byte to Bit
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = (*AddrOfFadtPointer3)->Gpe0Blk;
      break;

    default :
      AddrOfAllGASData[Index].AddressSpaceId    = EFI_ACPI_3_0_UNDEFINED;
      AddrOfAllGASData[Index].RegisterBitWidth  = 0;
      AddrOfAllGASData[Index].RegisterBitOffset = 0;
      AddrOfAllGASData[Index].Address           = 0;
      break;
    }

    switch (AddrOfAllGASData[Index].RegisterBitWidth) {
      //
      // To Update Access Size Type.
      //
    case 8:
      AddrOfAllGASData[Index].AccessSize = EFI_ACPI_3_0_BYTE;
      break;
      
    case 16:
      AddrOfAllGASData[Index].AccessSize = EFI_ACPI_3_0_WORD;
      break;

    case 32:
      AddrOfAllGASData[Index].AccessSize = EFI_ACPI_3_0_DWORD;
      break;

    case 64:
      AddrOfAllGASData[Index].AccessSize = EFI_ACPI_3_0_QWORD;
      break;

    default :
      AddrOfAllGASData[Index].AccessSize = EFI_ACPI_3_0_UNDEFINED;
      break;
    }
  }

  return EFI_SUCCESS;
}

//[-start-111215-IB02430207-modify]//
EFI_STATUS
AcpiFacp30AboveCommonUpdate (
  IN EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE   **AddrOfFadtPointer3
  )
{
  Acpi30FacpUpdateForGASData (AddrOfFadtPointer3);
  
  (* AddrOfFadtPointer3)->Flags      |= EFI_ACPI_3_0_RESET_REG_SUP;
  (* AddrOfFadtPointer3)->ResetReg    = AddrOfAllGASData[SMM_RESET_ADDR];
  (* AddrOfFadtPointer3)->ResetValue  = SMM_OS_RESET_SMI_VALUE;
  (* AddrOfFadtPointer3)->XPm1aEvtBlk = AddrOfAllGASData[X_PM1A_EVT_BLK];
  (* AddrOfFadtPointer3)->XPm1bEvtBlk = AddrOfAllGASData[X_PM1B_EVT_BLK];
  (* AddrOfFadtPointer3)->XPm1aCntBlk = AddrOfAllGASData[X_PM1A_CNT_BLK];
  (* AddrOfFadtPointer3)->XPm1bCntBlk = AddrOfAllGASData[X_PM1B_CNT_BLK];
  (* AddrOfFadtPointer3)->XPm2CntBlk  = AddrOfAllGASData[X_PM2_CNT_BLK];
  (* AddrOfFadtPointer3)->XPmTmrBlk   = AddrOfAllGASData[X_PM_TMR_BLK];
  (* AddrOfFadtPointer3)->XGpe0Blk    = AddrOfAllGASData[X_GPE0_BLK];
  (* AddrOfFadtPointer3)->XGpe1Blk    = AddrOfAllGASData[X_GPE1_BLK];

  return EFI_SUCCESS; 
}  

EFI_STATUS
Acpi50FacpUpdateCommon (
  IN EFI_ACPI_5_0_FIXED_ACPI_DESCRIPTION_TABLE   **AddrOfFadtPointer5
  )
{
  return AcpiFacp30AboveCommonUpdate ((EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE **)(UINTN)AddrOfFadtPointer5);
}

EFI_STATUS
Acpi30FacpUpdateCommon (
  IN EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE   **AddrOfFadtPointer3
  )
{
  (*AddrOfFadtPointer3)->Header.Revision = EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION;
  (*AddrOfFadtPointer3)->Header.Length = sizeof(EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE);
  
  return AcpiFacp30AboveCommonUpdate (AddrOfFadtPointer3);
}
//[-end-111215-IB02430207-modify]//

EFI_STATUS
Acpi10FacpUpdateCommon (
  IN     EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE   **AddrOfFadtPointer1,
  IN OUT EFI_ACPI_COMMON_HEADER                      *Table,
  IN OUT EFI_ACPI_TABLE_VERSION                      *Version
  )
{
  *Version = EFI_ACPI_TABLE_VERSION_1_0B;
  (*AddrOfFadtPointer1)->IntModel = 0x01;     // INT_MODEL;
  (*AddrOfFadtPointer1)->Reserved1 = 0;       // RESERVED;
  (*AddrOfFadtPointer1)->Reserved4 = 0;       // RESERVED;
  (*AddrOfFadtPointer1)->Reserved5 = 0;       // RESERVED;
  (*AddrOfFadtPointer1)->Header.Revision = EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION;
  Table->Length = sizeof(EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE);
  
  return EFI_SUCCESS;
}