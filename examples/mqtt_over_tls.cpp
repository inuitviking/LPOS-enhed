#include <SPI.h>                  // built-in WiFi shield uses SPI
#include <WiFiNINA.h>              // WiFi support       https://www.arduino.cc/en/Reference/WiFi101
#include <WiFiUdp.h>
#include <Wire.h>

#include <SSLClient.h>            // https://github.com/OPEnSLab-OSU/SSLClient
#include <PubSubClient.h>         // https://pubsubclient.knolleary.net/

#include "Secrets/certificates.h"         //
#include "Secrets/wifi.h"         //
#include "Secrets/mqtt.h"

char ssid[] = SECRET_SSID;        // network SSID
char pass[] = SECRET_PASS;        // network password
int status  = WL_IDLE_STATUS;     // the WiFi radio's status

SSLClientParameters mTLS = SSLClientParameters::fromPEM(my_cert, sizeof my_cert, my_key, sizeof my_key);

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();
}

void printMacAddress(byte mac[]) {
	for (int i = 5; i >= 0; i--) {
		if (mac[i] < 16) {
			Serial.print("0");
		}
		Serial.print(mac[i], HEX);
		if (i > 0) {
			Serial.print(":");
		}
	}
	Serial.println();
}

void printCurrentNet() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print the MAC address of the router you're attached to:
	byte bssid[6];
	WiFi.BSSID(bssid);
	Serial.print("BSSID: ");
	printMacAddress(bssid);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI): ");
	Serial.println(rssi);

	// print the encryption type:
	byte encryption = WiFi.encryptionType();
	Serial.print("Encryption Type: ");
	Serial.println(encryption, HEX);
	Serial.println();
}

WiFiClient wifi;
SSLClient  tls(wifi, TAs, 2, A5);         // the last value is an Analog pin to draw random input from

PubSubClient mqttClient(MQTT_HOST, MQTT_PORT, callback, tls);

int counter = 0;

void reconnect() {
	// Loop until we're reconnected
	while (!mqttClient.connected()) {
		Serial.print("Reconnect() attempting MQTT connection...");
		// Attempt to connect
		if (mqttClient.connect("mkr1010", MQTT_ACCT, MQTT_PASS)) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			mqttClient.publish("/hospital/", "hello world");
			// This is a workaround to address https://github.com/OPEnSLab-OSU/SSLClient/issues/9
//			tls.flush();
			// ... and resubscribe
			mqttClient.subscribe("/hospital/");
			// This is a workaround to address https://github.com/OPEnSLab-OSU/SSLClient/issues/9
//			tls.flush();
		} else {
			Serial.print("failed, rc=");
			Serial.print(mqttClient.state());
			Serial.println(" try again in 5 seconds");
		}
	}
}

void setup() {
	// Start Serial
	Serial.begin(115200);
	while (!Serial);

	// Enable mutual TLS with SSLClient
	tls.setMutualAuthParams(mTLS);

	WiFi.setHostname("mkr1010");

	// check for the presence of the shield:
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("Setup: WiFi shield not present");
		// don't continue:
		while (true);
	}

	// attempt to connect to WiFi network:
	while ( WiFi.status() != WL_CONNECTED) {
		Serial.print("Setup: Attempting to connect to WPA SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network:
		status = WiFi.begin(ssid, pass);
	}

	printCurrentNet();

	Serial.println("Setup is done");
}

void loop() {
	if (!mqttClient.connected()) {
		reconnect();
	}
	mqttClient.publish("/hospital/", "hello world");
	delay(500);
	mqttClient.loop();
}