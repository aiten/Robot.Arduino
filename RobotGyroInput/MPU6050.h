#pragma once

// see: https://microcontrollerslab.com/mpu-6050-esp8266-nodemcu-oled/
// GPIO5 (I2C SCL) => D1
// GPIO4 (I2C SDA) => D2

#include "PushButton.h"

const uint8_t statusLed = LED_BUILTIN;
const long SEND_INTERVAL = 100;

inline void LimitPublishGo(uint direction, uint speed, uint duration)
{
  static bool _speed0Sent = false;
  if (speed < 30)
  {
    speed = 0;
  }
  else
  {
    speed = (uint)map((int)speed, 0, 255, 30, 255);
  }

  if (speed != 0 || _speed0Sent == false)
  {
    PublishGo(direction, speed, duration);
    _speed0Sent = speed == 0;
  }
}

inline void setupMPU6050()
{
  pinMode(statusLed, OUTPUT);

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      digitalWrite(statusLed, LOW);
      delay(75);
      digitalWrite(statusLed, HIGH);
      delay(75);
    }
    // never return
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println();
}

inline void loopMPU6050()
{
  static long until = 0;
  static bool isOn = false;
  static CPushButton pushButton(D3, LOW);

  if (pushButton.IsOn())
  {
    // toggle state
    isOn = !isOn;
    digitalWrite(statusLed, isOn);
  }

  if (millis() > until)
  {
    return;
  }

  until = millis() + SEND_INTERVAL;

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // g.gyro.x: max +/- 12
  auto x = map((uint)(g.gyro.x * 65535.0 / 12.0), 0, 65535, -255, 255);
  auto y = map((uint)(g.gyro.y * 65535.0 / 12.0), 0, 65535, -255, 255);

  auto direction = (uint)(atan2(-x, y) / PI * 180.0) + 180;
  auto speed = (uint)min(255, (int)sqrt((double)x * x + (double)y * y));

  LimitPublishGo(direction, speed, SEND_INTERVAL + 50);
  /*

  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println();
  */
}