/**
\file	ladybugCap.cpp
\brief	This is the entry point for the ladybug capture program
\authors John Kua
*/

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <Windows.h>

#include "LadybugReader.h"
//#define PROGINFO_USE_LADYBUG	// MUST DEFINE THIS IN PREPROCESSOR DEFINITIONS!
#include "ProgramInfo.h"

int main (int argc, char* argv[]){

	ProgramInfo progInfo;
	int num_args = argc;
	int automatedRun = 0;
	int cameraNumber = -1;
	unsigned int configNumber = 0;
	int retVal;
	string writePath;
	string configFile;
	int captureTime = -1;
	int argIter = 1;
	if (atoi(argv[num_args]) == 65) {
		automatedRun = 1;
		num_args--;
	}
	if (num_args < 2) {
		cout << "Usage: " << argv[0] << " <camera #> <write path> <collectTime>" << endl;
		cout << "        or" << endl;
		cout << "Usage: " << argv[0] << " <config file> <camera #> <write path> <collectTime>" << endl;
		LadybugReader::displayCameraInfoAll();
		return 1;
	}
	else {
		cameraNumber = atoi(argv[argIter]);
		if (cameraNumber < 0) {
			cerr << "Invalid camera number! Must be positive!" << endl;
			return 1;
		}
		if (cameraNumber == 0 && strlen(argv[argIter]) > 1) {
			configFile = string(argv[argIter]);
			argIter++;
			if (num_args > argIter) {
				int temp = atoi(argv[argIter]);
				if (temp == 0 && strlen(argv[argIter]) > 1) argIter--;
				else {
					if (temp < 0) {
						cout << "Invalid config number! Must be positive!" << endl;
						return 1;
					}
					configNumber = (unsigned int) temp;
				}
			}
		}
		argIter++;
	}
	if (num_args <= argIter) writePath = ".\\";
	else writePath = string(argv[argIter++]);
	if (num_args > argIter) captureTime = atoi(argv[argIter++]);

	if (captureTime < 0) cout << "Capture time is unlimited." << endl;
	else cout << "Capture time is " << captureTime << " seconds" << endl;
	cout << endl;

	CaptureManager capture;
	capture.setWatchdog(10000);

	LadybugReader::CameraProperties camProp;
	if (!configFile.empty()) {
		progInfo.readConfigFile(configFile);
		//progInfo.displayConfig();
		if (progInfo.getNumCameras() < 1) {
			cerr << "No cameras defined in config file! Aborting!" << endl;
			return 1;
		}
		if (configNumber >= progInfo.getNumCameras()) {
			cerr << "Only " << progInfo.getNumCameras() << " in config file! Config # " << configNumber << " is out of range!" << endl;
			return 1;
		}
		unsigned int camNum;
		map<unsigned long, LadybugReader::CameraProperties>::const_iterator it;
		for (it = progInfo.cameraMap.begin(); it != progInfo.cameraMap.end(); ++it) {
			camProp = (*it).second;
			if (camProp.configNumber == configNumber) break;
		}
		retVal = LadybugReader::findCameraBySerialNumber(camProp.serialNumber, camNum);
		if (retVal < 0) {
			return 1;
		}
		else cameraNumber = camNum;
	}
	
	// Setup cameras
	//LadybugReader::displayCameraInfoAll();
	vector<LadybugReader> lReader(1, LadybugReader());
	capture.addSensor(&lReader[0]);
	//capture.addSensor(&lReader[1]);

	lReader[0].settings = camProp;

	lReader[0].settings.display();

	retVal = lReader[0].initializeCamera(cameraNumber, 100);
	if (retVal < 0 ) return 2;
	lReader[0].setWritePath(writePath);
	/*cout << "Pausing 5 seconds before next init...";
	for (int i = 0; i < 5; i++) {
		cout << i << "..."; 
		Sleep(1000);
	}
	cout << endl;
	lReader[1].initializeCamera(1);
	lReader[1].setWriteFileName("E:\\temp\\camera1.pgr");
	*/

	// Start capture
	capture.startCapture();
	
	// Wait...
	int sleepTime = 500;
	int i = 0;
	while (1) {
		double timeElapsedMs = i * (double)sleepTime;
		if (captureTime > 0 && i >= captureTime*1000/sleepTime) break;
		if (captureTime > 0 && i % 20 == 0) {
			cout << endl << "*** Capture time remaining: " << ((double)captureTime - (double)timeElapsedMs/1000) << " sec ***" << endl << endl;
		}
		if (_kbhit()) {
			char key = _getch();
			if (key == 'q' || key == 'Q') {
				if (automatedRun == 1){
					break;
				}
				cout << "Are you sure you want to stop acquisition? (y/n): " << endl;
				key = _getch();
				if (key == 'y' || key == 'Y') {
					break;
				}
			}
		}

		cout.precision(4);
		if (lReader[0].getNumImagesWritten()) cout << lReader[0].getSensorName() << " - Images: " << lReader[0].getNumImagesWritten() << " | MB: " << fixed << lReader[0].getMegabytesWritten() << " | Rate: " << lReader[0].getFrameRate() << endl;
		//cout << lReader[1].getSensorName() << " - Images: " << lReader[1].getNumImagesWritten() << " | MB: " << lReader[1].getMegabytesWritten() << " | Rate: " << lReader[1].getFrameRate() << endl;
		Sleep(sleepTime);

		i++;
	}

	capture.stopCapture();

	cout << "End of program." << endl;

	return 0;
}