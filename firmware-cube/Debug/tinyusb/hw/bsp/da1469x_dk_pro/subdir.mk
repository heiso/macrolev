################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.c 

S_UPPER_SRCS += \
../tinyusb/hw/bsp/da1469x_dk_pro/gcc_startup_da1469x.S 

OBJS += \
./tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.o \
./tinyusb/hw/bsp/da1469x_dk_pro/gcc_startup_da1469x.o 

S_UPPER_DEPS += \
./tinyusb/hw/bsp/da1469x_dk_pro/gcc_startup_da1469x.d 

C_DEPS += \
./tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/da1469x_dk_pro/%.o tinyusb/hw/bsp/da1469x_dk_pro/%.su tinyusb/hw/bsp/da1469x_dk_pro/%.cyclo: ../tinyusb/hw/bsp/da1469x_dk_pro/%.c tinyusb/hw/bsp/da1469x_dk_pro/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
tinyusb/hw/bsp/da1469x_dk_pro/%.o: ../tinyusb/hw/bsp/da1469x_dk_pro/%.S tinyusb/hw/bsp/da1469x_dk_pro/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-da1469x_dk_pro

clean-tinyusb-2f-hw-2f-bsp-2f-da1469x_dk_pro:
	-$(RM) ./tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.cyclo ./tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.d ./tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.o ./tinyusb/hw/bsp/da1469x_dk_pro/da1469x-dk-pro.su ./tinyusb/hw/bsp/da1469x_dk_pro/gcc_startup_da1469x.d ./tinyusb/hw/bsp/da1469x_dk_pro/gcc_startup_da1469x.o

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-da1469x_dk_pro

