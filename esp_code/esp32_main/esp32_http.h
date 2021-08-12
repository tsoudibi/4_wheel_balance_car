#ifndef ESP32_HTTP_H
#define ESP32_HTTP_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>


#define TIMEOUT 170

const char* ssid_1 = "DB_TOTOLink_25";
const char* password_1 =  "DBDBDBDBB";
const char* ssid_2 = "BOB";
const char* password_2 =  "12345678";
const char* ssid_3 = "DBDB";
const char* password_3 =  "DBDBDBDBB";

const String SERVER_IP = "http://140.116.78.219:5005" ;

#define led_pin_tx 12
#define led_pin_rx 27
#define led_pin_err 35

/* create client object*/
HTTPClient http;

/*set led pin*/
const int freq = 5000;
const int resolution = 8;

void WIFI_INIT(){
  /* set led mode*/
  ledcSetup(1, freq, resolution);
  ledcSetup(2, freq, resolution);
  ledcSetup(3, freq, resolution);
  /* set ;ed pin*/
  ledcAttachPin(led_pin_tx, 1);
  ledcAttachPin(led_pin_rx, 2);
  ledcAttachPin(led_pin_err, 3);
  /* give 60 seconds to connect WIFI, else faild*/
  int i = 60;
  WiFi.begin(ssid_2, password_2);
  while (WiFi.status() != WL_CONNECTED && i) {
    delay(1000); i--;
    Serial.println("[Wi-Fi] Connecting to WiFi...");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Wi-Fi] Connection faild.");
  }else{
    Serial.println("[Wi-Fi] Connection successed.");
    /* first get request, begin and add header*/
    String HTTP = "/esp32?SSID=";
    HTTP = SERVER_IP + HTTP + WiFi.SSID();
    http.begin(HTTP);
    http.addHeader("Content-Type", "text/plain");  
    /* set client request timeout and sent SSID to server*/
    http.setTimeout(TIMEOUT);
    http.setConnectTimeout(TIMEOUT);
    int httpCode = http.GET(); //Send the request
    /* handle the server response*/
    String payload;
    if (httpCode > 0) {
      /* if get correct respone */
      payload = http.getString();   //Get the request response payload
      Serial.println("[Wi-Fi] server response: " + payload);             //Print the response payload
    }else{
      /* cannot send SSID to server, but keep going */
      Serial.println("[Wi-Fi] ERROR of response when first connecting to server");
      Serial.println("[Wi-Fi] keep going");
      if(httpCode == -1){ /* HTTPC_ERROR_CONNECTION_REFUSED */
        Serial.println("[Wi-Fi] Server offline, closing :( ");
        http.end();
      }
    }
  }
}  

String http_GET(char* which){
  /* connect and add header */
  String HTTP="/esp32?which=";
  HTTP = SERVER_IP + HTTP +which;
  http.begin(HTTP);
  http.addHeader("Content-Type", "text/plain"); 
  /* set client request timeout*/
  http.setTimeout(TIMEOUT);
  http.setConnectTimeout(TIMEOUT);
  /* sent GET request */ 
  int httpCode = http.GET(); //Send the request
  /* handle the server response*/
  if (httpCode > 0) { //Check the returning code
    /* if respone is normal, return respone as string */
    String payload = http.getString();   //Get the request response payload
    /* blink the led*/
    ledcWrite(2,10);
    delay(10);
    ledcWrite(2,0);
    return payload;
  }else{
    /* if respone is bad, return httpCode as String */
    /* blink the led*/
    ledcWrite(3,10);
    delay(10);
    ledcWrite(3,0);
    return String(httpCode); 
  }
  http.end();
}


/* control = analog control signal to motor
 * speed = encoder RMP to esp32
 * sensor = mass center from arduino to esp32 */

String http_POST(int controlL, int controlR,int speedL=0, int speedR=0, int sensor_x=0, int sensor_y=0){  
  /* create JSON formate*/
  StaticJsonDocument<200> doc;
  doc["control"] = serialized("["+String(controlL)+","+String(controlR)+"]");
  doc["RPM"] = serialized("["+String(speedL)+","+String(speedR)+"]");
  doc["sensor"] = serialized("["+String(sensor_x)+","+String(sensor_y)+"]");
  String output;
  /* serialize */
  serializeJson(doc, output);
  /* connect and add header */
  String HTTP="/esp32";
  HTTP = SERVER_IP + HTTP ;
  http.begin(HTTP);
  http.addHeader("Content-Type","application/json");
  /* set client request timeout*/
  http.setTimeout(TIMEOUT);
  http.setConnectTimeout(TIMEOUT);
  /* send POST request */
  int http_code = http.POST(output);
  /* handle the server response*/
  if(http_code == 200){
      /* if respone is normal, return respone as string */
      /* get respons json */
      String rsp = http.getString();
      DynamicJsonDocument doc(1024);
      /* json deserialize */
      deserializeJson(doc, rsp);
      JsonObject obj = doc.as<JsonObject>();
      String state = obj["state"];
      /* blink the led*/
      ledcWrite(1,10);
      delay(10);
      ledcWrite(1,0);
      return state;
  }else{
    /* if respone is bad, return http_code */
    /* blink the led*/
    ledcWrite(3,10);
    delay(10);
    ledcWrite(3,0);
    return String(http_code);
  }
  http.end();
}



#endif /* ESP32_HTTP_H */
