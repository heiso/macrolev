################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.c 

OBJS += \
./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.o 

C_DEPS += \
./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/%.o tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/%.su tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/%.cyclo: ../tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/%.c tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-plugins-2f-fake_function_framework-2f-examples-2f-fff_example-2f-src-2f-subfolder

clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-plugins-2f-fake_function_framework-2f-examples-2f-fff_example-2f-src-2f-subfolder:
	-$(RM) ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.cyclo ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.d ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.o ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/src/subfolder/zzz.su

.PHONY: clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-plugins-2f-fake_function_framework-2f-examples-2f-fff_example-2f-src-2f-subfolder

