################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/lib/networking/dhserver.c \
../tinyusb/lib/networking/dnserver.c \
../tinyusb/lib/networking/rndis_reports.c 

OBJS += \
./tinyusb/lib/networking/dhserver.o \
./tinyusb/lib/networking/dnserver.o \
./tinyusb/lib/networking/rndis_reports.o 

C_DEPS += \
./tinyusb/lib/networking/dhserver.d \
./tinyusb/lib/networking/dnserver.d \
./tinyusb/lib/networking/rndis_reports.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/lib/networking/%.o tinyusb/lib/networking/%.su tinyusb/lib/networking/%.cyclo: ../tinyusb/lib/networking/%.c tinyusb/lib/networking/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-lib-2f-networking

clean-tinyusb-2f-lib-2f-networking:
	-$(RM) ./tinyusb/lib/networking/dhserver.cyclo ./tinyusb/lib/networking/dhserver.d ./tinyusb/lib/networking/dhserver.o ./tinyusb/lib/networking/dhserver.su ./tinyusb/lib/networking/dnserver.cyclo ./tinyusb/lib/networking/dnserver.d ./tinyusb/lib/networking/dnserver.o ./tinyusb/lib/networking/dnserver.su ./tinyusb/lib/networking/rndis_reports.cyclo ./tinyusb/lib/networking/rndis_reports.d ./tinyusb/lib/networking/rndis_reports.o ./tinyusb/lib/networking/rndis_reports.su

.PHONY: clean-tinyusb-2f-lib-2f-networking

