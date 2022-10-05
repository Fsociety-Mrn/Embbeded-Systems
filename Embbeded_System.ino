#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); //Bago ito
//Sonar Sensor 1
int Trig1 = 2;
int Echo1 = 3;

//Sonar Sensor 2
int Trig2 = 4;
int Echo2 = 5;

//float switch
int Float1 = 6; //float switch 1
int Float2 = 7; //float switch 2

//Ph sensor analog pin
int phSensor = A0;

//water pump
int waterPump = 8;

//solenoid valve
int solenoidValve = 9;

void setup() {
  Serial.begin(9600);
  lcd.init(); 
  lcd.backlight(); 

// Sonar Sensors
  pinMode(Trig1, OUTPUT);
  pinMode(Echo1, INPUT);
  pinMode(Trig2, OUTPUT);
  pinMode(Echo2, INPUT);

//float sensor
  pinMode(Float1, INPUT_PULLUP);
  pinMode(Float2, INPUT_PULLUP);

//solenoid valve
  pinMode(solenoidValve, OUTPUT);

//water pump
  pinMode(waterPump, OUTPUT);  
}

void loop() {
//PH Level
   float phLvl = phLevel();
// floats
  int flt1 = digitalRead(Float1),
      flt2 = digitalRead(Float2);
      
// tank levels
  int tank1 = tankLevel(Trig1,Echo1,flt1,21), 
      tank2 = tankLevel(Trig2,Echo2,flt2,20);

turnOff(tank1,tank2);
//LCD print  
  LCD_PRINT(
    tank1, //sonar 1
    tank2, //sonar 2
    phLvl //Ph Level
    );

}

//call this function to print something in my LCD
void LCD_PRINT(
  int sonar1,
  int sonar2,
  float phLvl
  ){

    
  lcd.clear();

//  Tank Level
  lcd.print("T1: " + String(sonar1) + "%");
  lcd.print(" ");
  lcd.print("T2: " + String(sonar2) + "%");

  lcd.setCursor(0,1);

// PH Level
  lcd.print("pH level :" + String(phLvl));
  
  lcd.setCursor(0,2);

//tank 1
  
  if (sonar1 >= 80 && sonar1 <= 100){
      lcd.print("T1 level is Good");
     
  }else if(sonar1 >= 40 && sonar1 < 80){
      lcd.print("T1 is moderate level");
  
  }else if(sonar1 >= 0 && sonar1 < 40){
      lcd.print("T1 is critical level");
  }else{
      lcd.print("Tank 1 is undefined");
  }

//tank 2
  lcd.setCursor(0,3);
  if (sonar2 >= 80 && sonar2 <= 100){
      lcd.print("T2 level is Good");
      
  }else if(sonar2 >= 40 && sonar2 < 80){
      lcd.print("T2 is moderate level");

  }else if(sonar2 >= 0 && sonar2 < 40){
      lcd.print("T2 is critical level");
  }else{
      lcd.print("Tank 2 is undefined");
  }

  delay(1000);
}

// Group 1: Tank Filteration 

//for tank level
int tankLevel(int Trig, int Echo, int level,int lvl){
  
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Echo, LOW);

//  duration
  long duration = pulseIn(Echo, HIGH);

//return an inches
    int cm = duration * 0.034 / 2;
    int inch = cm * 0.3937;
    int percent = inch*100/lvl;
    if (level){

      return 100 - percent;
    }else{ 
//      digitalWrite(sole,LOW);
      return 100;
    }
    
}

//for ph sensor
int buffer_arr[10],temp;

float phLevel(){
  float calibration_value = 21.34 + 1.80;
//    float calibration_value = 21.34;
  unsigned long int avgval = 0; 
  
  for(int i=0;i<10;i++) 
    { 
      buffer_arr[i]=analogRead(phSensor); //push the ph raw data to array
      delay(30);
    }

//ascending the raw data from ph sensor
  for(int i=0;i<9;i++)
    {
      for(int j=i+1;j<10;j++)
        {
          if(buffer_arr[i]>buffer_arr[j])
            {
              temp=buffer_arr[i];
              buffer_arr[i]=buffer_arr[j];
                buffer_arr[j]=temp;
            }
        }
      }    

// pass the raw ascending to avgVal
  for(int i=2;i<8;i++) avgval+=buffer_arr[i];

// compute the raw data into pH level
  float volt=(float)avgval*5.0/1024/6;
  float ph_act = -5.70 * volt + calibration_value;

  return ph_act;
}


//turn off Water Tank and close solenoid
void turnOff(int sonar1, int sonar2){

  if(sonar1 <= 40 || sonar1 >= 100){
    digitalWrite(waterPump,HIGH);
 
  }else{
    digitalWrite(waterPump,LOW);
  }

    if(sonar2 < 100 || sonar2 >= 40){
    digitalWrite(solenoidValve,HIGH);
 
  }else{
    digitalWrite(solenoidValve,LOW);
  }
}
