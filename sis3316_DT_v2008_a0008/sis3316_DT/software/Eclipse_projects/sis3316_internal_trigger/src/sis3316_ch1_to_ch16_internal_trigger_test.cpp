/***************************************************************************/
/*                                                                         */
/*  Filename: sis3316_ch1_to_ch16_internal_trigger_test.cpp                */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 28.01.2013                                       */
/*  last modification:    12.03.2014                                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*  If Ethernet-UDP interface is used:                                     */
/*  Prepare IP-address on PC:                                              */
/*  for example:                                                           */
/*   Window_PC: arp -s 212.60.16.200  00-00-56-31-6x-xx                    */
/*   xxx: Serial Number                                                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*  SIS  Struck Innovative Systeme GmbH                                    */
/*                                                                         */
/*  Harksheider Str. 102A                                                  */
/*  22399 Hamburg                                                          */
/*                                                                         */
/*  Tel. +49 (0)40 60 87 305 0                                             */
/*  Fax  +49 (0)40 60 87 305 20                                            */
/*                                                                         */
/*  http://www.struck.de                                                   */
/*                                                                         */
/*  � 2014                                                                 */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

#include "project_system_define.h"		//define LINUX or WINDOWS
#include "project_interface_define.h"   //define Interface (sis1100/sis310x, sis3150usb or Ethnernet UDP)










#define CERN_ROOT_PLOT

#ifdef CERN_ROOT_PLOT
	#include "rootIncludes.h"
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


/******************************************************************************************************/





#include "sis3316_class.h"
//sis3316_adc *gl_sis3316_adc1 ;

#define MAX_NOF_SIS3316_ADCS			1
//#define BROADCAST_BASE_ADDR				0x40000000      
#define FIRST_MODULE_BASE_ADDR			0x30000000      
#define MODULE_BASE_OFFSET				0x01000000      

#include "sis3316.h"

/******************************************************************************************************/

#ifdef CERN_ROOT_PLOT

#include "sis3316_cern_root_class.h"  
sis_root_graph *gl_graph_raw ;

sis_root_channel_energy_histogram *gl_channel_energy_histogram ;

sis_root_graph_maw *gl_graph_maw ;
sis_root_intensity_graph *gl_intensity_raw ;

unsigned int gl_graph_zoom_raw_draw_length ;
unsigned int gl_graph_zoom_raw_draw_bin_offset ;

#endif
/******************************************************************************************************/


/*===========================================================================*/
/* Globals					  			     */
/*===========================================================================*/
#define MAX_NUMBER_LWORDS_64MBYTE			0x1000000       /* 64MByte */
//#define MAX_NUMBER_LWORDS_64MBYTE			0x1000000       /* 64MByte */

unsigned int gl_rblt_data[MAX_NUMBER_LWORDS_64MBYTE] ;

FILE *gl_FILE_DataEvenFilePointer           ;


char gl_cmd_ip_string[64];
char gl_command[256];

/*===========================================================================*/
/* Prototypes			                               		  			     */
/*===========================================================================*/

int WriteBufferHeaderCounterNofChannelToDataFile (unsigned int indentifier, unsigned int bank_loop_no, unsigned int channel_no, unsigned int nof_events, unsigned int event_length, unsigned int maw_length, unsigned int reserved);
int WriteEventsToDataFile (unsigned int* memory_data_array, unsigned int nof_write_length_lwords);




//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char* argv[])
{
	int i_arg;

	char ch ;
	char ch_string[64] ;

CHAR char_messages[128];
UINT nof_found_devices ;

int return_code ;
unsigned int module_base_addr   ;
unsigned int data;
unsigned int i ;
unsigned int loop_counter;
unsigned int bank_buffer_counter ;

unsigned int uint_channel_event_counter[16];


 
unsigned int got_nof_32bit_words;
unsigned int sample_length;
unsigned int sample_start_index;
unsigned int trigger_gate_window_length;
unsigned int address_threshold;
unsigned int nof_events;
unsigned int pre_trigger_delay ;
unsigned int bank1_armed_flag ;
unsigned int poll_counter ;

unsigned int sis3316_not_OK;

unsigned int ch_event_counter;

unsigned int event_length;
unsigned int header_length;


unsigned int iob_delay_value ;
unsigned int p_val ;
unsigned int g_val ;
unsigned int trigger_threshold_value ;

unsigned int uint_pileup ;
unsigned int uint_re_pileup ;
unsigned int internal_fir_trigger_delay ;

unsigned int i_adc_fpga ;
unsigned int i_ch ;

unsigned int maw_test_buffer_length ;
unsigned int maw_test_buffer_delay ;

unsigned int header_accu_6_values_enable_flag ;
unsigned int header_accu_2_values_enable_flag ;
unsigned int header_maw_3_values_enable_flag ;
unsigned int maw_test_buffer_enable_flag ;

unsigned int header_maw_3_values_offset ;
unsigned int header_accu_2_values_offset ;

char filename[128]  ;

unsigned int uint_DataEvent_OpenFlag ;
unsigned int file_header_indentifier ;
unsigned int clock_N1div, clock_HSdiv ;

unsigned int stop_after_loop_counts ;

unsigned int analog_ctrl_val ;
unsigned int analog_offset_dac_val ;




/******************************************************************************************/
/*                                                                                        */
/*  SIS3316 program parameter                                                             */
/*                                                                                        */
/******************************************************************************************/

	// VME module base address
	module_base_addr = FIRST_MODULE_BASE_ADDR ;

	//stop_after_loop_counts       = 10;    	// 0: endless  
	stop_after_loop_counts       = 0;    	// 0: endless  
	//nof_events                   = 1000 ;    // each Bank
	nof_events                   = 10 ;    // each Bank

	// file write
	uint_DataEvent_OpenFlag      =  1 ;  // not save to file
	//uint_DataEvent_OpenFlag      =  1 ;  // save to file

// Set Gain/Termination
	analog_ctrl_val = 0 ; // 5 V Range
	analog_ctrl_val = analog_ctrl_val + 0x01010101 ; // set to 2 V Range
	//analog_ctrl_val = analog_ctrl_val + 0x04040404 ; // disable 50 Ohm Termination

//set ADC offsets (DAC) :
	analog_offset_dac_val = 0x8000 ; // middle
	//analog_offset_dac_val = 100 ; // 100 mV - 4.9 V if 5V range
	//analog_offset_dac_val = 13000 ; // 100 mV - 1.9 V if 2V range



// internal trigger generation
	p_val = 6 ;
	g_val = 6 ;

	pre_trigger_delay = p_val + (p_val>>1) + g_val + 16 + 16;  // 16 is additional delay for 50% CFD trigger --> see edge at index 10
	internal_fir_trigger_delay             =  0  ;  //  
	pre_trigger_delay = pre_trigger_delay + (2 * internal_fir_trigger_delay) ;   

	trigger_threshold_value =  100  * p_val ; // 2V Range -> 1 bin = 122 uV
	// 100 --> 2V Range --> 12.2 mV

// data format 
	header_accu_6_values_enable_flag = 0 ;
	header_accu_2_values_enable_flag = 0 ;
	header_maw_3_values_enable_flag  = 0 ;

	maw_test_buffer_enable_flag = 0 ;
	maw_test_buffer_length     = 0 ;

// trigger_gate_window_length and sample length 
	trigger_gate_window_length = 1000;	
	sample_length              = 1000;	
	sample_start_index         = 0;	


// pileups 
	uint_pileup    = 100 ;  //  
	uint_re_pileup = 100 ;  //  



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
		for (i_arg=1;i_arg<argc;i_arg++) {
		    strcat(gl_command,argv[i_arg]);
		    strcat(gl_command," ");
		}
		printf("nof args %d    \n", argc );
		printf("gl_command %s    \n", gl_command );


		while ((ch = getopt(argc, argv, "?lhI:L:E:B:F:")) != -1)
		  //printf("ch %c    \n", ch );
		  switch (ch) {
		    case 'I':
			sscanf(optarg,"%s", ch_string) ;
			printf("-I %s    \n", ch_string );
			strcpy(gl_cmd_ip_string,ch_string) ;
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
		      printf("   \n");
			  printf("   -I string     SIS3316 IP Address       Default = %s\n", gl_cmd_ip_string);
		      printf("   -L num        sample length \n");
		      printf("   -E num        number of events/bank \n");
		      printf("   -B num        number of loops (banks) \n");
		      printf("   -F num        save to file flag (0: no; 1: yes) \n");
		      printf("   \n");
		      printf("   -h            print this message\n");
		      printf("   \n");
		      exit(1);
		    }

	      } // if (argc > 2)

	      printf("gl_cmd_ip_string          = %s\n",gl_cmd_ip_string);
	      usleep(1000);






/******************************************************************************************************************************/
/* VME Master Create, Open and Setup                                                                                          */
/******************************************************************************************************************************/
 

#ifdef PCI_VME_INTERFACE
	// create SIS1100/SIS310x vme interface device
	//sis1100 *vme_crate = new sis1100(0);
	gl_vme_crate = new sis1100(0);

#endif

#ifdef USB_VME_INTERFACE
USHORT idVendor;
USHORT idProduct;
USHORT idSerNo;
USHORT idFirmwareVersion;
USHORT idDriverVersion;
	// create SIS3150USB vme interface device
	//sis3150 *vme_crate = new sis3150(0);
	gl_vme_crate = new sis3150(0);
#endif

#ifdef USB3_VME_INTERFACE
USHORT idVendor;
USHORT idProduct;
USHORT idSerNo;
USHORT idDriverVersion;
USHORT idFxFirmwareVersion;
USHORT idFpgaFirmwareVersion;
	// create SIS3153USB vme interface device
	//sis3153 *vme_crate = new sis3153(0);
	gl_vme_crate = new sis3153(0);
#endif


#ifdef ETHERNET_UDP_INTERFACE

	char  pc_ip_addr_string[32] ;
	char  sis3316_ip_addr_string[32] ;

	strcpy(sis3316_ip_addr_string, gl_cmd_ip_string) ; // SIS3316 IP address
	//strcpy(sis3316_ip_addr_string,"192.168.1.100") ; // SIS3316 IP address
	#ifdef WINDOWS
    //return_code = WSAStartup();
    return_code = WinsockStartup();
	#endif

	//sis3316_eth *vme_crate = new sis3316_eth;
	gl_vme_crate = new sis3316_eth;

	// increase read_buffer size
	// SUSE needs following command as su: >sysctl -w net.core.rmem_max=33554432
	int	sockbufsize = 335544432 ; // 0x2000000
	return_code = gl_vme_crate->set_UdpSocketOptionBufSize(sockbufsize) ;

	//strcpy(pc_ip_addr_string,"212.60.16.49") ; // Window example: secocnd Lan interface IP address is 212.60.16.49
	strcpy(pc_ip_addr_string,"") ; // empty if default Lan interface (Window: use IP address to bind in case of 2. 3. 4. .. LAN Interface)
	return_code = gl_vme_crate->set_UdpSocketBindMyOwnPort( pc_ip_addr_string);

	gl_vme_crate->set_UdpSocketSIS3316_IpAddress( sis3316_ip_addr_string);

	gl_vme_crate->udp_reset_cmd();
	gl_vme_crate->vme_A32D32_write(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000); // kill request and grant from other vme interface
	gl_vme_crate->vme_A32D32_write(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x1); // request access to SIS3316 from UDP interface

    return_code = gl_vme_crate->vme_A32D32_read(SIS3316_MODID,&data);
	printf("return_code = 0X%08x   Module ID = 0X%08x \n",return_code, data);
    return_code = gl_vme_crate->vme_A32D32_read(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL,&data);
	printf("return_code = 0X%08x   ACCESS    = 0X%08x \n",return_code, data);

#endif


// open Vme Interface device
//	gl_vme_crate = vme_crate ; 

	return_code = gl_vme_crate->vmeopen ();  // open Vme interface
	gl_vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface
	printf("\n%s    (found %d vme interface device[s])\n\n",char_messages, nof_found_devices);

	if(return_code != 0x0) {
		printf("ERROR: gl_vme_crate->vmeopen: return_code = 0x%08x\n\n", return_code);
		return -1 ;
	}


/******************************************************************************************/
// additional Vme interface device informations
#ifdef USB_VME_INTERFACE
	gl_vme_crate->get_device_informations (&idVendor, &idProduct, &idSerNo, &idFirmwareVersion, &idDriverVersion);  //  
	printf("idVendor:           %04X\n",idVendor);
	printf("idProduct:          %04X\n",idProduct);
	printf("idSerNo:            %d\n",idSerNo);
	printf("idFirmwareVersion:  %04X\n",idFirmwareVersion);
	printf("idDriverVersion:    %04X\n",idDriverVersion);
	printf("\n\n");

#endif
/******************************************************************************************/



/******************************************************************************************/

	return_code = gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_MODID, &data);  
	printf("vme_A32D32_read: data = 0x%08x     return_code = 0x%08x\n", data, return_code);


	// kill request and grant from vme interface (in case of use using etehrnet interface)
	gl_vme_crate->vme_A32D32_write(module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000);
	// arbitrate
	gl_vme_crate->vme_A32D32_write(module_base_addr + SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 1);



	sis3316_not_OK = 0 ;
	if (return_code == 0) {
		printf("SIS3316_MODID                    = 0x%08x\n\n", data);

		gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
		gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
		gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits
		gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0x400 ); // Clear Link Error Latch bits

		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH1_4_FIRMWARE_REG, &data);  
		printf("SIS3316_ADC_CH1_4_FIRMWARE_REG   = 0x%08x \n", data);
		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH5_8_FIRMWARE_REG, &data);  
		printf("SIS3316_ADC_CH5_8_FIRMWARE_REG   = 0x%08x \n", data);
		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH9_12_FIRMWARE_REG, &data);  
		printf("SIS3316_ADC_CH9_12_FIRMWARE_REG  = 0x%08x \n", data);
		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH13_16_FIRMWARE_REG, &data);  
		printf("SIS3316_ADC_CH13_16_FIRMWARE_REG = 0x%08x \n\n", data);

		gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, 0xE0E0E0E0);  // clear error Latch bits 
		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, &data);  
		printf("SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS: data = 0x%08x     return_code = 0x%08x\n", data, return_code);
		if (data != 0x18181818) { sis3316_not_OK = 1 ; }

		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH1_4_STATUS_REG, &data);  
		printf("SIS3316_ADC_CH1_4_STATUS_REG     = 0x%08x \n", data);
		if (data != 0x130018) { sis3316_not_OK = 1 ; }

		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH5_8_STATUS_REG, &data);  
		printf("SIS3316_ADC_CH5_8_STATUS_REG     = 0x%08x \n", data);
		if (data != 0x130018) { sis3316_not_OK = 1 ; }

		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH9_12_STATUS_REG, &data);  
		printf("SIS3316_ADC_CH9_12_STATUS_REG    = 0x%08x \n", data);
		if (data != 0x130018) { sis3316_not_OK = 1 ; }

		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ADC_CH13_16_STATUS_REG, &data);  
		printf("SIS3316_ADC_CH13_16_STATUS_REG   = 0x%08x \n\n", data);
		if (data != 0x130018) { sis3316_not_OK = 1 ; }

	}
	else {
		printf("SIS3316_MODID                  = 0x%08x     return_code = 0x%08x\n", data, return_code);
	}

	//if (sis3316_not_OK != 0) {
	//	printf("sis3316_not_OK                 \n");
	//	program_stop_and_wait() ;
	//}


	sis3316_adc  *sis3316_adc1 ;
	sis3316_adc1 = new sis3316_adc( gl_vme_crate, module_base_addr);
	//sis3316_adc1 = sis3316_adc1 ;


/******************************************************************************************************************************/
/* CERN ROOT                                                                                                                  */
/******************************************************************************************************************************/

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
	//sis_root_graph *graph_raw = new sis_root_graph(root_graph_text, root_graph_x, root_graph_y, root_graph_x_size, root_graph_y_size) ;
	strcpy(root_graph_text,"SIS3316 Graph: Raw data") ;
	gl_graph_raw      = new sis_root_graph(root_graph_text, root_graph_x, root_graph_y, root_graph_x_size, root_graph_y_size) ;


#endif



	
/******************************************************************************************/


/******************************************************************************************/
/*                                                                                        */
/*  SIS3316 Setup                                                                         */
/*                                                                                        */
/******************************************************************************************/
	if (sis3316_adc1->adc_125MHz_flag == 0) {
		// 250  MHz
		file_header_indentifier      =  0  ;
	}
	else {
		// 125 MHz
		file_header_indentifier      =  1  ;
	}

	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_RESET, 0);  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_DISARM , 0);  //   


	if (sis3316_adc1->adc_125MHz_flag == 0) { // 250 MHz
		// 250.000 MHz
		clock_N1div      =  4  ;
		clock_HSdiv      =  5  ;
		//iob_delay_value  =  0x48   ; // ADC FPGA version A_0250_0003 
		iob_delay_value  =  0x1008 ; // ADC FPGA version A_0250_0004 and higher
	}
	else {
		// 125.000 MHz
		clock_N1div      =  8  ;
		clock_HSdiv      =  5  ;
		//iob_delay_value  =  0x7F ;   // 16 bit ADC FPGA version A_0125_0003  
		iob_delay_value  =  0x1008 ; // 16 bit ADC FPGA version A_0125_0004 and higher
	}
	sis3316_adc1->change_frequency_HSdiv_N1div(0, clock_HSdiv, clock_N1div) ;

/******************************************/
	usleep(100) ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
	usleep(100) ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
	usleep(100);

/******************************************/

// set ADC chips via SPI
	for (i=0;i<4;i++) {
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x81001444 ); // SPI (OE)  set binary
		usleep(1); //unsigned int uint_usec  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x81401444 ); // SPI (OE)  set binary
		usleep(1); //unsigned int uint_usec  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x8100ff01 ); // SPI (OE)  update
		usleep(1); //unsigned int uint_usec  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x8140ff01 ); // SPI (OE)  update
		usleep(1); //unsigned int uint_usec  
	}	


/******************************************/

// Gain/Termination  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_ANALOG_CTRL_REG, analog_ctrl_val); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_ANALOG_CTRL_REG, analog_ctrl_val ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_ANALOG_CTRL_REG, analog_ctrl_val ); // 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_ANALOG_CTRL_REG, analog_ctrl_val ); //  

/******************************************/

//  set ADC offsets (DAC)  
	for (i=0;i<4;i++) { // over all 4 ADC-FPGAs
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG, 0x80000000 + 0x8000000 +  0xf00000 + 0x1);  // set internal Reference
		usleep(1); //unsigned int uint_usec  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG, 0x80000000 + 0x2000000 +  0xf00000 + ((analog_offset_dac_val & 0xffff) << 4));  //  
		usleep(1); //unsigned int uint_usec  
	}	

/******************************************/

	data = module_base_addr + 0x0 ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_CHANNEL_HEADER_REG, data); //  
	data = module_base_addr + 0x00400000 ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_CHANNEL_HEADER_REG, data ); //  
	data = module_base_addr + 0x00800000 ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_CHANNEL_HEADER_REG, data ); // 
	data = module_base_addr + 0x00C00000 ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_CHANNEL_HEADER_REG, data ); //  




/******************************************/





	// disable all FIR Triggers  
	data = 0x00000000 ;
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_THRESHOLD_REG, data );  // disable all ch_sum
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch1, 5, 9, 13 
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch2, ..
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch3, ..
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_THRESHOLD_REG, data );  // disable ch4, ..
	}	


	// set HighEnergy Threshold
	data =  0x08000000 + (p_val * 1000) ; // gt 1000
	for (i_adc_fpga=0; i_adc_fpga<4; i_adc_fpga++) {
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_HIGH_ENERGY_THRESHOLD_REG, data);  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_HIGH_ENERGY_THRESHOLD_REG, data);  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_HIGH_ENERGY_THRESHOLD_REG, data);  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_HIGH_ENERGY_THRESHOLD_REG, data);  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_HIGH_ENERGY_THRESHOLD_REG, data);  
	}	


	// set FIR Trigger Setup
	for (i_adc_fpga=0;i_adc_fpga<4;i_adc_fpga++) {
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_SETUP_REG, 0) ; // clear FIR Trigger Setup -> a following Setup will reset the logic ! 
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_SETUP_REG, p_val + (g_val << 12)) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_SETUP_REG, 0) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_SETUP_REG, p_val + (g_val << 12)) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_SETUP_REG, 0) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_SETUP_REG, p_val + (g_val << 12)) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_SETUP_REG, 0) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_SETUP_REG, p_val + (g_val << 12)) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_SETUP_REG, 0) ;  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_SETUP_REG, p_val + (g_val << 12)) ;  
	}	

// configure Fir trigger logic	
	data =  0xB0000000 + 0x08000000 + trigger_threshold_value ;  // cfd 50 
	for (i_adc_fpga=0;i_adc_fpga<4;i_adc_fpga++) {
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH1_FIR_TRIGGER_THRESHOLD_REG, data);  //  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH2_FIR_TRIGGER_THRESHOLD_REG, data);  //   
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH3_FIR_TRIGGER_THRESHOLD_REG, data);  //  
		return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + (i_adc_fpga*SIS3316_FPGA_ADC_REG_OFFSET) + SIS3316_ADC_CH4_FIR_TRIGGER_THRESHOLD_REG, data);  //  
	}	

	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_VME_FPGA_LINK_ADC_PROT_STATUS, 0xE0E0E0E0);  // clear error Latch bits 




	
/******************************************/


	maw_test_buffer_delay  = p_val + (p_val>>1) + g_val + 16 + 20 ;
	if (maw_test_buffer_delay > 1024) {
		maw_test_buffer_delay  = 1024 ;
	}


	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_TRIGGER_GATE_WINDOW_LENGTH_REG, ((trigger_gate_window_length -2) & 0xffff) ); // trigger_gate_window_length

/******************************************/

	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_RAW_DATA_BUFFER_CONFIG_REG, ((sample_length & 0xffff) << 16) + (sample_start_index & 0xffff) ); // Sample Length


/******************************************/

	//pre_trigger_delay =  0;
	if (pre_trigger_delay > 2042) {
		pre_trigger_delay  = 2042 ;
	}
	//pre_trigger_delay = pre_trigger_delay + 0x8000 ; // set "Additional Delay of Fir Trigger P+G" Bit  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_PRE_TRIGGER_DELAY_REG, pre_trigger_delay ); //  

/******************************************/

	//pileup ;  
	data = ((uint_re_pileup & 0xffff) << 16) + (uint_pileup & 0xffff) ;

	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_PILEUP_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_PILEUP_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_PILEUP_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_PILEUP_CONFIG_REG, data ); //  



/******************************************/


// Select LEMO Output "Co"   
	data = 0x1 ; // Select Sample Clock
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_LEMO_OUT_CO_SELECT_REG, data ); //  

	// Enable LEMO Output "TO"  
	data = 0xffff ; // Select all triggers
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_LEMO_OUT_TO_SELECT_REG, data ); //  

		// Enable LEMO Output "UO"  
	//data = 0x4 ; // Select LogicBusy
	data = 0x2 ; // Select BankxArmed
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_LEMO_OUT_UO_SELECT_REG, data ); //  


	data = 0x2 ; // Enable Status lines to FP-Bus (test) 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_FP_LVDS_BUS_CONTROL, data ); //  



/******************************************/


#define SETUP_NO_VETO
//#define SETUP_UI_AS_VETO
//#define SETUP_UI_AS_GATE
//#define SETUP_TI_AS_VETO
//#define SETUP_TI_AS_Gate


#ifdef SETUP_NO_VETO
	//  Event Configuration
	data = 0x04040404 ; // internal trigger ch4 , ch3, ch2 ch1
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data ); // 

	data = 0x0 ; //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, data );  
#endif


#ifdef SETUP_UI_AS_VETO
// Enable LEMO Input "UI" as Local Veto function 
	data = 0x1000 ; // Enable Nim Input "UI" as Local Veto function
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_NIM_INPUT_CONTROL_REG, data ); //  
	
	//  Event Configuration
	data = 0x04040404 ; // internal trigger ch4 , ch3, ch2 ch1
	data = data + 0x80808080 ; // external Veto ch4 , ch3, ch2 ch1
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data ); // 

	// enbale external (global) functions
	data = 0x800 ; // enable "Local Veto function" as Veto
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, data );  
#endif

#ifdef SETUP_UI_AS_GATE
// Enable LEMO Input "UI" as Local Veto function 
	data = 0x1000 ; // Enable Nim Input "UI" as Local Veto function
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_NIM_INPUT_CONTROL_REG, data ); //  
	
	//  Event Configuration
	data = 0x04040404 ; // internal trigger ch4 , ch3, ch2 ch1
	data = data + 0x40404040 ; // external Gate ch4 , ch3, ch2 ch1
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data ); // 

	// enbale external (global) functions
	data = 0x800 ; // enable "Local Veto function" as Veto
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, data );  
#endif

#ifdef SETUP_TI_AS_VETO
// Enable LEMO Input "TI" as Local Veto function 
	data = 0x10 ; // Enable Nim Input "TI" as Trigger function
	data = data + 0x40 ; // set Level sensitiv
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_NIM_INPUT_CONTROL_REG, data ); //  
	
	//  Event Configuration
	data = 0x04040404 ; // internal trigger ch4 , ch3, ch2 ch1
	data = data + 0x80808080 ; // external Veto ch4 , ch3, ch2 ch1
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data ); // 

	// enbale external (global) functions
	data = 0x200 ; // enable "External Trigger function" as Veto
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, data );  
#endif


#ifdef SETUP_TI_AS_Gate
// Enable LEMO Input "TI" as Local Veto function 
	data = 0x10 ; // Enable Nim Input "TI" as Trigger function
	data = data + 0x40 ; // set Level sensitiv
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_NIM_INPUT_CONTROL_REG, data ); //  
	
	//  Event Configuration
	data = 0x04040404 ; // internal trigger ch4 , ch3, ch2 ch1
	data = data + 0x40404040 ; // external Gate ch4 , ch3, ch2 ch1
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_EVENT_CONFIG_REG, data ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_EVENT_CONFIG_REG, data ); // 

	// enbale external (global) functions
	data = 0x200 ; // enable "External Trigger function" as Veto
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, data );  
#endif


	if (maw_test_buffer_enable_flag == 0) {
		maw_test_buffer_length =  0 ;
	}



/******************************************/

// data format
	header_length = 3;	
	//header_accu_6_values_offset = 2 ;
	header_accu_2_values_offset = 2 ;
	header_maw_3_values_offset  = 2 ;

	data = 0 ;
	if (header_accu_6_values_enable_flag == 1) {
		header_length = header_length + 7 ;
		header_maw_3_values_offset  = header_maw_3_values_offset + 7 ;
		header_accu_2_values_offset  = header_accu_2_values_offset + 7 ;
		data = data + 0x1 ; // set bit 0
	}
	if (header_accu_2_values_enable_flag == 1) {
		header_length = header_length + 2 ;
		header_maw_3_values_offset  = header_maw_3_values_offset + 2 ;
		data = data + 0x2 ; // set bit 1
	}
	if (header_maw_3_values_enable_flag == 1) {
		header_length = header_length + 3 ;
		data = data + 0x4 ; // set bit 2
	}
	if (maw_test_buffer_enable_flag == 1) {
		data = data + 0x10 ; // set bit 4
	}

	event_length = (header_length + (sample_length / 2)  + maw_test_buffer_length);

	data = data + (data << 8) + (data << 16) + (data << 24);
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_DATAFORMAT_CONFIG_REG, data ); 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_DATAFORMAT_CONFIG_REG, data ); 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_DATAFORMAT_CONFIG_REG, data ); 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_DATAFORMAT_CONFIG_REG, data ); 

// MAW Test Buffer configuration
	data = maw_test_buffer_length + (maw_test_buffer_delay << 16) ;
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_MAW_TEST_BUFFER_CONFIG_REG, data ); 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_MAW_TEST_BUFFER_CONFIG_REG, data ); 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_MAW_TEST_BUFFER_CONFIG_REG, data ); 
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_MAW_TEST_BUFFER_CONFIG_REG, data ); 


	//address_threshold = 200;	
	address_threshold = (nof_events * event_length) - 1 ;  //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG, address_threshold ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_ADDRESS_THRESHOLD_REG, address_threshold); //   
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_ADDRESS_THRESHOLD_REG, address_threshold ); //     
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_ADDRESS_THRESHOLD_REG, address_threshold ); //  


	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH1_4_TRIGGER_STATISTIC_COUNTER_MODE_REG, 1 ); //  
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH5_8_TRIGGER_STATISTIC_COUNTER_MODE_REG, 1); //   
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH9_12_TRIGGER_STATISTIC_COUNTER_MODE_REG, 1 ); //     
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_ADC_CH13_16_TRIGGER_STATISTIC_COUNTER_MODE_REG, 1 ); //  


/******************************************************************************************/
	if (sis3316_adc1->adc_125MHz_flag == 1) {
		gl_graph_raw->sis3316_set_16bit_Yaxis (); //  
	}
	else {
		gl_graph_raw->sis3316_set_14bit_Yaxis (); //  
	}

	if (sample_length != 0) {
		gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y
	}


/******************************************************************************************/

	for (i_ch=0; i_ch<16; i_ch++) {
		uint_channel_event_counter[i_ch]  = 0;
	}
	loop_counter        = 0;
	bank_buffer_counter = 0 ;

	

/******************************************************************************************/


/******************************************************************************************/


	
	// Clear Timestamp  */
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_TIMESTAMP_CLEAR , 0);  //   
	
	// Start Readout Loop  */
	printf("Start Multievent \n");
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_DISARM_AND_ARM_BANK1 , 0);  //  Arm Bank1
	bank1_armed_flag = 1; // start condition
	printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");


	do {

		poll_counter = 0 ;
		do {
			poll_counter++;
			if (poll_counter == 100) {
				gSystem->ProcessEvents();  // handle GUI events
				poll_counter = 0 ;
    		}
			return_code = gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, &data);  
			//usleep(500000); //500ms  
			//printf("in Loop:  return_code = 0x%08x    addr = 0x%08x   SIS3316_ACQUISITION_CONTROL_STATUS = 0x%08x     \n", return_code , module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, data);
		} while ((data & 0x80000) == 0x0) ; // Address Threshold reached ?

		gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, &data);  
		//printf("SIS3316_ACQUISITION_CONTROL_STATUS before bank switch = 0x%08x\n", data);
		
		if (bank1_armed_flag == 1) {		
			return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_DISARM_AND_ARM_BANK2 , 0);  //  Arm Bank2
			bank1_armed_flag = 0; // bank 2 is armed
			printf("SIS3316_KEY_DISARM_AND_ARM_BANK2 \n");
		}
		else {
			return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_DISARM_AND_ARM_BANK1 , 0);  //  Arm Bank1
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


		//gl_vme_crate->vme_A32D32_read ( module_base_addr + SIS3316_ACQUISITION_CONTROL_STATUS, &data);
		//printf("SIS3316_ACQUISITION_CONTROL_STATUS after bank switch = 0x%08x\n", data);

		gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y
		for (i_ch=0; i_ch<16; i_ch++) {
			// read channel events 
			return_code = sis3316_adc1->read_MBLT64_Channel_PreviousBankDataBuffer(bank1_armed_flag /*bank2_read_flag*/, i_ch /* 0 to 15 */,  &got_nof_32bit_words, gl_rblt_data ) ;
			//printf("read_MBLT64_Channel_PreviousBankDataBuffer: i_ch %d  got_nof_32bit_words = 0x%08x  return_code = 0x%08x\n",i_ch,  got_nof_32bit_words, return_code);
			if (return_code != 0) {
				printf("read_MBLT64_Channel_PreviousBankDataBuffer: return_code = 0x%08x\n", return_code);

			}
			ch_event_counter = (got_nof_32bit_words  / event_length) ;
			uint_channel_event_counter[i_ch]  = uint_channel_event_counter[i_ch] + ch_event_counter;
			if (ch_event_counter > 0) {

				// plot events
				for (i=0; i<ch_event_counter; i++) {
					if (i<10) { // plot ony 10 events
						gl_graph_raw->sis3316_draw_chN (sample_length, &gl_rblt_data[i*(event_length) + header_length], i_ch); //  
					}
				}
				if (uint_DataEvent_OpenFlag == 1) {   ; // Open
					WriteBufferHeaderCounterNofChannelToDataFile (file_header_indentifier, bank_buffer_counter, i_ch, ch_event_counter, event_length , maw_test_buffer_length, 0 /* reserved */ ) ;
					WriteEventsToDataFile (gl_rblt_data, got_nof_32bit_words)  ;
    			}
			}
		}

		//usleep(500); //unsigned int uint_usec  
		//usleep(500000); //500ms  

		loop_counter++;
		bank_buffer_counter++;
		
		printf("    bank_buffer_counter = %d     \n",bank_buffer_counter);
		for (i_ch=0; i_ch<16; i_ch++) {
			if (uint_channel_event_counter[i_ch] != 0) {
				printf("        ch %d:    event counter  = %d        \n", i_ch+1, uint_channel_event_counter[i_ch]);
			}
		}
		printf("\n");
		printf("\n");
		gSystem->ProcessEvents();  // handle GUI events

	} while(((loop_counter < stop_after_loop_counts) || (stop_after_loop_counts == 0) ));

	if (uint_DataEvent_OpenFlag == 1) {   ; // Open
		fclose(gl_FILE_DataEvenFilePointer);
	}
	return_code = gl_vme_crate->vme_A32D32_write ( module_base_addr + SIS3316_KEY_DISARM , 0);  //   


	printf("sampling finished   \n\n");

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
/***********************************************************************************************************************************************/



