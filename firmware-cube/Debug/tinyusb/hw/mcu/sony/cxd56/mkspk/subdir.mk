################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.c \
../tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.c 

OBJS += \
./tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.o \
./tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.o 

C_DEPS += \
./tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.d \
./tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/mcu/sony/cxd56/mkspk/%.o tinyusb/hw/mcu/sony/cxd56/mkspk/%.su tinyusb/hw/mcu/sony/cxd56/mkspk/%.cyclo: ../tinyusb/hw/mcu/sony/cxd56/mkspk/%.c tinyusb/hw/mcu/sony/cxd56/mkspk/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-mcu-2f-sony-2f-cxd56-2f-mkspk

clean-tinyusb-2f-hw-2f-mcu-2f-sony-2f-cxd56-2f-mkspk:
	-$(RM) ./tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.cyclo ./tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.d ./tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.o ./tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.su ./tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.cyclo ./tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.d ./tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.o ./tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.su

.PHONY: clean-tinyusb-2f-hw-2f-mcu-2f-sony-2f-cxd56-2f-mkspk

