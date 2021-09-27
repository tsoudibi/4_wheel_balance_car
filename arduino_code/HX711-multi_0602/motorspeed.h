#ifndef motorspeed_h
#define motorspeed_h


#include "Queue.h"
#include "arduino.h"

Queue<unsigned long> queue_map_X(5);
Queue<unsigned long> queue_map_Y(5);

/*set time*/
unsigned long time_count = 0;

int flag = 0;
int count = 0;

/*reset point*/
int map_x_ori,map_y_ori;

/*reset speed*/
int speed_l = 0;
int speed_r = 0;

/*set weight*/
int weight_esp32 = 0;
void get_weight(int weight)
{
  weight_esp32 = weight;
}
/*speed control by HX711*/
void speed_control(int map_x_ori,int map_y_ori,int map_X, int map_Y){
  
  int FlagtoDetectchange = 0;
  int speed_l_temp;
  int speed_r_temp;
  //left
  if (map_X - map_x_ori > 200 && FlagtoDetectchange == 0)
  {
    speed_l++;
    speed_r--;
    speed_l_temp = speed_l;
    speed_r_temp = speed_r;
    FlagtoDetectchange = 1;
    if(speed_l - speed_r >= 5)
    {
      speed_l = speed_l_temp;
      speed_r = speed_r_temp;
    }
  }
  
  //right
  if(map_X - map_x_ori < -200 && FlagtoDetectchange == 0)
  {
    speed_l--;
    speed_r++;
    speed_l_temp = speed_l;
    speed_r_temp = speed_r;
    FlagtoDetectchange = 1;
    if(speed_l - speed_r <= 5)
    {
      speed_l = speed_l_temp;
      speed_r = speed_r_temp;
    }
  }
  
  if (-300 < map_X - map_x_ori < 300 && FlagtoDetectchange == 0) 
  {
    //forward
    if(map_Y - map_y_ori > 300)
    {
      speed_r++;
      speed_l = speed_r;
    }
    //slow
    if(map_Y - map_y_ori < -200)
    {
       speed_r = 0;
       speed_l = 0;
    }
    FlagtoDetectchange = 1;
  }
  if(speed_r < 0)
    speed_r = 0;
  if(speed_l < 0)
    speed_l = 0;
}

/*save location in queue*/
/*void LocationtoQueue(int map_X,int map_Y)
{
  queue_map_X.push(map_X);
  queue_map_Y.push(map_Y);

  if(queue_map_X.count()>=10)
  	queue_map_X.pop();
  if(queue_map_Y.count()>=10)
  	queue_map_Y.pop();
}*/

/*reset coordinate first time*/
void ResetCoordinate(int map_X,int map_Y)
{ 
  //reset
  if(flag == 0)
  {
    if(weight_esp32 > 300)//total weight is bigger than 150
    {
    flag = 1;
    //Serial.println("reset");
    delay(3000);
    map_x_ori = map_X;
    map_y_ori = map_Y;
    }
  }
  //whether come back to initialzation or not
  if(flag == 1)
  {
    if(weight_esp32 < 300)
    {
      /*set all parameters to 0*/
      flag = 0;
      map_x_ori = 0;
      map_y_ori = 0;
      speed_l = 0;
      speed_r = 0;
      /*//stop reset
      if(weight_esp32 > 600)
        count = 0;
        
      //reset successfully and stop
      if (count == 5)
        flag = 0;
        map_x_ori = 0;
        map_y_ori = 0;*/
    }
  }  
}
/*void Reset_HX711(int map_X,int map_Y,int Weight_total)
{
  //LocationtoQueue(map_X,map_Y);
  ResetCoordinate(map_X,map_Y,Weight);
}*/
/*speed output*/
void SpeedOutput(int map_X,int map_Y)
{
  ResetCoordinate(map_X,map_Y);
  //靜止
  if (flag == 0)
  {
    speed_l = 0;
    speed_r = 0;
  }
  //啟動
  if (flag == 1)
  { 
  /*set time to smooth the speed change*/
    while(millis()- time_count > 1000)
    {
      /*speed control */
      speed_control(map_x_ori,map_y_ori,map_X,map_Y);
      time_count = millis();
      ResetCoordinate(map_X,map_Y);
    }
  }
  /*
  //avoid negative speed
  if(speed_l < 0)
    speed_l = 0;
  if(speed_r < 0)
    speed_r = 0;*/

  //limit max speed
  if(speed_l > 10)
    speed_l = 10;
  if(speed_r >10)
    speed_r = 10;
  
  //transport data to esp32    
  Serial.println(String(map_X - map_x_ori)+","+String(map_Y - map_y_ori)+","+String(speed_l)+","+String(speed_r)+","+String(flag)+","+String(weight_esp32));
}
#endif
