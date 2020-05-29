/** @file
  Provide OEM to define the post key corresponding to the behavior of utility choosing (SCU or Boot Manager).

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PostKey.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>

//
// These must reflect order in KEY_ELEMENT
//
#define F1_KEY_BIT       0x01
#define F2_KEY_BIT       0x02
#define DEL_KEY_BIT      0x04
#define F10_KEY_BIT      0x08
#define F12_KEY_BIT      0x10
#define ESC_KEY_BIT      0x20
#define UP_ARROW_KEY_BIT 0x40
#define F3_KEY_BIT       0x80
#define F9_KEY_BIT       0x100
//
// module variables
//
SCAN_TO_OPERATION  *mPostKeyToOperation = NULL;
UINTN              mPostOperationCount = 0;

//
// helper function prototypes
//
VOID
InitialPostkeyTable (
  VOID
  );

/**
  Initialize module variable mPostKeyToOperation.

  @param                        None

  @retval                       None
**/
VOID
InitialPostkeyTable (
  VOID
  )
{
  UINTN         Index;
  UINTN         OldPostOperationCount;

  Index = 0;
  
  if (mPostKeyToOperation != NULL) {
    return;
  }
  //
  // Is frontpage supported? 
  //
  if (FeaturePcdGet(PcdFrontPageSupported)) {
    OldPostOperationCount = mPostOperationCount;
    mPostOperationCount++;
    mPostKeyToOperation = ReallocatePool (
                            OldPostOperationCount*sizeof(SCAN_TO_OPERATION), 
                            sizeof(SCAN_TO_OPERATION)*mPostOperationCount, 
                            mPostKeyToOperation
                            );
    
    mPostKeyToOperation[Index].KeyBit = ESC_KEY_BIT;
    mPostKeyToOperation[Index].ScanCode = SCAN_NULL;
    mPostKeyToOperation[Index].PostOperation = FRONT_PAGE_HOT_KEY;
    Index++;
  }
  //
  // Is ME_5MB_SUPPORT supported? 
  //
  if (FeaturePcdGet(PcdMe5MbSupported)) {
    OldPostOperationCount = mPostOperationCount;
    mPostOperationCount++;
    mPostKeyToOperation = ReallocatePool (
                            OldPostOperationCount*sizeof(SCAN_TO_OPERATION), 
                            sizeof(SCAN_TO_OPERATION)*mPostOperationCount, 
                            mPostKeyToOperation
                            );
    
    mPostKeyToOperation[Index].KeyBit = F9_KEY_BIT;
    mPostKeyToOperation[Index].ScanCode = SCAN_NULL;
    mPostKeyToOperation[Index].PostOperation = REMOTE_ASST_HOT_KEY;
    Index++;
  }
  if (FeaturePcdGet(PcdMe5MbSupported)) {
    OldPostOperationCount = mPostOperationCount;
    mPostOperationCount++;
    mPostKeyToOperation = ReallocatePool (
                            OldPostOperationCount*sizeof(SCAN_TO_OPERATION), 
                            sizeof(SCAN_TO_OPERATION)*mPostOperationCount, 
                            mPostKeyToOperation
                            );
    
    mPostKeyToOperation[Index].KeyBit = F10_KEY_BIT;
    mPostKeyToOperation[Index].ScanCode = SCAN_NULL;
    mPostKeyToOperation[Index].PostOperation = MEBX_HOT_KEY;
    Index++;
  }  

//[-start-140321-IB10300105-add]//
  if (FeaturePcdGet(PcdHotKeyFlashSupported)) {
    OldPostOperationCount = mPostOperationCount;
    mPostOperationCount++;
    mPostKeyToOperation = ReallocatePool (
                            OldPostOperationCount*sizeof(SCAN_TO_OPERATION), 
                            sizeof(SCAN_TO_OPERATION)*mPostOperationCount, 
                            mPostKeyToOperation
                            );
    
    mPostKeyToOperation[Index].KeyBit = F3_KEY_BIT;
    mPostKeyToOperation[Index].ScanCode = SCAN_NULL;
    mPostKeyToOperation[Index].PostOperation = FLASH_BOX_HOT_KEY;
    Index++;
  }  
//[-end-140321-IB10300105-add]//

  //
  // Add the end of table.
  //
  OldPostOperationCount = mPostOperationCount;
  mPostOperationCount++;
  mPostKeyToOperation = ReallocatePool (
                          OldPostOperationCount*sizeof(SCAN_TO_OPERATION), 
                          sizeof(SCAN_TO_OPERATION)*mPostOperationCount, 
                          mPostKeyToOperation
                          );
  mPostKeyToOperation[Index].KeyBit = 0;
  mPostKeyToOperation[Index].ScanCode = SCAN_NULL;
  mPostKeyToOperation[Index].PostOperation = NO_OPERATION;
  Index++;
}

/**
  This OemService provides OEM to define the post key corresponding to the behavior of utility choosing (SCU or Boot Manager). 

  @param[in]   KeyDetected           A bit map of the monitored keys found.
                                     Bit N corresponds to KeyList[N] as provided by the 
                                     GetUsbPlatformOptions () API of UsbLegacy protocol.
  @param[in]   ScanCode              The Scan Code.
  @param[out]  *PostOperation        Point to the operation flag which imply the behavior of utility choosing in post time. For example: choose the SCU or Boot Manager. 

  @retval      EFI_MEDIA_CHANGED     Get post operation success.
  @retval      EFI_SUCCESS           Get post operation failed.
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallPostKeyTable (
  IN  UINTN                          KeyDetected,
  IN  UINT16                         ScanCode,
  OUT UINTN                          *PostOperation
  )
{
  UINTN                                 Index;
  UINTN                                 PostOperationCount;
  SCAN_TO_OPERATION                     *PostKeyToOperation;

  InitialPostkeyTable ();
  PostOperationCount = mPostOperationCount;
  PostKeyToOperation = mPostKeyToOperation;

  for (Index = 0; Index < PostOperationCount; Index++) {
    //
    // Search table
    //
    if ((KeyDetected & PostKeyToOperation[Index].KeyBit) == PostKeyToOperation[Index].KeyBit &&
         ScanCode == PostKeyToOperation[Index].ScanCode){
      *PostOperation = (UINTN)PostKeyToOperation[Index].PostOperation;
      if (PostKeyToOperation[Index].PostOperation == NO_OPERATION) {
        break;
      }

      return EFI_MEDIA_CHANGED;
    } else if (KeyDetected == 0xffff && ScanCode == PostKeyToOperation[Index].ScanCode) {
      //
      // Pure EFI enviorment, no monitor key driver
      //
      *PostOperation = (UINTN)PostKeyToOperation[Index].PostOperation;
      if (PostKeyToOperation[Index].PostOperation == NO_OPERATION) {
        break;
      }

      return EFI_MEDIA_CHANGED;      
    }
  }

  return EFI_SUCCESS;
}
