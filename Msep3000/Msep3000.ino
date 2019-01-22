#include <LiquidCrystal.h>
#include "settings.h"
#include "initialize.h"
#include "cycleControl.h"



// =================== condutivimetro ==============================
float CalibrationEC=1; //EC value of Calibration solution is s/cm
int R1 = 1000;
int Ra = 25; //Resistance of powering Pins
int ECPin= A1;
int ECGround=A2;
int ECPower =A3;
float EC = 0;
int ppm = 0;
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float K=0;
float buffer=0;

 // ================= LCD + Keypad =================================
int lcd_key     = 0;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

// ================ Fluxometro ====================================
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
float calibrationFactor = 8.7;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;
volatile byte pulseCount;

// ===================== Bombas ==================================
float cal_pump1 = 0.6;
float cal_pump2 = 0.6;

// ===================== Valvulas ===============================


// ===================== Ciclos ====================================
int cycle[NUM_CYCLES * 5];

//***___***___***___***___SETUP___***___***___***___***
void setup()
{
 // ==== condutivimetro ===
      pinMode(ECPin,INPUT);
      pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
      pinMode(ECGround,OUTPUT);//setting pin for sinking current
      digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
      R1=(R1+Ra); 
      
 // ==== LCD + Keypad =====
      lcd.begin(16, 2);   
       
 // ==== Fluxometro =======
           pinMode(sensorPin, INPUT);
      digitalWrite(sensorPin, HIGH );
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
      
      pulseCount        = 0;
      flowRate          = 0.0;
      flowMilliLitres   = 0;
      totalMilliLitres  = 0;
      oldTime           = 0;
      
 // ==== Bombas ===========
           pinMode(PUMP_1, OUTPUT);    
           pinMode(PUMP_2, OUTPUT); 
      digitalWrite(PUMP_1, HIGH  );
      digitalWrite(PUMP_2, HIGH  );
      
 // ==== Valvulas =========
           pinMode(VALVE_IN , OUTPUT);   
           pinMode(VALVE_OUT, OUTPUT);   
      digitalWrite(VALVE_IN , HIGH  );  
      digitalWrite(VALVE_OUT, HIGH  );
      
 // ==== Ciclos =========== 
           pinMode(MOTOR, OUTPUT);   
      digitalWrite(MOTOR, LOW   );

 // Bypass para esvaziar
 lcd_key = read_LCD_buttons();
 if(lcd_key == btnRIGHT){
      force_empty(); 
 }
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
      rest(i, cycle[j + 4]);
      empty(i);
    }
    lcd.clear();
    lcd.print("ENCERRADO");
    delay(10000);
}
