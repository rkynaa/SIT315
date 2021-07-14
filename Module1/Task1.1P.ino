// C++ code
//
const int pirPin = 2; // PIR Pin
const int ledPin = 7; // LED Pin
volatile byte ledState = HIGH;
volatile byte pirState = LOW;

void setup() {
  Serial.begin(9600); // Starting Serial Terminal
  pinMode(pirPin, INPUT_PULLUP); // Set the pin for PIR
  pinMode(ledPin, OUTPUT); // Set the pin for LED
}

void loop() {
  pirState = digitalRead(pirPin);
  Serial.print("No motion");
  Serial.println();
  if (pirState == HIGH) {
    ledState = !ledState;
    Serial.print("Motion detected!");
  	Serial.println();
  }
  digitalWrite(ledPin, ledState);
  delay(1000);
}