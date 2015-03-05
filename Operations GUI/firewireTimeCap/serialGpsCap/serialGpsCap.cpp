// serialGpsCap - Application for capturing serial GPS data.
// This app focuses on timestamping as close to the time the data is received as possible.

#include <iostream>
#include <string>
#include <conio.h>
#include "SerialReader.h"

using namespace std;

int main (int argc, char* argv[]) {

	int num_args = argc;
	int automatedRun = 0;
	int portNum = 1;
	string configFile;
	unsigned int configNumber = 0;
	string writePath = ".\\";
	if (atoi(argv[num_args]) == 65) {
	automatedRun = 1;
	num_args--;
	}
	if (num_args <= 1) {
		cout << "Usage: " << argv[0] << " <port number> <write path>" << endl;
		cout << "        or" << endl;
		cout << "Usage: " << argv[0] << " <port number> <config file> <device #> <write path>" << endl;
		cout << endl;
		return 1;
	}
	int argIter = 1;
	if (num_args > argIter) {
		portNum = atoi(argv[argIter++]);
		if (portNum < 1) {
			cerr << "Port number must be positive!" << endl;
			return 1;
		}
	}
	/*if (argc > argIter) {
		string temp = argv[argIter++];
			configFile = temp;
			if (argc > argIter) {
				int temp2 = atoi(argv[argIter++]);
				if (temp2 < 0) {
					cout << "Invalid config number! Must be positive!" << endl;
					return 1;
				}
				configNumber = (unsigned int) temp2;
				
			}
		}
	}*/
	if (num_args > argIter) {
		writePath = string(argv[argIter++]);
		cout << "Write Path: " << writePath << endl;
	}
		
	SerialReader sReader(portNum);
	sReader.setWritePath(writePath);
	int retVal = sReader.open();
	if (retVal < 0) {
		cerr << "Failed to open serial port!" << endl;
		getchar();
		return 0;
	}
	sReader.longFlush();
	cout << "Configuring logging" << endl;
	sReader.write("UNLOGALL THISPORT\r\n");
	sReader.write("LOG TIME ONTIME 1.0\r\n");

	// Setup the capture loop
	CaptureManager capture;
	capture.setWatchdog(10000);
	capture.addSensor(&sReader);

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
		if (i % 20 == 0 && sReader.getTotalBytesCaptured()) cout << sReader.getSensorName() << " - Messages captured: " << sReader.getMessagesCaptured() << " | Bytes captured: " << sReader.getTotalBytesCaptured() << endl;
		Sleep(sleepTime);

		i++;
	}

	capture.stopCapture();

	cout << "Turning off logging" << endl;
	sReader.write("UNLOGALL THISPORT\r\n");
	cout << endl;
	cout << "End of program." << endl;

	return 0;
}