################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c \
../tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.c \
../tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c 

OBJS += \
./tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.o \
./tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.o \
./tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.o 

C_DEPS += \
./tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.d \
./tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.d \
./tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/raspberrypi/rp2040/%.o tinyusb/src/portable/raspberrypi/rp2040/%.su tinyusb/src/portable/raspberrypi/rp2040/%.cyclo: ../tinyusb/src/portable/raspberrypi/rp2040/%.c tinyusb/src/portable/raspberrypi/rp2040/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-raspberrypi-2f-rp2040

clean-tinyusb-2f-src-2f-portable-2f-raspberrypi-2f-rp2040:
	-$(RM) ./tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.cyclo ./tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.d ./tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.o ./tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.su ./tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.cyclo ./tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.d ./tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.o ./tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.su ./tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.cyclo ./tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.d ./tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.o ./tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-raspberrypi-2f-rp2040

