import serial  # 引用pySerial模組
import numpy as np
import matplotlib.pyplot as pyplot

COM_PORT = 'COM11'    # 指定通訊埠名稱
BAUD_RATES = 115200    # 設定傳輸速率
ser = serial.Serial(COM_PORT, BAUD_RATES)   # 初始化序列通訊埠

x = []
y = [] 
fig=pyplot.figure()
ax = fig.add_subplot(111)   
line1, = ax.plot(x, y, 'o-', lw=10, markersize=20)  #改變線條1樣式

pyplot.xlabel('number')      #改變x軸標題

pyplot.ylabel('AnalogRead')  #改變y軸標題       

pyplot.title('Title')        #改變圖表標題   
pyplot.axis([-2000, 2000, -2000, 2000])
pyplot.grid(axis = 'y')       
pyplot.ion()                   
pyplot.show()                  
i=0

class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s
    
    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)

try:
    while True:
        while ser.in_waiting:          # 若收到序列資料…
            
            i+=1
            #data_raw = ser.readline()  # 讀取一行
            #data=ser.readline().strip().decode('ascii')
            test=ReadLine(ser)
            data=test.readline()

            data = data.decode()   # 用預設的UTF-8解碼
            #print(i)
            #print(data)
            data=data.split(',')      #將csv格式資料拆到容器cmd的四個位置 
            x.append(int(data[0]))        #插新點在x容器的最後方
            y.append(int(data[1]))  #插新點在y1容器的最後方
            if i > 3 :        #可以保持畫面由左到右共100點，取消掉這一串後圖形可以累加數值
                x.pop(0)        #刪掉x容器的第一個數值           
                y.pop(0)       #刪掉y1容器的第一個數值
            line1.set_data(x, y)  
            ax.autoscale_view(True,True,True)   
            fig.canvas.draw()
            pyplot.pause(0.00001)

except KeyboardInterrupt:
    ser.close()    # 清除序列通訊物件
    print('再見！')

