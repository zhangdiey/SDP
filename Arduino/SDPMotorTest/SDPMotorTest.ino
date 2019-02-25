#include "SDPArduino.h"
#include "GrouveColorSensor.h"
#include <Wire.h>
#define GOING 3
#define OBSTRUCTED 1
#define HALTED 0
#define OBSTRUCTED_CLEARING 2
#define TURNING 4

#define value_for_black
#define value_for_white
#define value_for_red
#define value_for_green
#define value_for_blue

#define value_for_blue_upper
#define value_for_blue_lower

#define left 'a'
#define right 'c'
#define forward 'f'
#define backward 'b' //it is actually for turn 180 degrees

unsigned long timeUntilThresholdDistance = 3000;
int trigpin = 3;
int echopin = A3;
long duration;

GroveColorSensor colorSensor;
colorSensor.ledStatus = 1;

unsigned long obstructionTilHalt = 10000;
unsigned long timeTilClear = 700;
int state = HALTED;
unsigned long timeOfObstruction;
unsigned long timeOfClearing;

int red, green, blue;

char turning_state = forward;

void setup(){
  SDPsetup();
  delay(3000);
  helloWorld();
}



void loop(){
  switch(state){
    case GOING:
      if(obstructed()){
        halt();
        state=OBSTRUCTED;
        timeOfObstruction=millis();
        Serial.write('o');
      }
      else{
        detectSpot();
        followLine();
      }
      break;
    case OBSTRUCTED:
      if(obstructed()){
        if(millis()-timeOfObstruction > obstructionTilHalt){
          state=HALTED;
          Serial.write('O');
        }
      }
      else{
        timeOfClearing = millis();
        state=OBSTRUCTED_CLEARING;
      }
      break;
    case OBSTRUCTED_CLEARING:
        if(obstructed()){
          state=OBSTRUCTED;
        }
        else if(millis()-timeOfClearing>timeTilClear ){
          state=GOING;
          Serial.write('c');
        }
        break;
     case HALTED: 
        break;
     case TURNING;
       turn(); 
      
  }
}
int obstructed(){
    for (int attempt = 0;attempt<1;attempt++){
      digitalWrite(trigpin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigpin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigpin, LOW);
      if (pulseIn(echopin, HIGH, timeUntilThresholdDistance)){
        return true;
      }
    }
   return false;
}
void halt(){
 motorAllStop();
}
void detectSpot(){
  GroveColorSensor colorSensor;
  colorSensor.ledStatus = 1;
  colorSensor.readRGB(&red, &green, &blue);
  Serial.write('red:', red);
  Serial.write('green:', green);
  Serial.write('blue:', blue)
}
  
void followLine(){
  if(readAnalogSensordata(3)>value_for_black)
  {
    motorBackward(2, 20);
    motorForward(3, 20);
  }
  else if(readAnalogSensordata(2)>value_for_black)
  {
     motorBackward(4, 20);
     motorForward(5, 20);
  }
  else if(readAnalogSensordata(3)<value_for_white && readAnalogSensordata(2)<value_for_white)
  {
    motorBackward(2, 100);
    motorBackward(4, 100);
    motorForward(5, 100);
    motorForward(3, 100);
  }
 
}

void detectStop()
{
  colorSensor.readRGB(&red, &green, &blue);
  if(red < color_for_red && green < color_for_green && blue > color_for_blue)
  {
    Serial.write('n');
    state = TURNING;
  }
}

void serialEvent(){
  switch(Serial.read()){
    case 'g':
      state=GOING;
      Serial.println("state = going");
      break;
    case 'h':
      halt();
      state=HALTED;
      Serial.println("state = halted");
      break;
    case 'a':
      turning_state = 'a';
      break;
    case 'b':
      turning_state = 'b':
      break;
    case 'c':
      turning_state = 'c';
      break;
    case 'f':
      turning_state = 'f';
      break;            
  }
}

void turn()
{
   switch(turning_state)
   {
     case 'a':
       
   }
}

void turn_left()
{
  if(readAnalogSensordata(3)>value_for_black)
{
  if(readAnalogSensordata(2)>value_for_black)
  {
     motorBackward(4, 20);
     motorForward(5, 20);
  }  
}  
  else if(readAnalogSensordata(3)<value_for_white)
    state = GOING;
}

void turn_right()
{
  if(readAnalogSensordata(2)>value_for_black)
  {
    if(readAnalogSensordata(3)>value_for_black)
  {
    motorBackward(2, 20);
    motorForward(3, 20);
  }
  }
  else if(readAnalogSensordata(2)<value_for_white)
    state = GOING;  
}

void forward()
{
  if(readAnalogSensorData(3)>value_for_blue_lower && readAnalogSensorData(3)<value_for_blue_upper)
  {
    motorBackward(2, 20);
    motorForward(3, 20);
  }
  else if(readAnalogSensorData(2)>value_for_blue_lower && readAnalogSensorData(2)<value_for_blue_upper)
  {
     motorBackward(4, 20);
     motorForward(5, 20);
  }
  else if(readAnalogSensorData(2)<value_for_blue_lower || readAnalogSensorData(2)>value_for_blue_upper)
  {
    motorBackward(2, 100);
    motorBackward(4, 100);
    motorForward(5, 100);
    motorForward(3, 100);
  }
  else if(readAnalogSensorData(2)<value_for_white && readAnalogSensorData(3)>value_for_white)
  {
     motorBackward(4, 20);
     motorForward(5, 20);
  }
  else if(readAnalogSensorData(3)<value_for_white && readAnalogSensorData(3)>value_for_white)
  {
     motorBackward(2, 20);
     motorForward(3, 20);
  }
}

void backward()
{
  turn_right();
  turn_right_reverse();
}
  


