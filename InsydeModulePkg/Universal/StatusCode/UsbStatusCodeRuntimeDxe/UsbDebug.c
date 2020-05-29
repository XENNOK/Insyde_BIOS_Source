/** @file
  USB Status Code related definitions and fundtions

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
#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include "UsbDebug.h"

STATIC
UINT8 DataPID[2] =
{
  DATA0_PID,
  DATA1_PID
};


typedef struct {
  UINT32 Bypass;
  UINT32 EhciControl;
  UINT32 UsbDebugPort;
  UINT32 DebugPort;
  UINT32 BypassTimeout;
} DEBUGIO_CONFIG;

struct {
  DEBUGIO_CONFIG Config;
  UINT32 EhciBar;
} DebugIoConfig =
{
  { 1,				        // 0:Bypass connection off, 1:Bypass connection on
    FixedPcdGet8 (PcdUsbDebugEhciControl),	  	// Ehci port
    1,		            // USB debug port
    0x0080,			      // Debug port
    BYPASS_TIMEOUT    // Bypass timeout
  },
  FixedPcdGet32 (PcdUsbDebugEhciBaseAddress)
};

typedef struct _DeviceTable{
  UINT16 VenderID;
  UINT8  EhciHcDev[2];
  UINT8  EhciHcFun;
  UINT8  PwrSts;
  UINT8  DbgBase;
  UINT8  DbgPort;
} DeviceTable;

DeviceTable EhciTable[] =
{
  //
  // Intel ICH
  //
  { 0x8086, { 0x1d, 0x1a }, 0x07, 0x54, 0x5a, 0x00 },
  //
  // SIS 964
  //
  { 0x1039, { 0x03, 0x00 }, 0x03, 0x54, 0x5a, 0x00 },
  //
  // VIA 8251
  //
  { 0x1106, { 0x10, 0x00 }, 0x04, 0x84, 0x8a, 0x00 },
  //
  // ATI SB600
  //
  { 0x1002, { 0x13, 0x00 }, 0x05, 0xc4, 0xe6, 0x00 },
  //
  // ATI SB700
  //
  { 0x1002, { 0x12, 0x13 }, 0x02, 0xc4, 0xe6, 0x00 },
  //
  // AMD Hudson-2/Hudson-3, SB900
  //
  { 0x1022, { 0x12, 0x13 }, 0x02, 0xc4, 0xe6, 0x00 },
  //
  // nVidia MCP
  //
  { 0x10de, { 0x02, 0x00 }, 0x01, 0x84, 0x46, 0x74 },
  //
  // Empty for ending
  //
  { 0x0000, { 0x00, 0x00 }, 0x00, 0x00, 0x00, 0x00 },
};

#define DEVICE_NUMBER     (sizeof (EhciTable) / sizeof (DeviceTable))
#define NOT_FOUND_DEVICE  DEVICE_NUMBER
#define TIME_FRAMES       8



UINT32
GetEhciDevice (
  IN  UINTN                     Number,
  IN  BOOLEAN                   DeviceData
  )
{
  UINT32 EhciAddress;
  UINT32 EhciVender;
  UINT32 ClassCode;
  UINT16 VenderID;
  UINTN  Dev;
  UINTN  Fun;
  UINT32  HCNum;
  UINT32  Index;

  EhciVender = 0;
  //
  // EHCI Auto-detection
  //
  for (Dev = 0, HCNum = 0; Dev < 0x20; Dev ++) {
    for (Fun = 0; Fun < 8; Fun ++) {
      EhciAddress = PCI_LIB_ADDRESS(0, Dev, Fun, 0);
      ClassCode = PciRead32 (EhciAddress + PLATFORM_USB20_RID);
      if ((ClassCode & 0xFFFFFF00) == 0x0C032000) {
        VenderID = PciRead16 (EhciAddress + PLATFORM_USB20_VID);
        //
        // Check vender ID
        //
        for (Index = 0; EhciTable[Index].VenderID != 0; Index ++) {
          if (EhciTable[Index].VenderID == VenderID) {
            if (VenderID != 0x10de) {
              //
              // Non-nVidia method
              //
              if (EhciTable[Index].EhciHcDev[Number] == Dev) {
                EhciVender  = Index;
                if (DeviceData) {
                  return EhciAddress;
                } else {
                  return EhciVender;
                }
              }
            } else {
              //
              // nVidia method
              //
              if (HCNum == Number) {
                EhciVender  = Index;
                if (DeviceData) {
                  return EhciAddress;
                } else {
                  return EhciVender;
                }
              }
            }
          }
        }
        HCNum ++;
      }
    }
  }
  if (DeviceData) {
    return 0;
  } else {
    return NOT_FOUND_DEVICE;
  }
}

/**
 Get EHCI PCI address.

 @param [in]   Register         PCI register

 @retval None

**/
UINT32
GetEhciAddress (
  IN  UINT32                    Register
  )
{
  UINT32                        EhciAddress;

  EhciAddress = 0;
  EhciAddress = GetEhciDevice (DebugIoConfig.Config.EhciControl, TRUE);

  if (EhciAddress == 0) return 0;

  if (EhciAddress) return (EhciAddress + Register);

  return 0;
}

/**
 Read BAR address.

 @param None

 @retval None

**/
UINT32
ReadBAR (
  VOID
  )
{
  UINT32  BarAddress;
  UINT32  PciAddress;

  PciAddress = GetEhciAddress(PLATFORM_USB20_MEMBASE);
  BarAddress = PciRead32 (PciAddress);
  if (BarAddress != 0) {
    if (BarAddress != DebugIoConfig.EhciBar) {
      if ((*(UINT32*)(UINTN)BarAddress & 0xff000000) == 0x01000000 && (*(UINT32*)(UINTN)BarAddress & 0x000000ff) > 0) {
        return BarAddress;
      }
    }
  }
  PciWrite32 (PciAddress, DebugIoConfig.EhciBar);
  return DebugIoConfig.EhciBar;
}

/**
 GetCtrlPointer 0x20.

 @param None

 @retval Address                

**/
EFI_USB_CTRL_REG*
GetCtrlPointer (
  VOID
  )
{
  UINT32                        Address;

  Address = ReadBAR ();
  Address += ((EFI_USB_CAP_REG*)(UINTN)Address)->CapLength;
  return  (EFI_USB_CTRL_REG *)(UINTN)Address;
}

/**
 GetDebugPointer 0xA0.

 @param None

 @retval Address                0x98080400 + 0xA0

**/
EFI_USB_DBG_REG*
GetDebugPointer (
  VOID
  )
{
  UINT16  Offset;
  UINT32  Address;
  UINT32  PciAddress;
  UINT32  EhciVender;

  EhciVender = 0;
  EhciVender = GetEhciDevice (DebugIoConfig.Config.EhciControl, FALSE);

  if (EhciVender == NOT_FOUND_DEVICE) return 0;

  PciAddress = GetEhciAddress (EhciTable[EhciVender].DbgBase);
  Offset = PciRead16 (PciAddress);
  Address = ReadBAR () + (Offset & 0x1fff);
  return  (EFI_USB_DBG_REG *)(UINTN)Address;
}

/**
 Get USB debug port number.

 @param None

 @return Port Number

**/
UINTN
GetUsbDebugPortNum (
  VOID
  )
{
  UINT32  Address;
  UINTN   PortNum;

  Address = ReadBAR ();
  PortNum = (((EFI_USB_CAP_REG*)(UINTN)Address)->HcsParams >> 20) & 0x0f;
  if (PortNum) PortNum --;

  return PortNum;
}

/**
 Set USB debug port number.

 @param [in]   PortNum          Port Number

 @retval None

**/
VOID
SetUsbDebugPortNum (
  IN UINTN PortNum
  )
{
  UINT32  Data;
  UINT32  PciAddress;
  UINT32  EhciVender;

  EhciVender = 0;
  EhciVender = GetEhciDevice (DebugIoConfig.Config.EhciControl, FALSE);

  if (EhciVender == NOT_FOUND_DEVICE) return;

  if (EhciTable[EhciVender].VenderID == 0x10de) {
    //
    // nVidia MCP
    //
    PciAddress = GetEhciAddress (EhciTable[EhciVender].DbgPort);
    Data = PciRead32 (PciAddress);
    Data &= ~0x0000f000;
    Data |= (PortNum + 1) << 12;
    PciWrite32 (PciAddress, Data);
  }
}

/**
 DetectEhciExist.

 @param None

                                TRUE  : USB 2.0 exist
                                FALSE : USB 2.0 not exist

**/
BOOLEAN
DetectEhciExist (
  VOID
  )
{
  if (GetEhciAddress(PLATFORM_USB20_RID) == 0) return FALSE;
  return  TRUE;
}


/**
 Enable Ehci Reg0x04->0x06.

 @param None

 @retval None

**/
VOID
EnableEhci (
  VOID
  )
{
  UINT32  Data;
  UINT32  PciAddress;
  UINT32  EhciVender;

  EhciVender = 0;
  EhciVender = GetEhciDevice (DebugIoConfig.Config.EhciControl, FALSE);

  if (EhciVender == NOT_FOUND_DEVICE) return;
  //
  // Set power state to D0
  //
  Data = 0x00;
  PciAddress = GetEhciAddress (EhciTable[EhciVender].PwrSts);
  PciWrite16 (PciAddress, (UINT16)Data);
  //
  // Enable BME and MSE
  //
  Data = 0x06;
  PciAddress = GetEhciAddress (PLATFORM_USB20_PCICMD);
  PciWrite8 (PciAddress, (UINT8)Data);
}

/**
 Disable Ehci Reg0x04->0x00.

 @param None

 @retval None

**/
VOID
DisableEhci (
  VOID
  )
{
  UINT32  PciAddress;
  //
  // Disable BME and MSE
  //
  PciAddress = GetEhciAddress (PLATFORM_USB20_PCICMD);
  PciWrite8 (PciAddress, 0x00);
}

/**
 Sets EHCI Bar

 @param None

 @retval None

**/
VOID
SetMemBAR (
  VOID
  )
{
  UINT32  EhciBar;
  UINT32  PciAddress;

  PciAddress = GetEhciAddress(PLATFORM_USB20_MEMBASE);
  EhciBar = PciRead32 (PciAddress);

  if (EhciBar != 0) return;

  EhciBar = DebugIoConfig.EhciBar;
  PciWrite32 (PciAddress, EhciBar);
}

/**
 UsbDebugPortOut.
 Check DebugPtr->Sc.Done = 1

 @param [in]   Buffer
 @param [in]   Length
 @param [in]   TokenPid
 @param [in]   Toggle
 @param [in]   DebugPtr

 @retval EFI_SUCCESS
 @retval EFI_TIMEOUT

**/
EFI_STATUS
UsbDebugPortOut (
  IN  UINT8                     *Buffer,
  IN  UINTN                     Length,
  IN  UINT8                     TokenPid,
  IN  UINT8                     Toggle,
  IN  EFI_USB_DBG_REG           *DebugPtr
)
{
  UINTN                 Index;
  UINTN                 Timeout;
  UINT8                 Toggle0;
  UINT8                 Toggle1;
  EFI_USB_DBG_SC        DebugSC;
  //
  // Set TOKEN_PID_CNT
  //
  *(UINT32*)&DebugPtr->Pid = (Toggle << 8) | TokenPid;
  //
  // Fill the Data to Debug Buffer
  //
  if (Length != 0) {
    DebugPtr->Data0 = *(UINT32*)&Buffer[0];
    DebugPtr->Data1 = *(UINT32*)&Buffer[4];
  }
  //
  // Set WRITE_READ#=1 and GO_CNT=1
  //
  DebugSC = DebugPtr->Sc;
  DebugSC.DataLength = (UINT32)Length;
  DebugSC.Write = DEBUG_WRITE;
  DebugSC.Go = 1;
  DebugPtr->Sc = DebugSC;
  for(Index = 0, Toggle0 = 0, Timeout = PEI_TIMEOUT_TICKS * 2; Index < Timeout;) {

    if (DebugPtr->Sc.Done) {
      if (DebugPtr->Sc.Error && DebugPtr->Sc.Exception > 0) {
        return EFI_DEVICE_ERROR;
      }
      break;
    }
    Toggle1 = IoRead8(0x61);
    if (((Toggle0 ^ Toggle1) & 0x10) != 0) {
      Toggle0 = Toggle1;
      Index ++;
    }
  }
  if (Index == Timeout) {
    return EFI_TIMEOUT;
  }
  return EFI_SUCCESS;
}

/**
 UsbDebugPortIN.
 Check DebugPtr->Sc.Done = 1
 *** CAUTION : The size of Buffer must large then 8 bytes ***

 @param [in]   Buffer
 @param [in, out] Length
 @param [in]   TokenPid
 @param [in]   Toggle
 @param [in]   DebugPtr

 @retval EFI_SUCCESS
 @retval EFI_TIMEOUT

**/
EFI_STATUS
UsbDebugPortIN (
  IN     UINT8                  *Buffer,
  IN OUT UINTN                  *Length,
  IN     UINT8                  TokenPid,
  IN     UINT8                  Toggle,
  IN     EFI_USB_DBG_REG        *DebugPtr

)
{
  UINTN                 Index;
  UINTN                 Timeout;
  UINT8                 Toggle0;
  UINT8                 Toggle1;
  EFI_USB_DBG_SC        DebugSC;
  //
  // Set TOKEN_PID_CNT
  //
  *(UINT32*)&DebugPtr->Pid = (Toggle << 8) | TokenPid;
  //
  // Set WRITE_READ#=0 and GO_CNT=1
  //
  DebugSC = DebugPtr->Sc;
  DebugSC.DataLength = 0;
  DebugSC.Write = DEBUG_READ;
  DebugSC.Go = 1;
  DebugPtr->Sc = DebugSC;
  for(Index = 0, Toggle0 = 0, Timeout = PEI_TIMEOUT_TICKS * 2; Index < Timeout;) {

    if (DebugPtr->Sc.Done) {
      if (DebugPtr->Sc.Error && DebugPtr->Sc.Exception > 0) {
        return EFI_DEVICE_ERROR;
      }
      break;
    }
    Toggle1 = IoRead8(0x61);
    if (((Toggle0 ^ Toggle1) & 0x10) != 0) {
      Toggle0 = Toggle1;
      Index ++;
    }
  }
  if (Index == Timeout) {
    return EFI_TIMEOUT;
  }
  if (DebugPtr->Sc.DataLength > 8) {
    return EFI_ABORTED;
  }
  //
  // Fill the Data to Buffer
  //
  *Length = DebugPtr->Sc.DataLength;
  if (Buffer && DebugPtr->Sc.DataLength) {
    *(UINT32*)&Buffer[0] = DebugPtr->Data0;
    *(UINT32*)&Buffer[4] = DebugPtr->Data1;
  }
  return EFI_SUCCESS;
}

/**
 Control Transfer.

 @param [in]   Request
 @param [in]   DeviceAddress
 @param [in]   Buffer
 @param [in]   DebugPtr

 @retval EFI_SUCCESS
 @retval EFI_TIMEOUT

**/
EFI_STATUS
ControlTransfer (
  IN  EFI_USB_DEVICE_REQUEST    *Request,
  IN  UINT32                    DeviceAddress,
  IN  UINT8                     *Buffer,
  IN  EFI_USB_DBG_REG           *DebugPtr
)
{
  EFI_STATUS    Status;
  UINTN         DataLength = 0;

  DebugPtr->Addr.Address = DeviceAddress;
  DebugPtr->Addr.Endpoint = 0;
  //
  // Setup Stage
  //
  Status = UsbDebugPortOut ((UINT8*)Request, sizeof (EFI_USB_DEVICE_REQUEST), SETUP_TOKEN_ID, DATA0_PID, DebugPtr);
  if (Status != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  //
  // Data stage
  //
  if (Request->Length != 0) {
    Status = UsbDebugPortIN (Buffer, &DataLength, INPUT_TOKEN_ID, DATA1_PID, DebugPtr);
    if (Status != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
  }
  //
  // State stage
  //
  if ((Request->RequestType & 0x80) == 0) {
    //
    // This Control Transfer is Set, So its State stage is IN
    //
    Status = UsbDebugPortIN (Buffer, &DataLength, INPUT_TOKEN_ID, DATA1_PID, DebugPtr);
  }
  else {
    //
    // This Control Transfer is Get, So its State stage is OUT
    //
    Status = UsbDebugPortOut (Buffer, 0, OUTPUT_TOKEN_ID, DATA1_PID, DebugPtr);
    //
    // Return error if returned length is zero in data stage.
    //
    if (DataLength == 0) {
      return EFI_DEVICE_ERROR;
    }
  }
  if (Status != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  return   EFI_SUCCESS;
}

/**
 UsbIoWrite.

 @param [in, out] BufferSize
 @param [in]   Buffer

 @retval EFI_SUCCESS

**/
EFI_STATUS
DebugUsbWrite(
  IN OUT UINTN                  *BufferSize,
  IN VOID                       *Buffer
)
{
  EFI_STATUS       Status;
  INTN             StringLength;
  UINTN            NumBytesToSend;
  UINTN            Count0;
  UINT8            *Ptr;
  DEBUG_BULK       Bulk;
  EFI_USB_DBG_REG  *DebugPtr;
  UINT32           PciIndexSave;

  Status = EFI_SUCCESS;
  //
  // Save PCI index
  //
  PciIndexSave = IoRead32 (0xcf8);
  DebugPtr = GetDebugPointer ();
  //
  // Because Dxe Core will disable EHCI controller, so we should check it
  //
  if ((DebugPtr->Data0 == 0xFFFFFFFF) && (DebugPtr->Data1 == 0xFFFFFFFF)) {
    EnableEhci ();
  }
  //
  // Restore PCI index
  //
  IoWrite32 (0xcf8, PciIndexSave);
  //
  // If Usb Debug Port not been enable, don't send
  //
  if (DebugPtr->Sc.Enabled != 1) {
    return EFI_DEVICE_ERROR;
  }
  if ((DebugPtr->Sc.Error !=0) && (DebugPtr->Sc.Exception != 0)) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Load Variable to Data Buffer
  //
  *(UINT32*)&Bulk = DebugPtr->Data0;
  //
  // Calculation DataLength
  //
  Ptr = Buffer;
  StringLength = *BufferSize;
  //
  // Set Device and Endpoint Address
  //
  DebugPtr->Addr.Address = DEBUG_DEVICE_ADDR;
  for (Count0 = 0; Count0 < PEI_TIMEOUT_TICKS; Count0 ++) {
    DebugPtr->Addr.Endpoint = Bulk.EndpointOut;
    NumBytesToSend = (StringLength < 8) ? StringLength : 8;
    Status = UsbDebugPortOut (Ptr, NumBytesToSend, OUTPUT_TOKEN_ID, DataPID[Bulk.ToggleOut], DebugPtr);
    if (Status == EFI_SUCCESS) {
      Bulk.ToggleOut ^= 1;
      if (StringLength <= (INTN)NumBytesToSend) goto Finish;

      StringLength -= NumBytesToSend;
      Ptr += NumBytesToSend;
      Count0 = 0;
      DebugPtr->Addr.Endpoint = Bulk.EndpointIn;
    } else if (Status != EFI_TIMEOUT) {
      if ((DebugPtr->Sc.Error !=0) && (DebugPtr->Sc.Exception != 0)) {
        return EFI_DEVICE_ERROR;
      }
      MicroSecondDelay (1000);
    }
  }
  if (Count0 == PEI_TIMEOUT_TICKS) {
    Status = EFI_TIMEOUT;
  }
Finish:
  //
  // Store Variable to Data Buffer
  //
  DebugPtr->Data0 = *(UINT32*)&Bulk;
  return Status;
}

/**
 Prints a string to the USB port

 @param [in]   OutputString     Ascii string to print to serial port.

 @retval None

**/
VOID
EFIAPI
UsbDebugPrint (
  IN CHAR8    *OutputString
  )
{
  EFI_STATUS                    Status;
  UINTN                         StringLength;
  UINT8                         TempBuffer[8];

  Status       = EFI_SUCCESS;
  StringLength = 0;

  ZeroMem (TempBuffer, 8);

  for (; *OutputString != 0; OutputString++) {
    TempBuffer[StringLength] = *OutputString;
    StringLength++;
    if (StringLength == 8){
      DebugUsbWrite (&StringLength, TempBuffer);
      ZeroMem (TempBuffer, 8);
      MicroSecondDelay (1000);
      
      StringLength = 0;
    }
  }

  DebugUsbWrite (&StringLength, TempBuffer);

}
