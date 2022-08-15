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

/**
 * Initialises the program
 */
void setup() {
	// Start Serial terminal
	Serial.begin(9600);

	// Say what we're doing next
	String wifiStartupText = "Connecting to \n";
	wifiStartupText += SECRET_SSID;
	Serial.println(wifiStartupText);

	// Connect to Wi-Fi
	LPOSWiFi::WiFiStartup((char *) SECRET_SSID, (char *) SECRET_PASS, status);
	// Say what SSID we connected to, and what IP we got.
	LPOSWiFi::PrintWiFiStatus();
}

/***
 * The code to be executed
 */
void loop() {
// write your code here
}