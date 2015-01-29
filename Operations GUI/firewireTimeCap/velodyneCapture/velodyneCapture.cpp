/**
\file	velodyneCapture.cpp
\brief	This is the entry point for the Velodyne capture program
\authors John Kua
*/

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>

//#define PROGINFO_USE_VELODYNE	// MUST DEFINE THIS IN PREPROCESSOR DEFINITIONS!
#include "VelodyneReader.h"
#include "ProgramInfo.h"

#include <Windows.h>

int main (int argc, char* argv[]){

	int num_args = argc;
	int automatedRun = 0;
	int interfaceNum = 1;
	VelodyneReader::IpAddress sensorAddress(0,0,0,0);
	string serialNumber, ipAddressString;
	string configFile;
	unsigned int configNumber = 0;
	string writePath = ".\\";
	string filterString = "DEFAULT";
	bool overrideFilter = 0;
	if (atoi(argv[num_args]) == 65) {
		automatedRun = 1;
		num_args--;
	}
	if (num_args <= 1) {
		cout << "Usage: " << argv[0] << " <interface number> <sensor s/n / ip address> <write path> <filter string>" << endl;
		cout << "        or" << endl;
		cout << "Usage: " << argv[0] << " <interface number> <config file> <lidar #> <write path> <filter string>" << endl;
		cout << endl;
		cout << "Displaying interface list..." << endl;
		VelodyneReader::displayInterfaceList();
		return 1;
	}
	VelodyneReader::getInterfaceList();
	int argIter = 1;
	if (num_args > argIter) {
		interfaceNum = atoi(argv[argIter++]);
		if (interfaceNum <= 0 || interfaceNum > VelodyneReader::getNumInterfaces()) {
			cerr << "Invalid interface number! Must be from 1-" << VelodyneReader::getNumInterfaces() << endl;
			return 1;
		}
	}
	if (num_args > argIter) {
		string temp = argv[argIter++];
		if (temp[0] >= '0' && temp[0] <= '9') {
			if (temp.find('.') != string::npos) {
				ipAddressString = temp;
				sensorAddress = VelodyneReader::IpAddress(ipAddressString);
				serialNumber = VelodyneReader::convertIpAddressToSerialNumberString(sensorAddress);
			}
			else {
				serialNumber = temp;
				sensorAddress = VelodyneReader::convertSerialNumberToIpAddress(serialNumber);
			}
		}
		else {
			configFile = temp;
			if (num_args > argIter) {
				int temp2 = atoi(argv[argIter++]);
				if (temp2 < 0) {
					cout << "Invalid config number! Must be positive!" << endl;
					return 1;
				}
				configNumber = (unsigned int) temp2;
				
			}
		}
	}
	if (num_args > argIter) {
		writePath = string(argv[argIter++]);
		cout << "Write Path: " << writePath << endl;
	}
	if (num_args > argIter) {
		filterString = string(argv[argIter++]);
		overrideFilter = 1;
		cout << "Filter String: " << filterString << endl;
	}

	VelodyneReader vReader;
	ProgramInfo progInfo;
	if (!configFile.empty()) {
		progInfo.readConfigFile(configFile);
		//progInfo.displayConfig();
		if (progInfo.getNumVelodynes() < 1) {
			cerr << "No Velodynes defined in config file! Aborting!" << endl;
			return 1;
		}
		if (configNumber >= progInfo.getNumVelodynes()) {
			cerr << "Only " << progInfo.getNumVelodynes() << " in config file! Config # " << configNumber << " is out of range!" << endl;
			return 1;
		}
		vReader.settings = progInfo.velodyneProps[configNumber];
		stringstream ss;
		ss << vReader.settings.serialNumber;
		serialNumber = ss.str();
		sensorAddress = VelodyneReader::convertSerialNumberToIpAddress(serialNumber);
	}

	cout << endl;
	cout << "Reading from sensor " << serialNumber << " at address " << sensorAddress.str() << " on interface " << interfaceNum << "..." << endl;
	cout << endl;

	int retVal = vReader.initialize(interfaceNum, sensorAddress, filterString);
	if (retVal < 0) {
		cerr << "Failed to initialize lidar reader!" << endl;
		return 2;
	}
	vReader.setWritePath(writePath);

	CaptureManager capture;
	capture.setWatchdog(10000);
	capture.addSensor(&vReader);

	// Start capture
	cout << endl;
	capture.startCapture();
	
	// Wait...
	int captureTime = -1;
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
		if (vReader.getPacketsCaptured()) cout << vReader.getSensorName() << " - Packets captured: " << vReader.getPacketsCaptured() << endl;
		Sleep(sleepTime);

		i++;
	}

	capture.stopCapture();
	cout << endl;
	cout << "End of program." << endl;
	return 0;
}