#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>
#include <virtuabotixRTC.h>

// DEFINE BOUNDARIES
#define MIN_TEMP 20 // 20
#define MAX_TEMP 32 // 32
//#define MIN_TURB 0 //10
#define MAX_TURB 200 //100
#define MIN_PH 5 // 6
#define MAX_PH 8 // 7.5
//#define MIN_AMMONIA 0 //0.02
#define MAX_AMMONIA 1 // 0.5

// DEFINE DELAY
#define VALVE_DELAY 30 // in SECONDS
#define TIME_DELAY 1000 // in microseconds

// OFFSETS
float PH_OFFSET = 1;
float TURBIDITY_OFFSET = 2.36;
float Ro =  7.01; //Enter found Ro value

// Parameters for MQ137 Sensor
#define RL 1
#define m -0.263 //Enter calculated Slope
#define b 0.42 //Enter calculated intercept


// CLOCK RTC
virtuabotixRTC myRTC(5,6,7); // CLKD, DAT, RESET

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Data wire for temperature sensor
#define ONE_WIRE_BUS A3

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

// Pins Setup
const int pHOutputPin = A0;
const int turbidityPin = A1;
const int mq137Pin = A2;

const int hold_pin = 8;
const int buzzer_pin =9; // buzzer Pin
const int valve_pin = 4;
const int chipSelect = 10;// Chip select pin for the micro SD card module

float value;

// File for writing data to the micro SD card
File dataFile;
void setup() {
  Serial.begin(9600);
  Serial.print("Arduino UNO is Initializing...");

  // pinMODES
  pinMode(buzzer_pin, OUTPUT);
  pinMode(pHOutputPin, INPUT);
  pinMode(turbidityPin, INPUT);
  pinMode(mq137Pin,INPUT);
  pinMode(valve_pin,OUTPUT);
  pinMode(hold_pin, INPUT);

  digitalWrite(buzzer_pin, HIGH);
  delay(2000);
  digitalWrite(buzzer_pin, LOW);

  // Initialize the LCD screen
  lcd.init();
  lcd.backlight();

  // TESTING LCD SETUP
  lcd.setCursor(0, 0);
  lcd.print("WELCOME");
  delay(5000);
  lcd.clear();
  lcd.print("Configuring");
  lcd.setCursor(0,1);
  lcd.print("Sensors...");
  delay(5000);
  lcd.clear();

  // Initialize the temperature sensor
  sensors.begin();

  /*
  // Check if SD Card can be Initialized
  if (!SD.begin(chipSelect)) {
    lcd.setCursor(0, 0);
    lcd.print("SD Card failed");
    Serial.println("SD Card failed");
    delay(3000);
  }*/

  // CALIBRATE 
  // CHANGE FOR CALIBRATION
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SELECT MODE");
  delay(2000);
  
  int hold_counter = 0;
  for (int hold = 0; hold<5; hold++){
    hold_counter+= digitalRead(hold_pin);
    Serial.print(digitalRead(hold_pin));
    delay(1000);
  }
  if(hold_counter>3){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CALIBRATION_MODE");
      delay(2000);
    
    float turbidity_change, Ro_, pH_Change;
    calibration(turbidity_change, Ro_,pH_Change );
    /*
    Serial.println("THESE VALUES WILL CHANGE: ");
    Serial.print("CHANGE_TURBIDITY: ");
    Serial.println(turbidity_change);
    Serial.print("CHANGE_RO: ");
    Serial.println(Ro_);
    Serial.print("CHANGE_PH: ");
    Serial.println(pH_Change);*/

    // OFFSET CHANGE
    PH_OFFSET = pH_Change;
    TURBIDITY_OFFSET = turbidity_change;
    Ro =  Ro_; //Enter found Ro value
    }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ACTIVATION_MODE");
  delay(2000);
}

void loop() {
  // TEMPERATURE DS18B20 
  float temperature;
  // Request temperature readings from all devices on the bus
  sensors.requestTemperatures();
  // Read values
  temperature = sensors.getTempCByIndex(0);

  // TURBIDITY
  float turbidityVolts,turbidityValue;
  turbidityVolts =( analogRead(turbidityPin)*5.0/1024) + TURBIDITY_OFFSET;
  Serial.println("turbidityVolts: ");
  Serial.println(turbidityVolts);
  turbidityValue = (-1120.4* turbidityVolts*turbidityVolts)+ (5742.3*turbidityVolts)-4352.9;
  //prevent turbidity to go lower than 0
  if (turbidityValue < 0){
      turbidityValue = 0;
    }
  
  // PH VALUE 
  float pHVolts, pHValue;
  
  pHVolts= ( (analogRead(pHOutputPin) / 1024.0) * 5.0 ) ;
  pHValue = 3.5 * pHVolts + PH_OFFSET;

  // MQ-137 Ammonia Sensor
  float mq137Volts, RS_, ratio, ppm;
  mq137Volts = (analogRead(mq137Pin) / 1024.0) * 5.0;
  RS_ = ((5.0/mq137Volts)-1) * RL;
  ratio = RS_/Ro;
  ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm

    /*PRINT IN SERIAL VOLTS*/
  /*
  Serial.println();
  Serial.println("VOLTAGE OF SENSORS: ");
  Serial.println();
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  Serial.print("Turbidity: ");
  Serial.println(turbidityVolts);

  Serial.print("pHVOLTAGE: ");
  Serial.println(pHVolts); //Display calculated Ro
   
  Serial.print("MQ137 VOLTS: " );
  Serial.println(mq137Volts);
  Serial.print("RS/ R0: " );
  Serial.println(ratio);
  
  //PRINT IN SERIAL MONITOR
  Serial.println();
  Serial.println("VALUES OF SENSORS ");
  Serial.println();
  
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  Serial.print("Turbidity: ");
  Serial.println(turbidityValue);

  Serial.print("pH: ");
  Serial.println(pHValue); //Display calculated Ro
   
  Serial.print("Amm: " );
  Serial.println(ppm);
  */
  
  // PRINT IN LCD SCREEN
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print(" Tb:");
  lcd.print(turbidityValue);
  
  lcd.setCursor(0, 1);
  lcd.print("p: ");
  lcd.print(pHValue);
  lcd.print(" A: ");
  lcd.print(ppm);
  
  // CONDITIONS
  if (MIN_TEMP > temperature || MAX_TEMP < temperature){
      value = temperature;
      trip("TP OUT OF RANGE", "TEMP: " , value);
    }  
  if (MAX_TURB < turbidityValue){
      value = turbidityValue;
      trip("TB OUT OF RANGE", "TB: ", value);     
    }
  if (MIN_PH > pHValue || MAX_PH < pHValue){
      value = pHValue;
      trip("PH OUT OF RANGE", "pH: ", value);    
    }
  
  if ( MAX_AMMONIA < ppm){
      value = ppm;
      trip("AMM OUT OF RANGE", "AMM: ", value);
    }
   
  delay(TIME_DELAY);
}

void calibration(float& turbidity_change, float& Ro_, float& pH_Change) { 
  Ro_ = ((5.0/ (analogRead(mq137Pin)*(5.0/1023.0)) )-1) /3.6;
  Serial.print("Analog Value= ");
  Serial.println(analogRead(mq137Pin)); //Display calculated Ro
  /*
  Serial.print("Vrl= ");
  Serial.println(VRL); //Display calculated Ro
  Serial.print("Rs at fresh air = ");
  Serial.println(Rs_); //Display calculated Ro
  Serial.print("Ro at fresh air = ");
  Serial.println(Ro_); //Display calculated Ro
  delay(1000); //delay of 1sec
  */
  // TURBIDITY
  float turbidityVolts,turbidityValue;
  turbidityVolts =( analogRead(turbidityPin)*5.0/1024) ;
  Serial.println("turbidityVolts: ");
  Serial.println(turbidityVolts);
  turbidityValue = (-1120.4* turbidityVolts*turbidityVolts)+ (5742.3*turbidityVolts)-4352.9;
  turbidity_change = 4.2 - turbidityVolts;
 

  // PH VALUE 
  float pHVolts, pHValue;
  pHVolts= ( (analogRead(pHOutputPin) / 1024.0) * 5.0 ) ;
  pHValue = (3.5 * pHVolts);
  pH_Change =  6.86-pHValue;
  Serial.print(pHValue);

  // LCD MONITOR
  Serial.println();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CH_TURB: ");
  lcd.print(turbidity_change);
  
  Serial.print("CH_TURB: ");
  Serial.println(turbidity_change);
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CH_RO: ");
  lcd.print(Ro_);
  
  Serial.print("CH_RO: ");
  Serial.println(Ro_);
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CH_PH: ");
  lcd.print(pH_Change);
  
  Serial.print("CH_PH: ");
  Serial.println(pH_Change);
  delay(3000);
}

void trip(char str_trip [40], char str_dev[10], float value){
      // UPDATE TIME 
    myRTC.updateTime();
     // Start printing elements as individuals
    Serial.print("Current Date / Time: ");
    Serial.print(myRTC.dayofmonth);
    Serial.print("/");
    Serial.print(myRTC.month);
    Serial.print("/");
    Serial.print(myRTC.year);
    Serial.print("  ");
    Serial.print(myRTC.hours);
    Serial.print(":");
    Serial.print(myRTC.minutes);
    Serial.print(":");
    Serial.println(myRTC.seconds);
    
    digitalWrite(valve_pin, HIGH);
    digitalWrite(buzzer_pin, HIGH);
    delay(1000);
    digitalWrite(buzzer_pin, LOW);
    Serial.println(str_trip);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("VALVE OPEN");
    lcd.setCursor(0, 1);
    lcd.print(str_trip);
    
    for (int counter = VALVE_DELAY; counter>0; counter--){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("VALVE OPEN ");
        lcd.print(counter);
        lcd.setCursor(0, 1);
        lcd.print(str_dev);
        lcd.print(value);
        delay(1000);
      }
    
    lcd.clear();
    digitalWrite(valve_pin, LOW);

      /*
           // WRITE SD CARD
      dataFile = SD.open("dataLog2.txt", FILE_WRITE);
      //Serial.print(dataFile);
      if (dataFile) {
        Serial.println("Writing Data in SD CARD");
        dataFile.print("Temperature: ");
        dataFile.print(temperature);
        dataFile.print(" C, Turbidity: ");
        dataFile.print(turbidityValue);
        dataFile.print(", pH: ");
        dataFile.print(pHValue);
        dataFile.print(", Ammonia: ");
        dataFile.print(ppm);
        dataFile.println();
        dataFile.close();
      }*/
  
  }
