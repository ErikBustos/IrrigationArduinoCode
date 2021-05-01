#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <NTPClient.h>

#define pin34 34 //moisture sensor
#define pin32 32  //photoresistor sensor
#define pin35 35 //Rain Detector
#define pin27 27//WaterFlow sensor
#define RELAY 14 //Relay
#define pin13 13 //Temp Humidity

const char* ssid     = "IZZI-FAC9";
const char* password = "704FB841FAC9";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//Your Domain name with URL path or IP address with path
String serverHost = "http://processiotinput-env.eba-qusd4sbh.us-east-1.elasticbeanstalk.com";

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String macid;
int moisturevalue, rainValue, waterValue, airTemp, photoresistor_val;
float humidityValue, temperatureValue;

DHT dht1(pin13, DHT11); //El azul

void setup() {
  Serial.begin(115200);
  connectToWiFi(); //Connect to WiFi 
  //pinMode(RELAY, OUTPUT);  //Relay setup
  //digitalWrite(RELAY, HIGH);
  Serial.println("Configuring sensors setup...");
  
  dht1.begin();
  timeClient.begin();
  timeClient.setTimeOffset(-18000);
}

void loop() {
   while(!timeClient.update()) {
    timeClient.forceUpdate();
    }

   // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();

  //Read Sensors
  moisturevalue = analogRead(pin34);
  photoresistor_val= analogRead(pin32);
  rainValue= analogRead(pin35);
  waterValue = analogRead(pin27);
  temperatureValue = dht1.readTemperature();
  humidityValue = dht1.readHumidity();
  
  //Get macid 9C:9C:1F:C7:F7:E4
  macid= WiFi.macAddress();
  
  //Post Data to Server
  postToServer();
        
  //Receive if Irrigation is needed
  
  //Irrigate
  digitalWrite(RELAY, HIGH);   //turns the RELAY on

  digitalWrite(RELAY, HIGH);
  delay(1800000); // 30 minutes delay
}

void connectToWiFi() {
  //Connect to WIFI
  WiFi.begin(ssid, password);  
  Serial.println("Connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

//Post to server
void postToServer() {
  if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;
      String serverPath = serverHost + "/api/processInputData";
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath);
      // Specify content-type header
      http.addHeader("Content-Type", "application/json");

      //Set JSON
      StaticJsonDocument<200> jsonDoc;
      jsonDoc["airHumidity"] = humidityValue;
      jsonDoc["airTemperature"] = temperatureValue ;
      jsonDoc["light"] = photoresistor_val ;
      jsonDoc["soilHumidity"] = moisturevalue ;
      jsonDoc["rainDrops"] = rainValue ;
      jsonDoc["timestamp"] = formattedDate ;
      jsonDoc["macid"] = macid ;      
      
      String output;
      serializeJson(jsonDoc, output);
      Serial.println(output);
      //String jsondata= "{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"Moisture\":\" + moisturevalue +\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}";

      // Send HTTP POST request
      int httpResponseCode = http.POST(output);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print(http.getString());

     // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}
