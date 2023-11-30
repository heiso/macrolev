################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.c \
../tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.c 

S_UPPER_SRCS += \
../tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_ASM_ARMv7M.S 

OBJS += \
./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.o \
./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_ASM_ARMv7M.o \
./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.o 

S_UPPER_DEPS += \
./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_ASM_ARMv7M.d 

C_DEPS += \
./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.d \
./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/lib/SEGGER_RTT/RTT/%.o tinyusb/lib/SEGGER_RTT/RTT/%.su tinyusb/lib/SEGGER_RTT/RTT/%.cyclo: ../tinyusb/lib/SEGGER_RTT/RTT/%.c tinyusb/lib/SEGGER_RTT/RTT/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
tinyusb/lib/SEGGER_RTT/RTT/%.o: ../tinyusb/lib/SEGGER_RTT/RTT/%.S tinyusb/lib/SEGGER_RTT/RTT/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-tinyusb-2f-lib-2f-SEGGER_RTT-2f-RTT

clean-tinyusb-2f-lib-2f-SEGGER_RTT-2f-RTT:
	-$(RM) ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.cyclo ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.d ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.o ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT.su ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_ASM_ARMv7M.d ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_ASM_ARMv7M.o ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.cyclo ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.d ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.o ./tinyusb/lib/SEGGER_RTT/RTT/SEGGER_RTT_printf.su

.PHONY: clean-tinyusb-2f-lib-2f-SEGGER_RTT-2f-RTT

