################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/backup/sis3316_class.cpp \
../src/backup/sis3316_ethernet_access_class.cpp \
../src/backup/sis3316_sample_test_gui.cpp 

OBJS += \
./src/backup/sis3316_class.o \
./src/backup/sis3316_ethernet_access_class.o \
./src/backup/sis3316_sample_test_gui.o 

CPP_DEPS += \
./src/backup/sis3316_class.d \
./src/backup/sis3316_ethernet_access_class.d \
./src/backup/sis3316_sample_test_gui.d 


# Each subdirectory must supply rules for building sources it contributes
src/backup/%.o: ../src/backup/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/th/root/include -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


