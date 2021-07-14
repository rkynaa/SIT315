// C++ code
//
const int pirPin = 2; // PIR Pin
const int ledPin = 7; // LED Pin
volatile byte ledState = HIGH;
volatile byte pirState = LOW;

void setup()
{
    Serial.begin(9600);            // Starting Serial Terminal
    pinMode(pirPin, INPUT_PULLUP); // Set the pin for PIR
    pinMode(ledPin, OUTPUT);       // Set the pin for LED
    attachInterrupt(digitalPinToInterrupt(pirPin), turnLED, CHANGE);
}

void loop()
{
    pirState = digitalRead(pirPin);
    Serial.print("No motion");
    Serial.println();
    digitalWrite(ledPin, ledState);
    delay(1000);
}

void turnLED()
{
    ledState = !ledState;
    Serial.print("Interrupted!");
    Serial.println();
    delay(1000);
}