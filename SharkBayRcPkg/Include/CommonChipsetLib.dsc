#;****************************************************************************** 
#;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.  
#;* 
#;* You may not reproduce, distribute, publish, display, perform, modify, adapt,  
#;* transmit, broadcast, present, recite, release, license or otherwise exploit  
#;* any part of this publication in any form, by any means, without the prior 
#;* written permission of Insyde Software Corporation. 
#;* 
#;****************************************************************************** 
#; 
#;   
#; 
#;------------------------------------------------------------------------------ 
#; Module Name :
#;   CommonChipsetLib.dsc
#; Abstract :
#;   Libraries for common chipset utilization.

#################################################################################
##                                                                             ##
##                        Common Chipset DXE Library                           ##
##                                                                             ##
#################################################################################
DEFINE PROCESSOR=$(BUILD_TARGET_PROCESSOR)
$(COMMON_CHIPSET_DIR)/Library/PeiNullLib/PeiNullLib.inf
$(COMMON_CHIPSET_DIR)/Library/DxeNullLib/DxeNullLib.inf
$(COMMON_CHIPSET_DIR)/Guid/CommonChipsetGuidLib.inf
#[-start-110804-IB08450011-add]#
$(COMMON_CHIPSET_DIR)/Protocol/CommonChipsetProtocolLib.inf
#[-end-110804-IB08450011-add]#

#################################################################################
##                                                                             ##
##                        Common Chipset PEI Library                           ##
##                                                                             ##
#################################################################################
DEFINE PROCESSOR=IA32
!IF "$(EFI_BUILD_TARGET_X64)" == "YES"
$(COMMON_CHIPSET_DIR)/Library/BootScriptLib/BootScriptLib.inf
$(COMMON_CHIPSET_DIR)/Library/PeiNullLib/PeiNullLib.inf
!ENDIF
$(COMMON_CHIPSET_DIR)/Ppi/CommonChipsetPpiLib.inf
$(COMMON_CHIPSET_DIR)/Guid/CommonChipsetGuidIa32Lib.inf