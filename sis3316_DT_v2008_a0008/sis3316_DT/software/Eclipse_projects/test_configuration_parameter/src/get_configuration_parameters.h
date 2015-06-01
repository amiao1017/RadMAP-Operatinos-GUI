/*
 * get_configuration_parameters.h
 *
 *  Created on: 02.03.2014
 *      Author: th
 */

#ifndef GET_CONFIGURATION_PARAMETERS_H_
#define GET_CONFIGURATION_PARAMETERS_H_


#define TEXT_PARAMETER_NOF_EVENTS_EACH_BANK 		"PARAMETER_NOF_EVENTS_EACH_BANK"

#define TEXT_PARAMETER_TRIGGER_THRESHOLD   			"PARAMETER_TRIGGER_THRESHOLD"
#define TEXT_PARAMETER_TRIGGER_P_VALUE   			"PARAMETER_TRIGGER_P_VALUE"
#define TEXT_PARAMETER_TRIGGER_G_VALUE   			"PARAMETER_TRIGGER_G_VALUE"

#define TEXT_PARAMETER_PRE_TRIGGER_DELAY   			"PARAMETER_PRE_TRIGGER_DELAY"


#define TEXT_PARAMETER_RAW_SAMPLE_LENGTH   			"PARAMETER_RAW_SAMPLE_LENGTH"

#define TEXT_PARAMETER_GATE1_START_INDEX   			"PARAMETER_GATE1_START_INDEX"
#define TEXT_PARAMETER_GATE1_LENGTH   				"PARAMETER_GATE1_LENGTH"

#define TEXT_PARAMETER_GATE2_START_INDEX   			"PARAMETER_GATE2_START_INDEX"
#define TEXT_PARAMETER_GATE2_LENGTH   				"PARAMETER_GATE2_LENGTH"

#define TEXT_PARAMETER_GATE3_START_INDEX   			"PARAMETER_GATE3_START_INDEX"
#define TEXT_PARAMETER_GATE3_LENGTH   				"PARAMETER_GATE3_LENGTH"

#define TEXT_PARAMETER_BASELINE_AVERAGE_MODE   		"PARAMETER_BASELINE_AVERAGE_MODE"



#define TEXT_PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_X_VAL			"PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_X_VAL"
#define TEXT_PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_Y_VAL			"PARAMETER_PC_SHAPE_HISTOGRAM_DIVIDER_Y_VAL"

#define TEXT_PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_X_VAL		"PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_X_VAL"
#define TEXT_PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_Y_VAL		"PARAMETER_PC_TOF_SHAPE_HISTOGRAM_DIVIDER_Y_VAL"

#define TEXT_PARAMETER_PC_TOF_HISTOGRAM_DIVIDER_VAL				"PARAMETER_PC_TOF_HISTOGRAM_DIVIDER_VAL"

#define TEXT_PARAMETER_PC_CHARGE_HISTOGRAM_DIVIDER_VAL			"PARAMETER_PC_CHARGE_HISTOGRAM_DIVIDER_VAL"
#define TEXT_PARAMETER_PC_CHARGE_HISTOGRAM_CHARGE_SELECT_FLAG	"PARAMETER_PC_CHARGE_HISTOGRAM_CHARGE_SELECT_FLAG"












class sis3316_get_configuration_parameters
{

private:
	int getNofKewWordLetters (char* line_buf, int max_length);
	int getFollowingParameter(char* line_buf, int max_length);


	bool isKeyWord (char c ) ;

	bool isUpperLetter (char c );
	bool isDigit (char c );

	void calculateTriggerGateWindowLength (void);
	void calculatePileupWindowLength (void);


public:

	unsigned int uint_nof_events ;
	unsigned int uint_trigger_threshold ;
	unsigned int uint_trigger_p_value ;
	unsigned int uint_trigger_g_value ;

	unsigned int uint_pre_trigger_delay ;
	unsigned int uint_raw_sample_length ;
	unsigned int uint_trigger_gate_window_length;
	unsigned int uint_pileup_window_length;
	unsigned int uint_re_pileup_window_length;



	unsigned int uint_gate1_start_index ;
	unsigned int uint_gate1_length ;
	unsigned int uint_gate2_start_index ;
	unsigned int uint_gate2_length ;
	unsigned int uint_gate3_start_index ;
	unsigned int uint_gate3_length ;

	unsigned int uint_baseline_average_mode ;


	unsigned int uint_PC_shapeHistogram_divider_x_val ;
	unsigned int uint_PC_shapeHistogram_divider_y_val ;

	unsigned int uint_PC_tof_shapeHistogram_divider_x_val ;
	unsigned int uint_PC_tof_shapeHistogram_divider_y_val ;

	unsigned int uint_PC_tofHistogram_divider_val ;

	unsigned int uint_PC_chargeHistogram_divider_val ;
	unsigned int uint_PC_chargeHistogram_charge_select_flag;




public:
	sis3316_get_configuration_parameters(void);

	int read_parameter_file(char *path);

	~sis3316_get_configuration_parameters(void);
};






#endif /* GET_CONFIGURATION_PARAMETERS_H_ */
