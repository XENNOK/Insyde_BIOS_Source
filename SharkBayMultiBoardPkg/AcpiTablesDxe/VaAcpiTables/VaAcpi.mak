# MAK file for the ModulePart:VaAcpi

#-----------------------------------------------------------------------
# ASL compiler definition
#-----------------------------------------------------------------------
MASL 		= asl.exe 	# Microsoft ASL compiler
IASL 		= iasl.exe # Intel ASL compiler. Supports extended ACPI 3.0 asl objects
#-----------------------------------------------------------------------
ASL_COMPILER = IASL	# Default ASL compiler. Can be 'IASL' for Intel ASL and 'MASL' for Microsoft ASL compiler.
# Note. Msft. ASL compiler of version 1.0.14NT correctly process ACPI 2.0 extended ASL objects.
#-----------------------------------------------------------------------
EDK : VaAcpi

ALLVASEC = $(BUILD_DIR)\VAACPI.sec

VaAcpi:  $(BUILD_DIR)\VAACPI.ffs

$(BUILD_DIR)\VaAcpi.aml: $(VAACPI_DIR)\VaAcpi.asl
!if "$(ASL_COMPILER)" == "MASL"
	@$(MASL) /Fo=$@ $**
!elseif "$(ASL_COMPILER)" == "IASL"
	$(IASL) $(VAACPI_DIR)\VaAcpi.asl
	@move $(PROJECT_DIR)\VaAcpi.Aml $(BUILD_DIR)\VaAcpi.aml
!endif

$(BUILD_DIR)\VaAcpi.sec: $(BUILD_DIR)\VaAcpi.aml
  $(GENSECTION) -I $** -O $@ -S EFI_SECTION_RAW

$(BUILD_DIR)\VaAcpi.ffs: $(ALLVASEC) $(VAACPI_DIR)\VaAcpi.mak
  $(GENFFSFILE) -B $(BUILD_DIR) -V -o $@ -P1 <<$(BUILD_DIR)\VaAcpi.pkg
PACKAGE.INF
[.]
BASE_NAME                   = VAACPI
FFS_FILEGUID                = 161BE666-E9C5-49DB-AE86-C462AB54EEDA
FFS_FILETYPE                = EFI_FV_FILETYPE_FREEFORM
FFS_ATTRIB_CHECKSUM         = TRUE

IMAGE_SCRIPT =
{
  Compress (dummy) {
	VaAcpi.sec
  }
}
<<KEEP

