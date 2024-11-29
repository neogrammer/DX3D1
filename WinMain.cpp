#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	SystemClass* System;
	bool result;

	// create the system object
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	result = System->Initialize();
	if (result)
		{
		System->Run();
		}

	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}
