/** @file

  WheaErrorInj driver functions.

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

#include "WheaErrorInjectSmm.h"

UINT16                                 mEinjErrType;
//[-start-140425-IB10310054-add]//
UINT16                                 mUefiErrorRecordRevision;
//[-end-140425-IB10310054-add]//

STATIC EFI_WHEA_PLATFORM_PROTOCOL      *mWheaPlatform = NULL;
STATIC WHEA_EINJ_PARAM_BUFFER          *mEinjParaRange = NULL;

BOOLEAN                                mAcpi5Support = FALSE;
UINT64                                 mEinjMemAddress;
UINT64                                 mEinjMemAddressRange;

//
// WHEA Serialization Table
//
STATIC
APEI_WHEA_EINJ_INJECTION_INSTRUCTION   mSimEinj[9] = {
//Serialization Action Table
  {  //Action0
    WHEA_EINJ_BEGIN_INJECT_OP,          // BEGIN_INJECTION_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    APEI_WHEA_EINJ_PRESERVE_REGISTER,   // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (DWORD Memory) Address will be filled during boot
    EINJ_BEGIN_OPERATION,               // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {  //Action1
    WHEA_EINJ_GET_TRIGGER_ACTION_TABLE, // GET_TRIGGER_ERROR_STRUC
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x40, 0x00, 0x04, -1},     // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffffffffffff                  // Mask is only valid for 32-bits 
  },
  {  //Action2
    WHEA_EINJ_SET_ERROR_TYPE,           // SET_ERROR_TYPE
    INSTRUCTION_WRITE_REGISTER,         // WRITE_REGISTER
    APEI_WHEA_EINJ_PRESERVE_REGISTER,   // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {  //Action3
    WHEA_EINJ_GET_ERROR_TYPE,           // GET_ERROR_TYPE
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {  //Action4
    WHEA_EINJ_END_INJECT_OP,            // END_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,   // WRITE_REGISTER_VALUE
    APEI_WHEA_EINJ_PRESERVE_REGISTER,   // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (QWORD Memory) Address will be filled during boot
    EINJ_END_OPERATION,                 // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  {  //Action5
    WHEA_EINJ_EXECUTE_INJECT_OP,               // EXECUTE_OPERATION
    INSTRUCTION_WRITE_REGISTER_VALUE,          // WRITE_REGISTER_VALUE
    APEI_WHEA_EINJ_PRESERVE_REGISTER,          // Flags
    0x00,                                      // Reserved
    {0x01, 0x10, 0x00, 0x02, SMI_CMD_IO_PORT}, // GAS (WORD IO). Address will be filled in runtime
    EFI_WHEA_EINJ_EXECUTE_SWSMI,               // Value for InjectError()
    0xffff                                     // Mask is only valid for 16-bits 
  },
  {  //Action6
    WHEA_EINJ_CHECK_BUSY_STATUS,        // CHECK_BUSY_STATUS
    INSTRUCTION_READ_REGISTER_VALUE,    // READ_REGISTER
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x30, 0x00, 0x03, -1},     // GAS (QWORD Memory) Address will be filled during boot
    0x00000001,                         // Value for InjectError()
    0x00000001                          // Mask is only valid for 32-bits 
  },
  {   //Action7
    WHEA_EINJ_GET_CMD_STATUS,           // GET_OPERATION_STATUS
    INSTRUCTION_READ_REGISTER,          // READ_REGISTER
    APEI_WHEA_EINJ_PRESERVE_REGISTER,   // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x40, 0x00, 0x03, -1},      // GAS (QWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0x000001fe                          // Mask is only valid for 32-bits 
  },
  {   //Action8
    WHEA_EINJ_SET_ERROR_TYPE_WITH_ADDRES, // SET_ERROR_TYPE_WITH_ADDRESS
    INSTRUCTION_WRITE_REGISTER,           // WRITE_REGISTER_VALUE
    APEI_WHEA_EINJ_PRESERVE_REGISTER,     // Flags
    0x00,                                 // Reserved
    EinjRegisterFiller,                   // {0x00, 0x40, 0x00, 0x03, -1},    // GAS (QWORD Memory) Address will be filled during boot
    0,                                    // Value for InjectError()
    0xffffffff                            // Mask is only valid for 32-bits 
  }
};

STATIC
WHEA_EINJ_TRIGGER_ACTION_TABLE mSimEinjAction = {
  {sizeof (WHEA_ERROR_TRIGGER_ACTION_HEADER), 0, sizeof (WHEA_EINJ_TRIGGER_ACTION_TABLE), 4},
  { //Action0
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,           // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (DWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  { //Action1
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,           // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (DWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  { //Action2
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,           // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (DWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  },
  { //Action3
    WHEA_EINJ_TRIGGER_ERROR_ACTION,
    INSTRUCTION_NO_OPERATION,           // Intialized as NOP. To be filled by the runtime injection code
    FLAG_NOTHING,                       // Flags
    0x00,                               // Reserved
    EinjRegisterFiller,                 // {0x00, 0x20, 0x00, 0x03, -1},     // GAS (DWORD Memory) Address will be filled during boot
    0,                                  // Value for InjectError()
    0xffffffff                          // Mask is only valid for 32-bits 
  }

};


/**
  Install Error Injection Instructions to Einj table

  @param [in] None
  
  @retval EFI_SUCCESS             Error Injection Table update successfully.

**/
STATIC
EFI_STATUS
InsydeWheaUpdateEinject (
  VOID
  )
{ 
  EFI_STATUS                           Status;
  WHEA_EINJ_TRIGGER_ACTION_TABLE       *TriErrActTable;
  EFI_WHEA_SUPPORT_PROTOCOL            *WheaSupport;
  UINT8                                *ErrLogAddrRange;
  UINT8                                TriErrActTableNum;

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() Start\n", __FUNCTION__));

  //
  // Locate Whea Support protocol in order to call InsydeWheaInstallErrorInjectionMethod
  //
  Status = gBS->LocateProtocol (&gEfiWheaSupportProtocolGuid, NULL, &WheaSupport);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Allocate memory for Error status block to Injection error
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, 0x800, (VOID **)&ErrLogAddrRange);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (ErrLogAddrRange, 0x800);
  //
  // Initial Structure pointers and data for Einj parameter and Error log address range.
  //
  mEinjParaRange = (WHEA_EINJ_PARAM_BUFFER *)ErrLogAddrRange;    // Einj Parameter Range
  TriErrActTable = (WHEA_EINJ_TRIGGER_ACTION_TABLE *) (ErrLogAddrRange + ((sizeof(WHEA_EINJ_PARAM_BUFFER)/0x40)+1)*0x40);
  //
  // Init EINJ action table in the allocated reserved memory
  // Update Error injection Action table - Static initialization is sufficient
  //
  CopyMem(TriErrActTable, &mSimEinjAction, sizeof(WHEA_EINJ_TRIGGER_ACTION_TABLE));
  //
  // Init EINJ parameters
  //
  mEinjParaRange->TriggerActionTable = (UINT64)TriErrActTable;
  mEinjParaRange->ErrorInjectCap = INJECT_ERROR_MEMORY_UE_FATAL | INJECT_ERROR_MEMORY_UE_NON_FATAL | INJECT_ERROR_MEMORY_CE;
  if (mAcpi5Support) {
    mEinjParaRange->ErrorInjectCap |= INJECT_ERROR_VENDOR_DEF_TYPE;
  }
  //
  // Update Error injection serialization informations
  //
  mSimEinj[0].RegisterRegion.Address = (UINTN) &mEinjParaRange->OpState;            // Begin
  mSimEinj[1].RegisterRegion.Address = (UINTN) &mEinjParaRange->TriggerActionTable; // GetTriggerErrorActionTable
  mSimEinj[2].RegisterRegion.Address = (UINTN) &mEinjParaRange->ErrorToInject;      // SetErrorType
  mSimEinj[3].RegisterRegion.Address = (UINTN) &mEinjParaRange->ErrorInjectCap;     // GetErrorType
  mSimEinj[4].RegisterRegion.Address = (UINTN) &mEinjParaRange->OpState;            // End
  mSimEinj[6].RegisterRegion.Address = (UINTN) &mEinjParaRange->OpStatus;           // CheckBusyStatus
  mSimEinj[7].RegisterRegion.Address = (UINTN) &mEinjParaRange->CmdStatus;          // GetCommandStatus

  TriErrActTableNum = 8;
  if (mAcpi5Support) {
    mSimEinj[8].RegisterRegion.Address = (UINTN) &mEinjParaRange->SetErrorAddress; // SetErrorTypeWithAddress
    mEinjParaRange->SetErrorAddress.VendorErrorTypeExtenOffset = (UINT32)((UINTN)&mEinjParaRange->VendorExt - (UINTN)&mEinjParaRange->SetErrorAddress);
    //
    // Set Default to 0
    //
    mEinjParaRange->SetErrorAddress.ApicId = 0;
    mEinjParaRange->SetErrorAddress.MemoryAddress = 0;
    mEinjParaRange->SetErrorAddress.MemoryAddressRange = 0;
    mEinjParaRange->SetErrorAddress.PcieSBDF.Data = 0;
    mEinjParaRange->SetErrorAddress.Flags.Data = 0;

    //
    // if Vendor Error Type present, set Dummy data to Vendor error type extension structure.
    //
    mEinjParaRange->VendorExt.SBDF     = 0; // This provides a PCIe Segment, Bus, Device and Function number which can be used to read the Vendor ID, Device ID and Rev ID
    mEinjParaRange->VendorExt.VendorId = EFI_WHEA_EINJ_VENDOR_ERROR_VENDOR_ID;
    mEinjParaRange->VendorExt.DeviceId = EFI_WHEA_EINJ_VENDOR_ERROR_DEVICE_ID;
    mEinjParaRange->VendorExt.RevId    = EFI_WHEA_EINJ_VENDOR_ERROR_REV_ID;
    TriErrActTableNum = 9;
  }
  //
  // Install Whea Error injection method by call InsydeWheaInstallErrorInjectionMethod from WheaSupport protocol
  //  
  Status = WheaSupport->InsydeWheaInstallErrorInjectionMethod (
                          WheaSupport,
                          INJECT_ERROR_PROCESSOR_CE | INJECT_ERROR_PROCESSOR_UE_FATAL | INJECT_ERROR_PROCESSOR_UE_NON_FATAL | \
                          INJECT_ERROR_MEMORY_CE | INJECT_ERROR_MEMORY_UE_FATAL | INJECT_ERROR_MEMORY_UE_NON_FATAL | \
                          INJECT_ERROR_PCIE_CE | INJECT_ERROR_PCIE_UE_FATAL | INJECT_ERROR_PCIE_UE_NON_FATAL | \
                          INJECT_ERROR_PLATFORM_CE | INJECT_ERROR_PLATFORM_UE_FATAL | INJECT_ERROR_PLATFORM_UE_NON_FATAL,
                          TriErrActTableNum,
                          &mSimEinj
                          );
  
  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Simulate the error injection flow

  @param [in]      DispatchHandle      The handle of this callback, obtained when registering.
  @param [in]      DispatchContext     The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer          Common Buffer.
  @param [in, out] CommBufferSize      Common Buffer Size.

  @retval EFI_SUCCESS             Error Injection Log successfully.

**/
STATIC
EFI_STATUS
InsydeWheaEinjCreateElog (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *DispatchContext, OPTIONAL
  IN OUT   VOID                        *CommBuffer,      OPTIONAL
  IN OUT   UINTN                       *CommBufferSize   OPTIONAL
  )
{
  MEMORY_DEV_INFO                      MemInfo;
  UINT64                               ErrorToInject;

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() Start\n", __FUNCTION__));

//[-start-140425-IB10310054-add]//
  ZeroMem (&MemInfo, sizeof(MEMORY_DEV_INFO));
//[-end-140425-IB10310054-add]//

  mEinjParaRange->OpStatus = 0;
  mEinjParaRange->CmdStatus = WHEA_EINJ_CMD_SUCCESS;

  ErrorToInject = mEinjParaRange->ErrorToInject & 0x7fffffff;

  if (mEinjParaRange->OpState != EINJ_BEGIN_OPERATION || ErrorToInject > INJECT_ERROR_PLATFORM_UE_FATAL) {
    mEinjParaRange->CmdStatus = WHEA_EINJ_CMD_INVALID_ACCESS;
    return EFI_INVALID_PARAMETER;
  }

  if (mWheaPlatform == NULL) {
    mEinjParaRange->CmdStatus = WHEA_EINJ_CMD_UNKNOWN_FAILURE;
    return EFI_INVALID_PARAMETER;
  }

  mEinjParaRange->OpStatus = 1;
//[-start-140425-IB10310054-add]//
  if (mUefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    MemInfo.UefiErrorRecordRevision = GENERIC_ERROR_SECTION_REVISION_UEFI231;
  } else {
    MemInfo.UefiErrorRecordRevision = GENERIC_ERROR_SECTION_REVISION;
  }

  MemInfo.ValidBits = 0; 
  MemInfo.Node       = 0x01;
  MemInfo.ValidBits |= PLATFORM_MEM_NODE_VALID;
  MemInfo.Dimm       = 0x02;
  MemInfo.ValidBits |= PLATFORM_MEM_MODULE_VALID;
  MemInfo.Bank       = 0x03;
  MemInfo.ValidBits |= PLATFORM_MEM_BANK_VALID;
  MemInfo.Device     = 0x04;
  MemInfo.ValidBits |= PLATFORM_MEM_DEVICE_VALID;
  MemInfo.Row        = 0x05;
  MemInfo.ValidBits |= PLATFORM_MEM_ROW_VALID;
  MemInfo.Column     = 0x06;
  MemInfo.ValidBits |= PLATFORM_MEM_COLUMN_VALID;

  MemInfo.ErrorType  = PLATFORM_MEM_ERROR_UNKNOWN;

  if (MemInfo.UefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    MemInfo.ValidBits |= (PLATFORM_RANK_NUMBER_VALID | PLATFORM_CARD_HANDLE_VALID | PLATFORM_Module_HANDLE_VALID);
    MemInfo.Rank               = 0xf1;
    MemInfo.SmBiosCardHandle   = 0xf2;
    MemInfo.SmBiosModuleHandle = 0xf2;    
  }
//[-end-140425-IB10310054-add]//

  if (mAcpi5Support) {
    if ((mEinjParaRange->SetErrorAddress.ErrorTpye & 0x7fffffff) != 0) {
      ErrorToInject = mEinjParaRange->SetErrorAddress.ErrorTpye & 0x7fffffff;

      if (mEinjParaRange->SetErrorAddress.Flags.Bits.ProcessorValid == 0x01) {
        //
        // specifies the physical APIC ID or the X2APIC ID of the processor which is a target for the injection. 
        //
        mEinjParaRange->SetErrorAddress.Flags.Bits.ProcessorValid = 0;
      }
      if (mEinjParaRange->SetErrorAddress.Flags.Bits.MemoryAddressValid == 0x01) {
        MemInfo.Node    = 0x01;
        MemInfo.Branch  = 0x02;
        MemInfo.Channel = 0x03;
        MemInfo.Dimm    = 0x04;
        MemInfo.Rank    = 0x05;
        //
        // specifies the physical address of the memory which is the target for the injection.
        //
        mEinjMemAddress = mEinjParaRange->SetErrorAddress.MemoryAddress;
        //
        // provided a range mask for the address field.
        //
        mEinjMemAddressRange = mEinjParaRange->SetErrorAddress.MemoryAddressRange;
        mEinjParaRange->SetErrorAddress.Flags.Bits.MemoryAddressValid = 0;
      }
      if (mEinjParaRange->SetErrorAddress.Flags.Bits.PcieSBDFValid == 0x01) {
        //
        // Set PCIe Segment,Bus,Device,Function fieldss
        //
        mEinjParaRange->SetErrorAddress.Flags.Bits.PcieSBDFValid = 0;
      }
    }

  }

  mWheaPlatform->InsydeWheaPlatformRestartAllElog (mWheaPlatform);
  
  switch (ErrorToInject) {
  case INJECT_ERROR_MEMORY_CE:
    DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: INJECT_ERROR_MEMORY_CE\n"));
    mWheaPlatform->InsydeWheaPlatformElogMemory (
                     mWheaPlatform,
                     GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED,
                     &MemInfo
                     );
    InsydeWheaTriggerSci (gSmst);
    break;

  case INJECT_ERROR_MEMORY_UE_FATAL:
  case INJECT_ERROR_MEMORY_UE_NON_FATAL:  
    DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: INJECT_ERROR_MEMORY_UE\n"));
    mWheaPlatform->InsydeWheaPlatformElogMemory (
                     mWheaPlatform,
                     GEN_ERR_SEV_PLATFORM_MEMORY_FATAL,
                     &MemInfo
                     );
    InsydeWheaTriggerNmi (gSmst);
    break;

  default: /// unsupported error type
    mEinjParaRange->CmdStatus = WHEA_EINJ_CMD_UNKNOWN_FAILURE;
    break;
  }

  mEinjParaRange->OpStatus = 0;

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}
  
/**
  According the inject error type to trigger NMI or SCI

  @param [in]      DispatchHandle      The handle of this callback, obtained when registering.
  @param [in]      DispatchContext     The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer          Common Buffer.
  @param [in, out] CommBufferSize      Common Buffer Size.
  
  @retval EFI_SUCCESS             Error Injection trigger NMI or SCI successfully.

**/
STATIC
EFI_STATUS
InsydeWheaEinjTriggerNmiSci (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *DispatchContext, OPTIONAL
  IN OUT   VOID                        *CommBuffer,      OPTIONAL
  IN OUT   UINTN                       *CommBufferSize   OPTIONAL
  )
{
  MEMORY_DEV_INFO                      MemInfo;
  PCIE_PCI_DEV_INFO                    PcieInfo;

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() Start\n", __FUNCTION__));

//[-start-140425-IB10310054-modify]//
  ZeroMem (&MemInfo, sizeof(MEMORY_DEV_INFO));
  ZeroMem (&PcieInfo, sizeof(PCIE_PCI_DEV_INFO));

  if (mUefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    MemInfo.UefiErrorRecordRevision = GENERIC_ERROR_SECTION_REVISION_UEFI231;
  } else {
    MemInfo.UefiErrorRecordRevision = GENERIC_ERROR_SECTION_REVISION;
  }

  //
  // Declare fake error source value
  //
  MemInfo.ValidBits = 0; 
  MemInfo.Node       = 0x01;
  MemInfo.ValidBits |= PLATFORM_MEM_NODE_VALID;
  MemInfo.Dimm       = 0x02;
  MemInfo.ValidBits |= PLATFORM_MEM_MODULE_VALID;
  MemInfo.Bank       = 0x03;
  MemInfo.ValidBits |= PLATFORM_MEM_BANK_VALID;
  MemInfo.Device     = 0x04;
  MemInfo.ValidBits |= PLATFORM_MEM_DEVICE_VALID;
  MemInfo.Row        = 0x05;
  MemInfo.ValidBits |= PLATFORM_MEM_ROW_VALID;
  MemInfo.Column     = 0x06;
  MemInfo.ValidBits |= PLATFORM_MEM_COLUMN_VALID;

  MemInfo.ErrorType  = PLATFORM_MEM_ERROR_UNKNOWN;

  if (mUefiErrorRecordRevision == GENERIC_ERROR_SECTION_REVISION_UEFI231) {
    MemInfo.ValidBits |= (PLATFORM_RANK_NUMBER_VALID | PLATFORM_CARD_HANDLE_VALID | PLATFORM_Module_HANDLE_VALID);
    MemInfo.Rank               = 0xf1;
    MemInfo.SmBiosCardHandle   = 0xf2;
    MemInfo.SmBiosModuleHandle = 0xf2;    
  }
//[-end-140425-IB10310054-modify]//

  PcieInfo.Bus      = 0x00;
  PcieInfo.Device   = 0x00;
  PcieInfo.Function = 0x00;
  PcieInfo.Segment  = 0x00;
  //
  // Clear all error data in all Error status blocks 
  // by using InsydeWheaPlatformRestartAllElog from WheaPlatform Support Protocol
  //
  mWheaPlatform->InsydeWheaPlatformRestartAllElog (mWheaPlatform);
  switch (mEinjErrType) {

  case INJECT_ERROR_MEMORY_CE:
    DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: INJECT_ERROR_MEMORY_CE\n"));
    mWheaPlatform->InsydeWheaPlatformElogMemory (
                     mWheaPlatform,
                     GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED,
                     &MemInfo
                     );
    InsydeWheaTriggerSci (gSmst);
    break;

  case INJECT_ERROR_MEMORY_UE_FATAL:
  case INJECT_ERROR_MEMORY_UE_NON_FATAL:  
    DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: INJECT_ERROR_MEMORY_UE\n"));
    mWheaPlatform->InsydeWheaPlatformElogMemory (
                     mWheaPlatform,
                     GEN_ERR_SEV_PLATFORM_MEMORY_FATAL,
                     &MemInfo
                     );
    InsydeWheaTriggerNmi (gSmst);
    break;

  case INJECT_ERROR_PCIE_CE:
    DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: INJECT_ERROR_PCIE_CE\n"));
    mWheaPlatform->InsydeWheaPlatformElogPcieRootDevBridge (
                     mWheaPlatform,
                     GEN_ERR_SEV_PCIE_CORRECTED,
                     &PcieInfo
                     );
    
    InsydeWheaTriggerSci (gSmst);
    break;

  case INJECT_ERROR_PCIE_UE_FATAL:
  case INJECT_ERROR_PCIE_UE_NON_FATAL:  
    DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: INJECT_ERROR_PCIE_UE\n"));
    mWheaPlatform->InsydeWheaPlatformElogPcieRootDevBridge(
                     mWheaPlatform,
                     GEN_ERR_SEV_PCIE_FATAL,
                     &PcieInfo
                     );
    InsydeWheaTriggerNmi (gSmst);
    break;
  
  default:
    break;
  }
  //
  // Call function (WheaPlatformHooks) to end Whea error dat log
  //
  mWheaPlatform->InsydeWheaPlatformEndElog (mWheaPlatform);

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() End\n", __FUNCTION__));
  return EFI_SUCCESS;
}

/**
  Entry point of the Whea Error inject driver. 

  @param [in] ImageHandle         The image handle of the DXE Driver, DXE Runtime Driver, 
                                  DXE SMM Driver, or UEFI Driver.
  @param [in] SystemTable         A pointer to the EFI System Table.

  @retval EFI_SUCCESS             Driver initialized successfully.

**/
EFI_STATUS
EFIAPI
InsydeWheaErrorInjectSmm (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{   
  EFI_STATUS                           Status;
  EFI_HANDLE                           SwHandle;
  EFI_SMM_SW_DISPATCH2_PROTOCOL        *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT          SwContext;

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() Start\n", __FUNCTION__));

  if (InSmm ()) {
    //
    // Get Setup Variable to determine if this module should be loaded.
    //

    //
    // Get WHEA inject error type from setup variable
    //
    mEinjErrType = PcdGet16 (PcdApeiErrorInjectType);
    if (PcdGet8 (PcdApeiAcpiVersion) == 0x03) {
      mAcpi5Support = TRUE;
    }

//[-start-140425-IB10310054-add]//
    //
    // Get WHEA UEFI Error record Recision.
    //
    mUefiErrorRecordRevision = PcdGet16 (PcdApeiUefiVersion);
//[-end-140425-IB10310054-add]//

    //
    // Update EINJ table information
    //
    Status = InsydeWheaUpdateEinject ();
    if (EFI_ERROR(Status)) {
      return Status;
    }
    //
    // Locate the PCH SMM SW dispatch protocol
    //
    Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "[WheaErrorInjectSmm] Locate gEfiSmmSwDispatch2ProtocolGuid Status = %r\n", Status));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
    //
    // Register SW SMI handler to build error data for simulated error injection
    //
    SwContext.SwSmiInputValue = EFI_WHEA_EINJ_EXECUTE_SWSMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           InsydeWheaEinjCreateElog,
                           &SwContext,
                           &SwHandle
                           );
    ASSERT_EFI_ERROR (Status);
    //
    // Register SW SMI handler to trigger NMI/SCI
    //
    SwContext.SwSmiInputValue = EFI_WHEA_EINJ_TRIGGER_SWSMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           InsydeWheaEinjTriggerNmiSci,
                           &SwContext,
                           &SwHandle
                           );
    ASSERT_EFI_ERROR (Status);
    //
    // Get Whea Platform Support protocol instance
    //
    Status = gSmst->SmmLocateProtocol (&gEfiWheaPlatformProtocolGuid, NULL, &mWheaPlatform);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  DEBUG ((DEBUG_INFO, "[APEI_INJECT] INFO: %a() End\n", __FUNCTION__));
  return EFI_SUCCESS;
}
