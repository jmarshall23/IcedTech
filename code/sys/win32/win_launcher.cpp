// sys_launcher.cpp
//

#include <windows.h>
#include <cstdio>

int (*WINAPI DoomMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HMODULE handle = LoadLibraryA("DoomDLL.dll");

	printf("Launcher Starting...\n");
	if (handle == NULL){
		int error = GetLastError();
		printf("Couldn't open DoomDLL!\n");
		return error;
	}
	
	DoomMain = (int(__cdecl *)(HINSTANCE, HINSTANCE, LPSTR, int))GetProcAddress(handle, "DoomMain");
	if (DoomMain == NULL){
		printf("Couldn't find symbol DoomMain in DoomDLL!\n");
		return 2;
	}

	return DoomMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}