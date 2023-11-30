################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/f1c100s/f1c100s.c 

OBJS += \
./tinyusb/hw/bsp/f1c100s/f1c100s.o 

C_DEPS += \
./tinyusb/hw/bsp/f1c100s/f1c100s.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/f1c100s/%.o tinyusb/hw/bsp/f1c100s/%.su tinyusb/hw/bsp/f1c100s/%.cyclo: ../tinyusb/hw/bsp/f1c100s/%.c tinyusb/hw/bsp/f1c100s/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-f1c100s

clean-tinyusb-2f-hw-2f-bsp-2f-f1c100s:
	-$(RM) ./tinyusb/hw/bsp/f1c100s/f1c100s.cyclo ./tinyusb/hw/bsp/f1c100s/f1c100s.d ./tinyusb/hw/bsp/f1c100s/f1c100s.o ./tinyusb/hw/bsp/f1c100s/f1c100s.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-f1c100s

