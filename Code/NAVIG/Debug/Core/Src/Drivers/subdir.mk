################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Drivers/bmp390.c \
../Core/Src/Drivers/bno055.c \
../Core/Src/Drivers/neom9n.c \
../Core/Src/Drivers/uart_device.c 

OBJS += \
./Core/Src/Drivers/bmp390.o \
./Core/Src/Drivers/bno055.o \
./Core/Src/Drivers/neom9n.o \
./Core/Src/Drivers/uart_device.o 

C_DEPS += \
./Core/Src/Drivers/bmp390.d \
./Core/Src/Drivers/bno055.d \
./Core/Src/Drivers/neom9n.d \
./Core/Src/Drivers/uart_device.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Drivers/%.o Core/Src/Drivers/%.su Core/Src/Drivers/%.cyclo: ../Core/Src/Drivers/%.c Core/Src/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F205xx -c -I../Core/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F2xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Drivers

clean-Core-2f-Src-2f-Drivers:
	-$(RM) ./Core/Src/Drivers/bmp390.cyclo ./Core/Src/Drivers/bmp390.d ./Core/Src/Drivers/bmp390.o ./Core/Src/Drivers/bmp390.su ./Core/Src/Drivers/bno055.cyclo ./Core/Src/Drivers/bno055.d ./Core/Src/Drivers/bno055.o ./Core/Src/Drivers/bno055.su ./Core/Src/Drivers/neom9n.cyclo ./Core/Src/Drivers/neom9n.d ./Core/Src/Drivers/neom9n.o ./Core/Src/Drivers/neom9n.su ./Core/Src/Drivers/uart_device.cyclo ./Core/Src/Drivers/uart_device.d ./Core/Src/Drivers/uart_device.o ./Core/Src/Drivers/uart_device.su

.PHONY: clean-Core-2f-Src-2f-Drivers

