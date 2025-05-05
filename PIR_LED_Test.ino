/*
 * PIR sensor tester with event counter
 */
 
int ledPin       = 13;      // LED pin
int inputPin     = 9;       // PIR sensor pin
int pirState     = LOW;     // current PIR state
int val          = 0;       // reading from PIR
int motionCount  = 0;       // counter

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);
  Serial.begin(9600);
}

void loop(){
  val = digitalRead(inputPin);
  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);
    if (pirState == LOW) {
      // motion started
      motionCount++;                                  
      Serial.print("Motion detected! count = ");      
      Serial.println(motionCount);                    
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW);
    if (pirState == HIGH){
      // motion ended
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
}
