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

#include "resonon_imager_pgr.h"
#include "resonon_imager_base.h"


const int LINE_COUNT = 1000; //size of datacube to record
const std::string filename = "2iHyperSpecDataCube"; //location to save datacube
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

Resonon::Pika2I imager;

int main(int argc, char* argv[])
{
	
	return 0;
}

void makeCube(std::pair<unsigned short *,int> myData)
{
	calls++;
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	char fileDay [100];
	strftime(fileDay,100,"%Y%m%d",timeinfo);
	char fileTime [100];
	strftime(fileTime,100,"%H:%M:%S",timeinfo);
	char directoryTime [100];
	strftime(directoryTime,100,"%H%M%S",timeinfo);
	std::string saveName;
	std::string cubeSaveName;
	std::string stringCalls;
	saveName = filename;
	stringCalls = std::to_string(calls);
	saveName.append(stringCalls);
	saveName.append("-");
	saveName.append(fileDay);
	std::cout << "\nmakeCube successfully called\n" << std::endl;
	std::cout << "Recording Complete\nWriting Datacube to Disk" << std::endl;			//write an ENVI compatible header file
	if (calls <= 1)
	{
		directoryName = ".\\";
		directoryName.append(fileDay);
		directoryName.append(directoryTime);
		//directoryName.append("/");
	}
	CreateDirectoryA(directoryName.c_str(),NULL);
	header_filename = directoryName;
	header_filename.append("\\");
	header_filename.append(saveName);
	header_filename.append(".hdr");
	std::ofstream outfile(header_filename.c_str());
	outfile << "ENVI\n";
	outfile << "File created at " << fileTime << " On " << fileDay << "\n";
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
	cubeSaveName = directoryName + "\\" + saveName + ".bil";		
	cubefile.open(cubeSaveName.c_str(), std::ios::out | std::ios::binary);
	//cubefile.write((const char*) buffer, cubesize * sizeof(unsigned short));
	cubefile.write((const char*) myData.first, framesize * myData.second * sizeof(unsigned short));
	cubefile.close();
	std::cout << "Done." << std::endl;

	// free allocated resources
	delete [] myData.first;		
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
		std::cout << "\nWrite thread owns Mutex" << std::endl;			
		if (!myQueue.empty()) //while there is still data in the queue keep writing cubes
		{
			std::cout << "\nQueue not empty" << std::endl;
			std::pair<unsigned short *, int> myData = myQueue.front();
			std::cout << "\nGot pair from queue" << std::endl; 
			myQueue.pop();
			std::cout << "\nPop data from queue" << std::endl;
			ReleaseMutex(myMutex);
			std::cout << "\nRelased Mutex" << std::endl;
			makeCube(myData);
			std::cout << "\nmakeCube called" << std::endl;
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
