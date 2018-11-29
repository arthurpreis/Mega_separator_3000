#include <LiquidCrystal.h>
#include "settings.h"
#include "initialize.h"
#include "cycleControl.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

float calibrationFactor = 8.7;
float cal_pump1 = 0.6;
float cal_pump2 = 0.6;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

int lcd_key     = 0;


int cycle[NUM_CYCLES * 5];


void setup()
{
 lcd.begin(16, 2);              // start the library
 pinMode(WATER_VALVE, OUTPUT);    
 pinMode(PUMP_1, OUTPUT);    
 pinMode(PUMP_2, OUTPUT); 
 pinMode(MOTOR, OUTPUT);   
 
 digitalWrite(WATER_VALVE, HIGH);
 digitalWrite(PUMP_1, HIGH);
 digitalWrite(PUMP_2, HIGH);
 digitalWrite(MOTOR, LOW);
 
 pinMode(sensorPin, INPUT);
 digitalWrite(sensorPin, HIGH);

}
 
void loop()
{
    bool prompt = false;
    while (!prompt){
      cycle_set(cycle);
      prompt = prompt_ok();
      delay(200);
    }
    lcd.clear();
    lcd.print("Iniciando");
    delay(2000);

    for (int i=0; i<NUM_CYCLES; i++){
      int j = 5 * i;
      add_liquid(1, i, cycle[j + 0]);
      add_liquid(2, i, cycle[j + 1]);
      add_liquid(3, i, cycle[j + 2]);
      mix(i, cycle[j + 3]);
//      rest(i, cycle[j + 4]);
      //empty(i);
    }
    lcd.clear();
    lcd.print("ENCERRADO");
    delay(10000);
}
