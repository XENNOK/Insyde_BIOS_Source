/** @file
  Provides an opportunity for OEM to define the SMBIOS policy.

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

#include <Library/DxeOemSvcKernelLib.h>

/*++

Todo:
  Define the relateaed data.

MISC_SUBCLASS              MiscSubclassDefault[] = {
//{ RecordNumber, MiscSubclassEnables, Instance }

};

EFI_DATAHUB_RECORD_POLICY        DataHubRecordPolicy = {
//CACHE_SMBIOS_TYPE          CacheEnables
  {
  },

//ProcessorEnables
  {
  },

//MemoryEnables

  {
  },

//MiscSubclass
  {MiscSubclassDefault}
};

--*/

/**
  Provide OEM to define the SMBIOS policy. The MiscSubClass table in SMBIOS is constructed by the instance of SMBIOS Type.
  This policy decides which instance of Type (RecordNumber) will be added into the table.

  @param[out]  *MiscSubclassDefaultSize  Pointer to the data size of *mDataHubRecordPolicy
  @param[out]  *mDataHubRecordPolicy     Pointer to EFI_DATAHUB_RECORD_POLICY

  @retval      EFI_UNSUPPORTED           Returns unsupported by default.
  @retval      EFI_SUCCESS               Get SMBIOS policy success.
  @retval      EFI_MEDIA_CHANGED         The value of IN OUT parameter is changed. 
  @retval      Others                    The error status bases on OEM design.
**/
EFI_STATUS
OemSvcInstallDmiSwitchTable (
  OUT UINTN                          *MiscSubclassDefaultSize,
  OUT EFI_DATAHUB_RECORD_POLICY      **mDataHubRecordPolicy
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
