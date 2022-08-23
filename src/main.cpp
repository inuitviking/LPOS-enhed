// Official libraries
#include <Arduino.h>	// Require to easier talk with the Arduino
#include <SPI.h>		// Required by the ECCX08CSR library.
#include <Wire.h>		// Required by the ECCX08CSR library.

// Own libraries
#include "Heartbeat.h"	// Library for listening to the pulse sensor
#include "Serial/LPOSSerial.h"

// Variables
Heartbeat heartbeat;

unsigned long lastMillis = millis();

/**
 * Initialises the program.
 */
void setup () {
	Serial.begin(115200);					// Set the baud rate to 115200
	pinMode(LED_BUILTIN, OUTPUT);	// Set the builtin LED to output, so we can use it
}

/**
 * Loops forever.
 */
void loop () {
	LPOSSerial::Clear();
	Serial.print("BPM: ");
	Serial.println(heartbeat.GetBPM(A1, LED_BUILTIN));

	delay(50);
}