// C++ code
//
const byte pirPin = 2;
const byte echoPin = 13;    // Ultrasonic Echo Pin
const byte trigPin = A0;    // Ultrasonic Trigger Pin
const byte ledRedPin = 7;   // LED RED Pin
const byte ledGreenPin = 6; // LED GREEN Pin
volatile byte ledState = HIGH;
volatile byte pirState = LOW;

void setup()
{
    Serial.begin(9600);            // Starting Serial Terminal
    pinMode(pirPin, INPUT_PULLUP); // Set the pin for PIR
    pinMode(trigPin, OUTPUT);      // Set the pin for trigger
    pinMode(echoPin, INPUT);       // Set the pin for echo
    pinMode(ledRedPin, OUTPUT);    // Set the pin for LED RED
    pinMode(ledGreenPin, OUTPUT);  // Set the pin for LED GREEN
    attachInterrupt(digitalPinToInterrupt(pirPin), turnLED, RISING);
}

void loop()
{
    long duration, distance; // Distance in cm

    // Calculating distance
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration / 29 / 2;

    // Detecting motion to trigger interruption
    pirState = digitalRead(pirPin);

    //Serial.print("No motion");
    Serial.print(distance);
    Serial.print("cm");
    Serial.println();
    digitalWrite(ledRedPin, ledState);
    digitalWrite(ledGreenPin, !ledState);
    delay(500);
}

void turnLED()
{
    ledState = !ledState;
    Serial.print("Motion Detected! Interrupted!");
    Serial.println();
    delay(2000);
}