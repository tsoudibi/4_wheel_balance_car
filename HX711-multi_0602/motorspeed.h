#ifndef motorspeed_h
#define motorspeed_h


#include "Queue.h"
#include "arduino.h"

Queue<unsigned long> queue_map_X(20);
Queue<unsigned long> queue_map_Y(20);

int flag = 0;
int count = 0;

/*reset point*/
int map_x_ori,map_y_ori;

/*reset speed*/
int speed_l = 0;
int speed_r = 0;

/*speed control by HX711*/
void speed_control(int map_x_ori,int map_y_ori,int map_X, int map_Y){
  
  int FlagtoDetectchange = 0;

  //加速
  if (map_Y - map_y_ori > 600 && FlagtoDetectchange == 0)
  {
    speed_l++;
    speed_r++;
    FlagtoDetectchange = 1;
  }
  
  
  //減速
  if(map_Y - map_y_ori < -300 && FlagtoDetectchange == 0)
  {
    speed_l--;
    speed_r--;
    FlagtoDetectchange = 1;
  }
  
  //左右轉
    if (-300 < map_Y - map_y_ori < 600 && FlagtoDetectchange == 0) 
  {

    if(map_X - map_x_ori > 400)
    {
      speed_r = speed_l - 8;
    }
    if(map_X - map_x_ori < -400)
    {
      speed_l = speed_r - 8;
    }
    map_y_ori = map_Y;
    FlagtoDetectchange = 1;
  }  
}

/*save location in queue*/
void LocationtoQueue(int map_X,int map_Y)
{
  queue_map_X.push(map_X);
  queue_map_Y.push(map_Y);

  if(queue_map_X.count()>=15)
  	queue_map_X.pop();
  if(queue_map_Y.count()>=15)
  	queue_map_Y.pop();
}

/*reset coordinate first time*/
void ResetCoordinate(int map_X,int map_Y)
{ 
  //reset
  if(flag == 0)
  {
    if(abs(queue_map_X.peek() - map_X) > 10 && abs(queue_map_Y.peek()- map_Y) > 10)
    {
    flag = 1;
    //Serial.println("reset");
    delay(5000);
    map_x_ori = map_X;
    map_y_ori = map_Y;
    }
  }
  //whether come back to initialzation or not
  if(flag == 1)
  {
    if(abs(queue_map_X.peek() == 0) && abs(queue_map_Y.peek() == 0))
    {
      count++;
      
      //stop reset
      if(abs(queue_map_X.peek() > 30) || abs(queue_map_Y.peek() > 30))
        count = 0;
        
      //reset successfully
      if (count == 15)
        flag = 0;
    }
  }  
}

/*speed output*/
void SpeedOutput(int map_X,int map_Y)
{
  LocationtoQueue(map_X,map_Y);
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
    speed_control(map_x_ori,map_y_ori,map_X,map_Y);
  }
  
  //avoid negative speed
  if(speed_l < 0)
    speed_l = 0;
  if(speed_r < 0)
    speed_r = 0;

  //limit max speed
  if(speed_l > 10)
    speed_l = 10;
  if(speed_r >10)
    speed_r = 10;

  //transport data to esp32    
  Serial.println(String(map_X)+","+String(map_Y)+","+String(speed_l)+","+String(speed_r));
}
#endif
