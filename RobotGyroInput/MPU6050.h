#pragma once

#include <Wire.h>

// see: https://microcontrollerslab.com/mpu-6050-esp8266-nodemcu-oled/
// GPIO5 (I2C SCL) => D1
// GPIO4 (I2C SDA) => D2

#include "PushButton.h"

const long SEND_INTERVAL = 150;
const long SEND_INTERVAL_ADD = 150;   // add time to go
const int MIN_SPEED = 60; 

inline void LimitPublishGo(uint direction, uint speed, uint duration)
{
  static bool _speed0Sent = false;
  if (speed < MIN_SPEED)
  {
    speed = 0;
  }

  if (speed != 0 || _speed0Sent == false)
  {
    PublishGo(direction, speed, duration);
    _speed0Sent = speed == 0;
  }
}

inline void setupMPU6050()
{
  #if defined(SDA_PIN)
   // used for esp01
  Wire.begin(SDA_PIN, SCL_PIN);
  #endif  

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    statusLed.SetOnOffTime(75);
    while (1)
    {
      delay(10);
      statusLed.Loop();
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
  static bool isOn = PUSHBUTTON_ISON;
  static CPushButton pushButton(PUSHBUTTON_PIN, LOW);

  if (pushButton.IsOn())
  {
    // toggle state
    isOn = !isOn;
    //statusLed.SetOnOffTime(isOn ? 1000: 100);
    statusLed.SetLed(isOn, 100000000);
  }

  if (millis() < until || !isOn)
  {
    return;
  }

  until = millis() + SEND_INTERVAL;

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculation
  // 0 position: x,y = 0, z = 9.81
  // full forward (90Grad in x) x=9.81, y=0, z=0
  // full backward (-90Grad in x) x=-9.81, y=0, z=0
  // right (90Grad in y) x=0, y=9.81, y=0, z=0
  // left (-90Grad in y) x=0, y=-9.81, y=0, z=0
  // forward+right (90 in x, yin 90) x=sin(45)*9.81, y=cos(45)*9.81, z=0

  auto myx = a.acceleration.x;
  auto myy = a.acceleration.y;

  auto gyro_x = (int)(myx * 100.0); // range -1000 to +1000
  auto gyro_y = (int)(myy * 100.0); // range -1000 to +1000

  gyro_x = min(max(gyro_x, -1000), 1000);
  gyro_y = min(max(gyro_y, -1000), 1000);

  auto x = map(gyro_x, -1000, 1000, -255, 255);
  auto y = map(gyro_y, -1000, 1000, -255, 255);

  auto direction = (uint)(atan2(GYROCONVERT(x,y)) / PI * 180.0) + 180;
  auto speed = (uint)min(255, (int)sqrt((double)x * x + (double)y * y));

  LimitPublishGo(direction, speed, SEND_INTERVAL + SEND_INTERVAL_ADD);
}