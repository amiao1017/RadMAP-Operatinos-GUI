################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/neu/sis3316_class.cpp \
../src/neu/sis3316_ethernet_access_class.cpp \
../src/neu/sis3316_sample_test_gui.cpp 

OBJS += \
./src/neu/sis3316_class.o \
./src/neu/sis3316_ethernet_access_class.o \
./src/neu/sis3316_sample_test_gui.o 

CPP_DEPS += \
./src/neu/sis3316_class.d \
./src/neu/sis3316_ethernet_access_class.d \
./src/neu/sis3316_sample_test_gui.d 


# Each subdirectory must supply rules for building sources it contributes
src/neu/%.o: ../src/neu/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/th/root/include -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


