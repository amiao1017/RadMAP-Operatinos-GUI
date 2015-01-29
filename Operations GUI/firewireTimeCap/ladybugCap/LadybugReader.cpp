/**
\file	LadybugReader.cpp
\brief	Defines methods for Point Grey Ladybug camera access/capture
\authors John Kua
*/

#include "LadybugReader.h"

LadybugReader::LadybugStatic LadybugReader::staticData;

LadybugReader::LadybugReader(): cameraInitialized(0), streamContext(NULL), frameCounter(0), frameRate(0), totalMBWritten(0), totalNumberOfImagesWritten(0), droppedFrameCounter(0) {
	ladybugDataFormat = LADYBUG_DATAFORMAT_COLOR_SEP_SEQUENTIAL_JPEG;
	baseSensorName = "Ladybug";
	sensorName = baseSensorName;
	writeFileExtension = ".pgr";
	timestampFileExtension = ".txt";

	// Initialize context
	error = ::ladybugCreateContext( &context );
	_HANDLE_ERROR;
}
LadybugReader::~LadybugReader() {
	if ( streamContext != NULL ) {
		error = ladybugDestroyStreamContext ( &streamContext );
		_HANDLE_ERROR;
	}
	// Destroy context - for some reason, doing this causes the program to crash and allocate all the memory
	//error = ::ladybugDestroyContext( &context );
	//_HANDLE_ERROR;
}

int LadybugReader::enumerateCameras() {
	unsigned int numCams = staticData.estimatedNumCameras;

	// Enumerate cameras on the bus
	LadybugError error = ::ladybugBusEnumerateCameras(staticData.context, staticData.camInfoAll, &numCams);
	_HANDLE_ERROR;
	staticData.numCamerasOnBus = numCams;

	// If the number of cameras on the bus is larger than estimated, reallocate the array any try again.
	if (numCams > staticData.estimatedNumCameras) {
		delete[] staticData.camInfoAll;
		staticData.camInfoAll = new LadybugCameraInfo[numCams];
		error = ::ladybugBusEnumerateCameras(staticData.context, staticData.camInfoAll, &numCams);
		_HANDLE_ERROR;
	}

	// If this is not true, this would indicate that the number of cameras changed between the first and second enumeration
	assert(numCams == staticData.numCamerasOnBus);

	staticData.camerasEnumerated = 1;
	return staticData.numCamerasOnBus;
}

int LadybugReader::displayCameraInfo ( const LadybugCameraInfo& cameraInfo ) {
	cout << "\tSerial number - Head: " << cameraInfo.serialHead << " | Base: " << cameraInfo.serialBase << endl;
	cout << "\tColor Camera: " << cameraInfo.bIsColourCamera << endl;
	cout << "\tLadybug Device Type: " ;
	switch (cameraInfo.deviceType) {
		case LADYBUG_DEVICE_LADYBUG:
			cout << "Ladybug base unit (Original Ladybug unit)" << endl;
			break;
		case LADYBUG_DEVICE_COMPRESSOR:
			cout << "Ladybug JPEG Compressor (Ladybug2)" << endl;
			break;
		case LADYBUG_DEVICE_LADYBUG3:
			cout << "Ladybug3" << endl;
			break;
		case LADYBUG_DEVICE_LADYBUG5:
			cout << "Ladybug5" << endl;
			break;
		case LADYBUG_DEVICE_UNKNOWN:
			cout << "Not a Ladybug camera!" << endl;
			break;
		default:
			cout << "Unrecognized Ladybug type! (" << cameraInfo.deviceType << ')' << endl;
			break;
	}
	cout << "\tModel name: " << cameraInfo.pszModelName << endl;
	cout << "\tSensor Info: " << cameraInfo.pszSensorInfo << endl;
	cout << "\tVendor name: " << cameraInfo.pszVendorName << endl;
	cout << "\tCompliant with IEEE 1394 version: " << cameraInfo.iDCAMVer << endl;
	cout << "\tBus number: " << cameraInfo.iBusNum << endl;
	cout << "\tNode number: " << cameraInfo.iNodeNum << endl;
	cout << "\tMax bus speed: " << cameraInfo.maxBusSpeed << endl;
	cout << endl;
	return 0;
}
int LadybugReader::displayCameraInfo () {
	if (!cameraInitialized) {
		cerr << "No camera initialized for this context!" << endl;
		return -1;
	}
	return displayCameraInfo(camInfo);
}
int LadybugReader::displayCameraInfoAll ( const unsigned int cameraNum ) {
	if (!staticData.camerasEnumerated) enumerateCameras();
	if (cameraNum >= staticData.numCamerasOnBus) {
		cerr << "Camera index " << cameraNum << " invalid! Valid range is 0-" << staticData.numCamerasOnBus << endl;
		return -1;
	}
	if (staticData.camInfoAll[cameraNum].deviceType == LADYBUG_DEVICE_UNKNOWN) {
		cout << "Camera at index " << cameraNum << " is not a Ladybug camera!" << endl;
		return 1;
	}
	cout << "Camera at index: " << cameraNum << endl;
	cout << "------------------------------" << endl;
	return displayCameraInfo(staticData.camInfoAll[cameraNum]);
}
int LadybugReader::displayCameraInfoAll ( void ) {
	if (!staticData.camerasEnumerated) enumerateCameras();
	for (unsigned int i = 0; i < staticData.numCamerasOnBus; i++) {
		displayCameraInfoAll(i);
	}

	return 0;
}

int LadybugReader::findCameraBySerialNumber( const LadybugSerialNumber serialNumber, unsigned int& cameraNum ) {
	if (!staticData.camerasEnumerated) enumerateCameras();
	for (unsigned int i = 0; i < staticData.numCamerasOnBus; i++) {
		if (staticData.camInfoAll[i].serialHead == serialNumber) {
			cameraNum = i;
			return 0;
		}
	}
	cerr << "Could not find camera with serial number " << serialNumber << endl;
	return -1;
}

int LadybugReader::initializeCamera ( const unsigned int cameraNum, const unsigned int numBuffers ) {
	if (!staticData.camerasEnumerated) enumerateCameras();
	if (cameraNum >= staticData.numCamerasOnBus) {
		cerr << "Camera index " << cameraNum << " invalid! Valid range is 0-" << staticData.numCamerasOnBus-1 << endl;
		return -1;
	}
	if (staticData.camInfoAll[cameraNum].deviceType == LADYBUG_DEVICE_UNKNOWN) {
		cerr << "Camera at index " << cameraNum << " is not a Ladybug camera!" << endl;
		return -1;
	}
	//cout << "initCam - cameraNumber: " << cameraNum << " | busNumber: " << staticData.camInfoAll[cameraNum].iBusNum << endl;
	//return initializeCameraByBusIndex( staticData.camInfoAll[cameraNum].iBusNum, numBuffers );
	return initializeCameraByBusIndex( cameraNum, numBuffers );
}
int LadybugReader::initializeCameraBySerialNumber( const LadybugSerialNumber serialNumber, const unsigned int numBuffers) {
	if (!staticData.camerasEnumerated) enumerateCameras();
	unsigned int cameraNum = 0;
	int retVal = findCameraBySerialNumber(serialNumber, cameraNum);
	if (retVal != 0) return -1;
	//return initializeCameraByBusIndex( staticData.camInfoAll[cameraNum].iBusNum, numBuffers );
	return initializeCameraByBusIndex( cameraNum, numBuffers );
}
int LadybugReader::initializeCameraByBusIndex ( const unsigned int busIndex, const unsigned int numBuffers ) {
	error = ladybugInitializePlus( context, busIndex, numBuffers, NULL, 0);
	_HANDLE_ERROR;
	error = ladybugGetCameraInfo( context, &camInfo );
	_HANDLE_ERROR;
	settings.busNumber = camInfo.iBusNum;
	settings.serialNumber = camInfo.serialHead;
	stringstream ss;
	ss << camInfo.serialHead;
	serialNumber = ss.str();

	ss.str("");
	ss << baseSensorName << " " << camInfo.serialHead;
	sensorName = ss.str();
	cout << "Initializing " << sensorName << "..." << endl;

	// Set the timeout value for grab function to zero.
	// Do not wait if there is no image waiting. 
	ladybugSetGrabTimeout( context, 0 );

	// Reset the camera to default factory settings by asserting bit 0
	error = ladybugSetRegister( context, PGR_INITIALIZE, 0x80000000 );
	_HANDLE_ERROR;

	setProperty( context, LADYBUG_FRAME_RATE, settings.frameRate );
	setProperty( context, LADYBUG_BRIGHTNESS, settings.brightness );
	setProperty( context, LADYBUG_AUTO_EXPOSURE, settings.exposure );
	setProperty( context, LADYBUG_WHITE_BALANCE, settings.whiteBalance );
	setProperty( context, LADYBUG_GAMMA, settings.gamma );
	setProperty( context, LADYBUG_SHUTTER, settings.shutter );
	setProperty( context, LADYBUG_GAIN, settings.gain );

	if (settings.autoExposureRange.onOff) {
		setRangeRegister( context, PGR_AUTO_EXPOSURE_RANGE, (unsigned int)settings.autoExposureRange.valueA, (unsigned int)settings.autoExposureRange.valueB);
	}
	else{
		setRangeRegister( context, PGR_AUTO_EXPOSURE_RANGE, 0, 0xFFF);
	}
	if (settings.autoShutterRange.onOff) {
		setRangeRegister( context, PGR_AUTO_SHUTTER_RANGE, (unsigned int)settings.autoShutterRange.valueA, (unsigned int)settings.autoShutterRange.valueB);
	}
	else{
		setRangeRegister( context, PGR_AUTO_SHUTTER_RANGE, 0, 0xFFF);
	}
	if (settings.autoGainRange.onOff) {
		setRangeRegister( context, PGR_AUTO_GAIN_RANGE, (unsigned int)settings.autoGainRange.valueA, (unsigned int)settings.autoGainRange.valueB);
	}
	else{
		setRangeRegister( context, PGR_AUTO_GAIN_RANGE, 0, 0xFFF);
	}

	// Set strobe outputs
	for (size_t i = 0; i < settings.strobes.size(); i++) {
		unsigned long registerVal;
		ladybugGetRegister(context, PGR_PIO_DIRECTION, &registerVal);
		registerVal |= (1 << settings.strobes[i].uiSource);
		ladybugSetStrobe(context, &settings.strobes[i]);
	}

	// Set AE Mask
	unsigned long aeMask;
	ladybugGetRegister( context, PGR_AE_STATS_MASK, &aeMask);
	aeMask &= 0xFFFFFFC0;
	for (int i = 0; i < 6; i++) {
		if (settings.aeCamerasUsed[i]) aeMask |= ((unsigned long)0x20 >> i);
	}
	ladybugSetRegister( context, PGR_AE_STATS_MASK, aeMask);

	// Set JPEG settings
	ladybugSetAutoJPEGBufferUsage( context, settings.jpegAutoBufferUsage);
	ladybugSetJPEGQuality( context, settings.jpegCompressionQuality);
	ladybugSetAutoJPEGQualityControlFlag( context, settings.jpegAutoQualityControl);

	// Enable All Embedded Frame Info, including timestamp, gain, shutter, brightness, exposure, white balance
	error = ladybugSetRegister( context, PGR_FRAME_INFO, 0x800001FF );

	// Start Ladybug with the specified data format
	//error = ::ladybugStartLockNext( context, LADYBUG_RESOLUTION_ANY, ladybugDataFormat);
	error = ::ladybugStartLockNextEx( context, LADYBUG_RESOLUTION_ANY, ladybugDataFormat, 8976, 0);
	if ( error != LADYBUG_OK ) {
		printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) ); 
		error = ladybugStop( context );
		if ( error != LADYBUG_OK ) printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) );
		return -1;
	}

	// Not sure what this is for - in ladybugSampleRecording.cpp, line 818 
	// Seems to go through a few iterations - maybe checking for operation.
	int iTryTimes = 0;
	do {
		//cout << "Trying unlock all - attempt " << iTryTimes + 1 << endl;
		ladybugUnlockAll( context );
		Sleep( 100 );
		error = ladybugLockNext( context, &image_Prev );
	} while ( ( error != LADYBUG_OK )  && ( iTryTimes++ < 10) );    
	if ( error != LADYBUG_OK ) {
		printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) ); 
		ladybugUnlockAll( context );
		error = ladybugStop( context );
		if ( error != LADYBUG_OK ) printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) );
		return -1;
	}

	// Set the trigger mode
	ladybugSetTriggerMode(context, &settings.triggerMode);

	// Set frame rate - this needs to be after the ladybugStartLockNext call for some reason.
	setProperty( context, LADYBUG_FRAME_RATE, settings.frameRate );

	return 0;
}

double LadybugReader::getFrameRate() {
	return frameRate;
}
double LadybugReader::getMegabytesWritten() {
	return totalMBWritten;
}
unsigned long LadybugReader::getNumImagesWritten() {
	return totalNumberOfImagesWritten;
}
string LadybugReader::getSensorName() {
	return sensorName;
}

HANDLE LadybugReader::startCapture ( void ) {

	// Autogenerate a filename from the time and path if one has not been set
	if ( writeFileName.empty() ) setWriteFileName();

	// Start thread
	//threadHandle = (HANDLE)_beginthread(threadStaticEntryPoint,0,(void *)this);	// _beginthread automatically closes the thread handle at termination - we need it to stick around so we know the thread closed properly. _beginthreadex leaves it open.
	threadHandle = (HANDLE)_beginthreadex(NULL, 0, threadStaticEntryPoint, (void *)this, 0, &threadID);	// Start thread

	return threadHandle;
}

unsigned __stdcall LadybugReader::threadStaticEntryPoint(void * pThis){

	LadybugReader * pthX = (LadybugReader*)pThis;   // the tricky cast
	pthX->imageCaptureThread();           // now call the true entry-point-function

	// A thread terminates automatically if it completes execution,
	// or it can terminate itself with a call to _endthread().

	return 0;
}
LadybugError LadybugReader::stopCamera(){
	ladybugUnlockAll( context );
	error = ladybugStop( context );
	if ( error != LADYBUG_OK ) printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) );
	error = ladybugStopStream( streamContext );
	if ( error != LADYBUG_OK ) printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) );
	return error;
}

double LadybugReader::imageTimeStampToSeconds(unsigned int uiRawTimestamp)
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



/******************************************************************************
* This method is the image capture thread that runs in parallel with all other* 
* processes                                                                   *
******************************************************************************/

void LadybugReader::imageCaptureThread()
{	
	// Setup the output stream
	if ( streamContext == NULL ) {
		error = ladybugCreateStreamContext( &streamContext );
		if ( error != LADYBUG_OK ) {
			cout << sensorName << ": Could not create the stream context!" << endl;
			stopCamera();
			return;
		}
	} 

	// Setup the timestamp file
	ofstream timestampFile(timestampFileName);
	if (!timestampFile.is_open()) {
		cerr << sensorName << ": Failed to open the timestamp file for write! " << timestampFileName << endl;
		stopCamera();
		return;
	}
        
	// Used to track image/frame stats
	totalMBWritten = 0;
	totalNumberOfImagesWritten = 0;
         
	error = ladybugInitializeStreamForWriting( streamContext, writeFileName.c_str(), context, streamNameOpened, true );
	if ( error == LADYBUG_OK ) cout << sensorName<< ": Recording to " << streamNameOpened << endl;
	else {
		cout << sensorName << ": Could not initialize stream! Returning!" << endl;
		stopCamera();
		timestampFile.close();
		return;
	}

	cout << sensorName << " capturing..." << endl;

	SYSTEMTIME winTime;
	unsigned long cycleTime;
	while (1) {
		// Capture until the stop event signals
		DWORD retVal = WaitForSingleObject(stopEvent, 0);
		if (retVal == WAIT_OBJECT_0) {
			cout << sensorName << ": Stopping capture..." << endl;
			stopCamera();
			timestampFile.close();
			break;
		}

		error = ladybugLockNext( context, &image_Current );

		double timeDiff = 0;
		int droppedFrames = 0;
		double time;
		switch ( error ) {
		case LADYBUG_OK:
			// Calculate frame rate
			timeDiff = (( image_Current.timeStamp.ulCycleSeconds * 8000.0 + 
				image_Current.timeStamp.ulCycleCount ) -  
				( image_Prev.timeStamp.ulCycleSeconds * 8000.0 + 
				image_Prev.timeStamp.ulCycleCount ) ) / 8000.0;
			frameRate =  1.0 / timeDiff;

			// Save the image to disk
			if ( streamContext != NULL ) {
				error = ladybugWriteImageToStream( streamContext, &image_Current, &totalMBWritten, &totalNumberOfImagesWritten ); 
				if ( error != LADYBUG_OK ) {
					stopCamera();
					timestampFile.close();
					return;
				}
			}
			
			GetSystemTime(&winTime);
			ladybugGetRegister( context, 0xFF100200, &cycleTime);
			time = imageTimeStampToSeconds(cycleTime);
			// Write to timestamp file
			timestampFile << camInfo.serialHead << ' '
				<< totalNumberOfImagesWritten << ' '
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

			// Check if any frames were dropped
			//droppedFrames = (int) round(timeDiff/(1.0/settings.frameRate.valueA)) - 1;
			droppedFrames = 0;	//Disable for now
			if (droppedFrames > 0) {
				droppedFrameCounter += (unsigned int) droppedFrames;
				cout << sensorName << ": dropped " << droppedFrames << " frame(s) before frame " << totalNumberOfImagesWritten << "! | dT: " << timeDiff << "s | Total: " << droppedFrameCounter << endl;
			}

			ladybugUnlock( context, image_Prev.uiBufferIndex );
			image_Prev = image_Current;
			if ( error != LADYBUG_OK ) printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) );
			break;      
		case LADYBUG_TIMEOUT:
			Sleep(10);
			break;

		default: 
			ladybugUnlockAll( context );
			if ( error != LADYBUG_OK ) printf( "Ladybug library reported %s\n", ::ladybugErrorToString( error ) );
			Sleep(10);
		}
	}

	cout << sensorName << ": Captured frames: " << totalNumberOfImagesWritten << " | Dropped frames: " << droppedFrameCounter << endl;
	cout << sensorName << ": Returning from capture thread..." << endl;
	return;
}

LadybugError LadybugReader::setProperty( LadybugContext& context, LadybugProperty flyProp, CameraProperties::CameraProperty const & camProp ) {
	LadybugError error;

	if (camProp.absVal) {
		error = ladybugSetAbsPropertyEx( context, flyProp, FALSE, camProp.onOff, camProp.autoManual, (float)camProp.valueA);
	}
	else {
		error = ladybugSetPropertyEx( context, flyProp, FALSE, camProp.onOff, camProp.autoManual, (int)camProp.valueA, (int)camProp.valueB);
	}

	return(error);
}

LadybugError LadybugReader::setRangeRegister( LadybugContext& context, unsigned long registerAddr, unsigned int minValue, unsigned int maxValue ){
	LadybugError error;

	// Ensure these are 12-bit values
	if (minValue > 0xFFF) minValue = 0xFFF;
	if (maxValue > 0xFFF) maxValue = 0xFFF;

	unsigned long curRegisterValue, newRegisterValue;

	// Get current register value
	error = ladybugGetRegister( context, registerAddr, &curRegisterValue);

	// Build new register value
	newRegisterValue = (curRegisterValue & 0xFF000000);
	newRegisterValue = newRegisterValue | ( (unsigned long) minValue ) << 12;
	newRegisterValue = newRegisterValue | ( (unsigned long) maxValue );
	error = ladybugSetRegister( context, registerAddr, newRegisterValue );

	return(error);
}

void LadybugReader::CameraProperties::CameraProperty::display() const {
	stringstream output;
	if (onOff) cout << "ON  | ";
	else cout << "OFF | ";
	if (autoManual) cout << " AUTO  | ";
	else cout << "MANUAL | ";
	if (absVal) cout << "ABS | ";
	else cout << "REL | ";
	cout.setf(ios::fixed);
	cout.width(10);
	cout.precision(3);
	cout << valueA << " | " << valueB << endl;
}

void LadybugReader::CameraProperties::CameraProperty::set(bool onOffIn, bool autoManualIn, bool absValIn, double valueAIn, double valueBIn){
	onOff = onOffIn;
	autoManual = autoManualIn;
	absVal = absValIn;
	valueA = valueAIn;
	valueB = valueBIn;
}

/// Sets default values for the camera properties
LadybugReader::CameraProperties::CameraProperties() : serialNumber(0), configNumber(0) {
	// on/off, auto/manual, absVal, valueA, valueB
	frameRate.set(1, 0, 1, 15.0, 0);
	brightness.set(1, 1, 1, 0, 0);			// Brightness auto
	exposure.set(1, 1, 1, 0, 0);			// Exposure auto
	whiteBalance.set(1, 1, 0, 500, 500);	// White Balance auto
	gamma.set(1, 0, 1, 1, 0);				// Gamma 1.0
	shutter.set(1, 1, 1, 10, 0);			// Auto Shutter
	gain.set(1, 1, 1, 0, 0);				// Auto Gain
	autoExposureRange.set(1, 1, 0, 0, 0xFFF);	// Full exposure range
	//autoShutterRange.set(1, 1, 0, 0, 0xFFF);	// Full shutter range
	autoShutterRange.set(1, 1, 0, 0, 1290);	// Full shutter range
	autoGainRange.set(1, 1, 0, 0, 0xFFF);		// Full gain range
	aeCamerasUsed[0] = 1;
	aeCamerasUsed[1] = 1;
	aeCamerasUsed[2] = 1;
	aeCamerasUsed[3] = 1;
	aeCamerasUsed[4] = 1;
	aeCamerasUsed[5] = 0;	// Don't use sky camera for AE

	triggerMode.bOnOff = 1;
	triggerMode.uiMode = 15;
	triggerMode.uiParameter = 15;
	triggerMode.uiPolarity = 1;
	triggerMode.uiSource = 2;

	strobes.push_back(LadybugStrobeControl());
	strobes.back().bOnOff = 1;
	strobes.back().uiSource = 1;
	strobes.back().uiPolarity = 1;
	strobes.back().fDelay = 0;
	strobes.back().fDuration = 1.0;

	resolution = LADYBUG_RESOLUTION_ANY;
	
	jpegAutoBufferUsage = 0;
	jpegAutoQualityControl = 1;
	jpegCompressionQuality = 80;
}

void LadybugReader::CameraProperties::display() const {
	cout << "Ladybug Serial #: " << serialNumber << endl;
	cout << "-------------------------------------" << endl;
	cout << "Frame Rate     |  ";
	frameRate.display();
	cout << "Brightness     |  ";
	brightness.display();
	cout << "Exposure       |  ";
	exposure.display();
	cout << "White Balance  |  ";
	whiteBalance.display();
	cout << "Gamma          |  ";
	gamma.display();
	cout << "Shutter        |  ";
	shutter.display();
	cout << "Gain           |  ";
	gain.display();
	cout << "Auto Exposure  |  ";
	autoExposureRange.display();
	cout << "Auto Shutter   |  ";
	autoShutterRange.display();
	cout << "Auto Gain      |  ";
	autoGainRange.display();
	cout << endl;
	cout << "Cameras used for AE | ";
	for (int i = 0; i < 6; i++) {
		cout << i+1 << ":" << ( aeCamerasUsed[i] ? "YES" : "NO" );
		if (i < 5) cout << " | ";
		else cout << endl;
	}

	stringstream output;
	output << "Trigger mode   |  ";
	if (triggerMode.bOnOff) output << "ON  | ";
	else output << "OFF | ";
	output << "Src:   " << setw(2) << triggerMode.uiSource << " | ";
	output << "Mode:  " << setw(2) << triggerMode.uiMode << " | ";
	output << "Param: " << setw(2) << triggerMode.uiParameter << " | ";
	output << "Pol:   " << setw(2) << triggerMode.uiPolarity;
	
	cout << output.str() << endl;

	for (size_t i = 0; i < strobes.size(); i++) {
		output.str("");
		output << "Strobe         |  ";
		if (strobes[i].bOnOff) output << "ON  | ";
		else output << "OFF | ";
		output << "Src:   " << setw(2) << strobes[i].uiSource << " | ";
		output << "Delay: " << setprecision(3) << strobes[i].fDelay << " | ";
		output << "Dur:   " << setprecision(3) << strobes[i].fDuration << " | ";
		output << "Pol:   " << setw(2) << strobes[i].uiPolarity;
		cout << output.str() << endl;
	}

	cout << endl;

}


