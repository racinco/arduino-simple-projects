#include "SoftwareSerial.h"
 
// Define the Arduino pins for software serial communication
const int RXPin = 16;
const int TXPin = 17;
 
// Set the default baud rate for the NEO-6M GPS module
const int GPSBaud = 9600;
 
// Create a software serial object for GPS communication
SoftwareSerial gpsSerial(RXPin, TXPin);
 
void setup()
{
// Start the hardware serial port for communication with the computer
Serial.begin(9600);
 
// Start the software serial port for GPS communication
gpsSerial.begin(GPSBaud);
}
 
void loop()
{
    // Check if there is data available from the GPS module
    while (gpsSerial.available() > 0)
    {
    // Serial.println("START");
    // Read the incoming data and send it to the computer
    char data = gpsSerial.read();
    Serial.write(data);
    }

}