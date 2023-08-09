#include <Arduino.h>
#include <SPI.h>
#include "mcp2515.h"

struct can_frame can_kirim_arduAtas;
struct can_frame can_terima_arduAtas;
MCP2515 mcp2515(10);

const int sharpGP1 = A0;
const int sharpGP2 = A1;
const int sharpGP3 = A2;
const int sharpGP4 = A3;
int s1 = 0;
int s2 = 0;
int s3 = 0;
int s4 = 0;
const int PWM_ANGKAT_MAGNET = 6;
const int PWM_TURUN_MAGNET = 5;

void setup() {
  pinMode(sharpGP1, INPUT);
  pinMode(sharpGP2, INPUT);
  pinMode(sharpGP3, INPUT);
  pinMode(sharpGP4, INPUT);
  pinMode(PWM_ANGKAT_MAGNET, OUTPUT);
  pinMode(PWM_TURUN_MAGNET, OUTPUT);
  Serial.begin(9600);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();
  // put your setup code here, to run once:
}



void loop() {
  s1 = map(analogRead(sharpGP1), 0, 1023, 0, 255);
  s2 = map(analogRead(sharpGP2), 0, 1023, 0, 255);
  s3 = map(analogRead(sharpGP3), 0, 1023, 0, 255);
  s4 = map(analogRead(sharpGP4), 0, 1023, 0, 255);


  can_kirim_arduAtas.can_id = 0x036;  //CAN id as 0x036
  can_kirim_arduAtas.can_dlc = 8;     //CAN data length as 8
  can_kirim_arduAtas.data[0] = s1;    //Update humidity value in [0]
  can_kirim_arduAtas.data[1] = s2;    //Update temperature value in [1]
  can_kirim_arduAtas.data[2] = s3;    //Rest all with 0
  can_kirim_arduAtas.data[3] = s4;
  can_kirim_arduAtas.data[4] = 0x00;
  can_kirim_arduAtas.data[5] = 0x00;
  can_kirim_arduAtas.data[6] = 0x00;
  can_kirim_arduAtas.data[7] = 0x00;

  //mcp2515.sendMessage(&can_kirim_arduAtas);
  //delay(50);
  // Serial.println(s4);
  // Serial.println(s2);
  // Serial.println(s3);
  // Serial.println(s4);

  if (mcp2515.readMessage(&can_terima_arduAtas) == MCP2515::ERROR_OK) {
    //if (can_terima_arduAtas.can_id == 0x048) {
      int perintahmotor = can_terima_arduAtas.data[0];
      if (perintahmotor == 100) {
        Serial.println("naik");
        analogWrite(PWM_ANGKAT_MAGNET, 255);
        analogWrite(PWM_TURUN_MAGNET, 0);
      }
      if (perintahmotor == 200) {
        Serial.println("turun");
        analogWrite(PWM_ANGKAT_MAGNET, 0);
        analogWrite(PWM_TURUN_MAGNET, 255);
      }
   // }
  }


  // if (Serial.available()) {
  //   a = Serial.read();
  //   if (a == 'a') {
  //     Serial.println("naik");
  //     analogWrite(PWM_ANGKAT_MAGNET, 255);
  //     analogWrite(PWM_TURUN_MAGNET, 0);
  //   }

  //   if (a == 'b') {
  //     Serial.println("turun");
  //     analogWrite(PWM_ANGKAT_MAGNET, 0);
  //     analogWrite(PWM_TURUN_MAGNET, 255);
  //   }
  // }


  // put your main code here, to run repeatedly:
}
