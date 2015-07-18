#include "main.h"

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
    LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{

	int result;

    SystemClass system;
	result = system.Initialize();
	if(result) 
		system.Run();
	else 
		return system.errorcode;

	system.ShutDown();

	return 0;
}