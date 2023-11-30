################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.c 

OBJS += \
./tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.o 

C_DEPS += \
./tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/same5x/boards/same54_xplained/%.o tinyusb/hw/bsp/same5x/boards/same54_xplained/%.su tinyusb/hw/bsp/same5x/boards/same54_xplained/%.cyclo: ../tinyusb/hw/bsp/same5x/boards/same54_xplained/%.c tinyusb/hw/bsp/same5x/boards/same54_xplained/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-same5x-2f-boards-2f-same54_xplained

clean-tinyusb-2f-hw-2f-bsp-2f-same5x-2f-boards-2f-same54_xplained:
	-$(RM) ./tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.cyclo ./tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.d ./tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.o ./tinyusb/hw/bsp/same5x/boards/same54_xplained/same54_xplained.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-same5x-2f-boards-2f-same54_xplained

