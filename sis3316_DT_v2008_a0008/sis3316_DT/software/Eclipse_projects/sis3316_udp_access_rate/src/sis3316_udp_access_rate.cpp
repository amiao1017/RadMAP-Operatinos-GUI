
/***************************************************************************/
/*                                                                         */
/*  LINUX: Scientific                                                      */
/*  Filename: sis3316_udp_access_rate.cpp                                  */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 07.09.2014                                       */
/*  last modification:    16.09.2014                                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*  Prepare IP-address on PC:  (for example with 212.60.16.211)            */
/*   Window_PC: arp -s 212.60.16.211  00-00-56-31-6x-xx                    */
/*   LINUX_PC:  arp -s 212.60.16.211  00:00:56:31:6x:xx                    */
/*   xxx: Serial Number (hex)  ; Serial Number = 40 -> 028                 */
/*                                                                         */
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
/***************************************************************************/
#include <iostream>
using namespace std;

#define LINUX    // else WINDOWS

#ifndef LINUX
	#define WINDOWS
#endif






#ifdef LINUX
	#include <sys/types.h>
	#include <sys/socket.h>

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


//#include "stdafx.h"
int SIS3316_ETH_UDP_CreateUdpSocket(void); // return_value = udp_socket
int SIS3316_ETH_UDP_SetUdpSocketOptionTimeout(int udp_socket, unsigned int recv_timeout_sec, unsigned int recv_timeout_usec);
int SIS3316_ETH_UDP_SetUdpSocketOptionSetBufSize(int udp_socket, int sockbufsize);
int SIS3316_ETH_UDP_UdpSocketBindToDevice(int udp_socket, char* eth_device);
int SIS3316_ETH_UDP_UdpSocketBindMyOwnPort(int udp_socket, unsigned int udp_port);


int SIS3316_ETH_UDP_ResetCmd(int udp_socket, struct sockaddr *socket_dest_addr);
int SIS3316_ETH_UDP_RegisterWrite(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int reg_addr, unsigned int reg_data);
int SIS3316_ETH_UDP_RegisterRead(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int reg_addr, unsigned int* reg_data);

int SIS3316_ETH_UDP_SIS3316_RegisterRead(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_read_registers, unsigned int* reg_addr_ptr, unsigned int* reg_data_ptr);
int SIS3316_ETH_UDP_SIS3316_RegisterWrite(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_write_registers, unsigned int* reg_addr_ptr, unsigned int* reg_data_ptr);
int SIS3316_ETH_UDP_SIS3316_FifoRead(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_read_words, unsigned int reg_addr, unsigned int* data_ptr);
int SIS3316_ETH_UDP_SIS3316_FifoWrite(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_write_words, unsigned int fifo_addr, unsigned int* data_ptr);




int SIS3316_ETH_UDP_TestAccess(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int cmd, unsigned int length, unsigned int reg_addr, unsigned int* reg_data);

void get_time_clock(double* double_second) ;


//#define  MEMORY_EVENTBLOCK_SIZE 		0x04000000  // 64 MByte
#define  MEMORY_EVENTBLOCK_SIZE 		0x0400000  // 4 MByte


/* ***************************************************************************************************************** */
//handling cmd line input
char gl_cmd_ip_string[64];
unsigned int gl_cmd_ethernet_device_no = 0;

char gl_command[256];

unsigned int gl_rd_data[0x1000000] ; // 64 MByte
unsigned int gl_wr_data[0x1000000] ; // 64 MByte

/* ***************************************************************************************************************** */
int main(int argc, char* argv[]) {
//int main() {

	int i_arg;
	unsigned int i, j;
	  int return_code;

	  int udp_socket;
	  unsigned char *udp_recv_buffer ;


	  struct sockaddr_in SIS3316_sock_addr_in   ;

	  unsigned int udp_port ;

	  unsigned int data ;

	  unsigned int loop_counter ;
	  unsigned int bad_return_code_counter ;

	  //struct timeval struct_time;

	  struct timeval tv;tv.tv_sec = 30;
	  //const char device[] = DEVICE_ETH0 ;
	  char  eth_device[64] ;


	  unsigned int *SIS3316_memory_read_data ;


	  char ch ;
	  char ch_string[64] ;

	//	clock_t time_clock_start, time_clock_finish;
	double time_clock_start, time_clock_finish;
	double  double_cycles_duration;
	double  double_Byte_Per_Second;
	double  double_KByte_Per_Second;
	double  double_MByte_Per_Second;

	 // unsigned int  timeout_counter;
	 // unsigned int  print_counter;
	 // unsigned int  led_flag;

		unsigned int write_register_length ;
		unsigned int write_address_array[64] ;
		unsigned int write_data_array[64] ;
		unsigned int read_register_length ;
		unsigned int read_address_array[64] ;
		unsigned int read_data_array[64] ;


		unsigned int sub_write_block_length ;
		unsigned int sub_write_nof_blocks ;
		unsigned int write_fifo_length ;

		unsigned int speed_loop ;



	//strcpy(gl_cmd_ip_string,"212.60.16.211") ;
	strcpy(gl_cmd_ip_string,"192.168.1.100") ;



	/********************************************************************************************************************************************/
	/*  Command interpreter                                                                                                                     */
	/********************************************************************************************************************************************/

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


		while ((ch = getopt(argc, argv, "?lhI:A:DC:E:S:")) != -1)
		  //printf("ch %c    \n", ch );
		  switch (ch) {
		    case 'I':
			sscanf(optarg,"%s", ch_string) ;
			printf("-I %s    \n", ch_string );
			strcpy(gl_cmd_ip_string,ch_string) ;
			break;

		    case 'A':
			sscanf(optarg,"%x",&data) ;
			gl_cmd_ethernet_device_no = data ;
			break;

		    case '?':
		    case 'h':
		    default:
		      printf("Usage: %s  [-?h] [-I ip] [-A num]  ", argv[0]);
		      printf("   \n");
		      printf("       -I string     SIS3316 IP Address                                          Default = %s\n", gl_cmd_ip_string);
		      printf("       -A num        Ethernet Device Number (0 = ETH0, 1 = ETH1, .. , 3 = ETH3)  Default = %d\n", gl_cmd_ethernet_device_no);
		      printf("   \n");
		      printf("   \n");
		      printf("       -h            Print this message\n");
		      printf("   \n");
		      exit(1);
		    }

	      } // if (argc > 2)

	      printf("gl_cmd_ip_string          = %s\n",gl_cmd_ip_string);
	      printf("gl_cmd_ethernet_device_no = %d\n",gl_cmd_ethernet_device_no);
	      usleep(1000);

	/********************************************************************************************************************************************/
	/*  Create and Setup UDP Socket                                                                                                             */
	/********************************************************************************************************************************************/

	#ifdef WINDOWS
	    //return_code = WSAStartup();
	    return_code = WinsockStartup();
	#endif

	/* Construct the SIS3316 sockaddr_in structure)  */
	      memset(&SIS3316_sock_addr_in, 0, sizeof(SIS3316_sock_addr_in));
	      strcpy(eth_device,"eth0") ;
	      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("212.60.16.200");
	      SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr(gl_cmd_ip_string);

		  udp_port = 0xE000 ;

	      switch (gl_cmd_ethernet_device_no) {
		  case 0:
		      strcpy(eth_device,"eth0") ;
		      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("212.60.16.200");
		      udp_port = 0xE001 ;
		      break;
		  case 1:
		      strcpy(eth_device,"eth1") ;
		      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("213.60.16.200");
		      udp_port = 0xD001 ;
		      break;
		  case 2:
		      strcpy(eth_device,"eth2") ;
		      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("214.60.16.200");
		      udp_port = 0xD002 ;
		      break;
		  case 3:
		      strcpy(eth_device,"eth3") ;
		      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("215.60.16.200");
		      udp_port = 0xD003 ;
		      break;
		  default:
		      break;
	      }
	// prepare ethernet connection
	      SIS3316_sock_addr_in.sin_family = AF_INET;
	      SIS3316_sock_addr_in.sin_port = htons(udp_port);
	      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("213.60.16.201");
	      //SIS3316_sock_addr_in.sin_addr.s_addr = inet_addr("127.0.0.201");
	      memset(&(SIS3316_sock_addr_in.sin_zero),0,8);

		// Create udp_socket
		do {
			if ((udp_socket = SIS3316_ETH_UDP_CreateUdpSocket()) == -1) {
				printf("Error: SIS3316_ETH_UDP_CreateUdpSocket\n");
			}
			// set Receive Timeout to 100ms
			return_code = SIS3316_ETH_UDP_SetUdpSocketOptionTimeout(udp_socket, 0 /* sec */, 100000 /* usec */); // 100ms
			if (return_code == -1) {
				printf("Error: SIS3316_ETH_UDP_SetUdpSocketOptionTimeout\n");
			}

			// increase read_buffer size
			// SUSE needs following command as su: >sysctl -w net.core.rmem_max=33554432
			int sockbufsize = 335544432 ; // 0x2000000
			return_code = SIS3316_ETH_UDP_SetUdpSocketOptionSetBufSize(udp_socket, sockbufsize); //
			if (return_code == -1) {
				printf("Error: SIS3316_ETH_UDP_SetUdpSocketOptionSetBufSize\n");
			}

			// needs to be superuser
			// bind the udp_socket to one network device
			return_code = SIS3316_ETH_UDP_UdpSocketBindToDevice(udp_socket, eth_device); //
			if (return_code == -1) {
				printf("Error: SIS3316_ETH_UDP_UdpSocketBindToDevice --> required super user \n");
			}

			return_code = SIS3316_ETH_UDP_UdpSocketBindMyOwnPort(udp_socket, udp_port); //
			if (return_code == -1) {
				printf("Error: SIS3316_ETH_UDP_UdpSocketBindMyOwnPort   udp_socket = 0x%08x \n",udp_socket);
				udp_port++;
				SIS3316_sock_addr_in.sin_port = htons(udp_port);
				printf("Try with new Port again   udp_port = 0x%08x \n",udp_port);
			}
		} while (return_code != 0) ;





	 // Hinweis: wenn UDP port schon benutzt wird, gibt es einen Fehler !!
		  // Fehler in SIS3316_ETH_UDP_UdpSocketBindMyOwnPort auswerten und gegebenfalls neue Port addresse suchen
		  // mit "netstat -a" auf console ueberpruefen


	/********************************************************************************************************************************************/
	/*  malloc buffers                                                                                                                           */
	/********************************************************************************************************************************************/

	      udp_recv_buffer = (unsigned char*)malloc(0x4000); // 16384
	      if(udp_recv_buffer == NULL) {
		  printf("Error: malloc:udp_recv_buffer\n");
		  exit(1);
	      }

	      SIS3316_memory_read_data = (unsigned int*)malloc(MEMORY_EVENTBLOCK_SIZE);
	      if(SIS3316_memory_read_data == NULL) {
		  printf("Error: malloc:SIS3316_memory_read_data\n");
		  exit(1);
	      }

	/********************************************************************************************************************************************/
	/*  Reset and check UDP Connection                                                                                                          */
	/********************************************************************************************************************************************/

	      SIS3316_ETH_UDP_ResetCmd(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in);

	      loop_counter = 0 ;
	      bad_return_code_counter = 0 ;
	      //

	    // set UDP Interface Request bit (access to register 0x20 and higher)
		SIS3316_ETH_UDP_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x10, 0x1); // set request bit

		printf("\n");
		return_code = SIS3316_ETH_UDP_RegisterRead( udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x4, &data);
		if (return_code == 0) {
			printf("Module and Version register:  0x%08x       \n",data );
		}
		else {
			printf("return_code  0x%08x  --> not valid IP address      \n",return_code );
			do {
				usleep(100000);
			} while (1) ;
		}
		printf("\n");
		printf("\n");

		do {
		/* single internal UDP Register write cycles (without handshake)  ->  SIS3316_ETH_UDP_RegisterWrite */
			speed_loop = 10000 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				SIS3316_ETH_UDP_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x0, 0x10001); // toogle led U
			}
			get_time_clock(&time_clock_finish);
			//printf( "start = %2.6f seconds\n", (double)(time_clock_start));
			//printf( "finish = %2.6f seconds\n", (double)(time_clock_finish)  );

			double_cycles_duration = (double)(time_clock_finish - time_clock_start) ;
			//printf( "%2.3f seconds\n", double_cycles_duration );

			double_Byte_Per_Second =  (double)(speed_loop * 4) / double_cycles_duration ;
			double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			printf( "Single SIS3316_ETH_UDP_RegisterWrite cycle rate:               %4.3f KByte/sec \n", double_KByte_Per_Second );
			//printf( "%2.1f transfer speed \n", double_KByte_Per_Second );


		/* single SIS3316 Register write cycles (with handshake)  ->  SIS3316_ETH_UDP_SIS3316_RegisterWrite */
			write_register_length = 1 ;
			write_address_array[0] = 0x78 ;
			write_data_array[0] = 0x80000000 ;  // pulse on Lemo UO

			speed_loop = 10000 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				return_code = SIS3316_ETH_UDP_SIS3316_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, write_register_length, write_address_array, write_data_array);
				if (return_code != 0) {
					printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
					bad_return_code_counter++ ;
				}
			}
			get_time_clock(&time_clock_finish);
			double_cycles_duration = (double)(time_clock_finish - time_clock_start) ;
			double_Byte_Per_Second =  (double)(speed_loop * 4) / double_cycles_duration ;
			double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			printf( "Single SIS3316_ETH_UDP_SIS3316_RegisterWrite cycle rate:       %4.3f KByte/sec \n", double_KByte_Per_Second );


		/* burst of 64 SIS3316 Register write cycles (with handshake)  ->  SIS3316_ETH_UDP_SIS3316_RegisterWrite */
			write_register_length = 64 ;
			for (i = 0;i<write_register_length;i++) {
				write_address_array[i] = 0x78 ;
				write_data_array[i] = 0x80000000 ;  // pulse on Lemo UO
			}
			speed_loop = 10000 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				return_code = SIS3316_ETH_UDP_SIS3316_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, write_register_length, write_address_array, write_data_array);
				if (return_code != 0) {
					printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
					bad_return_code_counter++ ;
				}
			}
			get_time_clock(&time_clock_finish);
			double_cycles_duration = (double)(time_clock_finish - time_clock_start)  ;
			double_Byte_Per_Second =  (double)(speed_loop * 4 * write_register_length) / double_cycles_duration ;
			double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			printf( "Burst of 64 SIS3316_ETH_UDP_SIS3316_RegisterWrite cycle rate:  %4.3f KByte/sec \n", double_KByte_Per_Second );


			printf("\n");


		/* single internal UDP Register read cycles (without handshake)  ->  SIS3316_ETH_UDP_RegisterRead */
			speed_loop = 10000 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				return_code = SIS3316_ETH_UDP_RegisterRead(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x0, &data); //
				if (return_code != 0) {
					printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
					bad_return_code_counter++ ;
				}
			}
			get_time_clock(&time_clock_finish);

			double_cycles_duration = (double)(time_clock_finish - time_clock_start)  ;

			double_Byte_Per_Second =  (double)(speed_loop * 4) / double_cycles_duration ;
			double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			printf( "Single SIS3316_ETH_UDP_RegisterRead cycle rate:                %4.3f KByte/sec \n", double_KByte_Per_Second );


		/* single SIS3316 Register read cycles (with handshake)  ->  SIS3316_ETH_UDP_SIS3316_RegisterRead */
			read_register_length = 1 ;
			read_address_array[0] = 0x20 ;

			speed_loop = 10000 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				return_code = SIS3316_ETH_UDP_SIS3316_RegisterRead(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, read_register_length, read_address_array, read_data_array);
				if (return_code != 0) {
					printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
					bad_return_code_counter++ ;
				}
			}
			get_time_clock(&time_clock_finish);
			double_cycles_duration = (double)(time_clock_finish - time_clock_start) ;
			double_Byte_Per_Second =  (double)(speed_loop * 4) / double_cycles_duration ;
			double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			printf( "Single SIS3316_ETH_UDP_SIS3316_RegisterRead cycle rate:        %4.3f KByte/sec \n", double_KByte_Per_Second );


		/* burst of 16 SIS3316 Register read cycles (with handshake)  ->  SIS3316_ETH_UDP_SIS3316_RegisterRead */
			read_register_length = 16 ;
			for (i = 0;i<4;i++) {
				read_address_array[i]    = 0x1120 + (4*i) ; // Previous Bank Sample address ch 1-4
				read_address_array[4+i]  = 0x2120 + (4*i) ; // Previous Bank Sample address ch 5-8
				read_address_array[8+i]  = 0x3120 + (4*i) ; // Previous Bank Sample address ch 9-12
				read_address_array[12+i] = 0x4120 + (4*i) ; // Previous Bank Sample address ch 13-16
			}
			speed_loop = 10000 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				return_code = SIS3316_ETH_UDP_SIS3316_RegisterRead(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, read_register_length, read_address_array, read_data_array);
				if (return_code != 0) {
					printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
					bad_return_code_counter++ ;
				}
			}
			get_time_clock(&time_clock_finish);
			double_cycles_duration = (double)(time_clock_finish - time_clock_start)  ;
			double_Byte_Per_Second =  (double)(speed_loop * 4 * read_register_length) / double_cycles_duration ;
			double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			printf( "Burst of 16 SIS3316_ETH_UDP_SIS3316_RegisterRead cycle rate:   %4.3f KByte/sec \n", double_KByte_Per_Second );


			printf("\n");


	// Memory write (test feature)
			write_register_length  = 1 ;
			write_address_array[0] = 0x80 ;
			write_data_array[0]    = 0xC0000000 ; // start write transfer FSM
			return_code = SIS3316_ETH_UDP_SIS3316_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, write_register_length, write_address_array, write_data_array);
			if (return_code != 0) {
				printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
				bad_return_code_counter++ ;
			}

			for (i=0;i<0x10000;i++) {
				gl_wr_data[i] = 0x11000000 + i;
			}


			sub_write_block_length = 0x100; // max 0x100
			sub_write_nof_blocks   = 0x100; // 0x100 blocks -> 0x10000 -> 64K 32-bit words
			write_fifo_length = sub_write_nof_blocks * sub_write_block_length ;

			speed_loop = 50 ;
			get_time_clock(&time_clock_start);
			for (i = 0;i<speed_loop;i++) {
				for (j = 0;j<sub_write_nof_blocks;j++) {
					return_code = SIS3316_ETH_UDP_SIS3316_FifoWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, sub_write_block_length, 0x100000, &gl_wr_data[j*sub_write_block_length]);
					if (return_code != 0) {
						printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
						bad_return_code_counter++ ;
					}
				}
			}
			get_time_clock(&time_clock_finish);
			double_cycles_duration = (double)(time_clock_finish - time_clock_start)  ;
			double_Byte_Per_Second =  (double)(speed_loop * 4 * write_fifo_length) / double_cycles_duration ;
			//double_KByte_Per_Second =  double_Byte_Per_Second / 1024.0 ;
			double_MByte_Per_Second =  double_Byte_Per_Second / 1048576.0 ; // 1M
			printf( "SIS3316_ETH_UDP_SIS3316_FifoWrite cycle rate:                  %4.3f MByte/sec \n", double_MByte_Per_Second );
			printf("\n");



			// Memory read
			write_register_length  = 1 ;
			write_address_array[0] = 0x80 ;
			write_data_array[0]    = 0x80000000 ; // start read transfer FSM
			return_code = SIS3316_ETH_UDP_SIS3316_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, write_register_length, write_address_array, write_data_array);
			if (return_code != 0) {
				printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
				bad_return_code_counter++ ;
			}

			unsigned int read_fifo_length ;
			read_fifo_length  = 0x10000 ;

			unsigned int read_block_length ;
			read_block_length  = 0x40 ;

			for (j=0;j<11;j++) {
				read_fifo_length  = read_block_length << j  ;
				speed_loop = 10000 ;
				if(j > 4) {speed_loop = 5000 ; }
				if(j > 8) {speed_loop = 1000 ; }
				get_time_clock(&time_clock_start);
				for (i = 0;i<speed_loop;i++) {
					return_code = SIS3316_ETH_UDP_SIS3316_FifoRead(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, read_fifo_length, 0x100000, gl_rd_data);
					//printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
					if (return_code != 0) {
						printf("i = 0x%08x   return_code = 0x%08x   \n",i, return_code);
						bad_return_code_counter++ ;
					}
				}
				get_time_clock(&time_clock_finish);
				double_cycles_duration = (double)(time_clock_finish - time_clock_start) ;
				double_Byte_Per_Second =  (double)(speed_loop * 4 * read_fifo_length) / double_cycles_duration ;
				double_MByte_Per_Second =  double_Byte_Per_Second / 1048576.0 ; // 1M

				printf( "SIS3316_ETH_UDP_SIS3316_FifoRead cycle rate:    read_fifo_length = %d (0x%08x)  \t  %4.3f MByte/sec \n", read_fifo_length, read_fifo_length, double_MByte_Per_Second );
			}


			printf("\n");
			printf("\n");

			loop_counter++ ;
			printf( "loop_counter = %d         bad_return_code_counter = %d  \n", loop_counter, bad_return_code_counter  );


			printf("\n");
			printf("\n");




	} while  (bad_return_code_counter < 10) ;

	printf("Program stopped      \n");
	do {
		usleep(100000);
	} while (1) ;


	return 0;
}









/* ***************************************************************************************************************** */
/* Ethernet-UDP routines                                                                                             */
/* ***************************************************************************************************************** */

/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_CreateUdpSocket(void)
{
  int return_code;
  return_code = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  return return_code;
}

/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_SetUdpSocketOptionTimeout(int udp_socket, unsigned int recv_timeout_sec, unsigned int recv_timeout_usec)
{
  int return_code;

#ifdef LINUX
  struct timeval struct_time;

  struct_time.tv_sec  = recv_timeout_sec;
  struct_time.tv_usec = recv_timeout_usec; //

  return_code = setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&struct_time,sizeof(struct timeval));
#endif

#ifdef WINDOWS
	unsigned int msec;
	msec = (1000 * recv_timeout_sec) + ((recv_timeout_usec+999) / 1000) ;

	return_code = setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&msec,sizeof(msec));
#endif


  if (return_code == -1) {
      printf("Error: SIS3316_ETH_UDP_SetUdpSocketOptionTimeout \n");
  }
  return return_code;
}

/* ***************************************************************************************************************** */


int SIS3316_ETH_UDP_SetUdpSocketOptionSetBufSize(int udp_socket, int sockbufsize)
{
  int return_code;
  unsigned char recv_data[16];
  socklen_t addr_len;

  return_code = setsockopt(udp_socket, SOL_SOCKET,SO_RCVBUF, (char *) &sockbufsize, (int)sizeof(sockbufsize));
  if (return_code == -1) {
      printf("Error: SIS3316_ETH_UDP_SetUdpSocketOptions, setsockopt\n");
      return return_code;
  }

#ifdef LINUX
  return_code = getsockopt(udp_socket, SOL_SOCKET,SO_RCVBUF, &recv_data, &addr_len);
  if (return_code == -1) {
      printf("Error: SIS3316_ETH_UDP_SetUdpSocketOptions, getsockopt\n");
  }
#endif
  return return_code;
}


/* ***************************************************************************************************************** */

// must be call as superuser
int SIS3316_ETH_UDP_UdpSocketBindToDevice(int udp_socket, char* eth_device)
{
  int return_code=0;

#ifdef LINUX
  return_code = setsockopt(udp_socket, SOL_SOCKET, SO_BINDTODEVICE, eth_device, sizeof(eth_device));
  if (return_code == -1) {
      printf("Error: SIS3316_ETH_UDP_UdpSocketBindToDevice \n");
  }
#endif
  return return_code;
}


/* ***************************************************************************************************************** */
//int SIS3316_ETH_UDP_UdpSocketBindMyOwnPort(int udp_socket, char* eth_device, char* eth_ip_string, unsigned int udp_port, struct sockaddr *socket_dest_addr)

// must be call as superuser
int SIS3316_ETH_UDP_UdpSocketBindMyOwnPort(int udp_socket, unsigned int udp_port)
{
  int return_code;
  struct sockaddr_in my_addr;

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(udp_port);
  my_addr.sin_addr.s_addr = 0x0 ; //ADDR_ANY;
  memset(&(my_addr.sin_zero),0,8);
  //bzero(&(my_addr.sin_zero),8);


  return_code = bind(udp_socket,(struct sockaddr *)&my_addr, sizeof(my_addr));
  if (return_code == -1) {
      printf("Error: SIS3316_ETH_UDP_UdpSocketBindMyOwnPort \n");
  }
  return return_code;
}




/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_ResetCmd(int udp_socket, struct sockaddr *socket_dest_addr)
{
    int return_code;
    char send_data[4];
    // write Cmd
    send_data[0] = 0xFF ; // reset
    return_code = sendto(udp_socket, send_data, 1, 0, socket_dest_addr, sizeof(struct sockaddr));

    return return_code;
}

/* ***************************************************************************************************************** */
int SIS3316_ETH_UDP_RegisterWrite(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int reg_addr, unsigned int reg_data)
{
    int return_code;
    char send_data[16];
    // write to register
    send_data[0] = 0x11 ; // register write CMD
    send_data[1] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
    send_data[2] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
    send_data[3] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
    send_data[4] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
    send_data[5] = (unsigned char)  (reg_data & 0xff) ;        // data(7 dwonto 0)
    send_data[6] = (unsigned char) ((reg_data >>  8) & 0xff) ; // data(15 dwonto 8)
    send_data[7] = (unsigned char) ((reg_data >> 16) & 0xff) ; // data(23 dwonto 16)
    send_data[8] = (unsigned char) ((reg_data >> 24) & 0xff) ; // data(31 dwonto 24)
    return_code = sendto(udp_socket, send_data, 9, 0, socket_dest_addr, sizeof(struct sockaddr));

    return return_code;
}

/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_RegisterRead(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int reg_addr, unsigned int* reg_data)
{
    int return_code;
    int receive_bytes;
    unsigned int *data_ptr;
    //unsigned int data;
    char send_data[16];
    char recv_data[16];
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);

    // send Read Req Cmd
    send_data[0] = 0x10 ; // register read CMD
    send_data[1] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
    send_data[2] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
    send_data[3] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
    send_data[4] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
    return_code = sendto(udp_socket, send_data, 5, 0, socket_dest_addr, sizeof(struct sockaddr));

    //receive_bytes = read(socket,recv_data,9);
    receive_bytes = recvfrom(udp_socket, recv_data, 9, 0,  socket_dest_addr, &addr_len);

    if(receive_bytes == 9) {
	data_ptr = (unsigned int*)&recv_data[5];
	//data = *data_ptr;
	*reg_data = *data_ptr;
  		//printf("receive_bytes  %d \n",receive_bytes);
		//printf("register read  0x%08x \n",data);
	return 0;
    }

    return receive_bytes;
    // -1: timeout, > 0; wrong number in receive_bytes
}


/* ***************************************************************************************************************** */
/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_SIS3316_RegisterRead(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_read_registers, unsigned int* reg_addr_ptr, unsigned int* reg_data_ptr)
{
    unsigned int i;
    int return_code;
    int receive_bytes;
    unsigned int nof_32bit_word;
    unsigned int reg_addr;
    unsigned int *data_ptr;
    //unsigned int data;
    char send_data[512];
    char recv_data[512];
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);

	nof_32bit_word = nof_read_registers ;
	if (nof_read_registers == 0) {
		nof_32bit_word = 1 ;
	}
	if (nof_read_registers > 64) {
		nof_32bit_word = 64 ;
	}

	send_data[0] = 0x20 ; // send SIS3316 Register Read Req Cmd
    send_data[1] = (unsigned char)  ((nof_32bit_word-1) & 0xff);           //  lower length
    send_data[2] = (unsigned char) (((nof_32bit_word-1) >>  8) & 0xff);    //  upper length
	for (i=0;i<nof_32bit_word;i++) {
		reg_addr = reg_addr_ptr[i] ;
		send_data[(4*i)+3] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
		send_data[(4*i)+4] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
		send_data[(4*i)+5] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
		send_data[(4*i)+6] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
	}
    return_code = sendto(udp_socket, send_data, 3 + (4*nof_32bit_word), 0, socket_dest_addr, sizeof(struct sockaddr));

    //receive_bytes = read(socket,recv_data,9);
    receive_bytes = recvfrom(udp_socket, recv_data, 512, 0,  socket_dest_addr, &addr_len);
    //	printf("receive_bytes  %d \n",receive_bytes);

			//		  memcpy(&uint_adc_data_buffer[udp_data_copy_to_buffer_index], &recv_data[2], receive_bytes-2) ;

    if((unsigned int)receive_bytes == (2 + (4*nof_32bit_word))) {
		for (i=0;i<nof_32bit_word;i++) {
			data_ptr = (unsigned int *)&recv_data[2+(4*i)] ;
			reg_data_ptr[i] = *data_ptr ;
			//printf("register read  0x%08x \n",reg_data_ptr[i]);
		}
		return 0;
    }

    return receive_bytes;
    // -1: timeout, > 0; wrong number in receive_bytes
}

/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_SIS3316_RegisterWrite(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_write_registers, unsigned int* reg_addr_ptr, unsigned int* reg_data_ptr)
{
	unsigned int i;
    int return_code;
    int receive_bytes;
    unsigned int nof_32bit_word;
    unsigned int reg_addr, reg_data;
   // unsigned int *data_ptr;
    //unsigned int data;
    char send_data[512];
    char recv_data[512];
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);

	nof_32bit_word = nof_write_registers ;
	if (nof_write_registers == 0) {
		nof_32bit_word = 1 ;
	}
	if (nof_write_registers > 64) {
		nof_32bit_word = 64 ;
	}

	send_data[0] = 0x21 ; // send SIS3316 Register Read Req Cmd
    send_data[1] = (unsigned char)  ((nof_32bit_word-1) & 0xff);           //  lower length
    send_data[2] = (unsigned char) (((nof_32bit_word-1) >>  8) & 0xff);    //  upper length
	for (i=0;i<nof_32bit_word;i++) {
		reg_addr = reg_addr_ptr[i] ;
		send_data[(8*i)+3] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
		send_data[(8*i)+4] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
		send_data[(8*i)+5] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
		send_data[(8*i)+6] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
		reg_data = reg_data_ptr[i] ;
		send_data[(8*i)+7]  = (unsigned char)  (reg_data & 0xff) ;        // reg_data(7 dwonto 0)
		send_data[(8*i)+8]  = (unsigned char) ((reg_data >>  8) & 0xff) ; // reg_data(15 dwonto 8)
		send_data[(8*i)+9]  = (unsigned char) ((reg_data >> 16) & 0xff) ; // reg_data(23 dwonto 16)
		send_data[(8*i)+10] = (unsigned char) ((reg_data >> 24) & 0xff) ; // reg_data(31 dwonto 24)
	}
    return_code = sendto(udp_socket, send_data, 3 + (8*nof_32bit_word), 0, socket_dest_addr, sizeof(struct sockaddr));

    //receive_bytes = read(socket,recv_data,9);
    receive_bytes = recvfrom(udp_socket, recv_data, 512, 0,  socket_dest_addr, &addr_len);
 	//	printf("receive_bytes  %d \n",receive_bytes);

			//		  memcpy(&uint_adc_data_buffer[udp_data_copy_to_buffer_index], &recv_data[2], receive_bytes-2) ;

    if(receive_bytes ==  2 ) {
    	return_code = 0;
    }
    else {
       	return_code = -1;

    }

    return return_code ;
    // -1: timeout, > 0; wrong number in receive_bytes
}





/* ***************************************************************************************************************** */

int SIS3316_ETH_UDP_SIS3316_FifoRead(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_read_words, unsigned int reg_addr, unsigned int* data_ptr)
{
    //int i;
    int return_code;
    int receive_bytes;
	int send_length;
	unsigned int nof_32bit_word;
    char send_data[512];
    char recv_data[2048];
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);

	nof_32bit_word = nof_read_words ;
	if (nof_read_words == 0) {
		nof_32bit_word = 1 ;
	}
	if (nof_read_words > 0x10000) {
		nof_32bit_word = 0x10000 ;
	}
	//
	send_data[0] = 0x30 ; // send SIS3316 Register Read Req Cmd
    send_data[1] = (unsigned char)  ((nof_32bit_word-1) & 0xff);           //  lower length
    send_data[2] = (unsigned char) (((nof_32bit_word-1) >>  8) & 0xff);    //  upper length
	send_data[3] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
	send_data[4] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
	send_data[5] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
	send_data[6] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
	send_length = 7 ;
	return_code = sendto(udp_socket, send_data, send_length, 0, socket_dest_addr, sizeof(struct sockaddr));
	if (return_code != send_length) {
		printf("Error: sendto return_code =  %d   \n",return_code);
	}
    //receive_bytes = read(socket,recv_data,9);
	unsigned int udp_data_copy_to_buffer_index ;
	unsigned int nof_read_data_bytes ;
	int rest_length_byte ;
	rest_length_byte = 4 * nof_32bit_word ;
	udp_data_copy_to_buffer_index = 0 ;
	//printf("rest_length_byte  %d   \n",rest_length_byte);

	unsigned int soft_packet_number;
	unsigned int packet_number;
	unsigned char* uchar_ptr;
	uchar_ptr = (unsigned char* ) data_ptr ;
	packet_number = 0;
	soft_packet_number = 0;
	do {
		receive_bytes = recvfrom(udp_socket, recv_data, 2000, 0,  socket_dest_addr, &addr_len);
		if (receive_bytes == -1) {
			printf(" \n");
			printf("Error: recvfrom receive_bytes =  %d   \n",receive_bytes);
			printf("receive_bytes  %d   Ack = %2x  Status = %2x  data = %2x \n",receive_bytes, (unsigned char) recv_data[0], (unsigned char) recv_data[1], (unsigned char) recv_data[2]);
			printf("soft_packet_number  %d   \n",soft_packet_number);
			printf("udp_data_copy_to_buffer_index  %d   \n",udp_data_copy_to_buffer_index);
			printf(" \n");
			return -1 ;
			//break ;
		}
		soft_packet_number++;

		//printf("receive_bytes  %d   Ack = %2x  Status = %2x  data = %2x \n",receive_bytes,(unsigned char)  recv_data[0], (unsigned char) recv_data[1], (unsigned char) recv_data[2]);
		//printf("udp_data_copy_to_buffer_index  %d   \n",udp_data_copy_to_buffer_index);

		nof_read_data_bytes = receive_bytes-2  ;
		memcpy(&uchar_ptr[udp_data_copy_to_buffer_index], &recv_data[2], nof_read_data_bytes) ;
		udp_data_copy_to_buffer_index = udp_data_copy_to_buffer_index +  nof_read_data_bytes;

		rest_length_byte = rest_length_byte - nof_read_data_bytes ;

		//printf("rest_length_byte  %d   \n",rest_length_byte);

	} while (rest_length_byte > 0) ;
 	//printf("end \n");
    return 0;
    // -1: timeout, > 0; wrong number in receive_bytes
}


int SIS3316_ETH_UDP_SIS3316_FifoWrite(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int nof_write_words, unsigned int fifo_addr, unsigned int* data_ptr)
{
    unsigned int i;
    int return_code;
    int receive_bytes;
	int send_length;
	unsigned int nof_32bit_word;
    unsigned int data;
    //unsigned int data;
    char send_data[2048];
    char recv_data[2048];
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);

	nof_32bit_word = nof_write_words ;
	if (nof_write_words == 0) {
		nof_32bit_word = 1 ;
	}
	if (nof_write_words > 0x100) {
		nof_32bit_word = 0x100 ;
	}

	send_data[0] = 0x31 ; // send SIS3316 Register Read Req Cmd
    send_data[1] = (unsigned char)  ((nof_32bit_word-1) & 0xff);           //  lower length
    send_data[2] = (unsigned char) (((nof_32bit_word-1) >>  8) & 0xff);    //  upper length
	send_data[3] = (unsigned char)  (fifo_addr & 0xff) ;        // address(7 dwonto 0)
	send_data[4] = (unsigned char) ((fifo_addr >>  8) & 0xff) ; // address(15 dwonto 8)
	send_data[5] = (unsigned char) ((fifo_addr >> 16) & 0xff) ; // address(23 dwonto 16)
	send_data[6] = (unsigned char) ((fifo_addr >> 24) & 0xff) ; // address(31 dwonto 24)
	send_length = 7 + (4*nof_32bit_word);
	for (i=0;i<nof_32bit_word;i++) {
		data = data_ptr[i] ;
		send_data[7+(4*i)] = (unsigned char)  (data & 0xff) ;        // address(7 dwonto 0)
		send_data[8+(4*i)] = (unsigned char) ((data >>  8) & 0xff) ; // address(15 dwonto 8)
		send_data[9+(4*i)] = (unsigned char) ((data >> 16) & 0xff) ; // address(23 dwonto 16)
		send_data[10+(4*i)] = (unsigned char) ((data >> 24) & 0xff) ; // address(31 dwonto 24)
	}
	return_code = sendto(udp_socket, send_data, send_length, 0, socket_dest_addr, sizeof(struct sockaddr));
	if (return_code != send_length) {
		printf("Error: sendto return_code =  %d   \n",return_code);
	}
    //receive_bytes = read(socket,recv_data,9);
	receive_bytes = recvfrom(udp_socket, recv_data, 2000, 0,  socket_dest_addr, &addr_len);
	if (receive_bytes == -1) {
		printf("Error: recvfrom receive_bytes =  %d   \n",receive_bytes);
	}

	//printf("receive_bytes  %d   Ack = %2x  Status = %2x \n",receive_bytes, (unsigned char) recv_data[0], (unsigned char) recv_data[1]);
    if(receive_bytes ==  2 ) {
    	return_code = 0;
    }
    else {
       	return_code = -1;
    }

    return return_code ;
   // -1: timeout, > 0; wrong number in receive_bytes
}



int SIS3316_ETH_UDP_TestAccess(int udp_socket, struct sockaddr *socket_dest_addr, unsigned int cmd, unsigned int length, unsigned int reg_addr, unsigned int* reg_data)
{
    unsigned int data;
	int i;
    int return_code;
    int receive_bytes;
    unsigned int *data_ptr;
    unsigned int length_32bit;
    char send_data[128];
    char recv_data[128];
    socklen_t addr_len;
    addr_len = sizeof(struct sockaddr);

	unsigned int temp;
	temp = reg_addr ;
	for (i=0;i<10;i++) {
		send_data[3 + 0 + (4*i)] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
		send_data[3 + 1 + (4*i)] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
		send_data[3 + 2 + (4*i)] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
		send_data[3 + 3 + (4*i)] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
 		temp = temp+4 ;
	}


	length_32bit = length + 1 ;
    // send Read Req Cmd
    //send_data[0] = 0x10 ; // register read CMD
	send_data[0] = (unsigned char)  (cmd & 0xff) ;   // sis3316 register space read CMD
    send_data[1] = (unsigned char)  (length & 0xff);        //  lower length
    send_data[2] = (unsigned char) ((length >>  8) & 0xff);        //  upper length
    send_data[3] = (unsigned char)  (reg_addr & 0xff) ;        // address(7 dwonto 0)
    send_data[4] = (unsigned char) ((reg_addr >>  8) & 0xff) ; // address(15 dwonto 8)
    send_data[5] = (unsigned char) ((reg_addr >> 16) & 0xff) ; // address(23 dwonto 16)
    send_data[6] = (unsigned char) ((reg_addr >> 24) & 0xff) ; // address(31 dwonto 24)
    return_code = sendto(udp_socket, send_data, 3 + (4 * length_32bit), 0, socket_dest_addr, sizeof(struct sockaddr));

    //receive_bytes = read(socket,recv_data,9);
    receive_bytes = recvfrom(udp_socket, recv_data, 1000, 0,  socket_dest_addr, &addr_len);
 		printf("receive_bytes  %d \n",receive_bytes);

    //if(receive_bytes == 9) {
	data_ptr = (unsigned int*)&recv_data[0];
	//data = *data_ptr;
	printf("read  ");
	for (i=0;i<receive_bytes;i++) {
 		printf("%02x ", recv_data[i]);
	}
 		printf("\n");
	data_ptr = (unsigned int*)&recv_data[3];
	*reg_data = *data_ptr;

	data = *data_ptr;
	printf("register read  0x%08x \n",data);

	return 0;
   // }

    return receive_bytes;
    // -1: timeout, > 0; wrong number in receive_bytes
}



/* ***************************************************************************************************************** */

#ifdef WINDOWS
void usleep(unsigned int uint_usec)
{
    unsigned int msec;
	if (uint_usec <= 1000) {
		msec = 1 ;
	}
	else {
		msec = (uint_usec+999) / 1000 ;
	}
	Sleep(msec);

}
#endif


/* ***************************************************************************************************************** */

void get_time_clock(double* double_second)
{
#ifdef WINDOWS
	*double_second  = (double) (clock() /  CLOCKS_PER_SEC) ;

#endif
#ifdef LINUX
	timeval time ;
	gettimeofday(&time, 0);
	*double_second  = (double) (time.tv_sec + (time.tv_usec / 1000000.0));
#endif

}


