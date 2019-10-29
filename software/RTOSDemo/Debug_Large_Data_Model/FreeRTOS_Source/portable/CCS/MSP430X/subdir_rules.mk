################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS_Source/portable/CCS/MSP430X/%.obj: ../FreeRTOS_Source/portable/CCS/MSP430X/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/justin/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --include_path="/home/justin/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="/home/justin/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source/Common_Demo_Files/include" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source/MSP-EXP430F5438_HAL" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source/F5XX_6XX_Core_Lib" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/FreeRTOS_Source/include" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/FreeRTOS_Source/portable/CCS/MSP430X" --include_path="/home/justin/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="FreeRTOS_Source/portable/CCS/MSP430X/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS_Source/portable/CCS/MSP430X" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS_Source/portable/CCS/MSP430X/%.obj: ../FreeRTOS_Source/portable/CCS/MSP430X/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/justin/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/bin/cl430" -vmspx --code_model=large --data_model=large --use_hw_mpy=F5 --include_path="/home/justin/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="/home/justin/ti/ccs920/ccs/ccs_base/msp430/include" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source/Common_Demo_Files/include" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source/MSP-EXP430F5438_HAL" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source/F5XX_6XX_Core_Lib" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/Demo_Source" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/FreeRTOS_Source/include" --include_path="/home/justin/workspace/mburst/ATACS/software/RTOSDemo/FreeRTOS_Source/portable/CCS/MSP430X" --include_path="/home/justin/ti/ccs920/ccs/tools/compiler/ti-cgt-msp430_18.12.3.LTS/include" --define=__MSP430F5438__ -g --printf_support=full --diag_warning=225 --abi=eabi --silicon_errata=CPU15 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="FreeRTOS_Source/portable/CCS/MSP430X/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS_Source/portable/CCS/MSP430X" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


