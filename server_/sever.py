import json
from flask import Flask, render_template, Response, request, redirect, url_for, flash, jsonify
from pyecharts.charts import Line
import plotly as py
import numpy as np
import pandas as pd
import plotly.graph_objs as go

app = Flask(__name__)


@app.route("/")
def controller():
    return render_template("index.html")


@app.route('/sensor', methods=['GET', 'POST'])
def sensor():
    scatter = create_plot()
    return render_template('sensor_plot.html', plot=scatter)


@app.route('/mode1_button_click')
def direction_instructions():
    btn = request.args.get('a')
    if btn == 'forward':
        print('forward')
    elif btn == 'left':
        print('left')
    elif btn == 'right':
        print('right')
    elif btn == 'stop':
        print('stop')
    elif btn == 'backward':
        print('backward')
    else:
        print('btn error')

    return "nothing"


@app.route('/mode2_button_click')
def plot_trigger():
    btn = request.args.get('a')
    if btn == '1':
        print('action')
    else:
        print('end')


@app.route('/fresh', methods=['GET', 'POST'])
def fresh():
    scatter = create_plot()
    print(scatter)
    return scatter


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
    app.run()