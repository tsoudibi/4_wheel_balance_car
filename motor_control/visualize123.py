import serial 
import os
import numpy as np
import matplotlib.pyplot as plt

#define connection port, baud rates
ser = serial.Serial('COM3', 115200)  


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

plt.ion()
fig = plt.figure(figsize=(17,6))
t = []
t_now = 0
Left = []
Right = []

skip = 1
kill = True

try:    
    while kill:
        while ser.in_waiting :    
            if skip:      
                test = ReadLine(ser)
                data_in = test.readline()
                skip = skip-1
            else :
                test = ReadLine(ser)
                data_in = test.readline()
                data = data_in.decode('utf-8')   
                data_2 = data.split() 
                #print(len(data_2[0]))
                
                if len(data_2) > 0:
                    if len(data_2[0]) >= 3 and data_2[0][0]!=',' and data_2[0][0]!='0':
                        data_2 = data_2[0].split(',')     
                        t_now = t_now + 1
                        t.append(t_now)
                        Left.append(int(data_2[0]))        
                        Right.append(int(data_2[1]))  
                        plt.plot(t,Left,'-r')
                        plt.plot(t,Right,'-b')
                        #plt.get_current_fig_manager.full_screen_toggle()
                        plt.pause(0.000001)
                    else:
                        print(data_2)
                else:
                    print(data_2)

except  KeyboardInterrupt:
    ser.close()   
    plt.close()
    print('goodbye')
    os.exit()
 

    