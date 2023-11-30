################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/examples/typec/power_delivery/src/main.c 

OBJS += \
./tinyusb/examples/typec/power_delivery/src/main.o 

C_DEPS += \
./tinyusb/examples/typec/power_delivery/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/examples/typec/power_delivery/src/%.o tinyusb/examples/typec/power_delivery/src/%.su tinyusb/examples/typec/power_delivery/src/%.cyclo: ../tinyusb/examples/typec/power_delivery/src/%.c tinyusb/examples/typec/power_delivery/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-examples-2f-typec-2f-power_delivery-2f-src

clean-tinyusb-2f-examples-2f-typec-2f-power_delivery-2f-src:
	-$(RM) ./tinyusb/examples/typec/power_delivery/src/main.cyclo ./tinyusb/examples/typec/power_delivery/src/main.d ./tinyusb/examples/typec/power_delivery/src/main.o ./tinyusb/examples/typec/power_delivery/src/main.su

.PHONY: clean-tinyusb-2f-examples-2f-typec-2f-power_delivery-2f-src

