/*
 MSGEQ7 Demo app
 Look for the breakout board on www.whizoo.com
 
 
 
*/

// Include the library
#include <EEPROM.h>

// Hardware-specific defines
#define MSGEQ7_STROBE_PIN      7
#define MSGEQ7_RESET_PIN       8
#define MSGEQ7_ANALOG_PIN      A0


#define NUM_FREQUENCY_BANDS    7


// Duemilanove only has 6 PWM outputs, so the last LED won't respond properly.  Your
// board may have more PWM outputs.  Typically you only want to monitor the lowest
// frequency bands because that is where the beat is.
int led[NUM_FREQUENCY_BANDS] = {3, 5, 6, 9, 10, 11, 0};

// There is a concept of "persistence of vision" with LED's.  The LED has to be on long enough
// for the eye to recognise that it is on.  When a high volume is received on a frequency band,
// The LED is turned on (at a high PWM value) and then gradually faded until the next beat in
// that frequency.
int ledPWMValue[NUM_FREQUENCY_BANDS] = {0, 0, 0, 0, 0, 0, 0};
  

void setup() {
  // Set the LED pins as outputs
  for (int i=0; i<NUM_FREQUENCY_BANDS; i++)
    pinMode(led[i], OUTPUT);
  
  // Set up the MSGEQ7 IC
  pinMode(MSGEQ7_ANALOG_PIN, INPUT);
  pinMode(MSGEQ7_STROBE_PIN, OUTPUT);
  pinMode(MSGEQ7_RESET_PIN, OUTPUT);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
}


// This loop executes around 100 times per second
void loop() {
  int frequencyBandVolume;
  
  // Toggle the RESET pin of the MSGEQ7 to start reading from the lowest frequency band
  digitalWrite(MSGEQ7_RESET_PIN, HIGH);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  
  // Read the volume in every frequency band from the MSGEQ7
  for (int i=0; i<NUM_FREQUENCY_BANDS; i++) {
    digitalWrite(MSGEQ7_STROBE_PIN, LOW);
    delayMicroseconds(30); // Allow the output to settle
    frequencyBandVolume = analogRead(MSGEQ7_ANALOG_PIN);
    digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
    
    // The read value is 10-bit (0 to 1024).  PWM needs a value from 0 to 255, so divide by 4
    frequencyBandVolume = frequencyBandVolume >> 2;
    
    // Fade the current LED value for this band
    ledPWMValue[i] = ledPWMValue[i] > 7? ledPWMValue[i] - 7 : 0;
    
    // Don't show the lower values
    if (frequencyBandVolume > 70) {
      // If the new volume is greater than that currently being showed then show the higher volume
      if (frequencyBandVolume > ledPWMValue[i])
        ledPWMValue[i] = frequencyBandVolume;
    }
    
    // Set the LED PWM value to the frequency band's volume
    analogWrite(led[i],  ledPWMValue[i]);
  }
  
  // Wait before executing this loop again
  delay(10);
}

