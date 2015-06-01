//============================================================================
// Name        : sis3316_external_trigger.cpp
// Author      : th
// Version     :
// Copyright   : Your copyright notice
// Description : sis3316_external_trigger in C++, Ansi-style
//============================================================================
#include "project_system_define.h"		//define LINUX or WINDOWS
#include "project_interface_define.h"		//define VME Interface or UDP Interface


#include <iostream>
using namespace std;







#define CERN_ROOT_PLOT

#ifdef CERN_ROOT_PLOT
	#include "rootIncludes.h"
#endif

#ifdef CERN_ROOT_PLOT

#include "sis3316_cern_root_class.h"
sis_root_graph *gl_graph_raw ;

#endif




/******************************************************************************************************************************/


// choose Interface
#include "vme_interface_class.h"

#ifdef PCI_VME_INTERFACE
	#include "sis1100w_vme_class.h"
	sis1100 *gl_vme_crate ;
#endif

#ifdef USB_VME_INTERFACE
	#include "sis3150w_vme_class.h"
	sis3150 *gl_vme_crate ;
#endif

#ifdef USB3_VME_INTERFACE
	#include "sis3153w_vme_class.h"
	sis3153 *gl_vme_crate ;
#endif




#ifdef ETHERNET_UDP_INTERFACE
	#include "sis3316_ethernet_access_class.h"
	sis3316_eth *gl_vme_crate ;

	#ifdef LINUX
		#include <sys/types.h>
		#include <sys/socket.h>
	#endif

	#ifdef WINDOWS
		#include <winsock2.h>
		#pragma comment(lib, "ws2_32.lib")
		//#pragma comment(lib, "wsock32.lib")
		typedef int socklen_t;
		char  gl_sis3316_ip_addr_string[32] ;

		long WinsockStartup()
		{
		  long rc;

		  WORD wVersionRequested;
		  WSADATA wsaData;
		  wVersionRequested = MAKEWORD(2, 1);

		  rc = WSAStartup( wVersionRequested, &wsaData );
		  return rc;
		}
	#endif

#endif







#ifdef LINUX
//	#include <sys/types.h>
	//#include <sys/socket.h>

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
	using namespace std;

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <tchar.h>
	#include <winsock2.h>
	#include <stdlib.h>
	#include <string.h>

	#pragma comment(lib, "ws2_32.lib")
	//#pragma comment(lib, "wsock32.lib")
	typedef int socklen_t;

	long WinsockStartup()
	{
	  long rc;

	  WORD wVersionRequested;
	  WSADATA wsaData;
	  wVersionRequested = MAKEWORD(2, 1);

	  rc = WSAStartup( wVersionRequested, &wsaData );
	  return rc;
	}

	#include "wingetopt.h"
	void usleep(unsigned int uint_usec) ;

#endif
/* ***************************************************************************************************************** */



#include "vme_interface_class.h"
#include "sis3316_ethernet_access_class.h"

#include "sis3316_class.h"


//handling cmd line input
char gl_cmd_ip_string[64];
unsigned int gl_cmd_ethernet_device_no = 0;

char gl_command[256];

#define MAX_NUMBER_LWORDS_64MBYTE			0x1000000       /* 64MByte */
//#define MAX_NUMBER_LWORDS_64MBYTE			0x1000000       /* 64MByte */


unsigned int gl_rblt_data[MAX_NUMBER_LWORDS_64MBYTE] ;
FILE *gl_FILE_DataEvenFilePointer ;




/*===========================================================================*/
/* Prototypes			                               		  			     */
/*===========================================================================*/

int WriteBufferHeaderCounterNofChannelToDataFile (unsigned int indentifier, unsigned int bank_loop_no, unsigned int channel_no, unsigned int nof_events, unsigned int event_length, unsigned int maw_length, unsigned int reserved);
int WriteEventsToDataFile (unsigned int* memory_data_array, unsigned int nof_write_length_lwords);


/* ***************************************************************************************************************** */
int main(int argc, char* argv[]) {
//int main() {

	int i_arg;
	unsigned int i, j;
	unsigned int data;

	char ch ;
	char ch_string[64] ;

	char char_messages[128] ;
	unsigned int nof_found_devices ;

	unsigned int auto_trigger_enable ;
	unsigned int stop_after_loop_counts ;
	unsigned int nof_events ;
	unsigned int trigger_gate_window_length ;
	unsigned int sample_length ;
	unsigned int sample_start_index ;

	unsigned int pre_trigger_delay ;
	unsigned int header_length ;
	unsigned int event_length ;
	unsigned int address_threshold ;
	unsigned int uint_DataEvent_OpenFlag ;
	unsigned int analog_ctrl_val ;
	unsigned int analog_offset_dac_val ;


	char filename[128]  ;
	unsigned int file_header_indentifier ;

	unsigned int clock_N1div, clock_HSdiv ;
	unsigned int iob_delay_value ;



	// Set Gain/Termination
	analog_ctrl_val = 0 ; // 5 V Range
	analog_ctrl_val = analog_ctrl_val + 0x01010101 ; // set to 2 V Range
	//analog_ctrl_val = analog_ctrl_val + 0x04040404 ; // disable 50 Ohm Termination

	//set ADC offsets (DAC) :
	analog_offset_dac_val = 0x8000 ; // middle
	//analog_offset_dac_val = 100 ; // 100 mV - 4.9 V if 5V range
	//analog_offset_dac_val = 13000 ; // 100 mV - 1.9 V if 2V range

//default values
	stop_after_loop_counts       =  10 ;
	nof_events                   =  100 ;    // each Bank
	auto_trigger_enable          =  0 ;

	trigger_gate_window_length   =  50000;
	sample_length                =  50000; // 200 us
	sample_start_index           =  0;
	pre_trigger_delay            =  4  ;  //

	uint_DataEvent_OpenFlag      =  0 ;  // save to file

	//strcpy(gl_sis3316_ip_addr_string,"212.60.16.200") ; // SIS3316 IP address
	strcpy(gl_cmd_ip_string,"192.168.1.100") ; // SIS3316 IP address


	/* Save command line into string "command" */
	  memset(gl_command,0,sizeof(gl_command));
	  // memset(startchoice,0,sizeof(startchoice));
	  for (i_arg=0;i_arg<argc;i_arg++) {
	      strcat(gl_command,argv[i_arg]);
	      strcat(gl_command," ");
	  }


	    if (argc > 1) {
		/* Save command line into string "command" */
		memset(gl_command,0,sizeof(gl_command));
		// memset(startchoice,0,sizeof(startchoice));
		for (i_arg=1;i_arg<argc;i_arg++) {
		    strcat(gl_command,argv[i_arg]);
		    strcat(gl_command," ");
		}
		printf("nof args %d    \n", argc );
		printf("gl_command %s    \n", gl_command );


		while ((ch = getopt(argc, argv, "?lhI:AL:E:B:F:")) != -1)
		  //printf("ch %c    \n", ch );
		  switch (ch) {
		    case 'I':
			sscanf(optarg,"%s", ch_string) ;
			printf("-I %s    \n", ch_string );
			strcpy(gl_cmd_ip_string,ch_string) ;
			break;

		    case 'A':
			auto_trigger_enable          =  1 ;
			break;



		    case 'L':
			sscanf(optarg,"%d",&data) ;
			//printf("-L %d    \n", data );
			if (data > 50000) {
				trigger_gate_window_length   =  50000;
				sample_length                =  50000; // 200 us
			}
			else {
				trigger_gate_window_length   =  data;
				sample_length                =  data; // 200 us
			}
			break;

		    case 'E':
			sscanf(optarg,"%d",&data) ;
			if (data > 100000) {
				nof_events                   = 100000 ;    // each Bank
			}
			else {
				nof_events                   = data ;    // each Bank
			}
			break;

		    case 'B':
			sscanf(optarg,"%d",&data) ;
			if (data > 100000) {
				stop_after_loop_counts                   = 100000 ;    //
			}
			else {
				stop_after_loop_counts                   = data ;    //
			}
			break;

		    case 'F':
			sscanf(optarg,"%d",&data) ;
			if (data == 1) {
				uint_DataEvent_OpenFlag = 1 ;
			}
			break;


		    case '?':
		    case 'h':
		    default:
			  //printf("Usage: %s  [-?h] [-I ip] [-A num]  ", argv[0]);
			  printf("Usage: %s  [-?h] [-I ip] [-L sample length] [-E number of events/loop] [-B number of loops] [-F save flag] ", argv[0]);
		      printf("   \n");
		      //printf("       -I string     SIS3316 IP Address                                          Default = %s\n", gl_cmd_ip_string);
		      //printf("       -A num        Ethernet Device Number (0 = ETH0, 1 = ETH1, .. , 3 = ETH3)  Default = %d\n", gl_cmd_ethernet_device_no);
		      printf("   \n");
			  printf("   -I string     SIS3316 IP Address       Default = %s\n", gl_cmd_ip_string);
		      printf("   -L num        sample length \n");
		      printf("   -E num        number of events/bank \n");
		      printf("   -A            autotrigger (keytrigger , sets number of events/bank = 1 ) \n");
		      printf("   -B num        number of loops (banks) \n");
		      printf("   -F num        save to file flag (0: no; 1: yes) \n");
		      printf("   \n");
		      printf("   -h            print this message\n");
		      printf("   \n");
		      exit(1);
		    }

	      } // if (argc > 2)

	      printf("gl_cmd_ip_string          = %s\n",gl_cmd_ip_string);
	      printf("gl_cmd_ethernet_device_no = %d\n",gl_cmd_ethernet_device_no);
	      usleep(1000);

	      if (auto_trigger_enable == 1) {
				nof_events  = 1 ;    // each Bank
	      }



	cout << "sis3316_external_trigger" << endl; // prints sis3316_external_trigger


/******************************************************************************************************************************/
/* CERN ROOT                                                                                                                  */
/******************************************************************************************************************************/







#ifdef ETHERNET_UDP_INTERFACE

	char  pc_ip_addr_string[32] ;
	char  sis3316_ip_addr_string[32] ;
	int return_code ;

	strcpy(sis3316_ip_addr_string, gl_cmd_ip_string) ; // SIS3316 IP address
	//strcpy(sis3316_ip_addr_string,"212.60.16.200") ; // SIS3316 IP address
	//strcpy(sis3316_ip_addr_string,"192.168.1.100") ; // SIS3316 IP address
	#ifdef WINDOWS
    //return_code = WSAStartup();
    return_code = WinsockStartup();
	#endif
	sis3316_eth *vme_crate = new sis3316_eth;
	// increase read_buffer size
	// SUSE needs following command as su: >sysctl -w net.core.rmem_max=33554432
	int	sockbufsize = 335544432 ; // 0x2000000
	return_code = vme_crate->set_UdpSocketOptionBufSize(sockbufsize) ;

	//strcpy(pc_ip_addr_string,"212.60.16.49") ; // Window example: secocnd Lan interface IP address is 212.60.16.49
	strcpy(pc_ip_addr_string,"") ; // empty if default Lan interface (Window: use IP address to bind in case of 2. 3. 4. .. LAN Interface)
	return_code = vme_crate->set_UdpSocketBindMyOwnPort( pc_ip_addr_string);

	vme_crate->set_UdpSocketSIS3316_IpAddress( sis3316_ip_addr_string);

	vme_crate->udp_reset_cmd();
	vme_crate->vme_A32D32_write(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000); // kill request and grant from other vme interface
	vme_crate->vme_A32D32_write(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x1); // request access to SIS3316 from UDP interface

    return_code = vme_crate->vme_A32D32_read(SIS3316_MODID,&data);
	printf("return_code = 0X%08x   Module ID = 0X%08x \n",return_code, data);
    return_code = vme_crate->vme_A32D32_read(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL,&data);
	printf("return_code = 0X%08x   ACCESS    = 0X%08x \n",return_code, data);

#endif

	// open Vme Interface device
	return_code = vme_crate->vmeopen ();  // open Vme interface
	vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface
    printf("get_vmeopen_messages = %s , nof_found_devices %d \n",char_messages, nof_found_devices);

/*************************************************************************************************************************/

	sis3316_adc  *sis3316_adc1 ;
	sis3316_adc1 = new sis3316_adc( vme_crate, 0x0);
	return_code = sis3316_adc1->register_read(SIS3316_MODID, &data);
	printf("sis3316_adc1->register_read: return_code = 0X%08x   data = 0X%08x \n",return_code, data);

	if(return_code != 0) {
		printf("Error: no connection to SIS3316 !! \n");
		return -1 ;
	}

/*************************************************************************************************************************/


#ifdef CERN_ROOT_PLOT

	int root_graph_x ;
	int root_graph_y ;
	int root_graph_x_size ;
	int root_graph_y_size ;
	char root_graph_text[80] ;

	root_graph_x_size = 1000 ;
	root_graph_y_size = 500 ;

	root_graph_x = 10 ;
	root_graph_y = 620 ;

	TApplication theApp("SIS3316 Application: Test", &argc, (char**)argv);
	strcpy(root_graph_text,"SIS3316 Graph: Raw data") ;
	gl_graph_raw      = new sis_root_graph(root_graph_text, root_graph_x, root_graph_y, root_graph_x_size, root_graph_y_size) ;


	if (sis3316_adc1->adc_125MHz_flag == 0) {
		gl_graph_raw->sis3316_set_14bit_Yaxis() ;// 250 MHz
	}
	else {
		gl_graph_raw->sis3316_set_16bit_Yaxis() ;// 125 MHz
	}

#endif

/*************************************************************************************************************************/



/*  configure sis3316 */

	return_code = sis3316_adc1->register_write(SIS3316_KEY_RESET, 0);
	return_code = sis3316_adc1->register_write(SIS3316_KEY_DISARM, 0);

/******************************************/

	if (sis3316_adc1->adc_125MHz_flag == 0) {
		// 250  MHz
		file_header_indentifier      =  0  ;
	}
	else {
		// 125 MHz
		file_header_indentifier      =  1  ;
	}

/******************************************/
	// sample clock setup, necessary (only) one time after Powerup

	if (sis3316_adc1->adc_125MHz_flag == 0) {
		// 250  MHz
		printf("250 !! \n");
		clock_N1div      =  4  ;
		clock_HSdiv      =  5  ;
		//iob_delay_value  =  0x48   ; // ADC FPGA version A_0250_0003
		iob_delay_value  =  0x1008 ; // ADC FPGA version A_0250_0004 and higher
	}
	else {
		// 125 MHz
		printf("125 !! \n");
		clock_N1div      =  8  ;
		clock_HSdiv      =  5  ;
		//iob_delay_value  =  0x7F   ; // ADC FPGA version A_0250_0003
		iob_delay_value  =  0x1020 ; // ADC FPGA version A_0250_0004 and higher
	}
	sis3316_adc1->change_frequency_HSdiv_N1div(0, clock_HSdiv, clock_N1div) ;
	usleep(1000) ;

	// enable ADC chip outputs
	for (i=0;i<4;i++) {
		return_code = sis3316_adc1->register_write((i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x01000000 ); // enable ADC chip outputs
	}


	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	usleep(1000) ;
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	usleep(1000) ;


/******************************************/

	// Select LEMO Output "Co"
	data = 0x1 ; // Select Sample Clock
	return_code = sis3316_adc1->register_write(SIS3316_LEMO_OUT_CO_SELECT_REG, data ); //

	// Enable LEMO Output "TO"
	data = 0xffff ; // Select all triggers
	return_code = sis3316_adc1->register_write(SIS3316_LEMO_OUT_TO_SELECT_REG, data ); //


/******************************************/

	data = 0x0 ;
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_CHANNEL_HEADER_REG, data); //
	data = 0x00400000 ;
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_CHANNEL_HEADER_REG, data ); //
	data = 0x00800000 ;
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_CHANNEL_HEADER_REG, data ); //
	data = 0x00C00000 ;
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_CHANNEL_HEADER_REG, data ); //

/******************************************/


/******************************************/

	// Gain/Termination
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_ANALOG_CTRL_REG, analog_ctrl_val); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_ANALOG_CTRL_REG, analog_ctrl_val ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_ANALOG_CTRL_REG, analog_ctrl_val ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_ANALOG_CTRL_REG, analog_ctrl_val ); //

	//
	//  set ADC offsets (DAC)
	for (i=0;i<4;i++) { // over all 4 ADC-FPGAs
		return_code = sis3316_adc1->register_write((i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG, 0x80000000 + 0x8000000 +  0xf00000 + 0x1);  // set internal Reference
		usleep(1); //unsigned int uint_usec
		return_code = sis3316_adc1->register_write((i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG, 0x80000000 + 0x2000000 +  0xf00000 + ((analog_offset_dac_val & 0xffff) << 4) );  //
		//return_code = sis3316_adc1->register_write((i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG, 0x80000000 + 0x2000000 +  0xf00000 + ((15000 & 0xffff) << 4) );  //
		return_code = sis3316_adc1->register_write((i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG, 0xC0000000 );  //
		usleep(1); //unsigned int uint_usec
	}


/******************************************/

	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length

	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length

/******************************************/


	if (pre_trigger_delay > 2042) {
		pre_trigger_delay  = 2042 ;
	}
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //

/******************************************/

	// Enable LEMO Input "TI" as Trigger External Trigger
    if (auto_trigger_enable == 1) {
    	data = 0x0 ; // Not Enable Nim Input "TI"
    }
    else {
    	data = 0x10 ; // Enable Nim Input "TI"
    }
	return_code = sis3316_adc1->register_write(SIS3316_NIM_INPUT_CONTROL_REG, data ); //


/******************************************/

	//  Event Configuration

	data = 0x08080808 ; //  external trigger
	//data = 0x00080008 ; //  external trigger only ch1, 3, 5, 7 ..
	//data = 0x00000008 ; //  external trigger only ch1, 5, 9, 13
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data ); //  r

/******************************************/
	// data format

	header_length = 3;
	data = 0 ;

	data = data + (data << 8) + (data << 16) + (data << 24);
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_DATAFORMAT_CONFIG_REG, data );
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_DATAFORMAT_CONFIG_REG, data );
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_DATAFORMAT_CONFIG_REG, data );
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_DATAFORMAT_CONFIG_REG, data );

/******************************************/
	event_length = header_length + (sample_length / 2)  ;

	//address_threshold = 200;
	address_threshold = (nof_events * event_length) - 1 ;  //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG, address_threshold ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH5_8_ADDRESS_THRESHOLD_REG, address_threshold); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH9_12_ADDRESS_THRESHOLD_REG, address_threshold ); //
	return_code = sis3316_adc1->register_write(SIS3316_ADC_CH13_16_ADDRESS_THRESHOLD_REG, address_threshold ); //

/******************************************************************************************/

	if (sample_length != 0) {
		gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y
	}
		//gl_graph_raw->sis3316_draw_XYaxis (20); // clear and draw X/Y



/******************************************************************************************/

	unsigned int poll_counter;
	unsigned int plot_counter;
	unsigned int loop_counter;
	unsigned int timeout_counter;
	unsigned int bank1_armed_flag;
	plot_counter=0;
	loop_counter=0;
	timeout_counter = 0 ;

	printf("Start Multievent \n");

	unsigned int bank_buffer_counter ;
	bank_buffer_counter = 0 ;

	// enbale external (global) functions
	data = 0x100 ; // enable "external Trigger function" (NIM In, if enabled and VME key write)
	data = data + 0x400 ; // enable "external Timestamp clear function" (NIM In, if enabled and VME key write)
	return_code = sis3316_adc1->register_write(SIS3316_ACQUISITION_CONTROL_STATUS, data );

	// Clear Timestamp  */
	return_code = sis3316_adc1->register_write(SIS3316_KEY_TIMESTAMP_CLEAR , 0);  //

	// Start Readout Loop  */
	return_code = sis3316_adc1->register_write(SIS3316_KEY_DISARM_AND_ARM_BANK1 , 0);  //  Arm Bank1
	bank1_armed_flag = 1; // start condition
	printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");


	unsigned int ch_event_counter = 0;


	gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y


	do {
	    if (auto_trigger_enable == 1) {
	    	return_code = sis3316_adc1->register_write(SIS3316_KEY_TRIGGER , 0);  //  Soft key trigger
	    }

		poll_counter = 0 ;
		sis3316_adc1->register_read(SIS3316_ACQUISITION_CONTROL_STATUS, &data);
		do {
			poll_counter++;
			if (poll_counter == 100) {
				gSystem->ProcessEvents();  // handle GUI events
				poll_counter = 0 ;
    		}
			sis3316_adc1->register_read(SIS3316_ACQUISITION_CONTROL_STATUS, &data);
			//usleep(500000); //500ms
			//printf("in Loop:   SIS3316_ACQUISITION_CONTROL_STATUS = 0x%08x     \n", data);
		} while ((data & 0x80000) == 0x0) ; // Address Threshold reached ?


		if (bank1_armed_flag == 1) {
			return_code = sis3316_adc1->register_write(SIS3316_KEY_DISARM_AND_ARM_BANK2 , 0);  //  Arm Bank2
			bank1_armed_flag = 0; // bank 2 is armed
			printf("SIS3316_KEY_DISARM_AND_ARM_BANK2 \n");
		}
		else {
			return_code = sis3316_adc1->register_write(SIS3316_KEY_DISARM_AND_ARM_BANK1 , 0);  //  Arm Bank1
			bank1_armed_flag = 1; // bank 1 is armed
			printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");
		}

		// file write
		if (uint_DataEvent_OpenFlag == 1) {   ; // Open
			sprintf(filename,"sis3316_test_data_%d.dat",bank_buffer_counter ) ;
			printf("%s\n",filename) ;
			gl_FILE_DataEvenFilePointer = fopen(filename,"wb") ;
			//gl_FILE_DataEvenFilePointer = fopen("sis3316_test_data.dat","wb") ;
		}


		gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y
		unsigned int i_ch;
		unsigned int got_nof_32bit_words;
		for (i_ch=0; i_ch<16; i_ch++) {
		//for (i_ch=0; i_ch<1; i_ch++) {
			// read channel events
			return_code = sis3316_adc1->read_DMA_Channel_PreviousBankDataBuffer(bank1_armed_flag /*bank2_read_flag*/, i_ch /* 0 to 15 */, nof_events * event_length, &got_nof_32bit_words, gl_rblt_data ) ;
			//return_code = sis3316_adc1->read_MBLT64_Channel_PreviousBankDataBuffer(bank1_armed_flag /*bank2_read_flag*/, i_ch /* 0 to 15 */,  &got_nof_32bit_words, gl_rblt_data ) ;
			printf("read_MBLT64_Channel_PreviousBankDataBuffer: i_ch %d  got_nof_32bit_words = 0x%08x  return_code = 0x%08x\n",i_ch,  got_nof_32bit_words, return_code);
			//if (return_code != 0) {
				//printf("read_MBLT64_Channel_PreviousBankDataBuffer: return_code = 0x%08x\n", return_code);
				//gl_stopReq = TRUE;
			//}
			ch_event_counter = (got_nof_32bit_words  / event_length) ;
			if (ch_event_counter > 0) {

				// plot events
				for (i=0; i<ch_event_counter; i++) {
					//if (i<10) { // plot ony 10. event
					if (i==0) { // plot ony 1. event
						gl_graph_raw->sis3316_draw_chN (sample_length, &gl_rblt_data[i*(event_length) + header_length], i_ch); //
					}
				}

				if (uint_DataEvent_OpenFlag == 1) {   ; // Open
					WriteBufferHeaderCounterNofChannelToDataFile (file_header_indentifier, bank_buffer_counter, i_ch, ch_event_counter, event_length , 0 /* maw length */, 0 /* reserved */ ) ;
					WriteEventsToDataFile (gl_rblt_data, got_nof_32bit_words)  ;
    			}


			}
		}
		if (uint_DataEvent_OpenFlag == 1) {   ; // Open
			fclose(gl_FILE_DataEvenFilePointer);
		}

		loop_counter++;
		bank_buffer_counter++;

		printf("bank_buffer_counter = %d     \n",bank_buffer_counter);
		printf("ch_event_counter    = %d     \n", ch_event_counter);
		gSystem->ProcessEvents();  // handle GUI events

	//} while((gl_stopReq == FALSE) );
	} while((loop_counter < stop_after_loop_counts) );


	return_code = sis3316_adc1->register_write(SIS3316_KEY_DISARM , 0);  //

#ifdef raus
	BOOL gl_stopReq = FALSE;
	gl_stopReq = FALSE;
	do {
		gSystem->ProcessEvents();  // handle GUI events
	} while((gl_stopReq == FALSE) );
#endif

	printf("sampling finished   \n");



	do {
		gSystem->ProcessEvents();  // handle GUI events

	} while (1) ;



	return 0;
}




/***********************************************************************************************************************************************/
/***********************************************************************************************************************************************/
/***********************************************************************************************************************************************/

#define FILE_FORMAT_EVENT_HEADER        	0xDEADBEEF
#define FILE_FORMAT_EOF_TRAILER        		0x0E0F0E0F
int WriteBufferHeaderCounterNofChannelToDataFile (unsigned int indentifier, unsigned int bank_loop_no, unsigned int channel_no, unsigned int nof_events, unsigned int event_length, unsigned int maw_length, unsigned int reserved)
{
int written ;
int data ;
  //header
	data = FILE_FORMAT_EVENT_HEADER ;
    written=fwrite(&data,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
	written+=fwrite(&indentifier,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
   //Buffer No
    written+=fwrite(&bank_loop_no,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    //channel  No
    written+=fwrite(&channel_no,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    //nof events
    written+=fwrite(&nof_events,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    //event length
    written+=fwrite(&event_length,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    // maw buffer length
    written+=fwrite(&maw_length,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
    written+=fwrite(&reserved,0x4,0x1,gl_FILE_DataEvenFilePointer); // write one  uint word
 	return written;

}


//---------------------------------------------------------------------------
int WriteEventsToDataFile (unsigned int* memory_data_array, unsigned int nof_write_length_lwords)
{
int nof_write_elements ;
int written ;
//int data ;
//char messages_buffer[256] ;

// gl_uint_DataEvent_RunFile_EvenSize : length

		nof_write_elements = nof_write_length_lwords ;
		written=fwrite(memory_data_array,0x4,nof_write_elements,gl_FILE_DataEvenFilePointer); // write 3 uint value
		//gl_uint_DataEvent_LWordCounter = gl_uint_DataEvent_LWordCounter + written  ;
		if(nof_write_elements != written) {
    		printf ("Data File Write Error in  WriteEventToDataFile()  \n");
 		 }

 	return 0;

}

/***********************************************************************************************************************************************/
/***********************************************************************************************************************************************/
