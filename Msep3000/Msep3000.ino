#include <LiquidCrystal.h>
#include "settings.h"
#include "initialize.h"
#include "cycleControl.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key     = 0;

int cycle[NUM_CYCLES * 5];


void setup()
{
 lcd.begin(16, 2);              // start the library
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
    lcd.print("iniciando");
    delay(2000); 
}
