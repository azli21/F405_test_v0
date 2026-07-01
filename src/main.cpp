#include <Arduino.h>

int myLED = PB5; // PC13; // Example pin for an LED

#ifndef SMARTTIMER_TYPE_H
#define SMARTTIMER_TYPE_H

enum TimerType { 
  TIMER_TYPE_HZ,   // Frequency in Hertz
  TIMER_TYPE_MS,   // Duration in milliseconds
  TIMER_TYPE_US    // Duration in microseconds
};

class SmartTimer {
public:
  SmartTimer(TimerType TType, uint16_t Measure) : ttype(TType), start(micros()), measure(Measure) {}
  
  // Returns elapsed time in milliseconds
  uint32_t elapsed() const {
    return micros() - start;
  }

  // Resets the timer
  void reset() {
    start = micros();
  }

bool running() {

    switch (ttype) {
      case TIMER_TYPE_HZ:
        return elapsed() < (1000000 / measure);
      case TIMER_TYPE_MS:
        return elapsed() < (measure * 1000);
      case TIMER_TYPE_US:
        return elapsed() < measure;
    }

    return false; // Default case (should not reach here)
}

bool completed() {
    return !running();
}

private:
  TimerType ttype;
  uint16_t measure;  // Frequency in Hz or duration in ms/us
  uint32_t start; // Start time in microseconds
};

#endif // SMARTTIMER_TYPE_H


void setup() {
  pinMode(myLED, OUTPUT); // Set the LED pin as output
}


void blinkLED() {
  static SmartTimer timer(TIMER_TYPE_HZ, 3); // 3 Hz timer
  if (timer.running()) return;

  digitalWrite(myLED, !digitalRead(myLED)); // Toggle LED state
  timer.reset();

  // Alternative approach using completed()  
  // if (timer.completed()) {
  //   digitalWrite(myLED, !digitalRead(myLED)); // Toggle LED state
  //   timer.reset();
  // }
  
}


void loop() {
  // // blink the LED
  // digitalWrite(myLED, HIGH); // Turn the LED on
  // delay(1000); // Wait for a second
  // digitalWrite(myLED, LOW); // Turn the LED off 
  // delay(100); // Wait for a second
  blinkLED();
}

