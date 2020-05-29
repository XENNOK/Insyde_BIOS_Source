/** @file

   Whea driver functions.

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

#include <Library/WheaLib.h>

#include <IndustryStandard/Pci.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>

//
// Defination in PchRegs.h
//
#define DEFAULT_PCI_BUS_NUMBER_PCH                0
#define PCI_FUNCTION_NUMBER_PCH_LPC               0

//
// Defination in PchRegsLpc.h
//
#define PCI_DEVICE_NUMBER_PCH_LPC                 31
#define R_PCH_LPC_ACPI_BASE                       0x40
#define R_PCH_LPC_GPIO_BASE                       0x48
#define R_PCH_ACPI_GPE0a_STS                      0x20
#define R_PCH_GPIO_GPI_INV                        0x2C
#define R_PCH_LPC_GPI_ROUT                        0xB8

#define PCH_TCO_BASE                              0x60
#define R_PCH_TCO1_CNT                            0x08
#define R_PCH_TCO1_STS                            0x04
#define R_PCH_ACPI_GPE0a_EN                       0x28
#define R_PCH_RTC_INDEX                           0x70
#define R_PCH_NMI_SC                              0x61
#define R_PCH_ALT_GP_SMI_EN                       0x38

//
// HW Error notification method used for FW first model
//
#define     EFI_GAS_DEFINITION              {0, 64, 0, 0, 0x0000}
#define     EFI_SCI_ERROR_DEFINATION        {ERR_NOTIFY_SCI, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0, 0, 0, 0}
#define     EFI_NMI_ERROR_DEFINATION        {ERR_NOTIFY_NMI, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0, 0, 0, 0}

//
// Generic error HW error sources - Tip: list all FW First error sources
// Note: Used 1KB Error Status block for each error source
//
GENERIC_HW_ERROR_SOURCE         MemoryCeSrc = \
  {0xffff,  4096, EFI_GAS_DEFINITION, EFI_SCI_ERROR_DEFINATION, 4096};
GENERIC_HW_ERROR_SOURCE         MemoryUeSrc = \
  {0xffff,  4096, EFI_GAS_DEFINITION, EFI_NMI_ERROR_DEFINATION, 4096};

SYSTEM_GENERIC_ERROR_SOURCE     SysGenErrSources[] = {
  {GENERIC_ERROR_CORRECTED, 0,  0,  &MemoryCeSrc},
  {GENERIC_ERROR_FATAL,     0,  0,  &MemoryUeSrc},
};
UINTN NumSysGenericErrorSources = (sizeof(SysGenErrSources)/sizeof(SYSTEM_GENERIC_ERROR_SOURCE));

//
// Native error sources - Tip: Errors that can be handled by OS directly
// Note: Used 1KB Error Status block for each error source
//
SYSTEM_NATIVE_ERROR_SOURCE   SysNativeErrSources[] = {0};
UINTN                        NumNativeErrorSources = 0;

/**
  Upadte the Whea Acpi Table Information.

  @param [in, out] TableHeader    A pointer of the WHEA ACPI table address

  @retval None

**/
VOID
InsydeWheaUpdateAcpiTableIds(
  IN OUT EFI_ACPI_DESCRIPTION_HEADER     *TableHeader
  )
{
  //
  // Update the OEM and Creator information for every WHEA table.
  //
  *(UINT32 *) (TableHeader->OemId)      = 'I' + ('N' << 8) + ('S' << 16) + ('Y' << 24);
  *(UINT16 *) (TableHeader->OemId + 4)  = 'D' + ('E' << 8);
  
  TableHeader->OemRevision     = EFI_ACPI_OEM_REVISION;
  TableHeader->OemTableId      = EFI_ACPI_OEM_TABLE_ID;
  TableHeader->CreatorId       = EFI_ACPI_CREATOR_ID;
  TableHeader->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  
}

/**
  Get the Memory Device Fru Information.

  @param [in] Smst                A pointer of Smst.

  @retval EFI_SUCCESS             Trigger SCI Success.

**/
EFI_STATUS
InsydeWheaTriggerSci (
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
  )
{
  EFI_STATUS                        Status;
  UINT16                            PmBase;
  UINT32                            TempData;
  UINT32                            GpioBaseAddr;
  UINT16                            GpioBase;

  //
  // Get the PM Base Address
  //
  PmBase = InsydeWheaGetPmBaseAddr ();

  //
  //  Get GPIO base address from ICH LPC 0x48h
  //
  GpioBaseAddr = PciRead32 (PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   R_PCH_LPC_GPIO_BASE
                   ));

  GpioBase = (UINT16)(GpioBaseAddr & 0xFF80);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: GpioBase : 0x%x\n", GpioBase));

  //
  // Trigger SCI by invert the GPIO[0] singal 
  //
  Status = Smst->SmmIo.Io.Read (
                            &Smst->SmmIo, 
                            SMM_IO_UINT32,
                            GpioBase + R_PCH_GPIO_GPI_INV,
                            1,
                            &TempData
                            );
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: TempData : 0x%08x\n", TempData));
  TempData ^= WHEA_GPIO_SCI;
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: TempData ^= WHEA_GPIO_SCI : 0x%08x\n", TempData));

  Status = Smst->SmmIo.Io.Write (
                            &Smst->SmmIo,
                            SMM_IO_UINT32,
                            GpioBase + R_PCH_GPIO_GPI_INV,
                            1,
                            &TempData
                            );
  //
  // Check GPE Status register to verify sci be generated.
  //
  while (TRUE) {
    Status = Smst->SmmIo.Io.Read (
                              &Smst->SmmIo, 
                              SMM_IO_UINT32,
                              PmBase + R_PCH_ACPI_GPE0a_STS,
                              1,
                              &TempData
                              );
    DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: TempData : 0x%08x\n", TempData));

    if (TempData & WHEA_GPIO_GPE) {
      //
      // Restore the GPIO Inv register
      //
      Status = Smst->SmmIo.Io.Read (
                                &Smst->SmmIo, 
                                SMM_IO_UINT32,
                                GpioBase + R_PCH_GPIO_GPI_INV,
                                1,
                                &TempData
                                );
      
      TempData ^= WHEA_GPIO_SCI;

      Status = Smst->SmmIo.Io.Write (
                                &Smst->SmmIo,
                                SMM_IO_UINT32,
                                GpioBase + R_PCH_GPIO_GPI_INV,
                                1,
                                &TempData
                                );

      break;
    }
  }
  return EFI_SUCCESS;
}

/**
  Set the specific IO port to trigger NMI.

  @param [in] SmmBase             A pointer of Smm Base protocol interface.
  @param [in] Smst                A pointer of Smst.

  @retval EFI_SUCCESS             Trigger NMI Success.

**/
EFI_STATUS
InsydeWheaTriggerNmi (
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
  )
{ 
  UINT16                            PmBase = 0;
  UINT16                            Temp = 0;
  UINT16                            NMI_EN_REG = 0;
  UINT16                            Temp2 = 0;
  
  //
  // Get the PM Base Address
  //
  PmBase = InsydeWheaGetPmBaseAddr ();

  //
  //  Read the NMI2SMI_EN bit, and set the NMI2SMI_EN bit to 0
  //
  InsydeWheaReadIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_CNT, &Temp, Smst);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));
  Temp = IoRead16 ((PmBase + PCH_TCO_BASE + R_PCH_TCO1_CNT));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));

  Temp = Temp & (~BIT9);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp & (~BIT9) : 0x%08x\n", Temp));

  InsydeWheaWriteIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_CNT, Temp, Smst);

  //
  // Store the NMI EN Reg and Enable NMI_EN bit
  //
  InsydeWheaReadIo16 (R_PCH_RTC_INDEX + 4, &NMI_EN_REG, Smst);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: NMI_EN_REG : 0x%08x\n", NMI_EN_REG));
  NMI_EN_REG = IoRead16 ((R_PCH_RTC_INDEX + 4));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: NMI_EN_REG : 0x%08x\n", NMI_EN_REG));

  Temp2 = (NMI_EN_REG & 0x7F);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: NMI_EN_REG & (~BIT7) : 0x%08x\n", Temp2));

  InsydeWheaWriteIo16 (R_PCH_RTC_INDEX, Temp2, Smst);

  //
  // Set NMI_NOW = 1 to clear this bit.
  //
  InsydeWheaReadIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_CNT, &Temp, Smst);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));
  Temp = IoRead16 ((PmBase + PCH_TCO_BASE + R_PCH_TCO1_CNT));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));

  Temp |= BIT8;
  
  InsydeWheaWriteIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_CNT, Temp, Smst);

  //
  // Clear the DMISERR_STS in TCOBASE + 04h[12], 
  //
  InsydeWheaReadIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_STS, &Temp, Smst);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));
  Temp = IoRead16 ((PmBase + PCH_TCO_BASE + R_PCH_TCO1_STS));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));

  Temp |= BIT12;

  InsydeWheaWriteIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_STS, Temp, Smst);

  //
  // Check the NMI2SMI_STS bit if set
  //
  InsydeWheaReadIo16 (PmBase + PCH_TCO_BASE + R_PCH_TCO1_STS, &Temp, Smst);
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));
  Temp = IoRead16 ((PmBase + PCH_TCO_BASE + R_PCH_TCO1_STS));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp : 0x%08x\n", Temp));

  if (Temp & BIT0) {
    //
    // 7.Check port 0x61
    //
    InsydeWheaReadIo16 (R_PCH_NMI_SC, &Temp2, Smst);
    DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp2 : 0x%08x\n", Temp2));
    Temp2 = IoRead16 ((R_PCH_NMI_SC));
    DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: Temp2 : 0x%08x\n", Temp2));

    if (Temp2 & BIT7) {
      //
      // Set PCI_SERR_EN bit
      //
      Temp2 |= BIT2;
      Temp2 &= 0x0F;
      InsydeWheaWriteIo16 (R_PCH_NMI_SC, Temp2, Smst);
      //
      // Clear PCI_SERR_EN bit
      //
      Temp2 &= 0x0B;

      InsydeWheaWriteIo16 (R_PCH_NMI_SC, Temp2, Smst);
    }
  }
  //
  // Restore NMI_EN 
  //
  NMI_EN_REG = (NMI_EN_REG & 0x7F);
  InsydeWheaWriteIo16 (R_PCH_RTC_INDEX, NMI_EN_REG, Smst);

  return EFI_SUCCESS;
}

/**
  Initialize and configure GPIO[0] parameter for preparing to trigger SCI.

  @param [in] EnableWhea          Is WHEA enable.
  @param [in] SmmBase             A pointer of Smm Base protocol interface.
  @param [in] Smst                A pointer of Smst.

  @retval EFI_SUCCESS             Initial the GPIO Success.

**/
EFI_STATUS
InsydeWheaProgramSignalsHook (
  IN BOOLEAN                           EnableWhea,
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
  )
{
  EFI_STATUS                        Status;
  UINT16                            Temp;
  UINT32                            Temp2;
  UINT16                            PmBase;
  UINT32                            GpioBaseAddr;
  UINT16                            GpioBase;

  Status = EFI_UNSUPPORTED;
  
  if (EnableWhea) {
    //
    // Get the PM Base Address
    //
    PmBase = InsydeWheaGetPmBaseAddr ();

    //
    //  Get GPIO base address from ICH LPC 0x48h
    //
    GpioBaseAddr = PciRead32 (PCI_LIB_ADDRESS (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC,
                     R_PCH_LPC_GPIO_BASE
                     ));

    GpioBase = (UINT16)(GpioBaseAddr & 0xFF80);

    //
    // Clear ALT_GPI_SMI_EN for GPIO[0] let it doesn't cause a SMI
    //
    Status = Smst->SmmIo.Io.Read (
                              &Smst->SmmIo, 
                              SMM_IO_UINT16,
                              PmBase + R_PCH_ALT_GP_SMI_EN,
                              1,
                              &Temp
                              );
    Temp &= ~WHEA_GPIO_SCI;
    Status = Smst->SmmIo.Io.Write (
                              &Smst->SmmIo,
                              SMM_IO_UINT16,
                              PmBase + R_PCH_ALT_GP_SMI_EN,
                              1,
                              &Temp
                              );

    //
    //  Set GPE Enable Reg for GPI0 to generate SCI when asserted
    //
    Status = Smst->SmmIo.Io.Read (
                              &Smst->SmmIo, 
                              SMM_IO_UINT32,
                              PmBase + R_PCH_ACPI_GPE0a_EN,
                              1,
                              &Temp2
                              );
    Temp2 |= WHEA_GPIO_GPE;
    Status = Smst->SmmIo.Io.Write (
                              &Smst->SmmIo,
                              SMM_IO_UINT32,
                              PmBase + R_PCH_ACPI_GPE0a_EN,
                              1,
                              &Temp2
                              );

    //
    // Check GPE Status register to verify GPIO pin polarity
    //
    if (TRUE) {
      Status = Smst->SmmIo.Io.Read (
                                &Smst->SmmIo, 
                                SMM_IO_UINT32,
                                PmBase + R_PCH_ACPI_GPE0a_STS,
                                1,
                                &Temp2
                                );

      if (Temp2 & WHEA_GPIO_GPE) {
        //
        // Restore the GPIO Inv register
        //
        Status = Smst->SmmIo.Io.Read (
                                  &Smst->SmmIo, 
                                  SMM_IO_UINT32,
                                  GpioBase + R_PCH_GPIO_GPI_INV,
                                  1,
                                  &Temp2
                                  );
        Temp2 ^= WHEA_GPIO_SCI;
        Status = Smst->SmmIo.Io.Write (
                                  &Smst->SmmIo,
                                  SMM_IO_UINT32,
                                  GpioBase + R_PCH_GPIO_GPI_INV,
                                  1,
                                  &Temp2
                                  );

      }
    }
    //
    // Route the ICH GPI to generate SCI
    //
    Temp2 = PciRead32 (PCI_LIB_ADDRESS (
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              R_PCH_LPC_GPI_ROUT
              ));

    Temp2 = (Temp2 & (~WHEA_GPIO_ROUTE0))| WHEA_GPIO_ROUTE1;

    PciWrite32 (PCI_LIB_ADDRESS (
      DEFAULT_PCI_BUS_NUMBER_PCH,
      PCI_DEVICE_NUMBER_PCH_LPC,
      PCI_FUNCTION_NUMBER_PCH_LPC,
      R_PCH_LPC_GPI_ROUT
      ), Temp2);

    //
    // Clear GPE0_STS in case SCI is asserted
    //
    Status = Smst->SmmIo.Io.Read (
                              &Smst->SmmIo, 
                              SMM_IO_UINT32,
                              PmBase + R_PCH_ACPI_GPE0a_STS,
                              1,
                              &Temp2
                              );
    Temp2 |= WHEA_GPIO_GPE;
    Status = Smst->SmmIo.Io.Write (
                              &Smst->SmmIo,
                              SMM_IO_UINT32,
                              PmBase + R_PCH_ACPI_GPE0a_STS,
                              1,
                              &Temp2
                              );

    Status = EFI_SUCCESS;
  }
  
  return Status;
}

/**
  Get PM Base Address from ICH LPC 0x40h.

  @param None

  @retval Return the Power management base address.

**/
UINT16
InsydeWheaGetPmBaseAddr(
  VOID
  )
{

  UINT32                            PmBaseAddr;
  UINT16                            PmBase;

  //
  //  Get PM base address from ICH LPC 0x40h
  //
  PmBaseAddr = PciRead32 (PCI_LIB_ADDRESS (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_LPC,
                 PCI_FUNCTION_NUMBER_PCH_LPC,
                 R_PCH_LPC_ACPI_BASE
                 ));
  PmBase = (UINT16)(PmBaseAddr & 0xFF80);
  
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: PmBase : 0x%08x\n", PmBase));

  return PmBase;
  
}

/**
  Write IO port.

  @param [in] mIoPort             IO port.
  @param [in] mValue              Value to write.
  @param [in] Smst                A pointer of Smst.

  @retval None

**/
VOID
InsydeWheaWriteIo16 (
  IN UINT16                            mIoPort,
  IN UINT16                            mValue,
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
  )
{
  EFI_STATUS            Status;

  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mIoPort : 0x%08x\n", mIoPort));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mValue : 0x%08x\n", mValue));
  Status = Smst->SmmIo.Io.Write (
                            &Smst->SmmIo,
                            SMM_IO_UINT16,
                            mIoPort,
                            1,
                            &mValue
                            );
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mIoPort : 0x%08x\n", mIoPort));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mValue : 0x%08x\n", mValue));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: %a() : %r\n", __FUNCTION__, Status));

  return;
}

/**
  Read IO port.

  @param [in] mIoPort             IO port.
  @param [in] mValue              Value to write.
  @param [in] Smst                A pointer of Smst.

  @retval None

**/
VOID
InsydeWheaReadIo16 (
  IN  UINT16                           mIoPort,
  OUT UINT16                           *mValue,
  IN  EFI_SMM_SYSTEM_TABLE2            *Smst
  )
{
  EFI_STATUS            Status;

  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mIoPort : 0x%08x\n", mIoPort));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mValue : 0x%08x\n", *mValue));
  Status = Smst->SmmIo.Io.Read (
                            &Smst->SmmIo,
                            SMM_IO_UINT16,
                            mIoPort,
                            1,
                            mValue
                            );
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mIoPort : 0x%08x\n", mIoPort));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: mValue : 0x%08x\n", *mValue));
  DEBUG ((DEBUG_INFO, "[APEI_LIB] INFO: %a() : %r\n", __FUNCTION__, Status));

  return;
}

