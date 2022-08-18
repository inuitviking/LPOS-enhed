// Includes
// - Libraries
#include <Arduino.h>
#include <protothreads.h>

// Variables
// - Millis
unsigned long lastMillis = 0;
const int LED = 13;
// - Protothreads
pt ptBlink;

int blinkThread(struct pt* pt) {
	PT_BEGIN(pt);

		// Loop forever
		for(;;) {
			digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
			PT_SLEEP(pt, 1000);
			digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
			PT_SLEEP(pt, 1000);
		}

	PT_END(pt);
}

/**
 * Initialises the program
 */
void setup() {
	// Start Serial terminal
	Serial.begin(9600);

	PT_INIT(&ptBlink);
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
}

/**
 * The code to be executed
 */
void loop() {
	PT_SCHEDULE(blinkThread(&ptBlink));
	Serial.println("loop");
}