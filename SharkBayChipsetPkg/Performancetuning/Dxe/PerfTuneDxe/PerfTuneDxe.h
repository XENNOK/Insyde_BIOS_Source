#ifndef _PERTUNE_H_
#define _PERTUNE_H_

#include <CpuRegs.h>
#include <ChipsetSetupConfig.h>
#include <ChipsetCmos.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/BaseLib.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/XtuPolicy/XtuPolicy.h>

/**
  
  This routine will take a GUIDed file name and provide a buffer of the data.
  
  NOTE:
  Need to remove this if DXE Core creating correct Loaded Image Handle!
  Dependencies: 
  
  @param  NameGuid             Pointer to a GUID of the file name.
  @param  Buffer                    Pointer to a buffer for the file contents.
  @param  Size                      Size of the buffer, in bytes

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetFvImage (
  IN EFI_GUID   *NameGuid,
  IN OUT VOID   **Buffer,
  IN OUT UINTN  *Size
  );
#endif
