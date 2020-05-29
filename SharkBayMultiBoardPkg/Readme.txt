#;******************************************************************************
#;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
#;*
#;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#;* transmit, broadcast, present, recite, release, license or otherwise exploit
#;* any part of this publication in any form, by any means, without the prior
#;* written permission of Insyde Software Corporation.
#;*
#;******************************************************************************
#

Build procedure.
1. Modify the platform settings and compiler tool settings in ProjectBuild.bat
   under the platform package folder SharkBayMultiBoardPkg
2. Executing ProjectBuild.bat by clicking it on the file manager.
3. To build release version, type
     nmake uefi64
   to build DDT debug version, type
     nmake uefi64ddt
   to build EFI debug version, type
     nmake efidebug

