#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>

#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
// #include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>

#define PCI_AUDIO_DEV_SIGNATURE SIGNATURE_32 ('p', 'a', 'd', 'o')

typedef struct _PCI_AUDIO_PROTOCOL PCI_AUDIO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *NO_FUNCTION)(
  IN PCI_AUDIO_PROTOCOL *This
  );

typedef struct _PCI_AUDIO_PROTOCOL{
  NO_FUNCTION    *NoFunction;
} PCI_AUDIO_PROTOCOL;

///
/// Device instance.
///
typedef struct {
  UINT64                           Signature;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  EFI_UNICODE_STRING_TABLE         *ControllerNameTable;
  PCI_AUDIO_PROTOCOL               PciAudioProtocol;

} PCI_AUDIO_DEV;

#define PCI_AUDIO_DEV_FROM_AUDIO_PROTOCOL(a) \
    CR(a, PCI_AUDIO_DEV, PciAudioProtocol, PCI_AUDIO_DEV_SIGNATURE)

extern EFI_DRIVER_BINDING_PROTOCOL   gPciAudioDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL   gPciAudioComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL  gPciAudioComponentName2;

extern EFI_GUID                      gEfiPciAudioProtocolGuid;

EFI_STATUS
EFIAPI
PciUtilityUnload (
  IN EFI_HANDLE ImageHandle
  );

EFI_STATUS
EFIAPI
PciAudioDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
PciAudioDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
PciAudioDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI
PciAudioComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

EFI_STATUS
EFIAPI
PciAudioComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );