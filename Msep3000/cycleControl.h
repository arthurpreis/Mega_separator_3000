#include <LiquidCrystal.h>
#include "functions.h"


extern LiquidCrystal lcd;
extern int lcd_key;
extern int cycle[];

void set_init_parm(int j, char unit_name[3], char cycle_item[10], int cycle_item_index_pos)
{  
   lcd.clear();
   lcd.write("Qte ");
   lcd.write(cycle_item);
   lcd.setCursor(cycle_item_index_pos, 0);
   lcd.write(j+1); //TODO: CONSERTAR CHAR DISPLAY
   lcd.setCursor(0,1);
   lcd.write('0');
   printUnit(unit_name);
   lcd_key = btnNONE; 
}

int btn_dir_ctrl()
{ 
      lcd_key = read_LCD_buttons();
      while(lcd_key == btnNONE){
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
        }
}


int qnt_ctrl(int D, int MAX, int MIN)
{
  int temp = 0;
  while(lcd_key != btnSELECT)
   {
      int b = btn_dir_ctrl();
      temp += D*b;
      if (temp >= MAX){
        temp = MAX;
      }
      else if(temp <= MIN){
        temp = MIN;
      }  
      
      clearLine();
      lcd.write(temp);
      delay(200);
   }
   return temp;   
}

int set_cycle_qnt(int i, char unit_name[3], char cycle_item[10], int cycle_item_index_pos, int D, int MAX, int MIN){
   set_init_parm(i, unit_name, cycle_item, 14);
   return qnt_ctrl(D, MAX, MIN);   
}

void cycle_set()
{
  lcd.setCursor(0,0);
  for (int i = 0; i<5; i++){
    cycle[5*i+0] = set_cycle_qnt(i, "L  ", "agua   :     ",  9, D_WATER  , MAX_WATER  , MIN_WATER  );
    cycle[5*i+1] = set_cycle_qnt(i, "mL ", "sol.acida  : ", 14, D_ACID   , MAX_ACID   , MIN_ACID   );
    cycle[5*i+2] = set_cycle_qnt(i, "mL ", "sol.salina   ", 16, D_SALINE , MAX_SALINE , MIN_SALINE );
    cycle[5*i+3] = set_cycle_qnt(i, "min", "tempo mix  : ", 14, D_TIME   , MAX_TIME   , MIN_TIME   );
    cycle[5*i+4] = set_cycle_qnt(i, "mL ", "tempo hold   ", 16, D_TIME   , MAX_TIME   , MIN_TIME   );
   
  
   delay(50);  
  }

}
