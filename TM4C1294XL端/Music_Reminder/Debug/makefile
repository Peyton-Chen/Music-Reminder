################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./tm4c1294ncpdt_startup_ccs.obj" \
"./uartstdio.obj" \
"./ADC/ADC.obj" \
"./EPIinit/EPIinit.obj" \
"./I2C/i2c.obj" \
"./TFTinit/TFT_400x240_OTM4001A_16bit.obj" \
"./TOUCHinit/TOUCH_TSC2046.obj" \
"./Tool/KalmanFilter.obj" \
"../tm4c1294ncpdt.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/ti/TivaWare_C_Series-2.1.0.12573/driverlib/ccs/Debug/driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include ADC/subdir_vars.mk
-include EPIinit/subdir_vars.mk
-include I2C/subdir_vars.mk
-include TFTinit/subdir_vars.mk
-include TOUCHinit/subdir_vars.mk
-include Tool/subdir_vars.mk
-include subdir_rules.mk
-include ADC/subdir_rules.mk
-include EPIinit/subdir_rules.mk
-include I2C/subdir_rules.mk
-include TFTinit/subdir_rules.mk
-include TOUCHinit/subdir_rules.mk
-include Tool/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Music_Reminder.out \

EXE_OUTPUTS__QUOTED += \
"Music_Reminder.out" \

BIN_OUTPUTS += \
Music_Reminder.hex \

BIN_OUTPUTS__QUOTED += \
"Music_Reminder.hex" \


# All Target
all: Music_Reminder.out

# Tool invocations
Music_Reminder.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --display_error_number --diag_warning=225 --diag_wrap=off -z -m"Music_Reminder.map" --heap_size=0 --stack_size=512 -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"G:/M4/Workspace/I2C" -i"C:/ti/TivaWare_C_Series-2.1.0.12573/driverlib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="Music_Reminder_linkInfo.xml" --rom_model -o "Music_Reminder.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

Music_Reminder.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "Music_Reminder.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "main.d" "tm4c1294ncpdt_startup_ccs.d" "uartstdio.d" "ADC\ADC.d" "EPIinit\EPIinit.d" "I2C\i2c.d" "TFTinit\TFT_400x240_OTM4001A_16bit.d" "TOUCHinit\TOUCH_TSC2046.d" "Tool\KalmanFilter.d" 
	-$(RM) "main.obj" "tm4c1294ncpdt_startup_ccs.obj" "uartstdio.obj" "ADC\ADC.obj" "EPIinit\EPIinit.obj" "I2C\i2c.obj" "TFTinit\TFT_400x240_OTM4001A_16bit.obj" "TOUCHinit\TOUCH_TSC2046.obj" "Tool\KalmanFilter.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

