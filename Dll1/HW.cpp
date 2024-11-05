// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "HW.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>


static bool g_isConnected = false;
static HANDLE g_socketMutex = NULL;
static const int SOCKET_TIMEOUT_MS = 5000; // 5 second timeout

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5011"
static WSADATA wsaData;
static int iResult;
static SOCKET ConnectSocket = INVALID_SOCKET;
static char recvbuf[DEFAULT_BUFLEN];
static int recvbuflen = DEFAULT_BUFLEN;


int initSocket() {
    // Create mutex for thread-safe socket operations
    g_socketMutex = CreateMutex(NULL, FALSE, NULL);
    if (g_socketMutex == NULL) {
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo* result = NULL, *ptr = NULL, hints;
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

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Set socket timeout
    DWORD timeout = SOCKET_TIMEOUT_MS;
    setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    g_isConnected = true;
    return 0;
}

int send_data(const char* sendbuf) {
    if (!g_isConnected) return 1;

    WaitForSingleObject(g_socketMutex, INFINITE);
    int Result = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    ReleaseMutex(g_socketMutex);

    if (Result == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        g_isConnected = false;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

double recv_data() {
    if (!g_isConnected) return 0.0;

    WaitForSingleObject(g_socketMutex, INFINITE);
    int Result = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    ReleaseMutex(g_socketMutex);

    if (Result > 0) {
        printf("Bytes received: %d\n", Result);
        return atof(recvbuf);
    }
    else {
        g_isConnected = false;
        return 0.0;
    }
}

ManipulatorStatus recv_status_data() {
    if (!g_isConnected) return Done;

    WaitForSingleObject(g_socketMutex, INFINITE);
    int Result = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    ReleaseMutex(g_socketMutex);

    if (Result > 0) {
        return static_cast<ManipulatorStatus>(atoi(recvbuf));
    }
    else {
        g_isConnected = false;
        return Done;
    }
}

int GDS_MA_Initialize(void* mainWindow) //     Use this function to initialize objects and structures in a manipulator
										//hardware library.This function is called when a new manipulator hardware
										//library is loaded in the manipulator library.
{
	return initSocket();
    return 0;
}

int GDS_MA_Finalize() {
    if (g_socketMutex != NULL) {
        CloseHandle(g_socketMutex);
        g_socketMutex = NULL;
    }

    if (g_isConnected) {
        const char* sendbuf = "exit\n";
        send_data(sendbuf);
        shutdown(ConnectSocket, SD_SEND);
        closesocket(ConnectSocket);
        WSACleanup();
        g_isConnected = false;
    }
    return 0;
}

char* GDS_MA_GetLibInfo() {
    char* ch = new char[40] /* 11 = len of Hello Heap + 1 char for \0*/;
    strcpy_s(ch, 40, "Technion Manipulatr Library, By Nitzav.");
    
    return ch;
 
}

int GDS_MA_GetManipulatorInfo(ManipulatorInfo* manipulatorInfo) {
	Axis ax1;
	strcpy_s(ax1.Name, 6 ,"Polar");
	strcpy_s(ax1.Units, 4, "Deg");
	ax1.Rotation = true;
	ax1.UseLimits = true;
	ax1.UpperLimit = 20;
	ax1.LowerLimit = -20;

	Axis ax2;
	strcpy_s(ax2.Name, 5, "Tilt");
	strcpy_s(ax2.Units, 4, "Deg");
	ax2.Rotation = true;
	ax2.UseLimits = false;
	ax2.UpperLimit = 1000;
	ax2.LowerLimit = -1000;

	Axis ax3;
	strcpy_s(ax3.Name, 4, "phi");
	strcpy_s(ax3.Units, 4, "Deg");
	ax3.Rotation = true;
	ax3.UseLimits = false;
	ax3.UpperLimit = -1000;
	ax3.LowerLimit = 1000;

	Axis ax4;
	strcpy_s(ax4.Name, 2, "X");
	strcpy_s(ax4.Units, 3, "mm");
	ax4.Rotation = false;
	ax4.UseLimits = false;
	ax4.UpperLimit = 10;
	ax4.LowerLimit = -10;

	Axis ax5;
	strcpy_s(ax5.Name, 2 ,"Y");
	strcpy_s(ax5.Units, 3, "mm");
	ax5.Rotation = false;
	ax5.UseLimits = false;
	ax5.UpperLimit = 10;
	ax5.LowerLimit = -10;

	Axis ax6;
	strcpy_s(ax6.Name, 2, "Z");
	strcpy_s(ax6.Units, 3, "mm");
	ax6.Rotation = false;
	ax6.UseLimits = false;
	ax6.UpperLimit = 0;
	ax6.LowerLimit = -140;

	manipulatorInfo->AxisCount = 6;
	manipulatorInfo->Axes[0] = ax1;
	manipulatorInfo->Axes[1] = ax2;
	manipulatorInfo->Axes[2] = ax3;
	manipulatorInfo->Axes[3] = ax4;
	manipulatorInfo->Axes[4] = ax5;
	manipulatorInfo->Axes[5] = ax6;
	
	manipulatorInfo->HasSpeed = false;
	return 0;
    
}




ManipulatorStatus GDS_MA_Status() {
    if (!g_isConnected) return Done;

    const char* move_buf = "STATUS?\n";
    if (send_data(move_buf) != 0) {
        return Done;
    }
    return recv_status_data();
}

int GDS_MA_MoveTo(const double* position, const double* speed) // This sends the motors to a location. This does not handle anything else
{
	std::string strr = "MOVR";
	strr += std::to_string(position[0]);
	//str += "T" + std::to_string(position[1]); //We only want to change polar!
   // std::string strx = "MOVX";
   // strx += std::to_string(position[3]);
   // std::string stry = "MOVY";
   // stry += std::to_string(position[4]);
   // std::string strz = "MOVZ";
   // strz += std::to_string(position[5]);
    std::string str;
    str = strr + "\n"; //+  strx + "\n" +  stry + "\n" +  strz + "\n";
	const char* c = str.c_str();

	send_data(c);
	return 0;
}


int GDS_MA_ReadPos(double* curPos, double* curSpeed) {
    if (!g_isConnected) {
        // Fill with zeros if disconnected
        for (int i = 0; i < 6; i++) {
            curPos[i] = 0.0;
        }
        return 1;
    }

    const char* queries[] = {"R?\n", "T?\n", "P?\n", "X?\n", "Y?\n", "Z?\n"};
    
    for (int i = 0; i < 6; i++) {
        if (send_data(queries[i]) != 0) {
            g_isConnected = false;
            // Fill remaining positions with zeros
            for (; i < 6; i++) {
                curPos[i] = 0.0;
            }
            return 1;
        }
        curPos[i] = recv_data();
        if (!g_isConnected) {
            // Fill remaining positions with zeros
            for (i++; i < 6; i++) {
                curPos[i] = 0.0;
            }
            return 1;
        }
    }

    return 0;
}


int GDS_MA_Stop() {
	const char* sendbuf = "STOP\n";
	send_data(sendbuf);
	return 0;
}