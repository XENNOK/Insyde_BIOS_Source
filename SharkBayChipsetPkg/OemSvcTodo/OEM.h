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

#ifndef _OEM_H_
#define _OEM_H_

#define IO_DECODE_BASS_ADDRESS_1  0
#define IO_DECODE_RANGE_ADDRESS_1 0
#define IO_DECODE_BASS_ADDRESS_2  0
#define IO_DECODE_RANGE_ADDRESS_2 0
#define IO_DECODE_BASS_ADDRESS_3  0
#define IO_DECODE_RANGE_ADDRESS_3 0
#define IO_DECODE_BASS_ADDRESS_4  0
#define IO_DECODE_RANGE_ADDRESS_4 0

//
// LPC I/F Enable Data
//
#define PCH_LPC_ENABLE_AND_DATA   0xFF88
#define PCH_LPC_ENABLE_OR_DATA    0x10
#define PCH_LPC_ENABLE_LIST       B_PCH_LPC_ENABLES_CNF2_EN   |\
                                  B_PCH_LPC_ENABLES_CNF1_EN   |\
                                  B_PCH_LPC_ENABLES_MC_EN     |\
                                  B_PCH_LPC_ENABLES_KBC_EN    |\
                                  B_PCH_LPC_ENABLES_GAMEH_EN  |\
                                  B_PCH_LPC_ENABLES_GAMEL_EN  |\
                                  B_PCH_LPC_ENABLES_COMB_EN   |\
                                  B_PCH_LPC_ENABLES_COMA_EN

#endif
