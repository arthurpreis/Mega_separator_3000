#include <LiquidCrystal.h>
#include "functions.h"


extern LiquidCrystal lcd;
extern int lcd_key;
volatile byte pulseCount;

extern float calibrationFactor;
extern float flowRate;
extern unsigned int flowMilliLitres;
extern unsigned long totalMilliLitres;
extern unsigned long oldTime;

extern byte sensorInterrupt;
extern byte sensorPin;
  
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
    lcd.print('%');
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
    c[j + 0] = set_cycle_qnt("Qte agua "      , i, "L  ", D_WATER  , MAX_WATER  , MIN_WATER , c[j + 0]);
    c[j + 1] = set_cycle_qnt("Qte sol.acida " , i, "mL ", D_ACID   , MAX_ACID   , MIN_ACID  , c[j + 1]);
    c[j + 2] = set_cycle_qnt("Qte sol.salina ", i, "mL ", D_SALINE , MAX_SALINE , MIN_SALINE, c[j + 2]);
   // c[j + 3] = set_cycle_qnt("Tempo mix "     , i, "min", D_TIME   , MAX_TIME   , MIN_TIME  , c[j + 3]);
   // c[j + 4] = set_cycle_qnt("Tempo hold "    , i, "min", D_TIME   , MAX_TIME   , MIN_TIME  , c[j + 4]);
    delay(50);
  }

}

void measure_water(int max_amt){
  while (totalMilliLitres < max_amt){
  
    if((millis() - oldTime) > 1000)    // Only process counters once per second
      { 
        // Disable the interrupt while calculating flow rate and sending the value to
        // the host
        detachInterrupt(sensorInterrupt);
        
        // Because this loop may not complete in exactly 1 second intervals we calculate
        // the number of milliseconds that have passed since the last execution and use
        // that to scale the output. We also apply the calibrationFactor to scale the output
        // based on the number of pulses per second per units of measure (litres/minute in
        // this case) coming from the sensor.
        flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
        // Note the time this processing pass was executed. Note that because we've
        // disabled interrupts the millis() function won't actually be incrementing right
        // at this point, but it will still return the value it was set to just before
        // interrupts went away.
        oldTime = millis();
    
        // Divide the flow rate in litres/minute by 60 to determine how many litres have
        // passed through the sensor in this 1 second interval, then multiply by 1000 to
        // convert to millilitres.
        flowMilliLitres = (flowRate / 60) * 1000;
    
        // Add the millilitres passed in this second to the cumulative total
        totalMilliLitres += flowMilliLitres;
      
        unsigned int frac;
    
        // Print the flow rate for this second in litres / minute
    
        // Print the cumulative total of litres flowed since starting
    
        lcd.setCursor(0,1);
        lcd.print(totalMilliLitres);
        lcd.println("mL"); 
    }
  }
}


void add_liquid(int L, int i, int qty){
    if (L==1){
      display_status("Adc. agua      ", i);
       digitalWrite(WATER_VALVE, LOW);
       digitalWrite(PUMP_1, HIGH);
       digitalWrite(PUMP_2, HIGH);

       measure_water(qty*1000);

       digitalWrite(WATER_VALVE, HIGH);
       digitalWrite(PUMP_1, HIGH);
       digitalWrite(PUMP_2, HIGH);
    }
    else if (L==2){
      display_status("Adc. sol. acida", i);
    }
    else if (L==3){
      display_status("Adc. sol salina", i);
    }

    delay(100);

 
}
