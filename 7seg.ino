int red1 = 22;
int green1 = 2;
int yellow1 = 24;
int red2 = 4;
int green2 = 5;
int yellow2 = 3;
int button = 21;
int buzzer = 12;
int clockPin = 6; //clock pin of 74hc is connected to digital pin 6
int latchPin = 7; //Rclk pin of 74hc is connected to digital pin 7
int dataPin = 9; //data pin (SER) of 74hc is connected to pin 9
int digit2 = 10;
int digit1 = 11;

byte leds = 0;		// Variable to hold the pattern of which LEDs are currently turned on or off

int A = 7;
int B = 6;
int C = 5;
int D = 4;
int E = 3;
int F = 2;
int G = 1;
int H = 0;

volatile byte state = LOW;

void setup() {
  // Set all the pins of 74HC595 as OUTPUT
  pinMode(buzzer, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(digit1, OUTPUT);  
  pinMode(digit2, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), buttonISR, FALLING);

  digitalWrite(digit2, HIGH); //start with both digits off
  digitalWrite(digit1, HIGH);

  //interupt setup
  //set pins as outputs
  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

}

//timer setup for timer0, timer1, and timer2.
//For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

//this code will enable one arduino timer interrupt
//timer1 will interrupt at 1Hz

//storage variables
boolean toggle1 = 0;
int timeLeft = 46;
// Timer1's interrupt service routing (ISR)
// The code in ISR will be executed every time timer1 interrupt occurs
// That is, the code will be called once every second
// TODO
//   you need to set a flag to trigger an evaluation of the conditions
//   in your state machine, then potentially transit to next state
//
int timer = 0;
ISR(TIMER1_COMPA_vect){
  if (toggle1){
    timeLeft--;
    timer++;
    toggle1 = 0;
  }
  else{
    timeLeft--;
    timer++;
    toggle1 = 1;
  }
  if(timer >= 46){
    timer = 0;
    timeLeft = 46;
  }
}

void buttonISR(){
  state = !state;
}

void loop() {
  int i;
  if (!state){
    //turn off display
    leds = 0;
    updateShiftRegister();
    //flash red
    digitalWrite(green1, LOW);
    digitalWrite(green2, LOW);
    digitalWrite(yellow1, LOW);
    digitalWrite(yellow2, LOW);
    digitalWrite(red1, HIGH);
    digitalWrite(red2, HIGH);
    delay(1000);
    digitalWrite(red1, LOW);
    digitalWrite(red2, LOW);
    delay(1000);
  }
  else if (timer < 20 && timer > 0 ){
    if(timer == 17){
      for(i=0;i<80;i++){
        displayNumber(timeLeft - 26);
        digitalWrite(buzzer,HIGH);
        delay(1);//wait for 1ms
        digitalWrite(buzzer,LOW);
        delay(1);//wait for 1ms
      }
    }
    displayNumber(timeLeft - 26);
    state1();
  }
  else if (timer >= 20 && timer < 23){
    if(timer == 20){
      for(i=0;i<80;i++){
        displayNumber(timeLeft - 23);
        digitalWrite(buzzer,HIGH);
        delay(1);//wait for 1ms
        digitalWrite(buzzer,LOW);
        delay(1);//wait for 1ms
      }
    }
    displayNumber(timeLeft - 23);
    state2();
  }
  else if (timer >= 23 && timer < 43){
    if(timer == 40){
      for(i=0;i<80;i++){
        displayNumber(timeLeft - 3);
        digitalWrite(buzzer,HIGH);
        delay(1);//wait for 1ms
        digitalWrite(buzzer,LOW);
        delay(1);//wait for 1ms
      }
    }
    displayNumber(timeLeft - 3);
    state3();
  }
  else if (timer >= 43 && timer <= 46){
    if(timer == 43){
      for(i=0;i<80;i++){
        displayNumber(timeLeft);
        digitalWrite(buzzer,HIGH);
        delay(1);//wait for 1ms
        digitalWrite(buzzer,LOW);
        delay(1);//wait for 1ms
      }
    }
    if(timer < 46){
      displayNumber(timeLeft);
    }
    state4();
  }
}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

void displayOneDigit(int number){
  leds = 0;
  updateShiftRegister();
  if (number < 10 && number >= 0){
    switch(number){
    case (0):
      bitSet(leds, A);
      bitSet(leds, B);
      bitSet(leds, C);
      bitSet(leds, D);
      bitSet(leds, E);
      bitSet(leds, F);
      break;
    case (1):
      bitSet(leds, B);
      bitSet(leds, C);
      break;
    case(2):
      bitSet(leds, A);
      bitSet(leds, B);
      bitSet(leds, G);
      bitSet(leds, E);
      bitSet(leds, D);
      break;
    case(3):
      bitSet(leds, A);
      bitSet(leds, B);
      bitSet(leds, G);
      bitSet(leds, C);
      bitSet(leds, D);
      break;
    case(4):
      bitSet(leds, F);		// Set the bit that controls that LED in the variable 'leds'
      bitSet(leds, G);
      bitSet(leds, C);
      bitSet(leds, B);
      break;
    case(5):
      bitSet(leds, A);
      bitSet(leds, G);
      bitSet(leds, C);
      bitSet(leds, F);
      bitSet(leds, D);
      break;
    case(6):
      bitSet(leds, A);
      bitSet(leds, G);
      bitSet(leds, F);
      bitSet(leds, C);
      bitSet(leds, D);
      bitSet(leds, E);
      break;
    case(7):
      bitSet(leds, A);
      bitSet(leds, B);
      bitSet(leds, C);
      break;
    case(8):
      bitSet(leds, A);
      bitSet(leds, B);
      bitSet(leds, C);
      bitSet(leds, D);
      bitSet(leds, E);
      bitSet(leds, F);
      bitSet(leds, G);
      break;
    case(9):
      bitSet(leds, A);
      bitSet(leds, B);
      bitSet(leds, C);
      bitSet(leds, D);
      bitSet(leds, F);
      bitSet(leds, G);
      break;
  }
  updateShiftRegister();
  }
  else {
    //error
  }
}

void displayNumber(int number){
  if (number < 21 && number >= 10){
    switch (number){
      case (20):
        displayOneDigit(2);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(0);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (19):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(9);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (18):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(8);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (17):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(7);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (16):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(6);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (15):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(5);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (14):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(4);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (13):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(3);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (12):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(2);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;
      case (11):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(1);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break;   
      case (10):
        displayOneDigit(1);
        digitalWrite(digit2, LOW);
        digitalWrite(digit1, HIGH);
        delay(5);
        displayOneDigit(0);
        digitalWrite(digit2, HIGH);
        digitalWrite(digit1, LOW);
        delay(5);
        break; 
    }
  }
  else if (number < 10 && number >= 0){
    digitalWrite(digit2, HIGH);
    digitalWrite(digit1, LOW);
    switch(number){
      case(9):
        displayOneDigit(9);
        break;
      case(8):
        displayOneDigit(8);
        break;
      case(7):
        displayOneDigit(7);
        break;
      case(6):
        displayOneDigit(6);
        break;
      case(5):
        displayOneDigit(5);
        break;
      case(4):
        displayOneDigit(4);
        break;
      case(3):
        displayOneDigit(3);
        break;
      case(2):
        displayOneDigit(2);
        break;
      case(1):
        displayOneDigit(1);
        break;
      case(0):
        displayOneDigit(0);
        break;
    }
  }
  else{
    //error
  }
}

void state1(){
  digitalWrite(red1, HIGH);
  digitalWrite(yellow1, LOW);
  digitalWrite(green1, LOW);

  digitalWrite(red2, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(green2, HIGH);
}

void state2(){
  digitalWrite(red1, HIGH);
  digitalWrite(yellow1, LOW);
  digitalWrite(green1, LOW);

  digitalWrite(red2, LOW);
  digitalWrite(yellow2, HIGH);
  digitalWrite(green2, LOW);
}

void state3(){
  digitalWrite(red1, LOW);
  digitalWrite(yellow1, LOW);
  digitalWrite(green1, HIGH);

  digitalWrite(red2, HIGH);
  digitalWrite(yellow2, LOW);
  digitalWrite(green2, LOW);
}

void state4(){
  digitalWrite(red1, LOW);
  digitalWrite(yellow1, HIGH);
  digitalWrite(green1, LOW);

  digitalWrite(red2, HIGH);
  digitalWrite(yellow2, LOW);
  digitalWrite(green2, LOW);
}