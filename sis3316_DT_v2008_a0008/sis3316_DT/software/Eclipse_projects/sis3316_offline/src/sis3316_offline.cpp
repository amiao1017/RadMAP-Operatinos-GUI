//============================================================================
// Name        : sis3316_offline.cpp
// Author      : th
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "project_system_define.h"		//define LINUX or WINDOWS


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


/*===========================================================================*/
/* Globals					  			     */
/*===========================================================================*/
#define MAX_NUMBER_LWORDS_64MBYTE			0x1000000       /* 64MByte */

unsigned int gl_ch_data[MAX_NUMBER_LWORDS_64MBYTE] ;



FILE *gl_FILE_DataEvenFilePointer           ;

/*===========================================================================*/
/* Prototypes			                               		  			     */
/*===========================================================================*/

int ReadBufferHeaderCounterNofChannelToDataFile (unsigned int* indentifier, unsigned int* bank_loop_no, unsigned int* channel_no, unsigned int* nof_events, unsigned int* event_length, unsigned int* maw_length, unsigned int* reserved);
int ReadEventsFromDataFile (unsigned int* memory_data_array, unsigned int nof_write_length_lwords);


/* ***************************************************************************************************************** */
int main(int argc, char* argv[]) {
	cout << "sis3316_offline" << endl; // prints sis3316_offline

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
		strcpy(root_graph_text,"SIS3316 Graph: Raw data") ;
		gl_graph_raw      = new sis_root_graph(root_graph_text, root_graph_x, root_graph_y, root_graph_x_size, root_graph_y_size) ;

	#endif

		#ifdef raus
		// file read
		gl_FILE_DataEvenFilePointer = fopen("ne213/sis3316_test_data.dat","rb") ;
		if (gl_FILE_DataEvenFilePointer == NULL) {
			printf("gl_FILE_DataEvenFilePointer == NULL \n");
			return -1;
		}
		else {
			printf("file is opened \n");
		}
		#endif
		unsigned valid_BankBufferHeader_valid_flag ;
		int nof_read ;
		unsigned buffer_no ;
		unsigned i_event ;
		unsigned nof_events ;
		unsigned buffer_length ;
		unsigned event_length ;
		unsigned header_length ;
		unsigned sample_length ;
		unsigned channel_no ;
		unsigned maw_buffer_length ;
		unsigned i_ch ;
		unsigned headerformat ;
		unsigned header_indentifier ;
		unsigned header_reserved ;


		//gl_graph_raw->sis3316_draw_XYaxis (50000); // clear and draw X/Y
		//gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y
		unsigned bank_buffer_counter ;

		char filename[128]  ;
		unsigned int i_file;

		bank_buffer_counter = 8 ;
		for (i_file=0; i_file<10; i_file++) {

			//sprintf(filename,"../data_files/sample_test_gui/sis3316_test_data_%d.dat",i_file ) ;
			//sprintf(filename,"../data_files/external/sis3316_test_data_%d.dat",i_file ) ;
			sprintf(filename,"../data_files/internal/sis3316_test_data_%d.dat",i_file ) ;
			printf("%s\n",filename) ;
			gl_FILE_DataEvenFilePointer = fopen(filename,"rb") ;
			if (gl_FILE_DataEvenFilePointer != NULL) {


				do {


					valid_BankBufferHeader_valid_flag = 0 ;
					nof_read = ReadBufferHeaderCounterNofChannelToDataFile (&header_indentifier, &buffer_no, &channel_no, &nof_events, &event_length, &maw_buffer_length, &header_reserved) ;
					//event_length = buffer_length / nof_events ;
					buffer_length = event_length * nof_events ;


					printf("\n");
					printf("header information: \tnof_read = %d    \tindentifier = %d   \tbuffer_no = %d  \tchannel_no = %d   \n", nof_read, header_indentifier, buffer_no, channel_no);
					printf("header information: \tnof_events = %d  \tbuffer_length = %d   \tevent_length = %d    \n", nof_events, buffer_length, event_length );
					printf("header information: \tmaw_buffer_length = %d \treserved = %d \n",  maw_buffer_length, header_reserved);
					unsigned uint_plot_axis_flag;
					if (nof_read == 8) {
						uint_plot_axis_flag = 1 ;
						valid_BankBufferHeader_valid_flag = 1 ;
						nof_read =  ReadEventsFromDataFile (gl_ch_data, buffer_length);
						headerformat =  (gl_ch_data[0] & 0xf) ;

						printf("nof_read = %d  \tch = %d   \theaderformat = 0x%02X \n",  nof_read,	(gl_ch_data[0] & 0xfff0) >> 4, headerformat);
						i_ch = (gl_ch_data[0] & 0xfff0) >> 4 ;

						header_length = 3 ; // if headerformat == 0
						if((headerformat & 0x1) == 1) {header_length = header_length + 7; }
						if((headerformat & 0x2) == 2) {header_length = header_length + 2; }
						if((headerformat & 0x4) == 4) {header_length = header_length + 3; }
						if((headerformat & 0x8) == 8) {header_length = header_length + 2; }

						sample_length = 2 * (gl_ch_data[header_length-1] & 0x3ffffff) ; // if headerformat == 0
						if(uint_plot_axis_flag == 1) {
							//printf("uint_plot_axis_flag == 1\n");
							uint_plot_axis_flag = 0 ;
							gl_graph_raw->sis3316_draw_XYaxis (sample_length); // clear and draw X/Y
						}
						for (i_event = 0; i_event < nof_events; i_event++) {
							if (i_event<10) { // plot ony the first 10. events
								//if (i_event<1) { // plot ony 1. event
								gl_graph_raw->sis3316_draw_chN (sample_length, &gl_ch_data[i_event*(event_length) + header_length], i_ch&0xf); //
							}

						}

					}
					else {
						valid_BankBufferHeader_valid_flag = 0 ;
					}
				} while((valid_BankBufferHeader_valid_flag == 1) ) ;
				printf("\n");

				fclose(gl_FILE_DataEvenFilePointer);
				printf("file closed and finished   \n");
				bank_buffer_counter++;
			}

		}



		#ifdef raus
		fclose(gl_FILE_DataEvenFilePointer);
		printf("file closed and finished   \n");
		#endif
		do {
			gSystem->ProcessEvents();  // handle GUI events
		} while (1) ;


	return 0;
}


//---------------------------------------------------------------------------

int ReadBufferHeaderCounterNofChannelToDataFile (unsigned int* indentifier, unsigned int* bank_loop_no, unsigned int* channel_no, unsigned int* nof_events, unsigned int* event_length, unsigned int* maw_length, unsigned int* reserved)
{
int nof_read ;
int data ;
  //header
	nof_read=fread(&data, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(indentifier, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(bank_loop_no, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(channel_no, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(nof_events, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(event_length, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(maw_length, 0x4, 0x1, gl_FILE_DataEvenFilePointer);
	nof_read=nof_read+fread(reserved, 0x4, 0x1, gl_FILE_DataEvenFilePointer);

 	return nof_read;
}


//---------------------------------------------------------------------------
int ReadEventsFromDataFile (unsigned int* memory_data_array, unsigned int nof_write_length_lwords)
{
int nof_read ;

	nof_read=fread(memory_data_array, 0x4, nof_write_length_lwords, gl_FILE_DataEvenFilePointer);
 	return nof_read;
}


