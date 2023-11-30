################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.c 

OBJS += \
./tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.o 

C_DEPS += \
./tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/mindmotion/mm32/%.o tinyusb/src/portable/mindmotion/mm32/%.su tinyusb/src/portable/mindmotion/mm32/%.cyclo: ../tinyusb/src/portable/mindmotion/mm32/%.c tinyusb/src/portable/mindmotion/mm32/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-mindmotion-2f-mm32

clean-tinyusb-2f-src-2f-portable-2f-mindmotion-2f-mm32:
	-$(RM) ./tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.cyclo ./tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.d ./tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.o ./tinyusb/src/portable/mindmotion/mm32/dcd_mm32f327x_otg.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-mindmotion-2f-mm32

