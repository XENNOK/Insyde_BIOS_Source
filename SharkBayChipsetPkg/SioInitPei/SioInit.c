/** @file
  Statements that include other files.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PostCode.h>
#include <ChipsetSetupConfig.h>
#include "SioRegs.h"
#include "Ppi/ReadOnlyVariable2.h"
//[-start-120510-IB10820050-modify]//
#include <Ppi/SioInitialized.h>
//[-end-120510-IB10820050-modify]//
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>

//[-start-121113-IB05280009-add]//
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-121113-IB05280009-add]//
#include <Library/SerialPortLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//

static EFI_PEI_PPI_DESCRIPTOR mSioInitializedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSioInitializedPpiGuid,
  NULL
};

EFI_SIO_TABLE mSioTableDenlow[] ={
  //
  // Init serial port 1
  //
  {
    PILOT3_LOGICAL_DEVICE,
    PILOT3_SIO_COM1
  },
  {
    PILOT3_BASE_ADDRESS_HIGH0,
    0x03
  },
  {
    PILOT3_BASE_ADDRESS_LOW0,
    0xf8
  },
  {
    PILOT3_PRIMARY_INTERRUPT_SELECT,
    0x04
  },
  {
    PILOT3_ACTIVATE,
    0x01
  },
  {
    0xff,
    0xff
  }
};

/**

   INIT the SIO. Ported this code and I don't undertand the comments either.

   @param   FfsHeader             FV this PEIM was loaded from.
   @param   PeiServices           General purpose services available to every PEIM.
   
   @retval  None
 
**/
EFI_STATUS
EFIAPI
SioInitPeim (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  UINT16                            ConfigPort;
  UINT16                            IndexPort;
  UINT16                            DataPort;
  UINT8                             Index;
  EFI_STATUS                        Status;
  UINTN                             VariableSize;
  VOID                              *SystemConfiguration;
  EFI_GUID                          SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  UINT8                             Data8;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices; 
//[-start-121113-IB05280009-add]//
  EFI_SIO_TABLE                     *mSioTable;
  UINTN                             Size;
  EFI_SIO_GPIO_TABLE                *mSioGpioTable;
//[-end-121113-IB05280009-add]//
  
  POST_CODE (PEI_SIO_INIT); //PostCode = 0x70, Super I/O initial

//[-start-121128-IB05280020-add]//
  mSioTable=(EFI_SIO_TABLE *)PcdGetPtr (PcdPeiSioTable);
  ASSERT (mSioTable != NULL);

  mSioGpioTable=(EFI_SIO_GPIO_TABLE *)PcdGetPtr (PcdPeiSioGpioTable1);
  ASSERT (mSioGpioTable != NULL);
//[-end-121128-IB05280020-add]//

//[-start-121113-IB05280009-add]//
//[-start-130524-IB05160451-modify]//
//[-start-121128-IB05280020-modify]//
  Status = OemSvcHookInitSio (mSioTable,mSioGpioTable);
//[-end-121128-IB05280020-modify]//
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcSioInitHook, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
  if ( Status == EFI_SUCCESS ) {
    return EFI_SUCCESS;
  }
//[-end-121113-IB05280009-add]//

  if (FeaturePcdGet (PcdDenlowServerSupported)) {
    //
    // hard code for sio init
    //
    ConfigPort = SIO_CONFIG_PORT2;
    IndexPort  = PILOT3_SIO_INDEX_PORT;
    DataPort   = PILOT3_SIO_DATA_PORT;

    //
    // Program and Enable SIO Base Addresses 2E and 4E
    //
    PchLpcPciCfg16Or (R_PCH_LPC_ENABLES, B_PCH_LPC_ENABLES_CNF1_EN | B_PCH_LPC_ENABLES_CNF2_EN);

    //
    // Enter Config Mode
    //
    IoWrite8 (ConfigPort, PILOT3_SIO_UNLOCK);
    IoWrite8 (ConfigPort, 0x00); // 0x00 be end

    //
    // Configure SIO
    //
    for (Index = 0; Index < sizeof(mSioTableDenlow)/sizeof(EFI_SIO_TABLE); Index++) {
      IoWrite8 (IndexPort, mSioTableDenlow[Index].Register);
      IoWrite8 (DataPort, mSioTableDenlow[Index].Value);
    }

    //
    // Exit Config Mode
    //
    IoWrite8 (ConfigPort, PILOT3_SIO_LOCK);
    IoWrite8 (ConfigPort, 0x00); // 0x00 be end


    //
    // Enable LPC COM port 0x3F8 resource decoded
    //
    PchLpcPciCfg16AndThenOr (R_PCH_LPC_IO_DEC, (UINT16)(~B_PCH_LPC_COMA_DEC), V_PCH_LPC_COMA_3F8);
    PchLpcPciCfg16Or (R_PCH_LPC_ENABLES, B_PCH_LPC_ENABLES_COMA_EN);

    //
    // Init Serial Port
    //
    SerialPortInitialize();
  } else {
    //
    // hard code for sio init
    //
//[-start-121207-IB06460475-modify]//
    ConfigPort = PcdGet16(PcdSioConfigPort);
    DataPort   = PcdGet16(PcdSioDataPort);
    IndexPort  = PcdGet16(PcdSioIndexPort);
//[-end-121207-IB06460475-modify]//

    //
    // Program and Enable SIO Base Addresses
    //
//[-start-130613-IB05160456-remove]//
//    IoWrite32 (0xcf8, 0x8000f888);
////[-start-121113-IB05280009-modify]//
//    IoWrite32 (0xcfc, 0x3 << 18 | (PcdGet16(PcdSioConfigPort) & (~0xF)) | 1);
////[-end-121113-IB05280009-modify]//
//    IoWrite32 (0xcf8, 0x8000f884);
////[-start-121113-IB05280009-modify]//
//    IoWrite32 (0xcfc, 0x3 << 18 | (PcdGet16(PcdSioBaseAddress) & (~0x7F)) | 1);
////[-end-121113-IB05280009-modify]//
//[-end-130613-IB05160456-remove]//
    IoWrite32 (0xcf8, 0x8000f880);
    IoWrite32 (0xcfc, 0x3c030010);

    //
    // Enter Config Mode
    //
    IoWrite8 (ConfigPort, 0x55);

//[-start-121113-IB05280009-add]//
//[-start-121128-IB05280020-remove]//
//  mSioTable=(EFI_SIO_TABLE *)PcdGetPtr (PcdPeiSioTable);
//  ASSERT (mSioTable != NULL);
//[-end-121128-IB05280020-remove]//
    Size = LibPcdGetSize(PcdToken (PcdPeiSioTable))/sizeof(EFI_SIO_TABLE);
//[-end-121113-IB05280009-add]//

    //
    // Configure SIO
    //
//[-start-121113-IB05280009-modify]//
    for (Index = 0; Index < Size; Index++) {
//[-end-121113-IB05280009-modify]//
      IoWrite8 (IndexPort, mSioTable[Index].Register);
      IoWrite8 (DataPort, mSioTable[Index].Value);
    }

    //
    // Exit Config Mode
    //
//[-start-121113-IB05280009-modify]//
    IoWrite8 (PcdGet16(PcdSioConfigPort), 0xAA);
//[-end-121113-IB05280009-modify]//
//[-start-121113-IB05280009-add]//
//[-start-121128-IB05280020-remove]//
////[-start-121122-IB05280018-modify]//
//  mSioGpioTable=(EFI_SIO_GPIO_TABLE *)PcdGetPtr (PcdPeiSioGpioTable1);
////[-end-121122-IB05280018-modify]//
//  ASSERT (mSioGpioTable != NULL);
//[-end-121128-IB05280020-remove]//
//[-start-121122-IB05280018-modify]//
    Size = LibPcdGetSize(PcdToken (PcdPeiSioGpioTable1))/sizeof(EFI_SIO_GPIO_TABLE);
//[-end-121122-IB05280018-modify]//
//[-end-121113-IB05280009-add]//

//[-start-121113-IB05280009-modify]//
    for (Index = 0; Index < Size; Index++) {
      IoWrite8 (PcdGet16(PcdSioBaseAddress) + mSioGpioTable[Index].Register, mSioGpioTable[Index].Value);
//[-end-121113-IB05280009-modify]//
    }
    //
    // GPIO 15-17:IN  10-11:IN 12:OUT 13-14:IN  Enable RS232  ref: Page40 of CRB_SCH
    //
//[-start-121113-IB05280009-modify]//
    IoWrite8 (PcdGet16(PcdSioBaseAddress) + 0x0c, 0x1f);
//[-end-121113-IB05280009-modify]//

    //
    // GPIO 30-37, GPIO37/GPIO31:80 OR GPIO37/GPIO31:02
    //

    // Get setup variable from NVRAM.

    Status = PeiServicesLocatePpi (
                              &gEfiPeiReadOnlyVariable2PpiGuid,
                              0,
                              NULL,
                              (VOID **) &VariableServices
                              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    
//[-start-130709-IB05160465-modify]//
    VariableSize = PcdGet32 (PcdSetupConfigSize);
    SystemConfiguration = AllocateZeroPool (VariableSize);
//[-start-140625-IB05080432-add]//
    if (SystemConfiguration == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
//[-end-140625-IB05080432-add]//
    
    Status = VariableServices->GetVariable(
                       VariableServices,
                       L"Setup",
                       &SystemConfigurationGuid,
                       NULL,
                       &VariableSize,
                       SystemConfiguration
                       );
//[-end-130709-IB05160465-modify]//

    if (!EFI_ERROR (Status)) {
      //
      // modify SIO GPIO_31 & GPIO_37 to select backlight control mode : PWM or GMBUS (Graphic SMBUS, ie. I2C)
      //
//[-start-121113-IB05280009-modify]//
      Data8 = IoRead8 (PcdGet16(PcdSioBaseAddress) + 0x0E) & (~0x82);  // clear GPIO_31(bit1), GPIO_37(bit7)
//[-end-121113-IB05280009-modify]//
//[-start-130710-IB05160465-modify]//
      switch (((CHIPSET_CONFIGURATION *)SystemConfiguration)->BacklightControl) {
//[-end-130710-IB05160465-modify]//

        case BKLT_SEL_PWM_INVERTED :
        case BKLT_SEL_PWM_NORMAL   :
//[-start-121113-IB05280009-modify]//
          IoWrite8 (PcdGet16(PcdSioBaseAddress) + 0x0E, (Data8 | 0x02));       // "PWM" is selected
//[-end-121113-IB05280009-modify]//
          break;
        case BKLT_SEL_GMBUS_INVERTED  :
        case BKLT_SEL_GMBUS_NORMAL    :
//[-start-121113-IB05280009-modify]//
          IoWrite8 (PcdGet16(PcdSioBaseAddress) + 0x0E, (Data8 | 0x80));       // "GMBus" is selected
//[-end-121113-IB05280009-modify]//
          break;
      }
    }

//[-start-130325-IB10930027-add]//
    //
    // Enable LPC COM port 0x3F8 resource decoded
    //
    if (FeaturePcdGet(PcdH2ODdtSupported)) {
      PchLpcPciCfg16AndThenOr (R_PCH_LPC_IO_DEC, (UINT16)(~B_PCH_LPC_COMA_DEC), V_PCH_LPC_COMA_3F8);
      PchLpcPciCfg16Or (R_PCH_LPC_ENABLES, B_PCH_LPC_ENABLES_COMA_EN);
    }
//[-end-130325-IB10930027-add]//
  }


  Status = (*PeiServices)->InstallPpi (PeiServices, &mSioInitializedPpi);

  
  return Status;
}

