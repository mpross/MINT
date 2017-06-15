
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int setMotor(int index){
  static int motorselection;
  if(index>0){
    motorselection=index;
  }  
  return motorselection;
}

void setup() {
  
  Serial.begin(9600);
  Serial.println("initialized");

  AFMS.begin(); 

}


void loop() {
  while (Serial.available() > 0) {
    int motorspeed = 150;
    String full;
    
    int motorselection=setMotor(0);
    if(motorselection==1){
      Serial.println("Moving motor left pitch.");
    }
    else if(motorselection==2){
      Serial.println("Moving motor left yaw.");
    }
    else if(motorselection==3){
      Serial.println("Moving motor right pitch.");
    }
    else if(motorselection==4){
      Serial.println("Moving motor right yaw.");
    }

        
    full=Serial.readString();
    Serial.println(full);
    String first=getValue(full,' ',0);
    String second=getValue(full,' ',1);
    String third=getValue(full,' ',2);
    
    int rotationaldirection = 1; //1 for forward, -1 for reverse
    
    Adafruit_DCMotor *myMotor = AFMS.getMotor(motorselection);

    if(first=="motor" || first=="Motor"){
      if((second=="left" || second=="Left") && (third=="pitch" || third=="Pitch")){
        motorselection=setMotor(1);
      }
      else if((second=="left" || second=="Left") && (third=="yaw" || third=="Yaw")){
        motorselection=setMotor(2);
      }
      else if((second=="right" || second=="Right") && (third=="pitch" || third=="Pitch")){
        motorselection=setMotor(3);
      }
      else if((second=="right" || second=="Right") && (third=="yaw" || third=="Yaw")){
        motorselection=setMotor(4);
      }
      Serial.println("Done");   
    }
    if(first=="move" || first=="Move"){
      if(motorselection==0){
        Serial.println("No motor selected.");      
      }
      double angle = second.toDouble(); //in ms 360 deg ~3.7s  
      int duration = (int)(((double)angle)/360.0*3700.0);   
      myMotor->setSpeed(motorspeed);
      if (duration>0)
      {
        myMotor->run(BACKWARD);
        delay(duration);
        myMotor->run(RELEASE);
      }
      if (duration < 0)
      {
        myMotor->run(FORWARD);
        delay(-duration);
        myMotor->run(RELEASE);
      }
      Serial.println("Done");     
    }
    if(first=="help" || first=="Help"){
      Serial.println("Commands: \n \t move (angle in degress) - Moves previously selected motor. Positive for clockwise, negative for counter clockwise.");
      Serial.println("\t motor (side: left or right) (direction: pitch or yaw)");
    }
  }
}

