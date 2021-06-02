import json
from flask import Flask, render_template, Response, request, redirect, url_for, flash, jsonify
import plotly as py
import numpy as np
import pandas as pd
import plotly.graph_objs as go

app = Flask(__name__)

class stat:
    def __init__(self):
        self.movement="NAN132"
        self.control_mode="1"

        self.speedL=0
        self.speedR=0
        self.sensor_x=0
        self.sensor_y=0

        self.cam_x=0
        self.cam_y=0
        self.cam_width=0
        self.cam_height=0
        self.cam_depth=0
        self.cam_Analog_x=0
        self.cam_Analog_y=0


car_stat = stat()


@app.route("/",methods=['GET', 'POST'])
def home():
    return render_template("index.html")

@app.route("/esp32",methods=['GET', 'POST'])
def esp32():
    # return wanted data to esp32
    if request.method == "GET":
        which = request.args.get('which')
        if which=='movement':
            return car_stat.movement
        elif which=='control_mode':
            return car_stat.control_mode
    # get data from esp32
    if request.method == "POST":
        data = request.get_json()
        print(data)
        car_stat.speedL=data["speed"][0]
        car_stat.speedR=data["speed"][1]
        car_stat.sensor_x=data["sensor"][0]
        car_stat.sensor_y=data["sensor"][1]
        return jsonify({"state": "ok"})

@app.route("/mode1_button_click",methods=['GET', 'POST'])
def direction_instructions():
    if request.method == "GET":
        btn = request.args.get('a')
        if btn == 'forward':
            print('forward')
            car_stat.movement="forward"
        elif btn == 'left':
            print('left')
            car_stat.movement="left"
        elif btn == 'right':
            print('right')
            car_stat.movement="right"
        elif btn == 'stop':
            print('stop')
            car_stat.movement="stop"
        elif btn == 'backward':
            print('backward')
            car_stat.movement="backward"
        elif btn == 'mode1':
            print('controller')
            car_stat.control_mode="button"
        elif btn == 'mode2':
            print('sensor')
            car_stat.control_mode="sensor"
        elif btn == 'mode3':
            print('camera')
            car_stat.control_mode="camera"
        else:
            print('btn error')

    return "nothing"


@app.route('/mode2_button_click')
def plot_trigger():
    btn = request.args.get('a')
    if btn == '1':
        scatter = create_plot()
        print('action')
    else:
        print('end')


@app.route('/fresh')
def fresh():
    scatter = create_plot()
    return jsonify({"scatter": scatter})


@app.route('/sensor', methods=['GET', 'POST'])
def index():
    scatter = create_plot()
    return render_template('sensor_plot.html', plot=scatter)


def create_plot():

    N = 5
    random_x = np.random.randint(-2000, 2000, N, dtype='int32')
    random_y = np.random.randint(-2000, 2000, N, dtype='int32')

    # Create a trace
    data = [go.Scatter(
        x=random_x,
        y=random_y,
        mode='markers',
        # marker_color='rgba(152, 0, 0, .8)',
        marker=dict(size=30, color='LightSkyBlue')
    )]

    graphJSON = json.dumps(data, cls=py.utils.PlotlyJSONEncoder)

    return graphJSON




if __name__ == "__main__":
    app.run(host='0.0.0.0',debug=True)
