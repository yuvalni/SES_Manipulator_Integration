// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "HW.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5000"
static WSADATA wsaData;
static int iResult;
static SOCKET ConnectSocket = INVALID_SOCKET;
static char recvbuf[DEFAULT_BUFLEN];
static int recvbuflen = DEFAULT_BUFLEN;


int initSocket() {
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	//SOCKET ConnectSocket = INVALID_SOCKET;
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}


	return 0;
}

int send_data(const char* sendbuf)
{
	int Result;
	Result = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (Result == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

double recv_data() {
	int Result;
	Result = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (Result > 0) {
		printf("Bytes received: %d\n", Result);
	}
	else if (Result == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());

	return atof(recvbuf);
}

int GDS_MA_Initialize(void* mainWindow) //     Use this function to initialize objects and structures in a manipulator
										//hardware library.This function is called when a new manipulator hardware
										//library is loaded in the manipulator library.
{
	return initSocket();
    return 0;
}

int GDS_MA_Finalize() {
	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	int Result;
	const char* sendbuf = "exit";
	send_data(sendbuf);
	Result = shutdown(ConnectSocket, SD_SEND);
	if (Result == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes the connection



	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}

char* GDS_MA_GetLibInfo() {
    char* ch = new char[40] /* 11 = len of Hello Heap + 1 char for \0*/;
    strcpy_s(ch, 40, "Technion Manipulatr Library, By Nitzav.");
    
    return ch;
 
}

int GDS_MA_GetManipulatorInfo(ManipulatorInfo* manipulatorInfo) {
	Axis ax1;
	strcpy_s(ax1.Name, 2, "P");
	strcpy_s(ax1.Units, 4, "Deg");
	ax1.Rotation = true;
	ax1.UseLimits = true;
	ax1.UpperLimit = 10;
	ax1.LowerLimit = -10;

	Axis ax2;
	strcpy_s(ax2.Name, 2, "Tilt");
	strcpy_s(ax2.Units, 4, "Deg");
	ax2.Rotation = true;
	ax2.UseLimits = true;
	ax2.UpperLimit = 10;
	ax2.LowerLimit = -10;
	manipulatorInfo->AxisCount = 2;
	manipulatorInfo->Axes[0] = ax1;
	manipulatorInfo->Axes[1] = ax2;
	
	manipulatorInfo->HasSpeed = false;
	return 0;
    
}



ManipulatorStatus GDS_MA_Status() // Here we handle the status.. moving, done movement ETC...
{
	return Done;
}

int GDS_MA_MoveTo(const double* position, const double* speed) // This sends the motors to a location. This does not handle anything else
{
	return 0;
}


int GDS_MA_ReadPos(double* curPos, double* curSpeed)
{
	for (int i = 0; i < 1; ++i)
	{
		const char* sendbuf = "X?";
		send_data(sendbuf);
		curPos[0] = recv_data();
		const char* sendbuf = "Y?";
		send_data(sendbuf);
		curPos[1] = recv_data()
	}

	return 0;
}


int GDS_MA_Stop() {
	return 0;
}