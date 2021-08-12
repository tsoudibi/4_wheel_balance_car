#include "esp32_http.h"
#include "motor_control.h"
#include "btn_control.h"
#include "camera_control.h"

#include "arduino_connect.h"
#include <esp32-hal-dac.h>
#include "pid.h"
#include "PID_v1.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

/*Serial data initialize*/
int Command_L = 0;
int Command_R = 0;
int ISR_HZ_L = 0;
int ISR_HZ_R = 0;
int map_x = 0;
int map_y = 0;
int DAC_R = 0;
int DAC_L = 0;

/*[HTTP]debug*/
unsigned long int time_now;

/*get control mode*/
int control_mode = 0;

/*declare the task for multitask*/
TaskHandle_t PID_Task;

/*PID function for muiltask*/
void PID_function(void * parameter)
{
  for (;;) {
    /* update data from encoder and sensor */
    update_data_all();
    /* encoder HZ from arduino*/
    ISR_HZ_L = get_serial_data(1);
    ISR_HZ_R = get_serial_data(2);
    /* mass center at map from arduino */
    map_x = get_serial_data(3);
    map_y = get_serial_data(4);
    /* make one motor reverse */
    digitalWrite(rev_pin, LOW);
    /* PID control and get result DAC command */
    PID(Command_L, Command_R, ISR_HZ_L, ISR_HZ_R);
    DAC_L = get_PID_result('l');
    DAC_R = get_PID_result('r');
    
    /* motor contorl */
    motor_control(DAC_L, DAC_R);

    /*to check use core*/
    //Serial.println(xPortGetCoreID());
  }
}

void setup() {
  /* serial set */
  Serial.begin(115200);
  setup_hard_serial();

  /* motor set */
  motor_setup();

  /* wifi connection set */
  WIFI_INIT();

  /* PID set */
  PID_setup();

  /*multitask setup*/
  xTaskCreatePinnedToCore(
    PID_function,       //Task function
    "PID control",      //Taskname
    10000,              //Task stack(usually use 10000)
    NULL,               //Task input
    0,                  //Task priortity(more big means that more need to deal with)
    &PID_Task,          //Task direction
    0                   //specify the core we use
  );
  /*neglect power problem to stop the infinite reset*/
   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
}

void loop() {
  delay(50);
  /* from server, get control mode */
  String response = http_GET("control_mode");
  if (response == "button") {
    control_mode = 1;
  } else if (response == "sensor") {
    control_mode = 2;
  } else if (response == "camera") {
    control_mode = 3;
  } else if (response == "-11" ) {
    /* error: timeout, HTTPC_ERROR_CONNECTION_REFUSED
       https://github.com/espressif/arduino-esp32/issues/1433#issuecomment-475875579 */
    Serial.println("[ERROR] server timeout when GET CONTOL MODE ");
  } else {
    Serial.print("[ERROR] wrong mode response from server (control mode), httpCode = ");
    Serial.println(response);
  }


  /* depend on mode, pick a function to run */
  switch (control_mode) {
    case 1:
      /* get the command HZ form btn control */
      btn_control(Command_L, Command_R);
      Command_L = return_command_hz_btn('l');
      Command_R = return_command_hz_btn('r');
      break;
    case 2:
      /* get the command HZ form Arduino (sensor */
      Command_L = get_serial_data(5);
      Command_R = get_serial_data(6);
      break;
    case 3:
      /* get the command HZ form camera control */
      camera_control();
      Command_L = return_command_hz_camera('l');
      Command_R = return_command_hz_camera('r');
      break;
  }
  delay(50);
  /* send data to server */
  time_now = millis();
  if (control_mode == 1) {
    String Response = http_POST(Command_L, Command_R, ISR_HZ_L, ISR_HZ_R, map_x, map_y);
    if (Response.toInt() == -11) {
      Serial.println("[ERROR] server timeout when POST data in mode 1 ");
    } else {
      String msg = "[ http] post:" + Response + " ,used time:" + (millis() - time_now);
      Serial.println(msg);
    }
  }
  if (control_mode == 2) {
    String Response = http_POST(Command_L, Command_R, ISR_HZ_L, ISR_HZ_R, map_x, map_y);
    if (Response.toInt() == HTTPC_ERROR_CONNECTION_REFUSED) {
      Serial.println("[ERROR] server timeout when POST data in mode 2 ");
    }
    Serial.println("[sensr] map:(" + String(map_x) + ", " + String(map_y) + ") speed:(" + String(Command_L) + ", " + String(Command_R) + ") ");
    String msg = "[ http] post:" + Response + " ,used time:" + (millis() - time_now);
    Serial.println(msg+"flag = "+String(get_serial_data(7)));
  }
  if (control_mode == 3) {
    String Response = http_POST(Command_L, Command_R, ISR_HZ_L, ISR_HZ_R, map_x, map_y);
    if (Response.toInt() == HTTPC_ERROR_CONNECTION_REFUSED) {
      Serial.println("[ERROR] server timeout when POST data in mode 2 ");
    }
    Serial.println("[sensr] map:(" + String(map_x) + ", " + String(map_y) + ") speed:(" + String(Command_L) + ", " + String(Command_R) + ") ");
    String msg = "[ http] post:" + Response + " ,used time:" + (millis() - time_now);
    Serial.println(msg);
  }
}
