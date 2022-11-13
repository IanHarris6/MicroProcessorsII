#include <Wire.h>
#include <MPU6050.h>

// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = A0; // analog pin connected to X output
const int Y_pin = A1; // analog pin connected to Y output

int buzzer = 12;//the pin of the active buzzer

int i;

int prevZ = 0;
int deltaZ;

MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);

  pinMode(buzzer,OUTPUT);//initialize the buzzer pin as an output

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  //checkSettings();
}

void loop() {
  // put your main code here, to run repeatedly
  int incomingByte = 0x01;
   if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    Serial.println(incomingByte);

    if(incomingByte == 'E'){
      printJoystickAndAccell();
    }

    if(incomingByte == 'K'){
      for(i=0;i<80;i++)
        {
          digitalWrite(buzzer,HIGH);
          delay(1);//wait for 1ms
          digitalWrite(buzzer,LOW);
          delay(1);//wait for 1ms
        }

    }
   }
}

void printJoystickAndAccell(){
  Vector rawAccel = mpu.readRawAccel();
  Vector normAccel = mpu.readNormalizeAccel();

 
  Serial.print("start ");
  Serial.print(normAccel.XAxis);
  //Serial.print(" \n");
  Serial.print(" ");
  Serial.print(normAccel.YAxis);
  //Serial.print(" \n");
  Serial.print(" ");
  Serial.print(normAccel.ZAxis);

  Serial.print(" ");
  Serial.print(analogRead(X_pin));
  //Serial.print(" \n");
  Serial.print(" ");
  Serial.print(analogRead(Y_pin));

  deltaZ = normAccel.ZAxis - prevZ;

  Serial.print(" ");
  Serial.print(deltaZ);




  Serial.print(" \n");

  
  
  prevZ = normAccel.ZAxis;


}


void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Clock Source:          ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:         ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());
  
  Serial.println();
}