// Includes
// - Libraries
#include <Arduino.h>
#include <WiFiNINA.h>
// - Custom classes
#include "WiFi/LPOSWiFi.h"
#include "Serial/LPOSSerial.h"
// - Custom defines
#include "Secrets/mqtt.h"
#include "Secrets/wifi.h"
#include "definitions.h"
#include "pins.h"

// Variables
// - Wifi
int keyIndex = 0;				// Your network key Index number (needed only for WEP).
int status = WL_IDLE_STATUS;	// Setting the status of the Wi-Fi to "idle" as default.
WiFiClient wiFiClient;			// Create the Wi-Fi client
// - Millis
unsigned long lastMillis = 0;

void setup() {
	// Initialisation
	Serial.begin(9600);

	String wifiStartupText = "Connecting to \n";	// Tell the user what we're doing
	wifiStartupText += SECRET_SSID;
	Serial.println(wifiStartupText);

	LPOSWiFi::WiFiStartup((char *) SECRET_SSID, (char *) SECRET_PASS, status);
	Serial.print("asdf");
	LPOSWiFi::PrintWiFiStatus();
}

void loop() {
// write your code here
}