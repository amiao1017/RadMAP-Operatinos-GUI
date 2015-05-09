/***************************************************************************/
/*                                                                         */
/*  Filename: sis3316_set_dhcp_mode.h                                      */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 10.01.2015                                       */
/*  last modification:    19.03.2015                                       */
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
/*  � 2015                                                                 */
/*                                                                         */
/***************************************************************************/

#include "project_system_define.h"		//define LINUX or WINDOWS
#include "project_interface_define.h"   //define Interface (sis1100/sis310x, sis3150usb or Ethnernet UDP)

#ifdef LINUX

#include <iostream>
using namespace std;

//#include <iostream>
//#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef WINDOWS
	#include <iostream>
	#include <iomanip>
	using namespace std;
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock2.h>

	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include "wingetopt.h" 
#endif




//#include "vme_interface_class.h"

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
	//sis3316_eth *gl_vme_crate ;

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


#include "sis3316_class.h"


vme_interface_class *intf;
sis3316_adc *adc;


int main(int argc, char *argv[])
{
	volatile int return_code ;
	int rc;
//	unsigned int addr;
	unsigned int data ;
	cout << "sis3316_set_dhcp_mode" << endl; // prints sis3316_set_dhcp_mode


	   //char char_command[256];
	   unsigned int dhcp_option ;
	   unsigned int vme_base_address ;
	   int int_ch ;
	   int ch ;
		char ch_string[64] ;

#ifdef ETHERNET_UDP_INTERFACE

	char  pc_ip_addr_string[32] ;
	char  sis3316_ip_addr_string[32] ;

	//strcpy(sis3316_ip_addr_string, gl_sis3316_ip_addr_string) ; // SIS3316 IP address
	//strcpy(sis3316_ip_addr_string,"212.60.16.200") ; // SIS3316 IP address
	strcpy(sis3316_ip_addr_string,"192.168.1.100") ; // SIS3316 IP address
#endif


	  // default
	dhcp_option = 0 ;
	vme_base_address = 0x00000000 ;

	if (argc > 1) {
		while ((int_ch = getopt(argc, argv, "?hCEDI:")) != -1)
			switch (int_ch) {
				case 'I':
					sscanf(optarg,"%s", ch_string) ;
					//printf("-I %s    \n", ch_string );
					strcpy(sis3316_ip_addr_string,ch_string) ;
					break;
				case 'C':
					dhcp_option = 0xC ;
					printf("-C : Clear DHCP option value   \n");
				break;
				case 'E':
					dhcp_option = 0xE ;
					printf("-E : Enable DHCP  \n");
				break;
				case 'D':
					dhcp_option = 0xD ;
					printf("-D : Disable DHCP  \n");
				break;
				case '?':
				case 'h':
				default:
					printf("Usage: %s  [-?h]  [-I ip] [-C or -E or -D] ", argv[0]);
					printf("   \n");
					printf("   -I string     SIS3316 IP Address       Default = %s\n", sis3316_ip_addr_string);
					printf("   -C            Clear DHCP option value (DHCP mode depends on Switch SW80-4) \n");
					printf("   -E            Enable DHCP               \n");
					printf("   -D            Disable DHCP               \n");
					printf("   \n");
					printf("   -h            Print this message\n");
					printf("   \n");
					printf("   \n");
					printf("   date: 19.03.2015 \n");
					printf("   \n");
					printf("   \n");
				exit(1);
				}

		} // if (argc > 1)
	printf("\n");






#ifdef ETHERNET_UDP_INTERFACE
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

	return_code = vme_crate->set_UdpSocketSIS3316_IpAddress( sis3316_ip_addr_string);

	//return_code = vme_crate->udp_reset_cmd();
	return_code = vme_crate->vme_A32D32_write(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x80000000); // kill request and grant from other vme interface
	if (return_code != 0) {
		printf("vme_A32D32_write return_code = 0X%08x   \n",return_code);
		return 0;
	}
	return_code = vme_crate->vme_A32D32_write(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL, 0x1); // request access to SIS3316 from UDP interface
	if (return_code != 0) {
		printf("vme_A32D32_write return_code = 0X%08x   \n",return_code);
		return 0;
	}
 
    return_code = vme_crate->vme_A32D32_read(SIS3316_MODID,&data);
	if (return_code != 0) {
		printf("vme_A32D32_read return_code = 0X%08x   \n",return_code);
		return 0;
	}
	//printf("return_code = 0X%08x   Module ID = 0X%08x \n",return_code, data);
 
	return_code = vme_crate->vme_A32D32_read(SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL,&data);
	if (return_code != 0) {
		printf("vme_A32D32_read return_code = 0X%08x   \n",return_code);
		return 0;
	}
	//printf("return_code = 0X%08x   ACCESS    = 0X%08x \n",return_code, data);
	//printf("\n");


#endif
	char char_messages[128] ;
	unsigned int nof_found_devices ;

	// open Vme Interface device
	return_code = vme_crate->vmeopen ();  // open Vme interface
	vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface
   // printf("get_vmeopen_messages = %s , nof_found_devices %d \n",char_messages, nof_found_devices);


	adc = new sis3316_adc(vme_crate, vme_base_address);
	rc = adc->register_read(0x4, &data);
	if ((rc != 0) || ((data & 0xffff0000) != 0x33160000)) {
		printf("not valid SIS3316 vme base address\n");
		printf("return_code      = 0x%08X \n", rc);
		printf("vme_base_address = 0x%08X \n", vme_base_address);
		printf("module ID        = 0x%08X \n", data);
		return -1;
	}
	else {
		printf("module ID / VME FPGA version       = 0x%08X \n", data);
		rc = adc->register_read(0x1100, &data);
		rc = adc->register_read(0x1100, &data);
		printf("module ID / ADC FPGA version       = 0x%08X \n", data);

	}
	printf("\n");
	return_code = vme_crate->vme_A32D32_read(SIS3316_SERIAL_NUMBER_REG,&data);
	//printf("return_code = 0X%08x   SIS3316_SERIAL_NUMBER_REG = 0X%08x\n",return_code, data);
	printf("Serial number = %d \n", data&0xffff);
	printf("DHCP option   = %d\n", (data&0xff000000)>>24);
	printf("\n");
 
	//				dhcp_option = 0xE ;
		unsigned char uchar_data[32] ;
		if(dhcp_option == 0xC ) {
			uchar_data[0] = 0x0 ;
		}
		if(dhcp_option == 0xE ) {
			uchar_data[0] = 0x1 ;
		}
		if(dhcp_option == 0xD ) {
			uchar_data[0] = 0x2 ;
		}
		if(dhcp_option != 0) {
			rc = adc->write_ow_dhcp_option( uchar_data);
			printf("write_ow_dhcp_option = 0x%02x      rc = 0X%08x   \n", uchar_data[0],rc);
		}

 		do{
			printf("\ndo you want to reboot the FPGAs (switch SW80-7 has to be on; Watchdog) ? [y/n]:");
	        fflush(stdout);
	        fflush(stdin);
	        do {
	        	ch = getchar();
		    }while(ch == 0xa);
			//printf("\n ch %d",ch);
	        fflush(stdout);
	        fflush(stdin);
	    }while(ch != 'y' && ch != 'n');
	    if(ch == 'n'){
	        return -1;
	    }

		rc = adc->register_write(0x0, 0x8000); // reboot FPGAs ; switch SW80-7 has to be on (Watchdog)

		return 0;
	}



