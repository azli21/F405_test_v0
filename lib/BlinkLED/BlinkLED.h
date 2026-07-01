#pragma once
#include <Arduino.h>

enum TimerType {
  TIMER_TYPE_HZ,  // Frequency in Hertz
  TIMER_TYPE_MS,  // Duration in milliseconds
  TIMER_TYPE_US   // Duration in microseconds
};

class SmartTimer {
public:
  SmartTimer(TimerType TType, uint16_t Measure);

  uint32_t elapsed() const;
  void reset();
  bool running();
  bool completed();

private:
  TimerType ttype;
  uint16_t measure;
  uint32_t start;
};

class BlinkLED {
public:
  BlinkLED(int pin, uint16_t hz);
  void run();

private:
  int _pin;
  SmartTimer _timer;
};
