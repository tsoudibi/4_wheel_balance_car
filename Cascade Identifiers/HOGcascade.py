import cv2
import numpy as np
import imutils
import time


#"C:\\OPCV\\haarcascades\\hogcascade_pedestrians.xml"
#cv2.HOGDescriptor_getDefaultPeopleDetector()
HOGCascade = cv2.HOGDescriptor()
HOGCascade.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

cap = cv2.VideoCapture(0)
# set image size of camera, smaller will run faster
width = 1280/3
height = 1280/3
cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

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
frame_count=2;

while frame_count:
    frame_count-=1
    if frame_count==1:
        # Read the frame
        _, img = cap.read()
        # turn img to gray
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        clahe = cv2.createCLAHE(clipLimit=15.0,tileGridSize=(8,8))
        gray = clahe.apply(gray)
        # HOG cascade
        global rects
        (rects, weights) = HOGCascade.detectMultiScale(img, winStride=(4, 4),padding=(16, 16), scale=1.5,finalThreshold=2.0)
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
        # print biggest body location
        cv2.putText(img,str(bigx+bigw/2)+','+str(bigy+bigh/2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
        # print fps
        cv2.putText(img,str(round(fps,2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
        # show image
        cv2.imshow('img', img) 
    else:
        _, img = cap.read()
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
        #print biggest body location
        cv2.putText(img,str(bigx+bigw/2)+','+str(bigy+bigh/2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
        # print fps
        cv2.putText(img,str(round(fps,2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX,1, (255, 0, 0),2, cv2.LINE_AA)
        # show image
        cv2.imshow('img', img)  
    # Stop if escape key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    
# Release the VideoCapture object
cap.release()
cv2.destroyAllWindows()

