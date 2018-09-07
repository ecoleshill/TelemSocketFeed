#ifndef __MYSOCKET_H__
#define __MYSOCKET_H__

#include <iostream>
#include <string>
#include <queue>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

enum SocketType {CLIENT, SERVER};
enum ConnectionType {TCP, UDP};

const int DEFAULT_SIZE = 100;		//Default size of a data packet through this connection

class MySocket
{
	char *Buffer;					//Buffer, dynamically allocated based on constructor
	SOCKET Welcome;					//Welcome socket used for server applications
	SOCKET Connection;				//Connection used for both client and server applications
	struct sockaddr_in SvrAddr;		//Server address information

	//Connection related information
	SocketType mySocket;			//The type of instantiated socket being used
	std::string IPAddr;				//IP Address to use for this connection
	int Port;						//Port numnber to be used for this connection
	ConnectionType connectionType;	//The type of connection to be made (TCP or UDP)

	bool bTCPConnect;				//Flag to determine if a TCP Connection has been established or not
	int MaxSize;					//Max size of the allocated data in the socket buffer

public:
	MySocket(SocketType type, std::string ip, int port, ConnectionType ct, int BuffSize) :
		mySocket(type), IPAddr(ip), Port(port), connectionType(ct), bTCPConnect(FALSE)
	{
		Buffer = nullptr;
		if (BuffSize == 0)
		{
			std::cout << "ERROR:  BuffSize Cannot be zero - Default set to 100 bytes" << std::endl;
			Buffer = new char[DEFAULT_SIZE];
			MaxSize = DEFAULT_SIZE;
		}
		else
		{
			Buffer = new char[BuffSize];
			MaxSize = BuffSize;
			if (Buffer != nullptr)
				std::cout << "MySocket Buffer allocated to " << BuffSize << " Bytes " << std::endl;
		}
	};

	~MySocket()
	{
		if (Buffer != nullptr)
			delete[] Buffer;
	}

	bool ConnectTCP();									//Establishes a TCP/IP Connection, starts the thread
	void DisconnectTCP();								//Closes a TCP/IP Connection
	void SendData(const char*, int);					//Tx Data out socket
	int GetData(char*);									//Rx Data from socket

	
	//Get-Set functionality for the MySocket object
	std::string GetIPAddr() { return IPAddr; };
	void SetIPAddr(std::string IPToUse) { IPAddr = IPToUse; };
	int GetPort() { return Port; };
	void SetPort(int PortToUse) { Port = PortToUse; };
	SocketType GetType() { return mySocket; };
	void SetType(SocketType newType) { mySocket = newType; };
};

#endif