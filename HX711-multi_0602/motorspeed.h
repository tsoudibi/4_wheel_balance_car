#ifndef motorspeed_h
#define motorspeed_h

//motor 控制
int motor(int map_X,int map_Y) {
  
  int motor_mode;

  ////////////前進時//////////////////
  
  if(map_Y < -300 && map_Y >= -350){
    if(map_X < 100 && map_X >=-100)
    motor_mode = 1;//原地不動 
    /*if(map_X >= 110 && map_X <150 || map_X < -50 && map_X > -150) 
    motor_mode = 2;//直線加*/
    if(map_X > 100)
    motor_mode = 4;//右轉
    if(map_X < -100)
    motor_mode = 3;//左轉      
  }
  if(map_Y > -300 && map_Y <= 200){
    if(map_X < 45 && map_X >=-45)
      motor_mode = 2;//直線加速
    if(map_X < -45)
      motor_mode = 3;//左轉
    if(map_X >= 45)
      motor_mode = 4;//右轉
    }
  if(map_Y > 200) {
    if(map_X < 150 && map_X >= -150)
     motor_mode = 2;//直線前進
    if(map_X < -150)
     motor_mode = 3;//左轉
    if(map_X > 150)
     motor_mode = 4;//右轉  
    }
    
  ///////////////後退時////////////////////
    
  if(map_Y > -200 && map_Y <= -300){
    if(map_X < 100 && map_X >=-100)
    motor_mode = 1;//原地不動
    if(map_X >= 100 && map_X < 200 || map_X < -100 && map_X > -200) 
    motor_mode = 5;//直線加速
    if(map_X > 200)
    motor_mode = 7;//右轉
    if(map_X < -200)
    motor_mode = 6;//左轉      
  }
  if(map_Y < -300 && map_Y >= -400){
    if(map_X < 200 && map_X >=-200)
      motor_mode = 5;//直線加速
    if(map_X < -200)
      motor_mode = 6;//左轉
    if(map_X >= 200)
      motor_mode = 7;//右轉
    }
  if(map_Y < -400) {
    if(map_X < 150 && map_X >= -150)
     motor_mode = 5;//直線前進
    if(map_X < -150)
     motor_mode = 6;//左轉
    if(map_X > 150)
     motor_mode = 7;//右轉  
    }
  return motor_mode;      
}

void speeddef(int motor_mode,int map_Y, int map_X){
  int speed_l;
  int speed_r;
  switch(motor_mode){
    case(1):
      speed_l = 0;
      speed_r = 0;
    break;
    case(2):
      speed_l = (map_Y+100)*0.6;
      speed_r = (map_Y+100)*0.6;
    break;
    case(3):
      speed_l = 0.7*(map_Y+100);
      speed_r = 0.7*(map_Y+100)-map_X;
    break;
    case(4):
      speed_l = 0.7*(map_Y+100)+map_X;
      speed_r = 0.7*(map_Y+100);
    break;
    /*case(5):
      speed_l = -(map_Y+100);
      speed_r = -(map_Y+100);
     break; 
    case(6):
      speed_l = 0.7*map_X-0.7*map_Y;
      speed_r = 0.7*map_Y;
    break;
    case(7):
      speed_l = 0.7*map_X-0.7*map_Y;
      speed_r = 0.7*map_Y;*/        
  }
  Serial.print(motor_mode);
  Serial.print(",");
  Serial.print(speed_l);
  Serial.print(",");
  Serial.println(speed_r);
}

#endif
