/** @file
  Usb legacy keyboard/mouse control driver

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

#include <UsbLegacyControl.h>

UINT32                          mHidOutputData = 0;
UINT32                          mHidStatusData = 0;
UINT16                          mSoftIrqGeneratePort;
EFI_GUID                        mUsbLegacyControlProtocolGuid = USB_LEGACY_CONTROL_PROTOCOL_GUID;

//
// The device path used for SMI registration. Use dev:0x1f fun:0x00 to it
//
struct {
  ACPI_HID_DEVICE_PATH          Acpi;
  PCI_DEVICE_PATH               Pci;
  EFI_DEVICE_PATH_PROTOCOL      End;
} mDevicePath = {
  { ACPI_DEVICE_PATH, ACPI_DP, (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)), (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID(0x0A03), 0 },
  { HARDWARE_DEVICE_PATH, HW_PCI_DP, (UINT8)(sizeof(PCI_DEVICE_PATH)), (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8), 0x00, 0x1f },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 }
};
//
// The body of UsbLegacyControlProtocol used for protocol installation
//
typedef struct {
  USB_LEGACY_CONTROL_PROTOCOL   UsbLegacyControlProtocol;
  BOOLEAN                       SmiHandlerInstalled;
} USB_LEGACY_CONTROL_PRIVATE;

USB_LEGACY_CONTROL_PRIVATE      mPrivate = {
  { UsbLegacyControl, NULL, NULL, FALSE },
  FALSE
};

EFI_SMM_SYSTEM_TABLE2                   *mSmst;

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Success
  @retval Others                Fail

**/
EFI_STATUS
EFIAPI
UsbLegacyControlDriverEntryPoint (
  IN  EFI_HANDLE                                ImageHandle,
  IN  EFI_SYSTEM_TABLE                          *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  UINT32                                PmBase;
  EFI_SMM_USB_DISPATCH2_PROTOCOL        *UsbDispatch;
  EFI_SMM_USB_REGISTER_CONTEXT          UsbContext;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  BOOLEAN                               InSmm;
  
  //
  // if port 64 is "0xff", it's legacyFree
  //

  if ( IoRead8 (0x64) != 0xFF) { 
    return EFI_UNSUPPORTED ;
  }  

  SmmBase = NULL;
  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase);
  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }
  
  //
  // Initialize the EFI Runtime Library
  //
  if (InSmm){
    Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    
    //
    // Register USB legacy SMI handler
    //
    Status = mSmst->SmmLocateProtocol (
					 &gEfiSmmUsbDispatch2ProtocolGuid,
					 NULL,
					 (VOID **)&UsbDispatch
					 );
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }
    UsbContext.Type = UsbLegacy;
    UsbContext.Device = (EFI_DEVICE_PATH_PROTOCOL*)&mDevicePath;
    Status = UsbDispatch->Register(
                            UsbDispatch,
                            (EFI_SMM_HANDLER_ENTRY_POINT2)UsbLegacyControlSmiHandler,
                            &UsbContext,
                            &Handle
                            );
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }
    //
    // This is SMM instance
    //
    mPrivate.UsbLegacyControlProtocol.InSmm = TRUE;
    }
  //
  // Install USB_LEGACY_CONTROL_PROTOCOL
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &mUsbLegacyControlProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivate.UsbLegacyControlProtocol
                  );
  //
  // Setup the Soft IRQ generate port
  //
  PmBase = PciRead32 (PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_ACPI_PM_BASE)) & ACPI_PM_BASE_MASK;
  mSoftIrqGeneratePort = (UINT16)(PmBase + 0x70);
  return Status;
}

/**

  Main routine for the USB legacy control

  @param  Command               USB_LEGACY_CONTROL_SETUP_EMULATION
                                USB_LEGACY_CONTROL_GET_KBC_DATA_POINTER
                                USB_LEGACY_CONTROL_GET_KBC_STATUS_POINTER
                                USB_LEGACY_CONTROL_GENERATE_IRQ  
  @param  Param                 The parameter for the command

  @retval EFI_SUCCESS           Success
  @retval Others                Fail

**/
EFI_STATUS
EFIAPI
UsbLegacyControl (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  )
{
  UINT8         Data8;
  UINT32        Data32;
  
  switch (Command) {
    case USB_LEGACY_CONTROL_SETUP_EMULATION:
      //
      // Used for emulation on/off
      //
      Data32 = PciRead32 (PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_LUKMC));
      //
      // Always turn off USBSMIEN && USBPIRQEN
      //
      Data32 &= ~(B_LUKMC_USBSMIEN | B_LUKMC_USBPIRQEN);
      if (!Param) {
        //
        // Turn off the trap SMI
        //
        Data32 &= ~(B_LUKMC_TRAP_60R | B_LUKMC_TRAP_60W | B_LUKMC_TRAP_64R | B_LUKMC_TRAP_64W);
      } else {
        //
        // Turn on the trap SMI
        //
        Data32 |= (B_LUKMC_TRAP_60R | B_LUKMC_TRAP_60W | B_LUKMC_TRAP_64R | B_LUKMC_TRAP_64W);
      }
      PciWrite32 (PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_LUKMC), Data32);
      //
      // Inspect the SMI handler data to see is it need to update
      //
      if (Param && !mPrivate.SmiHandlerInstalled && mPrivate.UsbLegacyControlProtocol.SmiHandler) {
        mPrivate.SmiHandlerInstalled = TRUE;
        //
        // Trap SMI if now in BS to pass the SMI handler related data to SMM instance
        //
        if (mPrivate.UsbLegacyControlProtocol.InSmm == FALSE) {
          //
          // Trigger SMI through 64 port read
          //
          IoRead8 (0x64);
        }
      }
      break;
    case USB_LEGACY_CONTROL_GET_KBC_DATA_POINTER:
      //
      // Get the "pointer" of KBC data (port 0x60)
      //
      *(UINT32**)Param = &mHidOutputData;
      break;
    case USB_LEGACY_CONTROL_GET_KBC_STATUS_POINTER:
      //
      // Get the "pointer" of KBC status (port 0x64)
      //
      *(UINT32**)Param = &mHidStatusData;
      break;
    case USB_LEGACY_CONTROL_GENERATE_IRQ:
      //
      // IRQ 1/12 generating
      //
      Data8 = IoRead8 (mSoftIrqGeneratePort);
      //
      // Make signal as low
      //
      Data8 &= ~(0x01 << ((mHidStatusData & 0x20) ? IRQ12 : IRQ1));
      IoWrite8 (mSoftIrqGeneratePort, Data8);
      //
      // Stall a while
      //
      Data8 = IoRead8 (0x61) & 0x10;
      while (Data8 == (IoRead8 (0x61) & 0x10));
      while (Data8 != (IoRead8 (0x61) & 0x10));
      //
      // make signal as high
      //
      IoWrite8 (mSoftIrqGeneratePort, 0x03);
      //
      // We don't need the output data right now
      //
      *(BOOLEAN*)Param = FALSE;
      break;
  }
  return EFI_SUCCESS;
}

/**

  The UsbLegacyControl SMI main handler

  @param  Handle
  @param  Context

  @retval VOID
**/
VOID
UsbLegacyControlSmiHandler (
	IN		  EFI_HANDLE								  Handle,
	IN	CONST EFI_SMM_USB_REGISTER_CONTEXT				  *Context
  )
{
  EFI_STATUS                    Status;
  UINT32                        Data32;
  UINT32                        TrapEvent;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  USB_LEGACY_CONTROL_PRIVATE    *Private;
  EFI_TPL                       OldTpl;
  
  TrapEvent = 0;
  Data32    = PciRead32 (PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_LUKMC));
  if (Data32 & B_LUKMC_TRAP_STATUS) {
    //
    // Clear the status
    //
    PciWrite32 (PCI_LIB_ADDRESS (LPC_BUS, LPC_DEV, LPC_FUN, R_LUKMC), Data32);
    TrapEvent = (Data32 & B_LUKMC_TRAP_STATUS) >> N_LUKMC_TRAP_STATUS;
  }
  //
  // Call sub handler
  //
  if (mPrivate.UsbLegacyControlProtocol.SmiHandler) {
    mPrivate.UsbLegacyControlProtocol.SmiHandler (TrapEvent, mPrivate.UsbLegacyControlProtocol.SmiContext);
  }
  //
  // Check is the sub handler "SmiHandler" been installed. Must launched in POST
  //
  if (!mPrivate.SmiHandlerInstalled) {
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
    gBS->RestoreTPL (OldTpl);
    if (OldTpl == TPL_HIGH_LEVEL) {
      gBS->RestoreTPL (TPL_CALLBACK);
    }
    mPrivate.SmiHandlerInstalled = TRUE;
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &mUsbLegacyControlProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < HandleCount; Index++) {
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &mUsbLegacyControlProtocolGuid,
                        (VOID **)&Private
                        );
        if (!EFI_ERROR (Status) && Private->UsbLegacyControlProtocol.InSmm == FALSE) {
          //
          // Get the SMI handler related data from BS driver
          //
          mPrivate.UsbLegacyControlProtocol.SmiHandler = Private->UsbLegacyControlProtocol.SmiHandler;
          mPrivate.UsbLegacyControlProtocol.SmiContext = Private->UsbLegacyControlProtocol.SmiContext;
          break;
        }
      }
    }
    gBS->FreePool (HandleBuffer);
    if (OldTpl == TPL_HIGH_LEVEL) {
      gBS->RaiseTPL (TPL_HIGH_LEVEL);
    }
  }
}
