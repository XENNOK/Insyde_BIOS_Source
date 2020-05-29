/** @file
  This driver is for Switchable Graphics Feature SMM initialize.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SwitchableGraphicsSmm.h>

//
// Function Prototypes
//
VOID
MxmInt15Callback (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs,
  IN VOID                               *Context
  );

VOID
IntelSgCallback (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs,
  IN VOID                               *Context
  );

STATIC
BOOLEAN
SmmGpioRead (
  IN UINT8                              Value
  );

STATIC
VOID
SmmGpioWrite (
  IN UINT8                              Value,
  IN BOOLEAN                            Level
  );

//
// Module globals
//
EFI_SMM_SYSTEM_TABLE2                         *mSmst;
H2O_SG_INFO_PROTOCOL                          *mSgInfoProtocol;

/**

  Switchable Graphics feature SMM driver entry point.
  This driver will install SG and MXM int 15 callback function.

  @param[in] ImageHandle   Image handle of this driver.
  @param[in] SystemTable   Pointer to standard EFI system table.

  @retval EFI_SUCCESS    Switchable Graphics feature DXE initialized successfully.
  @retval !EFI_SUCCESS   Switchable Graphics feature doesn't be supported.

**/
EFI_STATUS
SwitchableGraphicsSmmInitEntry (
  IN EFI_HANDLE                               ImageHandle,
  IN EFI_SYSTEM_TABLE                         *SystemTable
  )
{
  BOOLEAN                                     InSmm;
  EFI_SMM_BASE2_PROTOCOL                      *SmmBase2;
  EFI_SMM_INT15_SERVICE_PROTOCOL              *SmmInt15Service;
  EFI_STATUS                                  Status;

  SmmInt15Service = NULL;
  mSgInfoProtocol = NULL;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**)&SmmBase2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmBase2->InSmm (SmmBase2, &InSmm);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!InSmm) {
    return EFI_SUCCESS;
  }

  //
  // Great!  We're now in SMM!
  //
  Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Locate the SG Info Protocol
  //
  Status = gBS->LocateProtocol (&gH2OSwitchableGraphicsInfoProtocolGuid, NULL, (VOID **)&mSgInfoProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mSmst->SmmLocateProtocol (&gEfiSmmInt15ServiceProtocolGuid, NULL, (VOID **)&SmmInt15Service);
  if (EFI_ERROR(Status)) { 
    return Status;
  }

  Status = SmmInt15Service->InstallInt15ProtocolInterface (
                              SmmInt15Service,
                              SWITCHABLE_GRAPHICS_INT15_FUN_NUM,
                              IntelSgCallback,
                              NULL
                              );
  if (Status == EFI_ALREADY_STARTED) {
    //
    // use new callback function to replace original one
    //
    Status = SmmInt15Service->ReinstallInt15ProtocolInterface (
                                SmmInt15Service,
                                SWITCHABLE_GRAPHICS_INT15_FUN_NUM,
                                IntelSgCallback,
                                NULL
                                );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    Status = SmmInt15Service->InstallInt15ProtocolInterface (
                                SmmInt15Service,
                                MXM_INT15_FUN_NUM,
                                MxmInt15Callback,
                                NULL
                                );
    if (Status == EFI_ALREADY_STARTED) {
      //
      // use new callback function to replace original one
      //
      Status = SmmInt15Service->ReinstallInt15ProtocolInterface (
                                  SmmInt15Service,
                                  MXM_INT15_FUN_NUM,
                                  MxmInt15Callback,
                                  NULL
                                  );
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }
  }

  return EFI_SUCCESS;
}

/**

  MXM (Mobile PCI Express Module) INT15 Callback Routine specific function Number 0x5F80
  get MXM SIS (System Information Structure) binary or VBIOS.

  @param[in]  CpuRegs   The structure containing CPU Registers (AX, BX, CX, DX etc.).
                        CX - For most Settings (Configurations) described in the VBIOS Spec.
                        BX - In most cases, it is a calling register. It also can be used
                             as a return (for Settings/Configurations) register like CX
                        AX - Indicate the Status of Function Supported Fail/Success
  @param[in]  Context   Pointer to the context data registerd to the Event.

  @return None.

**/
VOID
MxmInt15Callback (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs,
  IN VOID                               *Context
  )
{
  UINT32                                *MxmBinSrc;
  UINT16                                MxmBinSize;

  MxmBinSrc  = NULL;
  MxmBinSize = 0;

  if ((CpuRegs->X.AX & 0xFFFF) != MXM_INT15_FUN_NUM) {
    return;
  }

  switch (CpuRegs->H.BL) {
    case 0:
      //
      // Function 0 -Return Specification Support Level
      //
      // Entry:
      //   AX = 5F80h
      //   BL = 00h
      //   BH = FFh
      //   EBX[16:31] = Adapter ID
      //   CX = Revision of the MXM software specification
      //        that is supported by the MXM Module
      //
      // Return:
      //   AX = 005Fh to indicate that the system bios supports this function
      //   BL = Revision of the MXM software specification that is supported
      //        by the System Format is binary coded decimal,
      //        for example: 30h = 3.0, etc.
      //   CX = MXM functions supported
      //        Bit 0 = 1
      //        Bit 1 = 1 if Function 1 is supported, 0 if not supported
      //        Bit 2 = 1 if Function 2 is supported, 0 if not supported
      //        Bit 3 = 1 if Function 3 is supported, 0 if not supported
      //        Bit 4 = 1 if Function 4 is supported, 0 if not supported
      //        Bit 7 = 1 if Function 7 is supported, 0 if not supported
      //        Bit 8 = 1 if Function 8 is supported, 0 if not supported
      //        Bit 9 = 1 if Function 9 is supported, 0 if not supported
      //
      CpuRegs->H.BL &= ~(0xFF);
      CpuRegs->H.BL |= 0x30; // Support 3.0

      if (mSgInfoProtocol->SgMode == SgModeDisabled) {
        //
        // Return Function supported but failed
        //
        CpuRegs->X.CX &= ~(0xFFFF);
        CpuRegs->X.CX = 0x0000;
        CpuRegs->X.AX &= ~(0xFFFF);
        CpuRegs->X.AX |= 0x015F;
        break;
      }
      if (mSgInfoProtocol->SgMode == SgModeDgpu) {
        //
        // Support function 0, 1
        //
        CpuRegs->X.CX &= ~(0xFFFF);
        CpuRegs->X.CX |= 0x0003;
      }
      if (mSgInfoProtocol->SgMode == SgModeMuxless) {
        //
        // Support function 0, 1, 7
        //
        CpuRegs->X.CX &= ~(0xFFFF);
        CpuRegs->X.CX |= 0x0083;
      }
      //
      // Return Function supported and successful
      //
      CpuRegs->X.AX &= ~(0xFFFF);
      CpuRegs->X.AX |= 0x005F;
      break;

    case 1:
      //
      // Function 1 -Return a Pointer to the MXM Structure
      //
      // Entry:
      //   AX = 5F80h
      //   BL = 01h
      //   BH = FFh
      //   EBX[16:31] = Adapter ID
      //   CX = Identifier for the data block to return
      //
      // Return:
      //   AX = 005Fh to indicate that the system bios supports this function
      //   BX = Vendor ID of data block if CX = 0x80-0x8F, else 0
      //   ES:DI = Pointer to the MXM structure in real mode memory (< 1 MB)
      //
      if (mSgInfoProtocol == NULL) {
        //
        // Return Function supported but failed
        //
        CpuRegs->X.AX &= ~(0xFFFF);
        CpuRegs->X.AX |= 0x015F;
        break;
      }
      if ((CpuRegs->E.EBX & 0xFF000000) > 0x01000000) { 
        MxmBinSrc  = (UINT32 *)mSgInfoProtocol->SlaveMxmBinFile.Address;
        MxmBinSize = (UINT16)mSgInfoProtocol->SlaveMxmBinFile.Size;
      } else {
        MxmBinSrc  = (UINT32 *)mSgInfoProtocol->MasterMxmBinFile.Address;
        MxmBinSize = (UINT16)mSgInfoProtocol->MasterMxmBinFile.Size;
      }

      if ((MxmBinSize > 0x1000) || (MxmBinSize == 0) || (MxmBinSrc == 0)) {
        //
        // Return Function supported but failed
        //
        CpuRegs->X.AX &= ~(0xFFFF);
        CpuRegs->X.AX |= 0x015F;
        break;
      }
      CpuRegs->X.ES &= ~(0xFFFF);
      CpuRegs->X.ES = (UINT16)((((UINTN)MxmBinSrc) & ~(0xFFFF)) >> 4);
      CpuRegs->E.EDI &= ~(0xFFFF);
      CpuRegs->E.EDI |= (UINT32)(((UINTN)MxmBinSrc) & (0xFFFF));
      //
      // The system bios supports this function but function failed (reserve bit31 - bit16)
      //
      CpuRegs->X.AX &= ~(0xFFFF);
      CpuRegs->X.AX |= 0x005F;
      break;

    case 7:
      //
      // Function 7 -Return a Pointer to the VBIOS Image for ROM-Less Adapters
      // 
      // Entry:
      //   AX = 5F80h
      //   BL = 07h
      //   BH = FFh
      //   EBX[16:31] = Adapter ID
      // 
      // Return:
      //   AX = 005Fh to indicate that the system bios supports this function
      //   ESDI = Physical Memory offset to the 128K aperture
      //          containing the selected VBIOS image.
      //
      if (mSgInfoProtocol == NULL) {
        //
        // The system bios supports this function but function failed (reserve bit31 - bit16)
        //
        CpuRegs->X.AX &= ~(0xffff);
        CpuRegs->X.AX |= 0x015F;
        break;
      }
      CpuRegs->X.ES = 0;
      CpuRegs->E.EDI &= ~(0xFFFFFFFF);
      CpuRegs->E.EDI |= (UINT32)((mSgInfoProtocol->Vbios.Address) & (0xFFFFFFFF));
      //
      // The system bios supports this function but function failed (reserve bit31 - bit16)
      //
      CpuRegs->X.AX &= ~(0xffff);
      CpuRegs->X.AX |= 0x005F;
      break;

    default:
      break;
  }
}

/**

  INTEL Switchable Graphics INT15 Callback Routine specific function Number 0x5F70 get SG mode status.

  @param[in]  CpuRegs   The structure containing CPU Registers (AX, BX, CX, DX etc.).
                        CX - For most Settings (Configurations) described in the VBIOS Spec.
                        BX - In most cases, it is a calling register. It also can be used
                             as a return (for Settings/Configurations) register like CX
                        AX - Indicate the Status of Function Supported Fail/Success
  @param[in]  Context   Pointer to the context data registerd to the Event.

  @return None.

**/
VOID
IntelSgCallback (
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs,
  IN VOID                               *Context
  )
{
  //
  // Int15h Hook 5F70h
  //
  switch (CpuRegs->H.CH) {
    case 2:
      //
      // Get SG/Non-SG Mode
      // AX = 5F70h
      // CH = 02, Get SG/ Non-SG
      // Return Status:
      // CL = SG Mode
      //      0 - SG Enabled
      //      1 - SG Disabled
      // AX = Return Status (function not supported if AL! = 5Fh):
      //    = 015Fh, Function supported but failed
      //    = 005Fh, Function supported and successful
      //
      if (mSgInfoProtocol->SgMode == SgModeMuxless) {
        //
        // SG Enable
        //
        CpuRegs->H.CL &= ~(0xFF);
        CpuRegs->H.CL |= 0x00;
      } else {
        //
        // SG Disabled
        //
        CpuRegs->H.CL &= ~(0xFF);
        CpuRegs->H.CL |= 0x01;
      }
      //
      // Return Function supported and successful
      //
      CpuRegs->X.AX &= ~(0xFFFF);
      CpuRegs->X.AX |= 0x005F;
      break;

    default:
      break;
  }
}

/**

  Read SG related GPIO pin value.

  @param[in]  Value   PCH GPIO number and Active value.
                      Bit0 to Bit7 - PCH GPIO Number.
                      Bit8         - GPIO Active value (0 = Active Low; 1 = Active High).

  @return             GPIO pin read value.

**/
STATIC
BOOLEAN
SmmGpioRead (
  IN UINT8                              Value
  )
{
  BOOLEAN                               Active;
  UINT32                                Data;
  UINT16                                BitOffset;
  UINT16                                GpioAddress;

  GpioAddress = mSgInfoProtocol->GpioBaseAddress;

  //
  // Extract GPIO number and Active value
  //
  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (mSgInfoProtocol->IsUltBoard) {
    //
    // Read specified value GPIO
    //
    mSmst->SmmIo.Io.Read (
                      &mSmst->SmmIo,
                      SMM_IO_UINT32,
                      (GpioAddress + R_PCH_GP_N_CONFIG0 + Value * 8 + 0x0),
                      1,
                      &Data
                      );
    if ((Data >> 2) & 1) {
      Data = (Data >> 30) & 1; // GPI_LVL
    } else {
      Data = (Data >> 31) & 1; // GPO_LVL
    }
  } else {
    if (Value < 0x20) {
      GpioAddress += R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      GpioAddress += R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      GpioAddress += R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
    //
    // Read specified value GPIO
    //
    mSmst->SmmIo.Io.Read (
                      &mSmst->SmmIo,
                      SMM_IO_UINT32,
                      GpioAddress,
                      1,
                      &Data
                      );
    Data >>= BitOffset;
  }

  if (Active == 0) {
     Data = ~Data;
  }

  return (BOOLEAN) (Data & 0x1);
}

/**

  Write SG related GPIO pin value.

  @param[in]  Value   PCH GPIO number and Active value.
                      Bit0 to Bit7 - PCH GPIO Number.
                      Bit8         - GPIO Active value (0 = Active Low; 1 = Active High).
  @param[in]  Level   Write data (0 = Disable; 1 = Enable).

  @return none.

**/
STATIC
VOID
SmmGpioWrite (
  IN UINT8                              Value,
  IN BOOLEAN                            Level
  )
{
  BOOLEAN                               Active;
  UINT32                                Data;
  UINT16                                BitOffset;
  UINT16                                GpioAddress;

  GpioAddress = mSgInfoProtocol->GpioBaseAddress;

  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (Active == 0) {
     Level = (~Level) & 0x1;
  }

  if (mSgInfoProtocol->IsUltBoard) {
    mSmst->SmmIo.Io.Read (
                      &mSmst->SmmIo,
                      SMM_IO_UINT32,
                      (GpioAddress + R_PCH_GP_N_CONFIG0 + Value * 8 + 0x0),
                      1,
                      &Data
                      );
    Data &= ~(0x1 << 31);
    Data |= (Level << 31);
    mSmst->SmmIo.Io.Write (
                      &mSmst->SmmIo,
                      SMM_IO_UINT32,
                      (GpioAddress + R_PCH_GP_N_CONFIG0 + Value * 8 + 0x0),
                      1,
                      &Data
                      );
  } else {
    if (Value < 0x20) {
      GpioAddress += R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      GpioAddress += R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      GpioAddress += R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
    mSmst->SmmIo.Io.Read (
                      &mSmst->SmmIo,
                      SMM_IO_UINT32,
                      GpioAddress,
                      1,
                      &Data
                      );
    Data &= ~(0x1 << BitOffset);
    Data |= (Level << BitOffset);
    mSmst->SmmIo.Io.Write (
                      &mSmst->SmmIo,
                      SMM_IO_UINT32,
                      GpioAddress,
                      1,
                      &Data
                      );
  }

  return;
}
