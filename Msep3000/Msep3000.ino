#include <LiquidCrystal.h>
#include "settings.h"
#include "initialize.h"
#include "cycleControl.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key     = 0;
int cycle[25];


void setup()
{
 lcd.begin(16, 2);              // start the library
}
 
void loop()
{
 cycle_set(); 
}
