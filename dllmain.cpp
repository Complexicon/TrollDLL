#include <Windows.h>

#define EXPORT extern "C" __declspec(dllexport)

int WINAPI thread(HMODULE hmodule) {

	while (!GetAsyncKeyState(VK_END))
		Sleep(10);

	FreeLibraryAndExitThread(hmodule, 0);
	CloseHandle(hmodule);

	return 1;
}

int WINAPI DllMain(HMODULE hModule, DWORD reason, void*) {
	DisableThreadLibraryCalls(hModule);
	if (reason == 1)
		CreateThread(0, 0, (PTHREAD_START_ROUTINE)thread, hModule, 0, 0);
	return 1;
}