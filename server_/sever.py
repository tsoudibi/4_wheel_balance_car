import json
import random
import time
from PIL import Image
import io

import numpy as np
import plotly as py
import plotly.graph_objs as go
from flask import Flask, render_template, request, jsonify, send_file
import mediapipe_pose as mp
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

        self.control_L = 0
        self.control_R = 0

        self.cam_x = 0
        self.cam_depth = 0
        self.cam_HZ_x = 0
        self.cam_HZ_y = 0

        self.SSID = 'NAN'

# esp32 data log queue
esp_log_queue = []
esp_log_string = None

car_stat = status()


@app.route("/")
def controller():
    car_stat.control_mode = "button"

    # 狀態初始化
    data = {'RPM_L': 'Getting...',
            'RPM_R': 'Getting...',
            'status_L': 'Getting...',
            'status_R': 'Getting...',
            'control_L': 'Getting...',
            'control_R': 'Getting...',
            'SSID': 'Getting...'
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
            'stopBtn': 'STOP'
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
            'stopBtn': 'START'
            }

    return render_template('camera.html', data=data)

def esp_log(method , argu = "None"):
    global esp_log_string, esp_log_queue
    esp_log_queue.append("[esp] " + method + argu)
    if len(esp_log_queue) > 5:
        esp_log_queue.pop()
    for logs in esp_log_queue:
        esp_log_string = esp_log_string + logs + '\n'


@app.route("/esp32", methods=['GET', 'POST'])
def esp32():
    # return wanted data to esp32
    if request.method == "GET":
        which = request.args.get('which')
        if which == 'movement':
            if car_stat.new ==1 :
                car_stat.new = 0
                esp_log("get " , car_stat.movement)
            return car_stat.movement
        elif which == 'control_mode':
            return str(car_stat.control_mode)
        elif which == 'RPM':
            return str(car_stat.RPM_L) + "," + str(car_stat.RPM_R)
        elif which == 'sensor':
            return str(car_stat.sensor_x) + "," + str(car_stat.sensor_y)
        elif which == 'new':
            return str(car_stat.new)
        elif which == 'cam':
            return str(car_stat.cam_x) + "," + str(car_stat.cam_y) + "," + str(car_stat.cam_width) + "," + str(
                car_stat.cam_height) + "," + str(car_stat.cam_depth) + "," + str(car_stat.cam_Analog_x) + "," + str(
                car_stat.cam_Analog_y)
        SSID = request.args.get('SSID')
        if SSID is not None:
            car_stat.SSID = SSID
            return car_stat.SSID

    # get data from esp32
    if request.method == "POST":
        data = request.get_json()
        print(data)
        car_stat.control_L = data["control"][0]
        car_stat.control_R = data["control"][1]
        car_stat.RPM_L = data["RPM"][0]
        car_stat.RPM_R = data["RPM"][1]
        car_stat.sensor_x = data["sensor"][0]
        car_stat.sensor_y = data["sensor"][1]
        newQueue(car_stat.queue_sensor_x, car_stat.queue_sensor_y, int(car_stat.sensor_x), int(car_stat.sensor_y))
        return jsonify({"state": "ok"})



@app.route("/mode1_button_click", methods=['GET', 'POST'])
def direction_instructions():
    if request.method == "GET":
        btn = request.args.get('a')
        if btn == 'forward':
            print('forward')
            car_stat.new = 1
            car_stat.movement = "forward"
        elif btn == 'left':
            print('left')
            car_stat.new = 1
            car_stat.movement = "left"
        elif btn == 'right':
            print('right')
            car_stat.new = 1
            car_stat.movement = "right"
        elif btn == 'stop':
            print('stop')
            car_stat.new = 1
            car_stat.movement = "stop"
        elif btn == 'backward':
            print('backward')
            car_stat.new = 1
            car_stat.movement = "backward"
        else:
            print('btn error')

    return "nothing"


@app.route('/mode2_button_click')
def plot_trigger():
    btn = request.args.get('btn')
    print(btn)
    if btn == 'STOP':
        data = {'stopBtn': 'CONTINUE'}
        return data
    else:
        data = {'stopBtn': 'STOP'}
        return data


@app.route('/mode3_button_click')
def camera_plot():
    btn = request.args.get('btn')  # get button name
    print(btn)
    if btn == 'STOP':  # when "STOP" clicked
        global t
        t.kill()
        data = {'stopBtn': 'CONTINUE'}
        return data
    elif btn == 'START':  # when "START"  clicked
        mp.camera_start(device='webcam')
        t = thread.thread_with_trace(target=mp.mediapipe_pose)
        t.start()
        data = {'stopBtn': 'STOP'}
        return data
    else:  # when "CONTINUE" clicked
        t = thread.thread_with_trace(target=mp.mediapipe_pose)
        t.start()
        data = {'stopBtn': 'STOP'}
        return data


@app.route('/RPM_newPlot', methods=['GET', 'POST'])
def RPM_newPlot():
    scatter = create_RPM_plot_real()
    return scatter


@app.route('/newPlot', methods=['GET', 'POST'])
def newPlot():
    scatter = create_plot_real()
    # print(scatter)
    return scatter


@app.route('/CAM_newIMG', methods=['GET'])
def CAM_newIMG():
    if mp.queue is None:
        return None
    else:
        # save position 
        car_stat.cam_depth = mp.average_position[0]
        car_stat.cam_x = mp.average_position[1]

        # convert numpy array to PIL Image
        img = Image.fromarray(mp.queue[0].astype('uint8'))

        # create file-object in memory
        file_object = io.BytesIO()

        # write PNG in file-object
        img.save(file_object, 'jpeg')

        # move to beginning of file so `send_file()` it will read from start
        file_object.seek(0)

        return send_file(file_object, mimetype='image/jpeg')


@app.route('/newStatus', methods=['GET', 'POST'])
def newStatus():
    data = {'RPM_L': car_stat.RPM_L,
            'RPM_R': car_stat.RPM_R,
            'status_L': car_stat.cam_depth,
            'status_R': str(car_stat.cam_x),
            'control_L': car_stat.control_L,
            'control_R': car_stat.control_R,
            'SSID': car_stat.SSID
            }

    return data


def create_RPM_plot_real():
    rpm_x = [-128, 128]
    rpm_y = [car_stat.RPM_L, car_stat.RPM_R]

    con_x = [-128, 128]
    con_y = [car_stat.control_L, car_stat.control_R]

    # Create a trace
    data = [go.Scatter(
        name='RPM',
        x=rpm_x,
        y=rpm_y,
        mode='lines+markers',  # lines+dots
        marker=dict(size=30, color='rgba(255, 109, 0, 1)')
    ), go.Scatter(
        name='Control',
        x=con_x,
        y=con_y,
        mode='lines+markers',  # lines+dots
        line_width=10,
        marker=dict(size=50, color='rgba(61, 90, 128, 0.8)')
    )]

    graphJSON = json.dumps(data, cls=py.utils.PlotlyJSONEncoder)

    return graphJSON


def create_plot_real():
    # Create a trace
    data = [go.Scatter(
        x = car_stat.queue_sensor_x,
        y = car_stat.queue_sensor_y,
        mode = 'lines+markers',  # lines+dots
        marker = dict(size=[20, 30, 40, 50, 60], color='rgba(255, 109, 0, 1)')
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
    app.run(host='0.0.0.0', debug=True)
