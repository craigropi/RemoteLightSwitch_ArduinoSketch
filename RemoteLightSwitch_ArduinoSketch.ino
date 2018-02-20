
#include <CurieBLE.h>
#include <Servo.h>

/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A8A8A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedCharCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A8A8A", BLERead | BLEWrite);
Servo myservo;  // create servo object to control a servo
                // twelve servo objects can be created on most boards

int pos = 90;    // variable to store the servo position

const int ledPin = 13; // pin to use for the LED

void setup() {
  myservo.attach(9, 700, 2300);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("Remote Lightswitch");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);

  // set the initial value for the characeristic:
  switchCharacteristic.setValue(0);

  // begin advertising BLE service:
  blePeripheral.begin();

  Serial.println("BLE Remote Lightswitch Peripheral");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0 "Turn on light"
          Serial.println(F("Lights off"));
          digitalWrite(ledPin, LOW);          // will turn the LED off
          for(pos = 90; pos>=-0; pos-=1)     // goes from 180 degrees to 0 degrees
          {
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            delay(10);                       // waits 15ms for the servo to reach the position
          }
          for(pos = 0; pos<=90; pos +=1)     // goes from 180 degrees to 0 degrees
          {
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            delay(10);                       // waits 15ms for the servo to reach the position
          }
        } else {                              // a 0 value "Turn off light"
          Serial.println("Lights on");
          digitalWrite(ledPin, HIGH);         // will turn the LED on
          for(pos = 90; pos <= 190; pos += 1) // goes from 0 degrees to 180 degrees
          {                                  // in steps of 1 degree
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            delay(10);                       // waits 15ms for the servo to reach the position
          }
          for(pos = 190; pos >= 90; pos -= 1) // goes from 0 degrees to 180 degrees
          {                                  // in steps of 1 degree
            myservo.write(pos);              // tell servo to go to position in variable 'pos'
            delay(10);                       // waits 15ms for the servo to reach the position
          }
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

