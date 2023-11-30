################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/valentyusb/eptri/dcd_eptri.c 

OBJS += \
./tinyusb/src/portable/valentyusb/eptri/dcd_eptri.o 

C_DEPS += \
./tinyusb/src/portable/valentyusb/eptri/dcd_eptri.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/valentyusb/eptri/%.o tinyusb/src/portable/valentyusb/eptri/%.su tinyusb/src/portable/valentyusb/eptri/%.cyclo: ../tinyusb/src/portable/valentyusb/eptri/%.c tinyusb/src/portable/valentyusb/eptri/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb/src" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-valentyusb-2f-eptri

clean-tinyusb-2f-src-2f-portable-2f-valentyusb-2f-eptri:
	-$(RM) ./tinyusb/src/portable/valentyusb/eptri/dcd_eptri.cyclo ./tinyusb/src/portable/valentyusb/eptri/dcd_eptri.d ./tinyusb/src/portable/valentyusb/eptri/dcd_eptri.o ./tinyusb/src/portable/valentyusb/eptri/dcd_eptri.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-valentyusb-2f-eptri

