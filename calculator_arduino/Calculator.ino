#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <math.h>
//#include <Adafruit_LiquidCrystal.h>
//#include <string.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'X', '0', '=', '/'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

// Created instances
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
  
String firstNum = "";
String secondNum = "";
double result = 0.0;
char operatr = ' ';
char operatr2 = ' '; //marker for the second operation O2 
LiquidCrystal_I2C lcd(0x27,20,4); 

void clr() ;
void removeTrailingZeros(String &str);

bool firstNumberState = true; //marker for 1st number input
bool secondNumberState = false; //marker for 2nd number input
bool contOp = false;
bool resetMarker = true; //marker to know if bagong reset or clear yung device
bool opMarker = false;
bool firstInputMarker = false; //marks when first input has been made
int firstNumLength = 0;
int secondNumLength =0;
int resultLength = 0;
int dotCheck = -1;
int eCheck = -1;

void setup() {
  lcd.init();  
  lcd.backlight();     
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0); // col - row
  lcd.print("CALCULATIONATION");
  lcd.setCursor(0, 1);
  lcd.print("By The MATRIX");
  delay(1000);
  clr();
}

void loop() {


  char newKey = myKeypad.getKey();

  // SETS UP THE DISPLAY COUNT
 

  //DOUBLE TAP DOUBLE TAP DOUBLE TAP
  if (newKey != NO_KEY){
    int counter = 1;
    for (int i = 0; i < 500; i++){
      if(newKey == myKeypad.getKey()){
      	 counter += 1; 
      }
      delay(1);
    }
  
    if (counter >= 2){
      if(newKey == 'X'){  
      	newKey = 'C'; //so that a quick double tap will "CLEAR"
      }else if (newKey == '0'){
      	newKey = '.'; //quick double tap of '0' will be '.'
      }
      else if (newKey == '3') {
        newKey = 'e'; //quick double tap of '3' will be 'e'
       }
      else{
      	newKey = NO_KEY;
      }
      Serial.println(newKey);
    }
  }
  

  //DELETE DELETE DELETE 
  if (newKey != NO_KEY && newKey == 'X'){
    
    if (firstNumberState == true && secondNumberState == false){     
      int length = firstNum.length();
      firstNum = firstNum.substring(0, length-1);
      firstNumLength = firstNum.length();
      printingLCD(16-firstNumLength,1,firstNum);
      lcd.setCursor(0,0);
      lcd.print(operatr);
    }

    else if (firstNumberState == false && secondNumberState == true) {
      int length = secondNum.length();
      secondNum = secondNum.substring(0, length-1);
      secondNumLength = secondNum.length();
      printingLCD(16-firstNumLength,0,firstNum);
      lcd.setCursor(16-secondNumLength,1); 
      lcd.print(secondNum);
      lcd.setCursor(0,0);
      lcd.print(operatr);
    }   
  }


  //123456789
  //123456789
  if (newKey != NO_KEY && (newKey == '1' || newKey == '2' || newKey == '3' || newKey == '4' || newKey == '5' || newKey == '6' || newKey == '7' || newKey == '8' || newKey == '9' || newKey == '0' || newKey == '.' || newKey == 'e')) {
    
    firstNumLength = firstNum.length();
    secondNumLength = secondNum.length();

    if(dotCheck >= 0 && newKey == '.'){
      //nothing happens so that there will be no two instance of decimal
    }

    else if (eCheck >= 0 && newKey == 'e'){
      //nothing happens so that there will be no two instance of e
    }

    else if (firstNumberState) {
      if (firstNum.toDouble() == 0 && dotCheck == -1) { //to remove zero inputs
        firstNum = "";
      }
      Serial.println("123456: IF IF IF");
      firstNum = firstNum + newKey;
      Serial.println(firstNum);
      firstNumLength = firstNum.length();
      printingLCD(16-firstNumLength,1,firstNum);
      resetMarker = false;
      firstInputMarker = true;
      dotCheck = firstNum.indexOf('.');
      eCheck = firstNum.indexOf('e');
    }
    else if (firstNumberState == false && opMarker == true) {
      if (secondNum.toDouble() == 0 && dotCheck == -1) { //removes zero inputs
        secondNum = "";
      }
      Serial.println("123456: ELSE ELSE ELSE");
      secondNumberState = true; //marker for any numerical second input
      secondNum = secondNum + newKey; 
      printingLCD(16-firstNumLength,0,firstNum);
      secondNumLength = secondNum.length();
      lcd.setCursor(16-secondNumLength,1); 
      lcd.print(secondNum);
      lcd.setCursor(0,0);
      lcd.print(operatr); 
      resetMarker = false;
      dotCheck = secondNum.indexOf('.');
      eCheck = secondNum.indexOf('e');
    }
   

  } // end of 123456789 numerical inputs  
  
  
  //OPERATORSSSS
  if (newKey != NO_KEY && (newKey == '+' || newKey == '-' || newKey == '*' || newKey == '/' || newKey == '=') && (resetMarker == false)) {
    
    opMarker = true; // marker that an operation has been pressed

    if (firstNumberState == true && newKey == '='){
      Serial.println("EQUAL PRESSED at FIRST INPUT");
      operatr = '+'; //saves the operation in the 'operatr' variable

      firstNumLength = firstNum.length();
      printingLCD(16-firstNumLength,0,firstNum);
      lcd.setCursor(0,0);
      lcd.print(operatr);
      firstNumberState = false; //marker to end first number input
    }

    else if (firstNumberState == true) {
      Serial.println("OP: firstNumberState TRUE");
      operatr = newKey; //saves the operation in the 'operatr' variable
      firstNumLength = firstNum.length();
      printingLCD(16-firstNumLength,0,firstNum);
      lcd.setCursor(0,0);
      lcd.print(operatr);
      firstNumberState = false; //marker to end first number input
    }

    else if (firstNumberState == false && secondNumberState == false) {    //condition when a second input value has not yet been made
      operatr = newKey;                       //allowing to change the operator while no 2nd input
      firstNumLength = firstNum.length();
      printingLCD(16-firstNumLength,0,firstNum);
      lcd.setCursor(0,0);
      lcd.print(operatr); // overwrites the operator in case "nagpalit isip sa kung anong operator ang gusto"
      Serial.println("OP: else if 1");
    }

    else if (secondNumberState == true) {  //when there is already a 2nd input for continuous opperation
      operatr2 = newKey;
      newKey = '='; //triggers the equal effect when an operation is pressed at second input   
      contOp = true; //marker for continuous operation
      Serial.println("OP: else if 2 secondNumberState TRUE");
    }
    
    else {
      Serial.println("OP: ELSE ELSE ELSE");
    }

    eCheck = -1;

  }// END of OPERATOR IF  
  
  
/// EQUALLLLL RESULT
  if (newKey != NO_KEY && newKey == '=' && secondNumberState == false && firstInputMarker == false) {
    //[Behavior number 9] does nothing when there is no valid input in the input row 
  }
  
  else if (newKey != NO_KEY && newKey == '=') {
    
    //handles the e exponent
  
    int eIndexFirstNum = firstNum.indexOf('e');
    int eIndexSecondNum = secondNum.indexOf('e');

    if (eIndexFirstNum >= 0){
      //converts firstNum to a number datatype
      String eBefore = firstNum.substring(0,eIndexFirstNum);
      String eAfter = firstNum.substring(eIndexFirstNum+1, firstNum.length());
      double firstNumDouble = eBefore.toDouble()*pow(10,eAfter.toDouble());
      Serial.println("FIRST NUM AFTER E");
      Serial.println(eBefore);
      Serial.println(eAfter);
      Serial.println(eAfter.toDouble());
      firstNum = String(firstNumDouble);
      Serial.println(firstNum);
      Serial.println(firstNumDouble);
      Serial.println("first indexcheck");
    }

    if (eIndexSecondNum >= 0){
      //converts firstNum to a number datatype
      String eBefore = secondNum.substring(0,eIndexSecondNum);
      String eAfter = secondNum.substring(eIndexSecondNum+1, secondNum.length());
      double secondNumDouble = eBefore.toDouble()*pow(10,eAfter.toDouble());
      secondNum = String(secondNumDouble);
      Serial.println(eBefore);
      Serial.println(eAfter);
      Serial.println(eAfter.toDouble());
      Serial.println("SECOND NUM AFTER E");
      Serial.println(eBefore);
      Serial.println(eAfter);
      Serial.println(secondNum);
      Serial.println(secondNumDouble);
    }


    if (operatr == '+') {
      result = firstNum.toDouble() + secondNum.toDouble();
    }

    if (operatr == '-') {
      result = firstNum.toDouble() - secondNum.toDouble();
    }

    if (operatr == '*') {
      result = firstNum.toDouble() * secondNum.toDouble();
    }

    if (operatr == '/') {
      result = firstNum.toDouble() / secondNum.toDouble();
    }

    String strResult = String(result, 15);
    removeTrailingZeros(strResult);
    resultLength = strResult.length(); // for adjusting the display
    
    //For detecting MATH ERROR
    char MathError[strResult.length() + 1];
    strResult.toCharArray(MathError, sizeof(MathError));
    char INFdetect = strstr(MathError, "INF");

    if (INFdetect != NULL){
    printingLCD(16-10, 0,"MATH ERROR");
     secondNum = "";
     INFdetect = '\0'; 
    }
    //End of detecting MATH ERROR

    else if (contOp == false && secondNumberState == true) {
      
      if (operatr == ' ' && resetMarker == true){        //so that pressing equal does nothing upon opening the device
        Serial.print("DOES NOTHING"); //DOES NOTHING
      }

      else if (operatr == ' '){
        operatr = '+';
        firstNumLength = firstNum.length();
        printingLCD(16-firstNumLength, 0,firstNum);
        lcd.setCursor(0,0);
        lcd.print(operatr);
        Serial.println("'=' empty");
        Serial.println(firstNum);
        firstNumberState = false;
        opMarker = true;
      }
      
      else {
        Serial.println("EQ: OPERATOR");
        printingLCD(16-resultLength, 0,strResult);
        secondNum = "";
        firstNum = strResult; //saves the string result as first number in case an operation is pressed
        opMarker = false; // marker to disable any numerical input unless another operation is pressed
        operatr = ' '; //resets the operator after calculation is done
      }
    
    Serial.println("contOp false");
    Serial.println(strResult);
    Serial.println(operatr);
    }

    else if (contOp == true){
     printingLCD(16-resultLength, 0,strResult);
     lcd.setCursor(0,0);
     lcd.print(operatr2); 
     firstNum = strResult; //saves the result as first variable in case operated again
     secondNum = ""; //resets the second number 
     operatr = operatr2;
     firstNumberState = false; //resets the first number state to original
     Serial.println("contOp true");
     Serial.println(strResult);
     contOp = false; //to revert to non continuous operation
     opMarker = true;
    }
    
    secondNumberState = false; //marker na walang second input after pressing equal
    firstInputMarker = false; 
    Serial.println(strResult);
    Serial.println(result);
    int dotCheck = -1;
    int eCheck = -1;
    Serial.println("EQUAL LAST LINE");
  } // end of 'if' when the '=' is pressed  
  
  
  
  //CLEAR FUNCTION  
  if (newKey != NO_KEY && newKey == 'C') {
    clr();
    Serial.println("CLEAR CLEAR CLEAR");
  }

  
}//END OF MAIN VOID LOOP


/*______________________________________________
*/
// LIST OF FUNCTIONS USED

//clear function
void clr() {
  lcd.clear();
  firstNum = "";
  secondNum = "";
  result = 0;
  operatr = ' ';
  operatr2 = ' ';
  contOp = false;
  secondNumberState = false; //resets the variable to original
  firstNumberState = true; //resets the variable to original
  resetMarker = true; //resets the reset marker
  opMarker = false; //resets the operation marker
  firstInputMarker = false;
  firstNumLength = 0;
  secondNumLength = 0;
  resultLength = 0;
  lcd.setCursor(16-5, 0);
  lcd.print("CLEAR");
  delay(250);
  lcd.clear();
  int dotCheck = -1;
  int eCheck = -1;
}

void removeTrailingZeros(String &str) {
  int decimalPointIndex = str.indexOf('.'); // Find the index of the decimal point
  int endIndex = str.length() - 1; // Last index of the string

  // Remove trailing zeros
  while (endIndex > decimalPointIndex && str.charAt(endIndex) == '0') {
    endIndex--;
  }

  // If the last character is a decimal point, remove it as well
  if (endIndex == decimalPointIndex) {
    endIndex--;
  }

  // Truncate the string
  str = str.substring(0, endIndex + 1);
}

void printingLCD(int cursor1, int cursor2, String printString){
        lcd.clear();
        lcd.setCursor(cursor1, cursor2);
        lcd.print(printString);
}





