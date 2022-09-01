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
Heartbeat heartbeat;																		// Define a heartbeat object.
int status  = WL_IDLE_STATUS;																// The Wi-Fi radio's status; default is idle.
SSLClientParameters mTLS = SSLClientParameters::fromPEM(arduinoCertificate,					// Define our certificate and key into our SSLClient. (Here we define the certificate).
														sizeof arduinoCertificate,			// (Here we define the size of the certificate).
														arduinoKey,							// (Here we define the certificate key).
														sizeof arduinoKey);					// (Here we define the size of the certificate key).
unsigned long lastMillis = millis();														// Get the last millis.
WiFiClient wiFiClient;																		// Define the Wi-Fi client
SSLClient tlsClient(wiFiClient, TAs, 2, PULSE_SENSOR);										// Define the SSLClient. The last value is an Analog pin to draw random input from.
PubSubClient mqttClient(MQTT_HOST, MQTT_PORT, MQTT::CallBack, tlsClient);					// Define the PubSubClient.
int lastBPM = 0;																			// Set the last BPM.
int currentBPM = 0;																			// Set the current BPM.
bool buttonDown = false;																	// We want to check if the button is down.
bool buttonUp = true;																		// We want to check if the button is up.
bool buttonPushed = false;																	// We want to check if the button has gone through a down+up.
unsigned int buttonPushes = 0;																// We want to count button pushes.
unsigned int buttonDownCount = 0;															// We want to count amount of button downs.
unsigned int buttonUpCount = 0;																// We want to count amount of button ups.
unsigned int buttonMillis = 0;																// This is used to reset the button variables.
bool nurseCalled = false;																	// Nurses are not called by default.
int speed = 40;																				// Delay adjustment.
pt ptBPMOverMQTT;																			// Define a proto-thread for sending BPM over MQTT.
pt ptCallNurse;																				// Define a proto-thread for calling a nurse.

// *************
// * FUNCTIONS *
// *************
/**
 * Play a positive jingle
 */
void jingle () {
	tone(BUZZER, 300);
	delay(speed);
	noTone(BUZZER);
	tone(BUZZER, 600);
	delay(speed);
	noTone(BUZZER);
	tone(BUZZER, 1000);
	delay(speed);
	noTone(BUZZER);
}

/**
 * Play a cancellation jingle
 */
void reverseJingle () {
	tone(BUZZER, 1000);
	delay(speed);
	noTone(BUZZER);
	tone(BUZZER, 600);
	delay(speed);
	noTone(BUZZER);
	tone(BUZZER, 300);
	delay(speed);
	noTone(BUZZER);
}

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
 * Send a 1 or a 0 according to if the user wants to call or cancel a call to a nurse.
 * @param pt
 * @return int
 */
int CallNurse (struct pt* pt) {
	PT_BEGIN(pt);
		String topic =  R"(hospital/)";
		topic = topic + "call/" + HOSTNAME;

		// Reset the button variables
		if (millis() - buttonMillis > 1000) {
			buttonUpCount = 0;
			buttonDownCount = 0;
			buttonPushes = 0;
			buttonMillis = millis();
		}

		// Register a down
		if (digitalRead(BUTTON_PIN) == 0 && !buttonDown) {
			buttonDown = true;
			buttonPushed = true;
			buttonUp = false;
			buttonDownCount++;
			buttonMillis = millis();
		}

		// Register an up
		if (digitalRead(BUTTON_PIN) == 1 && !buttonUp) {
			buttonDown = false;
			buttonUp = true;
			// We don't make buttonPushed false just yet, we need to check its boolean value later.
			buttonUpCount++;
			buttonMillis = millis();
		}

		// Increment the amount the button has been pushed.
		if (buttonDownCount != 0 && buttonDownCount == buttonUpCount && buttonPushed) {
			buttonPushes++;
			// Now we can false the variable, as we only want to increment once the button has completed a down+up.
			buttonPushed = false;
			buttonMillis = millis();
		}

		// When button is pushed once, a nurse is called.
		if (buttonPushes == 1 && !nurseCalled) {
			nurseCalled = true;
			mqttClient.publish(topic.c_str(), "1");
			jingle();
		}

		// When button is pushed more than once, the nurse call is cancelled and the button variables are reset.
		if (buttonPushes > 1 && nurseCalled) {
			nurseCalled = false;
			mqttClient.publish(topic.c_str(), "0");
			reverseJingle();
		}

		if (nurseCalled) {
			digitalWrite(LED_PIN, HIGH);
		} else {
			digitalWrite(LED_PIN, LOW);
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
	PT_INIT(&ptCallNurse);													// Initialize the proto-thread for ptCallNurse
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