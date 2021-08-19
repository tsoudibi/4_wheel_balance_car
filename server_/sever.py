import json
import random
import time
from PIL import Image
import io

import numpy as np
import plotly as py
import plotly.graph_objs as go
from flask import Flask, render_template, request, jsonify, send_file
import mediapipe_pose as mp_p
import threading
import thread

app = Flask(__name__)


# car status
class status:
    def __init__(self):
        self.new = 0

        self.movement = "NAN132"
        self.control_mode = "button"

        self.RPM_L = 0
        self.RPM_R = 0
        self.sensor_x = 0
        self.sensor_y = 0
        self.queue_sensor_x = []
        self.queue_sensor_y = []

        self.esp32_update_t1 = 0
        self.esp32_update_t2 = 0
        self.esp32_update_dt = 0

        self.control_L = 0
        self.control_R = 0

        self.cam_x = None
        self.cam_depth = None
        self.cam_HZ_L = None
        self.cam_HZ_R = None
        self.is_human = False

        self.SSID = 'NAN'


# esp32 data log queue
esp_log_queue = []
esp_log_string = ''
car_stat = status()

# camera mode btn flag
camera_btn_mode = 'None'


@app.route("/dinner_mode")
def dinner():
    # 狀態初始化
    data = {}
    return render_template("dinner.html", data=data)


@app.route('/', methods=['GET', 'POST'])
def visitor():

    # 狀態初始化
    data = {'RPM_L': 'Getting...',
            'RPM_R': 'Getting...',
            'status_L': 'Getting...',
            'status_R': 'Getting...',
            'control_L': 'Getting...',
            'control_R': 'Getting...',
            'SSID': 'Getting...',
            'stopBtn': 'START',
            'esp32_update_dt': '000'
            }

    return render_template('visitor.html', data=data)


@app.route("/button")
def button():
    car_stat.control_mode = "button"

    # 狀態初始化
    data = {'RPM_L': 'Getting...',
            'RPM_R': 'Getting...',
            'status': 'Getting...',
            'control_L': 'Getting...',
            'control_R': 'Getting...',
            'SSID': 'Getting...',
            'esp32_update_dt': '000'
            }

    return render_template("index.html", data=data)


@app.route('/sensor', methods=['GET', 'POST'])
def sensor():
    car_stat.control_mode = "sensor"

    # 狀態初始化
    data = {'RPM_L': 'Getting...',
            'RPM_R': 'Getting...',
            'status_L': 'Getting...',
            'status_R': 'Getting...',
            'control_L': 'Getting...',
            'control_R': 'Getting...',
            'SSID': 'Getting...',
            'stopBtn': 'STOP',
            'esp32_update_dt': '000'
            }

    return render_template('sensor_plot.html', data=data)


@app.route('/camera', methods=['GET', 'POST'])
def camera():
    car_stat.control_mode = "camera"

    # 狀態初始化
    data = {'RPM_L': 'Getting...',
            'RPM_R': 'Getting...',
            'status_L': 'Getting...',
            'status_R': 'Getting...',
            'control_L': 'Getting...',
            'control_R': 'Getting...',
            'SSID': 'Getting...',
            'stopBtn': 'START',
            'esp32_update_dt': '000'
            }

    return render_template('camera.html', data=data)


def esp_log_btn(method, argu="None"):
    global esp_log_string, esp_log_queue
    # record time
    now_time = time.localtime(time.time())
    esp_log_queue.append('[' + str(now_time.tm_hour) + ":" + str(now_time.tm_min) + ":" + str(now_time.tm_sec) + ']' +
                        ' esp ' + method + argu)
    # set the lenght of queue to 5
    if len(esp_log_queue) > 5:
        esp_log_queue.pop(0)
    # transforn esp_log from queue to string
    esp_log_string = ''
    for logs in esp_log_queue:
        esp_log_string = esp_log_string + logs + '\n'


@app.route("/esp32", methods=['GET'])
def esp32():
    # return wanted data to esp32
    if request.method == "GET":
        mode = request.args.get('mode')
        if mode == 'update':
            # update time
            if car_stat.esp32_update_t2 == 0:
                car_stat.esp32_update_t1 = time.time_ns()
            else:
                car_stat.esp32_update_t2 = time.time_ns()  # ns
                car_stat.esp32_update_dt = (car_stat.esp32_update_t2 - car_stat.esp32_update_t1) * 1000  # ms
            car_stat.control_L = float(request.args.get('controlL'))
            car_stat.control_R = float(request.args.get('controlR'))
            car_stat.RPM_L = float(request.args.get('speedL'))
            car_stat.RPM_R = float(request.args.get('speedR'))
            car_stat.sensor_x = float(request.args.get('sensor_x'))
            car_stat.sensor_y = float(request.args.get('sensor_y'))
            # save sensor coordinate into queue 
            newQueue(car_stat.queue_sensor_x, car_stat.queue_sensor_y, int(car_stat.sensor_x), int(car_stat.sensor_y))
            # loging 
            print('esp32 update finished! ' )
            return 'finished'
        elif mode == 'gather':
            which = request.args.get('which')
            if which == 'movement':
                if car_stat.new == 1:
                    car_stat.new = 0
                    esp_log_btn("get ", car_stat.movement)
                    print('esp32 GET movement: ' + str(car_stat.movement))
                    return car_stat.movement
                return "None"
            elif which == 'control_mode':
                print('esp32 GET control_mode: ' + str(car_stat.control_mode))
                return str(car_stat.control_mode)
            elif which == 'cam_HZ':
                print('esp32 GET cam_HZ: ' + str(car_stat.cam_HZ_L) + ", " + str(car_stat.cam_HZ_R))
                return str(car_stat.cam_HZ_L) + "," + str(car_stat.cam_HZ_R)
        elif mode == 'SSID':
        # upate SSID on server
            SSID = request.args.get('SSID')
            if SSID is not None:
                car_stat.SSID = SSID
                print('esp32 GET SSID: ' + car_stat.SSID)
                return car_stat.SSID
        else:
            return "wrong mode in get request!"


@app.route("/button_mode_button_click", methods=['GET', 'POST'])
def direction_instructions():
    if request.method == "GET":
        btn = request.args.get('a')
        if btn == 'forward':
            print('btn_mode :forward')
            car_stat.new = 1
            car_stat.movement = "forward"
        elif btn == 'left':
            print('btn_mode :left')
            car_stat.new = 1
            car_stat.movement = "left"
        elif btn == 'right':
            print('btn_mode :right')
            car_stat.new = 1
            car_stat.movement = "right"
        elif btn == 'stop':
            print('btn_mode :stop')
            car_stat.new = 1
            car_stat.movement = "stop"
        elif btn == 'backward':
            print('btn_mode :backward')
            car_stat.new = 1
            car_stat.movement = "backward"
        else:
            print('btn_mode :btn error')

    return "nothing"


@app.route('/sensor_mode_button_click')
def plot_trigger():
    btn = request.args.get('btn')
    print('sensor_btn: ' + btn)
    if btn == 'STOP':
        data = {'stopBtn': 'CONTINUE'}
        return data
    else:
        data = {'stopBtn': 'STOP'}
        return data


# esp_log in camera mode
def esp_log_cam(msg):
    global esp_log_string, esp_log_queue
    # record time
    now_time = time.localtime(time.time())
    esp_log_queue.append(
        '[' + str(now_time.tm_hour) + ":" + str(now_time.tm_min) + ":" + str(now_time.tm_sec) + ']' + msg)
    # set the lenght of queue to 5
    if len(esp_log_queue) > 5:
        esp_log_queue.pop(0)
    # transforn esp_log from queue to string
    esp_log_string = ''
    for logs in esp_log_queue:
        esp_log_string = esp_log_string + logs + '\n'


@app.route('/camara_mode_button_click')
def camera_plot():
    global camera_btn_mode
    camera_btn_mode = request.args.get('btn')  # get button name
    print(camera_btn_mode)
    if camera_btn_mode == 'STOP':  # when "STOP" clicked
        # stop the thread
        global t
        t.kill()
        t.join()
        if not t.isAlive():
            print('thread killed')
        # record esp_log
        esp_log_cam('camera stop')
        data = {'stopBtn': 'CONTINUE'}
        return data
    elif camera_btn_mode == 'START':  # when "START"  clicked
        # start the camera along with thread
        mp_p.camera_start(device='ipcam')
        t = thread.thread_with_trace(target=mp_p.mediapipe_pose)
        t.start()
        # record esp_log
        esp_log_cam('camera start, ip:' + mp_p.ip_address)
        data = {'stopBtn': 'STOP'}
        return data
    else:  # when "CONTINUE" clicked
        # start the thread
        t = thread.thread_with_trace(target=mp_p.mediapipe_pose)
        t.start()
        # record esp_log
        esp_log_cam('camera continue')
        data = {'stopBtn': 'STOP'}
        return data


@app.route('/HZ_RESET_button_click')
def HZ_reset():
    result = {'data': ''}
    if mp_p.HZ_L == 0 and mp_p.HZ_R == 0:
        result['data'] = "HZ doesn't need to reset !"
    else:
        mp_p.HZ_L = 0
        mp_p.HZ_R = 0
        result['data'] = 'Resetting completed !'
    return result


@app.route('/RPM_newPlot', methods=['GET', 'POST'])
def RPM_newPlot():
    scatter = create_RPM_plot_real()
    return scatter


@app.route('/newPlot', methods=['GET', 'POST'])
def newPlot():
    scatter = create_plot_real()
    return scatter


@app.route('/CAM_newIMG', methods=['GET'])
def CAM_newIMG():
    global camera_btn_mode, esp_log_queue
    if mp_p.image2server is None:
        return None
    else:
        # get position fome mediapipe.py and save to object 
        car_stat.cam_depth = mp_p.average_depth
        car_stat.cam_x = mp_p.average_x
        car_stat.cam_HZ_L = mp_p.HZ_L
        car_stat.cam_HZ_R = mp_p.HZ_R
        car_stat.is_human = mp_p.IS_HUMAN

        # convert numpy array to PIL Image 
        img = Image.fromarray(mp_p.image2server.astype('uint8'))

        # create file-object in memory
        file_object = io.BytesIO()

        # write jpeg in file-object
        img.save(file_object, 'jpeg')

        # move to beginning of file so `send_file()` it will read from start
        file_object.seek(0)

        # record esp_log, set interval of 1s
        now_time = time.localtime(time.time())
        time_in_log = esp_log_queue[len(esp_log_queue) - 1].split(']')
        sec = time_in_log[0].split(':')
        if camera_btn_mode != 'STOP' and sec[2] != str(now_time.tm_sec):
            esp_log_cam('depth: ' + str(round(car_stat.cam_depth, 2)) + ', x: ' + str(round(car_stat.cam_x, 2)))

        return send_file(file_object, mimetype='image/jpeg')


@app.route('/newStatus', methods=['GET', 'POST'])
def newStatus():
    data = {'RPM_L': car_stat.RPM_L,
            'RPM_R': car_stat.RPM_R,
            'control_L': car_stat.control_L,
            'control_R': car_stat.control_R,
            'SSID': car_stat.SSID,
            'status': esp_log_string,
            'esp32_update_dt': car_stat.esp32_update_dt,
            'direction': 'x: ' + str(car_stat.queue_sensor_x) + ' , y: ' + str(car_stat.queue_sensor_y)
            }

    return data


def create_RPM_plot_real():
    rpm_x = [-128, 128]
    rpm_y = [car_stat.RPM_L, car_stat.RPM_R]

    con_x = [-70, 70]
    con_y = [car_stat.control_L * 0.5, car_stat.control_R * 0.5]

    # Create a trace
    data = [go.Scatter(
        name='RPM',
        x=rpm_x,
        y=rpm_y,
        mode='lines+markers',  # lines+dots
        line_width=10,
        marker=dict(size=30, color='rgba(255, 109, 0, 1)')
    ), go.Scatter(
        name='Control',
        x=con_x,
        y=con_y,
        mode='lines+markers',  # lines+dots
        line_width=10,
        marker=dict(size=25, color='rgba(61, 90, 128, 1)')
    )
    ]

    graphJSON = json.dumps(data, cls=py.utils.PlotlyJSONEncoder)

    return graphJSON


def create_plot_real():
    R_low = 300  # turning right lower bound
    L_low = -300  # turning left lower bound
    F_low = 300  # going forward lower bound
    B_low = -200  # going backward lower bound
    # Create a trace
    data = [go.Scatter(
        x=[-1000, -1000, 1000, 1000, -1000],
        y=[-800, 800, 800, -800, -800],
        mode='markers',
        marker=dict(color='rgba(142, 202, 230, 0.5)'),
        fill='toself')
        , go.Scatter(
            x=[R_low, R_low, 1000, 1000, R_low],
            y=[-800, 800, 800, -800, -800],
            mode='markers',
            marker=dict(color='rgba(142, 202, 230, 0.5)'),
            fill='toself')
        , go.Scatter(
            x=[L_low, L_low, R_low, R_low, L_low],
            y=[F_low, 800, 800, F_low, F_low],
            mode='markers',
            marker=dict(color='rgba(33, 158, 168, 0.1)'),
            fill='toself')
        , go.Scatter(
            x=[L_low, L_low, R_low, R_low, L_low],
            y=[B_low, -800, -800, B_low, B_low],
            mode='markers',
            marker=dict(color='rgba(33, 158, 168, 0.1)'),
            fill='toself')
        , go.Scatter(
            x=[L_low, L_low, R_low, R_low, L_low],
            y=[B_low, F_low, F_low, B_low, B_low],
            mode='markers',
            marker=dict(color='rgba(2, 48, 71, 0.1)'),
            fill='toself')
        , go.Scatter(
            x=car_stat.queue_sensor_x,
            y=car_stat.queue_sensor_y,
            mode='lines+markers',  # lines+dots
            marker=dict(size=[20, 30, 40, 50, 60], color='rgba(255, 109, 0, 1)')
        )]

    graphJSON = json.dumps(data, cls=py.utils.PlotlyJSONEncoder)

    return graphJSON


def newQueue(queue_x, queue_y, new_x, new_y):
    if len(queue_x) == 5:
        del queue_x[0]  # pop x
        del queue_y[0]  # pop y

    queue_x.append(new_x)  # push new x to queue
    queue_y.append(new_y)  # push new y to queue


if __name__ == "__main__":
    # run production server on flask
    # reference: https://stackoverflow.com/questions/38982807/are-a-wsgi-server-and-http-server-required-to-serve-a-flask-app/38982989#38982989
    from waitress import serve
    serve(app, host='0.0.0.0', port=5000)
    #app.run(host='0.0.0.0', debug=True)
