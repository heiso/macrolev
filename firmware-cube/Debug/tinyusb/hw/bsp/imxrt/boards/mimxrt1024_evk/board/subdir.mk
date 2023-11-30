################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.c \
../tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.c 

OBJS += \
./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.o \
./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.o 

C_DEPS += \
./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.d \
./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/%.o tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/%.su tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/%.cyclo: ../tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/%.c tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-imxrt-2f-boards-2f-mimxrt1024_evk-2f-board

clean-tinyusb-2f-hw-2f-bsp-2f-imxrt-2f-boards-2f-mimxrt1024_evk-2f-board:
	-$(RM) ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.cyclo ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.d ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.o ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/clock_config.su ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.cyclo ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.d ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.o ./tinyusb/hw/bsp/imxrt/boards/mimxrt1024_evk/board/pin_mux.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-imxrt-2f-boards-2f-mimxrt1024_evk-2f-board

