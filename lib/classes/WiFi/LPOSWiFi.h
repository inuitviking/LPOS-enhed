#ifndef LPOS_ENHED_LPOSWIFI_H
#define LPOS_ENHED_LPOSWIFI_H

class LPOSWiFi {

public:
	/**
	 * Connects to an SSID.
	 * Does not return anything.
	 *
	 * In detail and order, the method does the following:
	 * 1. Checks if the Wifi module is installed
	 * 2. Checks if the firmware version is up to date
	 * 3. Connects to the specified SSID
	 *   a. If an attempt fails, it will reattempt a connection every 2 seconds
	 *   b. It will do this until it connects to the defined SSID
	 *
	 * @param ssid
	 * @param pass
	 * @param status
	 * @return void
	 */
	static void ConnectToWiFi(char *ssid, char *pass, int status);

	/**
	 * This method simply prints the Wifi connection SSID, IP and MAC address.
	 * @return void
	 */
	static void PrintWiFiStatus();

	/**
	 * Fetches the MAC address based off of a byte array.
	 * @param mac - byte array
	 * @return String
	 */
	static String GetMacAddress();
};

#endif //LPOS_ENHED_LPOSWIFI_H