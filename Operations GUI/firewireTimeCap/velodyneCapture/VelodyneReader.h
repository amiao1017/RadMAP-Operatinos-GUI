/**
\file	VelodyneReader.h
\brief	This class is used to capture packets from a Velodyne lidar
\authors John Kua
*/

#ifndef VELODYNEREADER_H
#define VELODYNEREADER_H

#include "pcap.h"	// This must be before Windows.h
#include "Sensor.h"


using namespace std;

class VelodyneReader : public Sensor {
public:
	VelodyneReader() : interfaceNumber(0), packetsCaptured(0) {
		baseSensorName = "Velodyne";
		sensorName = baseSensorName;
		writeFileExtension = ".pcap";

		adhandle = NULL;
		deviceAddress.byte1 = 192;
		deviceAddress.byte2 = 168;
		deviceAddress.byte3 = 0;
		deviceAddress.byte4 = 0;
	}
	~VelodyneReader() {
		if (adhandle) pcap_close(adhandle);
	}
	class IpAddress {
	public:
		IpAddress() : byte1(0), byte2(0), byte3(0), byte4(0) {
		}
		IpAddress(u_char b1, u_char b2, u_char b3, u_char b4) {
			byte1 = b1;
			byte2 = b2;
			byte3 = b3;
			byte4 = b4;
		}
		IpAddress(string addressString) {
			stringstream ss;
			string s;
			ss << addressString;
			getline(ss, s, '.');
			byte1 = atoi(s.c_str());
			getline(ss, s, '.');
			byte2 = atoi(s.c_str());
			getline(ss, s, '.');
			byte3 = atoi(s.c_str());
			getline(ss, s, '.');
			byte4 = atoi(s.c_str());
		}
		u_char byte1;
		u_char byte2;
		u_char byte3;
		u_char byte4;

		string str() {
			stringstream ss;
			ss << (unsigned int)byte1 << '.' << (unsigned int)byte2 << '.' << (unsigned int)byte3 << '.' << (unsigned int)byte4;
			return ss.str();
		}
	};
	typedef struct ip_address{
		u_char byte1;
		u_char byte2;
		u_char byte3;
		u_char byte4;
	};

	/* IPv4 header */
	typedef struct ip_header{
	    u_char  ver_ihl;        // Version (4 bits) + Internet header length (4 bits)
	    u_char  tos;            // Type of service 
	    u_short tlen;           // Total length 
	    u_short identification; // Identification
	    u_short flags_fo;       // Flags (3 bits) + Fragment offset (13 bits)
	    u_char  ttl;            // Time to live
	    u_char  proto;          // Protocol
	    u_short crc;            // Header checksum
	    ip_address  saddr;      // Source address
	    ip_address  daddr;      // Destination address
	    u_int   op_pad;         // Option + Padding
	};

	/* UDP header*/
	typedef struct udp_header{
	    u_short sport;          // Source port
	    u_short dport;          // Destination port
	    u_short len;            // Datagram length
	    u_short crc;            // Checksum
	};

	class VelodyneProperties {
	public:
		VelodyneProperties(): serialNumber(0), configNumber(0) {}

		/// Displays all the camera properties, except current*
		//void display() const;
		unsigned long serialNumber;
		unsigned int configNumber;

	} settings;

	static int getInterfaceList();
	static int getNumInterfaces();
	static int displayInterfaceList();
	int checkForDevice(IpAddress deviceAddress, string filterString = "DEFAULT");
	int printPacketInfo(const struct pcap_pkthdr *header, const u_char *pkt_data);
	uint64_t getPacketsCaptured();
	string getSensorName();
	static uint64_t convertIpAddressToSerialNumber(IpAddress address);
	static string convertIpAddressToSerialNumberString(IpAddress address);
	static IpAddress convertSerialNumberToIpAddress(uint64_t serialNumber);
	static IpAddress convertSerialNumberToIpAddress(string serialNumber);

	int initialize(unsigned int interfaceNum, IpAddress deviceAddress, string filterString = "DEFAULT");

	/// @brief Starts packet capture by launching the thread for PacketCaptureThread(). 
	virtual HANDLE startCapture ( void );

private:
	static class VelodyneStatic {
	private:
		VelodyneStatic() : numInterfaces(0) {
			alldevs = NULL;
		}
		~VelodyneStatic() {
			pcap_freealldevs(alldevs);
		}
		friend class VelodyneReader;

		pcap_if_t *alldevs;
		char errbuf[PCAP_ERRBUF_SIZE];
		unsigned int numInterfaces;
	} staticData;

	int openInterface(unsigned int interfaceNum);
	int openInterface();
	int closeInterface();

	/// @brief The special form for the _beginthread call. Only for launching the capture thread.
	static unsigned __stdcall threadStaticEntryPoint(void * pThis);

	/// @brief The special form for the _beginthread call. Only for launching the capture thread.
	static void callback(void * pThis);

	/// @brief The actual image capture loop where everything is done.
	void packetCaptureThread();

	/// @bried Callback function called by libpcap for every packet
	static void packetHandlerWrapper(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data);
	void packetHandler(const struct pcap_pkthdr* header, const u_char* pkt_data);

	unsigned int interfaceNumber;
	IpAddress deviceAddress;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_dumper_t *dumpfile;

	uint64_t packetsCaptured;
};

#endif