#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <iomanip>
#include <assert.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <queue>
#include <time.h>
#include <conio.h>
#include <string>

#include "resonon_imager_x.h"
#include "resonon_imager_base.h"


const int LINE_COUNT = 1000; //size of datacube to record
const std::string filename = "NIRHyperSpecDataCube"; //location to save datacube
std::string directoryName;
std::string header_filename; // header filename is the same with '.hdr' appended, see below
int framesize; //size of a single frame in records (number of elements in array)
int cubesize; //size of complete datacube in records (number of elements in array)
std::queue<std::pair<unsigned short *, int> > myQueue;
HANDLE myMutex;
HANDLE myThread;

void writeThread(void *);

int counter = 0;
int stpReceived = 0;
int calls = 0;

Resonon::PikaNIR imager;

int main(int argc, char* argv[])
{
	unsigned short * buffer;
	bool free_buffer = false; //keep track of whether we've allocated memory that must be released
	try
	{
		// Initialize imager.
		imager.connect(); //Be prepared to catch exceptions if the imager is not physically connected to the computer
		//imager.set_calibration_file("XEVA3986_TrueNUC_LG_RT_3986.xca"); // replace this filename with the calibration file that comes with your imager

		// Set spectral calibration info
		imager.set_slope(5.419);
		imager.set_intercept(650.11);

		// Set Windowing, here we are using the default windowing.  If default settings are used, this call is not
		// required, but it is shown here for completeness.  The default windowing for a Pika NIR uses the full camera
        // frame of 256 bands by 320 samples.
		imager.set_window_bounds(0, 256, 0, 320);

		// Set Camera properties
		imager.set_framerate(100.0); //Hz
		imager.set_integration_time(7.0); //milliseconds				

		imager.set_external_trigger();
		
		if(imager.is_trigger_external())
		{
		std::cout << "external triggering set \n";
		}

		//Print out the imager status
		std::cout << "Imager Status:\n";
		std::cout << "--------------\n";
		std::cout << std::setw(18) << "Imager Type: "  << imager.get_imager_type() << "\n";
		std::cout << std::setw(18) << "Integration Time: "  << imager.get_integration_time();
		std::cout << " [min: " << imager.get_min_integration_time();
		std::cout << ", max: " << imager.get_max_integration_time() << "]" << "\n";
		//std::cout << std::setw(18) << "Gain: " << imager.get_gain();							//potential crash point
		//std::cout << " [min: " << imager.get_min_gain();
		//std::cout << ", max: " << imager.get_max_gain() << "]" << "\n";
		std::cout << std::setw(18) << "Bands: " << imager.get_band_count();
		std::cout << " [min: " << imager.get_window_start_band();
		std::cout << ", max: " << imager.get_window_end_band() - 1 << "]" << "\n";
		std::cout << std::setw(18) << "Samples: " << imager.get_sample_count();
		std::cout << " [min: " << imager.get_window_start_sample();
		std::cout << ", max: " << imager.get_window_end_sample() - 1 << "]" << "\n";

		std::cout << "\nImage status printed but framesize not computed" << std::endl;
		
		//allocate memory for datacube
		framesize = imager.get_band_count() * imager.get_sample_count();
		assert (framesize * sizeof(unsigned short) == imager.get_frame_buffer_size_in_bytes());
		cubesize = framesize * LINE_COUNT;

		std::cout << "\nFramesize computed" << std::endl;
	
		myMutex = CreateMutex(NULL, FALSE, NULL);
		if (myMutex == NULL) 
    	{
      		printf("CreateMutex error: %d\n", GetLastError());
       		exit(EXIT_FAILURE);
   		}

   		std::cout << "\nMutex Created" << std::endl;

   		myThread = (HANDLE)_beginthread(writeThread, 0, NULL);	//makeCube called here
		if( myThread == NULL )
    	{
       		printf("_beginthread error: %d\n", GetLastError());
       		exit(EXIT_FAILURE);
   		}

   		std::cout << "\nThread Created" << std::endl;

   		bool grabbingFrames = FALSE;

   		//start thread that looks at buffer and pops and saves datacube if data exists

		// Start recording the data

		while(stpReceived == 0)
		{
			std::cout << "\nRecording Data" << std::endl;
			if(!grabbingFrames)
			{
				imager.start_frame_grabbing();
				grabbingFrames = TRUE;
			}
			buffer = new unsigned short[cubesize];
			if (buffer == 0)
			{
				std::cerr << "Error: memory could not be allocated for datacube";
				exit(EXIT_FAILURE);
			}
			free_buffer = true; //if an exception occurs below make sure we free the just allocated block of memory


			while (counter < LINE_COUNT  && stpReceived == 0)			
			{
				imager.get_frame(&buffer[counter * framesize]);
				std::cout << "Line " << counter + 1 << std::endl;
				counter++;
			}
			std::pair<unsigned short *, int> myPair = std::make_pair(buffer,counter);
			//std::cout << "\nMade data pair" << std::endl;
			WaitForSingleObject(myMutex,INFINITE);		//ownMutex?
			//std::cout << "\nGot Mutex" << std::endl;
			myQueue.push(myPair);						//put buffer to queue
			//std::cout << "\nPushed pair to queue" << std::endl;
			ReleaseMutex(myMutex);						//Release Mutex
    		//std::cout << "\nReleased Mutex" << std::endl;
    		counter = 0;
    	}
    	WaitForSingleObject(myThread,INFINITE);
       	CloseHandle(myThread);
       	//std::cout << "Thread Closed" << std::endl;
       	CloseHandle(myMutex);
       	//std::cout << "Mutex Closed" << std::endl;

		imager.stop_frame_grabbing();
		//std::cout << "Frame Grabbing Stopped" << std::endl;	 
		grabbingFrames = FALSE;
		imager.disconnect(); 			//disconnect
		//std::cout << "Imager Disconnected" << std::endl;
		
	} catch (std::exception const & e)
	{
		std::cerr << "Error: " << e.what();
		if (free_buffer == true)
			delete [] buffer;
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

void makeCube(std::pair<unsigned short *,int> myData)
{
	calls++;
	SYSTEMTIME st, lt;
	GetSystemTime(&st);
	GetLocalTime(&lt);

	std::string fileDayST = "%02d%02d%02d" %(st.wYear,st.wMonth,st.wDay);
	std::string fileDayLT = "%02d%02d%02d" %(lt.wYear,lt.wMonth,lt.wDay);

	std::cout << "ST looks like " << fileDayST << "\n" << "LT looks like " << fileDayLT << "\n" << std::endl;

	std::string fileTimeST = "%02d:%02d:%02d.%03d" %(st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
	std::string fileTimeLT = "%02d:%02d:%02d.%03d" %(lt.wHour,lt.wMinute,lt.wSecond,lt.wMilliseconds);

	std::cout << "ST looks like " << fileTimeST << "\n" << "LT looks like " << fileTimeLT << "\n" << std::endl;

	std::string directoryTimeST = "%02d%02d%02d%03d" %(st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
	std::string directoryTimeLT = "%02d%02d%02d%03d" %(lt.wHour,lt.wMinute,lt.wSecond,lt.wMilliseconds);

	std::cout << "ST looks like " << directoryTimeST << "\n" << "LT looks like " << directoryTimeLT << "\n" << std::endl;

	/*timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;
	//time_t rawtime;
	struct tm * timeinfo;
	//time (&rawtime);
	//timeinfo = localtime(&rawtime);
	timeinfo = localtime(&curTime.tv_sec)
	char fileDay [100];
	//strftime(fileDay,100,"%Y%m%d",timeinfo);
	strftime(fileDay,100,"%Y%m%d",timeinfo);
	char timeBufferC [100];
	strftime(timeBufferC,100,"%H:%M:%S",timeinfo);		//fileTime has colons
	char fileTime [100];
	sprintf(fileTime, "%s:%d", timeBufferC, milli);
	char timeBuffer [100];
	strftime(timeBuffer,100,"%H%M%S",timeinfo);	//directoryTime is for filename so no colons
	char directoryTime [100];
	sprintf(directoryTime, "%s%d", timeBuffer, milli);	//pos %03d for leading zeros*/
	std::string saveName;
	std::string cubeSaveName;
	std::string stringCalls;
	saveName = filename;
	stringCalls = std::to_string(calls);
	saveName.append(stringCalls);
	saveName.append("-");
	saveName.append(fileDayST);
	//std::cout << "\nmakeCube successfully called\n" << std::endl;
	std::cout << "Recording Complete\nWriting Datacube to Disk" << std::endl;			//write an ENVI compatible header file
	if (calls <= 1)
	{
		directoryName = ".\\";
		directoryName.append(fileDayST);
		directoryName.append(directoryTimeST);
		//directoryName.append("/");
	}
	CreateDirectoryA(directoryName.c_str(),NULL);
	header_filename = directoryName;
	header_filename.append("\\");
	header_filename.append(saveName);
	header_filename.append(".hdr");
	std::ofstream outfile(header_filename.c_str());
	outfile << "ENVI\n";
	outfile << "File created at " << fileTimeST << " On " << fileDayST << "\n";
	outfile << "interleave = bil\n";
	outfile << "data type = 12\n";
	outfile << "bit depth = 12\n";
	outfile << "samples = " << imager.get_sample_count() << "\n";
	outfile << "bands = " << imager.get_band_count() << "\n";
	outfile << "lines = " << LINE_COUNT << "\n";
	outfile << "framerate = " << imager.get_framerate() << "\n";
	outfile << "shutter = " << imager.get_integration_time() << "\n";
	outfile << "wavelength = {";
	outfile << std::setprecision(5);
	for(int i = imager.get_window_start_band(); i < imager.get_window_end_band(); i++)
	{
		outfile << imager.get_wavelength_at_band(i);
		if (i < imager.get_window_end_band() - 1)
			outfile << ", ";
	}
	outfile << "}\n";
	outfile.close();

	//write data file
	std::ofstream cubefile;	
	cubeSaveName = directoryName + "\\" + saveName + ".bil";		
	cubefile.open(cubeSaveName.c_str(), std::ios::out | std::ios::binary);
	//cubefile.write((const char*) buffer, cubesize * sizeof(unsigned short));
	cubefile.write((const char*) myData.first, framesize * myData.second * sizeof(unsigned short));
	cubefile.close();
	

	// free allocated resources
	delete [] myData.first;	
	//std::cout << "Buffer Deleted" << std::endl;
	std::cout << "Done." << std::endl;	
}

void writeThread(void *)
{
	while (stpReceived == 0)
	{
		if (_kbhit()) {						
			char key = _getch();
			if (key == 'q' || key == 'Q') {
				stpReceived = 1;
			}
		}

		WaitForSingleObject(myMutex,INFINITE);		//ownMutex?	
		//std::cout << "\nWrite thread owns Mutex" << std::endl;			
		if (!myQueue.empty()) //while there is still data in the queue keep writing cubes
		{
			//std::cout << "\nQueue not empty" << std::endl;
			std::pair<unsigned short *, int> myData = myQueue.front();
			//std::cout << "\nGot pair from queue" << std::endl; 
			myQueue.pop();
			//std::cout << "\nPop data from queue" << std::endl;
			ReleaseMutex(myMutex);
			//std::cout << "\nRelased Mutex" << std::endl;
			makeCube(myData);
			//std::cout << "\nmakeCube called" << std::endl;
		}
		else
		{
		ReleaseMutex(myMutex);						
		Sleep(1000);
		}
	}
	while (!myQueue.empty()) //while there is still data in the queue keep writing cubes
	{
		std::pair<unsigned short *, int> myData = myQueue.front();
		myQueue.pop();
		makeCube(myData);
	} 
}
