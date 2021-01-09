char cmd[15] ;
int testval = 0;

void setup(){

Serial.begin(9600);

pinMode(6,OUTPUT);
pinMode(8,OUTPUT);

pinMode(2,OUTPUT);
pinMode(4,OUTPUT);

Serial.setTimeout(10); // 設定為每10毫秒結束一次讀取(數字愈小愈快)

}

void loop(){

if(Serial.available()){

 Serial.readBytes(cmd,15);

 testval = atoi(cmd);
 if(testval < 10)
  testval = 0;
 Serial.print("PWM");
 Serial.println(testval);

 analogWrite(6,testval);
}

}
