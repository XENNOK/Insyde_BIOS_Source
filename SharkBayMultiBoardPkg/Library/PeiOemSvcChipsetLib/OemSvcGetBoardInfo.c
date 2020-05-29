/** @file
    This function offers an interface to get board information, ex. board id and fab id.

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

#include <Library/PeiOemSvcChipsetLib.h>
//[-start-130809-IB06720232-modify]//
//#include <Library/Eclib.h>
#include <Library/CommonEcLib.h>
#include <OemEc.h>
//[-end-130809-IB06720232-modify]//
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <PchAccess.h>
#include <OemBoardId.h>

//[-start-130809-IB06720232-add]//
EFI_STATUS
GetBoardId (
  OUT  UINT8        *BoardId,
  OUT  UINT8        *FabId
  );
//[-end-130809-IB06720232-add]//

//[-start-130424-IB05400400-add]//
/**
 Use GPIO to check if this is Haddock Creek CRB or not.

 @retval            TRUE                          This is Haddock Creek CRB.
 @retval            FALSE                         This is not Haddock Creek CRB.
 
*/
EFI_STATUS
CheckHaddockGpio (
  VOID
  )
{
  UINT32        GpioBase;
  UINT32        TempId;


  GpioBase = PchLpcPciCfg32 (R_PCH_LPC_GPIO_BASE) & B_PCH_LPC_GPIO_BASE_BAR;
  if (GpioBase == 0) {
    //
    // Set GPIO Base
    //
    PchLpcPciCfg32AndThenOr (R_PCH_LPC_GPIO_BASE, (UINT32)(~B_PCH_LPC_GPIO_BASE_BAR), PcdGet16 (PcdPchGpioBaseAddress));
    GpioBase = PcdGet16 (PcdPchGpioBaseAddress);
    //
    // Enable GPIO Base
    //
    PchLpcPciCfg8Or (R_PCH_LPC_GPIO_CNT, B_PCH_LPC_GPIO_CNT_GPIO_EN);
  }
  //
  // Program board id pins to GPIO
  //
  IoOr32 (GpioBase + R_PCH_GPIO_USE_SEL, BIT1 | BIT6 | BIT17);
  IoOr32 (GpioBase + R_PCH_GPIO_USE_SEL2, BIT2);
  //
  // Program board id pins to input
  //
  IoOr32 (GpioBase + R_PCH_GPIO_IO_SEL, BIT1 | BIT6 | BIT17);
  IoOr32 (GpioBase + R_PCH_GPIO_IO_SEL2, BIT2);
  //
  // Read it
  //
  TempId = IoBitFieldRead32 (GpioBase + R_PCH_GPIO_LVL, 6, 6) |
           (IoBitFieldRead32 (GpioBase + R_PCH_GPIO_LVL, 1, 1) << 1) |
           (IoBitFieldRead32 (GpioBase + R_PCH_GPIO_LVL2, 3, 3) << 2) |
           (IoBitFieldRead32 (GpioBase + R_PCH_GPIO_LVL, 17, 17) << 3);

  return ((TempId & 0x0F) == 0x08);

}
//[-end-130424-IB05400400-add]//



//[-start-130613-IB05400415-modify]//
/**
 This function offers an interface to get board id and fab id using hardware specified way.

 @param[out]        *BoardId                      Board Id get from current board.
 @param[out]        *FabId                        Fab Id get from current board.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcGetBoardInfo (
  OUT  UINT8   *BoardId,
  OUT  UINT8   *FabId
  )
{

  UINT16      LpcEnables;
  EFI_STATUS  Status;
  UINT8       TempBoardId;
  UINT8       TempFabId;



  //
  // Enable EC decode for these boards
  //
  LpcEnables = PchLpcPciCfg16 (R_PCH_LPC_ENABLES);
  if ((LpcEnables & B_PCH_LPC_ENABLES_MC_EN) != B_PCH_LPC_ENABLES_MC_EN) {
    PchLpcPciCfg16Or (R_PCH_LPC_ENABLES, LpcEnables | B_PCH_LPC_ENABLES_MC_EN);
  }

  //
  // Get board id & fab id flow
  //
  TempBoardId = *BoardId;
  TempFabId = *FabId;
  if (FeaturePcdGet (PcdDenlowServerSupported)) {
    //
    // Force Denlow Server support, assign Denlow Server board id directly
    //
    TempBoardId = V_CRB_BOARD_ID_AGATE_BEACH_CRB;
  } else {
    //
    // Get board id from EC for CRBs with EC
    //
    Status = GetBoardId (&TempBoardId, &TempFabId);
    if (!EFI_ERROR (Status)) {
      //
      // Treat these as same board id
      //
      switch (TempBoardId) {
      case V_EC_BOARD_ID_HARRIS_BEACH:
      case V_EC_BOARD_ID_HARRIS_BEACH_SKU2:
      case V_EC_BOARD_ID_HARRIS_BEACH_SKU3:
      case V_EC_BOARD_ID_HARRIS_BEACH_SKU4:
        TempBoardId = V_EC_BOARD_ID_HARRIS_BEACH;
        break;
      }
    } 
  }

//[-start-130424-IB05400400-add]//
  //
  // although boiard id is V_EC_BOARD_ID_FLATHEAD_CREEK_EV, it is possible this
  // is haddock creek because hardware/EC issue. Read GPIO to make sure which board it is.
  //
  if (TempBoardId == V_EC_BOARD_ID_FLATHEAD_CREEK_EV) {
    if (CheckHaddockGpio ()) {
      TempBoardId = V_EC_BOARD_ID_HADDOCK_CREEK;
    }
  }
//[-end-130424-IB05400400-add]//

  *BoardId = TempBoardId;
  *FabId = TempFabId;;
  
  return EFI_MEDIA_CHANGED;
}  
//[-end-130613-IB05400415-modify]//

//[-start-130809-IB06720232-add]//
/**
 Get board id and fab id from EC.

 @param[out]        BoardId             Board Id get from EC.
 @param[out]        FabId               Fab Id get from EC.

 @retval EFI_SUCCESS                    Get board id and fab id success.
 @retval !EFI_SUCCESS                   Get board id and fab id fail.
*/
EFI_STATUS
GetBoardId (
  OUT  UINT8        *BoardId,
  OUT  UINT8        *FabId
  )
{
  EFI_STATUS    Status;
  UINT16        TempBoardId;
  UINT8         BoardIdByteNum;
  UINT8         Temp;


  //
  // Get board id is two bytes or one byte
  //
  Status = WaitKbcIbe (SMC_CMD_STATE);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = WriteKbc (SMC_CMD_STATE, EC_READ_BORAD_ID_BYTE_CMD);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WaitKbcObf (SMC_CMD_STATE);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = ReadKbc (SMC_DATA, &BoardIdByteNum);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get board id
  //
  Status = WaitKbcIbe (SMC_CMD_STATE);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = WriteKbc (SMC_CMD_STATE, EC_READ_BORAD_ID_CMD);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = WaitKbcObf (SMC_CMD_STATE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = ReadKbc (SMC_DATA, &Temp);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  TempBoardId = Temp;
  
  if ((BoardIdByteNum & B_EC_BOARD_ID_TWO_BYTE) == B_EC_BOARD_ID_TWO_BYTE) {
    //
    // Two byte board id, need to read another
    //
    Status = WaitKbcObf (SMC_CMD_STATE);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = ReadKbc (SMC_DATA, &Temp);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    TempBoardId = (TempBoardId << 8) | Temp;
    *BoardId = (UINT8)(TempBoardId & B_EC_BOARD_ID_8);
    *FabId = (UINT8)((TempBoardId >> 9) & B_EC_FAB_ID);
  } else {
    //
    // One byte board id
    //
    *BoardId = (UINT8)(TempBoardId & B_EC_BOARD_ID_5);
    *FabId = (UINT8)((TempBoardId >> 5) & B_EC_FAB_ID);
  }

 return Status;
}
//[-end-130809-IB06720232-add]//

