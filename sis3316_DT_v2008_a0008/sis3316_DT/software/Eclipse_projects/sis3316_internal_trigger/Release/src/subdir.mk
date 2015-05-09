################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sis3316_cern_root_class.cpp \
../src/sis3316_ch1_to_ch16_internal_trigger_test.cpp \
../src/sis3316_class.cpp \
../src/sis3316_ethernet_access_class.cpp 

OBJS += \
./src/sis3316_cern_root_class.o \
./src/sis3316_ch1_to_ch16_internal_trigger_test.o \
./src/sis3316_class.o \
./src/sis3316_ethernet_access_class.o 

CPP_DEPS += \
./src/sis3316_cern_root_class.d \
./src/sis3316_ch1_to_ch16_internal_trigger_test.d \
./src/sis3316_class.d \
./src/sis3316_ethernet_access_class.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/th/root/include -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


