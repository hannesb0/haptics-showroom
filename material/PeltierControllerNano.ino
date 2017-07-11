#include <Servo.h>

/* -------------------------------------------------------------------------------
 * 
 * 
 * 
 * 
 * 
 * To Do:
 * 
 * - Change name of peltierDirection
 * - Remove Servo.h
 * - Set up intensity ?!
 * - Implement 2-point control mechanism
 * 
 * 
 * -------------------------------------------------------------------------------*/





// -------------------------------------------------------------------------------
// some global variables and defines
// -------------------------------------------------------------------------------

int incomingByte                = 0;    // for incoming serial data

const int analogPin             = 0;    // middle terminal of temperature sensor connected to analog pin 3
                                        // outside terminal connected to ground and +5V

const int peltierEnablePin      = 2;    // digital output to enable Peltier element connected to pin 2
const int peltierDirectionPin   = 3;    // digital output to set polarization of Peltier element connected to pin 3

int val                         = 1;    // this variable stores the read analog value
int val_old                     = 0;

bool tmp[]                      = {0, 0};
int x;
bool inc = false;
const int Tcold = 560, Thot = 660;
const int Thys = 5;
int T;

String str;

int recPeltierIntensity         = 0;
int peltierOnOff = LOW;
int peltierDirection = LOW;

long lastChangeMillis           = 0;
long currentTime                = 0;
int interval[]                  = {1000, 500, 200};

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

  T = (Thot + Tcold)/2;
}





// -------------------------------------------------------------------------------
// function for printing debug stuff
// -------------------------------------------------------------------------------

void debugPrint (void) {
  Serial.print("TempIs: ");
  Serial.println(val);
  Serial.print("TempSoll : ");
  Serial.println(T);
  Serial.print("Dir: ");
  if (peltierDirection == LOW)
    Serial.println("Cooling");
  else
    Serial.println("Heating");
  //(peltierDirection == LOW) ? Serial.println("Cooling") : Serial.println("Heating");
  Serial.print("Pwr: ");
  if (peltierOnOff == LOW)
    Serial.println("Off");
  else
    Serial.println("On");
  //(peltierOnOff == LOW) ? Serial.println("Off") : Serial.println("On");
  Serial.print("inc = ");
  Serial.println(inc);
  Serial.print("\n");
}





// -------------------------------------------------------------------------------
// main loop of this program
// -------------------------------------------------------------------------------

void loop()
{ 

  // ----------------------------------------
  // Read the temperature
  // ----------------------------------------
 
  val = analogRead(analogPin);    // read the input pin





  // ----------------------------------------
  // Print status
  // ----------------------------------------
  
  if (val >= val_old) {
    if (val - val_old > 3) {
      debugPrint();
      val_old = val;
    }
  }
  else if (val < val_old) {
    if (val_old - val > 3) {
      debugPrint();
      val_old = val;
    }
  }





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
    T = Thot;
    recPeltierIntensity = x;
    digitalWrite(peltierDirectionPin, HIGH);
    peltierDirection = HIGH;

  }
  else if(str == "COLD")
  {
    T = Tcold;
    recPeltierIntensity = x;
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
    
    recPeltierIntensity = 0;
  }

  // Reset str
  str = "";

  // set up current time
  //currentTime = millis();



  // ----------------------------------------
  // Set hysterisis direction
  // ----------------------------------------

  if (val >= (T + Thys)) {
    inc = false;
  }
  else if (val <= (T - Thys)) {
    inc = true;
  }





  // ----------------------------------------
  // Parse received commands: INTENSITY
  // ----------------------------------------

  if (recPeltierIntensity == 1) 
  {
      tmp[1] = 0;
      if (tmp[0] == 0){
        Serial.println("Peltier on");
        tmp[0] = 1;
      }


      if (peltierDirection == HIGH) {
        if (inc) {
          digitalWrite(peltierEnablePin, HIGH);
        }
        else {
          digitalWrite(peltierEnablePin, LOW);
        }
      }
      if (peltierDirection == LOW) {
        if (inc) {
          digitalWrite(peltierEnablePin, LOW);
        }
        else {
          digitalWrite(peltierEnablePin, HIGH);
        }
      }

      
  }
 
  else if (recPeltierIntensity == 0) 
  {
      tmp[0] = 0;
      if (tmp[1] == 0){
        Serial.println("Peltier off");
        tmp[1] = 1;
      }
      digitalWrite(peltierEnablePin, LOW);
  }


  /*

  // ----------------------------------------
  // Parse received commands: INTENSITY
  // ----------------------------------------

  if(recPeltierIntensity == 1) 
  {
      tmp[1] = 0;
      if (tmp[0] == 0){
        Serial.println("Peltier on");
        tmp[0] = 1;
      }
      if((currentTime-lastChangeMillis > interval[recPeltierIntensity])) 
      {
        
        lastChangeMillis = currentTime;
        
        if(peltierOnOff == LOW) 
        {
          peltierOnOff = HIGH;
          //Serial.println("Peltier on"); 
        } 
        else if (peltierOnOff == HIGH) 
        {
          peltierOnOff = LOW;
          //Serial.println("Peltier off"); 
        }
        
        digitalWrite(peltierEnablePin, peltierOnOff);
        
        lastChangeMillis = currentTime;
      }
  }
  else if(recPeltierIntensity == 2) 
  {
      if((currentTime-lastChangeMillis > interval[recPeltierIntensity]))
      {

        lastChangeMillis = currentTime;
        
        if(peltierOnOff == LOW) {
          peltierOnOff = HIGH;
        } else if (peltierOnOff == HIGH) {
          peltierOnOff = LOW;
        }
        
        digitalWrite(peltierEnablePin, peltierOnOff);
        
        lastChangeMillis = currentTime;
      }
  }
  else if(recPeltierIntensity == 3) {

      if((currentTime-lastChangeMillis > interval[recPeltierIntensity])) {
        
        lastChangeMillis = currentTime;
        if(peltierOnOff == LOW) {
          peltierOnOff = HIGH;
        } else if (peltierOnOff == HIGH) {
          peltierOnOff = LOW;
        }
        
        digitalWrite(peltierEnablePin, peltierOnOff);

        lastChangeMillis = currentTime;
      }
  }
  else if(recPeltierIntensity == 4) 
  {
      peltierOnOff = HIGH;
      digitalWrite(peltierEnablePin, peltierOnOff);
  }
  else if(recPeltierIntensity == 0) 
  {
      tmp[0] = 0;
      if (tmp[1] == 0){
        Serial.println("Peltier off");
        tmp[1] = 1;
      }
      peltierOnOff = LOW;
      digitalWrite(peltierEnablePin, peltierOnOff);
  }

  */
  
}




