/**
\file	Sensor.h
\brief	This class is used to capture images from a Point Grey Ladybug camera
\authors John Kua
*/

#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <assert.h>
#include <stdint.h>

#include <process.h>
#include <Windows.h>

using namespace std;

class Util {
public:
	static string stripExtension(const string& fileName) {
		size_t dotPos = fileName.rfind('.');
		if (dotPos != string::npos) {
			return fileName.substr(0, dotPos);
		}
		return "";
	}
	
	static bool checkExtension(const string& fileName, string desiredExtension) {
		size_t dotPos = fileName.rfind('.');
		if (dotPos != string::npos) {
			string ext = fileName.substr(dotPos);
			transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			if (ext.compare(desiredExtension) == 0) {
				return 1;
			}
		}
		else if (desiredExtension.empty()) return 1;
		return 0;
	}
	static wstring convertStringToWString(const string& s) {
		// Original string length
		int slength = (int) s.length()+1;

		// Wide string length
		int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		
		// Allocate
		wchar_t* buf = new wchar_t[len];
		
		// Convert
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		wstring r(buf);

		delete[] buf;

		return r;
	}
};

class Sensor {
public:
	Sensor() {
		stopEvent = NULL;
		threadHandle = NULL;
		threadID = 0;
		sensorName = "Sensor";
		serialNumber = "";
		writePath = "";
		writeFileName = "";
		timestampFileName = "";
		writeFileExtension = "";
		timestampFileExtension = "";
	}
	virtual HANDLE startCapture ( void ) = 0;
	HANDLE getThreadHandle() {
		return threadHandle;
	}
	unsigned int getThreadID() {
		return threadID;
	}
	int setStopEvent(HANDLE stopEventIn) {
		stopEvent = stopEventIn;
		return 0;
	}
	static int round(double number) {
		 return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
	}
	int setWriteFileName() {
		string name = writePath + generateFileNameFromTime();
		setWriteFileName(name);
		return 0;
	}
	int setWriteFileName(const string name) {
		writeFileName = name;
		timestampFileName = Util::stripExtension(writeFileName) + "_timestamps" + timestampFileExtension;
		return 0;
	}
	int setTimestampFileName(const string name) {
		timestampFileName = name;
		return 0;
	}
	int setWritePath(const string path) {
		stringstream name;
		name << path;
		if (path.back() != '\\') name << "\\";
		writePath = name.str();
		return 0;
	}
	string generateFileNameFromTime() {
		SYSTEMTIME winTime;
		GetSystemTime(&winTime);
	
		stringstream ss;
		ss << setfill('0');
		ss << setw(4) << winTime.wYear << setw(2) << winTime.wMonth << setw(2) << winTime.wDay;
		string datestr = ss.str();
	
		ss.str("");
		ss << setw(2) << winTime.wHour << setw(2) << winTime.wMinute << setw(2) << winTime.wSecond;
		string timestr = ss.str();
	
		stringstream name;
		name << baseSensorName + "_" << serialNumber << "_" + datestr + "_" + timestr + writeFileExtension;

		return name.str();
	}
protected:
	HANDLE stopEvent;
	HANDLE threadHandle;
	unsigned threadID;
	string baseSensorName;
	string sensorName;
	string writePath;
	string writeFileName;
	string writeFileExtension;
	string timestampFileName;
	string timestampFileExtension;
	string serialNumber;
};

class CaptureManager {
public:
	CaptureManager() {
		watchdogTime = 10000;
		stopEvent = CreateEvent(NULL, TRUE, FALSE, LPCWSTR("stopEvent"));
		if (stopEvent == NULL) {
			cerr << "CaptureManager() - Unable to create stopCaptureEvent!" << endl;
			exit(1);
		}
	}
	~CaptureManager() {
		CloseHandle(stopEvent);
		for (size_t i = 0; i < sensorThreads.size(); i++) {
			if (!sensorThreads[i]) CloseHandle(sensorThreads[i]);
		}
	}
	int addThread(HANDLE thread) {
		if (!thread) {
			cerr << "CaptureManager::addThread() - Invalid thread handle!" << endl;
			return -1;
		}
		sensorThreads.push_back(thread);
		return 0;
	}
	int addSensor(Sensor* sensor) {
		if (!sensor) {
			cerr << "CaptureManager::addSensor() - This is not a valid sensor pointer!" << endl;
			return -1;
		}
		sensor->setStopEvent(stopEvent);
		sensors.push_back(sensor);
		return 0;
	}
	int addSensors(vector<Sensor*>& sensorVec) {
		for (size_t i = 0; i < sensorVec.size(); i++) {
			addSensor(sensorVec[i]);
		}
	}
	int startCapture() {
		if (!sensors.size()) {
			cerr << "CaptureManager::startCapture() - No sensors to start!" << endl;
			return -1;
		}
		for (size_t i = 0; i < sensors.size(); i++) {
			cout << "Starting sensor " << i << "..." << endl;
			HANDLE tHandle = sensors[i]->startCapture();
			if (!tHandle) {
				cerr << "CaptureManager::startCapture() - sensor " << i << " failed to start!" << endl;
			}
			else addThread(tHandle);
		}
		return 0;
	}
	int stopCapture() {
		// Signal end of acquisition
		cout << "Signaling the capture threads to stop..." << endl;;
		SetEvent(stopEvent);
	
		// Wait for threads to terminate
		//cout << "Waiting for capture threads to stop..." << endl;
		int sleepTime = 250;
		int watchdog = watchdogTime/sleepTime;	// Enforce a max wait time
		while (watchdog >= 0) {
			DWORD retVal = WaitForMultipleObjects ((DWORD)sensorThreads.size(), &sensorThreads[0], TRUE, 0);
			//cout << "retVal: " << retVal << endl;
			if (retVal == WAIT_OBJECT_0 || retVal == WAIT_FAILED) {
				cout << "All threads have stopped." << endl;
				break;
			}		
			Sleep(sleepTime);
			watchdog--;
		}
		if (watchdog <= 0) {
			cout << "Threads took too long to quit! Continuing..." << endl;
			for (size_t i = 0; i < sensorThreads.size(); i++) {
				DWORD retVal = WaitForSingleObject (sensorThreads[i], 0);
				cout << "Thread " << i << " retVal: " << retVal << endl;
			}
		}

		return 0;
	}
	int setWatchdog(int time) {
		watchdogTime = time;
		return 0;
	}
	HANDLE stopEvent;
	vector<HANDLE> sensorThreads;
	vector<Sensor*> sensors;
	int watchdogTime;
};
#endif
