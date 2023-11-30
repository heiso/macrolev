################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/nrf/family.c 

OBJS += \
./tinyusb/hw/bsp/nrf/family.o 

C_DEPS += \
./tinyusb/hw/bsp/nrf/family.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/nrf/%.o tinyusb/hw/bsp/nrf/%.su tinyusb/hw/bsp/nrf/%.cyclo: ../tinyusb/hw/bsp/nrf/%.c tinyusb/hw/bsp/nrf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-nrf

clean-tinyusb-2f-hw-2f-bsp-2f-nrf:
	-$(RM) ./tinyusb/hw/bsp/nrf/family.cyclo ./tinyusb/hw/bsp/nrf/family.d ./tinyusb/hw/bsp/nrf/family.o ./tinyusb/hw/bsp/nrf/family.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-nrf

