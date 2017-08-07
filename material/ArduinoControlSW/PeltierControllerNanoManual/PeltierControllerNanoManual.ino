//==============================================================================
/*
  Filename: haptics_showroom-V02.cpp
  Project:  Haptics Showroom
  Authors:  Naina Dhingra, Ke Xu, Hannes Bohnengel 
  Revision: 0.2
  Remarks:  These files are tracked with git and are available on the github
        repository: https://github.com/hannesb0/haptics-showroom
*/
//==============================================================================

// -------------------------------------------------------------------------------
// some global variables and defines
// -------------------------------------------------------------------------------

int incomingByte                = 0;    // for incoming serial data

const int peltierEnablePin      = 2;    // digital output to enable Peltier element connected to pin 2
const int peltierDirectionPin   = 3;    // digital output to set polarization of Peltier element connected to pin 3

bool tmp[]                      = {0, 0};
int x;

String str;

int peltierIntensity         = 0;
int peltierOnOff = LOW;
int peltierDirection = LOW;

long lastChangeMillis           = 0;
long currentTime                = 0;
int interval[]                  = {600, 1000, 1400};
int period                      = 2000;

// -------------------------------------------------------------------------------




// -------------------------------------------------------------------------------
// function called at initialization of this program
// -------------------------------------------------------------------------------

void setup()
{
  Serial.begin(115200);       // opens serial port, sets data rate to 115200 bps
  
  pinMode(peltierEnablePin,  OUTPUT);
  pinMode(peltierDirectionPin,  OUTPUT);
    
  digitalWrite(peltierEnablePin,  peltierOnOff);    
  digitalWrite(peltierDirectionPin,  peltierDirection);   

  Serial.println("Program started!");
}





// -------------------------------------------------------------------------------
// main loop of this program
// -------------------------------------------------------------------------------

void loop()
{ 
  // ----------------------------------------
  // Check serial for received commands
  // ----------------------------------------
  
  if (Serial.available() > 0) 
  {
    delay(1);
    str = Serial.readStringUntil('#');
    x = Serial.parseInt();
  }
  str.trim();  





  // ----------------------------------------
  // Parse received commands: DIRECTION
  // ----------------------------------------
  
  if(str == "HOT")
  {
    peltierIntensity = x;
    digitalWrite(peltierDirectionPin, HIGH);
    peltierDirection = HIGH;
  }
  else if(str == "COLD")
  {
    peltierIntensity = x;
    digitalWrite(peltierDirectionPin, LOW);
    peltierDirection = LOW;
  }     
  else if(str == "RESET")
  {
    Serial.println("Reset"); 

    digitalWrite(peltierEnablePin, LOW);
    digitalWrite(peltierDirectionPin, LOW);

    peltierOnOff = LOW;
    peltierDirection = LOW;
    
    peltierIntensity = 0;
  }

  // Reset str
  str = "";

  // set up current time
  currentTime = millis();




  // ----------------------------------------
  // Parse received commands: INTENSITY
  // ----------------------------------------
  if(peltierIntensity == 0) 
  {
      tmp[0] = 0;
      if (tmp[1] == 0){
        Serial.println("Peltier off");
        tmp[1] = 1;
      }
      peltierOnOff = LOW;
      digitalWrite(peltierEnablePin, peltierOnOff);
  }
  else if(peltierIntensity == 1 || peltierIntensity == 2 || peltierIntensity == 3) 
  {
      tmp[1] = 0;
      if (tmp[0] == 0){
        Serial.println("Peltier on");
        tmp[0] = 1;
      }
      if(currentTime-lastChangeMillis < interval[peltierIntensity - 1])
      {
        peltierOnOff = HIGH;
      }
      if(currentTime-lastChangeMillis > interval[peltierIntensity - 1] && currentTime-lastChangeMillis < period) 
      {
        peltierOnOff = LOW;
      }
      if(currentTime-lastChangeMillis > period)
      {
        lastChangeMillis = currentTime;
      }
      
      digitalWrite(peltierEnablePin, peltierOnOff);
  }
  else if(peltierIntensity == 4) 
  {
      tmp[1] = 0;
      if (tmp[0] == 0){
        Serial.println("Peltier on");
        tmp[0] = 1;
      }
      peltierOnOff = HIGH;
      digitalWrite(peltierEnablePin, peltierOnOff);
  }
}




