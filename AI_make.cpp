// The code can be improved in several ways to make it cleaner, more efficient, and maintainable. Here's a refined version:

// ### Improvements:
// 1. **Remove unnecessary global variables.**
// 2. **Refactor duplicate code into reusable functions** (e.g., the similar logic in `doAction_H()`, `doAction_A()`, etc.).
// 3. **Enhance readability and modularization**.
// 4. **Use `enum` for user identification**, making it more readable and reducing hardcoded strings.
// 5. **Reduce duplication in RFID processing and SMS responses** by parameterizing repetitive functions.

// Here’s the refactored version:

// ```cpp
#include "config.h"
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

// Sender phone number with country code
const String PHONE = "+989121231212";  // edit

#define rxPin 15
#define txPin 4
#define BAUD_RATE 9600
#define RST_PIN 22
#define SS_PIN 21
HardwareSerial sim800(1);

enum User { HOSSEIN, AMIN, EHSAN, YOUNES, UNKNOWN };

unsigned long userStartTime[4] = {0};   // Timers for each user
unsigned long userStopTime[4] = {0};
int userPresence[4] = {0};              // Presence flag for each user (0 = out, 1 = in)

MFRC522 mfrc522(SS_PIN, RST_PIN);

// RFID UIDs for each user
const String userUIDs[4] = {
    "72 71 88 51",  // Hossein
    "5D D2 FD A9",  // Amin
    "E9 89 5F B3",  // Ehsan
    "79 55 63 B3"   // Younes
};

// AdafruitIO feeds (adjust as needed)
AdafruitIO_Feed *Text = io.feed("text");    // edit
AdafruitIO_Feed *Final[4] = { io.feed("final_H"), io.feed("final_A"), io.feed("final_E"), io.feed("final_Y") };    // edit
AdafruitIO_Feed *userFeeds[4] = { io.feed("Hossein"), io.feed("Amin"), io.feed("Ehsan"), io.feed("Younes") };       // edit

String smsStatus, senderNumber, receivedDate, msg;

void setup() {
    Serial.begin(9600);
    Serial.println("ESP32 serial initialize");
    sim800.begin(BAUD_RATE, SERIAL_8N1, rxPin, txPin);
    Serial.println("SIM800L serial initialize");

    // Initialize SMS settings
    sim800.print("AT+CMGF=1\r");  // SMS text mode
    delay(1000);

    // Initialize RFID
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("Scan a RFID tag");

    // Connect to Adafruit IO
    io.connect();
    while (io.status() < AIO_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println(io.statusText());
}

void loop() {
    handleSIM800();       // Process incoming SMS
    io.run();             // Process Adafruit IO
    processRFID();        // Process RFID scans
    delay(1000);
}

// Handles incoming data from SIM800
void handleSIM800() {
    if (sim800.available()) {
        parseData(sim800.readString());
    }
    while (Serial.available()) {
        sim800.println(Serial.readString());
    }
}

// Process RFID tags
void processRFID() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        String content = getTagUID();
        User currentUser = identifyUser(content);
        
        if (currentUser != UNKNOWN) {
            handleUserEntryExit(currentUser);
        } else {
            Serial.println("Unknown user detected.");
        }
    }
}

// Gets the UID from the RFID tag
String getTagUID() {
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        content += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        content += String(mfrc522.uid.uidByte[i], HEX);
    }
    content.toUpperCase();
    Serial.println("UID tag: " + content);
    return content.substring(1);  // Remove leading space
}

// Identify user based on RFID tag
User identifyUser(const String &uid) {
    for (int i = 0; i < 4; i++) {
        if (uid == userUIDs[i]) return static_cast<User>(i);
    }
    return UNKNOWN;
}

// Handles entry/exit actions for users
void handleUserEntryExit(User user) {
    if (userPresence[user] == 0) {
        userPresence[user] = 1;
        userStartTime[user] = millis();
        sendNotification(user, "Vared Shod");
    } else {
        userPresence[user] = 0;
        userStopTime[user] = millis();
        sendNotification(user, "Kharje Shod");
        Final[user]->save(userStopTime[user] - userStartTime[user]);
    }
}

// Sends entry/exit notification via serial, SMS, and Adafruit IO
void sendNotification(User user, const String &action) {
    String userName = getUserName(user);
    String message = userName + " " + action;
    
    Serial.println(message);
    Text->save(message);
    userFeeds[user]->save(userPresence[user] == 1 ? "1" : "0");
    Reply(userName[0] + String(userPresence[user] == 1 ? " in" : " out"));
}

// Extracts user name
String getUserName(User user) {
    switch (user) {
        case HOSSEIN: return "Hossein";
        case AMIN: return "Amin";
        case EHSAN: return "Ehsan";
        case YOUNES: return "Younes";
        default: return "Unknown";
    }
}

// Parse incoming data from SIM800
void parseData(String buff) {
    Serial.println(buff);
    unsigned int index = buff.indexOf("\r");
    buff.remove(0, index + 2);
    buff.trim();

    if (buff != "OK") {
        index = buff.indexOf(":");
        String cmd = buff.substring(0, index).trim();
        buff.remove(0, index + 2);

        if (cmd == "+CMTI") {
            String temp = "AT+CMGR=" + buff.substring(buff.indexOf(",") + 1) + "\r";
            sim800.println(temp);
        } else if (cmd == "+CMGR") {
            extractSms(buff);
            if (senderNumber == PHONE) {
                doAction();
            }
        }
    }
}

// Extract SMS details
void extractSms(String buff) {
    unsigned int index = buff.indexOf(",");
    smsStatus = buff.substring(1, index - 1);
    buff.remove(0, index + 2);

    senderNumber = buff.substring(0, 13);
    buff.remove(0, 19);
    
    receivedDate = buff.substring(0, 20);
    buff.remove(0, buff.indexOf("\r"));
    buff.trim();

    msg = buff.substring(0, buff.indexOf("\n\r")).trim();
    msg.toLowerCase();
}

// Performs action based on SMS
void doAction() {
    for (int i = 0; i < 4; i++) {
        if (userPresence[i] == 1) {
            Reply(getUserName(static_cast<User>(i))[0] + " in");
        } else {
            Reply(getUserName(static_cast<User>(i))[0] + " out");
        }
    }
}

// Sends an SMS reply
void Reply(String text) {
    sim800.print("AT+CMGF=1\r");
    delay(1000);
    sim800.print("AT+CMGS=\"" + PHONE + "\"\r");
    delay(1000);
    sim800.print(text);
    delay(100);
    sim800.write(0x1A);  // Send Ctrl+Z to finish SMS
    delay(1000);
    Serial.println("SMS Sent Successfully.");
}
// ```

// ### Key Changes:
// 1. **Refactored `doAction_X()` functions** into a single `handleUserEntryExit()` that works for all users.
// 2. **Moved repeated SMS handling logic** into `sendNotification()`.
// 3. **Simplified user identification using an enum** and `userUIDs[]` for each user.
// 4. **Consolidated the RFID handling logic** into more readable functions like `getTagUID()` and `identifyUser()`.

// These changes reduce code duplication, improve readability, and make the program more maintainable.