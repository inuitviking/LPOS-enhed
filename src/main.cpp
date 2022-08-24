// Official libraries
#include <Arduino.h>		// Require to easier talk with the Arduino.
#include <WiFiNINA.h>		// Required to connect to Wi-Fi.
#include <SSLClient.h>		// Required to for encrypted connections.
#include <PubSubClient.h>	// Required to send over MQTT.
#include <protothreads.h>	// Required to make threading available in Arduino.

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
WiFiClient wiFiClient;					// Define the wifi client
SSLClient tlsClient(wiFiClient, TAs, 2, A1);	// Define the SSLClient. The last value is an Analog pin to draw random input from.
PubSubClient mqttClient(MQTT_HOST, MQTT_PORT, MQTT::CallBack, tlsClient);		// Define the PubSubClient
int lastBPM = 0;		// Set the last BPM
int currentBPM = 0;		// Set the current BPM
pt ptBPMOverMQTT;		// Define a protothread to be used.

// *************
// * FUNCTIONS *
// *************
/**
 * Send the BPM collected over MQTT over TLS on a separate thread.
 * @param pt - The protothread to use.
 * @return int
 */
int SendBPMoverMQTT (struct pt* pt) {
	PT_BEGIN(pt);
		currentBPM = heartbeat.GetBPM(A1, LED_BUILTIN);
		if (currentBPM != lastBPM) {
		// The reason we're using String here instead of char *, was to figure out string concatenation.
		// Unfortunately, this way is pretty heavy, but there should be a better way.
		// TODO: Optimize string concatenation
		String topic = String("hospital/");
		topic = topic + HOSTNAME + "/bpm/";
		// Casting with (char *) resulted in very weird errors on the infoscreen side. Instead of, for example "76" it could send "x�Cpx!Cp"
		mqttClient.publish(topic.c_str(), String(currentBPM).c_str());
		lastBPM = currentBPM;
		}
	PT_END(pt);
}

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

	PT_INIT(&ptBPMOverMQTT);
}

/**
 * Loops forever.
 */
void loop () {
	/*
	 * If we're disconnected from the MQTT broker, attempt reconnection.
	 * Will loop until connected to MQTT broker.
	 */
	if (!mqttClient.connected()) {
		mqttClient = MQTT::Reconnect(mqttClient);
	}

	// Send BPM over MQTT with the help of a separate thread.
	PT_SCHEDULE(SendBPMoverMQTT(&ptBPMOverMQTT));

	// Loop MQTT so we also can receive messages.
	mqttClient.loop();
}