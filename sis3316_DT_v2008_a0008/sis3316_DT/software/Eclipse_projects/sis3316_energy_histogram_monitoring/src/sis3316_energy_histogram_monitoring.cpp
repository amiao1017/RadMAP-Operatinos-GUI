//============================================================================
// Name        : sis3316_energy_histogram_monitoring.cpp
// Author      : th
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================




#include <iostream>
using namespace std;


/******************************************************************************************************************************/



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


#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "histogram_shmsizes.h"

/******************************************************************************************************************************/

#define CERN_ROOT_PLOT

#ifdef CERN_ROOT_PLOT
	#include "rootIncludes.h"
	#include "sis3316_cern_root_energy_class.h"
	sis_root_canvas_with_Energy_spectrums *gl_energy_histogram ;

	sis_root_energy_spectrum_display_control_panel *gl_histogram_display_control ;


#endif


/******************************************************************************************************************************/



unsigned int i;
unsigned int return_code;
int shm_histogram_fd;
size_t shm_histogram_pg_size;
unsigned int  *uint_shm_histogram_start_ptr;

unsigned int  *uint_sis3316_histogram_start_ptr;
unsigned int  *uint_energy_histogram_start_ptr;


int main(int argc, char* argv[]) {
	cout << "sis3316_energy_histogram_monitoring" << endl; //


	shm_histogram_fd=shm_open("SIS3316_SHM_HISTOGRAM_BUF", O_RDONLY ,0);
    printf("return code: shm_open SIS3316_SHM_HISTOGRAM_BUF: %8x \n",shm_histogram_fd);
    if (shm_histogram_fd < 0) {
	return(-1);
    }
    //system("chmod 666 /dev/shm/SIS3316_SHM_HISTOGRAM_BUF");
    shm_histogram_pg_size = SHM_ENERGY_HISTOGRAM_SIZE;

    /* Map   */
	uint_shm_histogram_start_ptr = (unsigned int *)mmap(0, shm_histogram_pg_size, PROT_READ, MAP_SHARED, shm_histogram_fd, 0);



#ifdef CERN_ROOT_PLOT

    TApplication theApp("SIS3316 Application: Test", &argc, (char**)argv);


	char canvas_display_control_text[128] ;
	int canvas_display_control_x_pos ;
	int canvas_display_control_y_pos ;
	int canvas_display_control_x_size ;
	int canvas_display_control_y_size ;

/**************************************************/

	snprintf(canvas_display_control_text,128,"Display Energy Spectrum") ;
	canvas_display_control_x_pos  = 600 ;
	canvas_display_control_y_pos  = 10 ;
	canvas_display_control_x_size = 1000 ;
	canvas_display_control_y_size = 700 ;

	gl_histogram_display_control      = new sis_root_energy_spectrum_display_control_panel(canvas_display_control_text,
													canvas_display_control_x_pos, canvas_display_control_y_pos,
													canvas_display_control_x_size, canvas_display_control_y_size);




#endif
/**************************************************/


/**************************************************/
/**************************************************/






	do {
		gSystem->ProcessEvents();  // handle GUI events
		gl_histogram_display_control->update_parameters() ;

		uint_sis3316_histogram_start_ptr = uint_shm_histogram_start_ptr + SHM_SIS3316_HISTOGRAM_OFFSET ;
		uint_energy_histogram_start_ptr  = uint_sis3316_histogram_start_ptr + (gl_histogram_display_control->channelSelect_index * SHM_ENERGY_HISTOGRAM_CHANNEL_OFFSET) ;

		gl_histogram_display_control->sis3316_write_histogram(0, 0x10000, uint_energy_histogram_start_ptr) ;
		gl_histogram_display_control->sis3316_draw_histogram() ;
		printf("gl_histogram_display_control->sis3316_draw_histogram gl_histogram_display_control->channelSelect_index %d\n",gl_histogram_display_control->channelSelect_index);

/*****************************/
		for (i=0;i<10;i++) {
			gSystem->ProcessEvents();  // handle GUI events
			usleep(100000) ;
		}


		gSystem->ProcessEvents();  // handle GUI events
	} while (1) ;


	return 0;
}
