################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sis3316_cern_root_class.cpp \
../src/sis3316_cern_root_energy_class.cpp \
../src/sis3316_energy_histogram_monitoring.cpp 

OBJS += \
./src/sis3316_cern_root_class.o \
./src/sis3316_cern_root_energy_class.o \
./src/sis3316_energy_histogram_monitoring.o 

CPP_DEPS += \
./src/sis3316_cern_root_class.d \
./src/sis3316_cern_root_energy_class.d \
./src/sis3316_energy_histogram_monitoring.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/th/root/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


