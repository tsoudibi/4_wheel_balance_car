#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "esp32_http.h"
#include "motor_control.h"
#include "arduino_connect.h"
#include "pid.h"

#define threshold 20

/* save the HZ of motor should be */
/*The smallest speed is 7*/
int cam_HZ_L = 0;
int cam_HZ_R = 0;

/* backup HZ in case of error*/
int cam_HZ_L_backup = 0;
int cam_HZ_R_backup = 0;

/* store string as char array*/
char HZ_char_array[50] ;

void camera_control(){
  /* update data from encoder and sensor */
  update_data_all();

  /* get HZ command from sever */
  String HZ_string = http_GET("cam_HZ");

  /* to char array */
  int HZ_string_len = HZ_string.length() + 1;   //sensor
  HZ_string.toCharArray(HZ_char_array, HZ_string_len); //sensor
  
  /* split */
  char *arr_1;
  arr_1 = strtok(HZ_char_array, ",");
  int i = 0;
  int value_1[3];
  while (arr_1 != NULL)
  {
    value_1[++i] = atoi(arr_1); 
    arr_1 = strtok(NULL, ",");
  }

  /* if error, use bacup command */
  if (cam_HZ_L > 100 || cam_HZ_R > 100 || cam_HZ_L < 0 || cam_HZ_R < 0){
    cam_HZ_L = cam_HZ_L_backup;
    cam_HZ_R = cam_HZ_R_backup;
  }else{
    cam_HZ_L = value_1[1];
    cam_HZ_R = value_1[2];
  }
  
  /* store backup command */
  cam_HZ_L_backup = cam_HZ_L;
  cam_HZ_R_backup = cam_HZ_R;
  
  

  /* print data and send to server */
  /*time_now = millis();
  Serial.println( "[HTTP] post:"+http_POST(HZ_L,HZ_R,ISR_HZ_L,ISR_HZ_R,map_x,map_y)+" ,used time:"+(millis()-time_now));*/
  
}


int return_command_hz_camera(char which)
{
  if (which == 'l')
    return cam_HZ_L;
  if (which == 'r')
    return cam_HZ_R;
}

#endif /* CAMERA_CONTROL_H */
