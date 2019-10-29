################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Demo_Source/MSP-EXP430F5438_HAL/hal_board.obj: ../Demo_Source/MSP-EXP430F5438_HAL/hal_board.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/Common_Demo_Files/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/MSP-EXP430F5438_HAL" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/F5XX_6XX_Core_Lib" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/portable/CCS/MSP430X" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="Demo_Source/MSP-EXP430F5438_HAL/$(basename $(<F)).d_raw" --obj_directory="Demo_Source/MSP-EXP430F5438_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Demo_Source/MSP-EXP430F5438_HAL/hal_buttons.obj: ../Demo_Source/MSP-EXP430F5438_HAL/hal_buttons.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/Common_Demo_Files/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/MSP-EXP430F5438_HAL" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/F5XX_6XX_Core_Lib" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/portable/CCS/MSP430X" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="Demo_Source/MSP-EXP430F5438_HAL/$(basename $(<F)).d_raw" --obj_directory="Demo_Source/MSP-EXP430F5438_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Demo_Source/MSP-EXP430F5438_HAL/hal_lcd.obj: ../Demo_Source/MSP-EXP430F5438_HAL/hal_lcd.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/Common_Demo_Files/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/MSP-EXP430F5438_HAL" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/F5XX_6XX_Core_Lib" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/portable/CCS/MSP430X" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="Demo_Source/MSP-EXP430F5438_HAL/$(basename $(<F)).d_raw" --obj_directory="Demo_Source/MSP-EXP430F5438_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Demo_Source/MSP-EXP430F5438_HAL/hal_lcd_fonts.obj: ../Demo_Source/MSP-EXP430F5438_HAL/hal_lcd_fonts.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/Common_Demo_Files/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/MSP-EXP430F5438_HAL" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source/F5XX_6XX_Core_Lib" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/Demo_Source" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/include" --include_path="C:/Users/Justin/Desktop/MSP430X_MSP430F5438_CCS/FreeRTOS_Source/portable/CCS/MSP430X" --include_path="C:/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="Demo_Source/MSP-EXP430F5438_HAL/$(basename $(<F)).d_raw" --obj_directory="Demo_Source/MSP-EXP430F5438_HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


