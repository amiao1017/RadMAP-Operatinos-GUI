//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research, Inc. (PGR).
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

//=============================================================================
// This example illustrates the simplest procedure of aquiring an image from a
// Ladybug camera.
//
// This program does:
//  - create a context
//  - initialize a camera
//  - start transmission of images
//  - grab an image
//  - color process the grabbed image
//  - save the 6 raw images in BMP files
//  - destroy the context
//
//=============================================================================

//=============================================================================
// System Includes
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <windows.h>

//=============================================================================
// PGR Includes
//=============================================================================

//=============================================================================
// Project Includes
//=============================================================================
#include "ladybug.h"

#define _HANDLE_ERROR \
	if( error != LADYBUG_OK ) \
   { \
   printf( \
   "Error! Ladybug library reported %s\n", \
   ::ladybugErrorToString( error ) ); \
   exit( 1 ); \
   } \
   \

#define NUM_CAMS 2

using namespace std;

double imageTimeStampToSeconds(unsigned int uiRawTimestamp)
{
	// See "Interpreting Timestamp Information" in section 2.13.37 of the Point Grey Digital Camera Register Reference.
	// This timestamp results from FireWire clock which operates at 24.576 MHz.
	//
	// The timestamp consists of three components:
	// nSecond is the seconds counter, which rolls over every 128 seconds.
	// nCycleCount is a counter with 125us resolution (1/8000 sec), which rolls over every second.
	// nCycleOffset is a counter with 40.69ns resolution (1/3072 of a cycle count), which rolls over every 125us.
	// nCycleOffset is the same resolution as the FireWire clock.

	int nSecond      = (uiRawTimestamp >> 25) & 0x7F;   // get rid of cycle_* - keep 7 bits
	int nCycleCount  = (uiRawTimestamp >> 12) & 0x1FFF; // get rid of offset
	int nCycleOffset = (uiRawTimestamp >>  0) & 0xFFF;  // get rid of *_count

	return (double)nSecond + (((double)nCycleCount+((double)nCycleOffset/3072.0))/8000.0);
}

int main( int argc, char* argv[] )
{
	LadybugContext    context[NUM_CAMS];
	LadybugError	     error;
	//LadybugImage	     image;
	LadybugCameraInfo caminfo[NUM_CAMS];
	//unsigned char*    arpBuffers[ LADYBUG_NUM_CAMERAS ];
	//char		     pszOutputFilePath[ 256];
	//unsigned int	     uiCamera;

	string outputFileName("time.txt");
	if (argc > 1) outputFileName = string(argv[1]);

	// Setup file
	cout << "Output file: " << outputFileName << '\n';
	ofstream outputFile(outputFileName);

	for (int i = 0; i < NUM_CAMS; i++) {
		//
		// Initialize context.
		//
		error = ::ladybugCreateContext( &context[i] );
		_HANDLE_ERROR;

		//
		// Initialize the first ladybug on the bus.
		//
		printf( "Initializing camera %d ...\n", i+1 );
		error = ::ladybugInitializeFromIndex( context[i], i );
		_HANDLE_ERROR;

		//
		// Get camera info
		//
		error = ladybugGetCameraInfo( context[i], &caminfo[i] );
		_HANDLE_ERROR( "ladybugGetCameraInfo()" );
	}



	//
	// Get time
	//
	SYSTEMTIME winTime;
	unsigned long cycleTime;

	while (1) {
		for (int i = 0; i < NUM_CAMS; i++) {
			GetSystemTime(&winTime);
			error = ladybugGetRegister( context[i], 0xFF100200, &cycleTime);
			_HANDLE_ERROR;

			double time = imageTimeStampToSeconds(cycleTime);

			outputFile << caminfo[i].serialHead << ' '
				<< winTime.wYear << ' ' 
				<< winTime.wMonth << ' '
				<< winTime.wDay << ' '
				<< winTime.wHour << ' ' 
				<< winTime.wMinute << ' ' 
				<< winTime.wSecond << ' ' 
				<< winTime.wMilliseconds << ' ' 
				<< cycleTime << ' ' 
				<< time 
				<< '\n';
			cout << "Camera " << caminfo[i].serialHead << " | System Time (UTC): " 
				<< winTime.wHour << ':' 
				<< winTime.wMinute << ':' 
				<< winTime.wSecond << '.' 
				<< winTime.wMilliseconds 
				<< " | ";
			cout << "Firewire Time: " << time << '\n';

		}


		if (_kbhit() && _getch() == 'q') break;
		Sleep(1000);
	}

	outputFile.close();

	//
	// Destroy the context
	//
	//printf( "Destroying context...\n" );
	//error = ::ladybugDestroyContext( &context );
	//_HANDLE_ERROR;

	//
	// Clean up the buffers
	//
	/*for( uiCamera = 0; uiCamera < LADYBUG_NUM_CAMERAS; uiCamera++ )
	{
	delete  arpBuffers[ uiCamera ];
	}*/

	printf( "Done.\n" );

	return 0;
}


