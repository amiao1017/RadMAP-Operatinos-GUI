/**
\file	LadybugReader.h
\brief	This class is used to capture images from a Point Grey Ladybug camera
\authors John Kua
*/

#ifndef LADYBUGREADER_H
#define LADYBUGREADER_H

#include "Sensor.h"

#include <ladybug.h>
#include <ladybugGPS.h>
#include <ladybuggeom.h>
#include <ladybugRenderer.h>
#include <ladybugStream.h>

using namespace std;

///@{ 
/// @brief Camera register addresses
#define PGR_INITIALIZE			0x000
#define PGR_CURRENT_FRAME_RATE	0x600
#define PGR_POWER				0x610
#define PGR_BRIGHTNESS			0x800
#define PGR_AUTO_EXPOSURE		0x804
#define PGR_SHARPNESS			0x808
#define PGR_WHITE_BALANCE		0x80C
#define PGR_HUE					0x810
#define PGR_SATURATION			0x814
#define PGR_GAMMA				0x818
#define PGR_SHUTTER				0x81C
#define PGR_GAIN				0x820
#define PGR_IRIS				0x824
#define PGR_FOCUS				0x828
#define PGR_TEMPERATURE			0x82C
#define PGR_TRIGGER_MODE		0x830
#define PGR_TRIGGER_DELAY		0x834
#define PGR_FRAME_RATE			0x83C
#define PGR_PAN					0x884
#define PGR_TILT				0x888
#define PGR_ABS_AUTO_EXPOSURE	0x908
#define PGR_ABS_SHUTTER			0x918
#define PGR_ABS_GAIN			0x928
#define PGR_ABS_BRIGHTNESS		0x938
#define PGR_ABS_GAMMA			0x948
#define PGR_ABS_TRIGGER_DELAY	0x958
#define PGR_ABS_FRAME_RATE		0x968
#define PGR_IMAGE_DATA_FORMAT	0x1048
#define PGR_AUTO_EXPOSURE_RANGE	0x1088
#define PGR_AUTO_SHUTTER_RANGE	0x1098
#define PGR_AUTO_GAIN_RANGE		0x10A0
#define PGR_PIO_DIRECTION		0x11F8
#define PGR_FRAME_INFO			0x12F8
#define PGR_AE_STATS_MASK		0x1E90
///@}

//
// Macros to check, report on, and handle Ladybug API error codes.
//
#define _HANDLE_ERROR \
   if( error != LADYBUG_OK ) \
   { \
   printf( "Error! Ladybug library reported %s\n", \
   ::ladybugErrorToString( error ) ); \
   assert( false ); \
   exit( 1 ); \
   } \

#define _DISPLAY_ERROR_MSG_AND_RETURN \
   if( error != LADYBUG_OK ) \
   { \
   printf( "Ladybug library reported %s\n", \
   ::ladybugErrorToString( error ) ); \
   return; \
   } \

/*
/// @brief Contains and initializes static members for the LadybugReader class
class LadybugStatic {
public:
	LadybugStatic() : numCamerasOnBus(0), camerasEnumerated(0) {
		LadybugError error = ::ladybugCreateContext( &context );
		_HANDLE_ERROR;
		
		// Assume some number of cameras to begin with
		camInfoAll = new LadybugCameraInfo[estimatedNumCameras];
	}
	~LadybugStatic() {
		delete[] camInfoAll;
	}
	friend class LadybugReader;
private:
	/// The "instance" of the Ladybug library
	LadybugContext context;

	static const unsigned int estimatedNumCameras = 2;

	/// Array of info structures for all cameras on the bus
	LadybugCameraInfo* camInfoAll;

	unsigned int numCamerasOnBus;
	bool camerasEnumerated;
};
*/

/// @brief Used to configure and read from Point Grey Ladybug cameras. 
///
/// Also includes methods to capture time synchronization data.
/// One instance is used for all cameras on a FireWire bus.
/// See the Ladybug API Programming Reference, the Ladybug SDK Reference, and the Ladybug3 Technical Reference.
class LadybugReader : public Sensor
{
public:		
	/// @brief Constructor. Allocates the image buffers and sets the default exposure mode and frame rate.
	LadybugReader();

	/// @brief Destructor. Deallocates the images buffers.
	~LadybugReader();

	/// @brief Enumerate the cameras on the bus and get information about each
	/// @return Number of cameras found, -1 if error
	static int enumerateCameras ( void );

	/// @brief Display camera info given a pointer to a LadybugCameraInfo struct
	/// @return 0 if success, -1 if error;
	static int displayCameraInfo ( const LadybugCameraInfo& cameraInfo );
	/// @brief Display camera info for the camera initialized in this context
	/// @return 0 if success, -1 if error;
	int displayCameraInfo ( void );
	/// @brief Display camera info for all the cameras on the bus
	/// @return 0 if success, -1 if error;
	static int displayCameraInfoAll ( void );
	/// @brief Display camera info for a specific camera on the bus
	/// @return 0 if success, -1 if error;
	static int displayCameraInfoAll ( const unsigned int cameraNum );

	/// @brief Find the camera with the given serial number
	/// @return 0 if success, -1 if error;
	static int findCameraBySerialNumber( const LadybugSerialNumber serialNumber, unsigned int& cameraNum );

	/// @brief Initialize a camera by index and allocates buffers via the Ladybug API
	/// @return 0 if success, -1 if error;
	int initializeCamera( const unsigned int cameraNum, const unsigned int numBuffers = 40 );
	/// @brief Initialize a camera by serial number and allocates buffers via the Ladybug API
	/// @return 0 if success, -1 if error;
	int initializeCameraBySerialNumber( const LadybugSerialNumber serialNumber, const unsigned int numBuffers = 40 );
	/// @brief Initialize a camera by bus index and allocates buffers via the Ladybug API - the above two methods end up calling this
	/// @return 0 if success, -1 if error;
	int initializeCameraByBusIndex ( const unsigned int busIndex, const unsigned int numBuffers = 40 );

	/// @brief Starts image capture by launching the thread for ImageCaptureThread(). 
	virtual HANDLE startCapture ( void );

	unsigned long getNumImagesWritten();
	double getFrameRate();
	double getMegabytesWritten();
	string getSensorName();

	class CameraProperties {
	public:
		CameraProperties();

		/// Displays all the camera properties, except current*
		void display() const;

		class CameraProperty {
		public:
			bool onOff;			///< Enables or disables the camera property
			bool autoManual;	///< Sets whether the camera property is automatically of manually configured. Not supported by all properties.
			bool absVal;		///< Sets whether the values #valueA and #valueB are absolute or relative values. Not supported by all properties and the units are property specific.
			double valueA;		///< Value of the property. If both #valueA and #valueB are used, #valueA is the min value and #valueB is the max value.
			double valueB;		///< If both #valueA and #valueB are used, #valueA is the min value and #valueB is the max value.
		
			/// Prints the property values
			void display() const;
		
			/// Sets all the property values
			void set(bool onOff, bool autoManual, bool absVal, double valueA, double valueB);
		};
		CameraProperty brightness;			///< Brightness property - black level
		CameraProperty exposure;			///< Exposure property - desired average intensity
		CameraProperty whiteBalance;		///< White balance property - relative RGB gains
		CameraProperty gamma;				///< Gamma property - this controls the nonlinear input/output mapping
		CameraProperty shutter;				///< Shutter speed
		CameraProperty gain;				///< A/D converter gain
		CameraProperty autoExposureRange;	///< Controls the min/max exposure values when in auto
		CameraProperty autoShutterRange;	///< Controls the min/max shutter values when in auto
		CameraProperty autoGainRange;		///< Controls the min/max gain values when in auto
		CameraProperty frameRate;
		LadybugSerialNumber serialNumber;
		int busNumber;
		LadybugResolution resolution;
		bool aeCamerasUsed[6];

		LadybugTriggerMode triggerMode;
		vector<LadybugStrobeControl> strobes;

		unsigned int jpegAutoBufferUsage;
		bool jpegAutoQualityControl;
		int jpegCompressionQuality;

		string position;

		unsigned int configNumber;

	} settings;

	LadybugError setProperty( LadybugContext& context, LadybugProperty flyProp, CameraProperties::CameraProperty const & camProp );
	LadybugError setRangeRegister( LadybugContext& context, unsigned long registerAddr, unsigned int minValue, unsigned int maxValue );

	/// @brief Converts a raw timestamp value to a time in seconds.
	/// @param[in] uiRawTimestamp is the raw timestamp value as read from the camera register or from the image header
	/// @return Time in seconds
	static double imageTimeStampToSeconds(unsigned int uiRawTimestamp);
	
/*
	/// @brief Initialize the cameras on the bus with the settings stored in #cameraMap.
	/// @return TRUE if initialization successful
	BOOL InitializeCameras ( void );

	/// @brief Disconnects the cameras.
	void PowerDownCameras( void );

	/// @brief Reads a timestamp value from the camera bus.
	/// @return Raw timestamp value. Use imageTimeStampToSeconds() to convert.
	unsigned long GetTimeStamp ( void );



	/// @brief Gets the last timestamp recorded by a particular camera.
	/// @param[in] cameraNumber The index of the camera you wish to query.
	/// @return The timestamp value in seconds.
	double getLastTimestamp(int cameraNumber);

	/// @brief Gets the serial number for a particular camera.
	/// @param[in] cameraNumber The index of the camera you wish to query.
	/// @return The serial number of the camera.
	unsigned long getSerialNumber(int cameraNumber);

	/// @brief Gets the chassis temperature recorded by a particular camera.
	/// @param[in] cameraNumber The index of the camera you wish to query.
	/// @return The camera temperature in celsius.
	double getTemperature(int cameraNumber);

	/// @brief Gets the number of images captured.
	/// @return The number of images.
	int getImageCount();

	/// @brief Enables/disables auto exposure for all cameras.
	/// @param[in] mode 1 for auto exposure, 0 for manual exposure.
	/// @return 1 for success, -1 for error.
	int setAutoExposureMode(int mode);

	/// @brief Sets the capture frame rate for all cameras.
	/// @param[in] rate Frame rate in frames/second.
	void setFrameRate(float rate);

*/
private:	
	//static LadybugStatic staticData;
	/// @brief Contains and initializes static members for the LadybugReader class
	static class LadybugStatic {
	private:
		LadybugStatic() : numCamerasOnBus(0), camerasEnumerated(0) {
			LadybugError error = ::ladybugCreateContext( &context );
			_HANDLE_ERROR;
			
			// Assume some number of cameras to begin with
			camInfoAll = new LadybugCameraInfo[estimatedNumCameras];
		}
		~LadybugStatic() {
			delete[] camInfoAll;
		}
		friend class LadybugReader;

		/// The "instance" of the Ladybug library
		LadybugContext context;
	
		static const unsigned int estimatedNumCameras = 2;
	
		/// Array of info structures for all cameras on the bus
		LadybugCameraInfo* camInfoAll;
	
		unsigned int numCamerasOnBus;
		bool camerasEnumerated;
	} staticData;

	/// The "instance" of the Ladybug library
	LadybugContext context;
	
	/// Ladybug error
	LadybugError error;	
	
	/// Info structure for the camera initialized in this context
	LadybugCameraInfo camInfo;

	LadybugDataFormat ladybugDataFormat;
	LadybugImage image_Current, image_Prev; // Ladybug image

	LadybugStreamContext streamContext;	// output stream
	unsigned int	frameCounter;
	double			frameRate;
	double			totalMBWritten;
	unsigned long	totalNumberOfImagesWritten;
	unsigned int	droppedFrameCounter;

	char streamNameOpened[_MAX_PATH];

	bool cameraInitialized;

	/// @brief The special form for the _beginthread call. Only for launching the capture thread.
	static unsigned __stdcall threadStaticEntryPoint(void * pThis);

	/// @brief The actual image capture loop where everything is done.
	void imageCaptureThread();

	/// @brief Internal stop camera capture function
	LadybugError stopCamera();


/*

	/// Pointer to an array of camera serial numbers
	unsigned long * serialNumbers;

	/// The number of cameras detected on the bus
	unsigned int numCamsOnBus;

	/// The capture frame rate in frames/second
	float frameRate;

	/// The FlyCapture error array
	LadybugError   error[MAX_CAMS];	

	

	// =========== Data structures used by the FlyCapture auto detect process ==============
	/// Information structures used by FlyCapture.
	FlyCaptureInfoEx	g_arInfo[ MAX_CAMS ];

	/// Number of cameras on the bus. This is initially set to #MAX_CAMS until the bus is enumerated.
	unsigned int NUM_CAMS;

	/// Stores the current image for each camera.
	//FlyCaptureImagePlus  g_arImageplus[ MAX_CAMS ];

	/// Image buffers for the cameras.
	unsigned char**   g_arpBuffers[ MAX_CAMS ];
	// ==============================================

	/// Temperature values for the cameras in celsius
	double temperature[MAX_CAMS];

	/// The last timestamp captured by each of the cameras, in seconds
	double lastTimestamp[MAX_CAMS];

	/// Appears to be unused.
	int count;

	/// The number of images captured.
	int imageCount;

	/// Appears to be unused.
	char str2[50];
	//char str2 = (char *) malloc (50*sizeof(char));

	/// The auto exposure mode, where 1 is auto and 0 is manual.
	int autoExposureMode;

	/// @brief Used to set camera property registers.
	/// @param[in] context The FlyCapture handle to the camera to be configured.
	/// @param[in] flyProp The FlyCapture property which is to be modified.
	/// @param[in] camProp The values to be set for the camera property.
	/// @return The error values, if any.
	LadybugError setProperty( LadybugContext& context, FlyCaptureProperty flyProp, CameraProperty const & camProp );

	/// @brief Used to set camera range registers, e.g. auto shutter range.
	/// @param[in] context The FlyCapture handle to the camera to be configured.
	/// @param[in] registerAddr The camera address to be written to.
	/// @param[in] minValue The minimum value of the range.
	/// @param[in] maxValue The maximum value of the range.
	/// @return The error values, if any.
	LadybugError setRangeRegister( LadybugContext& context, unsigned long registerAddr, unsigned int minValue, unsigned int maxValue );
	*/
};

#endif
