import cv2
import time
import threading
from PIL import Image

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
ip_camera_url = 'http://admin:admin@10.1.1.3:8080/video'

# set image size of camera, smaller will run faster
# camera Info:
# max size: 1280*720
# max fps: 30
width = 1280 // 2
height = 960 // 2

# fps
time1 = time.time()
time2 = time.time()
fps = 0
queue = {}

cap = None
mode = None
def camera_start( device = 'webcam'):
    global cap, width, height, mode 
    mode = device
    if mode == 'webcam':
        print("webcam start")
        cap = cv2.VideoCapture(0,cv2.CAP_DSHOW)
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


def HOG():
    global queue, cap, mode
    print("HOG cascade start, mode:" + mode)
    while True:
        global group_size, frame_count, time1, time2, fps, width, height
        frame_count -= 1
        # Read the frame
        if mode == 'webcam':
            _, img = cap.read()
        elif mode == 'ipcam':
            img = cap.getframe()
        else :
            print("mode error, please check parameter 'mode'")
            break
        img = cv2.resize(img, (width, height))
        print("Wrwerwer")
        if frame_count == group_size - 1:
            time1 = time.time()
            # turn img to gray
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            clahe = cv2.createCLAHE(clipLimit=15.0, tileGridSize=(8, 8))
            gray = clahe.apply(gray)
            # HOG cascade
            global rects
            (rects, weights) = HOGCascade.detectMultiScale(gray, winStride=(4, 4), padding=(16, 16), scale=1.5,
                                                           finalThreshold=2.0)
        # Stop if escape key is pressed
        elif cv2.waitKey(1) & 0xFF == ord('q'):
            if mode == 'ipcam':
                cap.stop()
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
        im_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        if queue is None:
            queue = im_rgb
        else:
            queue[0] = im_rgb

        #im = Image.fromarray(im_rgb)
        #print(type(im))
        #im.save("static/img/cam.jpeg")


    # Release the VideoCapture object
    if mode == 'webcam':
        cap.release()
    elif mode == 'ipcam':
        cap.stop()
    cv2.destroyAllWindows()
