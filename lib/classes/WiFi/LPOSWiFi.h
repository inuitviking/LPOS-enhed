#ifndef LPOS_ENHED_LPOSWIFI_H
#define LPOS_ENHED_LPOSWIFI_H

class LPOSWiFi {
	[[noreturn]] static void WiFiStartup(char *ssid, char *pass, int status);
	static void PrintWiFiStatus();
};

#endif //LPOS_ENHED_LPOSWIFI_H