#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <iomanip>
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <queue>

#include "resonon_imager_pgr.h"
#include "resonon_imager_base.h"


const int LINE_COUNT = 1000; //size of datacube to record
const std::string filename = "2iHyperSpecData.bil"; //location to save datacube
std::string header_filename; // header filename is the same with '.hdr' appended, see below
int framesize; //size of a single frame in records (number of elements in array)
int cubesize; //size of complete datacube in records (number of elements in array)
unsigned short * buffer; //buffer to hold image data
std::queue<pair<int *, lineCounter>> myQueue;
HANDLE myMutex;

bool free_buffer = false; //keep track of whether we've allocated memory that must be released

//allocate memory for datacube
framesize = imager.get_band_count() * imager.get_sample_count();
assert (framesize * sizeof(unsigned short) == imager.get_frame_buffer_size_in_bytes());

cubesize = framesize * LINE_COUNT;

void makeCube(void *);
int counter = 0;

int main()
{
	try
	{
		// Initialize imager.
		Resonon::Pika2I imager;
		imager.connect(); //Be prepared to catch exceptions if the imager is not physically connected to the computer

		// Set spectral calibration info
		imager.set_slope(1.0447);
		imager.set_intercept(393.28);

		// Set Windowing, here we are using the default windowing.  If default settings are used, this call is not
		// required, but it is shown here for completeness.  The default windowing for a Pika 2I uses the full camera
        // frame of 240 bands by 640 samples.
		imager.set_window_bounds(0, 240, 0, 640);

		// Set Camera properties
		imager.set_framerate(100.0); //Hz					
		imager.set_integration_time(7.0); //milliseconds	
		imager.set_gain(0.0); //dB 						

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
		std::cout << std::setw(18) << "Gain: " << imager.get_gain();
		std::cout << " [min: " << imager.get_min_gain();
		std::cout << ", max: " << imager.get_max_gain() << "]" << "\n";
		std::cout << std::setw(18) << "Bands: " << imager.get_band_count();
		std::cout << " [min: " << imager.get_window_start_band();
		std::cout << ", max: " << imager.get_window_end_band() - 1 << "]" << "\n";
		std::cout << std::setw(18) << "Samples: " << imager.get_sample_count();
		std::cout << " [min: " << imager.get_window_start_sample();
		std::cout << ", max: " << imager.get_window_end_sample() - 1 << "]" << "\n";
		
	
		
		// Start recording the data
		while(cmd == str)
		{
			myMutex = CreateMutex(Null, FALSE, queueKey);
			if (ghMutex == NULL) 
    		{
        		printf("CreateMutex error: %d\n", GetLastError());
        		return 1;
    		}
			std::cout << "\nRecording Data" << std::endl;
			imager.start_frame_grabbing();
			buffer = new unsigned short[cubesize];
			if (buffer == 0)
			{
				std::cerr << "Error: memory could not be allocated for datacube";
				exit(EXIT_FAILURE);
			}
			free_buffer = true; //if an exception occurs below make sure we free the just allocated block of memory

			while (counter < LINE_COUNT)
			{
				imager.get_frame(&buffer[i * framesize]);
				std::cout << "Line " << i + 1 << std::endl;
				counter++;
			}
			
			mutexStatus = WaitForSingleObject(myMutex,500);		//ownMutex?
			switch(mutexStatus)
			{
				case WAIT_OBJECT_0:
					__try 
					{
						myQueue.push(buffer);						//put buffer to queue
					}
					__finally
					{
						ReleaseMutex(myMutex);						//Release Mutex
					}
					break;
				case WAIT_ABANDONED:
					//Mutex owned by other. can set wait to infinite and not have switch
			}

			myThread = CreateThread(NULL, 0, makeCube, NULL, 0, myThread);	//makeCube called here
			if( aThread[i] == NULL )
    		{
        		printf("CreateThread error: %d\n", GetLastError());
        		return 1;
    		}
    	}

       	CloseHandle(myThread);
       	CloseHandle(myMutex);
       	while (!myQueue.empty())
       	{
       		makeCube();
       	}
		
	} catch (std::exception const & e)
	{
		std::cerr << "Error: " << e.what();
		if (free_buffer == true)
			delete [] buffer;
		exit(EXIT_FAILURE);
	}

	imager.stop_frame_grabbing();	//probably not the correct place but when code ends need to stop taking data and 
	imager.disconnect(); 			//disconnect

	return EXIT_SUCCESS;

}


void makeCube(void *)
{
	if (cmd == str)
	{
		mutexStatus = WaitForSingleObject(myMutex,500);		//ownMutex?
			switch(mutexStatus)
			{
				case WAIT_OBJECT_0:
					__try 
					{					
						while (!myQueue.empty()) //while there is still data in the queue keep writing cubes
						{
							std::cout << "Recording Complete\nWriting Datacube to Disk" << std::endl;			//write an ENVI compatible header file
							header_filename = filename + ".hdr";
							std::ofstream outfile(header_filename.c_str());
							outfile << "ENVI\n";
							outfile << "interleave = bil\n";
							outfile << "data type = 12\n";
							outfile << "bit depth = 12\n";
							outfile << "samples = " << imager.get_sample_count() << "\n";
							outfile << "bands = " << imager.get_band_count() << "\n";
							outfile << "lines = " << LINE_COUNT << "\n";
							outfile << "framerate = " << imager.get_framerate() << "\n";
							outfile << "shutter = " << imager.get_integration_time() << "\n";
							outfile << "gain = " << imager.get_gain() << "\n";
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
							cubefile.open(filename.c_str(), std::ios::out | std::ios::binary);
							//cubefile.write((const char*) buffer, cubesize * sizeof(unsigned short));
							cubefile.write((const char*) myQueue.pop(), cubesize * sizeof(unsigned short));
							cubefile.close();
							std::cout << "Done." << std::endl;

							// free allocated resources
							if (free_buffer == true)
								delete [] buffer;
						} 
					}
					__finally
					{
						ReleaseMutex(myMutex);						//Release Mutex
						CloseHandle(myThread);

					}
					break;
				case WAIT_ABANDONED:
					//Mutex owned by other
			}
	}
	if (cmd == stp)
	{
		while (!myQueue.empty()) //while there is still data in the queue keep writing cubes
		{
			std::cout << "Recording Complete\nWriting Datacube to Disk" << std::endl;			//write an ENVI compatible header file
			header_filename = filename + ".hdr";
			std::ofstream outfile(header_filename.c_str());
			outfile << "ENVI\n";
			outfile << "interleave = bil\n";
			outfile << "data type = 12\n";
			outfile << "bit depth = 12\n";
			outfile << "samples = " << imager.get_sample_count() << "\n";
			outfile << "bands = " << imager.get_band_count() << "\n";
			outfile << "lines = " << LINE_COUNT << "\n";
			outfile << "framerate = " << imager.get_framerate() << "\n";
			outfile << "shutter = " << imager.get_integration_time() << "\n";
			outfile << "gain = " << imager.get_gain() << "\n";
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
			cubefile.open(filename.c_str(), std::ios::out | std::ios::binary);
			//cubefile.write((const char*) buffer, cubesize * sizeof(unsigned short));
			cubefile.write((const char*) myQueue.pop(), cubesize * sizeof(unsigned short));
			cubefile.close();
			std::cout << "Done." << std::endl;

			// free allocated resources
			if (free_buffer == true)
				delete [] buffer;
		} 
	}
}
