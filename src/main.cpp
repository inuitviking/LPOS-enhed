// Official libraries
#include <Arduino.h>				// Require to easier talk with the Arduino.
#include <WiFiNINA.h>				// Required to connect to Wi-Fi.
#include <SSLClient.h>				// Required to for encrypted connections.
#include <PubSubClient.h>			// Required to send over MQTT.
#include <protothreads.h>			// Required to make threading available in Arduino.

// Own libraries
#include "Heartbeat/Heartbeat.h"	// Library for listening to the pulse sensor
#include "Serial/LPOSSerial.h"		// Library for clearing the serial output (only works in putty)
#include "MQTT/MQTT.h"				// Library for general MQTT stuff.

// Macros
#include "Secrets/certificates.h"	// Where we store certificates
#include "Secrets/wifi.h"			// Where we store Wi-Fi credentials
#include "Secrets/mqtt.h"			// Where we store MQTT credentials
#include "WiFi/LPOSWiFi.h"			// Our own Wi-Fi functions
#include "definitions.h"			// Our own macros
#include "pins.h"					// Our own PIN macros

// *************
// * VARIABLES *
// *************
Heartbeat heartbeat;																					// Define a heartbeat object.
int status  = WL_IDLE_STATUS;																			// The Wi-Fi radio's status; default is idle.
SSLClientParameters mTLS = SSLClientParameters::fromPEM(arduinoCertificate,					// Define our certificate and key into our SSLClient. (Here we define the certificate).
														sizeof arduinoCertificate,			// (Here we define the size of the certificate).
														arduinoKey,							// (Here we define the certificate key).
														sizeof arduinoKey);					// (Here we define the size of the certificate key).
unsigned long lastMillis = millis();																	// Get the last millis.
WiFiClient wiFiClient;																					// Define the Wi-Fi client
SSLClient tlsClient(wiFiClient, TAs, 2, PULSE_SENSOR);	// Define the SSLClient. The last value is an Analog pin to draw random input from.
PubSubClient mqttClient(MQTT_HOST, MQTT_PORT, MQTT::CallBack, tlsClient);					// Define the PubSubClient.
int lastBPM = 0;																						// Set the last BPM.
int currentBPM = 0;																						// Set the current BPM.
int calledNurse = 0;																					// Set whether we have called a nurse or not.
unsigned long buttonPushes[2] = {0, 0};														// Store two button pushes with millis.
int buttonDown = 0;
int buttonPushed = 0;
pt ptBPMOverMQTT;																						// Define a proto-thread for sending BPM over MQTT.
pt ptCallNurse;																							// Define a proto-thread for calling a nurse.

// *************
// * FUNCTIONS *
// *************
/**
 * Send the BPM collected over MQTT over TLS on a separate thread.
 * @param pt - The proto-thread to use.
 * @return int
 */
int SendBPMoverMQTT (struct pt* pt) {
	PT_BEGIN(pt);																						// Begin proto-thread
		currentBPM = heartbeat.GetBPM(PULSE_SENSOR, LED_BUILTIN);						// Get current BPM.
		if (currentBPM != lastBPM) {																	// If current BPM doesn't match last BPM.
		// The reason we're using String here instead of char *, was to figure out string concatenation.
		// Unfortunately, this way is pretty heavy, but there should be a better way.
		// TODO: Optimize string concatenation
		String topic = R"(hospital/)";																	// Create a topic string.
		topic = topic + "bpm/" + HOSTNAME;
		// Casting with (char *) resulted in very weird errors on the infoscreen side. Instead of, for example "76" it could send "x�Cpx!Cp".
		mqttClient.publish(topic.c_str(), String(currentBPM).c_str());					// Send the current BPM to the defined topic.
		lastBPM = currentBPM;																			// Update last BPM
		}
	PT_END(pt)																							// End proto-thread
}

/**
 * Send a "1" over MQTT over TLS on a separate thread.
 * @param pt
 * @return int
 */
int CallNurse (struct pt* pt) {
	PT_BEGIN(pt);
		String topic =  R"(hospital/)";
		topic = topic + "call/" + HOSTNAME;

		if (millis() - buttonPushes[0] > 1000) {
			buttonPushes[0] = 0;
			buttonPushes[1] = 0;
			buttonPushed = 0;
		}

		if (digitalRead(BUTTON_PIN) == 0 && buttonDown == 0) {
			buttonDown = 1;
		}

		if (buttonDown == 1 && buttonPushed == 1) {
			if (calledNurse == 1) {
				calledNurse = 0;
				mqttClient.publish(topic.c_str(), "0");
			}
			buttonDown = 0;
			buttonPushed = 0;
			buttonPushes[1] = millis();
		}

		if (buttonDown == 1 && digitalRead(BUTTON_PIN) == 1) {
			buttonDown = 0;
			buttonPushed = 1;
			buttonPushes[0] = millis();
			if (calledNurse == 0) {
				calledNurse = 1;
				mqttClient.publish(topic.c_str(), "1");
			}
		}

	PT_END(pt);
}

// ***********
// * PROGRAM *
// ***********
/**
 * Initialises the program.
 */
void setup () {
	Serial.begin(115200);										// Set the baud rate to 115200
	pinMode(LED_BUILTIN, OUTPUT);						// Set the builtin LED to output, so we can use it
	pinMode(BUTTON_PIN, INPUT_PULLUP);					// Set button pin to PULLUP, as it is used for a button

	tlsClient.setMutualAuthParams(mTLS);							// Enable mutual TLS with SSLClient
	WiFi.setHostname(HOSTNAME);										// Set the Wi-Fi hostname

	if (WiFi.status() == WL_NO_SHIELD) {									// Check for the presence of the shield
		Serial.println("Setup: WiFi shield not present");
		while (true);														// Don't continue:
	}

	while ( WiFi.status() != WL_CONNECTED) {								// Attempt to connect to Wi-Fi network:
		Serial.print("Setup: Attempting to connect to WPA SSID: ");
		Serial.println(SECRET_SSID);
		status = WiFi.begin(SECRET_SSID, SECRET_PASS);	// Connect to WPA/WPA2 network:
	}

	LPOSWiFi::PrintWiFiStatus();											// Print the Wi-Fi connection status.
	PT_INIT(&ptBPMOverMQTT);												// Initialize the proto-thread for ptBPMOverMQTT
}

/**
 * Loops forever.
 */
void loop () {
	if (!mqttClient.connected()) {											// If we disconnect from the MQTT broker, attempt to reconnect.
		mqttClient = MQTT::Reconnect(mqttClient);							// The program will be stuck until we find the MQTT broker.
	}

	PT_SCHEDULE(SendBPMoverMQTT(&ptBPMOverMQTT));						// Send BPM over MQTT with the help of a separate thread.
	PT_SCHEDULE(CallNurse(&ptCallNurse));								// Call a nurse

	mqttClient.loop();														// Loop MQTT so we also can receive messages.
}