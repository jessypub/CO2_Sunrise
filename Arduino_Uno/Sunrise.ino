/**
 *******************************************************************************
 * @copyright   Copyright (C) by SenseAir AB. All rights reserved.
 * @file        sunrise_i2c_single.ino
 * @brief       Example for reading sensor data in single measurement mode.
 *              Only for revision.
 *              
 *              Based on the "I2C on Senseair Sunrise" documentation 
 *              (available on the www.senseair.com website). This example mainly
 *              covers operations in single measurement mode.
 * @details     Tested on Arduino Mega 2560, Adafruit ESP32 Feather     
 *              
 * @author      William Sandkvist
 * @version     0.08
 * @date        2019-12-20
 *
 *******************************************************************************
 */

 #include <Wire.h>
 /* Define serial EN pin */
 const int       SUNRISE_EN              = 2; //8;
 const uint8_t   ESP32_C3_SDA            = 9;  // default = 8
 const uint8_t   ESP32_C3_SCL            = 10; // default = 9
 
 /* Sunrise communication address, both for Modbus and I2C */
 const uint8_t   SUNRISE_ADDR            = 0x68;
 
 /* Register Addresses */
 const uint8_t ERROR_STATUS             = 0x01;
 const uint8_t MEASUREMENT_MODE         = 0x95;
 const uint8_t METER_CONTROL            = 0xA5;
 const uint8_t START_MEASUREMENT        = 0x93;
 const uint8_t ABC_TIME                 = 0xC4;
 
 /* Measurement modes */
 const uint16_t CONTINUOUS               = 0x0000;
 const uint16_t SINGLE                   = 0x0001;
 
 /* Delays in milliseconds*/
 const int STABILIZATION_MS              = 35;
 const int WAIT_FOR_PIN_MS               = 200;
 
 /* Reading period, in milliseconds. Default is 4 seconds */
 int readPeriodMs = 100;
 int delayPeriod = 300;
 
 /* Array for storing sensor state data */
 uint8_t state[24];
 
 /** 
  * @brief  Wakes up the sensor by initializing a write operation
  *         with no data.
  * 
  * @param  target:      The sensor's communication address
  * @note   This example shows a simple way to wake up the sensor.
  * @retval true if successful, false if failed
  */
 bool _wakeup(uint8_t target)
 {
   int error;
 
     Wire.beginTransmission(target);
     error = Wire.endTransmission(true);
 }
 
 /*******************************************************
  READ CO2 by Jessy
  *********************************************************/
  void read_sensor(uint8_t target){
   uint8_t byteHi;
   uint8_t byteLo;  
   uint8_t RegRead = 20;  
   uint8_t reg1[25];
     
   digitalWrite(SUNRISE_EN, HIGH);
   
   delay(35); 
   _wakeup(target); 
   Wire.beginTransmission(target);
   Wire.write(0x95); // measurement mode
   Wire.write(0x01);   // 0x95 = Single
   Wire.write(0x00);   // 0x96 = 0
   Wire.write(0x01);   // 0x97 = 2 s
   Wire.write(0x00);   // 0x98 = 0
   Wire.write(0x03);   // 0x99 = 4 samples (1.2s)
   Wire.endTransmission(true);   
   delay(20);  
   
   _wakeup(target); 
   Wire.beginTransmission(target);
   Wire.write(0x93);  // Start measurement
   Wire.write(0x01); 
   Wire.endTransmission(true);
   delay(20);  
 
   _wakeup(target); 
   Wire.beginTransmission(target);
   Wire.write(0xC3);  // Start measurement
   Wire.write(0x01); 
   Wire.endTransmission(true);
   delay(20);  
    
   delay(delayPeriod);  
 
   _wakeup(target); 
 //  Wire.requestFrom(target, RegRead /* how many bytes */, (uint32_t)0x00 /* from address*/, (uint8_t)1/* Address size - 1 byte*/, true /* STOP*/); 
   Wire.requestFrom(target, RegRead /* how many bytes */,  true /* STOP*/); 
   for(int n = 0 ; n < RegRead ; n++) {
     reg1[n] = Wire.read();
   }
 
   /* CO2 value */
   uint16_t co2Val = ((int16_t)(int8_t) reg1[0x06] << 8) | (uint16_t)reg1[0x07];     
   /* Print values */
  // Serial.print("CO2: ");
   Serial.print(co2Val);
   Serial.println();
  }
  
 /**
  * @brief  This function runs once at the start.
  *
  * @retval None
  */
 void setup()
 {
   pinMode(SUNRISE_EN, OUTPUT);
   digitalWrite(SUNRISE_EN, HIGH);
   /* Wait for sensor start-up and stabilization */
   delay(STABILIZATION_MS);
 //  Wire.begin(ESP32_C3_SDA, ESP32_C3_SCL);
   Wire.begin();
   /* Setup I2C clock to 100kHz */
   Wire.setClock(100000);    
   Serial.begin(115200);
 }
 
 /**
  * @brief  The main function loop. Reads the sensor's current
  *         CO2 value and error status and prints them to the 
  *         Serial Monitor.
  * 
  * @retval None
  */
 void loop() {
   read_sensor(SUNRISE_ADDR);
 }
 