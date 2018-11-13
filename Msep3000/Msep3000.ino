#include <LiquidCrystal.h>
#include "settings.h"
#include "initialize.h"
#include "functions.c"

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define MAX_TIME 600

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int adc_key_in  = 0;
int lcd_key     = 0;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/

 return btnNONE;  // when all others fail, return this...
}

void clearLine(){
  lcd.setCursor(0,1);
  lcd.print("    ");
  lcd.setCursor(0,1);
}

void timer_set()
{
  lcd.setCursor(0,0);
  lcd.print("Selec T");
  int t = 0;
  while(true)
  {
    clearLine();
    lcd.print(t);
    lcd_key = read_LCD_buttons(); 
    switch (lcd_key)               // depending on which button was pushed, we perform an action
      {
        case btnRIGHT:
        {
          t += 10;
          break;
        }
        case btnLEFT:
        {
          t -= 10;
          break;
        }
      }
    if (t >= MAX_TIME){
      t = MAX_TIME;
    }
    else if(t < 0){
      t = 0;
    }
    delay(100);
  }
}

void setup()
{
 lcd.begin(16, 2);              // start the library
 
}
 
void loop()
{
 timer_set(); 

 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     lcd.print("RIGHT ");
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT   ");
     break;
     }
   case btnUP:
     {
     lcd.print("UP    ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     lcd.print("NONE  ");
     break;
     }
 }

}
