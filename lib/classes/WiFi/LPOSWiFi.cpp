// Includes
// - Libraries
#include "WiFiNINA.h"
// - Custom
#include "LPOSWiFi.h"
#include "Serial/LPOSSerial.h"

void LPOSWiFi::ConnectToWiFi(char *ssid, char *pass, int status) {
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
		// Connect to WPA/WPA2 network.
		status = WiFi.begin(ssid, pass);
		// Wait 2 seconds for connection:
		for (int i = 0; i < 2; i++) {
			Serial.print(".");
			delay(1000);
		}
	}
	LPOSSerial::Clear();
}

void LPOSWiFi::PrintWiFiStatus() {
	Serial.print("SSID: ");			// Print SSID (WiFi name)
	Serial.println(WiFi.SSID());
	Serial.print("IP Address: ");	// Print local IP address
	Serial.println(WiFi.localIP());
	Serial.print("MAC: ");
	Serial.println(GetMacAddress());
}

String LPOSWiFi::GetMacAddress() {
	byte bssid[6];
	WiFi.BSSID(bssid);
	String macAddr = "";
	for (int i = 5; i >= 0; i--) {
		if (bssid[i] < 16) {
			macAddr = macAddr + String('0');
		}
		macAddr = macAddr + String(bssid[i], HEX);
		if (i > 0) {
			Serial.print(":");
			macAddr = macAddr + String(':');
		}
	}
	return macAddr;
}