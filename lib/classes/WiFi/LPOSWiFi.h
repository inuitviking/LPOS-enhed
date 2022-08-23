#ifndef LPOS_ENHED_LPOSWIFI_H
#define LPOS_ENHED_LPOSWIFI_H

class LPOSWiFi {

public:
	static void ConnectToWiFi(char *ssid, char *pass, int status);
	static void PrintWiFiStatus();
	static String GetMacAddress();
};

#endif //LPOS_ENHED_LPOSWIFI_H