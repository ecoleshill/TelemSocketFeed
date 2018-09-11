/*
Program:  TelemSocketFeed
Author:   Dr. Elliott Coleshill
Date:     September 2018
Purpose:  The purpose of this program is to extract and transmit simulated aircraft
          telemetry over a TCP/IP connection from either a pre-recorded telemetry 
		  file or by connecting directly to SimConnect
History:
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include "SimConnect.h"
#include "MySocket.h"

enum MODE {TELEMFILE, SIMCONNECT};

struct Telemetry
{
	float Alt;				//Altitude
	float Pitch;			//Additude Indicator Pitch angle
	float Bank;				//Additude Indicator Bank angle
	float VS;				//Vertical Speed
	float TC_Rate;			//Turn Coordinator - Rate of Turn
	float TC_Yaw;			//Turn Coordinator - Yaw
	float IAS;				//Indicated Airspeed
	float Heading;			//Heading Indicator
	int Terminate;			//Termination flag -- Last packet being Tx'ed
};

//This function contains the logic to read the telemetry from the file
//packetize it and transmit it over the provided socket
void Run_TelemFileMode(std::ifstream *ifs, MySocket &sock)
{
	std::string ReadLine;								//Line read from the file
	std::vector<std::string> TelemNames;				//Names of all provided telemetry values
	Telemetry T;										//A set of telemetry to be transmitted
	memset(&T, 0, sizeof(Telemetry));

	//Need to read the first two lines to ignore:
	//--- UNC Configuration Filename
	std::getline(*ifs, ReadLine);

	//extract out and record all the telemetry points for the loaded file
	std::getline(*ifs, ReadLine);
	int Index = 1;
	while (Index > 0)
	{
		Index = ReadLine.find(",");
		TelemNames.push_back(ReadLine.substr(0, Index));
		ReadLine = ReadLine.substr(Index + 1, ReadLine.length() - Index);
	}

	//Loop until eof extracting out 6-pack telemetry (if available)
	std::string tmp;			//temp location for parsed out parameter
	while (!ifs->eof())
	{
		std::getline(*ifs, ReadLine);

		//Get rid of the timestamp first
		Index = ReadLine.find(",");
		ReadLine = ReadLine.substr(Index + 1, ReadLine.length() - Index);

		for (size_t x = 0; x < TelemNames.size(); x++)
		{
			Index = ReadLine.find(",");
			tmp = ReadLine.substr(0, Index);
			ReadLine = ReadLine.substr(Index + 1, ReadLine.length() - Index);

			if (TelemNames[x] == "PLANE ALTITUDE") {T.Alt = atof(tmp.c_str());}
			else if (TelemNames[x] == "ATTITUDE INDICATOR PITCH DEGREES") { T.Pitch = atof(tmp.c_str()) * 57.2958; }
			else if (TelemNames[x] == "ATTITUDE INDICATOR BANK DEGREES") { T.Bank = T.TC_Rate = atof(tmp.c_str()) * 57.2958; }
			else if (TelemNames[x] == "VERTICAL SPEED") { T.VS = (atof(tmp.c_str()) * 100); }
			else if (TelemNames[x] == "TC RATE") { T.Bank = atof(tmp.c_str()) * 57.2958; }	//Temporarily using the Bank angle to control this
			else if (TelemNames[x] == "TURN COORDINATOR BALL") { T.TC_Yaw = atof(tmp.c_str()); 	}	//-127 to 127 
			else if (TelemNames[x] == "AIRSPEED INDICATED") { T.IAS = atof(tmp.c_str()); }
			else if (TelemNames[x] == "HEADING INDICATOR") { T.Heading = (atof(tmp.c_str()) * 57.2958); }
		}

		//Transmit the data to the GUI displays
		sock.SendData((char*)&T, sizeof(Telemetry));
		std::cout << ".";
		Sleep(10);
	}

	Sleep(2000);
	T.Terminate = 5;
	sock.SendData((char*)&T, sizeof(Telemetry));
	Sleep(1000);			//Give time for the transmission of the last packet before terminating the socket

	sock.DisconnectTCP();
}

//This function contains the logic to read the telemetry from a SimConnect
//simulator, packetize it and transmit it over to the provided socket
void Run_SimConnectMode(MySocket &sock)
{
	_SimConnect::CSimConnect myConnection("SimConnect.cfg");
}

int main(int argc, char *argv[])
{
	MODE opMode;					//Mode of operation -- set and based on number of command line arguments
	std::ifstream *ifs = nullptr;

	//Verify the command line arguments.  If 2 then use the pre-recorded file mode
	if (argc == 2)
	{
		std::cout << argc << " arguments detected - Running in Telemetry File Mode" << std::endl;
		ifs = new std::ifstream(argv[1]);
		if ((ifs->is_open()) && (!ifs->fail()))
		{
			std::cout << argv[1] << " \nsuccessfully opened" << std::endl;
			opMode = TELEMFILE;
		}
		else
		{
			std::cerr << "ERROR:  Telemetry file entered, but failed to open - Terminating program" << std::endl;
			return -1;
		}
	}
	else
		std::cout << argc << " arguments entered - Running in SimConnect Mode with SimConnect.cfg" << std::endl;

	//Startup the updated Avionics Instrument Control Demo program and let the replayed telemetry drive
	//the 6-pack
	
	STARTUPINFO startInfo = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };

	BOOL bSuccess = CreateProcess(TEXT("C:\\Users\\elliott.coleshill\\Documents\\AA_RESEARCH\\AvionicsInstrumentControlDemo\\bin\\Debug\\AvionicsInstrumentControlDemo.exe"), NULL, NULL, NULL, FALSE, NULL, NULL,
		NULL, &startInfo, &processInfo);
	
	//Create the TCP/IP socket and start the connection with the remote server
	MySocket sock(CLIENT, "127.0.0.1", 11000, TCP, 36);
	std::cout << "Waiting for connection" << std::endl;
	
	//Loop until a connection is made.  Not point continuing unless a connection is made
	while (!sock.ConnectTCP()) {}
	std::cout << std::endl << "Connection Established" << std::endl;

	if (opMode == TELEMFILE)
	{
		Run_TelemFileMode(ifs, sock);
		ifs->close();
	}
	else if (opMode == SIMCONNECT)
	{
		Run_SimConnectMode(sock);
	}
	else
		std::cerr << "ERROR:  Oops!  Invalid Operational Mode - Terminating" << std::endl;

	sock.DisconnectTCP();		//cleanly disconnect the TCP/IP socket connections
	return 1;
}