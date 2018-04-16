/*
Author: Dereck Portela

Title: Real-time temperature and humidity monitoring system of a server room

Description:
This program was designed to monitor a server room which needed to be in a certain temperature and humidity, otherwise 
the servers could be damaged.

To build the prototype were used: 
1. An ESP8266 with Wifi connection, 
2. A DHT22 which is a temperature and humidity sensor,
3. A cooler to cool down the room in case of high temperatures
4. A humidifier to moisten the room in case of low humidity
5. A speaker to be used as an alarm
6. Leds, Resitors and jumpers

The program monitor the temperature and humidity through the sensor, in case of high temperatures, low humidity, or both
the microcontroller actives the alarm, the cooler, the humidifier or both depending on the situation. Subsequently the ESP8266
sends an alert message to my (or whatever which the user desire) twitter account. When everything is stabilized another message 
is also sent. All datas can be monitored in real time via an IoT platform called "ThingSpeak".
 
*/





#include <DHT.h>  // Libraries to read the sensor and connect ESP8266 to WiFi
#include <ESP8266WiFi.h>
#define SensorPin 17 
#define ReadTime 1000 // Time to read the sensor and send datas to ThingSpeak
#define DHT22_READ 5 //Sensor pin
#define DHTTYPE DHT22 // Kind of sensor which was used
#define RedLedPin 0
#define BlueLedPin 2
#define BuzzerPin 12
#define CoolerPin 13
#define HumidifierPin 14

DHT dht(DHT22_READ, DHTTYPE); 
  
int TemperatureMeasurement(); // Function prototype to read the temperature
int HumidityMeasurement();  //Function prototype to read the humidity
void TwitterAlert(String data); //Function prototype to send the alert via twitter
void httpRequest(); // function prototype to send datas to ThingSpeak
void TemperatureAlert();
void HumidityAlert();
void StableTemperature();
void StableHumidity();

boolean TemperatureWarning = false, HumidityWarning = false; //Boolean variables to verify if the datas were sent correctly
int counter = 110; // Counter which will work as a "tag" to trick twitter's blocker of messages
long int CurrentMillis = 0, PrevisiousMillis = 0; // Variables to control the timmer
const char* ssid = "otavio"; // Network SSID
const char* password = "otavio6646"; // Network password
String Token = "MNMKFJBULU9BY7ER"; 
char TS_SERVER[] = "api.thingspeak.com"; // Sever where the datas were stored

WiFiClient client; 

void setup()
{
  
Serial.begin(9600);
pinMode(RedLedPin, OUTPUT); //Setting up the pins on ESP8266
pinMode(BlueLedPin, OUTPUT);
pinMode(BuzzerPin, OUTPUT);
pinMode(CoolerPin, OUTPUT);
pinMode(HumidifierPin, OUTPUT);
  
delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("Server started");
  Serial.println(WiFi.localIP()); 
}

void loop()
{
  CurrentMillis = millis(); //Start the timmer
  int AverageTemperature; // Variable to read the temperature
  int AverageHumidity; // Vatiable to read the humidity
  String tag = String(counter); // Convert the counter into string to be attached with the messages which will be sent to twitter
  
  if ((CurrentMillis - PrevisiousMillis) > ReadTime) {
    PrevisiousMillis = CurrentMillis; // update the timmer
    AverageTemperature = TemperatureMeasurement(); //Call the function to read the temperature
    AverageHumidity = HumidityMeasurement(); //Call The function to read humidity
    Serial.print("Temperature ---> ");
    Serial.println(AverageTemperature); //Print the temperature on serial monitor just for debugging
   if((AverageTemperature > 27) && (TemperatureWarning == false)){

      //High temperature alert
      TwitterAlert( tag + " " + "@DereckPortela High Temperature Alert! Automatic cooler has been enabled!");
      TemperatureAlert();
      TemperatureWarning = true;
    
    }
   
   else if ((HumidityWarning == false) && (AverageHumidity < 46)){
      
      
      //Low humidity alert
      TwitterAlert( tag + " " + "@DereckPortela Low Humidity Alert! Automatic humidifier has been enabled!");
      HumidityAlert();
      counter++;
      HumidityWarning = true;
    }
   
   else if ((HumidityWarning == true) && (AverageHumidity >= 46)){
      
      
      //warning when the Humidity is stabilized
      TwitterAlert( tag + " " + "@DereckPortela Stable Humidity! Automatic humidifier has been disabled!");
      StableHumidity();
      counter++;
      HumidityWarning = false;
    }
    else if ((TemperatureWarning == true) && (AverageTemperature <= 27)){
      
      
      //Warning when the temperature is stabilized
      TwitterAlert( tag + " " + "@DereckPortela Stable temperature! Automatic cooler has been disabled!");
      StableTemperature();
      counter++;
      TemperatureWarning = false;
    }
   
   if(client.connect(TS_SERVER,80)) { 
  
   //The process to send all datas to server (ThingSpeak)
    String postStr = ""; 
           postStr += Token;
           postStr +="&amp;field1=";
           postStr += String(AverageTemperature);
           postStr += "&amp;field2=";
           postStr += String(AverageHumidity); 
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+Token+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
  }
  
  }
  
}

int TemperatureMeasurement() {
  
  int  temperature = 0;
  //Read the temperature 
  do{
  temperature = dht.readTemperature();
  }
  while(temperature > 65);
  
  return temperature;

}
int HumidityMeasurement() {
  //read the humidity
  int  humidity = 0;
  do{
  humidity = dht.readHumidity();
  }
  while(humidity > 100);
  return humidity;

}
void TwitterAlert(String data){
    
   //The process to send the alert message to twitter	
    if(client.connect(TS_SERVER,80)){ 
    Serial.println("Sending a TWITTER!!");
    String TwitterToken = "WA23O4P6WDZ8I29N";   
    data = "api_key="+TwitterToken+"&status="+data;
    Serial.println(data);
    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");

    client.print(data);
   }  
}
void TemperatureAlert(){
  
  //Active the alarm and cooler in case of high temperatures
  digitalWrite(RedLedPin, HIGH);
  digitalWrite(CoolerPin, HIGH);
  digitalWrite(BuzzerPin, HIGH);
  
  }
 void HumidityAlert(){
  //Active Humidifier and the alarm in case of low humidity
  digitalWrite(BlueLedPin, HIGH);
  digitalWrite(HumidifierPin, HIGH);
  digitalWrite(BuzzerPin, HIGH);
  
  }
 void StableTemperature(){
    //turn off the alarm and cooler when the temperature is stabilized
    digitalWrite(RedLedPin, LOW);
    digitalWrite(CoolerPin, LOW);
    digitalWrite(BuzzerPin, LOW);
  }
 void StableHumidity(){
     //turn off the alarm and humidifier when the humidity is stabilized	
     digitalWrite(BlueLedPin, LOW);
     digitalWrite(HumidifierPin, LOW);
  }
