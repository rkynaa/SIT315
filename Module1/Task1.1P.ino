// C++ code
//
const int trigPin = 13; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 12; // Echo Pin of Ultrasonic Sensor
const int ledPin = 11; // LED Pin

void setup() {
  Serial.begin(9600); // Starting Serial Terminal
  pinMode(trigPin, OUTPUT); // Set the pin for trigger
  pinMode(echoPin, INPUT); // Set the pin for echo
  pinMode(ledPin, OUTPUT); // Set the pin for LED
}

void loop() {
  long duration, distance; // Distance in cm
  
  // Calculating object distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = calculateDist(duration);
  
  // Print which area the object is in
  if (distance >= 30 && distance <= 90) {
    digitalWrite(ledPin, HIGH);
  	Serial.print("YELLOW ZONE! ");
  } else if (distance > 90) {
    digitalWrite(ledPin, LOW);
  	Serial.print("GREEN ZONE! ");
  } else {
  	Serial.print("RED ZONE! ");
  }
  
  // Printing distance
  Serial.print(distance);
  Serial.print("cm");
  Serial.println();
}

// Function to calculate distance
long calculateDist(long duration) {
  return duration / 29 / 2;
}