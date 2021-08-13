#ifndef ESP32_HTTP_H
#define ESP32_HTTP_H

#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>


#define TIMEOUT 200

const char* ssid_1 = "DB_TOTOLink_25";
const char* password_1 =  "DBDBDBDBB";
const char* ssid_2 = "BOB";
const char* password_2 =  "12345678";
const char* ssid_3 = "DBDB";
const char* password_3 =  "DBDBDBDBB";

const String SERVER_IP = "http://140.116.78.219:5010" ;

#define led_pin_tx 12
#define led_pin_rx 27
#define led_pin_err 34

/* create client object*/
HTTPClient http;

/*set led pin*/
const int freq = 5000;
const int resolution = 8;


/*[HTTP]debug*/
unsigned long int time_now;

/* blink the led*/

void led_blink(int lednum){
  ledcWrite(lednum,10);
  delay(10);
  ledcWrite(lednum,0);
}


void WIFI_INIT(){
  /* set led mode*/
  ledcSetup(1, freq, resolution);
  ledcSetup(2, freq, resolution);
  ledcSetup(3, freq, resolution);
  /* set led pin*/
  ledcAttachPin(led_pin_tx, 1);
  ledcAttachPin(led_pin_rx, 2);
  ledcAttachPin(led_pin_err, 3);
  /* give 60 seconds to connect WIFI, else faild*/
  int i = 60;
  WiFi.begin(ssid_1, password_1);
  while (WiFi.status() != WL_CONNECTED && i) {
    delay(1000); i--;
    Serial.println("[Wi-Fi] Connecting to WiFi...");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Wi-Fi] Connection faild.");
  }else{
    Serial.println("[Wi-Fi] Connection successed.");
    /* first get request, begin and add header*/
    String HTTP = "/esp32?mode=SSID,SSID=";
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

/* 
 * only use get request and don't disconnect with server 
 * which call "http persistence" 
 * 
 * control = analog control signal to motor
 * speed = encoder RMP to esp32
 * sensor = mass center from arduino to esp32 */
 
/* send data to sever */
String server_update_http(int controlL, int controlR, int speedL = 0, int speedR = 0, int sensor_x = 0, int sensor_y = 0){
  /* connect and add header */
  String HTTP="/esp32?mode=update";
  HTTP = SERVER_IP + HTTP + 
    "&controlL=" + String(controlL) +
    "&controlR=" + String(controlR) +
    "&speedL=" + String(speedL) +
    "&speedR=" + String(speedR) +
    "&sensor_x=" + String(sensor_x) +
    "&sensor_y=" + String(sensor_y) ;
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
    /* blink the led(rx)*/
    led_blink(2);
    return payload;
  }else{
    /* if respone is bad, return httpCode as String */
    /* blink the led(error)*/
    led_blink(3);
    return String(httpCode); 
  }
  http.end();
}

/* get data from sever */
String server_gather_http(char* which){
  /* connect and add header */
  String HTTP="/esp32?mode=gather&which=";
  HTTP = SERVER_IP + HTTP + which;
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
    /* blink the led(rx)*/
    led_blink(2);
    return payload;
  }else{
    /* if respone is bad, return httpCode as String */
    /* blink the led(error)*/
    led_blink(3);
    return String(httpCode); 
  }
  http.end();
}



#endif /* ESP32_HTTP_H */
