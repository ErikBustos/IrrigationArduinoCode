#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define pin15 15 //moisture sensor
#define pin2 2  //photoresistor sensor
#define pin4 4 //Rain Detector
#define pin27 27//WaterFlow sensor
#define RELAY 14 //Relay

const char* ssid     = "IZZI-FAC9";
const char* password = "704FB841FAC9";

//Your Domain name with URL path or IP address with path
String serverName = "https://project-toolchain-iotpushtodb.mybluemix.net";

int moisturevalue, photoresistor_val, rainValue, waterValue, airTemp;

void setup() {
  Serial.begin(115200);
  connectToWiFi(); //Connect to WiFi
  //pinMode(RELAY, OUTPUT);  //Relay setup
  //digitalWrite(RELAY, HIGH);
  Serial.println("Configuring sensors setup...");

}

void loop() {
   delay(10000); // 10 seconds delay
   
  //Read Sensors
  moisturevalue = analogRead(pin15);
  photoresistor_val= analogRead(pin2);
  rainValue= analogRead(pin4);
  waterValue = analogRead(pin27);
  
  //Post Data to Server
  postToServer();
        
  //Receive if Irrigation is needed
  
  //Irrigate
  digitalWrite(RELAY, HIGH);   //turns the RELAY on

  //Print vals
  Serial.print("Moisture Val: ");
  Serial.println(moisturevalue);
  Serial.print("Photoresistor Val: ");
  Serial.println(photoresistor_val);
  Serial.print("Rain Value: ");
  Serial.println(rainValue);
  Serial.print("Water Flow  Value: ");
  Serial.println(waterValue);

  digitalWrite(RELAY, HIGH);
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
      String serverPath = serverName + "/api/pushtodb";
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath);
      // Specify content-type header
      http.addHeader("Content-Type", "application/json");

      //Set JSON
      StaticJsonDocument<200> jsonDoc;
      jsonDoc["sensor"] = "gps";
      jsonDoc["soilHumidity"] = moisturevalue ;
      jsonDoc["soilHumidity"] = airTemp ;
      jsonDoc["light"] = photoresistor_val ;
      jsonDoc["rainDrops"] = rainValue ;

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
