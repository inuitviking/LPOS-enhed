// Includes
// - Libraries
#include <Arduino.h>		// The main arduino library
// - Custom
#include "LPOSSerial.h"

/**
 * This method simply clears the serial terminal window.
 *
 * @return void
 */
void LPOSSerial::Clear() {
	Serial.write(27);		// ESC command
	Serial.print("[2J");	// clear screen command
	Serial.write(27);		// ESC command
	Serial.print("[H");		// cursor to home command

}
