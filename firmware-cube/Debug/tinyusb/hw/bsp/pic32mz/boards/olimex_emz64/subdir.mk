################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.c 

OBJS += \
./tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.o 

C_DEPS += \
./tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/%.o tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/%.su tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/%.cyclo: ../tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/%.c tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-pic32mz-2f-boards-2f-olimex_emz64

clean-tinyusb-2f-hw-2f-bsp-2f-pic32mz-2f-boards-2f-olimex_emz64:
	-$(RM) ./tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.cyclo ./tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.d ./tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.o ./tinyusb/hw/bsp/pic32mz/boards/olimex_emz64/olimex_emz64.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-pic32mz-2f-boards-2f-olimex_emz64

