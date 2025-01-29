/**
 * author: Rheymar Cinco
 * Date: April 2024
 * Email: racinco.ext@gmail.com
 * 
*/

#include "WiFi.h"
#include "HTTPClient.h"
#include <TinyGPS++.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define DEBUG 1
#define GPS_BAUDRATE 9600  // Set Baud Rate of 9600 for GPS Serial 2

// const char* ssid = "ZTE_2.4G_XafURV";
// const char* password = "bgUwR2wU";
const char* ssid = "Redmi10C";
const char* password = "12345678";

TinyGPSPlus gps;  // the TinyGPS++ object
String serverName = "https://logitrackserver-901e112e4d25.herokuapp.com/insert-location";

unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

double latitude = 0;
double longitude = 0;
double altitude = 0;
double speed = 0;

String dateTime = "";

void printDebug(String message); 
void getLocation(); 
void getAltitude();
void getSpeed(); 
void getGpsDateTime(); 

void httpGetRequest();
void httpPostRequest();

void setup() {
  Serial.begin(9600); 
  Serial2.begin(GPS_BAUDRATE);

  // Start Connecting to the Internet
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if (Serial2.available() > 0) {

    if (gps.encode(Serial2.read())) {

      getLocation(); 
      getAltitude();
      getSpeed();
      getGpsDateTime();
      httpPostRequest(); 

      Serial.println();

      delay(1000); 
    }

  }
}

void getLocation() {

  if (gps.location.isValid()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();

    printDebug("Latitude: " + String(latitude));
    printDebug("Latitude: " + String(longitude));

  } else {

    printDebug("Location INVALID");

  }

}

void getAltitude() {
    if (gps.altitude.isValid()) {

      altitude = gps.altitude.meters();
      printDebug("Altitude: " + String(altitude) + " m");

    } else {

      printDebug("Altitude INVALID");

    }
}

void getSpeed() {

    if (gps.speed.isValid()) {
      speed = gps.speed.kmph();
      printDebug("Speed: " + String(speed) + " km/h");
    } else {
      printDebug("Speed INVALID");
    }

}

void getGpsDateTime(){

      if (gps.date.isValid() && gps.time.isValid()) {

        dateTime = String(gps.date.year()) + "-" + String(gps.date.month()) + "-"
          + String(gps.date.day()) + "-" + String(gps.time.hour()) + "-"
          + String(gps.time.minute()) + ":" + String(gps.time.second()) ;

        printDebug("GPS-DATE-TIME: " + dateTime); 

      } else {
        
        printDebug("GPS-DateTime INVALID");

      }
}

void printDebug(String message){

  if(DEBUG){
    Serial.println("DEBUG:-- " + message);
  }

}

void httpGetRequest(){
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){

      HTTPClient http;

      http.begin(serverName.c_str());
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {  
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void httpPostRequest() {

  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(serverName.c_str());
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Specify content-type header
      // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // // Send HTTP POST request
      // int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      // String httPostRequestData = "{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}";
      
      // String httPostRequestData ="{\"latitude\":\"22\",\"longitude\":\"22\",\"driver_id\":\"4\",\"company_id\":\"1\",\"task_id\":\"1\"}"; 
      String httPostRequestData = "{\"latitude\":\"" + String(latitude,6) + "\",\"longitude\":\"" + String(longitude,6) + "\",\"driver_id\":\"4\",\"company_id\":\"1\",\"task_id\":\"1\"}";
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(httPostRequestData);

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
        
      http.end();
    }

    else {

      Serial.println("WiFi Disconnected");

    }

    lastTime = millis();
  }

}