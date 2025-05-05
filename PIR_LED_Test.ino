#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

const int CS_PIN    = 10;    // SD card chip‐select
const int LED_PIN   = 13;    // onboard LED
const int PIR_PIN   = 9;     // PIR input

RTC_PCF8523 rtc;
File     csvFile;

int  pirState   = LOW;
int  motionCount= 0;
int  lastDay    = -1;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  if (!rtc.begin()) {
    Serial.println(F("RTC not found"));
    while (1);
  }
  if (!SD.begin(CS_PIN)) {
    Serial.println(F("SD init failed"));
    while (1);
  }

  lastDay = rtc.now().day();
  createTodayCSV();
}

void loop() {
  DateTime now = rtc.now();

  // roll over at midnight
  if (now.day() != lastDay) {
    lastDay = now.day();
    createTodayCSV();
  }

  int val = digitalRead(PIR_PIN);
  if (val == HIGH) {
    digitalWrite(LED_PIN, HIGH);
    if (pirState == LOW) {
      pirState = HIGH;
      motionCount++;
      logEvent(now, "motion_start");
      Serial.print("Motion #");
      Serial.print(motionCount);
      Serial.print(" at ");
      Serial.println(formatTimestamp(now));
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    if (pirState == HIGH) {
      pirState = LOW;
      logEvent(now, "motion_end");
      Serial.print("Motion ended at ");
      Serial.println(formatTimestamp(now));
    }
  }
}

// Create MMDD.csv for today (with header) if it doesn't exist
void createTodayCSV() {
  char fileName[9];
  snprintf(fileName, sizeof(fileName), "%02d%02d.csv",
           rtc.now().month(), rtc.now().day());

  if (!SD.exists(fileName)) {
    csvFile = SD.open(fileName, FILE_WRITE);
    if (csvFile) {
      csvFile.println("timestamp,event");
      csvFile.close();
    }
  }
}

// Append one event line: "YYYY-MM-DD HH:MM:SS,event"
void logEvent(const DateTime &t, const char *event) {
  char fileName[9];
  snprintf(fileName, sizeof(fileName), "%02d%02d.csv",
           t.month(), t.day());

  csvFile = SD.open(fileName, FILE_WRITE);
  if (csvFile) {
    csvFile.print(t.year(), DEC);   csvFile.print('-');
    csvFile.print(t.month(), DEC);  csvFile.print('-');
    csvFile.print(t.day(), DEC);    csvFile.print(' ');
    csvFile.print(t.hour(), DEC);   csvFile.print(':');
    csvFile.print(t.minute(), DEC); csvFile.print(':');
    csvFile.print(t.second(), DEC); 
    csvFile.print(','); 
    csvFile.println(event);
    csvFile.close();
  } else {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}

// Helper to format Serial output
String formatTimestamp(const DateTime &t) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           t.year(), t.month(), t.day(),
           t.hour(), t.minute(), t.second());
  return String(buf);
}
