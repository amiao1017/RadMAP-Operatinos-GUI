//============================================================================
// Name        : test_configuration_parameter.cpp
// Author      : th
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
using namespace std;

#include "project_system_define.h"		//define LINUX or WIN
#include "get_configuration_parameters.h"




#ifdef LINUX
 	#include <sys/uio.h>

	#include <ctime>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <stdlib.h>
#endif


#ifdef WINDOWS
	#include <time.h>
	#include <iostream>
	#include <iomanip>

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <tchar.h>
	#include <stdlib.h>
	#include <string.h>

	}
#endif




int main() {
   char char_config_file[64];
   int return_code ;

	cout << "test_configuration_parameter" << endl; // prints test_configuration_parameter

	sprintf(char_config_file, "sis3316_jaea_histogram_running_parameter_test.ini");


	gl_sis3316_get_configuration_parameters      = new sis3316_get_configuration_parameters() ;

	return_code = gl_sis3316_get_configuration_parameters->read_parameter_file(char_config_file);


	printf("uint_nof_events                  = %d \n",gl_sis3316_get_configuration_parameters->uint_nof_events );

	printf("uint_trigger_threshold           = %d \n",gl_sis3316_get_configuration_parameters->uint_trigger_threshold );
	printf("uint_trigger_p_value             = %d \n",gl_sis3316_get_configuration_parameters->uint_trigger_p_value );
	printf("uint_trigger_g_value             = %d \n",gl_sis3316_get_configuration_parameters->uint_trigger_g_value );


	printf("uint_pre_trigger_delay           = %d \n",gl_sis3316_get_configuration_parameters->uint_pre_trigger_delay );
	printf("uint_raw_sample_length           = %d \n",gl_sis3316_get_configuration_parameters->uint_raw_sample_length );
	printf("uint_trigger_gate_window_length  = %d \n",gl_sis3316_get_configuration_parameters->uint_trigger_gate_window_length );

	printf("uint_pileup_window_length        = %d \n",gl_sis3316_get_configuration_parameters->uint_pileup_window_length );
	printf("uint_re_pileup_window_length     = %d \n",gl_sis3316_get_configuration_parameters->uint_re_pileup_window_length );


	printf("uint_gate1_start_index           = %d \n",gl_sis3316_get_configuration_parameters->uint_gate1_start_index );
	printf("uint_gate1_length                = %d \n",gl_sis3316_get_configuration_parameters->uint_gate1_length );

	printf("uint_gate2_start_index           = %d \n",gl_sis3316_get_configuration_parameters->uint_gate2_start_index );
	printf("uint_gate2_length                = %d \n",gl_sis3316_get_configuration_parameters->uint_gate2_length );

	printf("uint_gate3_start_index           = %d \n",gl_sis3316_get_configuration_parameters->uint_gate3_start_index );
	printf("uint_gate3_length                = %d \n",gl_sis3316_get_configuration_parameters->uint_gate3_length );

	printf("uint_baseline_average_mode       = %d \n",gl_sis3316_get_configuration_parameters->uint_baseline_average_mode );



	printf("\n");
	printf("uint_PC_shapeHistogram_divider_x_val        = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_shapeHistogram_divider_x_val );
	printf("uint_PC_shapeHistogram_divider_y_val        = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_shapeHistogram_divider_y_val );


	printf("uint_PC_tof_shapeHistogram_divider_x_val    = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_tof_shapeHistogram_divider_x_val );
	printf("uint_PC_tof_shapeHistogram_divider_y_val    = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_tof_shapeHistogram_divider_y_val );

	printf("uint_PC_tofHistogram_divider_val            = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_tofHistogram_divider_val );
	printf("uint_PC_chargeHistogram_divider_val         = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_chargeHistogram_divider_val );
	printf("uint_PC_tof_shapeHistogram_divider_y_val    = %d \n",gl_sis3316_get_configuration_parameters->uint_PC_chargeHistogram_charge_select_flag );
	printf("\n");



	return 0;
}
