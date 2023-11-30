################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.c \
../tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.c 

OBJS += \
./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.o \
./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.o 

C_DEPS += \
./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.d \
./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/%.o tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/%.su tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/%.cyclo: ../tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/%.c tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-plugins-2f-fake_function_framework-2f-examples-2f-fff_example-2f-test

clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-plugins-2f-fake_function_framework-2f-examples-2f-fff_example-2f-test:
	-$(RM) ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.cyclo ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.d ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.o ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_event_processor.su ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.cyclo ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.d ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.o ./tinyusb/test/unit-test/vendor/ceedling/plugins/fake_function_framework/examples/fff_example/test/test_foo.su

.PHONY: clean-tinyusb-2f-test-2f-unit-2d-test-2f-vendor-2f-ceedling-2f-plugins-2f-fake_function_framework-2f-examples-2f-fff_example-2f-test

