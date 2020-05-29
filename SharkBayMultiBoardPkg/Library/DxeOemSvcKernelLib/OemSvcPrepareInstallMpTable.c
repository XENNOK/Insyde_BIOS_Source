/** @file
  Returns platform specific MP Table information to the caller.

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

#include <Library/DxeOemSvcKernelLib.h>

//
// module variables
//
CHAR8 mOemIdString[8] = {
  'I','n','s','y','d','e',' ',' '
};

//
// Please modify the ProductIdString according to the platform you are porting.
//
CHAR8 mProductIdString[12] = {
  'S','h','a','r','k','B','a','y',' ',' ',' ',' '
};

EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC mIoApicDefault[] = {
  //
  // IO APIC of ICH first, then IOH, ordering is important!
  //
  // {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IOAPIC, IO APIC ID, IO APIC Version, {Enable, Reserved}, Address of IO APIC}
  // Please Modify this table according to your platform.
  //
  // If there is no any device, just use a NULL_ENTRY.
  //    
//  NULL_ENTRY
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IOAPIC, 0x02, 0x20, {1, 0}, 0xFEC00000},  
};

EFI_LEGACY_MP_TABLE_ENTRY_IO_INT mIoApicIntLegacy[] = {
  //
  // IRQ 0~15 for Legacy Bus, don't assign the Bus number.
  // The Bus number will be auto-assigned.
  //
  // {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, INT type, {Polarity, Trigger, 0}, Bus Number, {INT#, Dev, 0}, IO ApicId, Apic INT#}    
  // Please modify the IO ApicId number.
  //
  // If there is no any device, just use a NULL_ENTRY.
  //  
//  NULL_ENTRY  
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {0, 0x0, 0}  , 0x02, 0x0},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {1, 0x0, 0}  , 0x02, 0x1},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {1, 1, 0}, 0, {0, 0x0, 0}  , 0x02, 0x2}, 
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {3, 0x0, 0}  , 0x02, 0x3},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {0, 0x1, 0}  , 0x02, 0x4},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {1, 0x1, 0}  , 0x02, 0x5},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {2, 0x1, 0}  , 0x02, 0x6},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {3, 0x1, 0}  , 0x02, 0x7},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {0, 0x2, 0}  , 0x02, 0x8},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {1, 0x2, 0}  , 0x02, 0x9},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {2, 0x2, 0}  , 0x02, 0xA},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {3, 0x2, 0}  , 0x02, 0xB},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {0, 0x3, 0}  , 0x02, 0xC},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {1, 0x3, 0}  , 0x02, 0xD},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {2, 0x3, 0}  , 0x02, 0xE},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {0, 0, 0}, 0, {3, 0x3, 0}  , 0x02, 0xF}
};

EFI_LEGACY_MP_TABLE_ENTRY_IO_INT mIoApicIntDefault[] = {
  //
  // IRQ for device on host bus (bus 0)
  //
  // {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, INT type, {Polarity, Trigger, 0}, Bus Number, {INT#, Dev, 0}, IO Apic ID, Apic INT#}
  // Please fill in the Dev, IO Apic ID, Apic INT#.
  // Dev, Apic INT# are modified according to ASL code. 
  // IO Apic ID is modified according to the corresponding Apic INT#, and refer to mIoApicIntLegacy table to see what IO Apic ID is the Apic INT# connected to.
  //
  // If there is no any device, just use a NULL_ENTRY.
  //  
//  NULL_ENTRY

  //
  // AR00
  //
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x14, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x14, 0}  , 0x02, 0x11},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x14, 0}  , 0x02, 0x12},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x14, 0}  , 0x02, 0x13},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x15, 0}  , 0x02, 0x14},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x15, 0}  , 0x02, 0x15},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x16, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x16, 0}  , 0x02, 0x11},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x16, 0}  , 0x02, 0x12},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x16, 0}  , 0x02, 0x13},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x17, 0}  , 0x02, 0x16},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x19, 0}  , 0x02, 0x14},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x1a, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x1a, 0}  , 0x02, 0x15},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x1a, 0}  , 0x02, 0x12},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x1a, 0}  , 0x02, 0x13},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x1b, 0}  , 0x02, 0x16},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x1c, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x1c, 0}  , 0x02, 0x11},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x1c, 0}  , 0x02, 0x12},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x1c, 0}  , 0x02, 0x13},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x1d, 0}  , 0x02, 0x17},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x1d, 0}  , 0x02, 0x13},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x1d, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x1d, 0}  , 0x02, 0x12},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x1f, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x1f, 0}  , 0x02, 0x13},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x1f, 0}  , 0x02, 0x12},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x1f, 0}  , 0x02, 0x10},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x01, 0}  , 0x02, 0x10},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x01, 0}  , 0x02, 0x11},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x01, 0}  , 0x02, 0x12},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x01, 0}  , 0x02, 0x13},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x02, 0}  , 0x02, 0x10},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x03, 0}  , 0x02, 0x10},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x04, 0}  , 0x02, 0x10},

  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x18, 0}  , 0x02, 0x14}
};

MP_TABLE_ENTRY_IO_INT_SLOT mIoApicIntSlotDefault[] = {
  //
  // The Bus ID will be auto-assigned specified by bridge.
  //
  // {Bridge Bus#, Bridge Dev#, Bridge Func#,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, INT type, {Polarity, Trigger, 0}, Bus ID, {INT#, Dev#, 0}, IO Apic ID, Apic INT#}}
  //  Please modify bridge Bus/Dev/Func number which the device is connect to.
  // Dev, Apic INT# are modified according to ASL code.
  // IO Apic ID is modified according to the corresponding Apic INT#, and refer to mIoApicIntLegacy table to see what IO Apic ID is the Apic INT# connected to.
  //
  // If there is no any device, just use a NULL_ENTRY.
  //
//  NULL_ENTRY
  //
  // AR01
  //
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x15}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x16}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x17}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x14}},

  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x01, 0}  , 0x02, 0x16}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x01, 0}  , 0x02, 0x15}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x01, 0}  , 0x02, 0x14}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x01, 0}  , 0x02, 0x17}},

  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x02, 0}  , 0x02, 0x17}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x02, 0}  , 0x02, 0x14}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x02, 0}  , 0x02, 0x15}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x02, 0}  , 0x02, 0x16}},

  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x03, 0}  , 0x02, 0x13}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x03, 0}  , 0x02, 0x12}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x03, 0}  , 0x02, 0x15}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x03, 0}  , 0x02, 0x16}},

  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x04, 0}  , 0x02, 0x12}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x04, 0}  , 0x02, 0x17}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x04, 0}  , 0x02, 0x10}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x04, 0}  , 0x02, 0x14}},  

  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x05, 0}  , 0x02, 0x12}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x05, 0}  , 0x02, 0x14}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x05, 0}  , 0x02, 0x16}},
  {0x0, 0x1e, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x05, 0}  , 0x02, 0x15}},

  //
  // AR04
  //
  {0x0, 0x1c, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1c, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1c, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1c, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x13}},

  //
  // AR05
  //
  {0x0, 0x1c, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1c, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1c, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1c, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x10}},

  //
  // AR06
  //
  {0x0, 0x1c, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1c, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1c, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1c, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x11}},

  //
  // AR07
  //
  {0x0, 0x1c, 0x3,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1c, 0x3,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1c, 0x3,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1c, 0x3,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x12}},

  //
  // AR08
  //
  {0x0, 0x1c, 0x4,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1c, 0x4,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1c, 0x4,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1c, 0x4,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x13}},

  //
  // AR09
  //
  {0x0, 0x1c, 0x5,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1c, 0x5,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1c, 0x5,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1c, 0x5,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x10}},

  //
  // AR0E
  //
  {0x0, 0x1c, 0x6,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1c, 0x6,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1c, 0x6,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1c, 0x6,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x11}},

  //
  // AR0E
  //
  {0x0, 0x1c, 0x7,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1c, 0x7,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1c, 0x7,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1c, 0x7,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x12}},

  //
  // AR02
  //
  {0x0, 0x1, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1, 0x0,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x13}},

  //
  // AR0A
  //
  {0x0, 0x1, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x11}},
  {0x0, 0x1, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1, 0x1,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x10}},

  //
  // AR0B
  //
  {0x0, 0x1, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {0, 0x00, 0}  , 0x02, 0x12}},
  {0x0, 0x1, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {1, 0x00, 0}  , 0x02, 0x13}},
  {0x0, 0x1, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {2, 0x00, 0}  , 0x02, 0x10}},
  {0x0, 0x1, 0x2,{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_IO_INT, 0, {3, 3, 0}, 0, {3, 0x00, 0}  , 0x02, 0x11}}
};

EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT mLocalApicIntDefault[] = {
  //{EFI_LEGACY_MP_TABLE_ENTRY_TYPE_LOCAL_INT, INT type, {Polarity, Trigger, Reserved}, Src Bus ID, Src Bus Irq, Local Apic ID, Local Apic INT#}
  //
  // If there is no any device, just use a NULL_ENTRY.
  //
//  NULL_ENTRY
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_LOCAL_INT, 3, {0, 0, 0}, 0, {0}, 0xFF, 0},
  {EFI_LEGACY_MP_TABLE_ENTRY_TYPE_LOCAL_INT, 1, {0, 0, 0}, 0, {0}, 0xFF, 1}
};

/**
  Returns platform specific MP Table information to the caller.

  @param[out]  *CharNumOfOemIdString      The characters number of OEM ID string.
  @param[out]  **pOemIdStringEntry        The address of mOemIdString[].
  
  @param[out]  *CharNumOfProductIdString  The characters number of Product ID string
  @param[out]  **pProductIdStringEntry    The address of mProductIdString[].
  
  @param[out]  *NumOfIoApic               Total entry number of mIoApicDefault[].
  @param[out]  **pIoApicEntry             The address of mIoApicDefault[].
  
  @param[out]  *NumOfIoApicIntLegacy      Total entry number of mIoApicIntLegacy[].
  @param[out]  **pIoApicIntLegacyEntry    The address of mIoApicIntLegacy[].

  @param[out]  *NumOfIoApicInt            Total entry number of mIoApicIntDefault[].
  @param[out]  **pIoApicIntEntry          The address of mIoApicIntDefault[].

  @param[out]  *NumOfIoApicIntSlot        Total entry number of mIoApicIntSlotDefault[].
  @param[out]  **pIoApicIntSlotEntry      The address of mIoApicIntSlotDefault[].

  @param[out]  *NumOfLocalApicInt         Total entry number of mLocalApicIntDefault[].
  @param[out]  **pLocalApicIntEntry       The address of mLocalApicIntDefault[].
  
  @retval      EFI_MEDIA_CHANGED          MP Table is supported.
  @retval      Others                     Depends on customization.
**/
EFI_STATUS 
OemSvcPrepareInstallMpTable (
  OUT UINTN                               *CharNumOfOemIdString,  
  OUT CHAR8                               **pOemIdStringEntry,
  OUT UINTN                               *CharNumOfProductIdString,  
  OUT CHAR8                               **pProductIdStringEntry,
  OUT UINTN                               *NumOfIoApic,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IOAPIC    **pIoApicEntry,
  OUT UINTN                               *NumOfIoApicIntLegacy,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IO_INT    **pIoApicIntLegacyEntry,  
  OUT UINTN                               *NumOfIoApicInt,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_IO_INT    **pIoApicIntEntry,
  OUT UINTN                               *NumOfIoApicIntSlot,  
  OUT MP_TABLE_ENTRY_IO_INT_SLOT          **pIoApicIntSlotEntry,
  OUT UINTN                               *NumOfLocalApicInt,  
  OUT EFI_LEGACY_MP_TABLE_ENTRY_LOCAL_INT **pLocalApicIntEntry
  )
{
  (*CharNumOfOemIdString)     = sizeof(mOemIdString) / sizeof(mOemIdString[0]);
  (*pOemIdStringEntry)        = &mOemIdString[0];

  (*CharNumOfProductIdString) = sizeof(mProductIdString) / sizeof(mProductIdString[0]);
  (*pProductIdStringEntry)    = &mProductIdString[0];  

  if (mIoApicDefault[0].EntryType != NULL_ENTRY) {
    (*NumOfIoApic)              = sizeof(mIoApicDefault) / sizeof(mIoApicDefault[0]);
    (*pIoApicEntry)             = &mIoApicDefault[0];  
  }

  if (mIoApicIntLegacy[0].EntryType != NULL_ENTRY) {
    (*NumOfIoApicIntLegacy)     = sizeof(mIoApicIntLegacy) / sizeof(mIoApicIntLegacy[0]);
    (*pIoApicIntLegacyEntry)    = &mIoApicIntLegacy[0];
  }  
  if (mIoApicIntDefault[0].EntryType != NULL_ENTRY) {
    (*NumOfIoApicInt)           = sizeof(mIoApicIntDefault) / sizeof(mIoApicIntDefault[0]);
    (*pIoApicIntEntry)          = &mIoApicIntDefault[0];
  }
  if (mIoApicIntSlotDefault[0].BridgeBus != NULL_ENTRY) {
    (*NumOfIoApicIntSlot)       = sizeof(mIoApicIntSlotDefault) / sizeof(mIoApicIntSlotDefault[0]);
    (*pIoApicIntSlotEntry)      = &mIoApicIntSlotDefault[0];  
  }

  if (mLocalApicIntDefault[0].EntryType != NULL_ENTRY) {
    (*NumOfLocalApicInt)        = sizeof(mLocalApicIntDefault) / sizeof(mLocalApicIntDefault[0]);
    (*pLocalApicIntEntry)       = &mLocalApicIntDefault[0];
  }

  return EFI_MEDIA_CHANGED;
}      
