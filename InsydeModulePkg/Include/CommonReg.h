/** @file

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

#ifndef _COMMON_REG_H_
#define _COMMON_REG_H_

//
// Bit Definitions
//

#ifndef BIT0
#define BIT0          ( 1 << 0  )
#endif
#ifndef BIT1
#define BIT1          ( 1 << 1  )
#endif
#ifndef BIT2
#define BIT2          ( 1 << 2  )
#endif
#ifndef BIT3
#define BIT3          ( 1 << 3  )
#endif
#ifndef BIT4
#define BIT4          ( 1 << 4  )
#endif
#ifndef BIT5
#define BIT5          ( 1 << 5  )
#endif
#ifndef BIT6
#define BIT6          ( 1 << 6  )
#endif
#ifndef BIT7
#define BIT7          ( 1 << 7  )
#endif
#ifndef BIT8
#define BIT8          ( 1 << 8  )
#endif
#ifndef BIT9
#define BIT9          ( 1 << 9  )
#endif
#ifndef BIT10
#define BIT10         ( 1 << 10 )
#endif
#ifndef BIT11
#define BIT11         ( 1 << 11 )
#endif
#ifndef BIT12
#define BIT12         ( 1 << 12 )
#endif
#ifndef BIT13
#define BIT13         ( 1 << 13 )
#endif
#ifndef BIT14
#define BIT14         ( 1 << 14 )
#endif
#ifndef BIT15
#define BIT15         ( 1 << 15 )
#endif
#ifndef BIT16
#define BIT16         ( 1 << 16 )
#endif
#ifndef BIT17
#define BIT17         ( 1 << 17 )
#endif
#ifndef BIT18
#define BIT18         ( 1 << 18 )
#endif
#ifndef BIT19
#define BIT19         ( 1 << 19 )
#endif
#ifndef BIT20
#define BIT20         ( 1 << 20 )
#endif
#ifndef BIT21
#define BIT21         ( 1 << 21 )
#endif
#ifndef BIT22
#define BIT22         ( 1 << 22 )
#endif
#ifndef BIT23
#define BIT23         ( 1 << 23 )
#endif
#ifndef BIT24
#define BIT24         ( 1 << 24 )
#endif
#ifndef BIT25
#define BIT25         ( 1 << 25 )
#endif
#ifndef BIT26
#define BIT26         ( 1 << 26 )
#endif
#ifndef BIT27
#define BIT27         ( 1 << 27 )
#endif
#ifndef BIT28
#define BIT28         ( 1 << 28 )
#endif
#ifndef BIT29
#define BIT29         ( 1 << 29 )
#endif
#ifndef BIT30
#define BIT30         ( 1 << 30 )
#endif
#ifndef BIT31
#define BIT31         ( 1 << 31 )
#endif

#define ZERO          0

// COMMON Configuration Space Define

#define R_VENDOR_ID                                 0x00
#define R_DEVICE_ID                                 0x02

#define R_PCI_CFG_COMMAND                           0x04
#define   B_IO_ACCESS_ENABLE                          BIT0
#define     V_IO_ACCESS_ENABLE                        BIT0
#define     V_IO_ACCESS_DISABLE                       0
#define   B_MEMORY_ACCESS_ENABLE                      BIT1
#define     V_MEMORY_ACCESS_ENABLE                    BIT1
#define     V_MEMORY_ACCESS_DISABLE                   0
#define   B_BUS_MASTER_ENABLE                         BIT2
#define     V_BUS_MASTER_ENABLE                       BIT2
#define     V_BUS_MASTER_DISABLE                      0
#define   B_SPECIAL_CYCLE_RECOGNITION_ENABLE          BIT3
#define     V_SPECIAL_CYCLE_RECOGNITION_ENABLE        BIT3
#define     V_SPECIAL_CYCLE_RECOGNITION_DISABLE       0
#define   B_MEMORY_WRITE_AND_INVALIDATE_ENABLE        BIT4
#define     V_MEMORY_WRITE_AND_INVALIDATE_ENABLE      BIT4
#define     V_MEMORY_WRITE_AND_INVALIDATE_DISABLE     0
#define   B_VGA_PALETTE_SNOOP_ENABLE                  BIT5
#define     V_VGA_PALETTE_SNOOP_ENABLE                BIT5
#define     V_VGA_PALETTE_SNOOP_DISABLE               0
#define   B_PERR_DETECTION_ENABLE                     BIT6
#define     V_PERR_DETECTION_ENABLE                   BIT6
#define     V_PERR_DETECTION_DISABLE                  0
#define   B_WAIT_CYCLE_ENABLE                         BIT7
#define     V_WAIT_CYCLE_ENABLE                       BIT7
#define     V_WAIT_CYCLE_DISABLE                      0
#define   B_SERR_ENABLE                               BIT8
#define     V_SERR_ENABLE                             BIT8
#define     V_SERR_DISABLE                            0
#define   B_FAST_BACK_TO_BACK_ENABLE                  BIT9
#define     V_FAST_BACK_TO_BACK_ENABLE                BIT9
#define     V_FAST_BACK_TO_BACK_DISABLE               0
#define   B_INTERRUPT_DISABLE                         BIT10
#define     V_INTERRUPT_DISABLE                       BIT10
#define     V_INTERRUPT_ENABLE                        0

#define R_REVISION_ID                               0x08
#define R_PROGRAMABLE_INTERFACE                     0x09
#define R_CLASS_CODE                                0x0A
#define R_BASE_CODE                                 0x0B
#define R_CACHE_LINE                                0x0C
#define R_LATENCY_TIME                              0x0D
#define R_MULTIFUNC_CHECK                           0x0E

  //
  // Type 0
  //
#define R_BAR0                                      0x10
#define R_BAR1                                      0x14
#define R_BAR2                                      0x18
#define R_BAR3                                      0x1C
#define R_BAR4                                      0x20
#define R_BAR5                                      0x24
  //
  // Type 1
  //
#define R_PRIMARY_BUS_NUM                           0x18
#define R_SECONDARY_BUS_NUM                         0x19
#define R_SUB_BUS_NUM                               0x1A
#define R_SECONDARY_LATENCY_TIMER                   0x1B
#define R_IO_BASE                                   0x1C
#define R_IO_LIMIT                                  0x1D
#define R_SECONDARY_STATUS                          0x1E
#define R_MEM_BASE                                  0x20
#define R_MEM_LIMIT                                 0x22
#define R_PMEM_BASE                                 0x24
#define R_PMEM_LIMIT                                0x26
#define R_PMEM_BASE_UPPER                           0x28
#define R_PMEM_LIMIT_UPPER                          0x2C
#define R_IO_BASE_HI                                0x30
#define R_IO_LIMIT_HI                               0x32
#define R_IRQ_BRIDGE_CNTL                           0x3E


#define R_SUBSYSTEM_VENDOR_ID                       0x2C
#define R_SUBSYSTEM_DEVICE_ID                       0x2E

#define R_CAPABILITY_POINTER                        0x34

#define R_INT_LINE                                  0x3C
#define R_INT_PIN                                   0x3D

#endif
