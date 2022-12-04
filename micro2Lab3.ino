//www.elegoo.com
//2020.3.12

#include "IRremote.h"
#include "IR.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define ENABLE 5
#define DIRA 3
#define DIRB 4
#define button 31

IRrecv irrecv(RECEIVER);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();

  // Initialize DS3231
  clock.begin();
  
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2016, 12, 9, 11, 46, 00);
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__); 

  lcd.begin(16, 2);

  pinMode(ENABLE,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);
  pinMode(button, INPUT);

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


int direction = 0;
int power = 0;
int speed = 255;
int speedLevel = 2;

volatile byte state = LOW;

ISR(TIMER1_COMPA_vect){
  state = !state;
}

void loop()
{
  int tmpValue;
  // update time on LCD
  if(!state){
    if(digitalRead(button)==HIGH){
      direction++;
    }
    dt = clock.getDateTime();
    lcd.setCursor(0, 0);
    lcd.print(dt.hour); lcd.print(":");
    lcd.print(dt.minute); lcd.print(":");
    lcd.print(dt.second); lcd.print("     ");

    lcd.setCursor(10, 0);
    switch(speedLevel){
      case 4: lcd.print("FULL "); break;
      case 3: lcd.print("3/4  "); break;
      case 2: lcd.print("1/2  "); break;
      case 1: lcd.print("OFF  "); break;
    }
    delay(50);
  }

  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    for (int i = 0; i < 23; i++)
    {
      if ((keyValue[i] == results.value) && (i<KEY_NUM))
      {
        Serial.println(keyBuf[i]);
        tmpValue = results.value;
        switch(tmpValue){
          case 0xFFA25D:  power++; break; //POWER
          case 0xFFE21D:  break; //FUNC/STP
          case 0xFF629D:  break; //vol+
          case 0xFF22DD:  speed= speed - 10;    break; //fast back
          case 0xFF02FD:  break; //pause
          case 0xFFC23D:  speed= speed + 10;  break; //fast forward
          case 0xFFE01F:  direction++;   break; //down
          case 0xFFA857:  break; //vol-
          case 0xFF906F:  direction++;   break;
          default: Serial.println(" other button   ");
        }
      }
      else if(REPEAT==i)
      {
        results.value = tmpValue;
      }
    }
    irrecv.resume(); // receive the next value
  }

  if(state){
    if(digitalRead(button)==HIGH){
      direction++;
    }
    dt = clock.getDateTime();
    lcd.setCursor(0, 0);
    lcd.print(dt.hour); lcd.print(":");
    lcd.print(dt.minute); lcd.print(":");
    lcd.print(dt.second); lcd.print("     ");

    lcd.setCursor(10, 0);
    switch(speedLevel){
      case 4: lcd.print("FULL "); break;
      case 3: lcd.print("3/4  "); break;
      case 2: lcd.print("1/2  "); break;
      case 1: lcd.print("OFF  "); break;
    }
    delay(50);
  }

  if (speed > 255){
    speed = 255;
  }
  if (speed < 0){
    speed = 0;
  }
    //upate motor
    switch(power%2){
      case 0: 
      digitalWrite(ENABLE, LOW);
      speedLevel = 1;
      break;
      case 1: 
      if (speed > 183){
        digitalWrite(ENABLE, HIGH);
        speedLevel = 4;
      }
      else if(speed > 122){
        analogWrite(ENABLE, speed);
        speedLevel = 3;
      }
      else if(speed > 61){
        analogWrite(ENABLE, speed);
        speedLevel = 2;
      }
      else {
        digitalWrite(ENABLE, LOW);
        speedLevel = 1;
      }
      break;
    }


    switch(direction%2){
      //turn motor direction and print direction on LCD
      case 0: 
      digitalWrite(DIRA,HIGH); 
      digitalWrite(DIRB,LOW); 
      lcd.setCursor(0, 1); 
      lcd.print("Direction: CW  ");
      break;
      case 1: 
      digitalWrite(DIRA,LOW);
      digitalWrite(DIRB,HIGH);
      lcd.setCursor(0, 1); 
      lcd.print("Direction: CCW");
      break;
    }
}

//get IR data


