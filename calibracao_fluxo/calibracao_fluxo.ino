#include <LiquidCrystal.h>
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define water_valve A4

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
byte statusLed    = 13;
// litre/minute of flow.
float calibrationFactor = 8.7;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
int lcd_key     = 0;
long int s = 0;
int read_LCD_buttons()
{
  int adc_key_in  = 0;
 adc_key_in = analogRead(1);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void setup() {
  // put your setup code here, to run once:
lcd.begin(16, 2);
pinMode(water_valve, OUTPUT);    
digitalWrite(water_valve, HIGH);
pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

}


void loop() {
  //lcd_key = read_LCD_buttons();
  
//while(lcd_key == btnNONE){  // put your main code here, to run repeatedly:
  
  //int sensorValue = analogRead(A1);
  //lcd.print(sensorValue);
  //s += sensorValue;
  //lcd.setCursor(0,1);
  //lcd.print(s);
  //delay(500);
  //lcd_key = read_LCD_buttons();
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    lcd.clear();
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
    
    lcd.print(int(flowRate));  // Print the integer part of the variable
    lcd.print("L/min");
    

    // Print the cumulative total of litres flowed since starting
    lcd.setCursor(0,1);
    lcd.print(totalMilliLitres);
    lcd.println("mL"); 
    
    

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
//}
}
