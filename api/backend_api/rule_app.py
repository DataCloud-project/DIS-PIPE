############################################################
#____________________ALL IMPORT SECTION____________________#
############################################################
from flask import Blueprint, render_template, session,abort
import flask
import os
import sys
import requests
import json
import re
import glob
import subprocess
import datetime
import graphviz
import platform
import dateutil.parser
import decimal
import pm4py
import statistics
import ast
import pyodbc
import psycopg2
import sqlvalidator
import pandas as pd
from flask_oidc import OpenIDConnect
from flask_cors import CORS
from flask import Flask,redirect
from keycloak import KeycloakOpenID
from graphviz import Digraph
from flask import request, jsonify, render_template, redirect, send_file
from pm4py.algo.filtering.log.attributes import attributes_filter
from distutils.command.build_scripts import first_line_re
from jinja2 import Undefined
from collections import Counter
from multiprocessing import Process
from time import sleep
from os.path import exists
from pm4py.objects.log.importer.xes import importer as xes_importer
from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
from pm4py.visualization.dfg import visualizer as dfg_visualization
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
from pm4py.statistics.traces.generic.log import case_statistics
from pm4py.algo.organizational_mining.resource_profiles import algorithm
from pm4py.algo.discovery.temporal_profile import algorithm as temporal_profile_discovery
from pm4py.objects.log.exporter.xes import exporter as xes_exporter
sys.path.insert(1, 'database')
sys.path.insert(2, 'rule_filter')
sys.path.insert(3, 'backend_api')
from databaseFunctions import *
from utilities import *
from rule import *

from backend_classes import *

############################################################
#______________________API RULE FILTER_____________________#
############################################################

app_scan = Blueprint('app_scan',__name__)

@app_scan.route('/scan', methods=['POST'])
def scan():
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    #log = pm4py.read_xes(log_path)  
    allXESActivities = []
    for trace in log:
        activities = []
        for event in trace:
            activities.append(event["concept:name"])
        allXESActivities.append(activities)           
    allActivities = []
    actWithOccurence = []
    c = Counter()
    for act in allXESActivities:
        c[tuple(act)] += 1
        if act not in allActivities:
            allActivities.append(act)        
    listActivity = takeActions(allActivities) 
    for elem in c:
        list = [c[elem], elem]
        actWithOccurence.append(list)    
    clear()  
    actWithOccurence = sortFirstAscendentOrder(actWithOccurence)
    with open('rule_filter/segments.txt', 'w') as f:
        i = 0
        for line in actWithOccurence:
            f.write(str(line))
            f.write("segment_"+str(i))
            f.write('\n')
            i += 1
    f.close()
    replaceInFile("rule_filter/segments.txt")
    fileHandle = open("rule_filter/segments.txt", "r")
    texts = fileHandle.readlines()
    fileHandle.close()
    fileHandle = open("rule_filter/trace.txt", "w")
    for s in texts:
        fileHandle.write(s)
    fileHandle.close()
    timestamp = takeTimestamp()
    with open(os.path.join(session["TIMESTAMP_FOLDER"], 'log_' + timestamp + '.txt'), 'w+') as file:
        file.write("UPLOAD: " + timestamp + "\n")
    file.close()
    # flash("Successfully loaded", "success")       
    result = takeSegmentFromFile()

    return jsonify({"activity": listActivity, "nameFile": session["nomeupload"], "segments": result}) 

#__________________________________________________________________________________________________________________________________________________________________________________________________________________

@app_scan.route('/scan/start_activity', methods=['POST'])
def start_activity():
    result, removeSegment = rule_start_activity()
    return jsonify({"result": result, "remove": removeSegment}) 

@app_scan.route('/scan/end_activity', methods=['POST'])
def end_activity():
    result, removeSegment = rule_end_activity()
    return jsonify({"result": result, "remove": removeSegment}) 
          
@app_scan.route('/scan/existence', methods=['POST'])
def existence():
    result, removeSegment = rule_existence()
    return jsonify({"result": result, "remove": removeSegment})  

@app_scan.route('/scan/absence', methods=['POST'])
def absence():
    result, removeSegment = rule_absence()
    return jsonify({"result": result, "remove": removeSegment})               
   
@app_scan.route('/scan/choice', methods=['POST'])
def choice():
    result, removeSegment = rule_choice()
    return jsonify({"result": result, "remove": removeSegment})  

@app_scan.route('/scan/exclusive_choice', methods=['POST'])
def exclusive_choice():
    result, removeSegment = rule_exclusive_choice()
    return jsonify({"result": result, "remove": removeSegment}) 

@app_scan.route('/scan/responded_existence', methods=['POST'])
def responded_existence():
    result, removeSegment = rule_responded_existence()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/response', methods=['POST'])
def response():
    result, removeSegment = rule_response()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/alternate_response', methods=['POST'])
def alternate_response():
    result, removeSegment = rule_alternate_response()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/chain_response', methods=['POST'])
def chain_response():
    result, removeSegment = rule_chain_response()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/precedence', methods=['POST'])
def precedence():
    result, removeSegment = rule_precedence()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/alternate_precedence', methods=['POST'])
def alternate_precedence():
    result, removeSegment = rule_alternate_precedence()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/chain_precedence', methods=['POST'])
def chain_precedence():
    result, removeSegment = rule_chain_precedence()
    return jsonify({"result": result, "remove": removeSegment})  

@app_scan.route('/scan/co_existence', methods=['POST'])
def co_existence():
    result, removeSegment = rule_co_existence()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/succession', methods=['POST'])
def succession():
    result, removeSegment = rule_succession()
    return jsonify({"result": result, "remove": removeSegment})     
 
@app_scan.route('/scan/alternate_succession', methods=['POST'])
def alternate_succession():
    result, removeSegment = rule_alternate_succession()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/chain_succession', methods=['POST'])
def chain_succession():
    result, removeSegment = rule_chain_succession()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/not_co_existence', methods=['POST'])
def not_co_existence():
    result, removeSegment = rule_not_co_existence()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/not_succession', methods=['POST'])
def not_succession():
    result, removeSegment = rule_not_succession()
    return jsonify({"result": result, "remove": removeSegment})

@app_scan.route('/scan/not_chain_succession', methods=['POST'])
def not_chain_succession():
    result, removeSegment = rule_not_chain_succession()
    return jsonify({"result": result, "remove": removeSegment})     

@app_scan.route('/scan/del_rule', methods=['POST'])
def delete_rule():
    result, removeSegment = del_rule()
    order = request.form["order"]
    if order == "ascending": result = sortDescendentOrder(result)
    else: result = sortAscendentOrder(result)
    writeOnSegmentFile(result)
    return jsonify({"result": result, "remove": removeSegment}) 

@app_scan.route('/scan/write_apply', methods=['POST'])
def write_apply():
    fun = request.form["fun"]
    act1 = request.form["act1"]
    act2 = request.form["act2"]
    list_of_files = glob.glob(session["TIMESTAMP_FOLDER"] + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("APPLY: " + timestamp + "," + fun + "," + act1 + "," + act2 + "\n")
    file.close()
    return render_template('index.html');

@app_scan.route('/scan/write_delete', methods=['POST'])
def write_delete():
    fun = request.form["fun"]
    act1 = request.form["act1"]
    act2 = request.form["act2"]
    list_of_files = glob.glob(session["TIMESTAMP_FOLDER"] + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("DELETE: " + timestamp + "," + fun + "," + act1 + "," + act2 + "\n")
    file.close()
    return render_template('index.html');

@app_scan.route('/scan/show_trace', methods=['POST'])
def show_trace():      
    order = request.form["order"]
    result = takeSegmentFromTrace()
    removeSegment = []
    if(order == "ascending"): result = sortDescendentOrder(result)
    else: result = sortAscendentOrder(result)
    writeOnSegmentFile(result)     
    return jsonify({"result": result, "remove": removeSegment}) 
              
@app_scan.route('/scan/ascending_order', methods=['POST'])
def ascending_order():
    segments = takeSegmentFromFile()
    remove = takeRemoveSegmentFromFile()
    seg_ord = sortAscendentOrder(segments)
    rem_ord = sortAscendentOrder(remove)
    writeOnSegmentFile(seg_ord)
    writeOnRemoveSegmentFile(rem_ord)
    return jsonify({"result": seg_ord, "remove": rem_ord}) 

@app_scan.route('/scan/descending_order', methods=['POST'])
def descending_order():
    segments = takeSegmentFromFile()
    remove = takeRemoveSegmentFromFile()
    seg_ord = sortDescendentOrder(segments)
    rem_ord = sortDescendentOrder(remove)
    writeOnSegmentFile(seg_ord)
    writeOnRemoveSegmentFile(rem_ord)
    return jsonify({"result": seg_ord, "remove": rem_ord}) 
          
@app_scan.route('/scan/download_file')      
def download():
    downloadFile()
    list_of_files = glob.glob(session["TIMESTAMP_FOLDER"] + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("EXPORT: " + timestamp + "\n")
    file.close()
    return render_template('index.html');  

@app_scan.route('/scan/clear')
def clearDiv():
    clear()
    list_of_files = glob.glob(session["TIMESTAMP_FOLDER"] + '/*') 
    latest_file = max(list_of_files, key=os.path.getctime)
    with open(latest_file, 'a') as file:
        timestamp = takeTimestamp()
        file.write("CLEAR: " + timestamp + "\n")
    file.close()
    return render_template('index.html');