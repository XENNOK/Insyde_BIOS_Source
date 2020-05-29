/** @file
 PEI IPMI Package library implement code - Wait BMC ready.

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


#include <Library/PeiIpmiPackageLib.h>


/**
 This package function can wait for BMC ready via platform way.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiPpi             A pointer to H2O_IPMI_INTERFACE_PPI structure.

 @retval EFI_SUCCESS                    Wait for BMC ready success.
 @return EFI_ERROR (Status)             Implement code execute status.
*/
EFI_STATUS
IpmiWaitBmc (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       H2O_IPMI_INTERFACE_PPI       *IpmiPpi
  )
{
  //
  // Platform can implement this function to wait for BMC ready.
  // BMC may set a GPIO or response command when it is ready.
  // Example 1 is wait BMC via GPIO.
  // Example 2 is wait BMC via sending command.
  //
  // This function only has two return values:
  // EFI_UNSUPPORTED: Platform does not implement this function.
  // EFI_SUCCESS: Platform implement this function to wait BMC.
  //

  return EFI_UNSUPPORTED;

#if 0
  //
  // Example 1: Wait BMC via GPIO
  //
  UINT32  Data;
  UINT8   Round = 0;
  UINT32  StartTick;
  UINT32  EndTick;
  UINTN   OverflowPoint;
  UINTN   DelayTimeTicks = 0;
  UINTN   TotalTimeTicks = BMC_WARMUP_TIME * 100000 * TICK_OF_TEN_MS;

  //
  // Enable GPIO at early time
  //
  IoWrite32 (0xcf8, PCH_GPIO_BASE_ADDRESS_REG);
  IoWrite32 (0xcfc, PCH_GPIO_BASE_ADDRESS);
  IoWrite32 (0xcf8, PCH_GPIO_CONTROL_REG);
  IoWrite32 (0xcfc, PCH_GPIO_ENABLE);

  //
  // Program BMC_RDY_N bit
  //
  Data = IoRead32 (PCH_GPIO_BASE_ADDRESS + GPIO_USE_SEL);
  Data |= (1 << BMC_RDY_BIT);
  IoWrite32 (PCH_GPIO_BASE_ADDRESS + GPIO_USE_SEL, Data);

  //
  // Enable pin
  //
  Data = IoRead32 (PCH_GPIO_BASE_ADDRESS + GP_IO_SEL);
  Data |= (1 << BMC_RDY_BIT);
  IoWrite32 (PCH_GPIO_BASE_ADDRESS + GP_IO_SEL, Data);

  //
  // Start polling BMC_RDY bit
  //
  StartTick = IoRead32 (H2O_IPMI_ACPI_TIMER_ADDRESS);
  OverflowPoint = StartTick;

  while (TotalTimeTicks > DelayTimeTicks) {
    Data = IoRead32 (PCH_GPIO_BASE_ADDRESS + GP_LVL);

    if ((Data >> BMC_RDY_BIT) == 0) {
      return;
    }
    
    EndTick = IoRead32 (H2O_IPMI_ACPI_TIMER_ADDRESS);

    if (EndTick <= OverflowPoint) {
      ++Round;
      OverflowPoint = EndTick;
    }

    DelayTimeTicks = EndTick + H2O_IPMI_ACPI_TIMER_MAX_VALUE * Round - StartTick;

  }

  return EFI_SUCCESS;


  //
  // Example 2: Wait BMC via sendind command
  //
  EFI_STATUS             Status;
  UINT8                  RecvSize;
  H2O_IPMI_BMC_INFO      BmcInfo
  H2O_IPMI_CMD_HEADER    Request = {H2O_IPMI_BMC_LUN,
                                    H2O_IPMI_NETFN_APPLICATION,
                                    H2O_IPMI_CMD_GET_DEVICE_ID
                                    };

  while (TRUE) {
    Status = Ipmi->ExecuteIpmiCmd (
                     Ipmi,
                     Request,
                     NULL,
                     0,
                     &BmcInfo,
                     &RecvSize,
                     NULL
                     );

    if (EFI_ERROR (Status)) {
      TotalTime += PEI_H2O_IPMI_EXECUTE_TIMEOUT;

      if (TotalTime >= BMC_WARMUP_TIME) {
          break;
      }

    } else {
      break;
    }
  }

  return EFI_SUCCESS;
#endif

}

