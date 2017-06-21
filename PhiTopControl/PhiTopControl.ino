
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
double setPosition(double angle, int aIndex){
  static double pos1;
  static double pos2;
  static double pos3;
  if (aIndex==1){
    pos1+=angle; 
    return pos1;
  }
  if (aIndex==2){
    pos2+=angle; 
    return pos2;
  }
  if (aIndex==3){
    pos3+=angle; 
    return pos3;
  }
}
// Motor speed method. Call with zero as input to return current speed.
int setMotorSpeed(int spd){
  static int sped;
  if(spd>0){
    sped=spd;
  }  
  return sped;
}
// Com setup
void setup() {
  
  Serial.begin(9600);
  Serial.println("MINT Phi Top Control /n Use axis command to select axis /n or type help for full command list");
  setMotorSpeed(150);
  AFMS.begin(); 

}

// Main loop
void loop() {
  while (Serial.available() > 0) {
    int motorspeed = setMotorSpeed(0); 
    String full; // Full user input
    
    int motorselection=setMotor(0);
    //Tells user what motor is moving;
    if(motorselection==1){
      Serial.println("Moving Phi-axis.");
    }
    else if(motorselection==2){
      Serial.println("Moving X-axis.");
    }
    else if(motorselection==3){
      Serial.println("Moving Y-axis.");
    }

    // String parsing    
    full=Serial.readString();
    Serial.println(full);
    String first=getValue(full,' ',0); // Main command
    String second=getValue(full,' ',1); // Axis selection
    String temp;    
    
    Adafruit_DCMotor *myMotor = AFMS.getMotor(motorselection);

    // Speed set command
    if(first=="speed" || first=="Speed"){
      if(second==""){
        Serial.print("Current speed: ");
        Serial.print(setMotorSpeed(0));
        Serial.print("\n");
      }
    }
    
    // Motor selection command
    else if(first=="axis" || first=="Axis"){
      if(second=="phi" || second=="Phi"){
        motorselection=setMotor(1);        
        Serial.println("Axis selection done."); 
      }
      else if(second=="x" || second=="X"){
        motorselection=setMotor(2);        
        Serial.println("Axis selection done."); 
      }
      else if(second=="y" || second=="Y") {
        motorselection=setMotor(3);        
        Serial.println("Axis selection done."); 
      }
      else if(second=="" || second=="") {        
        if(motorselection==1){
          Serial.println("Current axis: Phi");
        }
        else if(motorselection==2){
          Serial.println("Current axis: X");
        }
        else if(motorselection==3){
          Serial.println("Current axis: Y");
        } 
      }
      else{
        Serial.println("Input error.");  
      }
    }
    
    // Move command
    else if(first=="move" || first=="Move"){
      if(motorselection==0){
        Serial.println("No motor selected.");      
      }
      int duration = second.toDouble();
      double cur=setPosition(duration,motorselection);
      
      // Current position from starting angle print out
      Serial.print("Current position (ms): ");
      Serial.print(cur);   
      
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
    else if(first=="help" || first=="Help"){
      Serial.println("Commands: \n \t move (time in ms) - Moves previously selected axis.");
      Serial.println("\t axis (phi, x, y) - Selects which axis to move.");
      Serial.println("\t speed (speed 0 to 255) - Sets speed of the motor.");
    }
    else{
        Serial.println("Input error.");  
    }
  }
}

