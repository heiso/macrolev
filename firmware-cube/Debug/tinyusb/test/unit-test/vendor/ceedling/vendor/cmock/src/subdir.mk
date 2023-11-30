################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.c 

OBJS += \
./tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.o 

C_DEPS += \
./tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/%.o tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/%.su tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/%.cyclo: ../tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/%.c tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-vendor-2f-cmock-2f-src

clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-vendor-2f-cmock-2f-src:
	-$(RM) ./tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.cyclo ./tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.d ./tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.o ./tinyusb/test/unit-test/vendor/ceedling/vendor/cmock/src/cmock.su

.PHONY: clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-vendor-2f-cmock-2f-src

