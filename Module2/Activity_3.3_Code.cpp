// C++ code
//

const byte LED_PIN = 13;
const byte METER_PIN = A4;

double timerFrequency = 0;
double val = 0;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);

  Serial.begin(9600);

  timerFrequency = 0.5;
  startTimer(timerFrequency);
}

void loop()
{
  // Code for reading the potentiometer sensor
  val = analogRead(METER_PIN); // Read the value from the sensor

  val = val / 100;     // Divide it by 100 to make the value smaller for the frequency
  Serial.println(val); // Prints the potentiometer value

  startTimer(val); // Pass the potentiometer value as the timerFrequency

  delay(1000); // Wait for 1 second
}

void startTimer(double timerFrequency)
{
  noInterrupts();

  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Do the same for TCCR1B register
  TCNT1 = 0;  // Initialize counter value to 0, max for timer1 is 65535

  // Set compare match register to the desired timer count
  OCR1A = (16000000 / (1024 * timerFrequency) - 1);

  // Compare match register for blinking every 2 second
  // is 31250 = (16*10^6 * 2) / (1*1024) - 1 (for 2 second)

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  interrupts();
}

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}
