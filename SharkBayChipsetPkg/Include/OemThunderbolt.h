//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_THUNDERBOLT_H_
#define _OEM_THUNDERBOLT_H_

//
// Thunderbolt definition
//
#define THUNDERBOLT_PCIE_ROOT_PORT_BUS_NUM     0x00
#define THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM     0x1C
#define THUNDERBOLT_PCIE_LOCATION_1            0
#define THUNDERBOLT_PCIE_LOCATION_2            4

#define CACTUS_RIDGE                           0
#define REDWOOD_RIDGE                          1

//
// Security Level definition for CACTUS_RIDGE
//
typedef enum {
  NormalModeWithNHI,
  NormalModeWithoutNHI,
  SignleDepth,
  RedriverOnly,
  TbtOff,
  TbtDebug
} TBT_CR_SECURITY_LEVEL;

//
// Security Level definition for REDWOOD_RIDGE
//
typedef enum {
  NoSecurity,
  UserAuthorization,
  DisplayPortOnly = 3,
} TBT_RR_SECURITY_LEVEL;

//
// Thunderbolt Configuration for OEM project
//
//#define THUNDERBOLT_CHIP_TYPE                  REDWOOD_RIDGE ==> PcdGet8 ( PcdThunderBoltChip )
#define THUNDERBOLT_PCIE_ROOT_PORT_FUN_NUM     THUNDERBOLT_PCIE_LOCATION_2

#endif
