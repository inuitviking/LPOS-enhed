#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
bool peaked = false;
bool caved = true;
int peakMinimum = 850;
int peakCount = 0;
int recordTime = 5;
unsigned long lastMillis = millis();
int bpm = 0;
int ledState = LOW;

int getBPM () {

	int sensorValue = analogRead(A1);
	if (sensorValue >= peakMinimum) {
		if (caved) {
			peaked = true;
			caved = false;
			ledState = HIGH;
			peakCount++;
		}
	} else if (sensorValue < peakMinimum) {
		if (peaked) {
			peaked = false;
			caved = true;
			ledState = LOW;
		}
	}

	if ((millis() - lastMillis) >= (recordTime * 1000)) {
		bpm = 60*(peakCount)/recordTime;
		peakCount = 0;
		lastMillis = millis();
	}

	digitalWrite(LED_BUILTIN, ledState);

	return bpm;
}

void setup () {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop () {

	Serial.print("BPM: ");
	Serial.println(getBPM());

	delay(50);
}