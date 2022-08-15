// Includes
// - Libraries
#include "WiFiNINA.h"
// - Custom
#include "LPOSWiFi.h"
#include "Serial/LPOSSerial.h"

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
[[noreturn]] void LPOSWiFi::WiFiStartup(char *ssid, char *pass, int status) {
	// Check for the Wi-Fi module
	if (WiFi.status() == WL_NO_MODULE) {
		Serial.println("Communication with WiFi module failed!");
		while (true);
	}
	// Check firmware version
	String fv = WiFiClass::firmwareVersion();
	if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
		Serial.println("Please upgrade the firmware");
	}

	// Attempt to connect to Wi-Fi network:
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pass);
		// Wait 2 seconds for connection:
		for (int i = 0; i < 2; i++) {
			Serial.print(".");
			delay(1000);
		}
		LPOSSerial::Clear();
	}
}

/**
 * This method simply prints the Wifi connection SSID and IP address.
 * @return void
 */
void LPOSWiFi::PrintWiFiStatus() {
	Serial.print("SSID: ");			// Print SSID (WiFi name)
	Serial.println(WiFi.SSID());
	Serial.print("IP Address: ");	// Print local IP address
	Serial.println(WiFi.localIP());
}
