############################################################
#____________________ALL IMPORT SECTION____________________#
############################################################
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

from flask import Flask, session
from flask_session import Session

from flask import Flask, redirect, url_for, render_template, request, session
from datetime import timedelta
import time

sys.path.insert(1, 'database')
sys.path.insert(2, 'rule_filter')
sys.path.insert(3, 'backend_api')
sys.path.insert(4, 'dsl_to_xes')

from backend_classes import *
from databaseFunctions import *
from utilities import *
from rule import *
from classes import *
from utils import *

from pm4py.objects.conversion.dfg import converter as dfg_mining
from pm4py.objects.conversion.log import converter as log_converter
from pm4py.algo.discovery.inductive import algorithm as inductive_miner
from pm4py.objects.conversion.process_tree import converter as pt_converter
from pm4py.objects.petri_net.exporter import exporter as pnml_exporter
from pm4py.algo.filtering.log.attributes import attributes_filter

import shutil

import datetime
from pm4py.objects.log.importer.xes import importer as xes_importer
from pm4py.objects.log.exporter.xes import exporter as xes_exporter
from pm4py.objects.log.log import EventLog, Trace, Event

from pathlib import Path


############################################################
#_____________________APP CONFIGUARTION____________________#
############################################################


from backend_classes import *

app_Segmentator = Blueprint('app_Segmentator',__name__)

############################################################
#__________________SEGMENTATOR API__________________#
############################################################

with open('../properties.txt') as f:
    lines = f.readlines()
    frontend = lines[0]
    frontend = frontend.split(': ')
    http = frontend[1]
    frontend = frontend[1]
    frontend = frontend.split('//')
    path_f = frontend[1].split(':')[0]
    port_f = frontend[1].split(':')[1]
    port_f = port_f.split('/')[0]
f.close()

@app_Segmentator.route('/segmentator', methods=['GET'])
def segmentator():

    working_dir=os.getcwd()
    #global backup_dir
    
    print("Current working directory: {0}".format(os.getcwd()))
    
    os.chdir(working_dir+'/AutSeg/segmentator')


    print("Current working directory, quella che mi serve: {0}".format(os.getcwd()))
    print("tutti i file sono: ")
    print(os.listdir())    
    # os.system("java -jar traceAligner.jar align d31.pnml d31.xes cost_file 10 40 SYMBA false")
    os.system('./al -d files/UILog.config')
    
    os.chdir(working_dir)

    print("Current working directory: {0}".format(os.getcwd()))

    return "work done"




def copy_folder(source_folder, destination_folder):
    try:
        shutil.copytree(source_folder, destination_folder)
        print("Folder copied successfully!")
    except shutil.Error as e:
        print(f"Folder copy failed: {e}")
    except OSError as e:
        print(f"Folder copy failed: {e}")



@app_Segmentator.route('/startSegmentator', methods=['POST'])
def startSegmentator():


    print("XDXD")
    print("start segmentator")
    print("XDXD")
    array_data = request.headers.get("Array-Data")

    # Convert the JSON string back to an array
    end_activity_array = json.loads(array_data)
    print(end_activity_array)
    print(end_activity_array[0])

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    target_trace = None
    number_of_traces=0
    for trace in log:
        #if trace.attributes['concept:name'] == 'Trace1':
        target_trace = trace
        #    break
        number_of_traces=number_of_traces+1

    if(number_of_traces==1):
        output_UILOG=process_string(session["segmentator"][1:]+'/'+session["log_name_clear"]+'_UILOG')


        with open(output_UILOG, 'w') as file:
            for event in target_trace:
                timestamp = event['time:timestamp']
                activity = event['concept:name']
                
                timestamp = datetime.datetime.fromisoformat(str(timestamp)).strftime('%Y-%m-%d %H:%M:%S%z')
                
                if(str(activity) in end_activity_array):
                    file.write(f'{timestamp+"+01:00"} {activity} {activity} ON Other_Activity\n')
                    file.write(f'{timestamp+"+01:00"} X X ON X\n')
                else:
                    file.write(f'{timestamp+"+01:00"} {activity} {activity} ON Other_Activity\n')

        copy_folder(os.getcwd()+'/AutSeg',process_string(session["segmentator"][1:]+"/AutSeg"))

        activities = attributes_filter.get_attribute_values(log, "concept:name")
        S_array = activities
        S_name = process_string(session["segmentator"][1:]+'/'+session["log_name_clear"]+'_UILOG')

        with open(process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator"+"/files"+"/UILog.config"), 'w') as file:
            file.write('sensor ' + ' '.join(S_array) + '\n')
            file.write('data ' + S_name + '\n')
            file.write('model model\n')
            file.write('numiterations -1\n')

        working_dir=os.getcwd()

        #os.chdir(process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator"))

        print("Current bis working directory: {0}".format(os.getcwd()))
        print("Current file in directory: {0}".format(os.listdir()))
        
        # os.system("java -jar traceAligner.jar align d31.pnml d31.xes cost_file 10 40 SYMBA false")
        # os.system('./al -d files/UILog.config')
        print('./'+process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator/al")+' -d '+process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator/files/UILog.config"))
        #os.system('./'+process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator/al")+' -d '+process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator/files/UILog.config"))
        process_segmentator = subprocess.Popen('./'+process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator/al")+' -d '+process_string(session["segmentator"][1:]+"/AutSeg"+"/segmentator/files/UILog.config") ,shell=True)
        
        session["pid_segmentator"]=process_segmentator.pid
        try:
            print('Running in process', session["pid_segmentator"])
            process_segmentator.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', session["pid_segmentator"])
            os.kill(session["pid_segmentator"], signal.SIGKILL) #or signal.SIGKILL 
            #process_jar.kill()
        print("\nsegmentator done")

        
        #os.chdir(working_dir)
        print("Current working directory: {0}".format(os.getcwd()))


        input_file = process_string(session["segmentator"][1:]+'/'+session["log_name_clear"]+'_UILOG.annotated')
        output_file = process_string(session["segmentator"][1:]+'/'+session["log_name_clear"]+'_output.txt')

        with open(input_file, 'r') as f_in, open(output_file, 'w') as f_out:
            for line in f_in:
                columns = line.strip().split('	')
                third_column_value = columns[2]
                first_and_second_column_value = ' '.join(columns[:2]).split('+')[0]
                #if(third_column_value!="X"):
                f_out.write(f"{third_column_value},{first_and_second_column_value},complete\n")

        print("File created successfully.")



        content = '''<log xes.version="1.0" xmlns="http://www.xes-standard.org">
        <extension name="Concept" prefix="concept" uri="http://www.xes-standard.org/concept.xesext"/>
        <extension name="Time" prefix="time" uri="http://www.xes-standard.org/time.xesext"/>
        <extension name="Lifecycle" prefix="lifecycle" uri="http://www.xes-standard.org/lifecycle.xesext"/>
        <global scope="trace">
            <string key="concept:name" value="Example Trace"/>
        </global>
        <global scope="event">
            <string key="org:resource" value="John Doe"/>
        </global>

        </log>'''

        
        existing_log_file = process_string(session["segmentator"][1:]+'/'+session["log_name_clear"]+'_annotated.xes')

        with open(existing_log_file, 'w') as file:
            file.write(content)

        print("File created successfully.")

        event_details_file = output_file
        add_events_to_trace(event_details_file, existing_log_file, session["log_path"])

    return (render_template("index.html", \
        stringF = "", \
        stringP = "", \
        traceDt = "", \
        varalternative ="",\
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
        filename = session["nomeupload"], \
        nameupload = session["nomeupload"]     ) )





def add_events_to_trace(event_details_file, existing_log_file, endfile):
    log = xes_importer.apply(existing_log_file)

    numeroTrace=0
    with open(event_details_file, 'r') as file:
        trace = Trace()
        numeroTrace=numeroTrace+1
        trace.attributes['concept:name']="Trace"+str(numeroTrace)

        for line in file:
            
            event_data = line.strip().split(',')
            concept_name = event_data[0]
            timestamp = event_data[1]
            lifecycle_transition = event_data[2]

            
            
            if(concept_name!="X"):
                event = Event()
                event['concept:name'] = concept_name
                event['time:timestamp'] = datetime.datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S')
                event['lifecycle:transition'] = lifecycle_transition
                print(event)


                trace.append(event)

            else:
                log.append(trace)

                trace = Trace()
                numeroTrace=numeroTrace+1
                trace.attributes['concept:name']="Trace"+str(numeroTrace)

            
    xes_exporter.apply(log, endfile)