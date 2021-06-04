# 4_wheel_balance_car
#ifndef ESP32_HTTP_H
#define ESP32_HTTP_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "DBDB";
const char* password =  "DBDBDBDBB";

String req;
String rsp;

HTTPClient http; 

void wifi_connect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void http_INIT(){
  http.begin("http://140.116.78.219:5000/esp32");
  http.addHeader("Content-Type", "text/plain");  
}  

void http_END(){
  http.end();  //Free resources
}

//using
//time_now = millis();
//Serial.println("[HTTP] get:"+http_GET("movement")+",used time:"+(millis()-time_now));
String http_GET(char* which){
  String HTTP="http://140.116.78.219:5000/esp32?which=";
  HTTP=HTTP+which;
  http.begin(HTTP);
  http.addHeader("Content-Type", "text/plain");  
  int httpCode = http.GET(); //Send the request
  if (httpCode > 0) { //Check the returning code
    String payload = http.getString();   //Get the request response payload
    //Serial.println(payload);             //Print the response payload
    return payload;
  }else{
    Serial.println("ERROR of GET");
  }
  http_END();
}

String http_POST(int speedL, int speedR, int sensor_x=0, int sensor_y=0){  
  // make json data
  StaticJsonDocument<200> doc;
  doc["speed"] = serialized("["+String(speedL)+","+String(speedR)+"]");
  doc["sensor"] = serialized("["+String(sensor_x)+","+String(sensor_y)+"]");
  String output;
  // serialize
  serializeJson(doc, output);
  // connect and add header
  http.begin("http://140.116.78.219:5000/esp32");
  http.addHeader("Content-Type","application/json");
  // send POST request
  int http_code = http.POST(output);
  if(http_code == 200)
  {
      // get respons json
      rsp = http.getString();
      DynamicJsonDocument doc(1024);
      // json deserialize
      deserializeJson(doc, rsp);
      JsonObject obj = doc.as<JsonObject>();
      String state = obj["state"];
      return state;
  }else{
    return String(http_code);
  }
}



#endif /* ESP32_HTTP_H */
