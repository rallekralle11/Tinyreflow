//#include <EEPROM.h>

#include <TinyWireM.h>

#include <Tiny4kOLED.h>

float cal = 0.9; //change to calibrate temperature

int Vo;
float R1 = 10000;
float logR2, R2, T;
float lasttemp;
int T2;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

int target, lasttarget, conval, phase;
int fet = 255; //power sent to the heater
bool heat = 0, lastheat, test;

long sn64pb37[] {150, 60000, 160, 120000, 230, 20000};  //reflow temps and time. preheat 150C for 60000ms, soak 160 for 120000ms, reflow 230 for 20000ms
long sac305[] {150, 60000, 170, 120000, 250, 15000};  //first two are from compuphase
long Sn42Bi576Ag04[] {90, 5000, 110, 90000, 165, 5000};  //this one's based on chipquik paste
long testarray[] {30, 5000, 40, 10000, 50, 10000};

long *values;

int paste = 2;  //used to cycle between temps, starts at bismuth mix temps

long getmillis, startmillis, timer;

void setup(){
  pinMode(1, OUTPUT);

  oled.begin();
  oled.setFont(FONT8X16);
  oled.on();
  //oled.switchRenderFrame();

//  EEPROM.update(0, 150);

//  target = EEPROM.read(0);

}

void loop(){

  getmillis = millis();

  Vo = analogRead(A3);     //shamelessly stolen thermistor calculations from circuit basics
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15; 
  T = T * cal;
  
  while(analogRead(A2) > 300 & analogRead(A2) < 550){  //probably overcomplicated button-reading
    conval = 0;
    while(analogRead(A2) > 300 & analogRead(A2) < 550){ //checks if you're holding the button
      delay(20);
      if(conval == 50){  //after 50 cycles/1 sec, run this
        oled.clear();
        oled.setCursor(0, 0);
        oled.print("Settings changed to:");
        delay(500);
        paste++;
        
        if(paste == 4){  //cycle back
          paste = 0;
        }

        if(paste == 0){
          values = sn64pb37;  //set values to the right temps and times
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("Sn64 Pb37");
          delay(1000);
        }

        if(paste == 1){
          values = sac305;
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("SAC 305");
          delay(1000);
        }

        if(paste == 2){
          values = Sn42Bi576Ag04;
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("Sn42 Bi57,6 Ag\n0,6");
          delay(1000);
        }

        if(paste == 3){
          values = testarray;
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("testarray");
          delay(1000);
        }
        
        target = target + 5;  //so the value doesn't change when this runs
        oled.clear();
      }
      conval++;
    }
    target = target - 5;
  }

  while(analogRead(A2) > 600){  //more overcomplicated button-reading
    conval = 0;
    while(analogRead(A2) > 600){
      delay(20);
      if(conval == 50){
        oled.clear();
        oled.setCursor(0, 0);
        if(heat){
          oled.print("Heater disabled");
        }
        else{
          oled.print("Heater enabled");
        }
        delay(1000);
        heat = !heat;
        target = target - 5;
        oled.clear();
      }
      conval++;
    }
    target = target + 5;
  }

  if(target != lasttarget || T2 != lasttemp){  //checks if the temps or targets have changed and only then updates the screen
    oled.clear();
  }

  oled.setCursor(0, 0);
  //oled.print("temp:");
  T2 = T;                //averages the temp to an int for easier reading and fewer display updates
  //oled.print(T2);
  //oled.print("C");
  
  oled.setCursor(0, 10);
  oled.print("target:");
  oled.print(target);


  if(heat){
    if(heat > lastheat){  //if the heat was just enabled, run this
      phase = 0;
      test = 0;
      
      /*for(int i; i != fet; i++){
        analogWrite(1, i);
        delay(1);
      }*/
    }

    if(phase == 0){  //if the first phase is active
      target = values[0];  //get the starting temp and time
      timer = values[1];

    }
    if(phase == 1){
      target = values[2];  //next values
      timer = values[3];
    }
    if(phase == 2){
      target = values[4];  //next values
      timer = values[5];
    }
    if(phase == 3){  //when it gets to this phase, shut down heater and reset the counter
      phase == 0;
      heat == 0;
    }
    
    if(T == target & !test){  //start timer and make sure it only runs once
      startmillis = getmillis;
      test = 1;
    }
    if(getmillis > (startmillis + timer)){  //if the timer's run out, run this
      phase++;
      test = 0;
    }
    
   if(target > T){
    analogWrite(1, fet);  //set heating power
    oled.setCursor(0, 0);
    oled.print("H");  //for heating
   }
  }
  else{
    analogWrite(1, 0);
  }

  lasttarget = target;
  lasttemp = T2;
  lastheat = heat;
  
//  delay(20);
}
