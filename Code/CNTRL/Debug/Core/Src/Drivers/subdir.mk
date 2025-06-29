################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Drivers/led.c \
../Core/Src/Drivers/pwm.c \
../Core/Src/Drivers/pyro.c \
../Core/Src/Drivers/servo.c \
../Core/Src/Drivers/w25q128jv.c 

OBJS += \
./Core/Src/Drivers/led.o \
./Core/Src/Drivers/pwm.o \
./Core/Src/Drivers/pyro.o \
./Core/Src/Drivers/servo.o \
./Core/Src/Drivers/w25q128jv.o 

C_DEPS += \
./Core/Src/Drivers/led.d \
./Core/Src/Drivers/pwm.d \
./Core/Src/Drivers/pyro.d \
./Core/Src/Drivers/servo.d \
./Core/Src/Drivers/w25q128jv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Drivers/%.o Core/Src/Drivers/%.su Core/Src/Drivers/%.cyclo: ../Core/Src/Drivers/%.c Core/Src/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../FATFS/Target -I../FATFS/App -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/ST/STM32_MotionFX_Library/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Drivers

clean-Core-2f-Src-2f-Drivers:
	-$(RM) ./Core/Src/Drivers/led.cyclo ./Core/Src/Drivers/led.d ./Core/Src/Drivers/led.o ./Core/Src/Drivers/led.su ./Core/Src/Drivers/pwm.cyclo ./Core/Src/Drivers/pwm.d ./Core/Src/Drivers/pwm.o ./Core/Src/Drivers/pwm.su ./Core/Src/Drivers/pyro.cyclo ./Core/Src/Drivers/pyro.d ./Core/Src/Drivers/pyro.o ./Core/Src/Drivers/pyro.su ./Core/Src/Drivers/servo.cyclo ./Core/Src/Drivers/servo.d ./Core/Src/Drivers/servo.o ./Core/Src/Drivers/servo.su ./Core/Src/Drivers/w25q128jv.cyclo ./Core/Src/Drivers/w25q128jv.d ./Core/Src/Drivers/w25q128jv.o ./Core/Src/Drivers/w25q128jv.su

.PHONY: clean-Core-2f-Src-2f-Drivers

