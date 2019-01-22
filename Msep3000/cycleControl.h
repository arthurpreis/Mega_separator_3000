#include <LiquidCrystal.h>
#include "functions.h"

// =================== condutivimetro ============================
extern float CalibrationEC; //EC value of Calibration solution is s/cm
extern int R1;
extern int Ra; //Resistance of powering Pins
extern int ECPin;
extern int ECGround;
extern int ECPower;
extern float EC;
extern int ppm;
extern float raw;
extern float Vin;
extern float Vdrop;
extern float Rc;
extern float K;
extern float buffer;

// ================= LCD + Keypad ================================
extern LiquidCrystal lcd;
extern int lcd_key;

// ================ Fluxometro ===================================
extern byte sensorInterrupt;
extern byte sensorPin;

extern volatile byte pulseCount;
extern float calibrationFactor;
extern float flowRate;

extern unsigned int flowMilliLitres;
extern unsigned long totalMilliLitres;
extern unsigned long oldTime;
// ===================== Bombas ==================================
extern float cal_pump1;
extern float cal_pump2;

// ===================== Valvulas ================================
// ===================== Ciclos ==================================


void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}


void set_init_parm(char cycle_item[10], int j, char unit_name[3], int old)
{
  lcd.clear();
  lcd.print(cycle_item);
  lcd.print(j + 1); 
  lcd.setCursor(0, 1);
  lcd.print(old);
  printUnit(unit_name);
  lcd_key = btnNONE;
}

int btn_dir_ctrl()
{
  lcd_key = read_LCD_buttons();
  while (lcd_key == btnNONE) {
    lcd_key = read_LCD_buttons();
  }
  switch (lcd_key)
  {
    case btnRIGHT:
      {
        return 1;
      }
    case btnLEFT:
      {
        return -1;
      }
    default:
      {
        return 0;
      }
  }
}

bool prompt_ok() {
  lcd.clear();
  lcd.print("<< Rever dados  ");
  lcd.setCursor(0, 1);
  lcd.print("    Continuar >>");
  int b = btn_dir_ctrl();
  if (b == 1) return true;
  else return false;
}

void display_status(char msg[16], int i){
    lcd.clear();
    lcd.print(msg);
    lcd.setCursor(3,1);
    lcd.setCursor(13,1);
    lcd.print(i+1);
    lcd.print('/');
    lcd.print(NUM_CYCLES);
}

int qnt_ctrl(int D, int MAX, int MIN, int old)
{
  int temp = old;
  while (lcd_key != btnSELECT)
  {
    int b = btn_dir_ctrl();
    temp += D * b;
    if (temp >= MAX) {
      temp = MAX;
    }
    else if (temp <= MIN) {
      temp = MIN;
    }

    clearLine();
    lcd.print(temp);
    delay(200);
  }
  return temp;
}

int set_cycle_qnt(char cycle_item[10], int i, char unit_name[3], int D, int MAX, int MIN, int old) {
  set_init_parm(cycle_item, i, unit_name, old);
  return qnt_ctrl(D, MAX, MIN, old);
}

void cycle_set(int *c)
{
  //int c[25];
  lcd.setCursor(0, 0);
  for (int i = 0; i < NUM_CYCLES; i++) {
    int j = 5 * i;
    c[j + 0] = set_cycle_qnt("Qte agua "      , i, "  L", D_WATER  , MAX_WATER  , MIN_WATER , c[j + 0]);
    c[j + 1] = set_cycle_qnt("Qte sol. 1 "    , i, " mL", D_SOL    , MAX_SOL    , MIN_SOL   , c[j + 1]);
    c[j + 2] = set_cycle_qnt("Qte sol. 2 "    , i, " mL", D_SOL    , MAX_SOL    , MIN_SOL   , c[j + 2]);;
    c[j + 3] = set_cycle_qnt("Tempo mix "     , i, "min", D_TIME   , MAX_TIME   , MIN_TIME  , c[j + 3]);
    c[j + 4] = set_cycle_qnt("Tempo rest "    , i, "min", D_TIME   , MAX_TIME   , MIN_TIME  , c[j + 4]);
    delay(50);
  }

}

void measure_water(int max_amt){
  while (totalMilliLitres < max_amt){
  
    if((millis() - oldTime) > 1000)    // Only process counters once per second
      { 
        
        detachInterrupt(sensorInterrupt);
        flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
        oldTime = millis();
        flowMilliLitres = (flowRate / 60) * 1000;
        totalMilliLitres += flowMilliLitres; 
       
        lcd.setCursor(0,1);
        lcd.print(totalMilliLitres/1000);
        lcd.println("L"); 
        
        pulseCount = 0;
        attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    }
  }
}


void mix(int i, int t){
   display_status("Misturando      ", i);
   digitalWrite(MOTOR, HIGH);
   int wait_time = t*60; //em segundos
   for(int n = 0; n<wait_time; n++){
    if (n%60 == 0){
      clearLine();
      lcd.print(n/60);
      lcd.print("min");
    }
    delay(1000);
   }
   digitalWrite(MOTOR, LOW);
}

void rest(int i, int t){
   display_status("Repousando    ", i);
   int wait_time = t*60; //em segundos
   for(int n = 0; n<wait_time; n++){
    if (n%60 == 0){
      clearLine();
      lcd.print(n/60);
      lcd.print("min");
    }
    delay(1000);
   }
}

void add_liquid(int L, int i, int qty){
  if (L==1){
      pulseCount        = 0;
      flowRate          = 0.0;
      flowMilliLitres   = 0;
      totalMilliLitres  = 0;
      oldTime           = 0;
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
          
      display_status("Adc. agua      ", i);
    
      digitalWrite(VALVE_IN, LOW); //ativo em baixo
      measure_water(qty*1000);
      digitalWrite(VALVE_IN, HIGH);
    }
  else if (L==2){
      display_status("Adc. sol. 1", i);
      digitalWrite(PUMP_1, LOW);
      int wait_time = qty/cal_pump1;
      for(int n = 0; n<wait_time; n++){
        clearLine();
        lcd.print(round(cal_pump1*n));
        lcd.print("mL");
        delay(1000);
       }
      digitalWrite(PUMP_1, HIGH);
    }
  else if (L==3){
      display_status("Adc. sol 2", i);
      digitalWrite(PUMP_2, LOW);
      int wait_time = qty/cal_pump2;
      for(int n = 0; n<wait_time; n++){
        clearLine();
        lcd.print(round(cal_pump1*n));
        lcd.print("mL");
        delay(1000);
      }
      digitalWrite(PUMP_2, HIGH);
    }
  delay(100);
}

void empty(int i){
    display_status("Esvaziando    ", i);
    Rc = 0;
    digitalWrite(VALVE_OUT, LOW);
    while (Rc < THRESHOLD){
      int k=1;
      buffer=0;

      while(k<=10){
          digitalWrite(ECPower,HIGH);
          raw = analogRead(ECPin);
          raw = analogRead(ECPin);// This is not a mistake, First reading will be low
          digitalWrite(ECPower,LOW);
          buffer=buffer+raw;
          k++;
          delay(50);
      };
      
      raw=(buffer/10);
      Vdrop= (((Vin)*(raw))/1024.0);
      Rc=(Vdrop*R1)/(Vin-Vdrop);
      Rc=Rc-Ra;
      clearLine();
      lcd.print(Rc);
    }
    digitalWrite(VALVE_OUT, HIGH);
 
}

void force_empty(){
  digitalWrite(VALVE_OUT, LOW);
  while(true){}
  
  }
