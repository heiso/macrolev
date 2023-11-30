################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c 

OBJS += \
./tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.o 

C_DEPS += \
./tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/nordic/nrf5x/%.o tinyusb/src/portable/nordic/nrf5x/%.su tinyusb/src/portable/nordic/nrf5x/%.cyclo: ../tinyusb/src/portable/nordic/nrf5x/%.c tinyusb/src/portable/nordic/nrf5x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-nordic-2f-nrf5x

clean-tinyusb-2f-src-2f-portable-2f-nordic-2f-nrf5x:
	-$(RM) ./tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.cyclo ./tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.d ./tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.o ./tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-nordic-2f-nrf5x

