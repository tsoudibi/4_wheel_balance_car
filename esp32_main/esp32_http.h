#ifndef ESP32_HTTP_H
#define ESP32_HTTP_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char* ssid_1 = "DB_TOTOLink_25";
const char* password_1 =  "DBDBDBDBB";
const char* ssid_2 = "BOB";
const char* password_2 =  "12345678";
const char* ssid_3 = "PawPatrolll";
const char* password_3 =  "as80894512";


String req;
String rsp;

HTTPClient http; 

void wifi_connect(){
  int i=100;
  WiFi.begin(ssid_2, password_2);
  while (WiFi.status() != WL_CONNECTED && i) {
    delay(1000);i--;
    Serial.println("[WIFI] Connecting to WiFi1..");
  }
  /*WiFi.begin(ssid_2, password_2);i=3;
  while (WiFi.status() != WL_CONNECTED and i) {
    delay(1000);i--;
    Serial.println("[WIFI] Connecting to WiFi2..");
  }
  WiFi.begin(ssid_3, password_3);i=3;
  while (WiFi.status() != WL_CONNECTED and i) {
    delay(1000);i--;
    Serial.println("[WIFI] Connecting to WiFi3..");
    if(i<0){
      Serial.println("[WIFI] failed to connect any WIFI");
      i=100;
    }
  }*/
  if (i!=100) Serial.println("[WIFI] Connected to the WiFi network");
  String HTTP="http://140.116.78.219:5005/esp32?SSID=";
  HTTP=HTTP+WiFi.SSID();
  http.begin(HTTP);
  http.addHeader("Content-Type", "text/plain");  
  int httpCode = http.GET(); //Send the request
  String payload;
  if (httpCode > 0) { //Check the returning code
    payload = http.getString();   //Get the request response payload
    Serial.println("[WIFI] server response: "+payload);             //Print the response payload
  }else{
    payload = http.getString();   //Get the request response payload
    Serial.println("[WIFI] ERROR of response when first connecting to server");
    Serial.println(payload);
  }
}

void http_INIT(){
  http.begin("http://140.116.78.219:5005/esp32");
  http.addHeader("Content-Type", "text/plain");  
}  

void http_END(){
  http.end();  //Free resources
}

//using
//time_now = millis();
//Serial.println("[HTTP] get:"+http_GET("movement")+",used time:"+(millis()-time_now));
String http_GET(char* which){
  String HTTP="http://140.116.78.219:5005/esp32?which=";
  HTTP=HTTP+which;
  http.begin(HTTP);
  http.addHeader("Content-Type", "text/plain");  
  int httpCode = http.GET(); //Send the request
  if (httpCode > 0) { //Check the returning code
    String payload = http.getString();   //Get the request response payload
    //Serial.println(payload);             //Print the response payload
    return payload;
  }else{
    String msg = "[ERRO] ERROR of GET when which = ";
    msg = msg + which;
    Serial.println(msg);
  }
  http_END();
}

//control = analog control signal to motor
//speed = encoder RMP to esp32
//sensor = mass center from arduino to esp32

String http_POST(int controlL, int controlR,int speedL=0, int speedR=0, int sensor_x=0, int sensor_y=0){  
  // make json data
  StaticJsonDocument<200> doc;
  doc["control"] = serialized("["+String(controlL)+","+String(controlR)+"]");
  doc["RPM"] = serialized("["+String(speedL)+","+String(speedR)+"]");
  doc["sensor"] = serialized("["+String(sensor_x)+","+String(sensor_y)+"]");
  String output;
  // serialize
  serializeJson(doc, output);
  // connect and add header
  http.begin("http://140.116.78.219:5005/esp32");
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
