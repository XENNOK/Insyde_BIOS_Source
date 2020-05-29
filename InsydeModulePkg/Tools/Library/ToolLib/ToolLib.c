/** @file
  Support routines for tool modules

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include <stdio.h>
#include <stdlib.h>
#ifdef __GNUC__
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <Uefi.h>
#include <Library/BaseLib.h>

#define MAX_AUTO_GEN_FILE_SIZE 65536

CHAR8   mAutoGenBuf[MAX_AUTO_GEN_FILE_SIZE];
BOOLEAN mAutoGenBufValid = FALSE;
UINTN   mBufIndex = 0;


VOID 
GetLineFromBuffer (
  CHAR8 *Line, 
  CHAR8 *Buffer,
  UINTN BufSize
  )
{
  UINTN Index;
  Index = 0;
  while (mBufIndex < BufSize) {
    if (Buffer[mBufIndex] == '\n') {
      mBufIndex++;
      break;
    }
    Line[Index++] = Buffer[mBufIndex++];
  }
  Line[Index] = 0;
}  

UINTN
Evaluate (
  CHAR8* ValueStr,
  BOOLEAN HexMode
  )
{
  UINTN Index;
  
  for (Index = 0; Index < AsciiStrLen(ValueStr); Index++) {
    if (!(ValueStr[Index] >= '0' && ValueStr[Index] <= '9')) {
      if (HexMode) {
        if ((ValueStr[Index]|0x20) >= 'a' && (ValueStr[Index]|0x20) <='z') {
          continue;
        }
      }    
      //                                  
      // Replace type modifier with spaces
      //
      ValueStr[Index] = ' ';
    }
  }
  //
  // Skip spaces
  //
  while (*ValueStr == ' ') {
    ValueStr++;
  }

  //
  // Replace the 1st trailing space with NULL character
  //
  for (Index = 0; Index < AsciiStrLen(ValueStr); Index++) {
    if (ValueStr[Index] == ' ') {
      ValueStr[Index] = 0;
      break;
    }
  }
  if (HexMode) {
    return AsciiStrHexToUintn (ValueStr);
  } else {
    return AsciiStrDecimalToUintn (ValueStr);
  }
}  

UINTN
EvaluatePcdValue (
  CHAR8 *Line
  )
{
  CHAR8 *ValueStr;
  UINTN Index;
  UINTN PcdValueStrLen;
  BOOLEAN HexMode;
  
  HexMode = FALSE;
  
  PcdValueStrLen = AsciiStrLen("_PCD_VALUE_");
  for (Index = 0; Index < AsciiStrLen(Line) - PcdValueStrLen; Index++) {
    if (AsciiStrnCmp(Line + Index, "_PCD_VALUE_", PcdValueStrLen) == 0) {
      while (*(Line + Index) != ' ' && *(Line + Index) != '\t') {
        Index++;
        if (Index >= AsciiStrLen(Line)) goto Done;
      }
      while (*(Line + Index) == ' ' || *(Line + Index) == '\t') {
        Index++;
        if (Index >= AsciiStrLen(Line)) goto Done;
      }
      ValueStr = Line + Index;
      if (ValueStr[0]=='0' && (ValueStr[1]|0x20) == 'x') {
        HexMode = TRUE;
        ValueStr += 2;
      }
      return Evaluate (ValueStr, HexMode);
    }
  }
Done:  
  return 0; 
}  
                                                 
UINTN 
GetPcdValue (
  CHAR8 *PcdName
  )
{
  CHAR8 CurrentDir[512];
  CHAR8 AutoGenHFileName[1024];
  CHAR8 Line[1024];
  CHAR8 PcdValueDefine[1024];
  FILE *AutoGenHFile;
  UINTN FileSize;
  UINTN ReadSize;
  
  FileSize = 0;
  ReadSize = 0;
  mBufIndex = 0;
  if (!mAutoGenBufValid) {
#ifdef __GNUC__
    getcwd(CurrentDir, sizeof(CurrentDir));
#else
    _getcwd(CurrentDir, sizeof(CurrentDir));
#endif

    sprintf (AutoGenHFileName, "%s/DEBUG/AutoGen.h", CurrentDir);
    AutoGenHFile = fopen(AutoGenHFileName, "r");
    if (AutoGenHFile == NULL) {
      fprintf(stderr, "Tool failure: unable to read %s\n", AutoGenHFileName);
      exit (-1);
    }
    fseek (AutoGenHFile, 0, SEEK_END);
    FileSize = ftell(AutoGenHFile);
    fseek (AutoGenHFile, 0, SEEK_SET);
    if (FileSize > MAX_AUTO_GEN_FILE_SIZE) {
      fprintf (stderr, "Tool failure: AutoGen.h file size is larger than 64KB\n");
      fclose (AutoGenHFile);
      exit (-1);
    }
    ReadSize = fread(mAutoGenBuf, 1, FileSize, AutoGenHFile);
    fclose (AutoGenHFile);   
  } 
  sprintf(PcdValueDefine, "_PCD_VALUE_%s", PcdName);
  while (mBufIndex < ReadSize) {
    GetLineFromBuffer(Line, mAutoGenBuf, ReadSize);
    
    if ((VOID *)AsciiStrStr(Line, PcdValueDefine) != NULL) {
      return EvaluatePcdValue(Line);
    }
  }
  fprintf (stderr, "Cannot find %s PCD, make sure to put the PCD in [Pcd] section of your INF file\n", PcdName);
  exit(-1);
}
