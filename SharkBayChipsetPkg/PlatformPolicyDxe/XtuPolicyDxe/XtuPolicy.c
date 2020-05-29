//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   XtuPolicy.c
//;

#include <XtuPolicy.h>

EFI_STATUS
XTUPolicyEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     Handle;
  DXE_XTU_POLICY_PROTOCOL        *XTUPlatformPolicy;

  Handle            = NULL;
  XTUPlatformPolicy = NULL;

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\nXTU Policy Entry\n" ) );

  //
  // Allocate and set XTU Policy structure to known value
  //
  XTUPlatformPolicy = NULL;
  XTUPlatformPolicy = AllocateZeroPool ( sizeof ( DXE_XTU_POLICY_PROTOCOL ) );
  ASSERT ( XTUPlatformPolicy != NULL );

  if ( XTUPlatformPolicy == NULL ) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialzie XTU Configuration
  //
  XTUPlatformPolicy->EnableRealtimeDevicesSptt                  = PcdGetBool ( PcdXTUPlatformPolicyEnableRealtimeDevicesSptt );
  XTUPlatformPolicy->EanbleRealtimeWdttAcpiTable                = PcdGetBool ( PcdXTUPlatformPolicyEanbleRealtimeWdttAcpiTable );
  XTUPlatformPolicy->EnableSwSmiRealtimeCommunicationsInterface = PcdGetBool ( PcdXTUPlatformPolicyEnableSwSmiRealtimeCommunicationsInterface );

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Install XTU Policy Protocol\n" ) );
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateDxeXtuPolicy (XTUPlatformPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateDxeXtuPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install XTU Policy Protocol.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gDxePlatformXtuPolicyGuid,
               EFI_NATIVE_INTERFACE,
               XTUPlatformPolicy
               );
    ASSERT_EFI_ERROR ( Status );
  }
//[-end-121220-IB10820206-modify]//
  //
  // Dump policy
  //
  DumpXTUPolicy ( XTUPlatformPolicy );

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "XTU Policy Exit\n" ) );

  return Status;
}

VOID
DumpXTUPolicy (
  IN      DXE_XTU_POLICY_PROTOCOL        *XTUPlatformPolicy
  )
{
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "XTUPlatformPolicy ( Address : 0x%x )\n", XTUPlatformPolicy ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-EnableRealtimeDevicesSptt                  : %x\n", XTUPlatformPolicy->EnableRealtimeDevicesSptt ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-EanbleRealtimeWdttAcpiTable                : %x\n", XTUPlatformPolicy->EanbleRealtimeWdttAcpiTable ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-EnableSwSmiRealtimeCommunicationsInterface : %x\n", XTUPlatformPolicy->EnableSwSmiRealtimeCommunicationsInterface ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );

  return;
}
