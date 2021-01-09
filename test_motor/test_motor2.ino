void setup() {
  // put your setup code here, to run once:
  pinMode(6,OUTPUT);
  pinMode(8,OUTPUT);

  pinMode(2,OUTPUT); 
  pinMode(4,OUTPUT);
  
  Serial.begin(9600);
  analogWrite(6,0);
  analogWrite(2,0);

    
  delay(1000);
 
   
  //digitalWrite(8,LOW);
  analogWrite(6,150);
  delay(100);
  analogWrite(6,60);
  
  //digitalWrite(4,HIGH);
  analogWrite(2,150);
  delay(100);
  analogWrite(2,60);
   
}

void loop() {
  // put your main code here, to run repeatedly:
  /*delay(1000);
analogWrite(6,100);*/
}
