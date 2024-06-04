#include "config.h"
#include "person.h"
#include <SPI.h>
#include <MFRC522.h>

//sender phone number with coATuntry code
const String PHONE = "+983232254554";  //edit

#define rxPin 15
#define txPin 4
#define BAUD_RATE 9600
#define RST_PIN 22
#define SS_PIN 21
int buzzerpin = 8;
HardwareSerial sim800(1);


String smsStatus, senderNumber, receivedDate, msg;
boolean isReply = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);

String user = ("");
int hozor_H = 0;
AdafruitIO_Feed *Text = io.feed("text");        //edit
AdafruitIO_Feed *Final_H = io.feed("final_H");  //edit
AdafruitIO_Feed *Final_A = io.feed("final_A");  //edit
AdafruitIO_Feed *Final_E = io.feed("final_E");  //edit
AdafruitIO_Feed *Final_Y = io.feed("final_Y");  //edit
AdafruitIO_Feed *hossein = io.feed("Hossein");  //edit
AdafruitIO_Feed *amin = io.feed("Amin");        //edit
AdafruitIO_Feed *ehsan = io.feed("Ehsan");      //edit
AdafruitIO_Feed *younes = io.feed("Younes");    //edit


char usernameArr[] = { "Hossein", "Amin", "Ehsan", "Younes" };                  // Character array for username
char idArr[] = { "72 71 88 51", "5D D2 FD A9", "E9 89 5F B3", "79 55 63 B3" };  // Character array for ID
Person Hossein(usernameArr[0], idArr[0], 0, 0, 0);                              // Hossein
Person Amin(usernameArr[1], idArr[1], 0, 0, 0);                                 // Amin
Person Ehsan(usernameArr[2], idArr[2], 0, 0, 0);                                // Ehsan
Person Younes(usernameArr[3], idArr[3], 0, 0, 0);                               // Younes

void setup() {


  pinMode(buzzerpin, OUTPUT);
  Serial.begin(9600);
  Serial.println("esp32 serial initialize");
  sim800.begin(BAUD_RATE, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800L serial initialize");

  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";

  sim800.print("AT+CMGF=1\r");  //SMS text mode
  delay(1000);

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scan a RFID tag");

  io.connect();

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(io.statusText());
}
void loop() {
  //////////////////////////////////////////////////
  while (sim800.available()) {
    parseData(sim800.readString());
  }
  //////////////////////////////////////////////////
  while (Serial.available()) {
    sim800.println(Serial.readString());
  }
  //////////////////////////////////////////////////
  io.run();

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Print UID of the card
    Serial.print("UID tag :");
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    Serial.println(content.substring(1));
    user = content.substring(1);


    // for Hossein
    if (user == Hossein.id) {
      if (Hossein.hozor == 0) {
        Serial.println("Hossein Vared Shod");
        Text->save("Hossein Vared Shod");
        hossein->save("1");
        tone(buzzerpin, 311);
        delay(150);
        noTone(buzzerpin);
        Hossein.hozor = 1;
        Hossein.start = millis();
        doAction();
      } else {
        Serial.println("Hossein kharej Shod");
        Text->save("Hossein kharej Shod");
        hossein->save("0");
        tone(buzzerpin, 311);
        delay(150);
        noTone(buzzerpin);
        Hossein.hozor = 0;
        Hossein.stop = millis();
        doAction();
        Final_H->save(Hossein.stop - Hossein.start);
      }
    }
  }
  delay(1000);
}  //main loop ends

//***************************************************
void parseData(String buff) {
  Serial.println(buff);

  unsigned int len, index;
  //////////////////////////////////////////////////
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();
  //////////////////////////////////////////////////

  //////////////////////////////////////////////////
  if (buff != "OK") {
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);

    if (cmd == "+CMTI") {
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index + 1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      //get the message stored at memory location "temp"
      sim800.println(temp);
    } else if (cmd == "+CMGR") {
      extractSms(buff);


      if (senderNumber == PHONE) {
        doAction();
      }
    }
    //////////////////////////////////////////////////
  } else {
    //The result of AT Command is "OK"
  }
}

//************************************************************
void extractSms(String buff) {
  unsigned int index;

  index = buff.indexOf(",");
  smsStatus = buff.substring(1, index - 1);
  buff.remove(0, index + 2);

  senderNumber = buff.substring(0, 13);
  buff.remove(0, 19);

  receivedDate = buff.substring(0, 20);
  buff.remove(0, buff.indexOf("\r"));
  buff.trim();

  index = buff.indexOf("\n\r");
  buff = buff.substring(0, index);
  buff.trim();
  msg = buff;
  buff = "";
  msg.toLowerCase();
}

void doAction() {
  // for Hossein
  if (Hossein.hozor == 1) {
    Reply("H in");
  } else if (Hossein.hozor == 0) {
    Reply("H out");
  }
  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";
}
void Reply(String text) {
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A);  //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}