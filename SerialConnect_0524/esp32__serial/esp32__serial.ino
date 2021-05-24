#include <HardwareSerial.h>    //导入ESP32串口操作库,使用这个库我们可以把串口映射到其他的引脚上使用

//我们测试需要用到的串口，ESP32一共有3个串口，串口0应该是用于程序烧写和打印输出调试用的，所以我们只用1和2
HardwareSerial MySerial_stm32(1);  
unsigned short i;
unsigned short j;
char temp;
char stm32_get[250];  //接收区的字符就是被赋值存储在这里了
char *revbuf[10] = {0}; //存放分割后的子字符串，这里写的是最多10个，可以自己加
char cut_str[40];  //目标字符部分，大小自己决定，别小鱼自己的实际大小就行，比如b@[data]@[data]@[data]e  这就是我们的目标字符串，那么最多也就只有10几20个字符，写个30就行
int data_get_0;  //这个就用来表示我们接收的字符串切割后强制转化成数字的第零个
/**********************************************************************************************************************************/
/**********************************************************************************************************************************/
//字符串分割，传入参数：src：目标分割字符串，separator：分割指定符，传入'@'那就是按照'@'切分，dest：切割完后存储的位置，num：分割后子字符串的个数

void split(char *src,const char *separator,char **dest,int *num) {
  /*src 源字符串的首地址(buf的地址)  separator 指定的分割字符  dest 接收子字符串的数组  num 分割后子字符串的个数*/
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止
        return;
     if (separator == NULL || strlen(separator) == 0) //如未指定分割的字符串，直接终止
        return;
     pNext = (char *)strtok(src,separator); //必须使用(char *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
     while(pNext != NULL) {
          *dest++ = pNext;
          ++count;
         pNext = (char *)strtok(NULL,separator);  //必须使用(char *)进行强制类型转换
    }  
    *num = count;
}

//数据分析代码，相当于：'b'是开始关键字' ， 'e' ：是结束关键字 ，  '@'：后面接的是数据
//数据分析第一步：找出'b''e'两个关键字，把想要的内容裁剪下来
void cut1()
{
  int position_begin = 0;
  int position_end = 0;
  if(stm32_get[1] == 'b'){
    Serial.println(stm32_get[1]);
    position_begin = 1;
    }
  for(i = 1 ; i++ ; i <= 63)
    if(stm32_get[i] == 'e')
    {
      position_end = i;
      strncpy(cut_str,stm32_get+position_begin,position_end-position_begin);
      Serial.print("cut_str1:");Serial.println(cut_str);
      break;
      }  
}
//在上一步已经得到我们想要的部分的字符串之后开始切割
void cut2()
{
  int num = 0;  //分割后子字符串的个数
  split(cut_str,"@",revbuf,&num); //调用函数进行分割
  for(i = 0;i < num; i ++) {
    //lr_output_message("%s\n",revbuf[i]);
    Serial.print("cut2:  ");
    Serial.println(revbuf[i]);  //输出返回的每个内容
  }
}

void data_analyse(){
  Serial.println("data analyse!!!");
  cut1();  //根据开始标志位与结束标志位裁剪出相应的部分。
  cut2();  //经过两个切割之后revbuf里面的数值已经是我们想要的东西了
  data_get_0 = atof(revbuf[0]);  //这个就是强制转化，这个函数可以直接把我们的字符串转化成数字（小数也可以）
 memset(cut_str, 0, sizeof(cut_str));    //清空我们的目标字符串存储区域
 }
/**********************************************************************************************************************************/
/**********************************************************************************************************************************/



//这个函数适用于清理串口读取缓存区的缓存，其实也就是用读一个删除一个的方式清理，我还会打印出有没有进行清理，清理了哪些内容
void clear_usart_buffer(){
  i = MySerial_stm32.available();
  j = 0;
  if(i != 0){
    Serial.print("清空串口接收区的缓存......");
    Serial.println(MySerial_stm32.available());
    while(i--)
      MySerial_stm32.read();    //读取串口接收回来的数据但是不做处理只给与打印
    }
  else
    Serial.println("串口接收区的缓存为空!!!");
  }
   
/*    关于串口读取还有一些别的函数，用的不多但是找的资料给大家罗列在这里
 *    
 *     int peek(void);  //返回接收缓存中第一个字节数据，但并不从中删除它；
 *     void flush(void);  //等待串口收发完毕；
 *     下面是两个发送的，我没怎么用过，我一般直接使用println来发送
 *     size_t write(uint8_t);写数据到TX FIFO，在发送FIFO中的数据会自动输出到TX端口上；该方法有很多重载，可以用来发送字符串、长整型、整形；如果TX FIFO已满，则该方法将阻塞；
 *     size_t write(const uint8_t *buffer, size_t size);写数据到TX FIFO，如果发送FIFO已满，则该方法将阻塞；
 *     size_t setRxBufferSize(size_t);  //设置接收缓存大小（默认为256字节）；ESP32默认有128字节的硬件RX FIFO，在RX FIFO收到数据后会移送到上面的接收缓存中；这个挺重要的但是我们直接使用默认的就好
*/
void read_usart(){
  i = MySerial_stm32.available();  //返回目前串口接收区内的已经接受的数据量
  if(i != 0){
    j = 0;
    Serial.print("串口接收到的数据量为:");
    Serial.println(MySerial_stm32.available());
    while(i--){
      j++;
      temp = MySerial_stm32.read();   //读取一个数据并且将它从缓存区删除
      Serial.print(temp);    //读取串口接收回来的数据但是不做处理只给与打印
      stm32_get[j] = temp;  //把读取到的字符存储下来
      }
    Serial.println("");
    data_analyse();    //至关重要的一步，也就是把读取回来的数据进行分步截取直接拿到我们想要的数据，我下一篇博文会讲如何自己写这个函数
    }
  else
    Serial.println("串口接收区没有数据！！！");
  }

void setup() {
  Serial.begin(115200);
  //串口的开启，这里还可以传一些别的参数，但是我们只传入下面四个最重要的：波特率，默认SERIAL_8N1为8位数据位、无校验、1位停止位，后面两个分别为 RXD,TXD 引脚
  MySerial_stm32.begin(115200, SERIAL_8N1, 14, 15);
  //MySerial_stm32.updateBaudRate(9600);  //重新设置波特率；  
  //MySerial_stm32.end();  //失能串口，释放占用的资源
  clear_usart_buffer();  //清空接收区缓存函数
 
}

void loop() {
  read_usart();
  MySerial_stm32.println("usart test!!!");
  read_usart();
  delay(500);
  read_usart();
  delay(500);
  delay(500);
}
