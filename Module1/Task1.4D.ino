// C++ code
//
const byte pirPin1 = 8;
const byte pirPin2 = 10;
const byte ledRedPin = 7; // LED RED Pin
const byte ledGreenPin = 6; // LED GREEN Pin
volatile byte ledRedState = LOW;
volatile byte ledGreenState = LOW;
volatile byte pirStateRed = LOW;
volatile byte pirStateGreen = LOW;

void setup()
{
  Serial.begin(9600);            // Starting Serial Terminal
  PCICR |= B00000001;
  PCMSK0 |= B00000101;
  pinMode(pirPin1, INPUT_PULLUP); // Set the pin for PIR1
  pinMode(pirPin2, INPUT_PULLUP); // Set the pin for PIR2
  pinMode(ledRedPin, OUTPUT);       // Set the pin for LED RED
  pinMode(ledGreenPin, OUTPUT);       // Set the pin for LED GREEN
  startTimer();
}

void loop()
{
  
  Serial.print("No motion. . . .");
  Serial.println();
  digitalWrite(ledRedPin, ledRedState);
  digitalWrite(ledGreenPin, ledGreenState);
  delay(500);
}


void startTimer(){
  noInterrupts();
  TCCR1A = 0;  //Reset Timer1 control Registor A

  bitClear(TCCR1B, WGM13); //Set CTC mode
  bitSet(TCCR1B, WGM12);


  bitSet(TCCR1B, CS12); //Set prescaler to 1024
  bitClear(TCCR1B, CS11);
  bitSet(TCCR1B, CS10);
  
  TCNT1 = 0;
  
  // To calculate the timer
  // OCR1A = (time(s) * clock Freq.)/prescaler
  // OCR1A = (1*16*10^6)/1024
  
  //OCR1A = 15625;   //for 1sec
  OCR1A = 2*16000000/1024;   //for 2sec
  
  
  bitSet(TIMSK1, OCIE1A); // Enable Timer1 compare interrupt
  sei();                  // Enable global interrupts
  interrupts();
}

ISR(PCINT0_vect){
  if (digitalRead(pirPin1) != pirStateGreen){
    Serial.print("Motion Detected! GREEN!\n");
  	ledGreenState = !ledGreenState;
  }
  else if (digitalRead(pirPin2) != pirStateRed){
    Serial.print("Motion Detected! RED!\n");
  	ledRedState = !ledRedState;
  }
}

ISR(TIMER1_COMPA_vect){
  ledRedState = !ledRedState;
  ledGreenState = !ledGreenState;
}