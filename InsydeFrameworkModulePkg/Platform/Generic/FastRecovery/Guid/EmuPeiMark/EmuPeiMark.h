//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/**
 
	GUID to identify Emu Pei phase has executed.

**/

#ifndef __EMU_PEI_MARK_GUID__
#define __EMU_PEI_MARK_GUID__

#define EMU_PEI_MARK_GUID \
  { 0xD56FF7F0, 0x8358, 0x4352, {0xBC, 0x87, 0xE8, 0x1B, 0xD2, 0x52, 0x45, 0x72}  }

extern EFI_GUID gEmuPeiMarkGuid;

                                             
#endif
