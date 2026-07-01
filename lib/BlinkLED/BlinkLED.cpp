#include "BlinkLED.h"

// --- SmartTimer ---

SmartTimer::SmartTimer(TimerType TType, uint16_t Measure)
  : ttype(TType), start(micros()), measure(Measure) {}

uint32_t SmartTimer::elapsed() const {
  return micros() - start;
}

void SmartTimer::reset() {
  start = micros();
}

bool SmartTimer::running() {
  switch (ttype) {
    case TIMER_TYPE_HZ: return elapsed() < (1000000 / measure);
    case TIMER_TYPE_MS: return elapsed() < (measure * 1000);
    case TIMER_TYPE_US: return elapsed() < measure;
  }
  return false;
}

bool SmartTimer::completed() {
  return !running();
}

// --- BlinkLED ---

BlinkLED::BlinkLED(int pin, uint16_t hz)
  : _pin(pin), _timer(TIMER_TYPE_HZ, hz) {
  pinMode(_pin, OUTPUT);
}

void BlinkLED::run() {
  if (_timer.running()) return;
  digitalWrite(_pin, !digitalRead(_pin));
  _timer.reset();
}
