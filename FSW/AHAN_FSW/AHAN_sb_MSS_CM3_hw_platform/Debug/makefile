################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include hal/CortexM3/subdir.mk
-include hal/CortexM3/GNU/subdir.mk
-include drivers_config/sys_config/subdir.mk
-include drivers/mss_uart/subdir.mk
-include drivers/mss_sys_services/subdir.mk
-include drivers/mss_spi/subdir.mk
-include drivers/mss_rtc/subdir.mk
-include drivers/mss_nvm/subdir.mk
-include drivers/mss_i2c/subdir.mk
-include drivers/mss_hpdma/subdir.mk
-include drivers/mss_gpio/subdir.mk
-include drivers/CoreUARTapb/subdir.mk
-include drivers/CoreI2C/subdir.mk
-include drivers/CoreGPIO/subdir.mk
-include CMSIS/subdir.mk
-include CMSIS/startup_gcc/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libAHAN_sb_MSS_CM3_hw_platform.a

# Tool invocations
libAHAN_sb_MSS_CM3_hw_platform.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Archiver'
	arm-none-eabi-ar -r "libAHAN_sb_MSS_CM3_hw_platform.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean::
	-$(RM) $(OBJS)$(C_DEPS)$(ARCHIVES) libAHAN_sb_MSS_CM3_hw_platform.a
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
