################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.c 

OBJS += \
./tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.o 

C_DEPS += \
./tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/%.o tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/%.su tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/%.cyclo: ../tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/%.c tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-mm32-2f-boards-2f-mm32f327x_pitaya_lite

clean-tinyusb-2f-hw-2f-bsp-2f-mm32-2f-boards-2f-mm32f327x_pitaya_lite:
	-$(RM) ./tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.cyclo ./tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.d ./tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.o ./tinyusb/hw/bsp/mm32/boards/mm32f327x_pitaya_lite/mm32f327x_pitaya_lite.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-mm32-2f-boards-2f-mm32f327x_pitaya_lite

