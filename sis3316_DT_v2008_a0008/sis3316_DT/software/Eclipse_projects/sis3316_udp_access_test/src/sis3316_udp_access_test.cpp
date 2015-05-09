/***************************************************************************/
/*                                                                         */
/*  LINUX: Scientific                                                      */
/*  Filename: sis3316_udp_access_test.cpp                                  */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 07.09.2014                                       */
/*  last modification:    07.09.2014                                       */
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

#include <time.h>




#ifdef LINUX
	#include <sys/types.h>
	#include <sys/socket.h>

	#include <sys/uio.h>

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




/* ***************************************************************************************************************** */
//handling cmd line input
char gl_cmd_ip_string[64];
unsigned int gl_cmd_ethernet_device_no = 0;

char gl_command[256];


/* ***************************************************************************************************************** */
int main(int argc, char* argv[]) {
//int main() {
	cout << "sis3316_udp_access_test" << endl; //

	int i;
	int return_code;

	int udp_socket;
	unsigned char *udp_recv_buffer ;
	struct sockaddr_in SIS3316_sock_addr_in   ;

	unsigned int udp_port ;
	unsigned int data ;
	unsigned int loop_counter ;

	char  eth_device[64] ;

	char ch ;
	char ch_string[64] ;

	//strcpy(gl_cmd_ip_string,"212.60.16.211") ;
	strcpy(gl_cmd_ip_string,"192.168.1.100") ;



	/********************************************************************************************************************************************/
	/*  Command interpreter                                                                                                                     */
	/********************************************************************************************************************************************/

	  /* Save command line into string "command" */
	  memset(gl_command,0,sizeof(gl_command));
	  // memset(startchoice,0,sizeof(startchoice));
	  for (i=0;i<argc;i++) {
	      strcat(gl_command,argv[i]);
	      strcat(gl_command," ");
	  }

	    if (argc > 1) {
		/* Save command line into string "command" */
		memset(gl_command,0,sizeof(gl_command));
		// memset(startchoice,0,sizeof(startchoice));
		for (i=1;i<argc;i++) {
		    strcat(gl_command,argv[i]);
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


	/********************************************************************************************************************************************/
	/*  Reset and check UDP Connection                                                                                                          */
	/********************************************************************************************************************************************/

	      SIS3316_ETH_UDP_ResetCmd(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in);
		  unsigned int  timeout_counter;
		  unsigned int  print_counter;
	 	  unsigned int  led_flag;
	      led_flag = 1 ;
	      print_counter = 0 ;
	      timeout_counter = 0 ;
	      loop_counter = 0 ;
	      // check UDP Connection

	    // set UDP Interface Request bit (access to register 0x20 and higher)
		SIS3316_ETH_UDP_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x10, 0x1); // set request bit




		  do {
	        print_counter++ ;
	        loop_counter++ ;

			return_code = SIS3316_ETH_UDP_RegisterRead( udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x4, &data);
			if (return_code == 0) {
				if(print_counter == 10000) {
					if (led_flag == 1) {
						led_flag = 0 ;
						SIS3316_ETH_UDP_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x0, 0x10000); // clr Led
					}
					else {
						led_flag = 1 ;
						SIS3316_ETH_UDP_RegisterWrite(udp_socket, (struct sockaddr *)&SIS3316_sock_addr_in, 0x0, 0x1); // set Led
					}

					print_counter = 0 ;
					printf("Module and Version register:  0x%08x   loop_counter = %d     timeout_counter  = %d   ",data, loop_counter, timeout_counter);
					if (led_flag == 1) {
						printf("Led U is On \n");
					}
					else {
						printf("Led U is Off \n");
					}
				}
		    }
		    else {
				timeout_counter++;
				if (return_code == -1) {
					printf("Module and Version register: read timeout %d \n",return_code);
				}
				else {
					printf("Module and Version register: wrong read length (must be 9) %d \n",return_code);
				}
		    }

		} while (1) ;
	    //  } while (return_code != 0) ;


		return 0;
	}











/* ***************************************************************************************************************** */
/* ***************************************************************************************************************** */
/* ***************************************************************************************************************** */
/* ***************************************************************************************************************** */



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
 		printf("receive_bytes  %d \n",receive_bytes);

			//		  memcpy(&uint_adc_data_buffer[udp_data_copy_to_buffer_index], &recv_data[2], receive_bytes-2) ;

    if((unsigned int)receive_bytes == (3 + (4*nof_32bit_word))) {
		for (i=0;i<nof_32bit_word;i++) {
			data_ptr = (unsigned int *)&recv_data[3+(4*i)] ;
			reg_data_ptr[i] = *data_ptr ;
			printf("register read  0x%08x \n",reg_data_ptr[i]);
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

    if(receive_bytes ==  3 ) {
		return 0;
    }

    return receive_bytes;
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
    //unsigned int reg_data;
    //unsigned int data;
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
			printf("Error: recvfrom receive_bytes =  %d   \n",receive_bytes);
			printf("receive_bytes  %d   Ack = %2x  Status = %2x  data = %2x \n",receive_bytes, (unsigned char) recv_data[0], (unsigned char) recv_data[1], (unsigned char) recv_data[2]);
			printf("soft_packet_number  %d   \n",soft_packet_number);
			printf("udp_data_copy_to_buffer_index  %d   \n",udp_data_copy_to_buffer_index);
			return -1 ;
			//break ;
		}
		soft_packet_number++;

		printf("receive_bytes  %d   Ack = %2x  Status = %2x  data = %2x \n",receive_bytes,(unsigned char)  recv_data[0], (unsigned char) recv_data[1], (unsigned char) recv_data[2]);
		//printf("udp_data_copy_to_buffer_index  %d   \n",udp_data_copy_to_buffer_index);

		nof_read_data_bytes = receive_bytes-2  ;
		memcpy(&uchar_ptr[udp_data_copy_to_buffer_index], &recv_data[2], nof_read_data_bytes) ;
		udp_data_copy_to_buffer_index = udp_data_copy_to_buffer_index +  nof_read_data_bytes;

		rest_length_byte = rest_length_byte - nof_read_data_bytes ;

		//printf("rest_length_byte  %d   \n",rest_length_byte);

	} while (rest_length_byte > 0) ;
 	printf("end \n");
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

		printf("receive_bytes  %d   Ack = %2x  Status = %2x \n",receive_bytes, (unsigned char) recv_data[0], (unsigned char) recv_data[1]);

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


