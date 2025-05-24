/*
 * This is the code for the Silent Wall and Desk Clock
 * Link1: https://www.thingiverse.com/thing:4876905
 * Link2: https://www.prusaprinters.org/prints/68108-silent-wall-and-desk-clock
 * 
 * The wireing diagram can be found on the linked pages.
 * 
 * The variable "Clock_direction" determains the direction of the spinning clock, the
 * default is clock-wise of course.
 * 
*/

#include <Arduino.h>
// "StepperDriver" by Laurentiu Badea v1.4.1
// Info: https://github.com/laurb9/StepperDriver
#include "BasicStepperDriver.h"

// "DS3231" by Andrew Wickert, Eric Ayars, Jean-Claude Wippler, Northern Widget LLC v1.1.2
// Info: https://github.com/NorthernWidget/DS3231

// Arduino Libraries:
#include <DS3231.h> // Info: https://github.com/NorthernWidget/DS3231
#include <Wire.h>



// Pindefinitions +++++++++++++++++

// Stepperdriver
#define Stepper_dir 12  // direction
#define Stepper_step 13 // step
#define Stepper_en 11   // enable

// constants 
const int CW = -1;
const int CCW = 1;
// Set direction: CW is clockwise, CCW ist anti clockwise #+#+#+#+#+#+#+#+#+#+
const int Clock_direction = CW;
// #+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#+#

// RTC
#define RTC_SDA A4 // I2C SDA
#define RTC_SCL A5 // I2C SCL
#define RTC_int 2  // Rtc interruptpin, 1Hz clock (SQW)



// Setup Stepperdriver ++++++++++++

#define MOTOR_STEPS 2038 // steps and gearing of steper motor
#define Stepper_Speed 2.5          // motor speed, 2.5 rpm should be inaudible
#define MICROSTEPS 16    // microsteps of the stepper driver

// initialize stepper class (no, of steps per revolution, direction and step pin
BasicStepperDriver stepper(MOTOR_STEPS, Stepper_dir, Stepper_step);

// Set the number of steps to move minute handle by one minute
// const unsigned long stepper_oneMin = 8152; // calculated
const unsigned long stepper_oneMin = 8192; // measured in practice
const unsigned long stepper_halfMin = 4096;
const unsigned long stepper_quarterMin = 2048;

// move minute handle every 15 seconds
int moving_threshold = 15;


// Setup RTC ++++++++++++++++++++++++
DS3231 Clock;


volatile int Seconds = 0;


void setup() {

  // Start stepper driver ###########################
  stepper.begin(Stepper_Speed, MICROSTEPS);
  pinMode(Stepper_en, OUTPUT);
  digitalWrite(Stepper_en,LOW);

  // Setup RTC ######################################
  Wire.begin(); // start I2C
  // Turn on oscillator pin, frequency 1 Hz
  Clock.enableOscillator(true, false, 0); 


  // Setup interrupt pin
  pinMode(RTC_int, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_int), secondsCounter, FALLING);
}

void loop() {

  // check if 15 seconds passed
  if (Seconds >= moving_threshold) {
    Seconds = 0;
    incrementQuarterMinute(); // this takes some time
    // incrementHalfMinute();
    // incrementOneMinute();
  }

}


void incrementQuarterMinute() {
  stepper.move(Clock_direction*stepper_quarterMin);
}

void incrementHalfMinute() {
  stepper.move(Clock_direction*stepper_halfMin);
}

void incrementOneMinute() {
  stepper.move(Clock_direction*stepper_oneMin);
}

void secondsCounter() {
  Seconds++;
}
