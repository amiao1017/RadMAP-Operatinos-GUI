//sis3316_ethernet_access_class
/***************************************************************************/
/*                                                                         */
/*  Filename: sis3316_ethernet_access_class.h                              */
/*                                                                         */
/*  Funktion:                                                              */
/*                                                                         */
/*  Autor:                TH                                               */
/*  date:                 26.07.2012                                       */
/*  last modification:    16.09.2014                                       */
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

#ifndef _SIS3316_ETHERNET_ACCESS_CLASS_
#define _SIS3316_ETHERNET_ACCESS_CLASS_

#include "project_system_define.h"		//define LINUX or WIN


/* nof_read_words: max. 0x10000 (64k 32-bit words = 256KBytes )  */
//#define UDP_MAX_NOF_READ_32bitWords    0x10000		   // SIS3316 sends N packets with full speed	

// use in case of slow Ethernet interface (for example MAC USB to Gigabit interface)
//#define UDP_MAX_NOF_READ_32bitWords    1400            // SIS3316 sends only 1 packet with full speed for each request
#define UDP_MAX_NOF_READ_32bitWords    360            // SIS3316 sends only 1 packet with full speed for each request (= 1440 Bytes)




/* nof_write_words: max. 0x100 (256 32-bit words = 1KBytes )  */
#define UDP_MAX_NOF_WRITE_32bitWords    0x100



#ifdef LINUX
	typedef char CHAR;
	typedef unsigned int UINT;
	typedef unsigned int* PUINT;

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

#ifdef WIN
	#include <Windows.h>
#endif

#include "vme_interface_class.h"




class sis3316_eth : public  vme_interface_class
{
private:
	CHAR char_messages[128] ;
	int udp_socket_status;
	int udp_socket;
	unsigned int udp_port ;
	struct sockaddr_in SIS3316_sock_addr_in   ;
	struct sockaddr_in myPC_sock_addr   ;
	unsigned int recv_timeout_sec  ;
	unsigned int recv_timeout_usec  ;
    char udp_send_data[2048];
    char udp_recv_data[16384];




public:
	sis3316_eth (void);

	int vmeopen ( void );
	int vmeclose( void );
	int get_vmeopen_messages( CHAR* messages, UINT* nof_found_devices );

	//int get_device_informations( USHORT* idVendor, USHORT* idProduct, USHORT* idSerNo, USHORT* idFirmwareVersion,  USHORT* idDriverVersion );
	int get_UdpSocketStatus( void );
	int get_UdpSocketPort(void );
	int set_UdpSocketOptionTimeout( void );
	int set_UdpSocketOptionBufSize( int sockbufsize );
	int set_UdpSocketBindToDevice( char* eth_device);
	int set_UdpSocketBindMyOwnPort( char* pc_ip_addr_string);
	int set_UdpSocketSIS3316_IpAddress( char* sis3316_ip_addr_string);

	int udp_reset_cmd( void);
	int udp_register_read (UINT addr, UINT* data);
	int udp_sis3316_register_read ( unsigned int nof_read_registers, UINT* addr_ptr, UINT* data_ptr);
	int udp_register_write (UINT addr, UINT data);
	int udp_sis3316_register_write ( unsigned int nof_read_registers, UINT* addr_ptr, UINT* data_ptr);

private:
	int udp_sub_sis3316_fifo_read ( unsigned int nof_read_words, UINT  addr, UINT* data_ptr);
	int udp_sis3316_fifo_read ( unsigned int nof_read_words, UINT  addr, UINT* data_ptr, UINT* got_nof_words );

	int udp_sub_sis3316_fifo_write ( unsigned int nof_write_words, UINT  addr, UINT* data_ptr);
	int udp_sis3316_fifo_write ( unsigned int nof_write_words, UINT addr, UINT* data_ptr, UINT* written_nof_words );

public:
	int vme_A32D32_read (UINT addr, UINT* data);
	int vme_A32DMA_D32_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32BLT32_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32MBLT64_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2EVME_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2ESST160_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2ESST267_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2ESST320_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );

	int vme_A32DMA_D32FIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32BLT32FIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32MBLT64FIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2EVMEFIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2ESST160FIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2ESST267FIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );
	int vme_A32_2ESST320FIFO_read (UINT addr, UINT* data, UINT request_nof_words, UINT* got_nof_words );




	int vme_A32D32_write (UINT addr, UINT data);
	int vme_A32DMA_D32_write (UINT addr, UINT* data, UINT request_nof_words, UINT* written_nof_words );
	int vme_A32BLT32_write (UINT addr, UINT* data, UINT request_nof_words, UINT* written_nof_words );
	int vme_A32MBLT64_write (UINT addr, UINT* data, UINT request_nof_words, UINT* written_nof_words );
	int vme_A32DMA_D32FIFO_write (UINT addr, UINT* data, UINT request_nof_words, UINT* written_nof_words );
	int vme_A32BLT32FIFO_write (UINT addr, UINT* data, UINT request_nof_words, UINT* written_nof_words );
	int vme_A32MBLT64FIFO_write (UINT addr, UINT* data, UINT request_nof_words, UINT* written_nof_words );

	int vme_IRQ_Status_read( UINT* data ) ;





};

#endif
