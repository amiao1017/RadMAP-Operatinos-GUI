// SerialReader.h - Class definition for a serial port access class
// Contact: John Kua <jkua@lbl.gov>

#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <iomanip>
#include "Sensor.h"

class SerialReader : public Sensor {
public:
	SerialReader(unsigned int portNumberIn) : portNumber(portNumberIn), hSerial(0), bufferSize(4096), totalBytesCaptured(0), messagesCaptured(0) {
		baseSensorName = "SerialPort";
		sensorName = baseSensorName;
	}
	~SerialReader() {
		close();
	}
	int open() {
		stringstream portName;
		portName << "COM" << portNumber;

		string portString = portName.str();
		serialNumber = portString;
		sensorName = baseSensorName + "_" + portString;
		wstring portNameWide = Util::convertStringToWString(portString);

		hSerial = CreateFile(portNameWide.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (!GetCommState(hSerial, &serialParams)) {
			cerr << "SerialCapture::open() - Failed to get state for " << portName.str() << endl;
			return -1;
		}
	
		// Default port settings
		serialParams.BaudRate = CBR_115200;
		serialParams.Parity = NOPARITY;
		serialParams.ByteSize = 8;
		serialParams.StopBits = ONESTOPBIT;
		serialParams.fOutxCtsFlow = FALSE;
		serialParams.fOutxDsrFlow = FALSE;
		serialParams.fOutX = FALSE;

		if (!SetCommState(hSerial, &serialParams)) {
			cerr << "SerialCapture::open() - Failed to set state for " << portName << endl;
			return -1;
		}

		// Timeouts
		COMMTIMEOUTS timeouts={0};
		timeouts.ReadIntervalTimeout = MAXDWORD;
		timeouts.ReadTotalTimeoutConstant = 0;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;

		if (!SetCommTimeouts(hSerial, &timeouts)) {
			cerr << "SerialCapture:: open() - Failed to set timeouts!" << endl;
			return -2;
		}

		return 0;
	}
	int longFlush() {
		if (!hSerial) return -1;
		// Flush the read buffer. There's some odd behavior at least with the Novatel in that 
		// if you loop a read until seemingly the buffer is clear, more data still comes a bit 
		// later. This code will pause and try again until no data comes.
		char* buffer = new char[bufferSize];	
		cout << sensorName << " flushing the read buffer..." << endl;
		unsigned int counter = 0;
		do {
			//cout << sensorName << " flushing the read buffer " << counter+1 << "..." << endl;
			
			// Inner quick flush
			unsigned int counterInner = 0;
			do {
				ReadFile(hSerial, buffer, bufferSize, &bytesTransferred, 0);
				//cout << "bufferFlush: " << bytesTransferred << endl;
				counterInner++;
			} while (bytesTransferred > 0);			

			// If no data in the inner flush, and it's not the first loop, then we're done.
			if (counter > 1 && counterInner <= 1) break;
		
			counter++;

			Sleep(10);
		} while (counter < 100);
		return 0;
	}
	int read(string& readString) {
		DWORD commModemStatus;
		SetCommMask(hSerial, EV_RXCHAR|EV_ERR); 
		cout << "Waiting for response..." << endl;
		WaitCommEvent(hSerial, &commModemStatus, 0);
		
		char* buffer = new char[bufferSize];	
		if (commModemStatus & EV_RXCHAR) {
			// Wait for the entire message to be received
			Sleep(10);
			ReadFile(hSerial, buffer, bufferSize, &bytesTransferred, 0);
			readString = string(buffer, buffer+bytesTransferred);
			// Trim CRLF
			if (readString.length() > 1) {
				if (readString[readString.length()-1] == '\n') readString.resize(readString.length()-1);
			}
			if (readString.length() > 1) {
				if (readString[readString.length()-1] == '\r') readString.resize(readString.length()-1);
			}
			cout << "Read: " << readString << endl;
			return 0;
		}
		else return 1;
	}
	int write(string command, bool checkForResponse = 1) {
		DWORD bytesWritten;
		cout << "Sending: " << command << endl;
		BOOL retVal = WriteFile(hSerial, command.c_str(), (DWORD)command.length(), &bytesWritten, 0);
		DWORD dwError;
		if (!retVal) {
			switch(dwError = GetLastError()) {
				case ERROR_HANDLE_EOF:
				printf("ERROR_HANDLE_EOF");
				break;
				case ERROR_IO_PENDING:
				printf("ERROR_IO_PENDING");
				break;
			
				default:
				printf("default\n%d",GetLastError());
				break;
			}
		}
		//cout << "bytesWritten: " << bytesWritten << endl;
		if (checkForResponse) {
			DWORD commModemStatus;
			SetCommMask(hSerial, EV_RXCHAR|EV_ERR); 
			cout << "Waiting for response..." << endl;
			WaitCommEvent(hSerial, &commModemStatus, 0);
			
			char* buffer = new char[bufferSize];	
			if (commModemStatus & EV_RXCHAR) {
				// Wait for the entire message to be received
				Sleep(100);
				ReadFile(hSerial, buffer, bufferSize, &bytesTransferred, 0);
				cout << "Reply: " << buffer << endl;
			}
		}
		return 0;
	}
	HANDLE startCapture(void) {
		// Autogenerate a filename from the time and path if one has not been set
		cout << "baseSensorName: " << baseSensorName << endl;
		if ( writeFileName.empty() ) setWriteFileName();
		cout << "writeFileName: " << writeFileName << endl;
	
		// Start thread
		//threadHandle = (HANDLE)_beginthread(threadStaticEntryPoint,0,(void *)this);	// _beginthread automatically closes the thread handle at termination - we need it to stick around so we know the thread closed properly. _beginthreadex leaves it open.
		threadHandle = (HANDLE)_beginthreadex(NULL, 0, threadStaticEntryPoint, (void *)this, 0, &threadID);	// Start thread
	
		return threadHandle;
	}
	int close() {
		if (hSerial) CloseHandle(hSerial);
		return 0;
	}
	string getSensorName() {
	return sensorName;
	}
	unsigned int getTotalBytesCaptured() {
		return totalBytesCaptured;
	}
	unsigned int getMessagesCaptured() {
		return messagesCaptured;
	}
private:
	/// @brief The special form for the _beginthread call. Only for launching the capture thread.
	static unsigned __stdcall threadStaticEntryPoint(void * pThis) {
		SerialReader * pthX = (SerialReader*)pThis;   // the tricky cast
		pthX->serialCaptureThread();           // now call the true entry-point-function

		// A thread terminates automatically if it completes execution,
		// or it can terminate itself with a call to _endthread().
	
		return 0;
	}
	
	/// @brief The actual capture loop where everything is done.
	void serialCaptureThread() {
		totalBytesCaptured = 0;
		messagesCaptured = 0;

		// Setup the output file
		ofstream outputFile(writeFileName + ".txt");
		if (!outputFile.is_open()) {
			cerr << sensorName << ": Failed to open the output file for write! " << writeFileName << endl;
			return;
		}

		SYSTEMTIME winTime;
		GetSystemTime(&winTime);
		outputFile << setfill('0') << "[" << setw(2) << winTime.wHour << ":" << setw(2) << winTime.wMinute << ":" << setw(2) << winTime.wSecond << "." << setw(3) << winTime.wMilliseconds << "] START" << '\n';
		
		char* buffer = new char[bufferSize];
		
		cout << sensorName << " capturing..." << endl;
		
		SetCommMask(hSerial, EV_RXCHAR|EV_ERR);
		unsigned int counter = 0;
		while (1) {
			// Capture until the stop event signals
			DWORD retVal = WaitForSingleObject(stopEvent, 0);
			if (retVal == WAIT_OBJECT_0) {
				cout << sensorName << ": Stopping capture..." << endl;
				outputFile << setfill('0') << "[" << setw(2) << winTime.wHour << ":" << setw(2) << winTime.wMinute << ":" << setw(2) << winTime.wSecond << "." << setw(3) << winTime.wMilliseconds << "] STOP" << '\n';
				outputFile.close();
				break;
			}

			WaitCommEvent(hSerial, &commModemStatus, 0);

			// First thing we do as soon as the event occurs is grab a timestamp;
			GetSystemTime(&winTime);

			// Grabbed a byte
			if (commModemStatus & EV_RXCHAR) {
				// Wait for the entire message to be received
				Sleep(10);
				ReadFile(hSerial, buffer, bufferSize, &bytesTransferred, 0);
				buffer[bytesTransferred] = '\0';
				string readString(buffer, bytesTransferred);
				// Trim CRLF
				if (readString.length() > 1) {
					if (readString[readString.length()-1] == '\n') readString.resize(readString.length()-1);
				}
				if (readString.length() > 1) {
					if (readString[readString.length()-1] == '\r') readString.resize(readString.length()-1);
				}
				if (bytesTransferred != 0) {
					outputFile << setfill('0') << "[" << setw(2) << winTime.wHour << ":" << setw(2) << winTime.wMinute << ":" << setw(2) << winTime.wSecond << "." << setw(3) << winTime.wMilliseconds << "] " << readString << '\n';
					cout << setfill('0') << "[" << setw(2) << winTime.wHour << ":" << setw(2) << winTime.wMinute << ":" << setw(2) << winTime.wSecond << "." << setw(3) << winTime.wMilliseconds << "] " << sensorName << ": Captured " << bytesTransferred << " bytes: " << readString << endl;

					messagesCaptured++;
					totalBytesCaptured += bytesTransferred;
				}
			}
			else if (commModemStatus & EV_ERR) {
				cerr << "SerialCapture::serialCaptureThread() - Some error occured on the serial port!" << endl;
				outputFile.close();
				break;
			}
			//counter++;
			//cout << "loopCount: " << counter << endl;

		}
		cout << sensorName << ": Captured Messages: " << messagesCaptured << " | Captured Bytes: " << totalBytesCaptured << endl;
		cout << sensorName << ": Returning from capture thread..." << endl;
	}

	HANDLE hSerial;
	unsigned int portNumber;
	DWORD bufferSize;
	DCB serialParams;
	DWORD bytesTransferred;
	DWORD commModemStatus;
	unsigned int totalBytesCaptured;
	unsigned int messagesCaptured;
};

#endif