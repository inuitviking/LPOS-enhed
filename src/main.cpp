// Official libraries
#include <Arduino.h>		// Require to easier talk with the Arduino
#include <SPI.h>			// Required by the ECCX08CSR library.
#include <Wire.h>			// Required by the ECCX08CSR library.
#include <WiFiNINA.h>		// Required to connect to Wi-Fi
#include <WiFiUdp.h>		// Required?
#include <SSLClient.h>		// https://github.com/OPEnSLab-OSU/SSLClient
#include <PubSubClient.h>	// https://pubsubclient.knolleary.net/

// Own libraries
#include "Heartbeat/Heartbeat.h"	// Library for listening to the pulse sensor
#include "Serial/LPOSSerial.h"		// Library for clearing the serial output (only works in putty)
#include "MQTT/MQTT.h"

// Macros
#include "Secrets/certificates.h"	// Where we store certificates
#include "Secrets/wifi.h"			// Where we store Wi-Fi credentials
#include "Secrets/mqtt.h"			// Where we store MQTT credentials
#include "WiFi/LPOSWiFi.h"
#include "definitions.h"

// Variables
Heartbeat heartbeat;					// Create a Heartbeat
int status  = WL_IDLE_STATUS;			// The Wi-Fi radio's status; default is idle.
// Define our certificate and key into our SSLClient
SSLClientParameters mTLS = SSLClientParameters::fromPEM(arduinoCertificate, sizeof arduinoCertificate, arduinoKey, sizeof arduinoKey);
unsigned long lastMillis = millis();	// Get the last millis.
WiFiClient wiFiClient;
SSLClient tlsClient(wiFiClient, TAs, 2, A1); // The last value is an Analog pin to draw random input from
PubSubClient mqttClient(MQTT_HOST, MQTT_PORT, MQTT::CallBack, tlsClient);
int counter = 0;
int lastBPM = 0;
int currentBPM = 0;

/**
 * Initialises the program.
 */
void setup () {
	Serial.begin(115200);					// Set the baud rate to 115200
	pinMode(LED_BUILTIN, OUTPUT);	// Set the builtin LED to output, so we can use it

	// Enable mutual TLS with SSLClient
	tlsClient.setMutualAuthParams(mTLS);

	WiFi.setHostname(HOSTNAME);

	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("Setup: WiFi shield not present");
		// don't continue:
		while (true);
	}

	// attempt to connect to Wi-Fi network:
	while ( WiFi.status() != WL_CONNECTED) {
		Serial.print("Setup: Attempting to connect to WPA SSID: ");
		Serial.println(SECRET_SSID);
		// Connect to WPA/WPA2 network:
		status = WiFi.begin(SECRET_SSID, SECRET_PASS);
	}

	LPOSWiFi::PrintWiFiStatus();
}

/**
 * Loops forever.
 */
void loop () {
	LPOSSerial::Clear();
	if (!mqttClient.connected()) {
		mqttClient = MQTT::Reconnect(mqttClient);
	}
	LPOSWiFi::PrintWiFiStatus();
	currentBPM = heartbeat.GetBPM(A1, LED_BUILTIN);
	if (currentBPM != lastBPM) {
		mqttClient.publish("hospital/", String(currentBPM).c_str());
		lastBPM = currentBPM;
	}
	delay(50);
	mqttClient.loop();
}