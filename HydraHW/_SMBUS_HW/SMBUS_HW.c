/** @file

  Keyboard Controller
  
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include"SMBUS_HW.h"
#include"HugeHeader.h"

///
/// Even SPD Addresses only as we read Words
///
const UINT8
  SpdAddress[] = { 2, 8, 116, 118, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144 };

EFI_STATUS
SMBusFrameHandler (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL        *DxePlatformSaPolicy,
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx
  )
{
  EFI_STATUS                      Status;

  CHAR16                                 *BaseFrameTitle[] = {
                                           L"SM Bus List"
                                           };
  CHAR16                                 *BaseFrameTail[] = { 
                                            L" Enter : Select Item / Confirm Modify | Ctrl+F1 : Exit",
                                            L" Left / Right Button: Select Item     ",
                                            L" Up / Down Botton: Select Previous    "
                                            };
  CHAR16                                 *ItemLists[] = {
                                           L"Slot 0",
                                           L"Slot 1",
                                           L"Slot 2",
                                           L"Slot 3",
                                           };
  CHAR16                                 **ItemListsPtr;
  UINT8                                  SelectedItem;

  UINT8                           ChannelASlotMap;
  UINT8                           ChannelBSlotMap;
  UINT8                           BitIndex;
  UINT8                           ChannelASlotNum;
  UINT8                           ChannelBSlotNum;
  UINT16                          MaxSockets;
  UINT8                           Dimm;
  BOOLEAN                         SlotPresent;

  MEM_INFO_PROTOCOL               *MemInfoHob;
  EFI_GUID                        MemInfoProtocolGuid = MEM_INFO_PROTOCOL_GUID;
  
  EFI_SMBUS_HC_PROTOCOL           *SmbusController;
  UINT8                           *SmbusBuffer;
  UINTN                           SmbusBufferSize;
  
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  // UINTN                           SmbusOffset;
  UINTN                           SmbusLength;
  // UINT8                           i;
  
  BOOLEAN                         Populated;
  BOOLEAN                         DimmExist[4] = {FALSE};
  
  ItemListsPtr = ItemLists;
  SelectedItem = NO_SELECT;

  SmbusBuffer     = NULL;
  SmbusBufferSize = 0x100;
  SmbusBuffer     = AllocatePool (SmbusBufferSize);
  if (SmbusBuffer == NULL) {
    // (gBS->FreePool) (StringBuffer);
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = gBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID **) &SmbusController);
  if (EFI_ERROR (Status)) {
    Print (L"LocateProtocol gEfiSmbusHcProtocolGuid ERROR\n");
  }

  Status = gBS->LocateProtocol (&MemInfoProtocolGuid, NULL, (VOID **) &MemInfoHob);
  if (EFI_ERROR (Status)) {
    Print (L"LocateProtocol MemInfoProtocolGuid ERROR\n");
  }
    
  ///
  /// Get the Memory DIMM info from platform policy protocols
  ///
  ChannelASlotMap = DxePlatformSaPolicy->MemoryConfig->ChannelASlotMap;
  ChannelBSlotMap = DxePlatformSaPolicy->MemoryConfig->ChannelBSlotMap;
  ChannelASlotNum = 0;
  ChannelBSlotNum = 0;
  for (BitIndex = 0; BitIndex < 8; BitIndex++) {
    if ((ChannelASlotMap >> BitIndex) & BIT0) {
      ChannelASlotNum++;
    }

    if ((ChannelBSlotMap >> BitIndex) & BIT0) {
      ChannelBSlotNum++;
    }
  }
  MaxSockets  = ChannelASlotNum + ChannelBSlotNum;
  
  ///
  /// For each existed socket whether it is populated or not generate Type 17.
  /// Type 20 is optional for existed and populated socket.
  ///
  /// The Desktop and mobile only support 2 channels * 2 slots per channel = 4 sockets totally
  /// So there is rule here for Desktop and mobile that there are no more 4 DIMMS totally in a system:
  ///  Channel A/ Slot 0 --> SpdAddressTable[0] --> DimmToDevLocator[0] --> MemInfoHobProtocol.MemInfoData.dimmSize[0]
  ///  Channel A/ Slot 1 --> SpdAddressTable[1] --> DimmToDevLocator[1] --> MemInfoHobProtocol.MemInfoData.dimmSize[1]
  ///  Channel B/ Slot 0 --> SpdAddressTable[2] --> DimmToDevLocator[2] --> MemInfoHobProtocol.MemInfoData.dimmSize[2]
  ///  Channel B/ Slot 1 --> SpdAddressTable[3] --> DimmToDevLocator[3] --> MemInfoHobProtocol.MemInfoData.dimmSize[3]
  ///
  for (Dimm = 0; Dimm < SA_MC_MAX_SOCKETS; Dimm++) {
    ///
    /// Use channel slot map to check whether the Socket is supported in this SKU, some SKU only has 2 Sockets totally
    ///
    SlotPresent = FALSE;
    if (Dimm < 2) {
      if (ChannelASlotMap & (1 << Dimm)) {
        SlotPresent = TRUE;
      }
    } else {
      if (ChannelBSlotMap & (1 << (Dimm - 2))) {
        SlotPresent = TRUE;
      }
    }
    ///
    /// Don't create Type 17 and Type 20 items for non-existing socket
    ///
    if (!SlotPresent) {
      continue;
    }
    
    ///
    /// Only read the SPD data if the DIMM is populated in the slot.
    ///
    Populated = MemInfoHob->MemInfoData.DimmExist[Dimm];

    if (Populated) {
      DimmExist [Dimm] = TRUE;
    }
  }

  ///
  /// Generate Memory Device info (Type 17)
  ///
  ZeroMem (SmbusBuffer, SmbusBufferSize);
  //
  // Show Huge Frame
  //
  HugeFrame (
    BaseFrameTitle[0],
    BaseFrameTail[0],
    BaseFrameTail[1],
    BaseFrameTail[2]
    );
  while (TRUE) {
    
    // SetCursorPosColor ( EFI_WHITE | EFI_BACKGROUND_BLACK,54,5);
    // Print (L"ChannelASlotNum = %0d\n",ChannelASlotNum);
    // Print (L"Slot 0 = %s\n",DimmExist [0] == TRUE? L"Exist":L"UnExist");
    // Print (L"Slot 1 = %s\n",DimmExist [1] == TRUE? L"Exist":L"UnExist");
    // Print (L"ChannelBSlotNum = %0d\n",ChannelBSlotNum);
    // Print (L"Slot 2 = %s\n",DimmExist [2] == TRUE? L"Exist":L"UnExist");
    // Print (L"Slot 3 = %s\n",DimmExist [3] == TRUE? L"Exist":L"UnExist");
    
    SelectedItem = HugeRightSelectItems (
                     InputEx,
                     TOTAL_ITEMS,
                     ItemListsPtr
                     );
    if (SelectedItem == NO_SELECT) {
      break;
    }
    
    CleanFrame (
      EFI_BACKGROUND_BLACK,
      3,
      25,
      0,
      54
      );
    if (!DimmExist [SelectedItem]) {
      SetCursorPosColor ( EFI_WHITE | EFI_BACKGROUND_RED,0,3);
      Print (L"UnExist DIMM");
      SET_COLOR (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      continue;
    } else {
      SetCursorPosColor ( EFI_YELLOW | EFI_BACKGROUND_BLACK,0,3);
      Print (L"Exist DIMM !!");
      ///
      /// Read the SPD for this DIMM
      ///
      SmbusSlaveAddress.SmbusDeviceAddress = (DxePlatformSaPolicy->MemoryConfig->SpdAddressTable[SelectedItem]) >> 1;
      SmbusLength = 256;
      ///
      /// Read only needed values from SMBus or DimmsSpdData pointer to improve performance.
      ///
      Status = SmbusController->Execute (
                                  SmbusController,
                                  SmbusSlaveAddress,
                                  0,
                                  EfiSmbusReadByte,
                                  FALSE,
                                  &SmbusBufferSize,
                                  SmbusBuffer
                                  );
      ShowHugeDataFrame (
        TypeByte,
        SmbusBuffer
        );
    }
    
  }
  
  return EFI_SUCCESS;
}

/**
  Keyboard Controller Entry Point.

  @param[in] ImageHnadle        While the driver image loaded be the ImageLoader(), an image handle is assigned to this
                                driver binary, all activities of the driver is tied to this ImageHandle.
  @param[in] SystemTable        A pointer to the system table, for all BS(Boot Services) and RT(Runtime Services).
 
  @return Status Code

**/
EFI_STATUS
EFIAPI
H_BasePages (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                             Status;

  UINTN    TextModeNum;
  // UINTN    Columns;
  // UINTN    Rows;
  INT32    OringinalMode;
  // UINTN    OrgColumns;
  // UINTN    OrgRows;
  
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL      *InputEx = NULL;
  
  DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy;

  DxePlatformSaPolicy           = NULL;
  //
  // Check Console Mode, save orignal mode and Set specification Mode
  //
  OringinalMode = gST->ConOut->Mode->Mode;
  
  ChkTextModeNum (
    DEF_COLS,
    DEF_ROWS,
    &TextModeNum
    );
  
  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          TextModeNum
                          );
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }

  //
  // Locate Simple Text Input Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  (VOID **) &InputEx
                  );

  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiSimpleTextInputExProtocolGuid Fail : %2d\n",
      Status
      );
    
    return Status;
  }

  ///
  /// Get the platform setup policy.
  ///
  Status = gBS->LocateProtocol (
                  &gDxePlatformSaPolicyGuid,
                  NULL,
                  (VOID **) &DxePlatformSaPolicy
                  );
  if (EFI_ERROR (Status)) {
    Print (L"LocateProtocol gDxePlatformSaPolicyGuid ERROR\n");
  }
    
  //
  // Keyboard Controller Handler
  //
  SMBusFrameHandler (
    DxePlatformSaPolicy,
    InputEx
    );


  //
  // Reset to Orginal Mode
  //
  CLEAN_SCREEN (VOID); 

  Status = gST->ConOut->SetMode (
                          gST->ConOut,
                          OringinalMode
                          );
  
  if (EFI_ERROR (Status)) {
    Print (L"SetMode ERROR\n");
  }
  
  return EFI_SUCCESS;
}
