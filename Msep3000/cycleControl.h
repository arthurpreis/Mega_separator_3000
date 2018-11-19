#include <LiquidCrystal.h>
#include "functions.h"


extern LiquidCrystal lcd;
extern int lcd_key;

void set_init_parm(char cycle_item[10], int j, char unit_name[3], int old)
{
  lcd.clear();
  lcd.print(cycle_item);
  lcd.print(j + 1); //TODO: CONSERTAR CHAR DISPLAY
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
    c[j + 3] = set_cycle_qnt("Tempo mix "     , i, "min", D_TIME   , MAX_TIME   , MIN_TIME  , c[j + 3]);
    c[j + 4] = set_cycle_qnt("Tempo hold "    , i, "min", D_TIME   , MAX_TIME   , MIN_TIME  , c[j + 4]);
    delay(50);
  }

}
