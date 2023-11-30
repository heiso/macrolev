################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.c \
../tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.c 

OBJS += \
./tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.o \
./tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.o 

C_DEPS += \
./tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.d \
./tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/rx/boards/gr_citrus/%.o tinyusb/hw/bsp/rx/boards/gr_citrus/%.su tinyusb/hw/bsp/rx/boards/gr_citrus/%.cyclo: ../tinyusb/hw/bsp/rx/boards/gr_citrus/%.c tinyusb/hw/bsp/rx/boards/gr_citrus/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-rx-2f-boards-2f-gr_citrus

clean-tinyusb-2f-hw-2f-bsp-2f-rx-2f-boards-2f-gr_citrus:
	-$(RM) ./tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.cyclo ./tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.d ./tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.o ./tinyusb/hw/bsp/rx/boards/gr_citrus/gr_citrus.su ./tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.cyclo ./tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.d ./tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.o ./tinyusb/hw/bsp/rx/boards/gr_citrus/hwinit.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-rx-2f-boards-2f-gr_citrus

