/**
\file	VelodyneReader.cpp
\brief	Defines methods for Velodyne packet capture
\authors John Kua
*/

#include "VelodyneReader.h"

VelodyneReader::VelodyneStatic VelodyneReader::staticData;

int VelodyneReader::getInterfaceList() {
	if (pcap_findalldevs(&staticData.alldevs, staticData.errbuf) == -1) {
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", staticData.errbuf);
		return -1;
	}
	pcap_if_t *d;
	staticData.numInterfaces = 0;
	for(d=staticData.alldevs; d; d=d->next) ++staticData.numInterfaces;
	return 0;
}
int VelodyneReader::getNumInterfaces() {
	int retVal;
	if (staticData.alldevs == NULL) {
		retVal = getInterfaceList();
		if (retVal < 0) return -1;
	}
	return staticData.numInterfaces;
}
int VelodyneReader::displayInterfaceList() {
	int retVal;
	if (staticData.alldevs == NULL) {
		retVal = getInterfaceList();
		if (retVal < 0) return -1;
	}
	pcap_if_t *d;
	int i = 0;
	for(d=staticData.alldevs; d; d=d->next)
    {
        printf("%d. %s", ++i, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }
	return 0;
}

int VelodyneReader::openInterface(unsigned int ifaceNum) {
	int retVal;
	if (staticData.alldevs == NULL) {
		retVal = getInterfaceList();
		if (retVal < 0) return -1;
	}
	if (staticData.numInterfaces == 0) {
		cerr << "There are no valid interfaces!" << endl;
		return -1;
	}
	if (ifaceNum < 1 || ifaceNum > staticData.numInterfaces) {
		cerr << "Invalid interface number! Valid interfaces are from 1-" << staticData.numInterfaces << endl;
		return -1;
	}
	if (adhandle != NULL) {
		cerr << "Interface already open!" << endl;
		return -1;
	}

	pcap_if_t *d;
	unsigned int i;
	/* Jump to the selected adapter */
	for(d=staticData.alldevs, i=0; i< ifaceNum-1 ;d=d->next, i++);

	// Open interface
	if ((adhandle= pcap_open_live(d->name,	// name of the device
							 65536,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 2000,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		return -1;
	}

	return 0;
}
int VelodyneReader::closeInterface() {
	if (adhandle) {
		pcap_close(adhandle);
		adhandle = NULL;
	}
	return 0;
}

int VelodyneReader::checkForDevice(IpAddress deviceAddress, string filterString) {
	if (adhandle == NULL) {
		cerr << "Interface is not open!" << endl;
		return -1;
	}

	string filter;
	if (filterString.compare("DEFAULT") == 0) {
		filter = "host " + deviceAddress.str() + " and (udp dst port 2368 or udp dst port 8308)";
	}
	else filter = filterString;
	cout << "Filter: " << filter << endl;

	//compile the filter
	struct bpf_program fcode;
	bpf_u_int32 netmask = 0xffffffff; // This shouldn't matter unless we're trying to set an IP broadcast filter?
	if (!filter.empty()) {
		if (pcap_compile(adhandle, &fcode, filter.c_str(), 1, netmask) < 0 ) {
	        fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
			return -1;
		}
		closeInterface();
		openInterface(1);
	    
	    //set the filter
	    if (pcap_setfilter(adhandle, &fcode) < 0) {
			fprintf(stderr,"\nError setting the filter.\n");
			return -1;
		}
	}
	else cout << "Skipping filter!" << endl;

	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	cout << "Checking for device...";

	for (int i = 0; i < 10; i++) {
		int retVal = pcap_next_ex( adhandle, &header, &pkt_data );
		if (retVal == 0) {	// timeout
			cout << "Timed out waiting for packet" << endl;
			return -1;
		}
		else {
			//cout << "Captured packet " << i+1 << ": ";
			//printPacketInfo(header, pkt_data);
			cout << i+1 << "...";
		}
	}
	cout << "found!" << endl;

	return 0;
}

int VelodyneReader::printPacketInfo(const struct pcap_pkthdr *header, const u_char *pkt_data) {
    struct tm ltime;
    char timestr[16];
    ip_header *ih;
    udp_header *uh;
    u_int ip_len;
    u_short sport,dport;
    time_t local_tv_sec;

    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);

    /* print timestamp and length of the packet */
    printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

    /* retireve the position of the ip header */
    ih = (ip_header *) (pkt_data +
        14); //length of ethernet header

    /* retireve the position of the udp header */
    ip_len = (ih->ver_ihl & 0xf) * 4;
    uh = (udp_header *) ((u_char*)ih + ip_len);

    /* convert from network byte order to host byte order */
    sport = ntohs( uh->sport );
    dport = ntohs( uh->dport );

    /* print ip addresses and udp ports */
    printf("%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d\n",
        ih->saddr.byte1,
        ih->saddr.byte2,
        ih->saddr.byte3,
        ih->saddr.byte4,
        sport,
        ih->daddr.byte1,
        ih->daddr.byte2,
        ih->daddr.byte3,
        ih->daddr.byte4,
        dport);

	return 0;
}

uint64_t VelodyneReader::getPacketsCaptured() {
	return packetsCaptured;
}
string VelodyneReader::getSensorName() {
	return sensorName;
}

int VelodyneReader::initialize(unsigned int interfaceNum, IpAddress deviceAddress, string filterString) {
	int retVal = openInterface(interfaceNum);
	if (retVal < 0) {
		cerr << "Failed to open interface " << interfaceNum << "!" << endl;
		return -1;
	}

	serialNumber = convertIpAddressToSerialNumberString(deviceAddress);
	stringstream ss;
	ss << baseSensorName << " " + serialNumber + " / " + deviceAddress.str();
	sensorName = ss.str();

	retVal = checkForDevice(deviceAddress, filterString);
	if (retVal < 0) {
		cerr << "Failed to find a lidar operating at address: " << deviceAddress.str() << endl;
		return -1;
	}

	return 0;
}

HANDLE VelodyneReader::startCapture ( void ) {

	// Autogenerate a filename from the time and path if one has not been set
	if ( writeFileName.empty() ) setWriteFileName();
	dumpfile = pcap_dump_open(adhandle, writeFileName.c_str());

	if (dumpfile == NULL) {
		cerr << "\nError opening output file: " << writeFileName << endl;
		return NULL;
	}

	// Start thread
	//threadHandle = (HANDLE)_beginthread(threadStaticEntryPoint,0,(void *)this);	// _beginthread automatically closes the thread handle at termination - we need it to stick around so we know the thread closed properly. _beginthreadex leaves it open.
	threadHandle = (HANDLE)_beginthreadex(NULL, 0, threadStaticEntryPoint, (void *)this, 0, &threadID);	// Start thread

	return threadHandle;
}

unsigned __stdcall VelodyneReader::threadStaticEntryPoint(void * pThis){

	VelodyneReader * pthX = (VelodyneReader*)pThis;   // the tricky cast
	pthX->packetCaptureThread();           // now call the true entry-point-function

	// A thread terminates automatically if it completes execution,
	// or it can terminate itself with a call to _endthread().

	return 0;
}

void VelodyneReader::packetCaptureThread() {
	pcap_loop(adhandle, 0, packetHandlerWrapper, (unsigned char *)this);
	pcap_dump_close(dumpfile);
	cout << sensorName << ": Captured packets: " << packetsCaptured << endl;
	cout << sensorName << ": Returning from capture thread..." << endl;
	return;
}

void VelodyneReader::packetHandlerWrapper(u_char* pThis, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	VelodyneReader * pthX = (VelodyneReader*)pThis;   // the tricky cast
	pthX->packetHandler(header, pkt_data);           // now call the true entry-point-function
}
void VelodyneReader::packetHandler(const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	/* save the packet on the dump file */
	pcap_dump((u_char*)dumpfile, header, pkt_data);

	packetsCaptured++;

	// Capture until the stop event signals
	DWORD retVal = WaitForSingleObject(stopEvent, 0);
	if (retVal == WAIT_OBJECT_0) {
		cout << sensorName << ": Stopping capture..." << endl;
		pcap_breakloop(adhandle);
	}
}

uint64_t VelodyneReader::convertIpAddressToSerialNumber(IpAddress address) {
	return ((address.byte3 << 8) | address.byte4) + 711020000;
}
string VelodyneReader::convertIpAddressToSerialNumberString(IpAddress address) {
	uint64_t serialNumber = convertIpAddressToSerialNumber(address);
	stringstream ss;
	ss << serialNumber;
	return ss.str();
}
VelodyneReader::IpAddress VelodyneReader::convertSerialNumberToIpAddress(uint64_t serialNumber) {
	serialNumber -= 711020000;
	IpAddress address;
	address.byte1 = 192;
	address.byte2 = 168;
	address.byte3 = (u_char) (serialNumber >> 8);
	address.byte4 = (u_char) (serialNumber & 0xff);
	return address;
}
VelodyneReader::IpAddress VelodyneReader::convertSerialNumberToIpAddress(string serialNumber) {
	uint64_t number;
	stringstream ss;
	ss.str(serialNumber);
	ss >> number;
	return convertSerialNumberToIpAddress(number);
}