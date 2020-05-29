/** @file
 H2O IPMI Interface DXE/SMM module common implement code.

 This c file provides Interface DXE/SMM module common code.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <IpmiInterfaceCommon.h>


//
// Module variable
//
H2O_IPMI_INTERFACE_ADDRESS  mAllBaseAddressTable[H2O_IPMI_MAX_BASE_ADDRESS_NUM];


/**
 Output lowest level rad data for debugging.

 @param[in]        Request              TRUE for send request data to BMC;
                                        FALSE for receive response data from BMC;
 @param[in]        Header               Request/Response common header.
                                        and various IPMI command, to send to BMC.
 @param[in]        Data                 Data buffer that contains request/response data.
 @param[in]        DataSize             Data buffer size.
 @param[out]       Status               Status of read/write transaction.
 @param[out]       CompletionCode       Completion code that BMC returned.
                                        Ignore when Request is set to TRUE.

*/
VOID
SystemInterfaceDebug (
  IN  BOOLEAN                  Request,
  IN  H2O_IPMI_CMD_HEADER      *Header,
  IN  VOID                     *Data,
  IN  UINT8                    DataSize,
  IN  EFI_STATUS               Status,
  IN  UINT8                    CompletionCode
  )
{
  UINTN  Index;
  UINT8  *Buffer;

  if (Request) {
    DEBUG ((DEBUG_VERBOSE, "  Request (%02x, %02x)\n", *(UINT8*)Header, Header->Cmd));
  } else {
    DEBUG ((DEBUG_VERBOSE, "  Response (%02x, %02x)\n", *(UINT8*)Header, Header->Cmd));
  }
  DEBUG ((DEBUG_VERBOSE, "    Status          : %r\n", Status));
  DEBUG ((DEBUG_VERBOSE, "    Network Function: %02x\n", Header->NetFn));
  DEBUG ((DEBUG_VERBOSE, "    LUN             : %02x\n", Header->Lun));
  DEBUG ((DEBUG_VERBOSE, "    Command         : %02x\n", Header->Cmd));
  if (!Request) {
    DEBUG ((DEBUG_VERBOSE, "    Completion Code : %02x\n", CompletionCode));
  }
  DEBUG ((DEBUG_VERBOSE, "    Data Size       : %02x\n", DataSize));
  DEBUG ((DEBUG_VERBOSE, "    Data            : "));

  if (Data != NULL) {
    Buffer = (UINT8*)Data;
    for (Index = 0; Index < DataSize; ++Index) {
      if ((Index & 0x0F) == 0 && Index != 0x00) {
        DEBUG ((DEBUG_VERBOSE, "\n                      "));
      }
      DEBUG ((DEBUG_VERBOSE, "%02x ", Buffer[Index]));
    }
  }

  DEBUG ((DEBUG_VERBOSE, "\n"));
}


/**
 Empty function for internal use.

 @param[in]        This                 A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]        Request              H2O_IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]        SendData             Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]        SendLength           When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]       RecvData             Data buffer to put the data read from BMC.
 @param[out]       RecvLength           Length of Data readed from BMC.
 @param[out]       StatusCodes          The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
                                        For KCS system interface:
                                        StatusCodes is valid when return value is EFI_ABORTED. If the return
                                        value is EFI_DEVICE_ERROR,it does not guarantee StatusCodes is valid,
                                        the caller must check bit 15.
                                        For SMIC system interface: It is always valid.
                                        For BT system interface: It is no function.

 @retval EFI_UNSUPPORTED                Always return EFI_UNSUPPORTED since this is an empty function.
*/
EFI_STATUS
EFIAPI
EmptyExecuteIpmiCmd (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  IN  H2O_IPMI_CMD_HEADER               Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINT8                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}


/**
 Return system interface address that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "H2O_IPMI_OS_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_SMM_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_POST_BASE_ADDRESS_INDEX"

 @retval The base address that BMC current use.
*/
UINT16
EFIAPI
GetBaseAddress (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  IN  UINT8                             Index
  )
{

  if (Index >= H2O_IPMI_MAX_BASE_ADDRESS_NUM) {
    return mAllBaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr;
  }

  return mAllBaseAddressTable[Index].Addr;

}


/**
 Return system interface offset that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "H2O_IPMI_OS_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_SMM_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_POST_BASE_ADDRESS_INDEX"

 @retval The base address offset that BMC current use.
*/
UINT16
EFIAPI
GetBaseAddressOffset (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  IN  UINT8                             Index
  )
{

  if (Index >= H2O_IPMI_MAX_BASE_ADDRESS_NUM) {
    return mAllBaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Offset;
  }

  return mAllBaseAddressTable[Index].Offset;

}


/**
 Return base address type that BMC currently use.

 @param[in]         This                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval H2O_IPMI_MEMORY                The base address is in memory space.
 @retval H2O_IPMI_IO                    The base address is in I/O space.
*/
H2O_IPMI_INTERFACE_ADDRESS_TYPE
EFIAPI
GetBaseAddressType (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  )
{
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->IpmiAddressType;
}


/**
 Return IPMI version that BMC current supports.

 @param[in]         This                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval IPMI version. Upper four bits are msb, lower four bits are lsb. For example, if
         IPMI version is 2.0, this function return 0x20h.
*/
UINT8
EFIAPI
GetIpmiVersion (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  )
{
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->IpmiVersion;

}


/**
 Return BMC firmware version.

 @param[in]         This                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval Major revision is in high byte, minor revision is in low byte.
*/
UINT16
EFIAPI
GetBmcFirmwareVersion (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  )
{
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return 0;
  }

  IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->BmcFirmwareVersion;

}


/**
 Return BMC status.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.

 @retval The enum H2O_IPMI_BMC_STATUS of BMC current status.
*/
H2O_IPMI_BMC_STATUS
EFIAPI
GetBmcStatus (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  )
{
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;

  if (This == NULL) {
    return BMC_STATUS_UNKNOWN;
  }

  IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (This);

  return IpmiData->BmcStatus;

}


/**
 Fill private data content

 @param[in]         IpmiHob             A pointer to H2O IPMI Hob
 @param[in]         InSmm               In SMM or not. It is possible to use different base address in SMM.
 @param[out]        IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
*/
VOID
InitialIpmiInterfaceContent (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  BOOLEAN                           InSmm,
  OUT H2O_IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT8                                 Index;

  if (InSmm) {
    Index = H2O_IPMI_SMM_BASE_ADDRESS_INDEX;
  } else {
    Index = H2O_IPMI_POST_BASE_ADDRESS_INDEX;
  }

  //
  // Initial Context Structure
  //
  IpmiData->Signature = H2O_IPMI_PRIVATE_DATA_SIGNATURE;
  IpmiData->H2OIpmiProtocol.GetIpmiBaseAddress = GetBaseAddress;
  IpmiData->H2OIpmiProtocol.GetIpmiBaseAddressOffset = GetBaseAddressOffset;
  IpmiData->H2OIpmiProtocol.GetIpmiBaseAddressType = GetBaseAddressType;
  IpmiData->H2OIpmiProtocol.GetIpmiVersion = GetIpmiVersion;
  IpmiData->H2OIpmiProtocol.GetBmcFirmwareVersion = GetBmcFirmwareVersion;
  IpmiData->H2OIpmiProtocol.GetBmcStatus = GetBmcStatus;
  IpmiData->BmcFirmwareVersion = IpmiHob->BmcFirmwareVersion;
  IpmiData->IpmiVersion = IpmiHob->IpmiVersion;
  IpmiData->BmcStatus = IpmiHob->BmcStatus;
  IpmiData->ProtocolHandle = NULL;
  IpmiData->IpmiBaseAddress = IpmiHob->InterfaceAddress[Index].Addr;
  IpmiData->IpmiBaseAddressOffset = IpmiHob->InterfaceAddress[Index].Offset;
  IpmiData->IpmiAddressType = IpmiHob->InterfaceAddressType;
  IpmiData->Sequence = 0;
  IpmiData->TimerAddress = FixedPcdGet16 (PcdIpmiAcpiTimerAddress);
  IpmiData->TimerMaxValue = FixedPcdGet32 (PcdIpmiAcpiTimerMaxValue);
  //
  // Link to correct ExecuteIpmiCmd function based on system interface type
  //
  switch (FixedPcdGet8 (PcdIpmiInterfaceType)) {
  case H2O_IPMI_KCS:
    InitialKcsSystemInterface (IpmiData);
    break;
  case H2O_IPMI_SMIC:
    InitialSmicSystemInterface (IpmiData);
    break;
  case H2O_IPMI_BT:
    InitialBtSystemInterface (IpmiData);
    break;
  default:
    InitialKcsSystemInterface (IpmiData);
    break;
  }
  //
  // If BMC is not ready or something else, use empty function since we cannot
  // do anything.
  //
  if (H2O_IPMI_VERSION_DEFAULT == IpmiHob->IpmiVersion) {
    IpmiData->H2OIpmiProtocol.ExecuteIpmiCmd = EmptyExecuteIpmiCmd;
  }

}

