import cv2
import numpy as np
import imutils
import time
import threading
from math import floor
# 接收攝影機串流影像，採用多執行緒的方式，降低緩衝區堆疊圖幀的問題。
class ipcamCapture:
    def __init__(self, URL):
        self.Frame = []
        self.status = False
        self.isstop = False
		
	# 攝影機連接。
        self.capture = cv2.VideoCapture(URL)

    def start(self):
	# 把程式放進子執行緒，daemon=True 表示該執行緒會隨著主執行緒關閉而關閉。
        print('ipcam started!')
        threading.Thread(target=self.queryframe, daemon=True, args=()).start()

    def stop(self):
	# 記得要設計停止無限迴圈的開關。
        self.isstop = True
        print('ipcam stopped!')
   
    def getframe(self):
	# 當有需要影像時，再回傳最新的影像。
        return self.Frame
        
    def queryframe(self):
        while (not self.isstop):
            self.status, self.Frame = self.capture.read()
        
        self.capture.release()

# define HOGDescriptor
HOGCascade = cv2.HOGDescriptor()
HOGCascade.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
# 'http://admin:admin@10.218.221.163:8080/video'

ip_camera_url = 'http://admin:admin@10.218.221.163:8080/video'
cap = ipcamCapture(ip_camera_url)
cap.start()
time.sleep(1)
# set image size of camera, smaller will run faster
# camera Info:
# max size: 1280*720
# max fps: 30
width = floor(1280/2)
height = floor(1280/2)

# fps
time1=time.time()
time2=time.time()
fps=0

# body size
biggest_body_area=0
bigx=0
bigy=0
bigw=0
bigh=0

#every "skip" times run HOG once
skip=2
# to make sure first time will run HOG
frame_count=2

while frame_count:
    frame_count-=1
    if frame_count==1:
        # Read the frame
        img = cap.getframe()
        img=cv2.resize(img,(width,height))
        # turn img to gray
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        clahe = cv2.createCLAHE(clipLimit=15.0,tileGridSize=(8,8))
        gray = clahe.apply(gray)
        # HOG cascade
        global rects
        (rects, weights) = HOGCascade.detectMultiScale(img, winStride=(4, 4),padding=(2,2), scale=1.5,finalThreshold=3)
            # HOGCascade.detectMultiScale(img, winStride=(4, 4),padding=(8, 8), scale=1.05)
            # winStride default 4,4 , can only be 4n,4n ,the length of steps
            # padding default 8,8 ,the padding outside body when scaning
            # scale default 1.1, bigger->faster
            # finalThreshold default 1, Coefficient to regulate the similarity threshold. When detected, some objects can be covered by many rectangles. 0 means not to perform grouping.
            # https://blog.csdn.net/qq_41603898/article/details/80494676
            # https://answers.opencv.org/question/22069/hogdetectmultiscale-parameters/
        # draw rectangle
        for (x, y, w, h) in rects:
            # caculate the biggest body
            if w*h>=biggest_body_area: 
                biggest_body_area=w*h
                bigx, bigy, bigw, bigh = x, y, w, h
            cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        biggest_body_area=0
        # print biggest body rect and location
        cv2.rectangle(img, (bigx, bigy), (bigx+bigw, bigy+bigh), (255, 0, 0), 4)
        cv2.putText(img,str(bigx+bigw/2)+','+str(bigy+bigh/2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (0, 0, 255),2, cv2.LINE_AA)
        # print fps
        cv2.putText(img,str(round(fps,2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (0, 0, 255),2, cv2.LINE_AA)
        # show image
        cv2.imshow('img', img) 
    else:
        img = cap.getframe()
        img=cv2.resize(img,(width,height))
        if frame_count==0:
            # caculte fps
            time2=time.time()
            fps=skip/(time2-time1) 
            frame_count=skip
        if frame_count==skip:
            time1=time.time()
        # draw rectangle
        for (x, y, w, h) in rects:
            # caculate the biggest body
            if w*h>=biggest_body_area:
                biggest_body_area=w*h
                bigx, bigy, bigw, bigh = x, y, w, h
            cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        biggest_body_area=0
        # print biggest body rect and location
        cv2.rectangle(img, (bigx, bigy), (bigx+bigw, bigy+bigh), (255, 0,0 ), 4)
        cv2.putText(img,str(bigx+bigw/2)+','+str(bigy+bigh/2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (0, 0, 255),2, cv2.LINE_AA)
        # print fps
        cv2.putText(img,str(round(fps,2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (0, 0, 255),2, cv2.LINE_AA)
        # show image
        cv2.imshow('img', img)  
    # Stop if escape key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        cap.stop()
        break
    
# Release the VideoCapture object
cv2.destroyAllWindows()

