#pragma once

//#ifdef MATHLIBRARY_EXPORTS
//#define MATHLIBRARY_API __declspec(dllexport)
//#else
//#define MATHLIBRARY_API __declspec(dllimport)
//#endif
#define MATHLIBRARY_API __declspec(dllexport)

extern "C"
{
    const int gMaxAxis = 6;

    enum ManipulatorStatus
    {
        Moving,
        Done,
        Aborted,
        Error
    };

#pragma pack(push, 1)
    struct Axis
    {
        char Name[32];
        char Units[32];
        bool Rotation;
        bool UseLimits;
        double LowerLimit;
        double UpperLimit;
    };

    struct ManipulatorInfo
    {
        int AxisCount;
        Axis Axes[gMaxAxis];
        bool HasSpeed;
    };
#pragma pack(pop)
}


extern "C" MATHLIBRARY_API int GDS_MA_Initialize(void* mainWindow);


extern "C" MATHLIBRARY_API char* GDS_MA_GetLibInfo();



extern "C" MATHLIBRARY_API int GDS_MA_GetManipulatorInfo(ManipulatorInfo * manipulatorInfo);

extern "C" MATHLIBRARY_API ManipulatorStatus GDS_MA_Status();
extern "C" MATHLIBRARY_API int GDS_MA_MoveTo(const double* position, const double* speed);
extern "C" MATHLIBRARY_API int GDS_MA_ReadPos(double* curPos, double* curSpeed);
extern "C" MATHLIBRARY_API int GDS_MA_Stop();
extern "C" MATHLIBRARY_API int GDS_MA_Finalize();