################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.c 

OBJS += \
./tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.o 

C_DEPS += \
./tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/%.o tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/%.su tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/%.cyclo: ../tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/%.c tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/heiso/Projects/heiso/macrolev/firmware-cube/tinyusb" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-hw-2f-bsp-2f-kinetis_k32l2-2f-boards-2f-frdm_k32l2a4s

clean-tinyusb-2f-hw-2f-bsp-2f-kinetis_k32l2-2f-boards-2f-frdm_k32l2a4s:
	-$(RM) ./tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.cyclo ./tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.d ./tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.o ./tinyusb/hw/bsp/kinetis_k32l2/boards/frdm_k32l2a4s/frdm_k32l2a4s.su

.PHONY: clean-tinyusb-2f-hw-2f-bsp-2f-kinetis_k32l2-2f-boards-2f-frdm_k32l2a4s

