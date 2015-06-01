################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sis3316_class.cpp \
../src/sis3316_eth_fpga_update.cpp \
../src/sis3316_ethernet_access_class.cpp 

OBJS += \
./src/sis3316_class.o \
./src/sis3316_eth_fpga_update.o \
./src/sis3316_ethernet_access_class.o 

CPP_DEPS += \
./src/sis3316_class.d \
./src/sis3316_eth_fpga_update.d \
./src/sis3316_ethernet_access_class.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/th/sis3316/software/Eclipse_projects/sis3316_fpga_update_udp/src -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


