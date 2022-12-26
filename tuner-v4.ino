
//4 Channel Eurorack Tuner by Skull & Circuits v1.2
//For use on the Arduino ATMEGA328PPU 8Bit microprocessor
//info@skullandcircuits.com
//November 2021
//https://www.skullandcircuits.com
//
//Frequency counter code is adapted from:
//sine wave freq detection with 38.5kHz sampling rate and interrupts
//by Amanda Ghassaei
//https://www.instructables.com/id/Arduino-Frequency-Detection/
//July 2012

/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
*/

//data storage variables
byte newData = 0;
byte prevData = 0;

//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period;
double frequency;
double NoteNumber = 0;
int RoundNoteNumber = 0;
int NoteValue = 0;
float Deviation = 0;
String Notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "-"};

const int numReadings = 25;     // smoothing of the data
double readings[numReadings];   // the readings from the analog input
int index = 0;                  // the index of the current reading
float total = 0;                // the running total
float averageFrequency = 0;     // the average

// SETUP

void setup() {

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;

  //declaring pins
  pinMode(2, OUTPUT); // 7 segment LED
  pinMode(3, OUTPUT); // 7 segment LED
  pinMode(4, OUTPUT); // 7 segment LED
  pinMode(5, OUTPUT); // 7 segment LED
  pinMode(6, OUTPUT); // 7 segment LED
  pinMode(7, OUTPUT); // 7 segment LED
  pinMode(8, OUTPUT); // 7 segment LED
  pinMode(9, OUTPUT); // 7 segment LED
  pinMode(10, OUTPUT); // -- LED
  pinMode(11, OUTPUT); // - LED
  pinMode(12, OUTPUT); // + LED
  pinMode(13, OUTPUT); // ++ LED

  cli();//diable interrupts

  //set up continuous sampling of analog pin 0

  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;

  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only

  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements

  sei();//enable interrupts
}

ISR(ADC_vect) {//when new ADC value ready

  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >= 127) { //if increasing and crossing midpoint
    period = timer;//get period
    timer = 0;//reset timer
  }
  // if there is no signal present the timer would never reset screwing up our readings
  // if the timing exceeds what to be expected from a signmal lower then 20Hz trigger it
  if ( timer > 2000 ) {
       period = 2000;//set period to minimum
    timer = 0;//reset timer
    }
  timer++;//increment timer at rate of 38.5kHz
}

void loop() {

  frequency = 38462 / period; //timer rate/period

  total = total - readings[index];  // subtract the last reading:
  readings[index] = frequency;      // Add the frequency to the array for averaging
  total = total + readings[index];  // add the reading to the total
  index++;
  if (index >= numReadings) {
    index = 0; // if we're at the end of the array wrap around to the beginning
  }
  averageFrequency = total / numReadings;  // calculate the average

  // Check whether the detected frequency lies between 20Hz to 10kHz, otherwise set note to "-"
  if ( averageFrequency < 20 || averageFrequency > 10000 ) {
    NoteValue = 12;
    Deviation = 0;
  } else {
    // convert the frequency to a midi note number ( to make life easy )
    NoteNumber = 12 * Log2( averageFrequency / 440 ) + 69;
    // Calculating deviation from the nearest note
    RoundNoteNumber = round( NoteNumber );
    NoteValue = RoundNoteNumber % 12;
    Deviation = NoteNumber - RoundNoteNumber;
  }
  // Update the display
  setDisplay();
  delay(10);
}

// set display and deviation leds

void setDisplay() {
  // set deviation
  if ( Deviation > 0.25 ) {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  } else if ( Deviation > 0.10 ) {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  } else if ( Deviation > -0.10 ) {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  } else if ( Deviation > -0.25 ) {
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  } else if ( Deviation < -0.25 ) {
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  } else {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    }
  // set 7-segment LED
  switch (NoteValue) {
    case 0:
      //C
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      break;
    case 1:
      //C#
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      break;
    case 2:
      //D
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      break;
    case 3:
      //D#
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(9, HIGH);
      break;
    case 4:
      //E
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      break;
    case 5:
      //F
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      break;
    case 6:
      //F#
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      break;
    case 7:
      //G
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      break;
    case 8:
      //G#
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      break;
    case 9:
      //A
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      break;
    case 10:
      //A#
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      break;
    case 11:
      //B
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
      break;
    case 12:
      // No detection
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      digitalWrite(9, LOW);
  }
}

// math function because math is math

double Log2( double n )
{
  return log( n ) / log( 2 );
}
