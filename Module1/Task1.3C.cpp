// C++ code
//
const byte pirPin1 = 2;
const byte pirPin2 = 3;
const byte ledRedPin = 7;   // LED RED Pin
const byte ledGreenPin = 6; // LED GREEN Pin
volatile byte ledRedState = LOW;
volatile byte ledGreenState = LOW;
volatile byte pirStateRed = LOW;
volatile byte pirStateGreen = LOW;

void setup()
{
    Serial.begin(9600);             // Starting Serial Terminal
    pinMode(pirPin1, INPUT_PULLUP); // Set the pin for PIR1
    pinMode(pirPin2, INPUT_PULLUP); // Set the pin for PIR2
    pinMode(ledRedPin, OUTPUT);     // Set the pin for LED RED
    pinMode(ledGreenPin, OUTPUT);   // Set the pin for LED GREEN
    attachInterrupt(digitalPinToInterrupt(pirPin1), turnLEDRed, RISING);
    attachInterrupt(digitalPinToInterrupt(pirPin2), turnLEDGreen, RISING);
}

void loop()
{
    // Detecting motion to trigger interruption

    Serial.print("No motion. . . .");
    Serial.println();
    delay(500);
}

void turnLEDRed()
{
    ledRedState = !ledRedState;
    digitalWrite(ledRedPin, ledRedState);
    Serial.print("Motion Detected! RED LED!");
    Serial.println();
    delay(1000);
}

void turnLEDGreen()
{
    ledGreenState = !ledGreenState;
    digitalWrite(ledGreenPin, ledGreenState);
    Serial.print("Motion Detected! GREEN LED!");
    Serial.println();
    delay(1000);
}