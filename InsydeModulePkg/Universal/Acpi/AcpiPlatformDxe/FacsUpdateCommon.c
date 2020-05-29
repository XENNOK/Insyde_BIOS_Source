/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/PciExpressLib.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/AcpiS3Save.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Pci22.h>
#include <KernelSetupConfig.h>

EFI_ACPI_S3_SAVE           mS3Save;

/**
  To get secure boot state hardware signature value.

  @return  UINT32    Hardware signature value for secure boot state.
**/
STATIC
UINT32
GetSecureBootHardwareSignatureValue (
  VOID
  )
{
  UINT8                                SecureBoot;
  UINTN                                BufferSize;
  EFI_STATUS                           Status;
  UINT8                                Buffer[21];
  UINT32                               Crc32;

  //
  // Using L"SecureBoot" and variable data to calculate CRC32 value fro secure boot state hardware
  // signature to identify the differences between other hardware signature settings.
  //
  BufferSize = sizeof (UINT8);
  Status = gRT->GetVariable (L"SecureBoot", &gEfiGlobalVariableGuid, NULL, &BufferSize, &SecureBoot);
  if (EFI_ERROR (Status)) {
    SecureBoot = 0xFF;
  }
  CopyMem (Buffer, L"SecureBoot", StrLen (L"SecureBoot") * sizeof (CHAR16));
  Buffer [StrLen (L"SecureBoot") * sizeof (CHAR16)] = SecureBoot;
  Crc32  = 0;
  Status = gBS->CalculateCrc32 (Buffer, StrLen (L"SecureBoot") * sizeof (CHAR16) + sizeof (UINT8), &Crc32);
  ASSERT_EFI_ERROR (Status);

  return Crc32;
}


/**
  To determine Hardware Signature by kernel default algorithm.

  @param[out]  HardwareSignature   Value of Hardware Signature.

  @retval                          None.
**/
VOID
DefaultUpdateAcpiFacsHardwareSignature (
  OUT  UINT32                  *HardwareSignature
)
{
  UINTN                                Bus;
  UINTN                                BusLimit;
  UINTN                                Device;
  UINTN                                DeviceLimit;
  UINTN                                Function;
  UINTN                                FunctionLimit;
  UINT16                               ClassCode;
  UINT8                                HeaderType;
  UINT8                                Value8;

  Bus           = 0;
  BusLimit      = 0;
  Device        = 0;
  DeviceLimit   = 0;
  Function      = 0;
  FunctionLimit = 0;
  ClassCode     = 0;
  HeaderType    = 0;
  Value8        = 0;

  //
  // Set Hardware Signature, the algorithm to calculate signature value depends on customization.
  //
  *HardwareSignature = 0;
  BusLimit = 1;
  for ( Bus = 0 ; Bus <= BusLimit ; Bus++ ) {
    for ( Device = 0 ; Device <= 0x1F ; Device++ ) {
      FunctionLimit = 7;
      for ( Function = 0 ; Function <= FunctionLimit ; Function++ ) {
        //
        // 1. Check header type & class code
        //
        HeaderType = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (
                                       Bus,
                                       Device,
                                       Function,
                                       PCI_HEADER_TYPE_OFFSET));
        ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (
                                       Bus,
                                       Device,
                                       Function,
                                       PCI_CLASSCODE_OFFSET + 1));
        if (Function == 0) {
          if (ClassCode == 0xFFFF) {
            break;
          }
          if (!(HeaderType & HEADER_TYPE_MULTI_FUNCTION)) {
            FunctionLimit = 0;
          }
        }
        if (ClassCode == 0xFFFF) {
          continue;
        }
        if (ClassCode == ((PCI_CLASS_BRIDGE << 8) | PCI_CLASS_BRIDGE_P2P)) {
          Value8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (
                                       Bus,
                                       Device,
                                       Function,
                                       PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
          if (Value8 > BusLimit) {
            BusLimit = Value8;
          }
        }

        *HardwareSignature += PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (
                                       Bus,
                                       Device,
                                       Function,
                                       PCI_VENDOR_ID_OFFSET));
        *HardwareSignature += PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (
                                       Bus,
                                       Device,
                                       Function,
                                       PCI_DEVICE_ID_OFFSET));
        *HardwareSignature += PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (
                                       Bus,
                                       Device,
                                       Function,
                                       PCI_REVISION_ID_OFFSET));
      }
    }
  }
  *HardwareSignature += GetSecureBootHardwareSignatureValue ();
}

/**
  Update FACS Hardware Signature, and call S3Save function.

  @param This                 A pointer to the EFI_ACPI_S3_SAVE_PROTOCOL instance.
  @param LegacyMemoryAddress  The base address of legacy memory.

  @return                     None.

**/
VOID
EFIAPI
UpdateFacsHardwareSignature (
  IN EFI_ACPI_S3_SAVE_PROTOCOL    *This,
  IN VOID                         *LegacyMemoryAddress
  )
{
  EFI_STATUS                                Status;
  EFI_ACPI_DESCRIPTION_HEADER               *Table;
  INTN                                      Index;
  UINTN                                     Handle;
  EFI_ACPI_TABLE_VERSION                    Version;
  EFI_ACPI_SUPPORT_PROTOCOL                 *AcpiSupport;

  This->S3Save = mS3Save;

  Index = 0;

  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&AcpiSupport);
  ASSERT_EFI_ERROR (Status);

  //
  // Search for FACS table
  //
  while (1) {
    Status = AcpiSupport->GetAcpiTable (AcpiSupport, Index, (VOID **)&Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    //
    // Updates once the table found
    //
    if (Table->Signature == EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
      //
      // Update Hardware Signature with kernel default algorithm.
      //
      DefaultUpdateAcpiFacsHardwareSignature (
        &(((EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE*)(UINTN)Table)->HardwareSignature)
      );

      //
      // OemServices for updating Hardware Signature.
      //
      Status = OemSvcUpdateAcpiFacsHardwareSignature (
          &(((EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE*)(UINTN)Table)->HardwareSignature)
      );
      if (!EFI_ERROR (Status)) {
        //
        // Oemservices has already updated hardware signature so free the acpi table.
        //
        gBS->FreePool ( Table );
        This->S3Save (This, LegacyMemoryAddress);
        return ;
      }
      //
      // If OemServices status is not EFI_SUCCESS (EFI_MEDIA_CHANGED or EFI_UNSUPPORTED),
      // kernel will update hardware signature.
      //
      AcpiSupport->SetAcpiTable (AcpiSupport, Table, TRUE, Version, &Handle);
      gBS->FreePool ( Table );
      This->S3Save (This, LegacyMemoryAddress);
      return ;
    }

    gBS->FreePool ( Table );
    Index++;
  }
  This->S3Save (This, LegacyMemoryAddress);
}

/**
  AcpiS3Save hook point, we have to promise update Facs table before save s3 information.
  So, hook AcpiS3Save->S3Save.

  @param                        None.

  @return EFI_SUCCESS           Hook AcpiS3Save->Save succeed.

**/
EFI_STATUS
AcpiS3SaveHook (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_ACPI_S3_SAVE_PROTOCOL  *AcpiS3Save;
  
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **)&AcpiS3Save);
  ASSERT_EFI_ERROR (Status);

  mS3Save = AcpiS3Save->S3Save;

  AcpiS3Save->S3Save = (EFI_ACPI_S3_SAVE)UpdateFacsHardwareSignature;

  return EFI_SUCCESS;
}

/**
  Update FACS table content
  Depends on ACPI Version option in Setup Variable for the 64 bits table support.

  @param  Table           The table to update
  @param  SetupVariable   SETUP Variable pointer

  @return EFI_SUCCESS     Update table success

**/
EFI_STATUS
FacsUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
)
{
  switch (SetupVariable->AcpiVer) {
  case 0x03:
  case 0x02:
    ((EFI_ACPI_4_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(Table))->Version = EFI_ACPI_4_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_VERSION; //0x02
    ((EFI_ACPI_4_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(Table))->OspmFlags = 0;
    (Table)->Length = sizeof(EFI_ACPI_4_0_FIRMWARE_ACPI_CONTROL_STRUCTURE);
    ((EFI_ACPI_4_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(Table))->Flags &= ~EFI_ACPI_4_0_64BIT_WAKE_SUPPORTED_F;
    break;
  case 0x01:
    (Table)->Length = sizeof(EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE);
    break;

  case 0:
  default:
    ((EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(Table))->Version = 0;
    (Table)->Length = sizeof(EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE);
    break;
  }
  AcpiS3SaveHook ();
  return EFI_SUCCESS;
}

