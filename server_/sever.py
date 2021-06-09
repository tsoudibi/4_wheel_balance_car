import json
import random

import numpy as np
import plotly as py
import plotly.graph_objs as go
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)


# car status
class status:
    def __init__(self):
        self.new = 0

        self.movement = "NAN132"
        self.control_mode = "1"

        self.speedL = 0
        self.speedR = 0
        self.sensor_x = 0
        self.sensor_y = 0

        self.control_L = 0
        self.control_R = 0

        self.cam_x = 0
        self.cam_y = 0
        self.cam_width = 0
        self.cam_height = 0
        self.cam_depth = 0
        self.cam_Analog_x = 0
        self.cam_Analog_y = 0


car_stat = status()


@app.route("/")
def controller():
    car_stat.control_mode = "button"
    return render_template("index.html", data=None)


@app.route('/sensor', methods=['GET', 'POST'])
def sensor():
    car_stat.control_mode = "sensor"

    # 狀態初始化
    data = {'stopBtn': 'stop'}

    return render_template('sensor_plot.html', data=data)


@app.route('/camera', methods=['GET', 'POST'])
def camera():
    car_stat.control_mode = "camera"

    # 狀態初始化
    data = {'speedL': 'Getting...',
            'speedR': 'Getting...',
            'statusL': 'Getting...',
            'statusR': 'Getting...',
            'control_L': 'Getting...',
            'control_R': 'Getting...'
            }

    return render_template('camera.html', data=data)


@app.route("/esp32", methods=['GET', 'POST'])
def esp32():
    # return wanted data to esp32
    if request.method == "GET":
        which = request.args.get('which')
        if which == 'movement':
            car_stat.new = 0
            return car_stat.movement
        elif which == 'control_mode':
            return str(car_stat.control_mode)
        elif which == 'speed':
            return str(car_stat.speedL)+","+str(car_stat.speedR)
        elif which == 'sensor':
            return str(car_stat.sensor_x)+","+str(car_stat.sensor_y)
        elif which == 'new':
            return str(car_stat.new)
    # get data from esp32
    if request.method == "POST":
        data = request.get_json()
        print(data)
        car_stat.speedL = data["speed"][0]
        car_stat.speedR = data["speed"][1]
        car_stat.sensor_x = data["sensor"][0]
        car_stat.sensor_y = data["sensor"][1]
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
    if btn == 'stop':
        data = {'stopBtn': 'continue'}
        return data
    else:
        data = {'stopBtn': 'stop'}
        return data


@app.route('/newPlot', methods=['GET', 'POST'])
def newPlot():
    scatter = create_plot()
    # print(scatter)
    return scatter


@app.route('/newStatus', methods=['GET', 'POST'])
def newStatus():
    data = {'speedL': random.randrange(100),
            'speedR': random.randrange(100),
            'statusL': random.randrange(100),
            'statusR': random.randrange(100),
            'control_L': random.randrange(100),
            'control_R': random.randrange(100)
            }

    return data


def create_plot():

    N = 5
    random_x = np.random.randint(-2000, 2000, N, dtype='int32')
    random_y = np.random.randint(-2000, 2000, N, dtype='int32')

    # Create a trace
    data = [go.Scatter(
        x=random_x,
        y=random_y,
        mode='lines+markers',  # lines+dots
        # marker_color='rgba(152, 0, 0, .8)',
        marker=dict(size=30, color='LightSkyBlue')
    )]

    graphJSON = json.dumps(data, cls=py.utils.PlotlyJSONEncoder)

    return graphJSON


if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
