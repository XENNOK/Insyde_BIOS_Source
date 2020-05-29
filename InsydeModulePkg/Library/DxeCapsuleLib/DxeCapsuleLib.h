/** @file
  Function definition for DxeCapsuleLib Library Instance

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

#ifndef _DXE_CAPSULE_LIB_H_
#define _DXE_CAPSULE_LIB_H_

/**
  Capsule persistence across reset

  @param[in] CapsuleHeader       Pointer to the Capsule header of the Capsule to
                                 be persisted, the Capsule header must be followed
                                 by Capsule image data

  @retval EFI_SUCCESS            The Capsule is successfully saved
  @return others                 Failed to persist the Capsule image

**/
EFI_STATUS
PersistCapsuleImage (
  EFI_CAPSULE_HEADER  *CapsuleHeader
  );


#endif
