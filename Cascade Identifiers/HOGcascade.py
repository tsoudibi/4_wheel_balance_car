import cv2
import numpy as np
import imutils
import time

# define HOGDescriptor
HOGCascade = cv2.HOGDescriptor()
HOGCascade.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
ip_camera_url = 'http://admin:admin@10.218.221.163:8080/video'
cap = cv2.VideoCapture(0)
# set image size of camera, smaller will run faster
# camera Info:
# max size: 1280*720
# max fps: 30
width = 1280 // 2
height = 960 // 2
cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

# fps
time1 = time.time()
time2 = time.time()
fps = 0


# biggest body
class biggest_body:

    def __init__(self):
        self.biggest_body_area = 0
        self.bigX = 0
        self.bigY = 0
        self.bigW = 0
        self.bigH = 0


B = biggest_body()

# to make sure first time will run HOG
group_size = 3
frame_count = group_size

while True:
    frame_count -= 1
    # Read the frame
    _, img = cap.read()
    img = cv2.resize(img, (width, height))
    if frame_count == group_size - 1:
        time1 = time.time()
        # turn img to gray
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        clahe = cv2.createCLAHE(clipLimit=15.0, tileGridSize=(8, 8))
        gray = clahe.apply(gray)
        # HOG cascade
        global rects
        (rects, weights) = HOGCascade.detectMultiScale(img, winStride=(4, 4), padding=(16, 16), scale=1.5,
                                                       finalThreshold=2.0)
    # Stop if escape key is pressed
    elif cv2.waitKey(1) & 0xFF == ord('q'):
        break
    elif frame_count == 0:
        time2 = time.time()
        fps = (group_size - 1) / (time2 - time1)
        frame_count = group_size
        # draw rectangle
    for (x, y, w, h) in rects:
        # calculate the biggest body
        if w * h >= B.biggest_body_area:
            B.biggest_body_area = w * h
            B.bigX, B.bigY, B.bigW, B.bigH = x, y, w, h
        cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
    B.biggest_body_area = 0
    # print biggest body rect and location
    cv2.rectangle(img, (B.bigX, B.bigY),
                  (B.bigX + B.bigW, B.bigY + B.bigH), (255, 0, 0), 4)
    cv2.putText(img, str(B.bigX + B.bigW / 2) + ',' + str(B.bigY + B.bigH / 2)
                , (400, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
    # print fps
    cv2.putText(img, str(round(fps, 2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
    # show image
    cv2.imshow('img', img)

# Release the VideoCapture object
cap.release()
cv2.destroyAllWindows()
