/**
@file	ProgramInfo.cpp
@brief	This source file contains the methods used to store program data and parse the configuration file.
@authors John Kua
*/

#include "ProgramInfo.h"

ProgramInfo::ProgramInfo() : configLoaded(0), numScanners(0), numCameras(0), useApplanix(0), 
							  useIntersense(0), numVelodynes(0), numSensors(0), cameraFrameRate(5.0), applanixPort(0), intersensePort(0)
{
}

bool ProgramInfo::getConfigLoaded() const{
	return(configLoaded);
}
unsigned int ProgramInfo::getNumScanners() const{
	return(numScanners);
}
unsigned int ProgramInfo::getNumCameras() const{
	return(numCameras);
}
unsigned int ProgramInfo::getNumVelodynes() const{
	return(numVelodynes);
}
bool ProgramInfo::getUseApplanix() const{
	return(useApplanix);
}
bool ProgramInfo::getUseIntersense() const{
	return(useIntersense);
}
unsigned int ProgramInfo::getScannerPort(unsigned int scannerNum) const{
	return(scannerPorts[scannerNum]);
}
string ProgramInfo::getScannerPortName(unsigned int scannerNum) const{
	return(scannerPortNames[scannerNum]);
}
unsigned int ProgramInfo::getApplanixPort() const{
	return(applanixPort);
}
unsigned int ProgramInfo::getIntersensePort() const{
	return(intersensePort);
}
unsigned int ProgramInfo::getNumSensors() const{
	return(numSensors);
}
#ifdef PROGINFO_USE_LADYBUG
int ProgramInfo::getCameraProperties(unsigned long serialNumber, LadybugReader::CameraProperties& properties) const{
	map<unsigned long, LadybugReader::CameraProperties>::const_iterator it;

	// Find the properties associated with the given serial number
	it = cameraMap.find(serialNumber);
	if (it == cameraMap.end()) return(-1);	// No properties for that serial number
	else properties = (*it).second;

	return(1);
}
float ProgramInfo::getCameraFrameRate() const{
	return(cameraFrameRate);
}
#endif

int ProgramInfo::readConfigFile(string fileName){
	
	// Open the configuration file for read
	ifstream configFile;
	configFile.open(fileName.c_str());
	if (!configFile.is_open()) {
		cout << "Could not open config file: " << fileName << " !" << endl;
		return(-1);
	}

	// Read until we get to the end of the file
	while(1) {
		stringstream curLine;
		string tempLine;
#ifdef PROGINFO_USE_LADYBUG
		LadybugReader::CameraProperties camProps;
#endif

		// Read a single line - stop if reached EOF
		getline(configFile, tempLine);
		if (configFile.eof() && tempLine.size() == 0) break;

		string token;
		unsigned int value;

		// Parse the first token and value
		curLine << tempLine;
		curLine >> token >> value;

		if (token.compare(string("Laser")) == 0) {
			numScanners++;
			if (value == 0) {
				cout << "Invalid Laser COM Port Value!" << endl;
				return(-1);
			}
			scannerPorts.push_back(value);
			stringstream nameStream;
			nameStream << "COM" << value;
			scannerPortNames.push_back(nameStream.str());
		}

		else if (token.compare(string("Camera")) == 0) {
			numCameras++;
#ifdef PROGINFO_USE_LADYBUG
			camProps.serialNumber = (unsigned long) value;
			camProps.configNumber = numCameras - 1;
			// Read the rest of the line
			parseCameraProperties(curLine, camProps);
			cameraMap.insert( pair<unsigned long, LadybugReader::CameraProperties>(camProps.serialNumber, camProps) );
#endif
		}
		else if (token.compare(string("CameraFrameRate")) == 0) {
			if (value < 1 || value > 150){
				cout << "Invalid Camera Frame Rate!" << endl;
				return(-1);
			}
			// Because the first value is defined as an integer
			cameraFrameRate = (float)value / 10;
		}
		else if (token.compare(string("Intersense")) == 0) {
			useIntersense = 1;
			if (value == 0) {
				cout << "Invalid Intersense COM Port Value!" << endl;
				return(-1);
			}
			intersensePort = value;
		}
		else if (token.compare(string("Applanix")) == 0) {
			useApplanix = 1;
			if (value == 0) {
				cout << "Invalid Applanix Trigger COM Port Value!" << endl;
				return(-1);
			}
			applanixPort = value;
		}
		else if (token.compare(string("Velodyne")) == 0) {
			numVelodynes++;
#ifdef PROGINFO_USE_VELODYNE
			velodyneProps.push_back(VelodyneReader::VelodyneProperties());
			velodyneProps.back().serialNumber = (unsigned long) value;
			velodyneProps.back().configNumber = numVelodynes - 1;
#endif
		}

		else if (token.size() == 0 || token[0] == '%');
		else {
			cout << "Invalid Token in config file!" << endl;
			return(-1);
		}

		if (configFile.eof()) break;
	}
	// Scanners + cameras + Intersense
	numSensors = numScanners + numCameras + (unsigned int)useIntersense;

	configLoaded = 1;

	// Close config file
	configFile.close();
	return(1);
}

void ProgramInfo::displayConfig() {
	cout << endl;
	cout << "Sensor Configuration" << endl;
	cout << "--------------------" << endl;
	cout << "# Laser Scanners:    " << numScanners << endl;
	if (numScanners > 0) {
		cout << "Laser Scanner Ports: ";
		for (unsigned int i = 0; i < numScanners; i++) {
			cout << "COM" << scannerPorts[i];
			if ( i < numScanners - 1 ) cout << ", ";
			else cout << endl;
		}
	}
	cout << "# Cameras:           " << numCameras << endl;
	cout << "Use Intersense:      ";
	if (useIntersense){
		cout << "YES" << " | Port: COM" << intersensePort << endl;
	}
	else cout << "NO" << endl;
	cout << "Use Applanix:        ";
	if (useApplanix){
		cout << "YES" << " | Port: COM" << applanixPort << endl;
	}
	else cout << "NO" << endl;
	cout << endl;

#ifdef PROGINFO_USE_LADYBUG
	if (numCameras > 0) cout << "Camera Frame Rate: " << cameraFrameRate << endl;
	map<unsigned long, LadybugReader::CameraProperties>::const_iterator it;
	for (it = cameraMap.begin(); it != cameraMap.end(); ++it) {
		(*it).second.display();
	}
#endif

	return;
}

#ifdef PROGINFO_USE_LADYBUG
int ProgramInfo::parseCameraProperties(stringstream& inputLine, LadybugReader::CameraProperties& camProps) {
	
	camProps.strobes.clear();

	// Process until the end of line is reached
	while (!inputLine.eof() ) {
		string propertyTag;
		int onOff;
		int autoManual;
		int absVal;
		double valueA;
		double valueB;

		LadybugReader::CameraProperties::CameraProperty tempProp;

		// Read the property tag
		inputLine >> propertyTag;
		if (inputLine.eof()) break;

		if(propertyTag[0] == 'P') {
			camProps.position = propertyTag.substr(1);
			continue;
		}
		// Exposure following modes
		/*
		else if (propertyTag[0] == '*') {
			switch (propertyTag[1]) {
				case 'M':
					camProps.exposureMode = LadybugReader::CameraProperties::Master;
					break;
				case 'S':
					unsigned int masterSerialNumber;
					double slaveExposureOffset;
					inputLine >> masterSerialNumber >> slaveExposureOffset;
					camProps.exposureMode = LadybugReader::CameraProperties::Slave;
					camProps.masterSerialNumber = masterSerialNumber;
					camProps.slaveExposureOffset = slaveExposureOffset;
					break;
				default:
					cout << "Invalid Exposure Mode! Should be either *M or *S!\n";
			}
			continue;
		}
		*/
		else if (propertyTag.compare(string("TRG")) == 0) {
			inputLine >> camProps.triggerMode.bOnOff >> camProps.triggerMode.uiSource >> camProps.triggerMode.uiMode >> camProps.triggerMode.uiParameter >> camProps.triggerMode.uiPolarity;
			if (inputLine.eof()) break;
			continue;
		}
		else if (propertyTag.compare(string("STR")) == 0) {
			camProps.strobes.push_back(LadybugStrobeControl());
			inputLine >> camProps.strobes.back().bOnOff >> camProps.strobes.back().uiSource >> camProps.strobes.back().fDelay >> camProps.strobes.back().fDuration >> camProps.strobes.back().uiPolarity;
			continue;
		}
		else if (propertyTag.compare(string("AEC")) == 0) {
			for (int i = 0; i < 6; i++) {
				inputLine >> camProps.aeCamerasUsed[i];
			}
			continue;
		}
		inputLine >> onOff
				  >> autoManual
				  >> absVal
				  >> valueA
				  >> valueB;

		if (inputLine.eof()) break;

		tempProp.onOff = onOff != 0;
		tempProp.autoManual = autoManual != 0;
		tempProp.absVal = absVal != 0;
		tempProp.valueA = valueA;
		tempProp.valueB = valueB;

		if (propertyTag.compare(string("BRI")) == 0) {
			camProps.brightness = tempProp;
		}
		else if (propertyTag.compare(string("EXP")) == 0) {
			camProps.exposure = tempProp;
		}
		/*else if (propertyTag.compare(string("SRP")) == 0) {
			camProps.sharpness = tempProp;
		}*/
		else if (propertyTag.compare(string("WHI")) == 0) {
			camProps.whiteBalance = tempProp;
		}
		/*else if (propertyTag.compare(string("HUE")) == 0) {
			camProps.hue = tempProp;
		}
		else if (propertyTag.compare(string("SAT")) == 0) {
			camProps.saturation = tempProp;
		}*/
		else if (propertyTag.compare(string("GAM")) == 0) {
			camProps.gamma = tempProp;
		}
		else if (propertyTag.compare(string("SHT")) == 0) {
			camProps.shutter = tempProp;
		}
		else if (propertyTag.compare(string("GAI")) == 0) {
			camProps.gain = tempProp;
		}
		else if (propertyTag.compare(string("AER")) == 0) {
			camProps.autoExposureRange = tempProp;
		}
		else if (propertyTag.compare(string("ASR")) == 0) {
			camProps.autoShutterRange = tempProp;
		}
		else if (propertyTag.compare(string("AGR")) == 0) {
			camProps.autoGainRange = tempProp;
		}
		else if (propertyTag.compare(string("FRA")) == 0) {
			camProps.frameRate = tempProp;
		}
		else {
			cout << "Invalid property tag! " << propertyTag << endl;
			return(-1);
		}

		// Set initial current settings
		/*camProps.currentShutter = camProps.shutter;
		camProps.currentGain = camProps.gain;
		camProps.currentWhiteBalance = camProps.whiteBalance;
		*/
	}


	return(1);
}
#endif