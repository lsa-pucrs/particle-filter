################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CU_SRCS += \
../vecAdd.cu 

CU_DEPS += \
./vecAdd.d 

OBJS += \
./vecAdd.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	nvcc -G -g -O0 -gencode arch=compute_30,code=sm_30 -odir "" -M -o "$(@:%.o=%.d)" "$<"
	nvcc --compile -G -O0 -g -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


