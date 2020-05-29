/** @file

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

#include "XhciSmiDispatcher.h"
//
// The device path used for SMI registration.
// This example sets the XHCI device plugged behind bridge BRIDGE_DEV_FOR_XHCI/BRIDGE_FUN_FOR_XHCI
// Please based on platform setting to change the device path for XHCI device
//
struct {
  ACPI_HID_DEVICE_PATH          Acpi;
  PCI_DEVICE_PATH               RootBridge;
  PCI_DEVICE_PATH               Pci;
  EFI_DEVICE_PATH_PROTOCOL      End;
} mXhciDevicePath = {
  { ACPI_DEVICE_PATH, ACPI_DP, (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)), (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8), EISA_PNP_ID(0x0A03), 0 },
  { HARDWARE_DEVICE_PATH, HW_PCI_DP, (UINT8)(sizeof(PCI_DEVICE_PATH)), (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8), 0, 0 },
  { HARDWARE_DEVICE_PATH, HW_PCI_DP, (UINT8)(sizeof(PCI_DEVICE_PATH)), (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8), XHCI_FUN, XHCI_DEV },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0 }
};

EFI_SMM_USB_DISPATCH2_PROTOCOL  mStockUsbDispatch;
DATABASE_RECORD                 *mRecord;
UINT16                          mPmBase;
UINT16                          mGpioBase;
UINT32                          mXhciCmd;
UINT32                          mXhciPwr;
UINT32                          mXhciXecp;
UINT32                          mXhciPcdSetting;
/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

**/
EFI_STATUS
EFIAPI
XhciSmiDispatcherDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_USB_DISPATCH2_PROTOCOL        *UsbDispatch;
  //
  // Checking the PCD to 
  //
  mXhciPcdSetting = PcdGet32 (PcdXhciAddonCardSetting);
  if (mXhciPcdSetting == 0) {
    //
    // Quit if no PCD specify
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Get the stock SmmUsbDispatcher protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmUsbDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&UsbDispatch
                    );
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Patch the SmmUsbDispatcher protocol
  //
  CopyMem (&mStockUsbDispatch, UsbDispatch, sizeof (EFI_SMM_USB_DISPATCH2_PROTOCOL));
  UsbDispatch->Register   = XhciSmiRegister;
  UsbDispatch->UnRegister = XhciSmiUnregister;
  //
  // Setup internal variables
  //
  mRecord      = NULL;
  mPmBase      = (UINT16)(PciRead32 (LPC_PCI_PMBASE) & ACPI_PM_BASE_MASK);
  mGpioBase    = (UINT16)(PciRead32 (LPC_PCI_GPIOBASE) & ACPI_GPIO_BASE_MASK);
  mXhciDevicePath.RootBridge.Function = (UINT8)BRIDGE_FUN_FOR_XHCI;
  mXhciDevicePath.RootBridge.Device   = (UINT8)BRIDGE_DEV_FOR_XHCI;
  return EFI_SUCCESS;
}

/**

  Provides the parent dispatch service for the USB SMI source generator.

  This service registers a function (DispatchFunction) which will be called when the USB-
  related SMI specified by RegisterContext has occurred. On return, DispatchHandle 
  contains a unique handle which may be used later to unregister the function using UnRegister().
  The DispatchFunction will be called with Context set to the same value as was passed into 
  this function in RegisterContext and with CommBuffer containing NULL and 
  CommBufferSize containing zero.

  @param[in]  This              Pointer to the EFI_SMM_USB_DISPATCH2_PROTOCOL instance.
  @param[in]  DispatchFunction  Function to register for handler when a USB-related SMI occurs. 
  @param[in]  RegisterContext   Pointer to the dispatch function's context.
                                The caller fills this context in before calling
                                the register function to indicate to the register
                                function the USB SMI types for which the dispatch
                                function should be invoked.
  @param[out] DispatchHandle    Handle generated by the dispatcher to track the function instance.

  @retval EFI_SUCCESS           The dispatch function has been successfully
                                registered and the SMI source has been enabled.
  @retval EFI_DEVICE_ERROR      The driver was unable to enable the SMI source.
  @retval EFI_INVALID_PARAMETER RegisterContext is invalid. The USB SMI type
                                is not within valid range.
  @retval EFI_OUT_OF_RESOURCES  There is not enough memory (system or SMM) to manage this child.

**/
EFI_STATUS
XhciSmiRegister (
  IN  CONST EFI_SMM_USB_DISPATCH2_PROTOCOL      *This,
  IN        EFI_SMM_HANDLER_ENTRY_POINT2        DispatchFunction,
  IN  CONST EFI_SMM_USB_REGISTER_CONTEXT        *RegisterContext,
  OUT       EFI_HANDLE                          *DispatchHandle
  )
{
  EFI_STATUS      Status;
  DATABASE_RECORD *Record;
  UINT32          Value;
  EFI_HANDLE      Handle;
  UINT32          Index;
  UINT32          Data;
  UINT32          Bus;
  UINT32          Bar;
  //
  // Check the owner
  //
  if (RegisterContext->Type == UsbLegacy && CompareMem(RegisterContext->Device, &mXhciDevicePath, sizeof(mXhciDevicePath)) == 0) {
    //
    // Create a XHCI SMI registration
    //
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (DATABASE_RECORD),
                      (VOID **)&Record
                      );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Record->Signature        = DATABASE_RECORD_SIGNATURE;
    Record->Next             = mRecord;
    Record->DispatchFunction = DispatchFunction;
    Record->RegisterContext  = *RegisterContext;
    if (!mRecord) {
      //
      // Setup the related registers and SMI root callback on first time setting
      //	
      gSmst->SmiHandlerRegister (
               XhciSmmCoreDispatcher,
               NULL,
               &Handle
               );
      //
      // Setup GPIO USE in GPIO registers for XHCI SMIB pin
      //
      Value = IoRead32 (mGpioBase + GP_USE_SEL);
      Value |= (1 << GPIO_PIN_FOR_XHCI_SMIB);
      IoWrite32 (mGpioBase + GP_USE_SEL, Value);
      //
      // Setup GPIO IO for input in GPIO registers for XHCI SMIB pin
      //
      Value = IoRead32 (mGpioBase + GP_IO_SEL);
      Value |= (1 << GPIO_PIN_FOR_XHCI_SMIB);
      IoWrite32 (mGpioBase + GP_IO_SEL, Value);
      //
      // Setup GPIO INV for invert signal GPIO registers for XHCI SMIB pin
      //
      Value = IoRead32 (mGpioBase + GP_INV_SEL);
      if (INVERSE_GPIO_SIGNAL == 1) {
        Value |= (1 << GPIO_PIN_FOR_XHCI_SMIB);
      } else {
        Value &= ~(1 << GPIO_PIN_FOR_XHCI_SMIB);
      }
      IoWrite32 (mGpioBase + GP_INV_SEL, Value);
      //
      // Make sure the GPIO_ROUT for XHCI SMIB pin is inactive before set the ALT_GP_SMI
      //
      Value = PciRead32 (LPC_PCI_GPIO_ROUT);
      if (Value & (0x03 << (GPIO_PIN_FOR_XHCI_SMIB * 2))) {
        Value &= ~(0x03 << (GPIO_PIN_FOR_XHCI_SMIB * 2));
        PciWrite32 (LPC_PCI_GPIO_ROUT, Value);
      }
      //
      // Setup ALT_GP_SMI for XHCI SMIB pin
      //
      Value = IoRead16 (mPmBase + ALT_GP_SMI_EN);
      Value |= (1 << GPIO_PIN_FOR_XHCI_SMIB);
      IoWrite16 (mPmBase + ALT_GP_SMI_EN, (UINT16)Value);
      //
      // Setup GPIO_ROUT in LPC for XHCI SMIB pin
      //
      Value = PciRead32 (LPC_PCI_GPIO_ROUT);
      Value &= ~(0x03 << (GPIO_PIN_FOR_XHCI_SMIB * 2));
      Value |= 0x01 << (GPIO_PIN_FOR_XHCI_SMIB * 2);
      PciWrite32 (LPC_PCI_GPIO_ROUT, Value);
      //
      // Setting the record;
      //
      mRecord = Record;
      if (SKIP_HAND_OFF_TO_BIOS_EVENT) {
        Bus       = (PciRead32 (PCI_LIB_ADDRESS (0, BRIDGE_DEV_FOR_XHCI, BRIDGE_FUN_FOR_XHCI, 0x18)) >> 8) & 0xff;
        Bar       = PciRead32 (PCI_LIB_ADDRESS (Bus, XHCI_DEV, XHCI_FUN, 0x10)) & ~0x0f;
        mXhciCmd  = PCI_LIB_ADDRESS (Bus, XHCI_DEV, XHCI_FUN, 0x04);
        //
        // Searching for USB Legacy Support Capability
        //
        mXhciXecp     = 0;
        Index         = ((*(UINT32*)(UINTN)(Bar + 0x10) & 0xffff0000) >> 14);
        while (((Data = *(UINT32*)(UINTN)(Bar + Index)) & 0xff) != 0x01 && (Data & 0xff00) != 0) {
          Index += ((Data & 0xff00) >> (8 - 2));
        }
        if ((Data & 0xff) == 0x01) {
          mXhciXecp = Bar + Index;
        }
        //
        // Get the offset of Power Control Status Register
        //
        for (Index = 0x34, Data = 0; Index < 0x100;) {
          Data = PciRead16 (PCI_LIB_ADDRESS (Bus, XHCI_DEV, XHCI_FUN, Index));
          if (Index != 0x34) {
            if ((Data & 0xff) == 0x01) {
              mXhciPwr = PCI_LIB_ADDRESS (Bus, XHCI_DEV, XHCI_FUN, Index + 4);
              break;
            } else {
      	      //
      	      // The position of next ptr will be next of ID
      	      //
      	      Index = (Data & 0xff00) >> 8;
            }
          } else {
            Index = Data & 0xff;
          }
        }
      }
    }
    *DispatchHandle = (EFI_HANDLE)Record;
    Status = EFI_SUCCESS;
  } else {
    //
    // Transfer to stock function
    //
    Status = mStockUsbDispatch.Register (
               This,
               DispatchFunction,
               RegisterContext,
               DispatchHandle
               );
  }
  return Status;
}

/**

  Unregisters a USB service.

  This service removes the handler associated with DispatchHandle so that it will no longer be 
  called when the USB event occurs.

  @param[in]  This              Pointer to the EFI_SMM_USB_DISPATCH2_PROTOCOL instance.
  @param[in]  DispatchHandle    Handle of the service to remove. 

  @retval EFI_SUCCESS           The dispatch function has been successfully
                                unregistered and the SMI source has been disabled
                                if there are no other registered child dispatch
                                functions for this SMI source.
  @retval EFI_INVALID_PARAMETER The DispatchHandle was not valid.

**/
EFI_STATUS
XhciSmiUnregister (
  IN CONST EFI_SMM_USB_DISPATCH2_PROTOCOL       *This,
  IN       EFI_HANDLE                           DispatchHandle
  )
{
  DATABASE_RECORD *Record;
  DATABASE_RECORD *Prev;
  UINT32          Value;
  
  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Record = mRecord;
  Prev   = NULL;
  while (Record != NULL) {
    if (Record == DispatchHandle) {
      //
      // Don't free the record due to SmmBase is DXE driver and can't accessed during runtime
      //
      if (Prev) {
        Prev->Next = Record->Next;
      }
      if (mRecord == DispatchHandle) {
      	//
      	// Turn off ALT_GP_SMI for XHCI SMIB pin
      	//
      	Value = IoRead16 (mPmBase + ALT_GP_SMI_EN);
        Value &= ~(1 << GPIO_PIN_FOR_XHCI_SMIB);
        IoWrite16 (mPmBase + ALT_GP_SMI_EN, (UINT16)Value);
        //
        // Turn off GPIO_ROUT in LPC for XHCI SMIB pin
        //
        Value = PciRead32 (LPC_PCI_GPIO_ROUT);
        Value &= ~(0x03 << (GPIO_PIN_FOR_XHCI_SMIB * 2));
        PciWrite32 (LPC_PCI_GPIO_ROUT, Value);
        //
        // Remove the record;
        //
      	mRecord = NULL;
      }
      return EFI_SUCCESS;
    }
    Prev   = Record;
    Record = Record->Next;
  }
  //
  // Transfer to stock function
  //
  return mStockUsbDispatch.UnRegister (
           This,
           DispatchHandle
           );
}

/**

  Stall in microsecond.

  @param[in]  MicroSecond       Microsecond for stall

**/
STATIC
VOID
Stall (
  IN UINTN                              MicroSecond
  )
{
  UINTN Elapse;
  UINT8 Toggle;
  
  Toggle = IoRead8(0x61) & 0x10;
  for (Elapse = 0; Elapse < MicroSecond; Elapse += (1000000 / 33333)) {
    while (Toggle == (IoRead8(0x61) & 0x10));
    while (Toggle != (IoRead8(0x61) & 0x10));
  }
}

/**

  Main entry point for an SMM handler dispatch or communicate-based callback.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers 
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should 
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still 
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
EFIAPI
XhciSmmCoreDispatcher (
  IN       EFI_HANDLE                           DispatchHandle,
  IN CONST VOID                                 *Context         OPTIONAL,
  IN OUT   VOID                                 *CommBuffer      OPTIONAL,
  IN OUT   UINTN                                *CommBufferSize  OPTIONAL
  )
{
  DATABASE_RECORD *Record;
  UINT16          Enabled;
  UINT16          Status;
  //
  // Check GPIO SMI for XHCI SMIB pin
  //
  Enabled = IoRead16 (mPmBase + ALT_GP_SMI_EN);
  Status  = IoRead16 (mPmBase + ALT_GP_SMI_STS);
  if ((Enabled & (1 << GPIO_PIN_FOR_XHCI_SMIB)) && (Status & (1 << GPIO_PIN_FOR_XHCI_SMIB))) {
    Record = mRecord;
    while (Record != NULL) {
      if (SKIP_HAND_OFF_TO_BIOS_EVENT) {
        UINT16  CmdState = 0x06;
        UINT16  PwrState = 0x00;
        BOOLEAN QuitFlag = FALSE;
        if (*(UINT32*)(UINTN)(mXhciXecp + 0) == 0xffffffff) {
          //
          // Setup and backup cmd and power state
          //
          CmdState = PciRead16 (mXhciCmd);
          if ((CmdState & 0x06) != 0x06) {
            PciWrite16 (mXhciCmd, CmdState | 0x06);
          }
          PwrState = PciRead16 (mXhciPwr);
          if (PwrState & 0x03) {
            PciWrite16 (mXhciPwr, PwrState & ~0x03);
          }
          //
          // Stall 10ms to waiting for signal stable
          //
          Stall(10 * 1000);
        }
        if (*(UINT32*)(UINTN)(mXhciXecp + 4) & 0x20000000) {
          //
          // Xhci owner changed
          //
          if (!(*(UINT32*)(UINTN)(mXhciXecp + 0) & 0x01000000)) {
            //
            // OS to BIOS, just to setup the Xecp register and leave
            //
            *(volatile UINT32*)(UINTN)(mXhciXecp + 0) = *(volatile UINT32*)(UINTN)(mXhciXecp + 0) | 0x00010000;
            *(volatile UINT32*)(UINTN)(mXhciXecp + 4) = *(volatile UINT32*)(UINTN)(mXhciXecp + 4) | 0x20000000;
            //
            // Stall 1ms to waiting for signal stable
            //
            Stall(1000);
            QuitFlag = TRUE;
          }
        }
        if ((CmdState & 0x06) != 0x06) {
          //
          // Restore the cmd
          //
          PciWrite16 (mXhciCmd, CmdState);
          //
          // Stall 10ms to waiting for signal stable
          //
          Stall(10 * 1000);
        }
        if (PwrState & 0x03) {
          //
          // Restore the power state
          //
          PciWrite16 (mXhciPwr, PwrState);
          //
          // Stall 10ms to waiting for signal stable
          //
          Stall(10 * 1000);
        }
        if (QuitFlag) {
          Record = Record->Next;
          continue;
        }
      }
      Record->DispatchFunction (
                (EFI_HANDLE)Record,
                &Record->RegisterContext,
                NULL,
                NULL
                );
      Record = Record->Next;
    }
    //
    // Clear source
    //
    IoWrite16 (mPmBase + ALT_GP_SMI_STS, (1 << GPIO_PIN_FOR_XHCI_SMIB));
  }
  //
  // Return EFI_WARN_INTERRUPT_SOURCE_QUIESCED to make subsequent root SMM handler get called
  //
  return EFI_WARN_INTERRUPT_SOURCE_QUIESCED;
}
