/**
@file	ProgramInfo.h
@brief	This header file declares the class used to store program data and parse the configuration file.
@authors John Kua
*/

#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#ifdef PROGINFO_USE_LADYBUG
#include "LadybugReader.h"
#endif
#ifdef PROGINFO_USE_VELODYNE
#include "VelodyneReader.h"
#endif

using namespace std;

/// @brief Class to store program settings and read the configuration file.
class ProgramInfo {

public:
	/// @brief Constructor. Initializes data members to defaults.
	ProgramInfo();

	/// @brief Checks to see if a configuration file has been loaded
	/// @return TRUE if a configuration file has been loaded
	bool getConfigLoaded() const;

	/// @brief Returns the number of scanners configured
	/// @return The number of scanners configured
	unsigned int getNumScanners() const;

	/// @brief Returns the number of cameras configured
	/// @return The number of cameras configured
	unsigned int getNumCameras() const;

	/// @brief Returns the number of Velodynes configured
	/// @return The number of Velodynes configured
	unsigned int getNumVelodynes() const;

	/// @brief Checks if the Applanix is configured
	/// @return TRUE if the Applanix is being used
	bool getUseApplanix() const;

	/// @brief Checks if the Intersense is configured
	/// @return TRUE if the Intersense is being used
	bool getUseIntersense() const;

	/// @brief Returns the number of sensors configured
	/// @return The number of sensors configured: numCameras + numScanners + useIntersense
	unsigned int getNumSensors() const;

	/// @brief Returns the port number of the requested laser scanner
	/// @param[in] scannerNum Selected scanner number (0 to numScanners-1). This is not sanity checked!
	/// @return The port number, e.g 15 for COM15
	unsigned int getScannerPort(unsigned int scannerNum) const;

	/// @brief Returns the port name of the requested laser scanner
	/// @param[in] scannerNum Selected scanner number (0 to numScanners-1). This is not sanity checked!
	/// @return The port name, e.g COM15.
	string getScannerPortName(unsigned int scannerNum) const;

	/// @brief Returns the port number of the Applanix serial trigger
	/// @return The port number, e.g 15 for COM15. If 0, the Applanix is not configured.
	unsigned int getApplanixPort() const;

	/// @brief Returns the port number of the Intersense IMU
	/// @return The port number, e.g 15 for COM15. If 0, the Intersense IMU is not configured.
	unsigned int getIntersensePort() const;

#ifdef PROGINFO_USE_LADYBUG
	/// @brief Get the properties for a camera
	/// @param[in] serialNumber Serial number of the desired camera.
	/// @param[out] properties Camera properties object.
	/// @return 1 if success, -1 if that serial number not found.
	int getCameraProperties(unsigned long serialNumber, LadybugReader::CameraProperties& properties) const;
#endif

	/// @brief Get the camera frame rate
	/// @return Camera frame rate in frames/second. 
	///         This will return the default value (5.0) if not cameras are configured
	float getCameraFrameRate() const;

	/// @brief Read in the configuration from a file.
	/// @param[in] fileName Path to the configuration file
	/// @return 1 if success, -1 if error.
	int readConfigFile(string fileName);

	/// @brief Display the configuration data.
	void displayConfig();

#ifdef PROGINFO_USE_LADYBUG
	/// Contains the camera properties for each camera, indexed by serial number.
	map<unsigned long, LadybugReader::CameraProperties> cameraMap;
#endif

#ifdef PROGINFO_USE_VELODYNE
	vector<VelodyneReader::VelodyneProperties> velodyneProps;
#endif

private:
	bool configLoaded;			///< TRUE if the configuration is loaded
	unsigned int numScanners;	///< The number of laser scanners configured
	unsigned int numCameras;	///< The number of cameras configured
	unsigned int numVelodynes;
	bool useApplanix;			///< TRUE if the Applanix trigger port is configured
	bool useIntersense;			///< TRUE if the Intersense IMU is configured
	unsigned int numSensors;	///< The total number of sensors configured, including the laser scanners, cameras, and Intersense IMU

	float cameraFrameRate;		///< The configured frame rate for the cameras
	
	vector<unsigned int> scannerPorts;	///< The list of laser scanner port numbers
	vector<string> scannerPortNames;	///< The list of laser scanner port names
	unsigned int applanixPort;			///< The Applanix trigger port number
	unsigned int intersensePort;		///< The Intersense IMU port number

#ifdef PROGINFO_USE_LADYBUG
	/// @brief Parses a camera property line from a config file
	/// @param[in] inputLine The input property line
	/// @param[out] camProps The parsed camera properties
	/// @return 1 if success, -1 if error
	int parseCameraProperties(stringstream& inputLine, LadybugReader::CameraProperties& camProps);
#endif

};

#endif