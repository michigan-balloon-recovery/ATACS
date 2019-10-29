################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./Demo_Source/Common_Demo_Files/GenQTest.obj" \
"./Demo_Source/Common_Demo_Files/TimerDemo.obj" \
"./Demo_Source/Common_Demo_Files/comtest.obj" \
"./Demo_Source/Common_Demo_Files/countsem.obj" \
"./Demo_Source/Common_Demo_Files/dynamic.obj" \
"./Demo_Source/F5XX_6XX_Core_Lib/HAL_PMM.obj" \
"./Demo_Source/F5XX_6XX_Core_Lib/hal_UCS.obj" \
"./Demo_Source/MSP-EXP430F5438_HAL/hal_board.obj" \
"./Demo_Source/MSP-EXP430F5438_HAL/hal_buttons.obj" \
"./Demo_Source/MSP-EXP430F5438_HAL/hal_lcd.obj" \
"./Demo_Source/MSP-EXP430F5438_HAL/hal_lcd_fonts.obj" \
"./Demo_Source/ParTest.obj" \
"./Demo_Source/RegTest.obj" \
"./Demo_Source/RunTimeStatsConfig.obj" \
"./Demo_Source/main.obj" \
"./Demo_Source/printf-stdarg.obj" \
"./Demo_Source/serial.obj" \
"./FreeRTOS_Source/list.obj" \
"./FreeRTOS_Source/queue.obj" \
"./FreeRTOS_Source/tasks.obj" \
"./FreeRTOS_Source/timers.obj" \
"./FreeRTOS_Source/portable/CCS/MSP430X/port.obj" \
"./FreeRTOS_Source/portable/CCS/MSP430X/portext.obj" \
"./FreeRTOS_Source/portable/MemMang/heap_1.obj" \
"../lnk_msp430f5438.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-llibmath.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Demo_Source/Common_Demo_Files/subdir_vars.mk
-include Demo_Source/F5XX_6XX_Core_Lib/subdir_vars.mk
-include Demo_Source/MSP-EXP430F5438_HAL/subdir_vars.mk
-include Demo_Source/subdir_vars.mk
-include FreeRTOS_Source/subdir_vars.mk
-include FreeRTOS_Source/portable/CCS/MSP430X/subdir_vars.mk
-include FreeRTOS_Source/portable/MemMang/subdir_vars.mk
-include subdir_rules.mk
-include Demo_Source/Common_Demo_Files/subdir_rules.mk
-include Demo_Source/F5XX_6XX_Core_Lib/subdir_rules.mk
-include Demo_Source/MSP-EXP430F5438_HAL/subdir_rules.mk
-include Demo_Source/subdir_rules.mk
-include FreeRTOS_Source/subdir_rules.mk
-include FreeRTOS_Source/portable/CCS/MSP430X/subdir_rules.mk
-include FreeRTOS_Source/portable/MemMang/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
RTOSDemo.out \

EXE_OUTPUTS__QUOTED += \
"RTOSDemo.out" \

BIN_OUTPUTS += \
RTOSDemo.hex \

BIN_OUTPUTS__QUOTED += \
"RTOSDemo.hex" \


# All Target
all: RTOSDemo.out

# Tool invocations
RTOSDemo.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: MSP430 Linker'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 -z -m"RTOSDemo.map" --heap_size=0 --stack_size=300 --cinit_hold_wdt=on -i"C:/ti/ccs920/ccs/ccs_base/msp430/include" -i"C:/ti/ccs920/ccs/ccs_base/msp430/include" -i"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/lib" -i"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" -i"C:/ti/ccs920/ccs/ccs_base/msp430/lib/5xx_6xx_FRxx" --reread_libs --warn_sections --xml_link_info="RTOSDemo_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "RTOSDemo.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

RTOSDemo.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: MSP430 Hex Utility'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/hex430" --memwidth=8 --romwidth=8 -o "RTOSDemo.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "Demo_Source\Common_Demo_Files\GenQTest.obj" "Demo_Source\Common_Demo_Files\TimerDemo.obj" "Demo_Source\Common_Demo_Files\comtest.obj" "Demo_Source\Common_Demo_Files\countsem.obj" "Demo_Source\Common_Demo_Files\dynamic.obj" "Demo_Source\F5XX_6XX_Core_Lib\HAL_PMM.obj" "Demo_Source\F5XX_6XX_Core_Lib\hal_UCS.obj" "Demo_Source\MSP-EXP430F5438_HAL\hal_board.obj" "Demo_Source\MSP-EXP430F5438_HAL\hal_buttons.obj" "Demo_Source\MSP-EXP430F5438_HAL\hal_lcd.obj" "Demo_Source\MSP-EXP430F5438_HAL\hal_lcd_fonts.obj" "Demo_Source\ParTest.obj" "Demo_Source\RegTest.obj" "Demo_Source\RunTimeStatsConfig.obj" "Demo_Source\main.obj" "Demo_Source\printf-stdarg.obj" "Demo_Source\serial.obj" "FreeRTOS_Source\list.obj" "FreeRTOS_Source\queue.obj" "FreeRTOS_Source\tasks.obj" "FreeRTOS_Source\timers.obj" "FreeRTOS_Source\portable\CCS\MSP430X\port.obj" "FreeRTOS_Source\portable\CCS\MSP430X\portext.obj" "FreeRTOS_Source\portable\MemMang\heap_1.obj" 
	-$(RM) "Demo_Source\Common_Demo_Files\GenQTest.d" "Demo_Source\Common_Demo_Files\TimerDemo.d" "Demo_Source\Common_Demo_Files\comtest.d" "Demo_Source\Common_Demo_Files\countsem.d" "Demo_Source\Common_Demo_Files\dynamic.d" "Demo_Source\F5XX_6XX_Core_Lib\HAL_PMM.d" "Demo_Source\F5XX_6XX_Core_Lib\hal_UCS.d" "Demo_Source\MSP-EXP430F5438_HAL\hal_board.d" "Demo_Source\MSP-EXP430F5438_HAL\hal_buttons.d" "Demo_Source\MSP-EXP430F5438_HAL\hal_lcd.d" "Demo_Source\MSP-EXP430F5438_HAL\hal_lcd_fonts.d" "Demo_Source\ParTest.d" "Demo_Source\RunTimeStatsConfig.d" "Demo_Source\main.d" "Demo_Source\printf-stdarg.d" "Demo_Source\serial.d" "FreeRTOS_Source\list.d" "FreeRTOS_Source\queue.d" "FreeRTOS_Source\tasks.d" "FreeRTOS_Source\timers.d" "FreeRTOS_Source\portable\CCS\MSP430X\port.d" "FreeRTOS_Source\portable\MemMang\heap_1.d" 
	-$(RM) "Demo_Source\RegTest.d" "FreeRTOS_Source\portable\CCS\MSP430X\portext.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

