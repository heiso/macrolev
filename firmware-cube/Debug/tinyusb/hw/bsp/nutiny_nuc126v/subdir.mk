################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.c 

OBJS += \
./tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.o 

C_DEPS += \
./tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/nutiny_nuc126v/%.o tinyusb/hw/bsp/nutiny_nuc126v/%.su tinyusb/hw/bsp/nutiny_nuc126v/%.cyclo: ../tinyusb/hw/bsp/nutiny_nuc126v/%.c tinyusb/hw/bsp/nutiny_nuc126v/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-nutiny_nuc126v

clean-tinyusb-2f-hw-2f-bsp-2f-nutiny_nuc126v:
	-$(RM) ./tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.cyclo ./tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.d ./tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.o ./tinyusb/hw/bsp/nutiny_nuc126v/nutiny_nuc126.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-nutiny_nuc126v

