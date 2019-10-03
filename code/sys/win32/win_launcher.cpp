// sys_launcher.cpp
//

#include <windows.h>

int (*WINAPI DoomMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HMODULE handle = LoadLibrary("DoomDLL.dll");
	if (handle == NULL)
		return 0;
	
	DoomMain = (int(__cdecl *)(HINSTANCE, HINSTANCE, LPSTR, int))GetProcAddress(handle, "DoomMain");
	if (DoomMain == NULL)
		return 0;

	return DoomMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}