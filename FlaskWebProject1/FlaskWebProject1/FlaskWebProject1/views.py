"""
Routes and views for the flask application.
"""
from datetime import datetime
from flask import render_template, request
from FlaskWebProject1 import app
import serial
n = 0
ser = serial.Serial('COM2', 9600)


@app.route('/')
@app.route('/home')
def home():
    """Renders the home page."""
    return render_template(
        'layout.html',
        title='home',
        nowtime=datetime.now().strftime("%Y-%m-%d"),
        year=datetime.now().year,
    )


@app.route('/refresh')
def refresh():
    data = ""
    data = ser.read(100)
    return data
    # return datetime.now().strftime("%Y-%m-%d %H:%M")


@app.route('/updateLED')
def updateLED():
    state = request.args.get('sta')
    ser.write(state.encode('utf-8'))
    return home()
