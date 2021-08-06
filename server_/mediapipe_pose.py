import cv2
import mediapipe as mp
import threading
import time
import numpy as np


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
ip_address = '10.1.1.3:8080'
ip_camera_url = 'http://admin:admin@' + ip_address + '/video'

# set image size of camera, smaller will run faster
# camera Info:
# max size: 1280*720
# max fps: 30
width = 1280 // 1
height = 720 // 1

depth = 0
x_center = 0
y_center = 0
depth_normalized = 0

# fps
time1 = time.time()
time2 = time.time()
fps = 0

# data to pass to server
image2server = None
position_queue = []
average_depth = 0
average_x = 0

cap = None
mode = None


def camera_start(device='webcam'):
    global cap, width, height, mode
    mode = device
    if mode == 'webcam':
        print("webcam start")
        cap = cv2.VideoCapture(0)
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    elif mode == 'ipcam':
        print("ipcam start")
        cap = ipcamCapture(ip_camera_url)
        cap.start()
        time.sleep(1)
        print("cam init finish")
    else:
        print("mode error, please check parameter 'mode'")


def mediapipe_pose(debug_mode = False):
    global queue, cap, mode, time1, time2, fps, depth, x_center, y_center
    global depth_normalized, position_queue, average_depth, average_x, image2server
    with mp_pose.Pose(
            min_detection_confidence=0.5,
            min_tracking_confidence=0.5) as pose:
        while True:
            # start timer
            time1 = time.time()
            # Read the frame
            if mode == 'webcam':
                _, image = cap.read()
            elif mode == 'ipcam':
                image = cap.getframe()
            else:
                print("mode error, please check parameter 'mode'")
                break
            img_height, img_width, img_channel = image.shape
            # Flip the image horizontally for a later selfie-view display, and convert
            # the BGR image to RGB.
            # image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
            image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
            # To improve performance, optionally mark the image as not writeable to
            # pass by reference.
            image.flags.writeable = False
            results = pose.process(image)

            # Draw the pose annotation on the image.
            image.flags.writeable = True
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            # mp_drawing.draw_landmarks( image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
            draw_pose(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
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
                    if idx == 12:
                        depth = abs(landmark.z * img_width)
                    if idx == 24:
                        x_right_hip = landmark.x
                        y_right_hip = landmark.y
                    if idx == 23:
                        x_left_hip = landmark.x
                        y_left_hip = landmark.y
                x_center = (x_right_hip + x_left_hip) / 2 - 0.5
                y_center = (y_right_hip + y_left_hip) / 2 - 0.5
                depth_normalized = (img_height / (y_max - y_min))
                cv2.rectangle(image, (x_min, y_min), (x_max, y_max), (0, 255, 0), 2)
                # draw cneter point of body
                cv2.circle(image, (round((x_center + 0.5) * img_width), round((y_center + 0.5) * img_height)),
                           (y_max - y_min) // 30, (224, 220, 164), -1)
                # https://stackoverflow.com/questions/66876906/create-a-rectangle-around-all-the-points-returned-from-mediapipe-hand-landmark-d
            time2 = time.time()
            fps = 1 / (time2 - time1)
            # print fps (10, 40) , body height (normalized) (img_width - 150, 40), x_center (img_width - 150, 80)
            cv2.rectangle(image, (5, 10), (90, 50), (0, 0, 0), -1)
            cv2.putText(image, str(round(fps, 2)), (10, 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2, cv2.LINE_AA)
            cv2.rectangle(image, (img_width - 155, 10), (img_width - 55, 90), (0, 0, 0), -1)
            cv2.putText(image, str(round(depth_normalized, 2)), (img_width - 150, 40), cv2.FONT_HERSHEY_SIMPLEX, 1,
                        (255, 255, 255), 2, cv2.LINE_AA)
            cv2.putText(image, str(round(x_center, 2)), (img_width - 150, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255),
                        2, cv2.LINE_AA)

            # save position in queue (size = 10)
            position_queue.append([depth_normalized, x_center])
            if len(position_queue) > 10:
                position_queue.pop(0)

            # caculate average value in the queue
            average_depth = 0
            average_x = 0
            for positions in position_queue:
                average_depth = average_depth + positions[0]
                average_x = average_x + positions[1]
            average_depth = average_depth / 10
            average_x = average_x / 10

            # caculate HZ for esp32 motor control
            caculate_HZ()

            # draw top view
            image = draw_top_view(image)

            if debug_mode: cv2.imshow('MediaPipe Pose', image)
            # convert back to RGB
            im_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            # save image in image2server
            image2server = im_rgb

            # except keyboard interupt
            if cv2.waitKey(5) & 0xFF == 27:
                break
    # Release the VideoCapture object
    if mode == 'webcam':
        cap.release()
    elif mode == 'ipcam':
        cap.stop()
    cv2.destroyAllWindows()
    # https://google.github.io/mediapipe/solutions/pose#static_image_mode


HZ_L = 0
HZ_R = 0
X_THRESHOLD = 0.1
DEPTH_FIX = 0.8
DEPTH_THRESHOLD = 0.2
DEPTH_BREAKPOINT = 0.1


def caculate_HZ():
    global average_depth, average_x, HZ_L, HZ_R
    # x range from -0.5 to 0.5
    if average_x > X_THRESHOLD:
        # body in right
        HZ_L = HZ_L + (average_x - X_THRESHOLD) * 0.05
    elif average_x < X_THRESHOLD * -1:
        # body in left
        HZ_R = HZ_R + (-1 * average_x - X_THRESHOLD) * 0.05
    # depth range from 0~1.6 (soft range)
    if average_depth > DEPTH_FIX + DEPTH_THRESHOLD:
        # too far, speed up
        HZ_L = HZ_L + (average_depth - DEPTH_FIX + DEPTH_THRESHOLD) * 0.05
        HZ_R = HZ_R + (average_depth - DEPTH_FIX + DEPTH_THRESHOLD) * 0.05
    elif average_depth < DEPTH_FIX - DEPTH_THRESHOLD:
        # too close, slow down
        HZ_L = HZ_L + (average_depth - DEPTH_FIX + DEPTH_THRESHOLD) * 0.05
        HZ_R = HZ_R + (average_depth - DEPTH_FIX + DEPTH_THRESHOLD) * 0.05
    # set break if too close 
    if average_depth <= DEPTH_BREAKPOINT :
        HZ_L = 0
        HZ_R = 0


def draw_top_view(image):
    global average_depth, average_x
    img_height, img_width, img_channel = image.shape
    # define base 
    base_x, base_y, base_w, base_h = round(img_width * 0.7), round(img_height * 0.55), 250 , 250
    overlay = image.copy()
    # draw base
    cv2.rectangle(overlay, (base_x, base_y), (base_x+base_w, base_y+base_h), (255, 255, 255), -1)
    # draw threshold line
    center = (base_x + round(base_w/2), base_y + round(base_h/2))
    threshold_height = (DEPTH_THRESHOLD * 2) / (DEPTH_FIX * 2) * base_h
    threshold_width = X_THRESHOLD * 2 * base_w
    point1 = (center[0] - round(threshold_width/2), center[1] - round(threshold_height/2))
    point2 = (center[0] + round(threshold_width/2), center[1] + round(threshold_height/2))
    cv2.rectangle(overlay, point1, point2, (141, 152, 235), -1)
    cv2.rectangle(overlay, point1, point2, (141, 98, 235),  2)
    # draw break line
    cv2.line(overlay, (base_x , base_y + base_h - round((DEPTH_BREAKPOINT) / (DEPTH_FIX * 2) * base_h)), (base_x+base_w, base_y + base_h - round((DEPTH_BREAKPOINT) / (DEPTH_FIX * 2) * base_h)), (141, 98, 235), 2)
    # draw axises
    cv2.line(overlay, (base_x , base_y + round(base_h/2)), (base_x+base_w, base_y + round(base_h/2)), (100, 0, 0), 2)
    cv2.line(overlay, (base_x + round(base_w/2), base_y ), (base_x + round(base_w/2), base_y + base_h), (100, 0, 0), 2)
    # draw camera 
    cv2.circle(overlay, (base_x + round(base_w/2), base_y + base_h), 10, (255, 126, 79), -1)
    cv2.circle(overlay, (base_x + round(base_w/2), base_y + base_h), 10, (255, 49, 79), 2)
    # draw body 
    depth_mapped = average_depth / (DEPTH_FIX * 2) * base_h
    x_mapped = (average_x + 0.5) * base_w
    cv2.circle(overlay, (base_x + round(x_mapped), base_y + base_h - round(depth_mapped)), 8, (93, 73, 237), -1)
    cv2.circle(overlay, (base_x + round(x_mapped), base_y + base_h - round(depth_mapped)), 8, (93, 0, 237), 2)

    alpha = 0.6
    image = cv2.addWeighted(overlay, alpha, image, 1 - alpha, 0)

    global HZ_L, HZ_R
    cv2.putText(image, str(round(HZ_L,1)), (base_x + 18 , base_y + 30), cv2.FONT_HERSHEY_DUPLEX, 0.7, (0, 0, 0), 1, cv2.LINE_AA)
    cv2.putText(image, str(round(HZ_R,1)), (base_x + 18 + round(base_w/2) , base_y + 30), cv2.FONT_HERSHEY_DUPLEX, 0.7, (0, 0, 0), 1, cv2.LINE_AA)
    
    return image


PRESENCE_THRESHOLD = 0.5
RGB_CHANNELS = 3
VISIBILITY_THRESHOLD = 0.5


def draw_pose(image, landmark_list, connections):
    if not landmark_list:
        return
    if image.shape[2] != RGB_CHANNELS:
        raise ValueError('Input image must contain three channel rgb data.')
    image_rows, image_cols, _ = image.shape
    idx_to_coordinates = {}
    for idx, landmark in enumerate(landmark_list.landmark):
        # filtering not seen landmarks
        if ((landmark.HasField('visibility') and
            landmark.visibility < VISIBILITY_THRESHOLD) or
                (landmark.HasField('presence') and
                landmark.presence < PRESENCE_THRESHOLD)):
            continue
        landmark_px = round(landmark.x * image_cols), round(landmark.y * image_rows)
        if landmark_px:
            # saving coordinates as index
            idx_to_coordinates[idx] = landmark_px

    if connections:
        num_landmarks = len(landmark_list.landmark)
        # Draws the connections if the start and end landmarks are both visible.
        for connection in connections:
            start_idx = connection[0]
            end_idx = connection[1]
            if start_idx % 2 == 1 and end_idx % 2 == 1:
                color = (135, 255, 245)
            elif start_idx % 2 == 0 and end_idx % 2 == 0 and start_idx != 0:
                color = (255, 196, 148)
            else:
                color = (255, 255, 255)
            if not (0 <= start_idx < num_landmarks and 0 <= end_idx < num_landmarks):
                raise ValueError(
                    f'Landmark index is out of range. Invalid connection 'f'from landmark #{start_idx} to landmark #{end_idx}.')
            if start_idx in idx_to_coordinates and end_idx in idx_to_coordinates:
                cv2.line(image, idx_to_coordinates[start_idx], idx_to_coordinates[end_idx], color, 3)
        # Draws landmark points after finishing the connection lines, which is
        # aesthetically better.
        for landmark_px in idx_to_coordinates.values():
            cv2.circle(image, landmark_px, 5, (93, 73, 237), -1)

#camera_start(device='webcam')
#mediapipe_pose(True)