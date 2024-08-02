// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "HW.h"
#include <string>
#include <cstring>


#pragma comment(lib, "Ws2_32.lib")


HANDLE hPipe;

DWORD dwWritten;
DWORD dwRead;
char buffer[1024];

int initSocket() {
	// Open the named pipe
	hPipe = CreateFile(TEXT("\\\\.\\pipe\\manipulatorPipe"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		return 1;
	}
	return 0;
}

int send_data(const char* sendbuf) {
	BOOL success = WriteFile(
		hPipe,          // Handle to the pipe
		sendbuf,        // Buffer to write
		strlen(sendbuf), // Number of bytes to write
		&dwWritten,     // Number of bytes that were written
		NULL            // Not using overlapped I/O
	);

	if (!success || dwWritten != strlen(sendbuf)) {
		return -1; // Indicate failure
	}

	return 0; // Indicate success
}

double recv_data() {
	ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
	buffer[dwRead] = '\0';
	return atof(buffer);
}

ManipulatorStatus recv_status_data() {
	ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL);
	buffer[dwRead] = '\0';
	return static_cast<ManipulatorStatus>(atoi(buffer));
}

int GDS_MA_Initialize(void* mainWindow) //     Use this function to initialize objects and structures in a manipulator
										//hardware library.This function is called when a new manipulator hardware
										//library is loaded in the manipulator library.
{
	return initSocket();
}

int GDS_MA_Finalize() {
	CloseHandle(hPipe);
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



ManipulatorStatus GDS_MA_Status() // Here we handle the status.. moving, done movement ETC...
{	
	const char* move_buf = "STATUS?\n";
	send_data(move_buf);
	return recv_status_data();
}

int GDS_MA_MoveTo(const double* position, const double* speed) // This sends the motors to a location. This does not handle anything else
{
	std::string str = "MOVR";
	str += std::to_string(position[0]) + "\n";
	//str += "T" + std::to_string(position[1]); //We only want to change polar!
	const char* c = str.c_str();

	send_data(c);
	return 0;
}


int GDS_MA_ReadPos(double* curPos, double* curSpeed)
{
		
		const char* sendbuf = "R?\n";
		send_data(sendbuf);
		curPos[0] = recv_data();

		sendbuf = "T?\n";
		send_data(sendbuf);
		curPos[1] = recv_data();

		sendbuf = "P?\n";
		send_data(sendbuf);
		curPos[2] = recv_data();

		sendbuf = "X?\n";
		send_data(sendbuf);
		curPos[3] = recv_data();

		sendbuf = "Y?\n";
		send_data(sendbuf);
		curPos[4] = recv_data();

		sendbuf = "Z?\n";
		send_data(sendbuf);
		curPos[5] = recv_data();

	return 0;
}


int GDS_MA_Stop() {
	const char* sendbuf = "STOP\n";
	send_data(sendbuf);
	return 0;
}