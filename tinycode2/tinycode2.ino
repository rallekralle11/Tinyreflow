#include <SmoothThermistor.h>

#include <TinyWireM.h>
#include <USI_TWI_Master.h>

#include <Tiny4kOLED.h>

float T;
float lasttemp;

int cal = 1;
int avgtemp;

int paste = 0;
int stage = 0;

bool heat = 0;

long startmillis;
long targetmillis;

SmoothThermistor therm(A3, ADC_SIZE_10_BIT, 100000, 100000, 3950, 25, 5);


void setup(){
  pinMode(1, OUTPUT);

  oled.begin();
  oled.setFont(FONT8X16);
  oled.on();


//SmoothThermistor smoothThermistor(A0,              // the analog pin to read from
//                                   ADC_SIZE_10_BIT, // the ADC size
//                                   10000,           // the nominal resistance
//                                   10000,           // the series resistance
//                                   3950,            // the beta coefficient of the thermistor
//                                   25,              // the temperature for nominal resistance
//                                   10);             // the number of samples to take for each measurement
  
}

void loop(){

 T = therm.temperature();

  avgtemp = T;
  
  if(avgtemp != lasttemp){  //checks if the temps or targets have changed and only then updates the screen
    oled.clear();
  }

  oled.setCursor(0, 0);
  oled.print(avgtemp);
  
  while(analogRead(A2) > 300 & analogRead(A2) < 550){  //probably overcomplicated button-reading
    while(analogRead(A2) > 300 & analogRead(A2) < 550){ //checks if you're holding the button
        oled.clear();
        oled.setCursor(0, 0);
        oled.print("Settings changed to:");
        delay(500);
        paste++;
        
        if(paste == 4){  //cycle back
          paste = 0;
        }

        if(paste == 0){
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("Sn64 Pb37");
          delay(1000);
        }

        if(paste == 1){
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("SAC 305");
          delay(1000);
        }

        if(paste == 2){
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("Sn42 Bi57,6 Ag\n0,6");
          delay(1000);
        }

        if(paste == 3){
          oled.clear();
          oled.setCursor(0, 0);
          oled.print("testval");
          delay(1000);
        }
        oled.clear();
      }
    }



  while(analogRead(A2) > 600){  //more overcomplicated button-reading
    while(analogRead(A2) > 600){}
        oled.clear();
        oled.setCursor(0, 0);
        oled.print("Heat enabled");
        delay(1000);
        heat = 1;
        oled.clear();
      }


if(heat && paste == 0){
  sn64();
}

if(heat && paste == 1){
  SAC();
}

if(heat && paste == 2){
  sn42();
}

lasttemp = avgtemp;
}

/*
Sn64 Pb37 soak 125C 90 sec, reflow 230C
SAC 305 soak 160C 90 sec, reflow 250C
Sn42 Bi57,6 Ag\n0,6 soak 110C 90 sec, reflow 165C
*/

void sn64(){
  
}

void SAC(){
  
}

void sn42(){
  startmillis = millis();
  while(1){
    T = therm.temperature();
      avgtemp = T;
  
//    if(avgtemp != lasttemp){  //checks if the temps or targets have changed and only then updates the screen
    oled.clear();
//    }

    oled.setCursor(0, 0);
    oled.print(avgtemp);

    oled.setCursor(25, 0);
    oled.print("C");

    analogWrite(1, 255);

    oled.setCursor(0, 26);
    oled.print("heating");

    if(avgtemp >= 110){
      break;
    }

    delay(500);
//    lasttemp = T;
  }

  while(1){
    T = therm.temperature();
      avgtemp = T;

    oled.setCursor(0, 0);
    oled.print(avgtemp);

    oled.setCursor(0, 26);
    oled.print("soaking");

    if(avgtemp < 110){
      analogWrite(1, 255);
    }
    else{
      analogWrite(1, 0);
    }
    if(millis() > startmillis + 90000){
      break;
    }

    delay(500);
    
    lasttemp = T;
  }
  



  while(1){
    T = therm.temperature();
      avgtemp = T;
  
//    if(avgtemp != lasttemp){  //checks if the temps or targets have changed and only then updates the screen
    oled.clear();
//    }

    oled.setCursor(0, 0);
    oled.print(avgtemp);

    oled.setCursor(25, 0);
    oled.print("C");

    analogWrite(1, 255);

    oled.setCursor(0, 26);
    oled.print("heating");

    if(avgtemp >= 160){
      break;
    }

    delay(500);
//    lasttemp = T;
  }

  startmillis = millis();
  
  while(1){
    T = therm.temperature();
      avgtemp = T;

    oled.setCursor(0, 0);
    oled.print(avgtemp);

    oled.setCursor(0, 26);
    oled.print("reflowing");

    if(avgtemp < 160){
      analogWrite(1, 255);
    }
    else{
      analogWrite(1, 0);
    }
    if(millis() > startmillis + 60000){
      break;
    }

    delay(500);
    
    lasttemp = T;
  }
  digitalWrite(1, 0);
  heat == 0;
}
