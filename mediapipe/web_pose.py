import cv2
import mediapipe as mp
import threading
import time

# using multiple thread to avoid delay or overflow
class ipcamCapture:
    def __init__(self, URL):
        self.Frame = []
        self.status = False
        self.isstop = False
        self.capture = cv2.VideoCapture(URL)
        print("ipcamera: connect success!")

    # subthread start
    def start(self):
        threading.Thread(target=self.queryframe, daemon=True, args=()).start()

    def stop(self):
        self.isstop = True
        print("ipcamera: stop ")
    
    # return newesst frame
    def getframe(self):
        return self.Frame
        
    # get image from ipcamera
    def queryframe(self):
        while (not self.isstop):
            self.status, self.Frame = self.capture.read()
        
        self.capture.release()

mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

ip_camera_url = 'http://admin:admin@192.168.137.49:8080/video'
#cap = cv2.VideoCapture(0)
cap = ipcamCapture(ip_camera_url)
cap.start()
time.sleep(1)

# set image size of camera, smaller will run faster
# camera Info:
# max size: 1280*960
# max fps: 30
width = 960 // 1
height = 960 // 1
depth = 0
x_center = 0
y_center = 0
body_height = 0


# fps
time1 = time.time()
time2 = time.time()
fps = 0

with mp_pose.Pose(
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as pose:
    while True:
        time1 = time.time()
        image = cap.getframe()
        img_height, img_width, img_channel = image.shape
        

        # Flip the image horizontally for a later selfie-view display, and convert
        # the BGR image to RGB.
        image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
        # To improve performance, optionally mark the image as not writeable to
        # pass by reference.
        image.flags.writeable = False
        results = pose.process(image)

        # Draw the pose annotation on the image.
        image.flags.writeable = True
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        mp_drawing.draw_landmarks( 
            image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
        # draw rectangle
        x_max = 0
        y_max = 0
        x_min = img_width
        y_min = img_height
        if results.pose_landmarks:
            for idx, landmark in enumerate(results.pose_landmarks.landmark):
                x, y = int(landmark.x * img_width), int(landmark.y * img_height)
                if x > x_max:
                    x_max = x
                if x < x_min:
                    x_min = x
                if y > y_max:
                    y_max = y
                if y < y_min:
                    y_min = y
                if idx == 12 :
                    depth = abs(landmark.z * width)
                    print(landmark.z)
                if idx == 24 :
                    x_right_hip = landmark.x
                    y_right_hip = landmark.y
                if idx == 23 :
                    x_left_hip = landmark.x
                    y_left_hip = landmark.y
            x_center = (x_right_hip + x_left_hip) / 2 - 0.5
            y_center = (y_right_hip + y_left_hip) / 2 - 0.5
            body_height = y_max - y_min
            cv2.rectangle(image, (x_min, y_min), (x_max, y_max), (0, 255, 0), 2)  
            cv2.circle(image, (round((x_center + 0.5) * width), round((y_center + 0.5) * height)), body_height // 30, (255, 255, 255), -1)   
            # https://stackoverflow.com/questions/66876906/create-a-rectangle-around-all-the-points-returned-from-mediapipe-hand-landmark-d
        time2 = time.time()
        fps = 1 / (time2 - time1)
        # print fps 
        cv2.putText(image, str(round(fps, 2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
        # print depth
        cv2.putText(image, str(round(body_height, 2)), (width-150, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
        cv2.putText(image, str(round(x_center, 2)), (width-150, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
        cv2.imshow('MediaPipe Pose', image)
        if cv2.waitKey(5) & 0xFF == 27:
            break
cap.stop()
# https://google.github.io/mediapipe/solutions/pose#static_image_mode
