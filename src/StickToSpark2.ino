#include <M5StickC.h>
#include "BluetoothSerial.h"

HardwareSerial HWSerial(2);
BluetoothSerial SerialBT;

#define HW_BAUD 1000000
#define PGM_NAME "Stick - Spark"
#define SCR_HEIGHT 10

#define SPARK_NAME "Spark 40 Audio"
#define MY_NAME    "StickToSpark"

uint8_t ser_byte, bt_byte;
bool connected;
uint8_t buf[5000];
int pos;
int scr_line;
int pre;

unsigned long count;

byte get_serial[]{0x01, 0xfe, 0x00, 0x00, 0x53, 0xfe, 0x17, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                  0xf0, 0x01, 0x01, 0x01, 0x02, 0x23, 0xf7};

byte set_preset[]{0x01, 0xfe, 0x00, 0x00, 0x53, 0xfe, 0x1a, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                  0xf0, 0x01, 0x01, 0x01, 0x01, 0x38, 
                  0x00, 0x00, 0x01, 0xf7}; 
                  
byte upd_preset[]{0x01, 0xfe, 0x00, 0x00, 0x41, 0xff, 0x1a, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                  0xf0, 0x01, 0x01, 0x01, 0x03, 0x38, 
                  0x00, 0x00, 0x01, 0xf7}; 

void start_bt() {
  if (!SerialBT.begin (MY_NAME, true)) {
    M5.Lcd.println("Init fail");
    while (true);
  }    
  connected = false;
}

void connect_to_spark() {
  int rec;

  while (!connected) {
    M5.Lcd.println("Connecting");
    connected = SerialBT.connect(SPARK_NAME);
    if (connected && SerialBT.hasClient()) {
       M5.Lcd.println("Connected");
    }
    else {
       connected = false;
       delay(3000);
    }
  }
  // flush anything read from Spark - just in case

  while (SerialBT.available())
    rec = SerialBT.read(); 
}

void next_line() {
  scr_line++;
  if (scr_line > 15) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println(PGM_NAME);    
    M5.Lcd.println(); 
    scr_line = 0; 
  }
}

void flush_in() {
  bt_byte = SerialBT.read();
  while (bt_byte != 0xf7)
    bt_byte = SerialBT.read();
}

void send_preset(int pres) {
  set_preset[24] = pres;
  SerialBT.write(set_preset, 26);
  flush_in(); 
  upd_preset[24] = pres;
  upd_preset[19] = 0x01;
  if (pres == 1) upd_preset[19] = 0x02;
  HWSerial.write(upd_preset, 26);
}

void setup() {
  M5.begin();
  M5.update();

  HWSerial.begin(HW_BAUD, SERIAL_8N1, 33, 32);
  
  M5.Lcd.setRotation(0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println(PGM_NAME);
  M5.Lcd.println(); 

  start_bt();
  connect_to_spark();

  pos = 0;
  scr_line = 3;
  count = millis();
  pre = 0;
}
void loop() {
  M5.update();
  
  if (M5.BtnA.isPressed()) {
    // this will force a clear screen
    scr_line = 20;
    next_line();
    delay(100);
  }

  if (M5.BtnB.isPressed()) {
    next_line();
    M5.Lcd.print("CHG PRESET ");
    M5.Lcd.println(pre, HEX);
    send_preset(pre);
    pre++;
    if (pre > 3) pre = 0;
    delay(400);
  }
  
  while (HWSerial.available()) {
    ser_byte = HWSerial.read();
    buf[pos++] = ser_byte;
    
    if (ser_byte == 0xf7) {
      // end of the packet, so write it to Spark
      SerialBT.write(buf, pos);
      pos = 0;
      count = millis();
      next_line();
     
      M5.Lcd.print("> ");
      M5.Lcd.print(buf[6], HEX);
      M5.Lcd.print(" ");
      M5.Lcd.print(buf[20], HEX);
      M5.Lcd.print(" ");
      M5.Lcd.println(buf[21], HEX);
    }
  }

  while (SerialBT.available()) {
    count = millis();
    bt_byte = SerialBT.read();
    HWSerial.write(bt_byte);
  }

  if (millis() - count > 10000) {
    // request serial number and read returned bytes and discard - keep-alive link to Spark
    next_line();
    M5.Lcd.println("KEEP ALIVE");
   
    count = millis();
    SerialBT.write(get_serial, 23);
    flush_in();
  }
}
