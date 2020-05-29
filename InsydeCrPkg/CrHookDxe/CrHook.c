/** @file
  CrHookDxe driver implementation

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

#include "CrHook.h"
#include "CrBds.h"

#include <Protocol/TerminalEscCode.h>
#include <Protocol/PciRootBridgeIo.h>

#define UART_MCR_OFFSET           4
#define PCI_CONFIG_COMMAND_WORD   4
#define UART_IER_OFFSET           0x01
#define PCI_CONFIG_BAR0           0x10

#define UART_MCR_OUT2_BIT               (1 << 3)
#define PCI_CONFIG_DISABLE_INTERRUPT    (1 << 10)
#define PCI_CONFIG_BUS_MASTER           (1 << 2)

EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL             *gRootBridgeIo;     
EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL    *gCrService;
EFI_CONSOLE_REDIRECTION_INFO                *gCrInfo;
EFI_LEGACY_BIOS_PLATFORM_HOOKS              mLegacyBiosPlatformHooks;
EFI_EVENT    mStoNotifyEvent;
VOID         *mStoRegistration;
EFI_EVENT    mCrServiceNotifyEvent;
VOID         *mCrServiceRegistration;
EFI_EVENT    mLegacyBootEvent;


//
// Baud Rate Divisor.   Baud Rate 1200 Divisor = 115200/1200 = 96
//
static UINT16 mBaudRateDivisor [] = {96, 48, 24, 12, 6, 3, 2, 1 };

//
// This table use for translate CrInfo terminal type to EBdaCrInfo's TerminalType
// This table is depend on AdvanceVfr.vfr (vt100=Bit0 vt100P=Bit1 vt-utf8=Bit2 pc-ansi=Bit3)
//
static UINT8  gTerminalFlagTable [] = {TP_VT100, TP_VT100P, TP_VTUTF8, TP_PCANSI};

EFI_LEGACY_BIOS_PLATFORM_PROTOCOL                 *mLegacyBiosPlatform;
EFI_HANDLE                                        mCrHookImageHandle;
extern EFI_CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL  mCrBdscr;

#pragma pack(1)
typedef struct {
  UINT8   TerminalType : 4;
  UINT8   Reserve : 1;
  UINT8   DataType : 3;
  UINT8   Data;
  UINT16  OffsetOfString;
} CR_EFI_ESC_SEQUENCE_CODE;

typedef struct {
  UINT16                      Count;
  CR_EFI_ESC_SEQUENCE_CODE    CrEscSequenceCode[1];
} CR_EFI_ESC_SEQUENCE_CODE_TABLE;

typedef struct {
  UINT16      CommandStrOffset;
  UINT8       CommandType;
  UINT16      Command;
} CR_EFI_SPECIAL_COMMAND;

typedef struct {
  UINT16                     Count;
  CR_EFI_SPECIAL_COMMAND     CrSpecialCommand[1];
} CR_EFI_SPECIAL_COMMAND_TABLE;
#pragma pack()


/**

  Clear UART IER Register to disable UART interrupt

**/
VOID
ClearUartIer (
  VOID
  )
{
  CR_PCI_SERIAL_DEVICE    *PciSerial;
  UINT64                  PciAddress;
  UINT16                  ComPortAddr;
  UINT16                  u16;
  UINT8                   u8;
  UINTN                   Index;
  
  if (gCrInfo == NULL) {
    return;
  }

  u16 = 0;
  for (Index = 0; Index < gCrInfo->DeviceCount; Index++) {

    ComPortAddr = 0;
    switch (gCrInfo->CRDevice[Index].Type) {

      case ISA_SERIAL_DEVICE:        
        ComPortAddr = gCrInfo->CRDevice[Index].Device.IsaSerial.ComPortAddress;
        break;

      case PCI_SERIAL_DEVICE:
        PciSerial = &gCrInfo->CRDevice[Index].Device.PciSerial;
        PciAddress = EFI_PCI_ADDRESS (PciSerial->Bus, PciSerial->Device, PciSerial->Function, PCI_CONFIG_BAR0);
        gRootBridgeIo->Pci.Read (gRootBridgeIo, EfiPciWidthUint16, PciAddress, 1, &u16);
        ComPortAddr = u16 & 0xfffc;
        break;

      default:
        break;
    }

    if (ComPortAddr != 0) {
      u8 = 0;
      gRootBridgeIo->Io.Write( gRootBridgeIo, EfiPciWidthUint8, ComPortAddr + UART_IER_OFFSET, 1, &u8);
    }
  }
}


/**

  Set interrupt of UART device

  @param  EnableIntr            Enable or disable UART interrupt

**/
VOID
SetSerialInterrupt (
  IN UINT8    EnableIntr
  )
{
  CR_ISA_SERIAL_DEVICE    *IsaSerial;
  CR_PCI_SERIAL_DEVICE    *PciSerial;
  UINT64                  PciAddress;
  UINT16                  u16;
  UINT8                   u8;
  UINTN                   Index;


  if (gCrInfo == NULL) {
    return;
  }

  u16 = 0;
  u8 = 0;
  for (Index = 0; Index < gCrInfo->DeviceCount; Index++) {

    switch (gCrInfo->CRDevice[Index].Type) {

      case ISA_SERIAL_DEVICE:
        IsaSerial = &gCrInfo->CRDevice[Index].Device.IsaSerial;
        gRootBridgeIo->Io.Read (gRootBridgeIo, EfiPciWidthUint8, IsaSerial->ComPortAddress + UART_MCR_OFFSET, 1, &u8);

        if (EnableIntr) {
          u8 = u8 | UART_MCR_OUT2_BIT;
        } else {
          u8 = u8 & ~UART_MCR_OUT2_BIT;
        }
        gRootBridgeIo->Io.Write (gRootBridgeIo, EfiPciWidthUint8, IsaSerial->ComPortAddress + UART_MCR_OFFSET, 1, &u8);
        break;

      case PCI_SERIAL_DEVICE:
        PciSerial = &gCrInfo->CRDevice[Index].Device.PciSerial;
        PciAddress = EFI_PCI_ADDRESS (PciSerial->Bus, PciSerial->Device, PciSerial->Function, PCI_CONFIG_COMMAND_WORD);
        gRootBridgeIo->Pci.Read (gRootBridgeIo, EfiPciWidthUint16, PciAddress, 1, &u16);

        if (EnableIntr) {
          u16 = u16 & ~PCI_CONFIG_DISABLE_INTERRUPT;
          u16 = u16 | PCI_CONFIG_BUS_MASTER;
        } else {
          u16 = u16 | PCI_CONFIG_DISABLE_INTERRUPT;
          u16 = u16 & ~PCI_CONFIG_BUS_MASTER;
        }
        gRootBridgeIo->Pci.Write (gRootBridgeIo, EfiPciWidthUint16, PciAddress, 1, &u16);
        break;

      default:
        break;
    }
  }
}


/**

  Enable interrupt of UART device

**/
VOID
EnableSerialInterrupt (
  VOID
  )
{
  SetSerialInterrupt (1);
}


/**

  Disable interrupt of UART device

**/
VOID
DisableSerialInterrupt (
  VOID
  )
{
  SetSerialInterrupt (0);

  //
  // Due to some UART device can't be disable interrupt by SetSerialInterrupt (0)
  // So, we need clear IER register setting to ensure UART device interrupt disable
  //
  ClearUartIer ();
}


/**

  Translate Esc Sequence Code into KeyBoard scan code 

  @param  TerminalEscCode  Pointer to Terminal ESC Sequence Code protocol
  @param  EscTableIndex      Index of Terminal ESC Sequence Code 

  @retval   return translate result

**/
UINT8
TranslateEfiScanCode (
  IN EFI_TERMINAL_ESC_CODE_PROTOCOL    *TerminalEscCode,
  IN UINTN                             EscTableIndex
  )
{
  EFI_TO_KB_SCANCODE_MAP    *pScanCodeMap;
  ESC_SEQUENCE_CODE         *pEscSeqCode;
  UINT8                     DataType;
  UINTN                     Index;


  DataType = TerminalEscCode->EscSequenceCode[EscTableIndex].DataType;

  if ((DataType == ESC_CODE_SCANCODE) || (DataType == ESC_CODE_EXTENTION)) {

    pEscSeqCode = TerminalEscCode->EscSequenceCode;
    pScanCodeMap = TerminalEscCode->EfiToKbScanCode;

    for (Index = 0; pScanCodeMap[Index].EfiScanCode != 0; Index++) {
      if (pScanCodeMap[Index].EfiScanCode == pEscSeqCode[EscTableIndex].Data) {
        return (UINT8) (pScanCodeMap[Index].KbScanCode);
      }
    }
  }

  return (UINT8)(TerminalEscCode->EscSequenceCode[EscTableIndex].Data);
}


/**

  Fill Console Redirection supported special commands to CR_EFI_INFO structure 

  @param  TerminalEscCode  Pointer to Console Redirection EFI Information structure

  @retval   EFI_SUCCESS               Special commands fill success
  @retval   EFI_UNSUPPORTED       Special commands fill fail

**/
EFI_STATUS
FillCrSpecialCommandTable (
  IN OUT CR_EFI_INFO    *CrEfiInfo
  )
{
  EFI_STATUS                        Status;
  EFI_TERMINAL_ESC_CODE_PROTOCOL    *TerminalEscCode;
  CR_EFI_SPECIAL_COMMAND_TABLE      *CrSpcTable;
  UINTN                             Index;
  CHAR8                             AsciiStr[128];
  UINTN                             StringLength;
  CHAR8                             *pCrSpcStr;

  Status = gBS->LocateProtocol (&gTerminalEscCodeProtocolGuid, NULL, (VOID **)&TerminalEscCode);
  if (EFI_ERROR (Status) || (TerminalEscCode->CrSpecialCommandCount == 0)) {
    CrEfiInfo->CrSpecialCommandTableOffset = 0;
    return EFI_UNSUPPORTED;
  }

  CrSpcTable = (CR_EFI_SPECIAL_COMMAND_TABLE *) CrEfiMemAlloc ( sizeof(CR_EFI_SPECIAL_COMMAND_TABLE) + 
                                                                sizeof (CR_EFI_SPECIAL_COMMAND) * 
                                                                (TerminalEscCode->CrSpecialCommandCount - 1));
  if (CrSpcTable == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  CrEfiInfo->CrSpecialCommandTableOffset = (UINT16)((UINTN)CrSpcTable - mCrMemStart);
  
  CrSpcTable->Count = TerminalEscCode->CrSpecialCommandCount;
  
  for (Index = 0; Index < TerminalEscCode->CrSpecialCommandCount; Index++) {
    CrSpcTable->CrSpecialCommand[Index].Command = TerminalEscCode->CrSpecialCommand[Index].Command;
    CrSpcTable->CrSpecialCommand[Index].CommandType = TerminalEscCode->CrSpecialCommand[Index].CommandType;
    
    UnicodeStrToAsciiStr ((CONST CHAR16 *)TerminalEscCode->CrSpecialCommand[Index].CommandStr, AsciiStr);
    
    StringLength = AsciiStrLen (AsciiStr);
    pCrSpcStr = (CHAR8 *) CrEfiMemAlloc (StringLength + 1);
    CopyMem (pCrSpcStr ,AsciiStr, StringLength + 1);
	
    CrSpcTable->CrSpecialCommand[Index].CommandStrOffset = (UINT16)((UINTN)pCrSpcStr - mCrMemStart);
  }
  
  return EFI_SUCCESS;
}


/**

  Fill Console Redirection supported Terminal ESC Sequence Code to CR_EFI_INFO structure 

  @param  TerminalEscCode  Pointer to Console Redirection EFI Information structure

  @retval   EFI_SUCCESS              Terminal ESC Sequence Code fill success
  @retval   EFI_UNSUPPORTED      Terminal ESC Sequence Code fill fail

**/
EFI_STATUS
FillTerminalEscCode (
  IN OUT CR_EFI_INFO    *CrEfiInfo
  )
{
  EFI_STATUS                        Status;
  EFI_TERMINAL_ESC_CODE_PROTOCOL    *TerminalEscCode;
  CR_EFI_ESC_SEQUENCE_CODE_TABLE    *CrEscCodeTable;
  UINTN                             Index;
  CHAR8                             AsciiStr[128];
  UINTN                             StringLength;
  CHAR8                             *pCrEscCode;
  UINT8                             Data;

  Status = gBS->LocateProtocol (&gTerminalEscCodeProtocolGuid, NULL, (VOID **)&TerminalEscCode);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Build ESC sequence code table 
  //
  CrEscCodeTable = (CR_EFI_ESC_SEQUENCE_CODE_TABLE *) CrEfiMemAlloc (sizeof(CR_EFI_ESC_SEQUENCE_CODE_TABLE) +
                                                                     sizeof(CR_EFI_ESC_SEQUENCE_CODE) *
                                                                     (TerminalEscCode->EscCodeCount - 1));
  if (CrEscCodeTable == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  CrEfiInfo->TerminalEscCodeOffset = (UINT16)((UINTN)CrEscCodeTable - mCrMemStart);

  CrEscCodeTable->Count = TerminalEscCode->EscCodeCount;
  
  for (Index = 0; Index < TerminalEscCode->EscCodeCount; Index++) {
  
    CrEscCodeTable->CrEscSequenceCode[Index].TerminalType = TerminalEscCode->EscSequenceCode[Index].TerminalType;
    CrEscCodeTable->CrEscSequenceCode[Index].Reserve = TerminalEscCode->EscSequenceCode[Index].Reserve;
    CrEscCodeTable->CrEscSequenceCode[Index].DataType = TerminalEscCode->EscSequenceCode[Index].DataType;
    Data = TranslateEfiScanCode(TerminalEscCode, Index);
    
    CrEscCodeTable->CrEscSequenceCode[Index].Data = Data;

    //
    // Process ESC sequence code string
    //
    UnicodeStrToAsciiStr ((CONST CHAR16 *)TerminalEscCode->EscSequenceCode[Index].EscSequenceCode, AsciiStr);
    StringLength = AsciiStrLen(AsciiStr);
    pCrEscCode = (CHAR8 *) CrEfiMemAlloc (StringLength + 1);
    CopyMem (pCrEscCode ,AsciiStr, StringLength + 1);

    CrEscCodeTable->CrEscSequenceCode[Index].OffsetOfString = (UINT16)((UINTN)pCrEscCode - mCrMemStart);
    
  }

  return EFI_SUCCESS;
}


/**

  Fill Console Redirection Terminal device information to CR_EFI_INFO structure 

  @param  CrPolicy              Pointer to Console Redirection policy protocol
  @param  CRInfo               Pointer to Console Redirection information structure

  @retval   EFI_SUCCESS                      Console Redirection Terminal device information fill success
  @retval   EFI_INVALID_PARAMETER     Console Redirection Terminal device information fill fail

**/
EFI_STATUS
FillCRInfo (
  IN EFI_CR_POLICY_PROTOCOL          *CrPolicy,
  IN EFI_CONSOLE_REDIRECTION_INFO    *CRInfo
  )
{
  CR_EFI_INFO    *CrEfiInfo;
  CR_DEVICE      *CRDevice;
  UINT8          UartProtocol;
  UINTN          CRInfoSize;
  UINTN          Index;
  
  if (CRInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UartProtocol = 0;
  if (CrPolicy->CRDataBits == CR_DATA_8BIT) {
    UartProtocol = UART_DATA_BITS_8;      
  } else { 
    UartProtocol = UART_DATA_BITS_7;      
  }

  if (CrPolicy->CRStopBits == CR_STOP_2BIT) {
    UartProtocol |= UART_STOP_BITS_2;     
  }
  
  switch (CrPolicy->CRParity) {
    case CR_PARITY_EVEN:
      UartProtocol |= UART_PARITY_EVEN;   
      break;

    case CR_PARITY_ODD:
      UartProtocol |= UART_PARITY_ODD;   
      break;

    default:
      break;
  }

  //
  // Caculate memory size that CRINFO need
  //
  CRInfoSize = sizeof(CR_EFI_INFO) + CRInfo->DeviceCount * sizeof(CR_DEVICE);

  //
  // Allocate memory from CR Efi Memory Manager
  //
  CrEfiInfo = (CR_EFI_INFO *) CrEfiMemAlloc (CRInfoSize);
  
  CrEfiInfo->Signature = SIGNATURE_16 ('C', 'R');
  CrEfiInfo->Revision = CR_INFO_REVISION;
  CrEfiInfo->HLength = sizeof (CR_EFI_INFO);
  CrEfiInfo->DeviceCount = CRInfo->DeviceCount;
  CRDevice = (CR_DEVICE *)((UINT8 *)CrEfiInfo + sizeof(CR_EFI_INFO));
  
  for (Index = 0; Index < CRInfo->DeviceCount; Index++) {

    if (CRInfo->CRDevice[Index].Type == ISA_SERIAL_DEVICE) {
      //
      // Process ISA Serial device
      //
      CRDevice[Index].Type = ISA_SERIAL_DEVICE;
      CRDevice[Index].Device.IsaDevice.PortAddress = CRInfo->CRDevice[Index].Device.IsaSerial.ComPortAddress;
      CRDevice[Index].Device.IsaDevice.Irq = CRInfo->CRDevice[Index].Device.IsaSerial.ComPortIrq;
    } else if (CRInfo->CRDevice[Index].Type == PCI_SERIAL_DEVICE) {
      //
      // Process PCI Serial device
      //
      CRDevice[Index].Type = PCI_SERIAL_DEVICE;
      CRDevice[Index].Device.PciDevice.Bus = CRInfo->CRDevice[Index].Device.PciSerial.Bus;
      CRDevice[Index].Device.PciDevice.DevFun = (CRInfo->CRDevice[Index].Device.PciSerial.Device << 3) | \
                                                 CRInfo->CRDevice[Index].Device.PciSerial.Function;
    } else {
      CRDevice[Index].Type = UNKNOW_SERIAL_DEVICE;
    }
    //
    // Set common data
    //
    CRDevice[Index].BaudRateDivisor = CRInfo->CRDevice[Index].BaudRateDivisor;
  }
  
  CrEfiInfo->BaudRateDivisor = mBaudRateDivisor[CrPolicy->CRBaudRate];
  CrEfiInfo->Protocol = UartProtocol;
  CrEfiInfo->FIFOLength = CrPolicy->CRFifoLength;
  CrEfiInfo->FlowControl = CrPolicy->CRFlowControl;
  CrEfiInfo->TerminalType = gTerminalFlagTable[CrPolicy->CRTerminalType];
  CrEfiInfo->FeatureFlag = CrPolicy->CRVideoType      | 
                           CrPolicy->CRTerminalKey    | 
                           CrPolicy->CRComboKey       |
                           CrPolicy->CRTerminalRows   | 
                           CrPolicy->CR24RowsPolicy   |
                           CrPolicy->CRUnKbcSupport   |
                           CrPolicy->CRAutoRefresh    |
                           CrPolicy->CRManualRefresh  |
                           CrPolicy->CRTerminalCharSet|
                           CrPolicy->CRAsyncTerm;


  CrEfiInfo->ShowHelp = CrPolicy->CRShowHelp;
  CrEfiInfo->InfoWaitTime = CrPolicy->CRInfoWaitTime;
  CrEfiInfo->CRAfterPost = CrPolicy->CRAfterPost;
  CrEfiInfo->CRHeadlessVBuffer = CrPolicy->CRHeadlessVBuffer;
  CrEfiInfo->CRWriteCharInterval = CrPolicy->CRWriteCharInterval;
  
  //
  // Set Headless flag to notify serial OPROM.  (H:Headless  V:Vga card)
  //
  if (CRInfo->Headless == TRUE) {
    CrEfiInfo->Headless = 'H';
  } else {
    CrEfiInfo->Headless = 'V';
  }
  
  FillTerminalEscCode (CrEfiInfo);
  
  FillCrSpecialCommandTable (CrEfiInfo);

  BDA_DATA(CR_MEM_MANAGER, UINT16) = (UINT16) mCrMemStart;
               
  BDA_DATA(CR_CRINFO, UINT16) = (UINT16)((UINTN)CrEfiInfo - mCrMemStart);
    
  return EFI_SUCCESS;
}


/**

  Load Console Redirection Option ROM

  @retval   EFI_SUCCESS             Console Redirection Option ROM load success
  @retval   EFI_UNSUPPORTED     Console Redirection Option ROM load fail

**/
EFI_STATUS
LoadOpRomImage (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;
  VOID                        *LocalRomImage;
  UINTN                       LocalRomSize;
  UINTN                       Flags;
  EFI_IA32_REGISTER_SET       RegisterSet;
  UINT16                      VideoOriginalMode;

  
  //
  // Load Serial Redirect Oprom
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  VideoOriginalMode = 0;
  LocalRomSize = 0;
  LocalRomImage = NULL;
  Status = GetSectionFromAnyFv (
             PcdGetPtr(PcdConsoleRedirectionOpRomFile),
             EFI_SECTION_RAW,
             0,
             &LocalRomImage,
             &LocalRomSize
             );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (LocalRomImage != NULL || LocalRomSize != 0) {
    //
    // Save Original Vedio Mode
    //
    ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));
    RegisterSet.X.AX = 0x4F03;
    LegacyBios->Int86 (LegacyBios, 0x10, &RegisterSet);
    if (RegisterSet.X.AX == 0x4F) {
      VideoOriginalMode = RegisterSet.X.BX;
    }
    //
    // Set the 80x25 Text VGA Mode before dispatch CRS ROM
    //
    RegisterSet.H.AH = 0x00;
    RegisterSet.H.AL = 0x03;
    LegacyBios->Int86 (LegacyBios, 0x10, &RegisterSet);

    Status = LegacyBios->InstallPciRom (
                           LegacyBios,
                           NULL,
                           &LocalRomImage,
                           &Flags,
                           NULL,
                           NULL,
                           NULL,
                           NULL
                           );
    //
    // Restore Original Vedio Mode
    //
    ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));
    RegisterSet.X.AX = 0x4F02;
    RegisterSet.X.BX = VideoOriginalMode;
    LegacyBios->Int86 (LegacyBios, 0x10, &RegisterSet);
  }

  return Status;
}


/**

  Notify function for LegacyBoot event be invoked

  @param  Event     The Event that is being processed
  @param  Context  The Event Context

**/
VOID
EFIAPI
LegacyBootNotifyFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINT16    *BdaEBdaOffset;
  UINT16    EBdaSegment;
  UINT8     *pBiosPhase;

  //
  // End of POST time for console redirection
  //
  BdaEBdaOffset = (UINT16 *)(UINTN)BDA_EBDA_OFFSET;
  EBdaSegment = *BdaEBdaOffset;
  pBiosPhase = (UINT8 *)(UINTN)(((UINT32)EBdaSegment << SEGMENT_SHIFT) + BIOS_PHASE);
  *pBiosPhase = BIOS_PHASE_OS;

  EnableSerialInterrupt ();
  
  gBS->CloseEvent (mLegacyBootEvent);
  
  return;
}


/**

  Allows Console Redirection perform specific required action after a LegacyBios operation.

  @param  This                         The protocol instance pointer
  @param  Mode                        Will bypass to next caller
  @param  Type                         Will bypass to next caller
  @param  DeviceHandle             Will bypass to next caller
  @param  ShadowAddress          Will bypass to next caller
  @param  Compatibility16Table  Will bypass to next caller
  @param  AdditionalData           Will bypass to next caller

  @retval   EFI_SUCCESS             The operation performed successfully.
  @retval   EFI_UNSUPPORTED     Mode is not supported on the platform.

**/
EFI_STATUS
EFIAPI
CrsPlatformHooks (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
  IN EFI_GET_PLATFORM_HOOK_MODE          Mode,
  IN UINT16                              Type,
  IN EFI_HANDLE                          DeviceHandle,
  IN OUT UINTN                           *ShadowAddress,
  IN EFI_COMPATIBILITY16_TABLE           *Compatibility16Table,
  IN VOID                                **AdditionalData OPTIONAL
  )
{
  EFI_STATUS    Status;

  //
  //  Execute original LegacyBios function for setup legacy environment before
  //  CROpROM running.
  //
  Status = mLegacyBiosPlatformHooks (
             This,
             Mode,
             Type,
             DeviceHandle,
             ShadowAddress,
             Compatibility16Table,
             AdditionalData
             );

  if (gCrService != NULL) {
    switch (Mode) {

    case EfiPlatformHookPrepareToScanRom:
      //
      // Set flag for CRS before Oprom runing
      //
      EBDA_DATA(BIOS_PHASE, UINT8) = BIOS_PHASE_OPROM;

      EnableSerialInterrupt ();
      break;

    case EfiPlatformHookShadowServiceRoms:
      break;
      
    case EfiPlatformHookAfterRomInit:

      DisableSerialInterrupt ();
      //
      // Set flag for CRS after Oprom runing
      //
      EBDA_DATA(BIOS_PHASE, UINT8) = BIOS_PHASE_INIT;

      break;

     default:
      break;
      
    }
  }
  
  return Status;
}


/**

  Confirm AB segment can be accessed

  @retval   EFI_SUCCESS               AB segment can be accessed.
  @retval   EFI_ACCESS_DENIED     AB segment can not be accessed.

**/
EFI_STATUS
AbSegAccessConfirm (
  VOID
  )
{
  UINT8    RestoreData;
  UINT8    *pVBuffer;
  UINT8    Data[2];
  UINT8    Index;

  RestoreData = 0;
  Data[0] = 0x55;
  Data[1] = 0xAA;
  pVBuffer = (UINT8 *)((UINTN)(TEXT_PAGE_0_START));

  CopyMem (&RestoreData, pVBuffer, sizeof (UINT8));

  for (Index = 0; Index < 2; Index++) {

    CopyMem (pVBuffer, &Data[Index], sizeof (UINT8));
    if (CompareMem (pVBuffer, &Data[Index], sizeof (UINT8)) != 0) {
      return EFI_ACCESS_DENIED;
    }
  }
  
  CopyMem (pVBuffer, &RestoreData, sizeof (UINT8));
  
  return EFI_SUCCESS;
}


/**

  Notify function for Simple Text Output protocol  installed

  @param  Event     The Event that is being processed
  @param  Context  The Event Context

**/
VOID
EFIAPI
EfiStoNotifyFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
)
{
  EFI_STATUS                      Status;
  EFI_SMM_ACCESS2_PROTOCOL        *SmmAccess;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  EFI_CR_POLICY_PROTOCOL          *CrPolicy;
  EFI_HANDLE                      Handle;
  UINTN                           HandleSize;
  UINT16                          *pVgaBuffer;
  UINT16                          TextElement;
  UINTN                           TextCount;
  

  //
  //  Check Console Redirection is Enable or Disable
  //
  Status = gBS->LocateProtocol (&gConsoleRedirectionServiceProtocolGuid, NULL, (VOID **)&gCrService);
  if (EFI_ERROR (Status)) {
    return;
  }
  
  if (gCrService->CRInfo == NULL) {
    return;
  } else {
    gCrInfo = gCrService->CRInfo;
  }
  
  Status = gBS->LocateProtocol (&gEfiCRPolicyProtocolGuid, NULL, (VOID **)&CrPolicy);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Hook in LegacyBiosPlatform to do update BiosPhase Flag
  //
  if (mLegacyBiosPlatform == NULL) {
    Status = gBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, (VOID **)&mLegacyBiosPlatform);
    if (Status == EFI_SUCCESS) {
      mLegacyBiosPlatformHooks = mLegacyBiosPlatform->PlatformHooks;
      mLegacyBiosPlatform->PlatformHooks = CrsPlatformHooks;
    }
  }
  
  //
  // If this is not headless platform that Serial Redirection OPROM must be loaded after VGA OPROM'
  // The VGA always have GraphicsOutput protocol and DevicePath protocol. we just check the two protocol 
  // to make sure VGA have be connect. And the VGA OPROM is be loaded.
  //
  if (!gCrInfo->Headless) {
    while (TRUE) {
      HandleSize = sizeof (EFI_HANDLE);
      Status = gBS->LocateHandle (
                      ByRegisterNotify,
                      NULL,
                      mStoRegistration,
                      &HandleSize,
                      &Handle
                      );
      if (Status == EFI_NOT_FOUND) {
        return;
      }
      ASSERT_EFI_ERROR (Status);
      Status = gBS->HandleProtocol (Handle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
      if (!EFI_ERROR(Status)) {
        Status = gBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
        if (!EFI_ERROR (Status)) {
          break;
        }
      }
    }
  }

  //
  // If Headless , Enable AB segment
  //
  if (gCrInfo->Headless && (CrPolicy->CRHeadlessVBuffer == CR_HEADLESS_USE_VBUFFER)) {

    Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&SmmAccess);
    
    if (Status == EFI_SUCCESS) {

      Status = SmmAccess->Open (SmmAccess);
      
      if (Status == EFI_SUCCESS) {
        Status = AbSegAccessConfirm ();
        if (Status == EFI_SUCCESS) {
       	  //
          // Initialize VGA buffer (B8000 ~ B9000) for Option Rom ,EX: SCSI , PXE ... etc.
          //
          TextElement = CHAR_SPACE | ((COLOR_BG_BLACK | COLOR_FG_WHITE) << 8);
          pVgaBuffer = (UINT16 *)((UINTN)(TEXT_PAGE_0_START));
          TextCount = 0;
          do {
            *pVgaBuffer = TextElement;
            pVgaBuffer++;
            TextCount++;
          } while (TextCount < TEXT_PAGE_SIZE / 2);
          DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nConsole Redirection : Usb AB Segment as VBuffer in Headless system\n" ) );
        }
      }
    }
    
    //
    // If CAN NOT use AB segment(VBuffer) be used, auto change to use EBDA space
    //
    if (EFI_ERROR (Status)) {
      CrPolicy->CRHeadlessVBuffer = CR_HEADLESS_USE_EBDA;
      DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nConsole Redirection : Usb EBDA as VBuffer in Headless system\n" ) );
    }
  }
  
  //
  // Fill CRInfo in CrEfiMemory space. That hand off CRInfo to Option Rom.
  //
  FillCRInfo (CrPolicy, gCrInfo);

  //
  // Load Console Redirection Option ROM
  //
  Status = LoadOpRomImage ();

  //
  //  When After Post time, we must notify OPRom to work and close SMM AB segment.
  //
  EfiCreateEventLegacyBootEx (TPL_NOTIFY, LegacyBootNotifyFunction, NULL, &mLegacyBootEvent);

  //
  // Load OpROM complete, free CREfiMemory space
  //
  FreeCrEfiMemorySpace ();

  //
  //  We have load OPRom then close SimpleTextInNotifyEvent and CRServiceNotifyEvent
  //
  gBS->CloseEvent (mStoNotifyEvent);
  gBS->CloseEvent (mCrServiceNotifyEvent);

  return;

}


/**
  The driver entry point.
  
  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:           Driver initialized successfully
  @retval  others                     Driver initialized unsuccessfully

**/
EFI_STATUS
EFIAPI
ConsoleRedirectionHookEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
)
{
  EFI_STATUS    Status;

  gCrInfo             = NULL;
  gCrService          = NULL;
  mLegacyBiosPlatform = NULL;
  mCrHookImageHandle  = ImageHandle;
  
  //
  // Install the CONSOLE_REDIRECTION_BDSCR_PROTOCOL interface
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gCRBdsHookProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mCrBdscr
                  );

  
  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL,(VOID **) &gRootBridgeIo);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EfiStoNotifyFunction,
                  NULL,
                  &mStoNotifyEvent
                  );
  if( EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gEfiSimpleTextOutProtocolGuid,
                  mStoNotifyEvent,
                  &mStoRegistration
                  );
  
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EfiStoNotifyFunction,
                  NULL,
                  &mCrServiceNotifyEvent
                  );
  if( EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gConsoleRedirectionServiceProtocolGuid,
                  mCrServiceNotifyEvent,
                  &mCrServiceRegistration
                  );

  return Status;
}

