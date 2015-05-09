
#include "project_system_define.h"		//define LINUX or WIN

#include "get_configuration_parameters.h"


#ifdef LINUX
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif

#ifdef WIN
	#include <iostream>
	#include <cstdio>
	void usleep(unsigned int uint_usec) ;
	using namespace std;
#endif






sis3316_get_configuration_parameters::sis3316_get_configuration_parameters(void)
{
	unsigned int temp;
// default values

	this->uint_nof_events        = 1000;


	this->uint_raw_sample_length = 40;

	this->uint_gate1_start_index = 2;
	this->uint_gate1_length      = 1;

	this->uint_gate2_start_index = 4;
	this->uint_gate2_length      = 6;

	this->uint_gate3_start_index = 19;
	this->uint_gate3_length      = 1;

	this->uint_baseline_average_mode = 3;  // 64 samples




	this->uint_PC_shapeHistogram_divider_x_val        = 64; // quotient must be less or equal 511
	this->uint_PC_shapeHistogram_divider_y_val        = 8;  // quotient must be less or equal 511

	this->uint_PC_tof_shapeHistogram_divider_x_val    = 25000 ; // 40ms on 500 bins
	this->uint_PC_tof_shapeHistogram_divider_y_val    = 64;  // quotient must be less or equal 511

	this->uint_PC_tofHistogram_divider_val            = 25 ; // 100 ns binning

	this->uint_PC_chargeHistogram_divider_val         = 1 ; // quotient must be less or equal 64K
	this->uint_PC_chargeHistogram_charge_select_flag  = 1;

	this->calculateTriggerGateWindowLength( );
	this->calculatePileupWindowLength( );


}



int sis3316_get_configuration_parameters::read_parameter_file(char *path){
	FILE *fp;

	if(path == NULL){
		return -100;
	}

#ifdef LINUX
	fp = fopen(path, "r");
	if(fp == NULL){
		printf("open err\n");
		return -101;
	}

	rewind(fp);


#endif
printf("open OK\n");

size_t len = 0;      // this will be intialize the buffer
char * line = NULL ; // this will be intialize the buffer

int get_length = 0;
int key_word_length  ;
int parameter_value  ;


int comp_err ;

while ((get_length = getline(&line, &len, fp)) != EOF) {
    //printf("%s",line);
	//printf("len = %d\n",len);
	//printf("get_length = %d\n",get_length);
    //printf("sizeof(line) = %d\n",sizeof(line));


    key_word_length = this->getNofKewWordLetters (line, get_length);
	//printf("key_word_length = %d\n",key_word_length);
	//printf("get_length = %d\n",get_length);

	if ((key_word_length != 0) && ((get_length - key_word_length) > 0)) {
		parameter_value = getFollowingParameter(&line[key_word_length], get_length - key_word_length);
		//printf("parameter_value = %d\n",parameter_value);
		if (parameter_value >= 0) {
			//printf("parameter_value = %d\n",parameter_value);

// ********************************************************************************************************

	// TEXT_PARAMETER_NOF_EVENTS_EACH_BANK
			comp_err = strncmp( line, TEXT_PARAMETER_NOF_EVENTS_EACH_BANK, key_word_length) ;
			if(comp_err == 0)  {
				uint_nof_events = parameter_value ;
			}

	// PARAMETER_TRIGGER_THRESHOLD
			comp_err = strncmp( line, TEXT_PARAMETER_TRIGGER_THRESHOLD, key_word_length) ;
			if(comp_err == 0)  {
				uint_trigger_threshold = parameter_value ;
			}

	// PARAMETER_TRIGGER_P_VALUE
			comp_err = strncmp( line, TEXT_PARAMETER_TRIGGER_P_VALUE, key_word_length) ;
			if(comp_err == 0)  {
				uint_trigger_p_value = parameter_value ;
			}

	// PARAMETER_TRIGGER_G_VALUE
			comp_err = strncmp( line, TEXT_PARAMETER_TRIGGER_G_VALUE, key_word_length) ;
			if(comp_err == 0)  {
				uint_trigger_g_value = parameter_value ;
			}

	// PARAMETER_PRE_TRIGGER_DELAY
			comp_err = strncmp( line, TEXT_PARAMETER_PRE_TRIGGER_DELAY, key_word_length) ;
			if(comp_err == 0)  {
				uint_pre_trigger_delay = parameter_value ;
			}






	// PARAMETER_RAW_SAMPLE_LENGTH
			comp_err = strncmp( line, TEXT_PARAMETER_RAW_SAMPLE_LENGTH, key_word_length) ;
			if(comp_err == 0)  {
				uint_raw_sample_length = parameter_value ;
				//printf("strncmp OK\n");
			}


	// PARAMETER_GATE1_START_INDEX
			comp_err = strncmp( line, TEXT_PARAMETER_GATE1_START_INDEX, key_word_length) ;
			if(comp_err == 0)  {
				uint_gate1_start_index = parameter_value ;
				//printf("strncmp OK\n");
			}

	// TEXT_PARAMETER_GATE1_LENGTH
			comp_err = strncmp( line, TEXT_PARAMETER_GATE1_LENGTH, key_word_length) ;
			if(comp_err == 0)  {
				uint_gate1_length = parameter_value ;
			}


	// PARAMETER_GATE2_START_INDEX
			comp_err = strncmp( line, TEXT_PARAMETER_GATE2_START_INDEX, key_word_length) ;
			if(comp_err == 0)  {
				uint_gate2_start_index = parameter_value ;
				//printf("strncmp OK\n");
			}

	// TEXT_PARAMETER_GATE2_LENGTH
			comp_err = strncmp( line, TEXT_PARAMETER_GATE2_LENGTH, key_word_length) ;
			if(comp_err == 0)  {
				uint_gate2_length = parameter_value ;
			}

	// PARAMETER_GATE3_START_INDEX
			comp_err = strncmp( line, TEXT_PARAMETER_GATE3_START_INDEX, key_word_length) ;
			if(comp_err == 0)  {
				uint_gate3_start_index = parameter_value ;
				//printf("strncmp OK\n");
			}

	// TEXT_PARAMETER_GATE3_LENGTH
			comp_err = strncmp( line, TEXT_PARAMETER_GATE3_LENGTH, key_word_length) ;
			if(comp_err == 0)  {
				uint_gate3_length = parameter_value ;
			}


	// TEXT_PARAMETER_BASELINE_AVERAGE_MODE
			comp_err = strncmp( line, TEXT_PARAMETER_BASELINE_AVERAGE_MODE, key_word_length) ;
			if(comp_err == 0)  {
				uint_baseline_average_mode = parameter_value ;
			}



//********************************




	// TEXT_PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_X_VAL
			comp_err = strncmp( line, TEXT_PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_X_VAL, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_shapeHistogram_divider_x_val = parameter_value ;
			}

	// TEXT_PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_Y_VAL
			comp_err = strncmp( line, TEXT_PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_Y_VAL, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_shapeHistogram_divider_y_val = parameter_value ;
			}


	// TEXT_PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_X_VAL
			comp_err = strncmp( line, TEXT_PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_X_VAL, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_tof_shapeHistogram_divider_x_val = parameter_value ;
			}

	// TEXT_PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_Y_VAL
			comp_err = strncmp( line, TEXT_PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_Y_VAL, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_tof_shapeHistogram_divider_y_val = parameter_value ;
			}


	// TEXT_PARAMETER_PC_TOF_HISTOGRAM_DIVIDER_VAL
			comp_err = strncmp( line, TEXT_PARAMETER_PC_TOF_HISTOGRAM_DIVIDER_VAL, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_tofHistogram_divider_val = parameter_value ;
			}

	// TEXT_PARAMETER_PC_CHARGE_HISTOGRAM_DIVIDER_VAL
			comp_err = strncmp( line, TEXT_PARAMETER_PC_CHARGE_HISTOGRAM_DIVIDER_VAL, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_chargeHistogram_divider_val = parameter_value ;
			}

	// TEXT_PARAMETER_PC_CHARGE_HISTOGRAM_CHARGE_SELECT_FLAG
			comp_err = strncmp( line, TEXT_PARAMETER_PC_CHARGE_HISTOGRAM_CHARGE_SELECT_FLAG, key_word_length) ;
			if(comp_err == 0)  {
				uint_PC_chargeHistogram_charge_select_flag = parameter_value ;
			}




		}
	}


	this->calculateTriggerGateWindowLength( );
	this->calculatePileupWindowLength( );

}






	return 0;
}

/*******************************************************************************/

int sis3316_get_configuration_parameters::getNofKewWordLetters (char* line_buf, int max_length)
{
	int i;
	for (i=0;i<max_length;i++) {
		if (this->isKeyWord(line_buf[i])) {
			//printf("%c\n",line_buf[i]);
		}
		else {
			//printf("end i = %d\n",i);
			return i ;
		}
	}
	return 0 ;
}


/*******************************************************************************/

int sis3316_get_configuration_parameters::getFollowingParameter(char* line_buf, int max_length)
{
	int i;
	int temp_data=0;
	//printf("max_length = %d\n",max_length);
	for (i=0;i<max_length;i++) {
		if (this->isDigit(line_buf[i])) {
			//printf("%c\n",line_buf[i]);
			sscanf(&line_buf[i],"%d\t", &temp_data) ;
			//printf("return temp_data = %d\n",temp_data);
			return temp_data ;
		}
	}
	return -1 ;
}


/*******************************************************************************/

bool sis3316_get_configuration_parameters::isKeyWord (char c )
{
	return (('A' <= c && c <= 'Z') || (c == '_') || ('0' <= c && c <= '9') ) ;
}

bool sis3316_get_configuration_parameters::isUpperLetter (char c )
{
	return (('A' <= c && c <= 'Z') || (c == '_') ) ;
}

bool sis3316_get_configuration_parameters::isDigit (char c )
{
	return ('0' <= c && c <= '9') ;
}

/*******************************************************************************/
void sis3316_get_configuration_parameters::calculateTriggerGateWindowLength (void)
{
unsigned int temp ;
	// calculation of the Trigger_Gate_Window Length
		this->uint_trigger_gate_window_length      = 8 ;

		if (this->uint_gate1_length  != 0) {
			temp = this->uint_gate1_start_index + this->uint_gate1_length ;
			if (temp > this->uint_trigger_gate_window_length) {
				this->uint_trigger_gate_window_length = temp ;
			}
		}

		if (this->uint_gate2_length  != 0) {
			temp = this->uint_gate2_start_index + this->uint_gate2_length ;
			if (temp > this->uint_trigger_gate_window_length) {
				this->uint_trigger_gate_window_length = temp ;
			}
		}

		if (this->uint_gate3_length  != 0) {
			temp = this->uint_gate3_start_index + this->uint_gate3_length ;
			if (temp > this->uint_trigger_gate_window_length) {
				this->uint_trigger_gate_window_length = temp ;
			}
		}

		this->uint_trigger_gate_window_length      = this->uint_trigger_gate_window_length + 3 ; // must be 3 clocks longer than the accumulation
	return   ;
}


/*******************************************************************************/
void sis3316_get_configuration_parameters::calculatePileupWindowLength (void)
{
	// calculation of the Pileup_Window Length
		this->uint_pileup_window_length  = this->uint_trigger_gate_window_length    ;


		unsigned int uint_pileup_window_length;
		unsigned int uint_re_pileup_window_length;
		this->uint_re_pileup_window_length  = 8    ;
		  switch (uint_baseline_average_mode) {

		    case 0:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 8    ;
				break;

		    case 1:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 16    ;
				break;

		    case 2:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 32    ;
				break;

		    case 3:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 64    ;
				break;

		    case 4:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 128    ;
				break;

		    case 5:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 256    ;
				break;

		    case 6:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 512    ;
				break;

		    default:
				this->uint_re_pileup_window_length  = this->uint_re_pileup_window_length + 512    ;
				break;
		    }


	return   ;
}


