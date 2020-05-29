//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _QUICK_LAUNCH_H_
#define _QUICK_LAUNCH_H_

#include EFI_PROTOCOL_CONSUMER (CpuIo)
#include "LinkedList.h"

#define EFI_Q2L_SERVICES_PROTOCOL_GUID \
  { \
    0x6c86c78, 0xcea3, 0x4d67, 0xad, 0x5d, 0x67, 0x84, 0x5a, 0x74, 0xc, 0xb0 \
  }

EFI_FORWARD_DECLARATION (EFI_Q2L_SERVICES_PROTOCOL);


typedef
VOID
(EFIAPI *EFI_AP_PROCEDURE) (
  IN  VOID                                     *Buffer
  );


typedef
EFI_STATUS
(EFIAPI *EFI_Q2L_SERVICE_STARTUP_AP) (
  IN  EFI_Q2L_SERVICES_PROTOCOL                *This,
  IN  EFI_AP_PROCEDURE                         Procedure,
  IN  UINTN                                    ProcessorNumber,
  IN  EFI_EVENT                                WaitEvent OPTIONAL,
  IN  UINTN                                    TimeoutInMicroSecs OPTIONAL,
  IN  OUT VOID                                 *ProcArguments OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_Q2L_SERVICE_SHUTDOWN_AP) (
  IN EFI_Q2L_SERVICES_PROTOCOL                 *This
  );

typedef
BOOLEAN
(EFIAPI *EFI_Q2L_SERVICE_QUERY_SHUTDOWN) (
  IN EFI_Q2L_SERVICES_PROTOCOL                 *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_Q2L_SERVICE_SELECTION) (
  IN  EFI_Q2L_SERVICES_PROTOCOL                *This,
  OUT UINT16                                   *ButtonKeyID
  );



typedef struct _EFI_Q2L_SERVICES_PROTOCOL {
  EFI_Q2L_SERVICE_STARTUP_AP     Q2LServiceStartupAP;
  EFI_Q2L_SERVICE_SHUTDOWN_AP    Q2LServiceShutdownAP;
  EFI_Q2L_SERVICE_QUERY_SHUTDOWN Q2LServiceQueryShutdown;
  EFI_Q2L_SERVICE_SELECTION      Q2LServiceSelection;
} EFI_Q2L_SERVICES_PROTOCOL;

extern EFI_GUID gEfiQ2LServiceProtocolGuid;

#endif
