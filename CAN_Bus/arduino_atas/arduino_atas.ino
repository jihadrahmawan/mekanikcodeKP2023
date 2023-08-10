#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "mcp2515.h"


struct can_frame canMsg;
MCP2515 mcp2515(10);

typedef struct {
  int distance;
  int strength;
  boolean receiveComplete;
} TFmini;

TFmini TFminiDepan = { 0, 0, false };
TFmini TFminiBawah = { 0, 0, false };
TFmini TFminiKanan = { 0, 0, false };
TFmini TFminiKiri = { 0, 0, false };


SoftwareSerial portSdepan(3, 4);
SoftwareSerial portSbawah(5, 6);
SoftwareSerial portSkanan(7, 8);
SoftwareSerial portSkiri(A1, A0);


void getTFminiData(SoftwareSerial* port, TFmini* tfmini) {
  port->listen();
  while (tfmini->receiveComplete == false) {
    static char i = 0;
    char j = 0;
    int checksum = 0;
    static int rx[9];
    if (port->available()) {
      rx[i] = port->read();
      if (rx[0] != 0x59) {
        i = 0;
      } else if (i == 1 && rx[1] != 0x59) {
        i = 0;
      } else if (i == 8) {
        for (j = 0; j < 8; j++) {
          checksum += rx[j];
        }
        if (rx[8] == (checksum % 256)) {
          tfmini->distance = rx[2] + rx[3] * 256;
          tfmini->strength = rx[4] + rx[5] * 256;
          tfmini->receiveComplete = true;
        }
        i = 0;
      } else {
        i++;
      }
    }
  }
}



int perintahMagnetNaik = 100;
int perintahMagnetTurun = 200;



void setup() {
  Serial.begin(9600);
  portSdepan.begin(115200);
  portSbawah.begin(115200);
  portSkanan.begin(115200);
  portSkiri.begin(115200);


  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();



  // put your setup code here, to run once:
}

void loop() {

  if (Serial.available()) {
    char a = Serial.read();
    if (a == 'n') {
      //Serial.println("Kirim naik");
      canMsg.can_id = 0x036;
      canMsg.can_dlc = 8;
      canMsg.data[0] = perintahMagnetNaik;
      canMsg.data[1] = 0x00;
      canMsg.data[2] = 0x00;
      canMsg.data[3] = 0x00;
      canMsg.data[4] = 0x00;
      canMsg.data[5] = 0x00;
      canMsg.data[6] = 0x00;
      canMsg.data[7] = 0x00;
      mcp2515.sendMessage(&canMsg);
    }
    if (a == 't') {
      //Serial.println("Kirim turun");
      canMsg.can_id = 0x036;
      canMsg.can_dlc = 8;
      canMsg.data[0] = perintahMagnetTurun;
      canMsg.data[1] = 0x00;
      canMsg.data[2] = 0x00;
      canMsg.data[3] = 0x00;
      canMsg.data[4] = 0x00;
      canMsg.data[5] = 0x00;
      canMsg.data[6] = 0x00;
      canMsg.data[7] = 0x00;
      mcp2515.sendMessage(&canMsg);
    }
  }

  static unsigned long lastTime = millis();
  static unsigned int count = 0;
  static unsigned int frequency = 0;

  getTFminiData(&portSdepan, &TFminiDepan);
  getTFminiData(&portSbawah, &TFminiBawah);
  getTFminiData(&portSkanan, &TFminiKanan);
  getTFminiData(&portSkiri, &TFminiKiri);

  if (TFminiDepan.receiveComplete == true && TFminiBawah.receiveComplete == true && TFminiKanan.receiveComplete == true && TFminiKiri.receiveComplete == true) {
    ++count;
    char buffer[33];

    if (millis() - lastTime > 999) {
      lastTime = millis();
      frequency = count;
      count = 0;
    }

    // Serial.print("Jarak Depan : ");
    // Serial.print(TFminiDepan.distance);
    // Serial.print("cm\t");
    // Serial.print("Jarak Bawah : ");
    // Serial.print(TFminiBawah.distance);
    // Serial.print("cm\t");
    // Serial.print("Jarak Kanan : ");
    // Serial.print(TFminiKanan.distance);
    // Serial.print("cm\t");
    // Serial.print("Jarak Kiri : ");
    // Serial.print(TFminiKiri.distance);
    // Serial.print("cm\t");
    // Serial.print(frequency);  //40~70Hz, It maybe higher if we don't print other thing.
    // Serial.println("Hz");

    sprintf(buffer, "a%4d%4d%4d%4d",
            TFminiDepan.distance,
            TFminiBawah.distance,
            TFminiKanan.distance,
            TFminiKiri.distance);
    Serial.println(buffer);

    TFminiDepan.receiveComplete = false;
    TFminiBawah.receiveComplete = false;
    TFminiKanan.receiveComplete = false;
    TFminiKiri.receiveComplete = false;
  }
}
