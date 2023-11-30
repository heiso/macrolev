################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/class/msc/msc_device.c \
../tinyusb/src/class/msc/msc_host.c 

OBJS += \
./tinyusb/src/class/msc/msc_device.o \
./tinyusb/src/class/msc/msc_host.o 

C_DEPS += \
./tinyusb/src/class/msc/msc_device.d \
./tinyusb/src/class/msc/msc_host.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/class/msc/%.o tinyusb/src/class/msc/%.su tinyusb/src/class/msc/%.cyclo: ../tinyusb/src/class/msc/%.c tinyusb/src/class/msc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-class-2f-msc

clean-tinyusb-2f-src-2f-class-2f-msc:
	-$(RM) ./tinyusb/src/class/msc/msc_device.cyclo ./tinyusb/src/class/msc/msc_device.d ./tinyusb/src/class/msc/msc_device.o ./tinyusb/src/class/msc/msc_device.su ./tinyusb/src/class/msc/msc_host.cyclo ./tinyusb/src/class/msc/msc_host.d ./tinyusb/src/class/msc/msc_host.o ./tinyusb/src/class/msc/msc_host.su

.PHONY: clean-tinyusb-2f-src-2f-class-2f-msc

