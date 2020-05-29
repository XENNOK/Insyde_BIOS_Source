/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DE_CONTROL_H_
#define _DE_CONTROL_H_


EFI_STATUS
FreeControlList (
  IN     H2O_CONTROL_LIST                    *ControlList
  );

EFI_STATUS
FreePanel (
  IN     H2O_PANEL_INFO                      *Panel
  );

VOID
CopyPanel(
  IN   H2O_PANEL_INFO                        *DestinationPanel,
  IN   H2O_PANEL_INFO                        *SourcePanel  
);

VOID
InitControlList (
 IN     H2O_CONTROL_LIST                    *ControlList
 );

#endif
