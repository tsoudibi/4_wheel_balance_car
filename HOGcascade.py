import cv2
import numpy as np
import imutils
import time


#"C:\\OPCV\\haarcascades\\hogcascade_pedestrians.xml"
#cv2.HOGDescriptor_getDefaultPeopleDetector()
HOGCascade = cv2.HOGDescriptor()
HOGCascade.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

cap = cv2.VideoCapture(0)
# 設定影像尺寸
width = 1280/3
height = 1280/3
# 設定擷取影像的尺寸大小
cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

#fps
time1=time.time()
time2=time.time()
fps=0

#face size
biggest_face_area=0
bigx=0
bigy=0
bigw=0
bigh=0
#設定跳過影格，每(skip)次執行一次
skip=2
frame_count=2;#確保第一次會執行

while frame_count:
    frame_count-=1
    if frame_count==1:
        # Read the frame
        _, img = cap.read()
        rame_count=skip; 
        # turn img to gray
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        clahe = cv2.createCLAHE(clipLimit=15.0,tileGridSize=(8,8))
        gray = clahe.apply(gray)
        # HOG cascade
        global rects
        (rects, weights) = HOGCascade.detectMultiScale(img, winStride=(4, 4),padding=(16, 16), scale=1.5)
            # HOGCascade.detectMultiScale(img, winStride=(4, 4),padding=(8, 8), scale=1.05)
            # scale default 1.1, bigger->faster
            # https://blog.csdn.net/qq_41603898/article/details/80494676
        # draw rectangle
        for (x, y, w, h) in rects:
            # caculate the biggest face
            if w*h>=biggest_face_area: 
                biggest_face_area=w*h
                bigx, bigy, bigw, bigh = x, y, w, h
            cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        biggest_face_area=0
        # print biggest face location
        cv2.putText(img,str(bigx+bigw/2)+','+str(bigy+bigh/2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
        # show image
        cv2.imshow('img', img) 
    else:
        _, img = cap.read()
        # reset counter
        if frame_count==0:
            time2=time.time()
            fps=skip/(time2-time1)
            #print(round(fps,2))  
            # print fps
            cv2.putText(img,str(round(fps,2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
            frame_count=skip
        if frame_count==skip:
            time1=time.time()
        # draw rectangle
        for (x, y, w, h) in rects:
            # caculate the biggest face
            if w*h>=biggest_face_area:
                biggest_face_area=w*h
                bigx, bigy, bigw, bigh = x, y, w, h
            cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        biggest_face_area=0
        #print biggest face location
        cv2.putText(img,str(bigx+bigw/2)+','+str(bigy+bigh/2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
        #time2=round(time.time()*1000)
        # show image
        cv2.imshow('img', img)  
    # Stop if escape key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    
# Release the VideoCapture object
cap.release()
cv2.destroyAllWindows()

