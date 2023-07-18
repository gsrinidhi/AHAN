################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/startup_gcc/newlib_stubs.c 

S_UPPER_SRCS += \
../CMSIS/startup_gcc/startup_m2sxxx.S 

OBJS += \
./CMSIS/startup_gcc/newlib_stubs.o \
./CMSIS/startup_gcc/startup_m2sxxx.o 

C_DEPS += \
./CMSIS/startup_gcc/newlib_stubs.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\CMSIS -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\CMSIS\startup_gcc -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\CoreGPIO -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\CoreI2C -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\CoreUARTapb -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_gpio -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_hpdma -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_i2c -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_nvm -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_spi -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_sys_services -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_timer -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers\mss_uart -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers_config -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\drivers_config\sys_config -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\hal -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\hal\CortexM3 -IC:\D_Drive\Srinidhi\BTech\SSpace\CDH\AHAN\FSW\AHAN_FSW\SoftConsole\AHAN_sb_MSS_CM3\AHAN_sb_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Assembler'
	arm-none-eabi-gcc -c -mthumb -mcpu=cortex-m3 -specs=bare.specs  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


