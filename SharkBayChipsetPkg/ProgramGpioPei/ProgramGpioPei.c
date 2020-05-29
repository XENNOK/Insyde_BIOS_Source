/** @file
  Init GPIO base and program GPIO.

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

//[-start-121101-IB05280007-modify]//
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <PchAccess.h>
#include <Library/IoLib.h>
//[-start-121214-IB10820195-remove]//
//#include <BaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <Library/PeiServicesLib.h>
#include <Library/PeiOemSvcChipsetLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
//[-start-121121-IB11410023-add]//
#include <Guid/PlatformInfoHob.h>
//[-start-130809-IB06720232-remove]//
//#include <Library/Eclib.h>
//[-end-130809-IB06720232-remove]//
//[-end-121121-IB11410023-add]//
//[-start-140604-IB06720256-add]//
#include <ChipsetGpio.h>
//[-end-140604-IB06720256-add]//

//[-start-121128-IB05280020-modify]//
VOID
ConfigureGpio(
  IN OUT  GPIO_SETTINGS_TABLE  *GpioDefinition
  );

VOID
ConfigureUltRvpGpio(
  IN OUT  GPIO_SETTINGS_TABLE  *GpioDefinition
  );
//[-end-121128-IB05280020-modify]//
//[-end-121101-IB05280007-modify]//

EFI_STATUS
ProgramGPIOEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
//[-start-121109-IB10820156-add]//
  EFI_STATUS    Status;
//[-end-121109-IB10820156-add]//
//[-start-121004-IB10370017-add]//
  PLATFORM_INFO_HOB                  *PlatformInfoHob;
//[-end-121004-IB10370017-add]//
//[-start-121121-IB11410023-add]//
  UINT8                               BoardId1;
  UINT8                               BoardId2;
//[-end-121121-IB11410023-add]//
//[-start-121128-IB05280020-add]//
  GPIO_SETTINGS_TABLE  *GpioTable;
//[-end-121128-IB05280020-add]//

  UINT32        GPIOBase;
//[-start-121214-IB11410031-add]//
  UINT16        Lpc_Enables;
//[-end-121214-IB11410031-add]//
  GPIOBase = 0;
//[-start-121217-IB11410032-add]//
  BoardId1 = 0x00;
  BoardId2 = 0x00;
//[-end-121217-IB11410032-add]//
  
//[-start-121214-IB11410031-add]//
  Lpc_Enables = PchLpcPciCfg16 (R_PCH_LPC_ENABLES);
  if ((Lpc_Enables & B_PCH_LPC_ENABLES_MC_EN) != B_PCH_LPC_ENABLES_MC_EN) {
    PchLpcPciCfg16Or (R_PCH_LPC_ENABLES, Lpc_Enables | B_PCH_LPC_ENABLES_MC_EN);
  }
//[-end-121214-IB11410031-add]//
  
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "\nProgram GPIO Entry\n") );
    
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "Check GPIO Base\n" ) );
  //
  // Check if GPIO Base is set
  //
  GPIOBase = PchLpcPciCfg32 ( R_PCH_LPC_GPIO_BASE ) & B_PCH_LPC_GPIO_BASE_BAR;
  if ( GPIOBase == 0 ) {
    DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "GPIO Base is not set\n" ) );

    DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "Set GPIO Base\n" ) );
    //
    // Set GPIO Base
    //
    PchLpcPciCfg32AndThenOr ( R_PCH_LPC_GPIO_BASE, ( UINT32 )( ~B_PCH_LPC_GPIO_BASE_BAR ), PcdGet16(PcdPchGpioBaseAddress));

    DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "Enable GPIO Base\n" ) );
    //
    // Enable GPIO Base
    //
    PchLpcPciCfg8Or ( R_PCH_LPC_GPIO_CNT, B_PCH_LPC_GPIO_CNT_GPIO_EN );
  }

  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "Program GPIO settings\n" ) );

  
//[-start-130420-IB05400398-add]//
  GpioTable = (GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable1);
  OemSvcGetGpioTable (&GpioTable);
//[-end-130420-IB05400398-add]//

//[-start-130420-IB05400398-remove]//
////[-start-130201-IB05280044-modify]//
//  if (FixedPcdGet8 (PcdDefaultBoardId) == 0xFF) {
//    BoardId1 = 0xFF;
//    BoardId2 = 0xFF;
//    GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable1);
//  } else {
//    if (!FeaturePcdGet (PcdUltFlag)) {
//      if (FeaturePcdGet(PcdDenlowServerSupported)) {
//        BoardId1 = V_CRB_BOARD_ID_AGATE_BEACH_CRB;
//      } else {
//        BoardId1 = V_CRB_BOARD_ID_FHC;
//      }
////      Status = GetBoardId (&BoardId1);
////      if (EFI_ERROR (Status)) {
////        if (FixedPcdGet8 (PcdDefaultBoardId) == 0x01) {
////          BoardId1 = V_EC_GET_CRB_BOARD_ID_BOARD_ID_GRAYS_REEF;
////        } else if (FixedPcdGet8 (PcdDefaultBoardId) == 0x03) {
////          BoardId1 = V_EC_GET_CRB_BOARD_ID_BOARD_ID_BASKING_RIDGE;
////        }
////      }
//    } else {
//      Status = GetUltBoardId (&BoardId1, &BoardId2);
//      if (!EFI_ERROR (Status)) {
//        switch (BoardId2) {
//        case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH:
//        case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH_SKU2:
//        case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH_SKU3:
//        case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH_SKU4:
//          BoardId2 = V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH;
//          break;
//        }
//      } else {
//        if (FixedPcdGet8 (PcdDefaultBoardId) == 0x24) {
//          BoardId2 = V_EC_GET_CRB_BOARD_ID_BOARD_ID_SAWTOOTH_PEAK;
//        } else if (FixedPcdGet8 (PcdDefaultBoardId) == 0x20) {
//          BoardId2 = V_EC_GET_CRB_BOARD_ID_BOARD_ID_WHITE_TIP_MOUNTAIN;
//        } else if (FixedPcdGet8 (PcdDefaultBoardId) == 0x30) {
//          BoardId2 = V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH;
//        }
//      }
//    }
//    //
//    // Program GPIO settings
//    //
//    if (!FeaturePcdGet (PcdUltFlag)) {
//      if ((BoardId1 & B_EC_GET_CRB_BOARD_ID_BOARD_ID) == V_CRB_BOARD_ID_AGATE_BEACH_CRB) {
//        GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable2);
//      } else{
//        GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable1);
//      }
//    } else {
//      switch (BoardId2) {
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_WHITE_TIP_MOUNTAIN:
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_WHITE_TIP_MOUNTAIN2:
//        GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable3);
//        break;
//
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_SAWTOOTH_PEAK:
//         GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable4);
//        break;
//
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH:
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH_SKU2:
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH_SKU3:
//      case V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH_SKU4:
//        GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable5);
//        break;
//      default:
//        GpioTable=(GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable3);
//        break;
//      }
//    }
//  }
////[-end-130201-IB05280044-modify]//
//[-end-130420-IB05400398-remove]//

  ASSERT (GpioTable != NULL);
//[-start-130524-IB05160451-modify]//
  Status = OemSvcModifyGpioSettingTable(GpioTable);

  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcModifyGpioSettingTable, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
  if ( Status == EFI_SUCCESS ) {
    return EFI_SUCCESS;
  }

  if (!FeaturePcdGet(PcdUltFlag)) {
    ConfigureGpio (GpioTable);
  } else {
    ConfigureUltRvpGpio (GpioTable);
  }
//[-end-121128-IB05280020-modify]//

//
// save the Board ID into Hob
//
  Status = PeiServicesCreateHob (EFI_HOB_TYPE_GUID_EXTENSION, 
                                  sizeof (PLATFORM_INFO_HOB), 
                                  (VOID **)&PlatformInfoHob);
  PlatformInfoHob->EfiHobGuidType.Name = gBoardIDGuid;

//[-start-130420-IB05400398-add]//
  PlatformInfoHob->BoardId = PcdGet8 (PcdCurrentBoardId);
  PlatformInfoHob->FabId = PcdGet8 (PcdCurrentFabId);
//[-end-130420-IB05400398-add]//
  if (!FeaturePcdGet (PcdUltFlag)) {
//[-start-121217-IB11410032-remove]//
//    GetBoardId (&BoardId1);
//[-end-121217-IB11410032-remove]//
    //
    // Grays Reef:         0x01(2 DIMM platform)(Default value)
    // Basking Ridge:      0x03(4 DIMM platform)
    //
    // If not sure refer to what CRB, BoardID should be set default value: Grays Reef
    //    Ex: PlatformInfoHob->BoardId = 0x01;
    //
//[-start-130420-IB05400398-remove]//
//    PlatformInfoHob->BoardId = BoardId1 & B_EC_GET_CRB_BOARD_ID_BOARD_ID;
//    PlatformInfoHob->FabId = (BoardId1 & B_EC_GET_CRB_BOARD_ID_FAB_ID) >> 5;
//[-end-130420-IB05400398-remove]//
    PlatformInfoHob->UltPlatform = FALSE;
  } else {
//[-start-121217-IB11410032-remove]//
//    GetUltBoardId (&BoardId1,&BoardId2);
//[-end-121217-IB11410032-remove]//
    //
    // WhiteTip Mountain:  0x20(2 DIMM DDR3L platform)(Default value)
    // SawTooth Peak:      0x24(on board LPDDR platform)
    //
    // If not sure refer to what CRB, BoardID should be set default value: WhiteTip Mountain
    //    Ex: PlatformInfoHob->BoardId = 0x20;
    //
//[-start-130420-IB05400398-remove]//
//    PlatformInfoHob->BoardId = BoardId2;
//    PlatformInfoHob->FabId = (BoardId1 & B_EC_GET_ULT_CRB_BOARD_ID_FAB_ID) >> 1;
//[-end-130420-IB05400398-remove]//
    PlatformInfoHob->UltPlatform = TRUE;
  }
  
  ASSERT_EFI_ERROR (Status);
//[-end-121004-IB10370017-add]//
  DEBUG ((EFI_D_ERROR | EFI_D_INFO,"Program GPIO Exit\n"));

  return EFI_SUCCESS;
}
//[-start-121128-IB05280020-modify]//
/**
  Configures GPIO device of normal gpio

  PIO_SETTINGS_TABLE  *GpioDefinition

  @retval None

**/

VOID
ConfigureGpio(
  IN OUT  GPIO_SETTINGS_TABLE  *GpioDefinition
  )
{
  UINTN      GpioTableCount; 
  UINT8      GpioPinNo;
  UINT8      GpioBankNo;
  UINT8      GpioBankPinNo;
  UINT32     RegUseSel;
  UINT32     RegIoSel;
  UINT32     RegLvl;
  UINT32     RegBlink;  
  UINT32     RegInv;
  UINT8      *UseSel;
  UINT8      *IoSel;
  UINT8      *Lvl;

//[-start-121211-IB05280023-modify]//
  GpioTableCount = LibPcdGetSize(PcdToken (PcdPeiGpioTable1))/sizeof(GPIO_SETTINGS_TABLE);
//[-end-121211-IB05280023-modify]//

  DEBUG ((EFI_D_INFO, "ConfigureGpio() Start\n"));


  UseSel = (UINT8 *)PcdGetPtr (PcdGpioUseSel);
  IoSel = (UINT8 *)PcdGetPtr (PcdGpioIoSel);
  Lvl = (UINT8 *)PcdGetPtr (PcdGpioLvl);

  GpioPinNo = 0;
  GpioBankPinNo = 0;
  GpioBankNo = 0;
  RegUseSel = 0;
  RegIoSel = 0;
  RegLvl = 0;
  RegBlink = 0;
  RegInv = 0;
  
  while (GpioPinNo < GpioTableCount) {
    RegUseSel |= (GpioDefinition[GpioPinNo].GpioUseSel << GpioBankPinNo);
    RegIoSel |= (GpioDefinition[GpioPinNo].GpioIoSel << GpioBankPinNo);
    RegLvl |= (GpioDefinition[GpioPinNo].GpoLvl << GpioBankPinNo);
    RegBlink |=(GpioDefinition[GpioPinNo].GpioBlink << GpioBankPinNo);
    RegInv |=(GpioDefinition[GpioPinNo].GpiInv << GpioBankPinNo);
      
    GpioPinNo++;
    GpioBankPinNo++;
    
    if ((GpioBankPinNo == 32) || (GpioPinNo == GpioTableCount)) {
      IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + UseSel[GpioBankNo], RegUseSel); 
      IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + IoSel[GpioBankNo], RegIoSel); 
      IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + Lvl[GpioBankNo], RegLvl); 
      if (GpioBankNo == 0) {
        IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + PcdGet8(PcdGpioBlink), RegBlink); 
        IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + PcdGet8(PcdGpioInv), RegInv); 
      }
      RegUseSel = 0;
      RegIoSel = 0;
      RegLvl = 0;
      
      GpioBankNo++;
      GpioBankPinNo = 0;
    }

  }
  DEBUG ((EFI_D_INFO, "ConfigureGpio() End\n"));

}

//[-start-120705-IB05330352-add]//
/**
 Configures GPIO device of ULT Lpt LP devices

 PIO_SETTINGS_TABLE  *GpioDefinition

 @retval None

**/
VOID
ConfigureUltRvpGpio(
  IN OUT  GPIO_SETTINGS_TABLE  *GpioDefinition
  ) 
{
//[-start-121031-IB05280007-modify]//
  UINTN      GpioTableCount; 
//[-end-121031-IB05280007-modify]//
  UINT8      GpioPinNo;
  UINT8      GpioBankNo;
  UINT8      GpioBankPinNo;
  UINT32     RegOwnership;
  UINT32     RegGpiRout;
  UINT32     RegGpiIe;
  UINT32     RegConfig0;
  UINT32     RegConfig4;

//[-start-121211-IB05280023-modify]//
  GpioTableCount = LibPcdGetSize(PcdToken (PcdPeiGpioTable1))/sizeof(GPIO_SETTINGS_TABLE);
//[-end-121211-IB05280023-modify]//

  DEBUG ((EFI_D_INFO, "ConfigureGpio() Start\n"));

  //
  // Initialize Variables
  //
  GpioPinNo = 0;
  GpioBankNo = 0;
  GpioBankPinNo = 0;
  RegOwnership = 0;
  RegGpiRout = 0;
  RegGpiIe = 0;

  while (GpioPinNo < GpioTableCount) {
    RegConfig0 = 0;
    RegConfig4 = 0;
    //
    // Configure GPIO line
    //
    RegConfig0 = (GpioDefinition[GpioPinNo].GpioUseSel << 0) |
                 (GpioDefinition[GpioPinNo].GpioIoSel << 2) |
                 (GpioDefinition[GpioPinNo].GpiInv << 3) |
                 (GpioDefinition[GpioPinNo].GpiLxEb << 4) |
                 (GpioDefinition[GpioPinNo].GpoLvl << 31);
    RegConfig4 = (GpioDefinition[GpioPinNo].GpinDis << 2) |
                 (GpioDefinition[GpioPinNo].GpiWp << 0);
    IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GP_N_CONFIG0 + GpioPinNo * 8 + 0x0, RegConfig0);
    IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GP_N_CONFIG0 + GpioPinNo * 8 + 0x4, RegConfig4);

    //
    // Setup ownership register
    //
    RegOwnership |= (GpioDefinition[GpioPinNo].GpioOwn << GpioBankPinNo);
    RegGpiRout |= (GpioDefinition[GpioPinNo].GpiRout << GpioBankPinNo);
    RegGpiIe |= (GpioDefinition[GpioPinNo].GpiIe << GpioBankPinNo);
    
    GpioPinNo++;
    GpioBankPinNo++;
    
    if ((GpioBankPinNo == 32) || (GpioPinNo == GpioTableCount)) {
      IoWrite32 ((UINT32)PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPIO_OWN0 + GpioBankNo * 4, RegOwnership);
      IoWrite32 ((UINT32)PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPIO_ROUT0 + GpioBankNo * 4, RegGpiRout);
      IoWrite32 ((UINT32)PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPI_IE0 + GpioBankNo * 4, RegGpiIe);
      RegOwnership = 0;
      RegGpiRout = 0;
      RegGpiIe = 0;
      GpioBankNo++;
      GpioBankPinNo = 0;
    }
  }

//[-start-121001-IB03780462-add]//
  //
  // Lynx Point spec v0.6.1 ch.22.16.5: The System BIOS must program GPIOBASE + 10h [15:0] = FFFFh to enable all
  // supported GPI pins to route to PIRQ[X:I] which then can generate IOxAPIC interrupts[39:24].
  // 
  IoWrite32 (PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GPIO_IOAPIC_SEL, V_PCH_GPIO_IOAPIC_SEL);
//[-end-121001-IB03780462-add]//
  
  DEBUG ((EFI_D_INFO, "ConfigureGpio() End\n"));
}
//[-end-120705-IB05330352-add]//
//[-end-121128-IB05280020-modify]//
