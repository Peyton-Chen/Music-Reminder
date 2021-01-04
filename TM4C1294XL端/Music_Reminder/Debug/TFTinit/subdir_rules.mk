################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
TFTinit/TFT_400x240_OTM4001A_16bit.obj: ../TFTinit/TFT_400x240_OTM4001A_16bit.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c1294xl" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/work/TFT_TOUCH" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573" -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="TFTinit/TFT_400x240_OTM4001A_16bit.d" --obj_directory="TFTinit" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


