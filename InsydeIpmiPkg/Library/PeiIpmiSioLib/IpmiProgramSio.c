/** @file
 PEI IPMI Chipset library implement code - Program Super I/O.

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


#include <Library/PeiIpmiSioLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Base.h>
#include <PchRegs.h>
#include <PchAccess.h>
#include <PciAccess.h>


#define SIO_CONFIG_PORT1          0x2E
#define SIO_CONFIG_PORT2          0x4E
#define REG_LOGICAL_DEVICE        0x07
#define ACTIVATE                  0x30
#define BASE_ADDRESS_HIGH0        0x60
#define BASE_ADDRESS_LOW0         0x61
#define BASE_ADDRESS_HIGH1        0x62
#define BASE_ADDRESS_LOW1         0x63

//
// ====== SIO PILOT III ======
//
#define PILOT3_SIO_INDEX_PORT     SIO_CONFIG_PORT2
#define PILOT3_SIO_DATA_PORT      (PILOT3_SIO_INDEX_PORT+1)

#define PILOT3_SIO_UNLOCK         0x5A
#define PILOT3_SIO_LOCK           0xA5

#define PILOT3_SIO_KCS1           0x06
#define PILOT3_SIO_KCS2           0x07
#define PILOT3_SIO_KCS3           0x08
#define PILOT3_SIO_KCS4           0x09
#define PILOT3_SIO_KCS5           0x0A


#pragma pack(1)

typedef struct {
  UINT8 Register;
  UINT8 Value;
} SIO_CONFIG_TABLE;

#pragma pack()


/**
 Programming Super I/O configuration for BMC.
 Such as activate KCS interface and change KCS address by Super I/O interface.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.
 @param[in]         IpmiSmmBaseAddress  Value from FixedPcdGet16 (PcdIpmiSmmBaseAddress).
 @param[in]         IpmiSmmRegOffset    Value from FixedPcdGet16 (PcdIpmiSmmRegOffset).
 @param[in]         IpmiPostBaseAddress Value from FixedPcdGet16 (PcdIpmiPostBaseAddress).
 @param[in]         IpmiPostRegOffset   Value from FixedPcdGet16 (PcdIpmiPostRegOffset).
 @param[in]         IpmiOsBaseAddress   Value from FixedPcdGet16 (PcdIpmiOsBaseAddress).
 @param[in]         IpmiOsRegOffset     Value from FixedPcdGet16 (PcdIpmiOsRegOffset).

 @retval EFI_SUCCESS                    Programming success.
 @retval EFI_UNSUPPORTED                Does not implement this function.
*/
EFI_STATUS
IpmiProgramSio (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       UINT16                       IpmiSmmBaseAddress,
  IN       UINT16                       IpmiSmmRegOffset,
  IN       UINT16                       IpmiPostBaseAddress,
  IN       UINT16                       IpmiPostRegOffset,
  IN       UINT16                       IpmiOsBaseAddress,
  IN       UINT16                       IpmiOsRegOffset
  )
{
  UINT8                                 Index;
  UINT8                                 ConfigPort;
  UINT8                                 IndexPort;
  UINT8                                 DataPort;
  SIO_CONFIG_TABLE                      SioConfigTable[] = {
                                          //
                                          // Active Logical Devices 9 (KCS4)
                                          //
                                          {
                                            REG_LOGICAL_DEVICE,
                                            PILOT3_SIO_KCS4
                                          },
                                          {
                                            BASE_ADDRESS_HIGH0,
                                            0
                                          },
                                          {
                                            BASE_ADDRESS_LOW0,
                                            0
                                          },
                                          {
                                            BASE_ADDRESS_HIGH1,
                                            0
                                          },
                                          {
                                            BASE_ADDRESS_LOW1,
                                            0
                                          },
                                          {
                                            ACTIVATE,
                                            0x01
                                          },
                                          {
                                            0xff,
                                            0xff
                                          }
                                        };

  SioConfigTable[1].Value = (UINT8)(IpmiPostBaseAddress >> 8);
  SioConfigTable[2].Value = (UINT8)(IpmiPostBaseAddress & 0xFF);
  SioConfigTable[3].Value = (UINT8)((IpmiPostBaseAddress + IpmiPostRegOffset) >> 8);
  SioConfigTable[4].Value = (UINT8)((IpmiPostBaseAddress + IpmiPostRegOffset) & 0xFF);

  ConfigPort = SIO_CONFIG_PORT2;
  IndexPort  = PILOT3_SIO_INDEX_PORT;
  DataPort   = PILOT3_SIO_DATA_PORT;

  //
  // Program and Enable SIO Base Addresses 0x2E and 0x4E
  //
  PchLpcPciCfg16Or (R_PCH_LPC_ENABLES, B_PCH_LPC_ENABLES_CNF1_EN | B_PCH_LPC_ENABLES_CNF2_EN);

  //
  // Enter Config Mode
  //
  IoWrite8 (ConfigPort, PILOT3_SIO_UNLOCK);
  IoWrite8 (ConfigPort, 0x00); // 0x00 be end

  //
  // Configure SIO
  //
  for (Index = 0; Index < sizeof (SioConfigTable) / sizeof (SIO_CONFIG_TABLE); Index++) {
    IoWrite8 (IndexPort, SioConfigTable[Index].Register);
    IoWrite8 (DataPort, SioConfigTable[Index].Value);
  }

  //
  // Exit Config Mode
  //
  IoWrite8 (ConfigPort, PILOT3_SIO_LOCK);
  IoWrite8 (ConfigPort, 0x00); // 0x00 be end

  return EFI_SUCCESS;

}

