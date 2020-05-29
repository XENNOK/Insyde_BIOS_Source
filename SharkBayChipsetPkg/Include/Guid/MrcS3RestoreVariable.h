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

#ifndef _MRC_S3_RESTORE_VARIABLE_H_
#define _MRC_S3_RESTORE_VARIABLE_H_

#define MRC_S3_RESTORE_VARIABLE \
  { \
    0x14ef381c, 0x9721, 0x434e, 0xbe, 0x9, 0x19, 0x2a, 0xb9, 0x7e, 0x78, 0x1f \
  }
//
// {14EF381C-9721-434e-BE09-192AB97E781F}
//

extern EFI_GUID gMrcS3RestoreVariableGuid;

typedef struct {
  UINT32                   ScramblerSeed;
  UINT32                   ScramblerSeedS3;
} MRC_S3_RESTORE_DATA;

#endif
