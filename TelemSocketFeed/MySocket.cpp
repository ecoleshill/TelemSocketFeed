#include "MySocket.h"

bool MySocket::ConnectTCP()
{
	if (bTCPConnect)
	{
		std::cout << "ERROR:  Connection already established" << std::endl;
		return false;
	}

	if (mySocket == CLIENT)
	{
		//starts Winsock DLLs
		WSADATA wsaData;
		if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
			std::cout << "ERROR:  WinSock failed to start" << std::endl;
			return false;
		}

		//initializes socket. SOCK_STREAM: TCP
		Connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (Connection == INVALID_SOCKET) {
			std::cout << "ERROR:  Failed to create socket" << std::endl;
			WSACleanup();
			return false;
		}

		//Connect socket to specified server
		SvrAddr.sin_family = AF_INET;							//Address family type itnernet
		SvrAddr.sin_port = htons(Port);							//port (host to network conversion)
		SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());	//IP address    //Console Version
		//inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr.s_addr);			//MFC Version
		if ((connect(Connection, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			std::cout << "ERROR:  Failed to connect" << std::endl;
			closesocket(Connection);
			WSACleanup();
			return false;
		}

		std::cout << "Connection Established" << std::endl;
	}
	else if (mySocket == SERVER)
	{
		//starts Winsock DLLs
		WSADATA wsaData;
		if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
			std::cout << "ERROR:  WinSock failed to load" << std::endl;
			return false;
		}

		//create welcoming socket at port and bind local address
		if ((Welcome = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			std::cout << "ERROR:  Failed to create WelcomeSocket" << std::endl;
			return false;
		}
		SvrAddr.sin_family = AF_INET;							//set family to internet
		SvrAddr.sin_addr.s_addr = INADDR_ANY;					//set the local IP address
		SvrAddr.sin_port = htons(Port);						//set the port number

		if ((bind(Welcome, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR)
		{
			std::cout << "ERROR:  Failed to Bind the socket" << std::endl;
			closesocket(Welcome);
			WSACleanup();
			return false;
		}

		//Specify the maximum number of clients that can be queued
		if (listen(Welcome, 1) == SOCKET_ERROR)
		{
			closesocket(Welcome);
			std::cout << "ERROR:  Failed to create Listening Port" << std::endl;
			return false;
		}

		//Main server loop - accept and handle requests from clients
		std::cout << "Waiting for client connection" << std::endl;
		Connection = SOCKET_ERROR;

		//Wait for an incoming connection from the Robot
		if ((Connection = accept(Welcome, NULL, NULL)) == SOCKET_ERROR)
		{
			std::cout << "ERROR:  Something went wrong - Connection Socket was rejected" << std::endl;
			return false;
		}
		else
		{
			std::cout << "Connection Established" << std::endl;
		}
	}
	else
		std::cout << "ERROR:  ConnectTCP() - Invalid socket type" << std::endl;

	bTCPConnect = TRUE;
	return true;
}

void MySocket::DisconnectTCP()
{
	//Only disconnect is a connection flag is set
	if (bTCPConnect)
	{
		//If we are configured as a server, close the welcome socket as well
		if (mySocket == SERVER)
			closesocket(Welcome);

		closesocket(Connection);

		bTCPConnect = FALSE;

		std::cout << "ERROR:  Socket Closed" << std::endl;
	}

	//frees Winsock DLL resources
	WSACleanup();
}

void MySocket::SendData(const char* Data, int Size)
{
	int TxSize = 0;

	switch (connectionType)
	{
	case TCP:
		//Only Tx if a connection has been established
		if (bTCPConnect)
		{
			//Tx Data
			TxSize = send(Connection, (char *)Data, Size, 0);
			if (TxSize < 0)
				std::cout << "Tx Failure" << std::endl;
		}
		else
			std::cout << "ERROR:  Failure in Tx.  No TCP connection established" << std::endl;

		break;

	case UDP:

		break;

	default:
		std::cout << "ERROR:  Failure Sending Data.  Unknown connectionType in MySocket" << std::endl;
	}
}

int MySocket::GetData(char* Data)
{
	int RxSize = 0;
	switch (connectionType)
	{
	case TCP:
		//Only Rx data if a connection is established
		if (bTCPConnect)
		{
			//Wait for Data
			RxSize = recv(Connection, (char *)Buffer, MaxSize, 0);
			if (RxSize > 0)
				memcpy(Data, Buffer, RxSize);
		}
		else
			std::cout << "ERROR:  Failure in Rx - No TCP connection established" << std::endl;

		break;

	case UDP:
		break;

	default:
		std::cout << "ERROR:  Failure Receiving Data.  Unknown connectionType in MySocket" << std::endl;
	}

	return RxSize;		//Should return 0 by default if UDP or TCP cases are not executed
}