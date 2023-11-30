################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/examples/device/hid_generic_inout/src/main.c \
../tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.c 

OBJS += \
./tinyusb/examples/device/hid_generic_inout/src/main.o \
./tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.o 

C_DEPS += \
./tinyusb/examples/device/hid_generic_inout/src/main.d \
./tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/examples/device/hid_generic_inout/src/%.o tinyusb/examples/device/hid_generic_inout/src/%.su tinyusb/examples/device/hid_generic_inout/src/%.cyclo: ../tinyusb/examples/device/hid_generic_inout/src/%.c tinyusb/examples/device/hid_generic_inout/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-examples-2f-device-2f-hid_generic_inout-2f-src

clean-tinyusb-2f-examples-2f-device-2f-hid_generic_inout-2f-src:
	-$(RM) ./tinyusb/examples/device/hid_generic_inout/src/main.cyclo ./tinyusb/examples/device/hid_generic_inout/src/main.d ./tinyusb/examples/device/hid_generic_inout/src/main.o ./tinyusb/examples/device/hid_generic_inout/src/main.su ./tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.cyclo ./tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.d ./tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.o ./tinyusb/examples/device/hid_generic_inout/src/usb_descriptors.su

.PHONY: clean-tinyusb-2f-examples-2f-device-2f-hid_generic_inout-2f-src

