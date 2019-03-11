#include "SDPArduino.h"
#include "GroveColorSensor.h"
#include <Wire.h>
#define value_for_black 400
#define value_for_white 100
int i = 0;
int red, blue, green;

unsigned long timeDetected;
unsigned long timeBlue;
#define value_for_blue 40
#define time_check 50
#define time_stop 5
#define turn_left 0
#define turn_right 1
#define go_forward 2
bool detected = false;
int state_turn = -1;

bool blue_spot = false;

void setup(){
  SDPsetup();
  helloWorld();

}

void loop(){
    GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;
  colorSensor.readRGB(&red,&green,&blue);
  Serial.println(blue);
  if(blue > value_for_blue )
  {
    blue_spot = true;
     
  }
  if(!blue_spot)
  {
    followLine();
  }
  else
  {
     motorAllStop();
     delay(2500);
     blue_spot = false;    
  }
}

void detectBlack(){
  if(readAnalogSensorData(3)>value_for_black)
  {
    state_turn = turn_right;
    timeDetected = millis();
    detected = true;
  }
  else if(readAnalogSensorData(2)>value_for_black)
  {
    state_turn = turn_left;
    timeDetected = millis();
    detected = true;
  }
  else if(readAnalogSensorData(3)<value_for_white && readAnalogSensorData(2)<value_for_white)
  {
    state_turn = go_forward;
  }
 
}
void followLine()
{
    if(!detected)
  {
    detectBlack();
    motorBackward(5, 100);
    motorBackward(3, 100);
    motorForward(2, 100);
    motorForward(4, 100);
  }
if(millis()-timeDetected < time_check && detected)
{
  switch(state_turn){
  case turn_right:
    motorBackward(5, 25);
    motorForward(4, 25);
    motorBackward(3, 100);
    motorForward(2, 100);
    break;
  case turn_left:
    motorBackward(3, 25);
    motorForward(2, 25);
    motorBackward(5, 100);
    motorForward(4, 100);
    break;
  case go_forward:
    motorBackward(5, 100);
    motorBackward(3, 100);
    motorForward(2, 100);
    motorForward(4, 100);
    break;
}
}
else
detected = false;
}
