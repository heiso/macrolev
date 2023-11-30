################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/renesas/rusb2/dcd_rusb2.c \
../tinyusb/src/portable/renesas/rusb2/hcd_rusb2.c \
../tinyusb/src/portable/renesas/rusb2/rusb2_common.c 

OBJS += \
./tinyusb/src/portable/renesas/rusb2/dcd_rusb2.o \
./tinyusb/src/portable/renesas/rusb2/hcd_rusb2.o \
./tinyusb/src/portable/renesas/rusb2/rusb2_common.o 

C_DEPS += \
./tinyusb/src/portable/renesas/rusb2/dcd_rusb2.d \
./tinyusb/src/portable/renesas/rusb2/hcd_rusb2.d \
./tinyusb/src/portable/renesas/rusb2/rusb2_common.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/renesas/rusb2/%.o tinyusb/src/portable/renesas/rusb2/%.su tinyusb/src/portable/renesas/rusb2/%.cyclo: ../tinyusb/src/portable/renesas/rusb2/%.c tinyusb/src/portable/renesas/rusb2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-renesas-2f-rusb2

clean-tinyusb-2f-src-2f-portable-2f-renesas-2f-rusb2:
	-$(RM) ./tinyusb/src/portable/renesas/rusb2/dcd_rusb2.cyclo ./tinyusb/src/portable/renesas/rusb2/dcd_rusb2.d ./tinyusb/src/portable/renesas/rusb2/dcd_rusb2.o ./tinyusb/src/portable/renesas/rusb2/dcd_rusb2.su ./tinyusb/src/portable/renesas/rusb2/hcd_rusb2.cyclo ./tinyusb/src/portable/renesas/rusb2/hcd_rusb2.d ./tinyusb/src/portable/renesas/rusb2/hcd_rusb2.o ./tinyusb/src/portable/renesas/rusb2/hcd_rusb2.su ./tinyusb/src/portable/renesas/rusb2/rusb2_common.cyclo ./tinyusb/src/portable/renesas/rusb2/rusb2_common.d ./tinyusb/src/portable/renesas/rusb2/rusb2_common.o ./tinyusb/src/portable/renesas/rusb2/rusb2_common.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-renesas-2f-rusb2

