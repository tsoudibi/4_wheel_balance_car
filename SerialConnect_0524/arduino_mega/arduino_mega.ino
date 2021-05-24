int str[20];
void setup() {
Serial.begin(115200);
}
void loop() {
  teststring();
}

void testint(){
  int value = 5;
  Serial.println(value);
  delay(100);
}



void teststring(){
  int x = 1;
  int y = 1;
  int maxvalue = 2;
  Serial.print("b"); 
  for (int i = 0;i <= maxvalue; i++)
  {
    str[i] = x;
    x += 1;
    Serial.print(str[i]);
    Serial.print(",");
    delay(100);
    if(i >= maxvalue)
    { 
      Serial.println("e"); 
    } 
  }
}
