/** @file
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

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

#include <MultiSkuInfoCommonDef.h>
#include <Ppi/VerbTable.h>
#include <Library/PeiOemSvcKernelLib.h>

//
// data type definitions
//

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sample :                                                                                                      //
//                                                                                                               //
//   #define OEM_VERB_TABLE_ID_(n)         n                                                                     //
//                                                                                                               //
//   #define OEM_VERB_TABLE_(n)_HEADER(a)  0x10EC0880, // Vendor ID / Device ID                                  //
//                                         0x00000000, // SubSystem ID                                           //
//                                         0x02,       // RevisionId                                             //
//                                         0x01,       // Front panel support ( 1 = Yes, 2 = No )                //
//                                         0x000A,     // Number of Rear Jacks = 10                              //
//                                         0x0002      // Number of Front Jacks = 2                              //
//                                                                                                               //
//   #define OEM_VERB_TABLE_(n)_DATA(a)    0x01171CF0,0x01171D11,0x01171E11,0x01171F41, // NID(0x11): 0x411111F0 //
//                                         0x01271C40,0x01271D09,0x01271EA3,0x01271F99, // NID(0x12): 0x99A30940 //
//                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To define Verb Table ID.
//
#define OEM_VERB_TABLE_ID_1          1

//[-start-120308-IB05330328-modify]//
//
// Verb Table for Shark Bay
//
#define OEM_VERB_TABLE_1_HEADER1     0x10EC0262, \
                                     0x00000000, \
                                     0xFF,       \
                                     0x01,       \
                                     0x0010,     \
                                     0x0000

#define OEM_VERB_TABLE_1_DATA1       0x00172000,0x00172100,0x00172200,0x00172300, \
                                     0x01171CF0,0x01171D11,0x01171E11,0x01171F41, \
                                     0x01271C40,0x01271D09,0x01271EA3,0x01271F99, \
                                     0x01471CF0,0x01471D11,0x01471E11,0x01471F41, \
                                     0x01571C10,0x01571D40,0x01571E21,0x01571F04, \
                                     0x01671CF0,0x01671D11,0x01671E11,0x01671F41, \
                                     0x01871CF0,0x01871D11,0x01871E11,0x01871F41, \
                                     0x01971C30,0x01971D98,0x01971EA1,0x01971F04, \
                                     0x01A71C3F,0x01A71D30,0x01A71E81,0x01A71F04, \
                                     0x01B71CF0,0x01B71D11,0x01B71E11,0x01B71F41, \
                                     0x01C71CF0,0x01C71D11,0x01C71E11,0x01C71F41, \
                                     0x01D71CF0,0x01D71D01,0x01D71E83,0x01D71F59, \
                                     0x01E71C20,0x01E71D01,0x01E71E43,0x01E71F99, \
                                     0x01F71CF0,0x01F71D11,0x01F71E11,0x01F71F41, \
                                     0x02471C11,0x02471D60,0x02471E01,0x02471F01, \
                                     0x02971C14,0x02971D20,0x02971E01,0x02971F01
//[-end-120308-IB05330328-modify]//

//
// To include verb table 1 with 2 codec header/data
// To congregate the verb table header/data.
//
#ifdef OEM_VERB_TABLE_ID_1
//[-start-120308-IB05330328-modify]//
DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(OEM_VERB_TABLE_ID_1);
COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(OEM_VERB_TABLE_ID_1);
//[-end-120308-IB05330328-modify]//
#endif

/**
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

  @param  *VerbTableHeaderDataAddress    A pointer to VerbTable data/header

  @retval EFI_MEDIA_CHANGED              Get verb table data/header success.
  @retval Others                         Base on OEM design.
**/
EFI_STATUS
OemSvcGetVerbTable (
  OUT COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress
  )
{
  *VerbTableHeaderDataAddress = VERB_TABLE_HEADER_DATA_BUFFER_ADDRESS(OEM_VERB_TABLE_ID_1);
  
  return EFI_MEDIA_CHANGED;
}
