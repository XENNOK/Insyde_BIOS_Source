//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    EmuPeiGate.h

Abstract:

  Fast recovery helper DXE driver. 
  Jump back to recovery only emulate PEI phase. Or do preparation for BDS's
  recovery process.

--*/

#ifndef __EMU_PEI_GATE__
#define __EMU_PEI_GATE__

EFI_STATUS 
GoLegacyModePei (
  IN    FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData
  );
    
EFI_STATUS 
UpdateHeaderChecksum (
  IN EFI_FFS_FILE_HEADER  *FileHeader
);


#endif
