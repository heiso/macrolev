################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.s 

C_SRCS += \
../tinyusb/hw/bsp/ch32f20x/ch32f20x_it.c \
../tinyusb/hw/bsp/ch32f20x/debug_uart.c \
../tinyusb/hw/bsp/ch32f20x/family.c \
../tinyusb/hw/bsp/ch32f20x/system_ch32f20x.c 

OBJS += \
./tinyusb/hw/bsp/ch32f20x/ch32f20x_it.o \
./tinyusb/hw/bsp/ch32f20x/debug_uart.o \
./tinyusb/hw/bsp/ch32f20x/family.o \
./tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.o \
./tinyusb/hw/bsp/ch32f20x/system_ch32f20x.o 

S_DEPS += \
./tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.d 

C_DEPS += \
./tinyusb/hw/bsp/ch32f20x/ch32f20x_it.d \
./tinyusb/hw/bsp/ch32f20x/debug_uart.d \
./tinyusb/hw/bsp/ch32f20x/family.d \
./tinyusb/hw/bsp/ch32f20x/system_ch32f20x.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/ch32f20x/%.o tinyusb/hw/bsp/ch32f20x/%.su tinyusb/hw/bsp/ch32f20x/%.cyclo: ../tinyusb/hw/bsp/ch32f20x/%.c tinyusb/hw/bsp/ch32f20x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
tinyusb/hw/bsp/ch32f20x/%.o: ../tinyusb/hw/bsp/ch32f20x/%.s tinyusb/hw/bsp/ch32f20x/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-ch32f20x

clean-tinyusb-2f-hw-2f-bsp-2f-ch32f20x:
	-$(RM) ./tinyusb/hw/bsp/ch32f20x/ch32f20x_it.cyclo ./tinyusb/hw/bsp/ch32f20x/ch32f20x_it.d ./tinyusb/hw/bsp/ch32f20x/ch32f20x_it.o ./tinyusb/hw/bsp/ch32f20x/ch32f20x_it.su ./tinyusb/hw/bsp/ch32f20x/debug_uart.cyclo ./tinyusb/hw/bsp/ch32f20x/debug_uart.d ./tinyusb/hw/bsp/ch32f20x/debug_uart.o ./tinyusb/hw/bsp/ch32f20x/debug_uart.su ./tinyusb/hw/bsp/ch32f20x/family.cyclo ./tinyusb/hw/bsp/ch32f20x/family.d ./tinyusb/hw/bsp/ch32f20x/family.o ./tinyusb/hw/bsp/ch32f20x/family.su ./tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.d ./tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.o ./tinyusb/hw/bsp/ch32f20x/system_ch32f20x.cyclo ./tinyusb/hw/bsp/ch32f20x/system_ch32f20x.d ./tinyusb/hw/bsp/ch32f20x/system_ch32f20x.o ./tinyusb/hw/bsp/ch32f20x/system_ch32f20x.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-ch32f20x

