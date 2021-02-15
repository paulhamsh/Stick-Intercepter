#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include <M5StickC.h>

BluetoothSerial SerialBT; //Object for Bluetooth
HardwareSerial HWSerial(2);

#define HW_BAUD 1000000
#define PGM_NAME "App - Stick"

uint8_t bt_byte, ser_byte;
uint8_t buf[5000];
int pos, len;
int scr_line;


#define SPARK_BT_NAME "Spark 40 Audio"

void setup() {
  M5.begin(true,true,false);

  HWSerial.begin(HW_BAUD, SERIAL_8N1, 33, 32);

  M5.Lcd.setRotation(0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.println(PGM_NAME);
  M5.Lcd.println();  
  
  M5.Lcd.println("Starting..");
  if (!SerialBT.begin(SPARK_BT_NAME)) 
  {
    M5.Lcd.println("Failed"); 
    while (true) {};
  }
  M5.Lcd.println("Ready ");

  pos = 0;
  len = -1;

  scr_line = 2;
}

void loop() {
  M5.update();

  if (M5.BtnA.isPressed()) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.println(PGM_NAME);
    M5.Lcd.println();  
    scr_line = 0;
    delay(100);
  }
  
  while (SerialBT.available()) {
    bt_byte = SerialBT.read();  
    HWSerial.write(bt_byte);    
  };
  
  while (HWSerial.available()) {
    ser_byte = HWSerial.read();

//    M5.Lcd.print("{");
//    M5.Lcd.print(ser_byte, HEX);
    
    buf[pos++] = ser_byte;
    if (pos == 7) {
      len = ser_byte; 
    }
    if (pos == len) {
      SerialBT.write(buf, pos);    
      pos = 0;
      len = -1;  
      
      scr_line++;
      if (scr_line > 15) {
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.println(PGM_NAME);    
        M5.Lcd.println(); 
        scr_line = 0;
      }
      
      M5.Lcd.print("< ");
      M5.Lcd.print(buf[6], HEX);
      M5.Lcd.print(" ");
      M5.Lcd.print(buf[20], HEX);
      M5.Lcd.print(" ");
      M5.Lcd.print(buf[21], HEX);
      M5.Lcd.print(" ");
      M5.Lcd.println(buf[24], HEX);
    }
  }
}
