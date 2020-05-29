/** @file
  When PEI begins to initialize clock generator, 
  this OemService sends the clock generator information to ProgClkGen driver. 
  According to the OEM specification, add the clock generator information to this service.

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

#include <Library/PeiOemSvcKernelLib.h>

//
// data type definitions
//
#define CK505                    0x06

#define CLOCKGEN_SMBUS_ADDR      (0xD2 >> 1)
#define CLOCKGEN_BLOCK_PROTOCOL  0x00 				// Use SMBus Block Read/Write
#define CLOCKGEN_DATA_BYTES      21

#define CLOCK_GEN_HIGH_BYTE      0x0F0
#define CLOCK_GEN_LOW_BYTE       0x00F

#define DATA_SIZE(a) (sizeof(a)/sizeof(CLOCK_GEN_DATA_INFO))
//
// Platform ID define (Set platform ID here)
//
typedef enum {
  NounPlatformId = 0xFF
} PLATFORM_ID_NUMBER;

//
// module variables
//

//
// Ck505 Internal Vga type data table
//

//
// Extern data table
//
// Structure Description:
// ChipVId        - set North bridge Vender Id
// PlatformId     - set platform Id
// VgaType        - set Vga type (VGA_UMA, VGA_DESCRIPTER and VGA_UNITED)
// InitType       - set initial type (PRI_INIT and INIT)
//
// If want to pass check item, parameter can set as below.
// CLOCK_GEN_EXTRA_INFO Ck505InternalVGAHeader = {
//   NO_VERFY_CHIP_VID, NounPlatformId, NO_VERFY_VGA, CLOCK_GEN_INIT
// };
//
CLOCK_GEN_EXTRA_INFO Ck505ExtraInfo = {
  I_CHIP_VID, NounPlatformId, NO_VERFY_VGA, CLOCK_GEN_INIT
};

//
// Pre-initial or Initial data table
//
// Structure Description:
// WriteType      - set CLKGEN write mode (CLKGEN_BLOCK_OPERATION and CLKGEN_BYTE_OPERATION and CLKGEN_FAST_BLOCK_OPERATION)
// Byte           - set CLKGEN offset
// AndSourceValue - clear data (If need to clear bit0, AndSourceValue set 0xFE)
// OrSourceValue  - set CLKGEN data
//
// CLKGEN_DATA_END is end of structure, CLKGEN_DATA_END can't delete.
// 
// If want to use fast block write, ¡§WriteType¡¨ parameter of first CLKGEN data need to set ¡§CLKGEN_FAST_BLOCK_OPERATION¡¨.
// Engine will get first CLKGEN table OrSourceValue to write CLKGEN data directly.
//
CLOCK_GEN_DATA_INFO Ck505DataInfo[] = {
//{                  WriteType, Byte, AndSourceValue, OrSourceValue}
//-------------------------------------------------------------
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x00,              0,         0x0A5},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x01,              0,         0x083},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x02,              0,         0x080},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x03,              0,         0x000},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x04,              0,         0x074},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x05,              0,         0x000},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x06,              0,         0x000},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x07,              0,         0x026},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x08,              0,         0x010},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x09,              0,         0X065},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x0A,              0,         0x060},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x0B,              0,         0x0F2},
//  {CLKGEN_FAST_BLOCK_OPERATION, 0x0C,              0,         0X00D},
  {            CLKGEN_DATA_END,    0,              0,             0}
};

//
// fast block write sample
//
// CLOCK_GEN_DATA_INFO Ck505InternalVGAData[] = {
// //{                  WriteType, Byte, AndSourceValue, OrSourceValue}
// //-------------------------------------------------------------
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x00,              0,         0x043},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x01,              0,         0x083},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x02,              0,         0x0FF},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x03,              0,         0x0FF},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x04,              0,         0x0FF},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x05,              0,         0x000},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x06,              0,         0x000},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x07,              0,         0x006},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x08,              0,         0X010},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x09,              0,         0X025},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x0A,              0,         0X000},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x0B,              0,         0X000},
//   {CLKGEN_FAST_BLOCK_OPERATION, 0x0C,              0,         0X00D},
//   {            CLKGEN_DATA_END,    0,              0,             0}
// };

//
// Union data table    - congregate data information table
//
// Structure Description:
// ExternInfo     - set CLOCK_GEN_EXTRA_INFO structure
// DataInfo       - set CLOCK_GEN_DATA_INFO structure
//
CLOCK_GEN_UNION_DATA_INFO gCk505UnionDataInfo = {
  &Ck505ExtraInfo,            //ExtraInfo
  Ck505DataInfo,              //DataInfo
}; 


//
// CLKGEN device Info    
//
// Structure Description:
// VenderId       - set Vender Id
// DeviceId       - set Device Id
// VenderIdReg    - Byte - set Vender Id byte offset 
//                (If Vender Id is byte7 at CLKGEN data, Byte need to set 0x07)
//                - Mask - set Vender Id bit 
//                (If Vender Id is bit0 ~ bit3, Mask need to set 0x0F)
// DeviceIdReg    - Byte - set Device Id byte offset(the same VenderIdReg)
//                - Mask - set Device Id bit(the same VenderIdReg)
//
// If no CLKGEN VenderId, parameter VenderId need to set NO_VERFY_CLKGEN_VID_DID and VenderIdReg need to set 0x0.
// If no CLKGEN DeviceId, parameter DeviceId need to set NO_VERFY_CLKGEN_VID_DID and DeviceIdReg need to set 0x0.
//
CLOCK_GEN_DEVICE_INFO Ck505 = {
  CK505,                          // VenderId
  0x00,                           // DeviceId
  {0x07, CLOCK_GEN_LOW_BYTE},     // VenderIdReg
  0x00                            // DeviceIdReg
};

//
// CLKGEN Big Table - congregate all table for CLKGEN engin to match
//
// Structure Description:
// UnionDataInfo     - set CLOCK_GEN_UNION_DATA_INFO structure
// DeviceInfo        - set CLOCK_GEN_DEVICE_INFO structure
//
//[-start-120924-IB06460446-modify]//
CLOCK_GEN_UNION_INFO ClkGenBigTable[] = {
//{              UnionDataInfo,  DeviceInfo}
//------------------------------------------------
//  {       &gCk505UnionDataInfo,      &Ck505}
  {    0,    0}
};
//[-end-120924-IB06460446-modify]//

/**
  When PEI begins to initialize clock generator, 
  this OemService sends the clock generator information to ProgClkGen driver. 
  According to the OEM specification, add the clock generator information to this service.
 
  @param[out]  *SlaveAddress         SlaveAddress of Clock generator.
  @param[out]  *StartOffset          The command of Smbus for this clock generator.
  @param[out]  *BigTable             Clock generator information table. The detail of this 
                                     table refers to the document "ClkGen Porting Guide_V0.7".
  @param[out]  *RealPlatformId       Real platform ID, if no platform ID that need to set 
                                     NounPlatformId (0xFF).
  @param[out]  *SizeInfo             Clock generator information table size.

  @retval      EFI_MEDIA_CHANGED     ChipsetModifyClockGenInfo run success.
**/
EFI_STATUS
OemSvcChipsetModifyClockGenInfo (
  OUT UINT8                                 *SlaveAddress,
  OUT UINT8                                 *StartOffset,
  OUT CLOCK_GEN_UNION_INFO                  **BigTable,
  OUT UINT16                                *RealPlatformId,
  OUT UINT16                                *SizeInfo
  )
{

//[-start-120924-IB06460446-add]//
  if (ClkGenBigTable[0].UnionDataInfo == 0) {
    return EFI_DEVICE_ERROR;  	
  } 
//[-end-120924-IB06460446-add]//

  *SlaveAddress = CLOCKGEN_SMBUS_ADDR;
  *StartOffset = CLOCKGEN_BLOCK_PROTOCOL;
  *BigTable = ClkGenBigTable;
  *RealPlatformId = NounPlatformId;
  *SizeInfo = sizeof(ClkGenBigTable)/sizeof(CLOCK_GEN_UNION_INFO);  
  
  return EFI_MEDIA_CHANGED;
}

