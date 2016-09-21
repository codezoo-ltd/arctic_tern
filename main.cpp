#include "mbed.h"
#include "Websocket.h"

//------------------------------------------------------------------------------------
// You need to configure these cellular modem / SIM parameters.
// These parameters are ignored for LISA-C200 variants and can be left NULL.
//------------------------------------------------------------------------------------
#include "MDM.h"
#include "GPS.h"
//! Set your secret SIM pin here (e.g. "1234"). Check your SIM manual.
#define SIMPIN      NULL
/*! The APN of your network operator SIM, sometimes it is "internet" check your 
  contract with the network operator. You can also try to look-up your settings in 
google: https://www.google.de/search?q=APN+list */
#define APN         "web.sktelecom.com"
//! Set the user name for your APN, or NULL if not needed
#define USERNAME    NULL
//! Set the password for your APN, or NULL if not needed
#define PASSWORD    NULL
//------------------------------------------------------------------------------------
//#define LARGE_DATA
//#define CELLOCATE
DigitalOut myled(LED1);
Serial pc(SERIAL_TX, SERIAL_RX);

int main() {
	pc.baud(115200);

	int ret;

	unsigned int loopcnt=0;
#ifdef LARGE_DATA
	char buf[2048] = "";
#else
	char buf[512] = "";
#endif
	// Create the GPS object
#if 1   // use GPSI2C class
	GPSI2C gps;
#else   // or GPSSerial class 
	GPSSerial gps; 
#endif

	// Create the modem object
	MDMSerial mdm; // use mdm(D1,D0) if you connect the cellular shield to a C027
	//mdm.setDebug(4); // enable this for debugging issues 
	// initialize the modem 
	MDMParser::DevStatus devStatus = {};
	MDMParser::NetStatus netStatus = {};
	bool mdmOk = mdm.init(SIMPIN, &devStatus);
	mdm.dumpDevStatus(&devStatus);
	if (mdmOk) {
		// wait until we are connected
		mdmOk = mdm.registerNet(&netStatus);
		mdm.dumpNetStatus(&netStatus);
	}
	if (mdmOk)
	{
		// join the internet connection 
		MDMParser::IP ip = mdm.join(APN,USERNAME,PASSWORD);
		if (ip == NOIP)
			printf("Not able to join network");
		else
		{
			mdm.dumpIp(ip);
			printf("Make a Http Post Request\r\n");
			int socket = mdm.socketSocket(MDMParser::IPPROTO_TCP);
			if (socket >= 0)
			{
				mdm.socketSetBlocking(socket, 10000);
				if (mdm.socketConnect(socket, "mbed.org", 80))
				{
					const char http[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\n\r\n";
					mdm.socketSend(socket, http, sizeof(http)-1);

					ret = mdm.socketRecv(socket, buf, sizeof(buf)-1);
					if (ret > 0)
						printf("Socket Recv \"%*s\"\r\n", ret, buf);
					mdm.socketClose(socket);
				}
				mdm.socketFree(socket);
			}

			int port = 7;
			const char* host = "echo.u-blox.com";
			MDMParser::IP ip = mdm.gethostbyname(host);
			char data[] = "\r\nxxx Socket Hello World\r\n"
#ifdef LARGE_DATA
				"00  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"01  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"02  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"03  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"04  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"

				"05  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"06  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"07  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"08  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"09  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"

				"10  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"11  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"12  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"13  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"14  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"

				"15  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"16  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"17  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"18  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
				"19  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
#endif            
				"End\r\n";

			printf("Testing TCP sockets with ECHO server\r\n");
			socket = mdm.socketSocket(MDMParser::IPPROTO_TCP);
			if (socket >= 0)
			{
				mdm.socketSetBlocking(socket, 10000);
				if (mdm.socketConnect(socket, host, port)) {
					memcpy(data, "\r\nTCP", 5); 
					ret = mdm.socketSend(socket, data, sizeof(data)-1);
					if (ret == sizeof(data)-1) {
						printf("Socket Send %d \"%s\"\r\n", ret, data);
					}
					ret = mdm.socketRecv(socket, buf, sizeof(buf)-1);
					if (ret >= 0) {
						printf("Socket Recv %d \"%.*s\"\r\n", ret, ret, buf);
					}
					mdm.socketClose(socket);
				}
				mdm.socketFree(socket);
			}

			printf("Testing UDP sockets with ECHO server\r\n");
			socket = mdm.socketSocket(MDMParser::IPPROTO_UDP, port);
			if (socket >= 0)
			{
				mdm.socketSetBlocking(socket, 10000);
				memcpy(data, "\r\nUDP", 5); 
				ret = mdm.socketSendTo(socket, ip, port, data, sizeof(data)-1);
				if (ret == sizeof(data)-1) {
					printf("Socket SendTo %s:%d " IPSTR " %d \"%s\"\r\n", host, port, IPNUM(ip), ret, data);
				}
				ret = mdm.socketRecvFrom(socket, &ip, &port, buf, sizeof(buf)-1);
				if (ret >= 0) {
					printf("Socket RecvFrom " IPSTR ":%d %d \"%.*s\" \r\n", IPNUM(ip),port, ret, ret,buf);
				}
				mdm.socketFree(socket);
			}

			// disconnect  
			mdm.disconnect();
		}

		// http://www.geckobeach.com/cellular/secrets/gsmcodes.php
		// http://de.wikipedia.org/wiki/USSD-Codes
		const char* ussd = "*130#"; // You may get answer "UNKNOWN APPLICATION"
		printf("Ussd Send Command %s\r\n", ussd);
		ret = mdm.ussdCommand(ussd, buf);
		if (ret > 0) 
			printf("Ussd Got Answer: \"%s\"\r\n", buf);
	}

	printf("SMS and GPS Loop\r\n");
	char link[128] = "";
	unsigned int i = 0xFFFFFFFF;
	const int wait = 100;
	bool abort = false;

#ifdef CELLOCATE    
	const int sensorMask = 3;  // Hybrid: GNSS + CellLocate       
	const int timeoutMargin = 5; // seconds
	const int submitPeriod = 60; // 1 minutes in seconds
	const int targetAccuracy = 1; // meters
	unsigned int j = submitPeriod * 1000/wait;
	bool cellLocWait = false;
	MDMParser::CellLocData loc;

	//Token can be released from u-blox site, when you got one replace "TOKEN" below 
	if (!mdm.cellLocSrvHttp("TOKEN"))
		mdm.cellLocSrvUdp();        
	mdm.cellLocConfigSensor(1);   // Deep scan mode
	//mdm.cellUnsolIndication(1);
#endif

	while (!abort) {
		myled = !myled;
#ifndef CELLOCATE
		while ((ret = gps.getMessage(buf, sizeof(buf))) > 0)
		{
			int len = LENGTH(ret);
			if ((PROTOCOL(ret) == GPSParser::NMEA) && (len > 6))
			{
				// talker is $GA=Galileo $GB=Beidou $GL=Glonass $GN=Combined $GP=GPS
				if ((buf[0] == '$') || buf[1] == 'G') {
#define _CHECK_TALKER(s) ((buf[3] == s[0]) && (buf[4] == s[1]) && (buf[5] == s[2]))
					if (_CHECK_TALKER("GLL")) {
						double la = 0, lo = 0;
						char ch;
						if (gps.getNmeaAngle(1,buf,len,la) && 
								gps.getNmeaAngle(3,buf,len,lo) && 
								gps.getNmeaItem(6,buf,len,ch) && ch == 'A')
						{
							loopcnt++;
							printf("GPS Location: %.5f %.5f\r\n", la, lo); 
							sprintf(link, "I am here! [%ld]\n"
									"https://maps.google.com/?q=%.5f,%.5f",loopcnt, la, lo);
							printf("%s \r\n",link);
						}
					} else if (_CHECK_TALKER("GGA") || _CHECK_TALKER("GNS") ) {
						double a = 0; 
						if (gps.getNmeaItem(9,buf,len,a)) // altitude msl [m]
							printf("GPS Altitude: %.1f\r\n", a); 
					} else if (_CHECK_TALKER("VTG")) {
						double s = 0; 
						if (gps.getNmeaItem(7,buf,len,s)) // speed [km/h]
							printf("GPS Speed: %.1f\r\n", s); 
					}
				}
			}
			::wait_ms(wait);
		}
#endif 
#ifdef RTOS_H
		Thread::wait(wait);
#else
		::wait_ms(wait);
#endif
	}
	gps.powerOff();
	mdm.powerOff();
	return 0;
}
