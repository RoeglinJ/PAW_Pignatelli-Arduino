/////////////////////////////////////////////////////////////////////////////////////
/*
 * PIR counter test sketch
 * With jumper set to "L" and both potentiometers set to most counter-clockwise position, motion should re-trigger after Tx = ~1.1s and Ti = ~4.5s.
 * Thus, a max of ~11.1 triggers per minute should be possible. Accordingly, data should be trimmed past 11 motions/minute to remove artifacts. 
 * Per neufeld.newton.ks.us/electronics/?p=208, a warm-up period of ~1 minute wherein "random" triggerings occur may be necessary. In my experience, this period lasted between 20 s and 5 min. 
 */
 
/////////////////////////////////////////////////////////////////////////////////////

// packages
#include <RTClib.h>
#include <SD.h>

#define CS_PIN 10
static bool WRITE_DATA = true;

// initialize RTC
RTC_PCF8523 rtc;

// track the date within the script rather than using SD.exists() to check
uint8_t lastDay;

// pulse gating and timing
bool hasLoggedThisPulse = false;
uint32_t    prevRiseTime       = 0;   // timestamp of prior pulse trigger
uint32_t    prevPulseDuration  = 0;   // Tx (prior pulse duration of pirPin = HIGH after trigger)
uint32_t    prevIntervalSec    = 0;   // Ti_observed (elapsed time between prior pulse trigger and current pulse trigger)

// initialize storage variables and sensor pins
int ledPin       = 13;            // LED pin
int pirPin       = 9;             // PIR pin
int motionCount  = 0;             // motion event counter
char filename[]  = "00000.CSV";   // char vector for .csv filename
char timeBuffer[9];               // space for "HH:MM:SS"


/////////////////////////////////////////////////////////////////////////////////////

// Format filename as MMDD.csv
void getFileName(DateTime now) {
  sprintf(filename, "%02d%02d.csv", now.month(), now.day());
}

// Event logging function
void logEvent(){
  DateTime now = rtc.now();

  if(now.day() != lastDay){
    lastDay = now.day();
    getFileName(now);
    File hdr = SD.open(filename, FILE_WRITE);
    if(hdr){
      hdr.println("PIR_event,time,Tx,Ti_observed");
      hdr.close();
    }
    motionCount = 1;  // reset counter for the new day
  }

  File data = SD.open(filename, FILE_WRITE);
  if(!data){
    Serial.print("Error opening ");
    Serial.println(filename);
    return;
  }
  data.print(motionCount);
  data.print(',');
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  data.print(timeBuffer);
  data.print(',');
  data.print(prevPulseDuration / 1000.0, 3);
  data.print(',');
  data.println(prevIntervalSec / 1000.0, 3);
  data.close();

  Serial.print("Logged PIR event ");
  Serial.print(motionCount);
  Serial.print(" @ ");
  Serial.print(timeBuffer);
  Serial.print("  Tx=");
  Serial.print(prevPulseDuration / 1000.0, 3);
  Serial.print("s  Ti=");
  Serial.print(prevIntervalSec / 1000.0, 3);
  Serial.println("s");
}

/////////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  delay(2000);  // give serial time to connect

  // Initialize RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) delay(10);
  }
  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC not initialized; setting time to compile time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();

  // Initialize SD
  if (! SD.begin(CS_PIN)) {
    Serial.println("SD failed to initialize. Please re-insert SD and reset.");
    while (1) delay(10);
  }

  DateTime now = rtc.now();
  lastDay = now.day();
  getFileName(now);

  File f = SD.open(filename, FILE_WRITE);
    if (! f) {
      Serial.print("Error creating ");
      Serial.println(filename);
      while (1) delay(10);
    }
    f.println("PIR_event,time,Tx,Ti_observed");
    f.close();

    Serial.print("Logging to: ");
    Serial.println(filename);

}

/////////////////////////////////////////////////////////////////////////////////////
/*
void loop() {
  int state = digitalRead(pirPin);
  Serial.print("PIR pin reads: ");
  if (state == HIGH) {
    Serial.println("HIGH");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("LOW");
    digitalWrite(ledPin, LOW);
  }
  delay(10);  // adjust to slow or speed up the print rate
}
*/

///*
void loop() {
  //Serial.println("→ loop start"); // for debugging purposes
  int reading = digitalRead(pirPin);
  digitalWrite(ledPin, reading ? HIGH : LOW);         // LED mirrors PIR

  if (reading == HIGH && !hasLoggedThisPulse) {
    uint32_t nowMs = millis();
    if (prevRiseTime > 0) {
      prevIntervalSec = nowMs - prevRiseTime;
    } else {
      prevIntervalSec = 0;
    }
    prevRiseTime = nowMs;

    hasLoggedThisPulse = true;
    motionCount++;
    logEvent();
  }
  
  else if (reading == LOW && hasLoggedThisPulse) {
    uint32_t nowMs = millis();
    prevPulseDuration = nowMs - prevRiseTime;
    hasLoggedThisPulse = false;
  //Serial.println("← loop end"); // for debugging purposes
  }
}
//*/

