//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;   Boot Mode PPI as defined in EFI 2.0
//;

#ifndef _PEI_VarHob_PPI_H
#define _PEI_VarHob_PPI_H
        
#define PEI_VARHOB_PEIM_PPI \
  {0xc64fea0e, 0x86d8, 0x4d7c, 0x89, 0xe2, 0x29, 0x54, 0x46, 0xd, 0xa1, 0x90}

EFI_FORWARD_DECLARATION (PEI_VARHOB_PPI);

typedef struct _VARHOB_CONFIG_DATA {  
  BOOLEAN                 ResetRequired; 
  UINT32                  NumofThreading;  
} VARHOB_CONFIG_DATA;

extern EFI_GUID gPeiVarHobPpiGuid;

#endif
