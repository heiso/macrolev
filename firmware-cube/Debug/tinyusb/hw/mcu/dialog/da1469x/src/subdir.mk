################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.c \
../tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.c \
../tinyusb/hw/mcu/dialog/da1469x/src/hal_system.c \
../tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.c \
../tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.c 

OBJS += \
./tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.o \
./tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.o \
./tinyusb/hw/mcu/dialog/da1469x/src/hal_system.o \
./tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.o \
./tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.o 

C_DEPS += \
./tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.d \
./tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.d \
./tinyusb/hw/mcu/dialog/da1469x/src/hal_system.d \
./tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.d \
./tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/mcu/dialog/da1469x/src/%.o tinyusb/hw/mcu/dialog/da1469x/src/%.su tinyusb/hw/mcu/dialog/da1469x/src/%.cyclo: ../tinyusb/hw/mcu/dialog/da1469x/src/%.c tinyusb/hw/mcu/dialog/da1469x/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-mcu-2f-dialog-2f-da1469x-2f-src

clean-tinyusb-2f-hw-2f-mcu-2f-dialog-2f-da1469x-2f-src:
	-$(RM) ./tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.cyclo ./tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.d ./tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.o ./tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.su ./tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.cyclo ./tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.d ./tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.o ./tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.su ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system.cyclo ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system.d ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system.o ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system.su ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.cyclo ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.d ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.o ./tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.su ./tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.cyclo ./tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.d ./tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.o ./tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.su

.PHONY: clean-tinyusb-2f-hw-2f-mcu-2f-dialog-2f-da1469x-2f-src

