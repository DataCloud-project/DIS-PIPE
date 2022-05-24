import flask

from flask import request, jsonify, render_template, redirect
import os
import sys


#IMAGES_FOLDER = os.path.join('static', 'images')
LOGS_FOLDER = "/event logs"

app = flask.Flask(__name__)
app.config["DEBUG"] = True
app.config['UPLOAD_FOLDER'] = LOGS_FOLDER

import requests
import graphviz
from graphviz import Digraph
from pm4py.visualization.dfg import visualizer as dfg_visualization

with open('../properties.txt') as f:
    lines = f.readlines()
    backend=lines[1]
    backend = backend.split(': ')
    path = backend[1]
    #print(path)

    frontend=lines[0]
    frontend = frontend.split(': ')
    http = frontend[1]
    frontend = frontend[1]
    frontend = frontend.split('//')
    path_f = frontend[1].split(':')[0]
    port_n = frontend[1].split(':')[1]
    port_n = port_n.split('/')[0]
    #print(path_f)
    print(http)
    #print(port_n)
    
f.close()

import platform
if platform.system() == "Windows":
        log_path = 'event logs\\running-example.xes'
if platform.system() == "Linux":
        log_path = 'event logs/running-example.xes'

def home(file):
 
    return render_template("index.html", \
        stringF = "", \
        stringP = "", \
        median = "", \
        total = "", \
        myPathF_init = "100", \
        myActF_init = "100", \
        myPathP_init = "100", \
        myActP_init = "100", \
        perf_checked = "false" , \
        path = http, \
        filename = file) 
    
@app.route('/', methods=['GET'])
def index():
    filename = 'running-example.xes'
    return home(filename)
    
    
@app.route('/start', methods=['GET', 'POST'])
def start():
    
    start =requests.get(path.strip('\n')+'start')
    return start.text

@app.route('/end', methods=['GET', 'POST'])
def end():
    
    end =requests.get(path.strip('\n')+'end')
    return end.text

@app.route('/', methods = ['POST'])
def upload_file():
  f = request.files['file']
  if f.filename == '':
    print("empty")
  #f.save("event logs/" + f.filename)
  f.save(log_path)
  return home(f.filename)
  #return redirect("http://127.0.0.1:8080", code=200)

@app.route('/petriFreq', methods=['GET', 'POST'])
def petriFreq():
    
    petriF = requests.get(path.strip('\n')+'petriNetFreq')

    return str(petriF.text)
    
@app.route('/petriPerf', methods=['GET', 'POST'])
def petriPerf():
    
    petriP = requests.get(path.strip('\n')+'petriNetPerf')

    return str(petriP.text)
    
@app.route('/bpmn', methods=['GET', 'POST'])
def bpmn():
    
    bpmn = requests.get(path.strip('\n')+'bpmn')

    return str(bpmn.text)
    
@app.route('/dfgFrequency', methods=['GET', 'POST'])
def dfgFrequency():
    f = requests.get(path.strip('\n')+'dfgFrequency')
    return str(f.text)
    
@app.route('/dfgPerformance', methods=['GET', 'POST'])
def dfgPerformance():
    p = requests.get(path.strip('\n')+'dfgPerformance')
    return str(p.text)
    
@app.route('/variants', methods=['GET', 'POST'])
def variants():
    p = requests.get(path.strip('\n')+'variants')
    return str(p.text)    
 
    
@app.route('/dfgFreqReduced', methods=['GET', 'POST'])
def dfgFreqReduced():
    # GET request
    #if request.method == 'GET':
    #f = requests.get('http://127.0.0.1:7777/dfgFrequency')
    #myPathF = request.form.get('myPathF')
    myPathF = request.args.get('myPathF')
    #myActF = request.form.get('myActF')
    myActF = request.args.get('myActF')
    #perfCheck = request.form.get('perf_checked')
    perfCheck = request.args.get('perf_checked')

    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    #path = request.args.get('myPathF')
    paramsF = {'myPathF' : myPathF, 'myActF' : myActF}
    f = requests.get(path.strip('\n')+'dfgFreqReduced', params = paramsF)

    #if request.form.get('updated') != None:
    if request.args.get('updated') != None:
        f = request.files['file']
        if f.filename != '': 
          #f.save("event logs/" + f.filename)
          f.save(log_path)
          return home(f.filename)      
      
        
    return str(f.text)


@app.route('/dfgPerfReduced', methods=['GET', 'POST'])
def dfgPerfReduced():
    # GET request
    if request.method == 'GET':
        myPathP = request.args.get('myPathP')
        myActP = request.args.get('myActP')
        perfCheck = request.args.get('perf_checked')
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathP' : myPathP, 'myActP' : myActP}
    p = requests.get(path.strip('\n')+'dfgPerfReduced', params = paramsP)

    #print(request.form.get('updated'))
    if request.form.get('updated') != None:
        f = request.files['file']
        if f.filename != '': 
          #f.save("event logs/" + f.filename)
          f.save(log_path)
          return home(f.filename)      
      
        
  
    return str(p.text)
    
@app.route('/filter', methods=['GET', 'POST'])
def filter():
    # GET request
    if request.method == 'GET':
        min_sec = request.args.get('min')
        max_sec = request.args.get('max')
        myPathF = request.args.get('myPathF')
        myActF = request.args.get('myActF')
        myPathP = request.args.get('myPathP')
        myActP = request.args.get('myActP')
        perfCheck = request.args.get('perf_checked')
        start = request.args.get('start')
        end = request.args.get('end')
        timeframe = request.args.get('timeframe')
        filterTime = request.args.get('filterTime')
        filterPerf = request.args.get('filterPerf')        
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    if filterPerf == None:
        filterPerf = "false";
        
    if filterTime == None:
        filterTime = "false";
        
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathF' : myPathF, 'myActF' : myActF, 'myPathP' : myPathP, 'myActP' : myActP, 'min' : min_sec, 'max' : max_sec, 'start' : start, 'end' : end, 'timeframe' : timeframe, "filterTime": filterTime, "filterPerf": filterPerf}
    p = requests.get(path.strip('\n')+'filter', params = paramsP)

    #print(request.form.get('updated'))
    if request.form.get('updated') != None:
        f = request.files['file']
        if f.filename != '': 
          #f.save("event logs/" + f.filename)
          f.save(log_path)
          return home(f.filename)      

    return str(p.text)        
'''
@app.route('/filterPerformance', methods=['GET', 'POST'])
def filterPerformance():
    # GET request
    if request.method == 'GET':
        min_sec = request.args.get('min')
        max_sec = request.args.get('max')
        myPathF = request.args.get('myPathF')
        myActF = request.args.get('myActF')
        myPathP = request.args.get('myPathP')
        myActP = request.args.get('myActP')
        perfCheck = request.args.get('perf_checked')
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathF' : myPathF, 'myActF' : myActF, 'myPathP' : myPathP, 'myActP' : myActP, 'min' : min_sec, 'max' : max_sec}
    p = requests.get(path.strip('\n')+'filterPerformance', params = paramsP)

    #print(request.form.get('updated'))
    if request.form.get('updated') != None:
        f = request.files['file']
        if f.filename != '': 
          #f.save("event logs/" + f.filename)
          f.save(log_path)
          return home(f.filename)      
      
        
    print(p.text)
    return str(p.text)

@app.route('/filterTimeframe', methods=['GET', 'POST'])
def filterTimeframe():
    # GET request
    if request.method == 'GET':
        start = request.args.get('start')
        end = request.args.get('end')
        timeframe = request.args.get('timeframe')
        myPathF = request.args.get('myPathF')
        myActF = request.args.get('myActF')
        myPathP = request.args.get('myPathP')
        myActP = request.args.get('myActP')
        perfCheck = request.args.get('perf_checked')
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathF' : myPathF, 'myActF' : myActF, 'myPathP' : myPathP, 'myActP' : myActP, 'start' : start, 'end' : end, 'timeframe' : timeframe}
    p = requests.get(path.strip('\n')+'filterTimeframe', params = paramsP)

    #print(request.form.get('updated'))
    if request.form.get('updated') != None:
        f = request.files['file']
        if f.filename != '': 
          #f.save("event logs/" + f.filename)
          f.save(log_path)
          return home(f.filename)      
      
        
    print(p.text)
    return str(p.text)
'''
app.run(host=path_f, port=int(port_n))