#include <iostream>
#include <Windows.h>

#define NUM_OF_TOOLS 5
struct TrackingState {
	double Trs[3]; // X, Y, Z
	double Quat[4]; // W, X, Y, Z

	bool bTracked; // true : Tracked, false : Untracked
};

struct TrackingMessage {
	TrackingState TrackingStates[NUM_OF_TOOLS];
	bool bLock = false;
};

HANDLE MemoryMapper = NULL;
HANDLE MemoryMapPtr = NULL;
TrackingMessage* Message = nullptr;

bool OpenSharedMemory()
{
	MemoryMapper = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"TrackingStates");

	if (!MemoryMapper)
	{
		std::cout << "Failed CreateFileMapping" << std::endl;
		return false;
	}

	MemoryMapPtr = (BYTE*)MapViewOfFile(
		MemoryMapper, FILE_MAP_ALL_ACCESS,
		0, 0, 0
	);

	if (!MemoryMapPtr)
	{
		CloseHandle(MemoryMapper);
		std::cout << "Failed MapViewOfFile" << std::endl;
		return false;
	}

	Message = (TrackingMessage*) MemoryMapPtr;
	return true;
}

void CloseSharedMemory()
{
	if (MemoryMapPtr)
	{
		UnmapViewOfFile(MemoryMapPtr);
	}

	if (MemoryMapper)
	{
		CloseHandle(MemoryMapper);
	}
}

int main(int argc, char** argv)
{
	std::cout << "MemoryReader" << std::endl;
	bool bMemOpened = OpenSharedMemory();

	while (bMemOpened)
	{
#if false
		char Key;
		std::cin >> Key;

		if (Key == 'q')
		{
			break;	
		}
		else if (Key == 's')
		{
			for (int Index = 0; Index < NUM_OF_TOOLS; Index++)
			{
				std::cout << TrackingStateList[Index].Trs[0] << ", " << TrackingStateList[Index].Trs[1] << ", " << TrackingStateList[Index].Trs[2] << std::endl;
			}
		}
#endif
		if (Message->bLock == false)
		{
			Message->bLock = true;
		}
		else
		{
			std::cout << "Locked" << std::endl;
			continue;
		}

		std::cout << "TrackingStates" << std::endl;
		for (int Index = 0; Index < NUM_OF_TOOLS; Index++)
		{
			std::cout << "[" << Index << "]" << "(x, y, z) : ";
			std::cout << Message->TrackingStates[Index].Trs[0] << ", " << Message->TrackingStates[Index].Trs[1] << ", " << Message->TrackingStates[Index].Trs[2] << std::endl;
		}
		Message->bLock = false;

		Sleep(90);
	}

	CloseSharedMemory();

	return 0;
}