################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/examples/host/msc_file_explorer/src/main.c \
../tinyusb/examples/host/msc_file_explorer/src/msc_app.c 

OBJS += \
./tinyusb/examples/host/msc_file_explorer/src/main.o \
./tinyusb/examples/host/msc_file_explorer/src/msc_app.o 

C_DEPS += \
./tinyusb/examples/host/msc_file_explorer/src/main.d \
./tinyusb/examples/host/msc_file_explorer/src/msc_app.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/examples/host/msc_file_explorer/src/%.o tinyusb/examples/host/msc_file_explorer/src/%.su tinyusb/examples/host/msc_file_explorer/src/%.cyclo: ../tinyusb/examples/host/msc_file_explorer/src/%.c tinyusb/examples/host/msc_file_explorer/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-examples-2f-host-2f-msc_file_explorer-2f-src

clean-tinyusb-2f-examples-2f-host-2f-msc_file_explorer-2f-src:
	-$(RM) ./tinyusb/examples/host/msc_file_explorer/src/main.cyclo ./tinyusb/examples/host/msc_file_explorer/src/main.d ./tinyusb/examples/host/msc_file_explorer/src/main.o ./tinyusb/examples/host/msc_file_explorer/src/main.su ./tinyusb/examples/host/msc_file_explorer/src/msc_app.cyclo ./tinyusb/examples/host/msc_file_explorer/src/msc_app.d ./tinyusb/examples/host/msc_file_explorer/src/msc_app.o ./tinyusb/examples/host/msc_file_explorer/src/msc_app.su

.PHONY: clean-tinyusb-2f-examples-2f-host-2f-msc_file_explorer-2f-src

