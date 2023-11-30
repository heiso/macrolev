################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/ch32v307/ch32v30x_it.c \
../tinyusb/hw/bsp/ch32v307/family.c \
../tinyusb/hw/bsp/ch32v307/system_ch32v30x.c 

OBJS += \
./tinyusb/hw/bsp/ch32v307/ch32v30x_it.o \
./tinyusb/hw/bsp/ch32v307/family.o \
./tinyusb/hw/bsp/ch32v307/system_ch32v30x.o 

C_DEPS += \
./tinyusb/hw/bsp/ch32v307/ch32v30x_it.d \
./tinyusb/hw/bsp/ch32v307/family.d \
./tinyusb/hw/bsp/ch32v307/system_ch32v30x.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/ch32v307/%.o tinyusb/hw/bsp/ch32v307/%.su tinyusb/hw/bsp/ch32v307/%.cyclo: ../tinyusb/hw/bsp/ch32v307/%.c tinyusb/hw/bsp/ch32v307/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-ch32v307

clean-tinyusb-2f-hw-2f-bsp-2f-ch32v307:
	-$(RM) ./tinyusb/hw/bsp/ch32v307/ch32v30x_it.cyclo ./tinyusb/hw/bsp/ch32v307/ch32v30x_it.d ./tinyusb/hw/bsp/ch32v307/ch32v30x_it.o ./tinyusb/hw/bsp/ch32v307/ch32v30x_it.su ./tinyusb/hw/bsp/ch32v307/family.cyclo ./tinyusb/hw/bsp/ch32v307/family.d ./tinyusb/hw/bsp/ch32v307/family.o ./tinyusb/hw/bsp/ch32v307/family.su ./tinyusb/hw/bsp/ch32v307/system_ch32v30x.cyclo ./tinyusb/hw/bsp/ch32v307/system_ch32v30x.d ./tinyusb/hw/bsp/ch32v307/system_ch32v30x.o ./tinyusb/hw/bsp/ch32v307/system_ch32v30x.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-ch32v307

