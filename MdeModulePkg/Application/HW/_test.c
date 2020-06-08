#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EfiShellParameters.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/MebxProtocol.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>

#include <MeSetup.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE  ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS          Status;
  ME_SETUP            mMeSetup;

  Status = gRT->GetVariable (
                  L"MeSetup",
                  &gMeSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mMeSetup
                  );
  

  gST->ConOut->OutputString(gST->ConOut, "s");

  Status = EFI_SUCCESS;
  return Status;
}