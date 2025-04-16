################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/led_module.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/timer_module.c 

OBJS += \
./Src/led_module.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/timer_module.o 

C_DEPS += \
./Src/led_module.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/timer_module.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F303VCTx -DSTM32 -DSTM32F3 -DSTM32F3DISCOVERY -c -I"C:/Users/Morti/OneDrive/Documents/GitHub/stm32f303-definitions/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/led_module.cyclo ./Src/led_module.d ./Src/led_module.o ./Src/led_module.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/timer_module.cyclo ./Src/timer_module.d ./Src/timer_module.o ./Src/timer_module.su

.PHONY: clean-Src

