
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// String splitter (Stolen)
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
// Motor selection method. Call with zero as input to return current motor.
int setMotor(int index){
  static int motorselection;
  if(index>0){
    motorselection=index;
  }  
  return motorselection;
}
// Position saving method. 
double setPosition(double angle){
  static double pos;
  pos+=angle; 
  return pos;
}
// Com setup
void setup() {
  
  Serial.begin(9600);
  Serial.println("MINT Alignment Mirror Control \n Use motor command to select motor \n or type help for full command list");

  AFMS.begin(); 

}

// Main loop
void loop() {
  while (Serial.available() > 0) {
    int motorspeed = 150; //Angle calibrated to this speed if changed must recalibrate
    String full; // Full user input
    
    int motorselection=setMotor(0);
    //Tells user what motor is moving;
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

    // String parsing    
    full=Serial.readString();
    Serial.println(full);
    String first=getValue(full,' ',0); // Main command
    String second=getValue(full,' ',1); // Direction selection
    String third=getValue(full,' ',2); // Mirror selection
    String temp;    
    
    Adafruit_DCMotor *myMotor = AFMS.getMotor(motorselection);
    // If mirror selection is second word switch second and third word
    if(third=="left"||third =="Left"||third=="Right"||third=="right"){
      temp=second;
      second=third;
      third=temp;
    }
    // Motor selection command
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
      Serial.println("Motor selection done.");   
    }
    
    // Move command
    if(first=="move" || first=="Move"){
      if(motorselection==0){
        Serial.println("No motor selected.");      
      }
      double angle = second.toDouble(); //in ms 360 deg ~3.7s
      double cur=setPosition(angle);
      
      // Current position from starting angle print out
      Serial.print("Current position: ");
      Serial.print(cur);
      Serial.print("\n");  
      int duration = (int)(((double)angle)/360.0*3700.0);  
      myMotor->setSpeed(motorspeed);
      
      // Direction logic to have positive be inward.
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

    // Help command
    if(first=="help" || first=="Help"){
      Serial.println("Commands: \n \t move (angle in degress) - Moves previously selected motor. Positive for inward, negative for outward.");
      Serial.println("\t motor (side: left or right) (direction: pitch or yaw) - Selects which motor to move. 0.1 deg = ~ 1 ms");
    }
  }
}

