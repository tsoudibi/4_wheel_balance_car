#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H
#include <HardwareSerial.h>
#include "esp32_http.h"
#include "motor_control.h"
#include "arduino_connect.h"
#include "pid.h"



void sensor_control(){
  /* update data from encoder and sensor */
  update_data_all();
  /* encoder HZ from arduino*/
  int ISR_HZ_L = get_serial_data(1);
  int ISR_HZ_R = get_serial_data(2);
  /* motor control command from arduino */
  int command_L = get_serial_data(5);
  int command_R = get_serial_data(6);
  /* mass center at map from arduino */
  int map_x = get_serial_data(3);
  int map_y = get_serial_data(4);
  /* PID control 
   * first set the command and encoder, then caculate*/
  set_EncodingHZ_PID(ISR_HZ_L,ISR_HZ_R);
  set_CommandHZ_PID(command_L,command_R);
  PIDcontrol_left();
  PIDcontrol_right();
  /* after PID */
  int after_PID_L = get_PID_result('l');
  int after_PID_R = get_PID_result('r');
  /* RUN the motor */
  motor_control(after_PID_L,after_PID_R);
  /* print data and send to server */

}



#endif /* SENSOR_CONTROL_H */
