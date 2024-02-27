class HCSr04
{
  uint8_t _trgPin;
  uint8_t _echoPin;
  uint8_t _samples;

public:
  HCSr04(uint8_t trgPin, uint8_t echoPin, uint8_t samples = 1)
  {
    _trgPin = trgPin;
    _echoPin = echoPin;
    _samples = samples;

    pinMode(_trgPin, OUTPUT);
    pinMode(_echoPin, INPUT);
  }

  uint32_t ReadDuration()
  {
      // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
      // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
      digitalWrite(_trgPin, LOW);
      delayMicroseconds(2);
      digitalWrite(_trgPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(_trgPin, LOW);

      // The same pin is used to read the signal from the PING))): a HIGH
      // pulse whose duration is the time (in microseconds) from the sending
      // of the ping to the reception of its echo off of an object.
      // timeout 340m/s (at 20°)=>  10000000 / 34 / 2 => 5m max
      noInterrupts();
      uint32_t duration = pulseIn(_echoPin, HIGH, 1000000 / 34 / 2);
      interrupts();

//      Serial.println(duration);Serial.print(" ");

      return duration;
  }

  uint32_t ReadDuration(uint8_t samples)
  {
    uint32_t durationsum = ReadDuration();

    for (uint8_t i = 1; i < samples; i++) {
      DelayBetweenMeasurement();
      durationsum += ReadDuration();
    }
    return durationsum;
  }

  static void DelayBetweenMeasurement()
  {
      delay(30);
  }

  uint ReadMM()
  {
    // The speed of sound is 340 m/s or 29 microseconds per centimeter.
    return (uint) (ReadDuration(_samples) / _samples / 3 / 2);
  }
};