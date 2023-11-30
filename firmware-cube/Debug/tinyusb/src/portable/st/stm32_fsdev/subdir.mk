################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c 

OBJS += \
./tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.o 

C_DEPS += \
./tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/st/stm32_fsdev/%.o tinyusb/src/portable/st/stm32_fsdev/%.su tinyusb/src/portable/st/stm32_fsdev/%.cyclo: ../tinyusb/src/portable/st/stm32_fsdev/%.c tinyusb/src/portable/st/stm32_fsdev/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-st-2f-stm32_fsdev

clean-tinyusb-2f-src-2f-portable-2f-st-2f-stm32_fsdev:
	-$(RM) ./tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.cyclo ./tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.d ./tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.o ./tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-st-2f-stm32_fsdev

