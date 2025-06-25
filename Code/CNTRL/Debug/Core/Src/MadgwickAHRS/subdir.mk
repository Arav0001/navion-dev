################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/MadgwickAHRS/MadgwickAHRS.c 

OBJS += \
./Core/Src/MadgwickAHRS/MadgwickAHRS.o 

C_DEPS += \
./Core/Src/MadgwickAHRS/MadgwickAHRS.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/MadgwickAHRS/%.o Core/Src/MadgwickAHRS/%.su Core/Src/MadgwickAHRS/%.cyclo: ../Core/Src/MadgwickAHRS/%.c Core/Src/MadgwickAHRS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../FATFS/Target -I../FATFS/App -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/ST/STM32_MotionGC_Library/Inc -I../Middlewares/ST/STM32_MotionGT_Library/Inc -I../Middlewares/ST/STM32_MotionMC_Library/Inc -I../Middlewares/ST/STM32_MotionFX_Library/Inc -I../Middlewares/ST/STM32_MotionAC_Library/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-MadgwickAHRS

clean-Core-2f-Src-2f-MadgwickAHRS:
	-$(RM) ./Core/Src/MadgwickAHRS/MadgwickAHRS.cyclo ./Core/Src/MadgwickAHRS/MadgwickAHRS.d ./Core/Src/MadgwickAHRS/MadgwickAHRS.o ./Core/Src/MadgwickAHRS/MadgwickAHRS.su

.PHONY: clean-Core-2f-Src-2f-MadgwickAHRS

