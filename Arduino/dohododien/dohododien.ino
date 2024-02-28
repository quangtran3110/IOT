#include <LiquidCrystal_I2C.h>      /*include LCD I2C Library*/
#include "INA226_WE.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); /*I2C scanned address defined + I2C screen size*/
INA226_WE ina226 = INA226_WE(0x40);

float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;

void monitor() {
  ina226.readAndClearFlags();
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
  current_mA = ina226.getCurrent_mA();
  power_mW = ina226.getBusPower();
  loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);
}

void lcd_screen() {
  lcd.setCursor(3, 0);
  lcd.print(busVoltage_V);
  lcd.setCursor(3, 1);
  lcd.print(current_mA);
  Serial.println(current_mA);
}
void setup() {
  Serial.begin(9600);
  lcd.init();             /*LCD display initialized*/
  lcd.clear();            /*Clear LCD Display*/
  lcd.backlight();        /*Turn ON LCD Backlight*/
  lcd.setCursor(0, 0);    /*Set cursor to Row 1*/
  lcd.print("U: "); /*print text on LCD*/
  lcd.setCursor(15, 0);    /*Set cursor to Row 1*/
  lcd.print("V"); /*print text on LCD*/
  lcd.setCursor(0, 1);    /*set cursor on row 2*/
  lcd.print("I: "); /*print message on LCD*/
  lcd.setCursor(14, 1);    /*set cursor on row 2*/
  lcd.print("mA"); /*print message on LCD*/

  ina226.init();
  ina226.setAverage(AVERAGE_16);             // choose mode and uncomment for change of default
  ina226.setConversionTime(CONV_TIME_1100);  //choose conversion time and uncomment for change of default
  ina226.setMeasureMode(CONTINUOUS);         // choose mode and uncomment for change of default
  ina226.setCurrentRange(MA_800);            // choose gain and uncomment for change of default
  //Serial.println("INA226 Current Sensor Example Sketch - Continuous");
  ina226.waitUntilConversionCompleted();  //if you comment this line the first data might be zero
  delay(1000);
}
void loop() {
  delay(1000);
  lcd_screen();
  monitor();
}