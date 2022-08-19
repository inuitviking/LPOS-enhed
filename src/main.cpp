#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "DFRobot_Heartrate.h"
DFRobot_Heartrate heartrate(DIGITAL_MODE);

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

int peakMinimum = 850;

unsigned long time;


void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
	heartrate.getValue(PIN_PA22);
	int sensorValue = analogRead(A1);
	if(sensorValue >= peakMinimum) {
		Serial.println(heartrate.getRate());
	}
	delay(250);
}