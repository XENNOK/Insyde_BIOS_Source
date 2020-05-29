/** @file
  Library for enumerate Flash Device Region Layout

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FLASH_REGION_LIB_H_
#define _FLASH_REGION_LIB_H_

#define    SHA256_DIGEST_SIZE          32
#define    FDM_ENTRY_REGION_ID_SIZE    16

#define ENTRY_HASH_SHA256               0

#pragma pack(push, 1)
typedef struct _H2O_FLASH_DEVICE_MAP_HEADER {
  UINT32 Signature;
  UINT32 Size;
  UINT32 Offset;
  UINT32 EntrySize;
  UINT8  EntryFormat;
  UINT8  Revision;
  UINT8  Reserved;
  UINT8  Checksum;
} H2O_FLASH_DEVICE_MAP_HEADER;

typedef struct _H2O_FLASH_DEVICE_MAP_ENTRY {
  EFI_GUID RegionType;
  UINT8    RegionId[16];
  UINT64   RegionOffset;
  UINT64   RegionSize;
  UINT32   Attribs;
//UINT8    Hash[¡K];
} H2O_FLASH_DEVICE_MAP_ENTRY;
#pragma pack(pop)

/**
  Get count of entries that FDM recorded 

  @param[out] Count             Unsigned integer that specifies the count of entries that FDM recorded.
  
  @retval EFI_SUCCESS           Get region count success.
  @retval EFI_NOT_FOUND         Can't find region type in the FDM.
  @retval EFI_UNSUPPORTED       Function called in runtime phase

**/
EFI_STATUS
EFIAPI
FdmGetCount (
  OUT  UINT8  *Count        
);


/**
  Get Region type record in FDM by specifying order in the list.

  @param[in]   RegionType            GUID that specifies the type of region that search for.
  @param[in]   Instance              Unsigned integer that specifies the N-th Region type instance
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the 
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry. 

                                
  @retval EFI_SUCCESS                find region type success.
  @retval EFI_INVALID_PARAMETER      Incorrect parameter.
  @retval EFI_SECURITY_VIOLATION     Region hash is not correct.    
  @retval EFI_NOT_FOUND              Can't find region type in the FDM.
  @retval EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet
  @retval EFI_UNSUPPORTED            Function called in runtime phase  

**/
EFI_STATUS
EFIAPI
FdmGetNAt (
  IN CONST EFI_GUID  *RegionType,  
  IN CONST UINT8     Instance,
  OUT UINT8          *RegionId,
  OUT UINT64         *RegionOffset,
  OUT UINT64         *RegionSize,
  OUT UINT32         *Attribs
  );
  
  
/**
  Get entry recorded in FDM

  @param[in]   RegionType            GUID that specifies the type of region that N-th entry is.
  @param[out]  Instance              Unsigned integer that specifies entry instance of FDM
  @param[out]  RegionId              Identifier that specifies the identifier of this region..
  @param[out]  RegionOffset          Unsigned integer that specifies the offset of this region relative to the 
                                     base of the flash device image.
  @param[out]  RegionSize            Unsigned integer that specifies the region size.
  @param[out]  Attribs               Bitmap that specifies the attributes of the flash device map entry. 

                                
  @retval EFI_SUCCESS                find region type success.
  @retval EFI_INVALID_PARAMETER      Incorrect parameter.
  @retval EFI_SECURITY_VIOLATION     Region hash is not correct.    
  @retval EFI_NOT_FOUND              Can't find region type in the FDM.
  @retval EFI_NOT_READY              EFI_HASH_PROTOCOL not installed yet  
  @retval EFI_UNSUPPORTED            Function called in runtime phase  

**/
EFI_STATUS
EFIAPI
FdmGetAt (
  IN CONST UINT8   Instance,
  OUT EFI_GUID     *RegionType,    
  OUT UINT8        *RegionId,
  OUT UINT64       *RegionOffset,
  OUT UINT64       *RegionSize,
  OUT UINT32       *Attribs
  );


#endif
