################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/fomu/fomu.c 

S_UPPER_SRCS += \
../tinyusb/hw/bsp/fomu/crt0-vexriscv.S 

OBJS += \
./tinyusb/hw/bsp/fomu/crt0-vexriscv.o \
./tinyusb/hw/bsp/fomu/fomu.o 

S_UPPER_DEPS += \
./tinyusb/hw/bsp/fomu/crt0-vexriscv.d 

C_DEPS += \
./tinyusb/hw/bsp/fomu/fomu.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/fomu/%.o: ../tinyusb/hw/bsp/fomu/%.S tinyusb/hw/bsp/fomu/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
tinyusb/hw/bsp/fomu/%.o tinyusb/hw/bsp/fomu/%.su tinyusb/hw/bsp/fomu/%.cyclo: ../tinyusb/hw/bsp/fomu/%.c tinyusb/hw/bsp/fomu/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-fomu

clean-tinyusb-2f-hw-2f-bsp-2f-fomu:
	-$(RM) ./tinyusb/hw/bsp/fomu/crt0-vexriscv.d ./tinyusb/hw/bsp/fomu/crt0-vexriscv.o ./tinyusb/hw/bsp/fomu/fomu.cyclo ./tinyusb/hw/bsp/fomu/fomu.d ./tinyusb/hw/bsp/fomu/fomu.o ./tinyusb/hw/bsp/fomu/fomu.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-fomu

