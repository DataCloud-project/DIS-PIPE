from distutils.command.build_scripts import first_line_re
import flask

from flask import request, jsonify, render_template, redirect
from pm4py.algo.filtering.log.attributes import attributes_filter
import os
import sys
from jinja2 import Undefined
import requests
import json
import re


#IMAGES_FOLDER = os.path.join('static', 'images')
LOGS_FOLDER = "/event logs"
IMAGE_FOLDER = os.path.join('static', 'images')

app = flask.Flask(__name__)
app.config["DEBUG"] = True
app.config['UPLOAD_FOLDER'] = LOGS_FOLDER

import graphviz
from graphviz import Digraph
#import file xes
from pm4py.objects.log.importer.xes import importer as xes_importer
from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
from pm4py.visualization.dfg import visualizer as dfg_visualization

import pm4py
from pm4py.objects.dfg.filtering import dfg_filtering
from pm4py.algo.discovery.inductive import algorithm as inductive_miner
from pm4py.visualization.petri_net import visualizer as pn_visualizer
from pm4py.objects.conversion.process_tree import converter
from pm4py.algo.filtering.log.start_activities import start_activities_filter
from pm4py.algo.filtering.log.end_activities import end_activities_filter
from pm4py.algo.discovery.footprints import algorithm as footprints_discovery
from pm4py.objects.log.util import interval_lifecycle
from pm4py.algo.filtering.log.variants import variants_filter
from pm4py.util import constants
#log = xes_importer.apply(log_path)

nomeupload=""
# start_case=False
# global first
first=0
dfg_f=None

with open('../properties.txt') as f:
    lines = f.readlines()
    backend=lines[1]
    backend = backend.split(': ')
    # backend = backend.split('//')*********
    
    path = backend[1]
    # path = backend[1].split(':')[0]**********

    # ******************************************+
    port_n = backend[1].split(':')[1]
    port_n = port_n.split('/')[0]
    # *******************************************

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
    global nomeupload 
    # global first
    # if(first==0):
    #     nomeupload=file
    
    # first=1
    # global start_case
    # start_case=False
 
    return render_template("index.html", \
        stringF = "", \
        stringP = "", \
        stringDuration = "", \
        stringUsedVarible = "", \
        stringEdgeDuration = "", \
        stringEdgeFrequency = "", \
        stringFrequency = "", \
        stringPetriNet = "", \
        median = "", \
        total = "", \
        myPathF_init = "100", \
        myActF_init = "100", \
        myPathP_init = "100", \
        myActP_init = "100", \
        perf_checked = "false" , \
        path = http, \
        filename = file, \
        nameupload = nomeupload     ) 







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
  print(f.filename)

  regex_expression="^[\w\.,\s-]+\.xes$"
  check = re.search(regex_expression, f.filename)

  global nomeupload
  if(check):
    print("")
  else:
    print("file not allowed")
    return home(nomeupload)

    
  

  if f.filename == '':
    print("empty")
  #f.save("event logs/" + f.filename)
  f.save(log_path)

  
  nomeupload = f.filename

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

def createGraphF(log):
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    
    #DFG - process discovery
    #dfg_freq = dfg_discovery.apply(log)
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    #visualize DFG - frequency
    
    #gviz_freq = dfg_visualization.apply(dfg_freq, log=log, variant=dfg_visualization.Variants.FREQUENCY)
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    #dfg_visualization.save(gviz_freq, "static/images/frequency.png")
    #freq_img = os.path.join(app.config['UPLOAD_FOLDER'], 'frequency.png')
    
    return gviz_freq


def createGraphP(log):

    # log = xes_importer.apply(log_path) 
    # log = interval_lifecycle.to_interval(log)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
                                                        
    #DFG - process discovery
    #dfg_perf = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    #parameters = {dfg_visualization.Variants.PERFORMANCE.value.Parameters.FORMAT: "svg"}
    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    dfg = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    
    #visualize DFG - performance 
    gviz_perf = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.PERFORMANCE,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    
    #gviz_perf = dfg_visualization.apply(dfg_perf, log=log, variant=dfg_visualization.Variants.PERFORMANCE)
    #dfg_visualization.view(gviz)
    #dfg_visualization.save(gviz_perf, "static/images/performance.png")
    #perf_img = os.path.join(app.config['UPLOAD_FOLDER'], 'performance.png')
    
    #return render_template("index.html", img_freq = freq_img, img_perf = perf_img, string = str(gviz_freq))
    #string_html = render_template("string.html", string = str(gviz_freq))
    #frequency = render_template("img_freq.html", img_freq = freq_img)
    #performance = render_template("img_perf.html", img_perf = perf_img)
    
    return gviz_perf


@app.route('/dfgFrequency', methods=['GET', 'POST'])
def dfgFrequency():
    log = xes_importer.apply(log_path)
    return str(createGraphF(log))


@app.route('/dfgPerformance', methods=['GET', 'POST'])
def dfgPerformance():
    log = xes_importer.apply(log_path)
    return str(createGraphP(log))


# ********************************************************

def createGraphFReduced(log):
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if request.args.get('myActF') == None:
        act = 100;
    else:
        act = int(request.args.get('myActF'))
    if request.args.get('myPathF') == None:
        path = 100;
    else:
        path = int(request.args.get('myPathF'))
    #print("Freq: "+str(act)+" "+str(path))

    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)
    gviz_f = dfg_visualization.apply(dfg_f, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
                                                
    return gviz_f


def createGraphPReduced(log):
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if request.args.get('myActP') == None:
        act = 100;
    else:
        act = int(request.args.get('myActP'))
    if request.args.get('myPathP') == None:
        path = 100;
    else:
        path = int(request.args.get('myPathP'))
    #print("Perf: "+str(act)+" "+str(path))
    

    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    dfg_p = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    gviz_f = dfg_visualization.apply(dfg_p, log=log, variant=dfg_visualization.Variants.PERFORMANCE, 
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})
                                                
    return gviz_f


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
        
    
    ##path = request.args.get('myPathF')
    #paramsF = {'myPathF' : myPathF, 'myActF' : myActF}

    #f = requests.get(path.strip('\n')+'dfgFreqReduced', params = paramsF)

    ##if request.form.get('updated') != None:
    #if request.args.get('updated') != None:
    #    f = request.files['file']
    #    if f.filename != '': 
    #      #f.save("event logs/" + f.filename)
    #      f.save(log_path)
    #      return home(f.filename)      
      
        
    # log = xes_importer.apply(log_path)

    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    global log
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if myActF == None:
        act = 100;
    else:
        act = int(myActF)
    
    if myPathF == None:
        path = 100;
    else:
        path = int(myPathF)
    #print("Freq: "+str(act)+" "+str(path))
    global dfg_f
    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)
    gviz_f = dfg_visualization.apply(dfg_f, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
                                                
    return str(gviz_f)


@app.route('/dfgPerfReduced', methods=['GET', 'POST'])
def dfgPerfReduced():
    # if request.method == 'GET':
    myPathP = request.args.get('myPathP')
    myActP = request.args.get('myActP')
    perfCheck = request.args.get('perf_checked')
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathP' : myPathP, 'myActP' : myActP}
    # p = requests.get(path.strip('\n')+'dfgPerfReduced', params = paramsP)

    # #print(request.form.get('updated'))
    # if request.form.get('updated') != None:
    #     f = request.files['file']
    #     if f.filename != '': 
    #       #f.save("event logs/" + f.filename)
    #       f.save(log_path)
    #       return home(f.filename)      
    
    # log = xes_importer.apply(log_path)

    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #print(type(request.args.get('myPahtF')))
    global log
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    if myActP == None:
        act = 100;
    else:
        act = int(myActP)
    
    if myPathP == None:
        path = 100;
    else:
        path = int(myPathP)
    #print("Perf: "+str(act)+" "+str(path))
    

    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    gviz_f = dfg_visualization.apply(dfg_p, log=log, variant=dfg_visualization.Variants.PERFORMANCE, 
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})
                                                
    return str(gviz_f)


# *****************************************************************************************************************

@app.route('/allduration', methods=['GET'])
def allduration():
    
    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics

    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    
    global log

    global activity_list
    activity_list = []
    for i in range(0, len(log)):
        for j in range(0, len(log[i])):
            if(log[i][j]['concept:name'] not in activity_list):
                activity_list.append(log[i][j]['concept:name'])

    # global activity_dictionary
    activity_dictionary=dict.fromkeys(activity_list)
    
    for a in activity_list:
        activity_dictionary[a]=[]
    
    for i in range(0, len(log)):
        for p in range(0, len(log[i])):
            activity_dictionary[log[i][p]['concept:name']].append(log[i][p]['@@duration'])

    

    mean_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        mean_dizionario[j]=statistics.mean(activity_dictionary[j])

        
    total_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        total_dizionario[j]=sum(activity_dictionary[j])
        
    median_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        median_dizionario[j]=activity_dictionary[j][int(len(activity_dictionary[j])/2)]
        
    max_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        max_dizionario[j]=max(activity_dictionary[j])
        
        
    min_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        min_dizionario[j]=min(activity_dictionary[j])

    return str(mean_dizionario)+"*"+str(total_dizionario)+"*"+str(median_dizionario)+"*"+str(max_dizionario)+"*"+str(min_dizionario)



@app.route('/alledgeduration', methods=['GET'])
def alledgeduration():

    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics

    global log

    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    follower_dictionary={}
    follower_list=[]
    follower_used_list=[]
    import datetime
    import statistics
    already_sum=[]

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            for k in range(j, len(log[i])):
                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg and j!=k and ((log[i][j]['concept:name']==log[i][k]['concept:name'] and j+1==k )or (log[i][j]['concept:name'],log[i][k]['concept:name']) not in already_sum )):  
                    already_sum.append((log[i][j]['concept:name'],log[i][k]['concept:name']))         
                    if((log[i][j]['concept:name'],log[i][k]['concept:name']) not in follower_list):
                        follower_list.append((log[i][j]['concept:name'],log[i][k]['concept:name']))
                        follower_used_list.append(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'])
                        follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=[]

                    time_result=log[i][k]['start_timestamp']-log[i][j]['time:timestamp']
                    time_result=time_result.total_seconds()
                    follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']].append(time_result)
                    # print(follower_dictionary)
        already_sum=[]

   
    mean_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        mean_edge_dizionario[j]=sum(follower_dictionary[j])/len(follower_dictionary[j])

    total_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        total_edge_dizionario[j]=sum(follower_dictionary[j])

    median_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        median_edge_dizionario[j]=statistics.median(follower_dictionary[j])

    max_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        max_edge_dizionario[j]=max(follower_dictionary[j])

    min_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        min_edge_dizionario[j]=min(follower_dictionary[j])

    return str(mean_edge_dizionario)+"*"+str(total_edge_dizionario)+"*"+str(median_edge_dizionario)+"*"+str(max_edge_dizionario)+"*"+str(min_edge_dizionario)
#******************************************************************************************************************


@app.route('/allfrequency', methods=['GET'])
def allfrequency():

    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics


    global log
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
    dictionary_absolute={}

    for trace in log:
        for ev in trace: 
            if(ev['concept:name'] in dictionary_absolute):
                dictionary_absolute[ev['concept:name']]=str(int(dictionary_absolute[ev['concept:name']])+1)
            else:
                dictionary_absolute[ev['concept:name']]=str(1)

    
    ###################

    #CASE FREQ
    dictionary_case={}
    already_case=[]

    for trace_case in log:
        for ev_case in trace_case:
            if(ev_case['concept:name'] not in already_case):
                already_case.append(ev_case['concept:name'])
                if(ev_case['concept:name'] in dictionary_case):
                    dictionary_case[ev_case['concept:name']]=str(int(dictionary_case[ev_case['concept:name']])+1)
                else:
                    dictionary_case[ev_case['concept:name']]=str(1)
                    
        already_case=[]

    ###################

    #max repetititions
    dictionary_max_trace={}
    dictionary_max={}


    for trace_max in log:
        for ev_max in trace_max:
    #         print(precedente)
    #         if(ev_max['concept:name']!=precedente):
            if(ev_max['concept:name'] in dictionary_max_trace):
                dictionary_max_trace[ev_max['concept:name']]=str(int(dictionary_max_trace[ev_max['concept:name']])+1)
            else:
                dictionary_max_trace[ev_max['concept:name']]=str(1)
                
            if(ev_max['concept:name'] in dictionary_max):
                if(int(dictionary_max_trace[ev_max['concept:name']])>int(dictionary_max[ev_max['concept:name']])):
                    dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
                
            else:
                dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
        
        dictionary_max_trace={}


    return str(dictionary_absolute)+"*"+str(dictionary_case)+"*"+str(dictionary_max)



@app.route('/alledgefrequency', methods=['GET'])
def alledgefrequency():

    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics


    global log
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    # print(dfg)
    # print(start_activities)
    # print(end_activities)

    #ABSOLUTE FREQUENCY
    #absolute frequency
    follower_dictionary_absolute={}
    import statistics

    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_absolute):
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)

    for i in range(0, len(log)):
        j=0
        if(log[i][j]['concept:name'] in start_activities):

            if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_absolute):
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
            else:
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']])+1)

        k=len(log[i])-1
        if(log[i][k]['concept:name'] in end_activities):

            if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_absolute):
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
            else:
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"])+1)



   
   
   
   
    ###################

    # case frequency
    follower_dictionary_case={}
    already_use_case=[]
    import statistics


    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):

            k=j+1
            if(log[i][j]['concept:name'] not in already_use_case):
                already_use_case.append(log[i][j]['concept:name'])

                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                    if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_case):
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                    else:
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
        already_use_case=[]



    for i in range(0, len(log)):
        j=0
        
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
        
            if(log[i][j]['concept:name'] in start_activities):

                if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_case):
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
                else:
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']])+1)
        already_use_case=[]
                    
            
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
                    
            k=len(log[i])-1
            if(log[i][k]['concept:name'] in end_activities):

                if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_case):
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
                else:
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"])+1)

        already_use_case=[]
                        

    ###################

    #max frequency
    follower_dictionary_max={}
    follower_max_dictionary_trace={}
    content=""
    import statistics

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
    #         for k in range(j, len(log[i])):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                content=log[i][j]['concept:name']+"#"+log[i][k]['concept:name']
                if(content not in follower_max_dictionary_trace):
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
                    
            if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] in follower_dictionary_max):
                if(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])>int(follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])):
                    follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
                
            else:
                follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
        
        follower_max_dictionary_trace={}

    

    return str(follower_dictionary_absolute)+"*"+str(follower_dictionary_case)+"*"+str(follower_dictionary_max)



# *****************************************************************************************************************

@app.route('/variants', methods=['GET', 'POST'])
def variants():
    if(1):
        log = xes_importer.apply(log_path)
        log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
        variants = variants_filter.get_variants(log)
        # global start_case
        # start_case= False

    
    variantsDict = '{'

    j=0
    for var, trace in variants.items():

        
        cases = len(trace)
        info = (list(variants.values())[j][0])
        info = info.__getattribute__('attributes')
        #Apri la variante
        if("variant-index" in info):
            variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            caseName = info['concept:name']

            
            variantsDict = variantsDict + '{"'+str(caseName)+'":['

            for x in trace[i]:
                timestamp = x['time:timestamp']
                x['time:timestamp'] = str(timestamp)
                start_timestamp = x['start_timestamp']
                start_timestamp_back=start_timestamp
                x['start_timestamp'] = str(start_timestamp)

                stringX = str(x).replace("'",'"')

                variantsDict = variantsDict + '' + stringX #+', '
            variantsDict = variantsDict + ']}' # chiude ogni caso
        variantsDict = variantsDict + ']' # chiude ogni variante
        j =j+1
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')        
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')

    return variantsDict



#****************************************************************************************************************************************


@app.route('/filter', methods=['GET', 'POST'])
def filter():
    # GET request
    if request.method == 'GET':
        __min_sec = request.args.get('min')
        __max_sec = request.args.get('max')

        __min_event = request.args.get('minevent')
        __max_event = request.args.get('maxevent')
        # print(__min_event);
        # print(__max_event)

        myPathF = request.args.get('myPathF')
        myActF = request.args.get('myActF')
        myPathP = request.args.get('myPathP')
        myActP = request.args.get('myActP')
        
        perfCheck = request.args.get('perf_checked')

        __Start = request.args.get('start')
        __End = request.args.get('end')

        filterTime = request.args.get('filterTime')
        timeframe = request.args.get('timeframe')
        
        filterPerf = request.args.get('filterPerf')  
        perfFrame = request.args.get('perfFrame') #

        filterAttr = request.args.get('filterAttr')  
        attrFrame = request.args.get('attrFrame') #
        attrFrameFilt = request.args.get('attrFilt') #

        list_attr= request.args.get('listattr') #

    list_attr=list_attr.replace("@",".")
              
    
    if perfCheck == None:
        perfCheck = "false";
    else:
        perfCheck = "true";
        
    if filterPerf == None:
        filterPerf = "false";
        
    if filterTime == None:
        filterTime = "false";

    if filterAttr == None:
        filterAttr = "false";
        
    
    
    
    #path = request.args.get('myPathF')
    paramsP = {'myPathF' : myPathF, 'myActF' : myActF, 'myPathP' : myPathP, 'myActP' : myActP, 'min' : __min_sec, 'max' : __max_sec, 'start' : __Start, 'end' : __End, 'timeframe' : timeframe, "filterTime": filterTime, "filterPerf": filterPerf}
    # p = requests.get(path.strip('\n')+'filter', params = paramsP)

    # #print(request.form.get('updated'))
    # if request.form.get('updated') != None:
    #     f = request.files['file']
    #     if f.filename != '': 
    #       #f.save("event logs/" + f.filename)
    #       f.save(log_path)
    #       return home(f.filename)      
    # global start_case
    global log
   
    # if(start_case==False):
        # log = xes_importer.apply(log_path)
        # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
        #                                                     constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
        #                                                     constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
        #open text file
    # text_file = open('C:\\Users\\jacop\\Desktop\\xp.txt', "w")
    
    # #write string to file
    # text_file.write(str(log))
    
    # #close file
    # text_file.close()
    variants = variants_filter.get_variants(log)
    
    

    
    

    
    from pm4py.algo.filtering.log.timestamp import timestamp_filter
    from pm4py.algo.filtering.log.cases import case_filter
    from dateutil.parser import parse
    from pm4py.algo.filtering.log.timestamp import timestamp_filter
    from pm4py.algo.filtering.log.attributes import attributes_filter
    
    # GET
    if __Start == None:
        start = 0;
    else:
        start = __Start
    
    if __End == None:
        end = 0;
    else:
        end = __End

    if __min_sec == None:
        min_sec = 0;
    else:
        min_sec = float(__min_sec)
    
    if __max_sec == None:
        max_sec = 100*3,154e+7;
    else:
        max_sec = float(__max_sec)
        

    if __max_event == None or __max_event == "":
        max_event = 100*3,154e+7;
    else:
        max_event = int(__max_event)

    if __min_event == None or __min_event == "":
        min_event = 0;
    else:
        min_event = int(__min_event)

    global boolean_case
    boolean_case=False

    # log = xes_importer.apply(log_path)

    if filterTime == "true":
        if timeframe == 'contained':
            # print("timeframe contained")
            
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.filter_traces_contained(log, start, end)
            log = filtered_log
    
        elif timeframe == 'intersecting':
            # print("timeframe intersecting")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log

        elif timeframe == 'started':
            # print("timeframe started")
            # log = xes_importer.apply(log_path)
            
            # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
            #                                                 constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
            #                                                 constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
            boolean_case=True
            variants = variants_filter.get_variants(log)
            filtered_log= pm4py.filter_log(lambda x: str(x[0]['start_timestamp'])>=(start) and str(x[0]['start_timestamp'])<=(end), log)
            # filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log

        elif timeframe == 'completed':
            # print("timeframe completed")
            # log = xes_importer.apply(log_path)
            
            # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
            #                                                 constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
            #                                                 constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
            boolean_case=True
            variants = variants_filter.get_variants(log)
            # print(x[len(x)-1]['time:timestamp'])
            # print(end)
            # print(parse(end))
            filtered_log= pm4py.filter_log(lambda x: str(x[len(x)-1]['time:timestamp'])<=(end) and str(x[len(x)-1]['time:timestamp'])>=(start), log)
            log = filtered_log

        elif timeframe == 'trim':
            # print("timeframe started")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.apply_events(log, start, end)
            # print(start)
            # print(end)
            log = filtered_log
            # print(len(log))
    # else:
        # print("else")
        # log = xes_importer.apply(log_path)
        # variants = variants_filter.get_variants(log)
        # filtered_log = log



    if filterPerf == "true":

        if perfFrame=="caseDuration":
            # print("performance case duration")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = case_filter.filter_case_performance(log, min_sec, max_sec)
            log = filtered_log

        elif perfFrame == 'eventsNumber':
            # __min_event
            # print("performance eventsNumber")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = pm4py.filter_log(lambda x: len(x) >=min_event and len(x) <=max_event , log)
            log = filtered_log

    # else:
        # print("else")
        # log = xes_importer.apply(log_path)
        # variants = variants_filter.get_variants(log)
        # filtered_log = log


    activity_list=[]
    resource_list=[]
    cost_list=[]
    cost_list_2=[]
    variant_list=[]
    caseId_list=[]
    resource_list=list_attr.split(",")
   

    if filterAttr == "true":

        if attrFrame == "activity":
            # print("attr activity")
            activity_list=list_attr.split(",")
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, activity_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, activity_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, activity_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log

            
        elif attrFrame == 'resource':
            # print("attr resource")
            
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                # print(log)

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log


        elif attrFrame == 'costs':
            # print("attr costs")
            cost_list=list_attr.split(",")
            # print(cost_list)
            
            for i in cost_list:
                cost_list_2.append(float(i))
            
            '''
            if attrFrameFilt=="selected":
                log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, cost_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="mandatory":
                log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="forbidden":
                log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log) 
                filtered_log = attributes_filter.apply(log, cost_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
            '''
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, cost_list_2 ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list_2 ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list_2, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log

        elif attrFrame == 'variants':
            # print("attr variants")
            variant_list=list_attr.split(",")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log) 
            filtered_log = attributes_filter.apply_trace_attribute(log, variant_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "variant", attributes_filter.Parameters.POSITIVE: True})
            log = filtered_log


        elif attrFrame == 'caseID':
            # print("attr caseID")
            caseId_list=list_attr.split(",")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log) 
            filtered_log = attributes_filter.apply_trace_attribute(log, caseId_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
            log = filtered_log            

    
    # else:
        # print("else")
        # filtered_log = log

    # f = createGraphFReduced(filtered_log)
    # p = createGraphPReduced(filtered_log)
    # variants = variants_filter.get_variants(filtered_log)


    # print(len(log))

    # f = createGraphFReduced(log)
    # p = createGraphPReduced(log)
    variants = variants_filter.get_variants(log)
    text_file = open('C:\\Users\\jacop\\Desktop\\xp2.txt', "w")
        
    #write string to file
    text_file.write(str(log))
    
    #close file
    text_file.close()

    # prova f
    # if(start_case==False):
        # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
        #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
        #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    boolean_case=True

    # start_case=True
    #print(type(request.args.get('myPahtF')))
    
    # GET
    #print(type(request.args.get('myPathF')))
    #x = request.args.get('myPathF')
    
    #print("Freq: "+str(act)+" "+str(path))

    dfg_f, sa_f, ea_f = pm4py.discover_directly_follows_graph(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    activities_count_f = pm4py.get_event_attribute_values(log, "concept:name")
    # dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_activities_percentage(dfg_f, sa_f, ea_f, activities_count_f, act/100)
    # dfg_f, sa_f, ea_f, activities_count_f = dfg_filtering.filter_dfg_on_paths_percentage(dfg_f, sa_f, ea_f, activities_count_f, path/100)

    gviz_f = dfg_visualization.apply(dfg_f, log=log,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_f,
                                                parameters.END_ACTIVITIES: ea_f})
    f=str(gviz_f)


    dfg_p, sa_p, ea_p = pm4py.discover_directly_follows_graph(log)
    # dfg_p = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters
    activities_count_p = pm4py.get_event_attribute_values(log, "concept:name")
    # dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_activities_percentage(dfg_p, sa_p, ea_p, activities_count_p, act/100)
    # dfg_p, sa_p, ea_p, activities_count_p = dfg_filtering.filter_dfg_on_paths_percentage(dfg_p, sa_p, ea_p, activities_count_p, path/100)
    
    gviz_p = dfg_visualization.apply(dfg_p, log=log,  
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: sa_p,
                                                parameters.END_ACTIVITIES: ea_p})

    p=gviz_p
    
    '''
    variantsDict = '{'

    cases = len(filtered_log)
    print(cases)
    j=0
    for i in range(0, cases):
        j=j+1
        info = filtered_log[i].__getattribute__('attributes')
        caseName = info['concept:name']
        if ("variant-index" in info):
            varIndex = info['variant-index']
            if (i == 0):
                variantsDict = variantsDict + '"' + str(varIndex) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        variantsDict = variantsDict + '{"' + str(caseName) + '":['

        for x in filtered_log[i]:
            timestamp = x['time:timestamp']
            x['time:timestamp'] = str(timestamp)
            stringX = str(x).replace("'", '"')
            variantsDict = variantsDict + '' + stringX  # +', '
        
        variantsDict = variantsDict + ']}'  # chiude ogni caso
        if ("variant-index" not in info):
            variantsDict = variantsDict + ']' # chiude ogni variante
    if ("variant-index" in info):
        variantsDict = variantsDict + ']' # chiude ogni variante
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')
    '''
    '''
    variantsDict = '{'

    j=-1
    for var, trace in variants.items():

        j =j+1
        cases = len(trace)
        print("Numero cases var "+str(j)+": "+str(cases))
        varEmpty = True
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            #print("info: "+str(info))
            caseName = info['concept:name']
            print()
            #print(trace[i])
            inFilter = False
            for k in range(0,len(filtered_log)):
                infoFiltered = filtered_log[k].__getattribute__('attributes')
                filteredCaseName = infoFiltered['concept:name']
                if(filteredCaseName == caseName):
                    inFilter = True
                    break

            if(inFilter == False):
                break
            if(i==0):
                varEmpty = False
                if("variant-index" in info):
                    variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
                else:
                    variantsDict = variantsDict + '"' + str(j) + '": ['
                #variantsDict = variantsDict + '"' + str(j) + '": ['
            variantsDict = variantsDict + '{"'+str(caseName)+'":['
            #print("Trac i len: "+str(len(trace[i])))
            for x in trace[i]:
                timestamp = x['time:timestamp']
                x['time:timestamp'] = str(timestamp)
                stringX = str(x).replace("'",'"')
                variantsDict = variantsDict + '' + stringX #+', '
            variantsDict = variantsDict + ']}' # chiude ogni caso
        if(varEmpty == False):
            variantsDict = variantsDict + ']' # chiude ogni variante

    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')
    #variantsDict = variantsDict.replace('}"','},"')
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')
    '''



    varianti_array=[]
    variants = variants_filter.get_variants(log)
    variantsDict = '{'

    j=0
    for var, trace in variants.items():

        
        cases = len(trace)
        info = (list(variants.values())[j][0])
        info = info.__getattribute__('attributes')
        # print(info)
        #Apri la variante
        if("variant-index" in info):
            # print(str(info['variant-index']))
            if(info['variant-index'] in varianti_array):
                variantsDict = variantsDict + '"' + str(max(varianti_array)+1) + '": ['
                varianti_array.append(max(varianti_array)+1)
            else:
                variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
                varianti_array.append(info['variant-index'])
        else:
            # print("else  "+str(j))
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            caseName = info['concept:name']
            
            
            variantsDict = variantsDict + '{"'+str(caseName)+'":['

            for x in trace[i]:
                timestamp = x['time:timestamp']
                timestamp_back=timestamp
              
                # print(timestamp)
                # print(type(timestamp))
                x['time:timestamp'] = str(timestamp)

                # if(boolean_case):
            
                start_timestamp = x['start_timestamp']
                start_timestamp_back=start_timestamp
                x['start_timestamp'] = str(start_timestamp)

                stringX = str(x).replace("'",'"')
                variantsDict = variantsDict + '' + stringX #+', '
                
                x['time:timestamp']=timestamp_back
                x['start_timestamp'] = start_timestamp_back


            variantsDict = variantsDict + ']}' # chiude ogni caso
        variantsDict = variantsDict + ']' # chiude ogni variante
        j =j+1
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')        
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')
    
    result = str(f)+"|||"+str(p)+"|||"+str(variantsDict)
    # result=str(variantsDict)
    # print(variantsDict)
    # start_case=True
    return result   

    # ******************************************************************************
    # ******************************************************************************

@app.route('/usedvariable', methods=['GET', 'POST'])
def usedvariable():
    log = xes_importer.apply(log_path)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    activities = attributes_filter.get_attribute_values(log, "concept:name")
    resources = attributes_filter.get_attribute_values(log, "org:resource")
    resources_cost = json.dumps(attributes_filter.get_attribute_values(log, "resourceCost")) 
    caseid = attributes_filter.get_trace_attribute_values(log, "concept:name")
    variant = attributes_filter.get_trace_attribute_values(log, "variant")

    stringX = str(str(activities)+"*"+str(resources)+"*"+str(resources_cost)+"*"+str(caseid)+"*"+str(variant)).replace("'",'"')

    return stringX

@app.route('/initialAction', methods=['GET', 'POST'])
def initialAction():
    # IMPORT
    global log
    global log_duplicate
    log = xes_importer.apply(log_path)
    #

    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    
    log_duplicate=log

    global dfg
    #DFG - process discovery
    #dfg_freq = dfg_discovery.apply(log)
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    #visualize DFG - frequency
    
    #gviz_freq = dfg_visualization.apply(dfg_freq, log=log, variant=dfg_visualization.Variants.FREQUENCY)
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    #dfg_visualization.save(gviz_freq, "static/images/frequency.png")
    #freq_img = os.path.join(app.config['UPLOAD_FOLDER'], 'frequency.png')
    
    grafo_frequency=(str(gviz_freq))
    #fine graph frequency______________________________________________________________________________________________________________

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    dfg = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters

    #visualize DFG - performance 
    gviz_perf = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.PERFORMANCE,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})

    

    grafo_performance=(str(gviz_perf))
    #fine graph performance______________________________________________________________________________________________________________

    activities = attributes_filter.get_attribute_values(log, "concept:name")
    resources = attributes_filter.get_attribute_values(log, "org:resource")
    resources_cost = json.dumps(attributes_filter.get_attribute_values(log, "resourceCost")) 
    caseid = attributes_filter.get_trace_attribute_values(log, "concept:name")
    variant = attributes_filter.get_trace_attribute_values(log, "variant")

    stringX = str(str(activities)+"*"+str(resources)+"*"+str(resources_cost)+"*"+str(caseid)+"*"+str(variant)).replace("'",'"')

    variabili_usare=(stringX)
    #fine all used variable______________________________________________________________________________________________________________  


    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics

    
    global activity_list
    activity_list = []
    for i in range(0, len(log)):
        for j in range(0, len(log[i])):
            if(log[i][j]['concept:name'] not in activity_list):
                activity_list.append(log[i][j]['concept:name'])

    # global activity_dictionary
    activity_dictionary=dict.fromkeys(activity_list)
    
    for a in activity_list:
        activity_dictionary[a]=[]
    
    for i in range(0, len(log)):
        for p in range(0, len(log[i])):
            activity_dictionary[log[i][p]['concept:name']].append(log[i][p]['@@duration'])

    
    mean_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        mean_dizionario[j]=statistics.mean(activity_dictionary[j])
        
    total_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        total_dizionario[j]=sum(activity_dictionary[j])
        
    median_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        median_dizionario[j]=activity_dictionary[j][int(len(activity_dictionary[j])/2)]
        
    max_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        max_dizionario[j]=max(activity_dictionary[j])
        
        
    min_dizionario=dict.fromkeys(activity_list)
    for j in activity_list:
        min_dizionario[j]=min(activity_dictionary[j])

    activity_durata=(str(mean_dizionario)+"*"+str(total_dizionario)+"*"+str(median_dizionario)+"*"+str(max_dizionario)+"*"+str(min_dizionario))
    #fine all duration_______________________________________________________________________________________________________________________

    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics

    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    follower_dictionary={}
    follower_list=[]
    follower_used_list=[]
    import datetime
    import statistics

    already_sum=[]

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            for k in range(j, len(log[i])):
                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg and j!=k and ((log[i][j]['concept:name']==log[i][k]['concept:name'] and j+1==k )or (log[i][j]['concept:name'],log[i][k]['concept:name']) not in already_sum )):  
                    already_sum.append((log[i][j]['concept:name'],log[i][k]['concept:name']))         
                    if((log[i][j]['concept:name'],log[i][k]['concept:name']) not in follower_list):
                        follower_list.append((log[i][j]['concept:name'],log[i][k]['concept:name']))
                        follower_used_list.append(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'])
                        follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=[]

                    time_result=log[i][k]['start_timestamp']-log[i][j]['time:timestamp']
                    time_result=time_result.total_seconds()
                    follower_dictionary[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']].append(time_result)
                    
        already_sum=[]


   
    mean_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        mean_edge_dizionario[j]=sum(follower_dictionary[j])/len(follower_dictionary[j])

    total_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        total_edge_dizionario[j]=sum(follower_dictionary[j])

    median_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        median_edge_dizionario[j]=statistics.median(follower_dictionary[j])

    max_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        max_edge_dizionario[j]=max(follower_dictionary[j])

    min_edge_dizionario=dict.fromkeys(follower_used_list)
    for j in follower_used_list:
        min_edge_dizionario[j]=min(follower_dictionary[j])

    durata_edge=(str(mean_edge_dizionario)+"*"+str(total_edge_dizionario)+"*"+str(median_edge_dizionario)+"*"+str(max_edge_dizionario)+"*"+str(min_edge_dizionario))
    

    #___________________________________________________________________________

    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics


    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
    dictionary_absolute={}

    for trace in log:
        for ev in trace: 
            if(ev['concept:name'] in dictionary_absolute):
                dictionary_absolute[ev['concept:name']]=str(int(dictionary_absolute[ev['concept:name']])+1)
            else:
                dictionary_absolute[ev['concept:name']]=str(1)

    
    ###################

    #CASE FREQ
    dictionary_case={}
    already_case=[]

    for trace_case in log:
        for ev_case in trace_case:
            if(ev_case['concept:name'] not in already_case):
                already_case.append(ev_case['concept:name'])
                if(ev_case['concept:name'] in dictionary_case):
                    dictionary_case[ev_case['concept:name']]=str(int(dictionary_case[ev_case['concept:name']])+1)
                else:
                    dictionary_case[ev_case['concept:name']]=str(1)
                    
        already_case=[]

    ###################

    #max repetititions
    dictionary_max_trace={}
    dictionary_max={}


    for trace_max in log:
        for ev_max in trace_max:
    #         print(precedente)
    #         if(ev_max['concept:name']!=precedente):
            if(ev_max['concept:name'] in dictionary_max_trace):
                dictionary_max_trace[ev_max['concept:name']]=str(int(dictionary_max_trace[ev_max['concept:name']])+1)
            else:
                dictionary_max_trace[ev_max['concept:name']]=str(1)
                
            if(ev_max['concept:name'] in dictionary_max):
                if(int(dictionary_max_trace[ev_max['concept:name']])>int(dictionary_max[ev_max['concept:name']])):
                    dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
                
            else:
                dictionary_max[ev_max['concept:name']]=str(dictionary_max_trace[ev_max['concept:name']])
        
        dictionary_max_trace={}


    activity_frequency= str(dictionary_absolute)+"*"+str(dictionary_case)+"*"+str(dictionary_max)

    #___________________________________________________________________________

    from pm4py.statistics.traces.generic.log import case_statistics
    from pm4py.algo.organizational_mining.resource_profiles import algorithm
    from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
    import time
    import statistics

    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
    #absolute frequency
    follower_dictionary_absolute={}
    import statistics

    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_absolute):
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_absolute[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)

    for i in range(0, len(log)):
        j=0
        if(log[i][j]['concept:name'] in start_activities):

            if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_absolute):
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
            else:
                follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_absolute["@@startnode"+"#"+log[i][j]['concept:name']])+1)

        k=len(log[i])-1
        if(log[i][k]['concept:name'] in end_activities):

            if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_absolute):
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
            else:
                follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_absolute[log[i][k]['concept:name']+"#"+"@@endnode"])+1)



   
   
   
   
   
   
    ###################

    # case frequency
    follower_dictionary_case={}
    already_use_case=[]
    import statistics


    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):

            k=j+1
            if(log[i][j]['concept:name'] not in already_use_case):
                already_use_case.append(log[i][j]['concept:name'])

                if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                    if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] not in follower_dictionary_case):
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                    else:
                        follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_dictionary_case[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
        already_use_case=[]
                        


    for i in range(0, len(log)):
        j=0
        
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
        
            if(log[i][j]['concept:name'] in start_activities):

                if("@@startnode"+"#"+log[i][j]['concept:name'] not in follower_dictionary_case):
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(1)
                else:
                    follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']]=str(int(follower_dictionary_case["@@startnode"+"#"+log[i][j]['concept:name']])+1)
        already_use_case=[]
                    
            
        if(log[i][j]['concept:name'] not in already_use_case):
            already_use_case.append(log[i][j]['concept:name'])
                    
            k=len(log[i])-1
            if(log[i][k]['concept:name'] in end_activities):

                if(log[i][k]['concept:name']+"#"+"@@endnode" not in follower_dictionary_case):
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(1)
                else:
                    follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"]=str(int(follower_dictionary_case[log[i][k]['concept:name']+"#"+"@@endnode"])+1)

        already_use_case=[]

    ###################

    #max frequency
    follower_dictionary_max={}
    follower_max_dictionary_trace={}
    content=""
    import statistics

    # print(dfg)
    # print("\n")
    for i in range(0, len(log)):
        for j in range(0, len(log[i])-1):
    #         for k in range(j, len(log[i])):
            k=j+1
            if((log[i][j]['concept:name'],log[i][k]['concept:name']) in dfg):
                content=log[i][j]['concept:name']+"#"+log[i][k]['concept:name']
                if(content not in follower_max_dictionary_trace):
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(1)
                else:
                    follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])+1)
                    
            if(log[i][j]['concept:name']+"#"+log[i][k]['concept:name'] in follower_dictionary_max):
                if(int(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])>int(follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])):
                    follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
                
            else:
                follower_dictionary_max[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']]=str(follower_max_dictionary_trace[log[i][j]['concept:name']+"#"+log[i][k]['concept:name']])
        
        follower_max_dictionary_trace={}

    

    frequency_edge= str(follower_dictionary_absolute)+"*"+str(follower_dictionary_case)+"*"+str(follower_dictionary_max)



    #___________________________________________________________________________
     
    variants = variants_filter.get_variants(log)
    # global start_case
    # start_case= False

    
    variantsDict = '{'

    j=0
    for var, trace in variants.items():

        
        cases = len(trace)
        info = (list(variants.values())[j][0])
        info = info.__getattribute__('attributes')
        #Apri la variante
        if("variant-index" in info):
            variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            caseName = info['concept:name']

            
            variantsDict = variantsDict + '{"'+str(caseName)+'":['

            for x in trace[i]:
                # timestamp = x['time:timestamp']
                # x['time:timestamp'] = str(timestamp)
                # start_timestamp = x['start_timestamp']
                # start_timestamp_back=start_timestamp
                # x['start_timestamp'] = str(start_timestamp)

                # stringX = str(x).replace("'",'"')

                # variantsDict = variantsDict + '' + stringX #+', '
                timestamp = x['time:timestamp']
                timestamp_back=timestamp
                x['time:timestamp'] = str(timestamp)
                start_timestamp = x['start_timestamp']
                start_timestamp_back=start_timestamp
                x['start_timestamp'] = str(start_timestamp)

                stringX = str(x).replace("'",'"')
                variantsDict = variantsDict + '' + stringX #+', '
                
                x['time:timestamp']=timestamp_back
                x['start_timestamp'] = start_timestamp_back

                if('resourceCost' not in x):
                    x['resourceCost']=0

            variantsDict = variantsDict + ']}' # chiude ogni caso
        variantsDict = variantsDict + ']' # chiude ogni variante
        j =j+1
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')        
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')

    varianti=(variantsDict)
    #variants fine_______________________________________________________________________________________________________________________





    #___________________________________________________________________________

    return grafo_frequency+"£"+grafo_performance+"£"+variabili_usare+"£"+activity_durata+"£"+varianti+"£"+durata_edge+"£"+activity_frequency+"£"+frequency_edge


@app.route('/swipeRemoveAction', methods=['GET', 'POST'])
def swipeRemoveAction():
    global log
    global log_duplicate

    log=log_duplicate

    ####start frequency_______________________________________________________________________________________
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    #visualize DFG - frequency
    
    #gviz_freq = dfg_visualization.apply(dfg_freq, log=log, variant=dfg_visualization.Variants.FREQUENCY)
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})
    #dfg_visualization.save(gviz_freq, "static/images/frequency.png")
    #freq_img = os.path.join(app.config['UPLOAD_FOLDER'], 'frequency.png')
    
    grafo_frequency=(str(gviz_freq))

    ####end frequency_________________________________________________________________________________________




    ####start performance_____________________________________________________________________________________
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    dfg = dfg_discovery.apply(log, variant=dfg_discovery.Variants.PERFORMANCE)
    parameters = dfg_visualization.Variants.PERFORMANCE.value.Parameters

    #visualize DFG - performance 
    gviz_perf = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.PERFORMANCE,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})

    

    grafo_performance=(str(gviz_perf))

    ####end performance_______________________________________________________________________________________


    ####start variants________________________________________________________________________________________

    variants = variants_filter.get_variants(log)
    # global start_case
    # start_case= False

    
    variantsDict = '{'

    j=0
    for var, trace in variants.items():

        
        cases = len(trace)
        info = (list(variants.values())[j][0])
        info = info.__getattribute__('attributes')
        #Apri la variante
        if("variant-index" in info):
            variantsDict = variantsDict + '"' + str(info['variant-index']) + '": ['
        else:
            variantsDict = variantsDict + '"' + str(j) + '": ['
        
        for i in range(0, cases):
            info = (list(variants.values())[j][i])
            info = info.__getattribute__('attributes')
            caseName = info['concept:name']

            
            variantsDict = variantsDict + '{"'+str(caseName)+'":['

            for x in trace[i]:
                # timestamp = x['time:timestamp']
                # x['time:timestamp'] = str(timestamp)
                # start_timestamp = x['start_timestamp']
                # start_timestamp_back=start_timestamp
                # x['start_timestamp'] = str(start_timestamp)

                # stringX = str(x).replace("'",'"')

                # variantsDict = variantsDict + '' + stringX #+', '
                timestamp = x['time:timestamp']
                timestamp_back=timestamp
                x['time:timestamp'] = str(timestamp)
                start_timestamp = x['start_timestamp']
                start_timestamp_back=start_timestamp
                x['start_timestamp'] = str(start_timestamp)

                stringX = str(x).replace("'",'"')
                variantsDict = variantsDict + '' + stringX #+', '
                
                x['time:timestamp']=timestamp_back
                x['start_timestamp'] = start_timestamp_back

            variantsDict = variantsDict + ']}' # chiude ogni caso
        variantsDict = variantsDict + ']' # chiude ogni variante
        j =j+1
    variantsDict = variantsDict + '}' # chiude tutto

    variantsDict = variantsDict.replace("][","],[")
    variantsDict = variantsDict.replace("}{","},{")
    variantsDict = variantsDict.replace(']"','],"')        
    variantsDict = variantsDict.replace('True','"True"')
    variantsDict = variantsDict.replace('False','"False"')

    varianti=(variantsDict)


    ####end variants_________________________________________________________________________________________



    return grafo_frequency+"£"+grafo_performance+"£"+varianti



@app.route('/conformanceChecking', methods=['GET', 'POST'])
def conformanceChecking():

    global log
    global dfg

    activities = pm4py.get_event_attribute_values(log, "concept:name")
    from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
    from pm4py.algo.discovery.alpha import algorithm as alpha_miner
    global dfg_f
    if(dfg_f!=None):
        dfg_conf =dfg_f
    else:
        dfg_conf = dfg

    from pm4py.objects.conversion.dfg import converter as dfg_mining
    # net, im, fm = dfg_mining.apply(dfg_conf)
    net, im, fm = alpha_miner.apply(log)

    gviz = pn_visualizer.apply(net, im, fm)
    # pn_visualizer.view(gviz)
    places = net.places
    transitions = net.transitions
    arcs = net.arcs
    trst=[]

    for tr in transitions:
        # print((str(tr.name)+" "+str(tr.label)))
        trst.append([str(tr.name), str(tr.label)])

    from pm4py.objects.petri_net.exporter import exporter as pnml_exporter
    # pnml_exporter.apply(net, im, "petri.pnml")



    pnml_exporter.apply(net, im, "net\\petri_final.pnml", final_marking=fm)
    with open('net\\marking.txt', 'w') as f:
        f.write(str(im))
        f.write('\n')
        f.write(str(fm))

    from pm4py.objects.log.exporter.xes import exporter as xes_exporter
    xes_exporter.apply(log, 'net\\petri_log.xes')

    return str(gviz)+"£"+str(im)+"£"+str(fm)+"£"+str(list(activities))+"£"+str(trst)


app.run(host=path_f, port=int(port_n))