//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for Prefetcher

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Prefetcher.h

**/

#ifndef _CPU_PREFETCHER_H_
#define _CPU_PREFETCHER_H_

#include "Cpu.h"

/**
  Detect capability of Hardware Prefetcher for specified processor.
  
  This function detects capability of Hardware Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
HardwarePrefetcherDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for Hardware Prefetcher for all processors.
  
  This function configures Processor Feature Lists for Hardware Prefetcher for all processors.

**/
VOID
HardwarePrefetcherConfigFeatureList (
  VOID
  );

/**
  Produces entry of HardwarePrefetcher in Register Table for specified processor.
  
  This function produces entry of Hardware Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
HardwarePrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of Adjacent Cache Line Prefetch for specified processor.
  
  This function detects capability of Adjacent Cache Line Prefetch for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
AdjacentCacheLinePrefetchDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for Adjacent Cache Line Prefetch for all processors.
  
  This function configures Processor Feature Lists for Adjacent Cache Line Prefetch for all processors.

**/
VOID
AdjacentCacheLinePrefetchConfigFeatureList (
  VOID
  );

/**
  Produces entry of Adjacent Cache Line Prefetch in Register Table for specified processor.
  
  This function produces entry of Adjacent Cache Line Prefetch in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
AdjacentCacheLinePrefetchReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of DCU Prefetcher for specified processor.
  
  This function detects capability of DCU Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
DcuPrefetcherDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for DCU Prefetcher for all processors.
  
  This function configures Processor Feature Lists for DCU Prefetcher for all processors.

**/
VOID
DcuPrefetcherConfigFeatureList (
  VOID
  );

/**
  Produces entry of DCU Prefetcher in Register Table for specified processor.
  
  This function produces entry of DCU Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
DcuPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of IP Prefetcher for specified processor.
  
  This function detects capability of IP Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
IpPrefetcherDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for IP Prefetcher for all processors.
  
  This function configures Processor Feature Lists for IP Prefetcher for all processors.

**/
VOID
IpPrefetcherConfigFeatureList (
  VOID
  );

/**
  Produces entry of IP Prefetcher in Register Table for specified processor.
  
  This function produces entry of IP Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
IpPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of MLC Streamer Prefetcher for specified processor.
  
  This function detects capability of MLC Streamer Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MlcStreamerPrefetcherDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for MLC Streamer Prefetcher for all processors.
  
  This function configures Processor Feature Lists for MLC Streamer Prefetcher for all processors.

**/
VOID
MlcStreamerPrefetcherConfigFeatureList (
  VOID
  );

/**
  Produces entry of MLC Streamer Prefetcher in Register Table for specified processor.
  
  This function produces entry of MLC Streamer Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MlcStreamerPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

/**
  Detect capability of MLC Spatial Prefetcher for specified processor.
  
  This function detects capability of MLC Spatial Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MlcSpatialPrefetcherDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for MLC Spatial Prefetcher for all processors.
  
  This function configures Processor Feature Lists for MLC Spatial Prefetcher for all processors.

**/
VOID
MlcSpatialPrefetcherConfigFeatureList (
  VOID
  );

/**
  Produces entry of MLC Spatial Prefetcher in Register Table for specified processor.
  
  This function produces entry of MLC Spatial Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MlcSpatialPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
