import cv2
import mediapipe as mp
import time
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

cap = cv2.VideoCapture(0)

# set image size of camera, smaller will run faster
# camera Info:
# max size: 1280*960
# max fps: 30
width = 1280 // 2
height = 960 //2
depth = 0
cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

# fps
time1 = time.time()
time2 = time.time()
fps = 0

with mp_pose.Pose(
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as pose:
    while cap.isOpened():
        time1 = time.time()
        success, image = cap.read()
        img_height, img_width, img_channel = image.shape
        if not success:
            print("Ignoring empty camera frame.")
            # If loading a video, use 'break' instead of 'continue'.
            continue

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
            cv2.rectangle(image, (x_min, y_min), (x_max, y_max), (0, 255, 0), 2) 
            scale = (y_max - y_min) / 160      
            # https://stackoverflow.com/questions/66876906/create-a-rectangle-around-all-the-points-returned-from-mediapipe-hand-landmark-d
        time2 = time.time()
        fps = 1 / (time2 - time1)
        # print fps 
        cv2.putText(image, str(round(fps, 2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
        # print depth
        cv2.putText(image, str(round((y_max - y_min), 2)), (width-150, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
        cv2.imshow('MediaPipe Pose', image)
        if cv2.waitKey(5) & 0xFF == 27:
            break
cap.release()
# https://google.github.io/mediapipe/solutions/pose#static_image_mode
