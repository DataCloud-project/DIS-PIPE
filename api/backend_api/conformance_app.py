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
#_________________CONFORMANCE CHECKING API_________________#
############################################################

app_conformance = Blueprint('app_conformance',__name__)

@app_conformance.route('/conformanceChecking', methods=['GET', 'POST'])
def conformanceChecking():
    # global log
    # global dfg
    dfg=session["dfg"]

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

    activities = pm4py.get_event_attribute_values(log, "concept:name")
    from pm4py.algo.discovery.dfg import algorithm as dfg_discovery
    from pm4py.algo.discovery.alpha import algorithm as alpha_miner
    dfg_f=session["dfg_f"]
    if(dfg_f!=None):
        dfg_conf =dfg_f
    else:
        dfg_conf = dfg

    # Print the current working directory
    working_dir=os.getcwd()
    #global backup_dir
    session["backup_dir"]=working_dir
    print("Current working directory: {0}".format(os.getcwd()))
    os.chdir(working_dir+'/jar')
    

    # os.system("java -jar traceAligner.jar align d31.pnml d31.xes cost_file 10 40 SYMBA false")
    
    # subprocess.call(['bash', './run_SYMBA_all'])

    os.chdir(working_dir)

    from pm4py.objects.conversion.dfg import converter as dfg_mining
    # net, im, fm = dfg_mining.apply(dfg_conf)
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    from pm4py.objects.conversion.log import converter as log_converter
    static_event_stream = log_converter.apply(log, variant=log_converter.Variants.TO_EVENT_STREAM)
    from pm4py.algo.discovery.inductive import algorithm as inductive_miner
    tree = inductive_miner.apply_tree(log, variant=inductive_miner.Variants.IMf)
    from pm4py.objects.conversion.process_tree import converter as pt_converter
    net, im, fm = pt_converter.apply(tree)

    # net, im, fm = alpha_miner.apply(log)
 
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




    pnml_exporter.apply(net, im, session["pnml_path"], final_marking=fm)
    with open(session["marking_path"], 'w') as f:
        f.write(str(im))
        f.write('\n')
        f.write(str(fm))

    xes_exporter.apply(log, session["xes_path"])

    

    with open(session["cost_file_path"], "w") as f:
        for index in trst:
            if(index[1].lower().replace(" ", "")=="none"):
                f.write(index[0].lower().replace(" ", "")+" 0 0")
            else:
                f.write(index[1].lower().replace(" ", "")+" 1 1")    
            f.write('\n')
        #f.write("none"+" 0 0") remove comment to consider invisible transitions
        f.close()

    session["dfg"]=dfg

    return str(gviz)+"£"+str(im)+"£"+str(fm)+"£"+str(list(activities))+"£"+str(trst)

@app_conformance.route('/jarCalling', methods=['GET', 'POST'])
def jarCalling():
    minLen = str(request.args.get('minLen'))
    maxLen = str(request.args.get('maxLen'))
    planner = str(request.args.get('planner'))
    duplicate = str(request.args.get('duplicate'))


    # Print the current working directory
    working_dir=os.getcwd()
    print("Current working directory: {0}".format(os.getcwd()))
    os.chdir(working_dir+'/jar')
    pnmlPath="../net/petri_final_remap.pnml"
    xesPath="../net/petri_log.xes"
    costPath="cost_file"

    #os.system("java -jar traceAligner.jar align "+pnmlPath+" "+xesPath+" "+costPath+" "+minLen +" "+maxLen+" "+planner+" "+duplicate)
    global process_jar
    process_jar = subprocess.Popen( "java -jar traceAligner11.jar align "+pnmlPath+" "+xesPath+" "+costPath+" "+minLen +" "+maxLen+" "+planner+" "+duplicate , shell=True)
    
    try:
        print('Running in process', process_jar.pid)
        process_jar.wait()
    except subprocess.TimeoutExpired:
        print('Timed out - killing', process_jar.pid)
        process_jar.kill()
    print("\njar done")
    
    
    print("  \n")
    global plans_path
    global process_script
    
    if(planner=="FD"):    
        plans_path="./jar/fast-downward/src/plans"
        
        process_script=subprocess.Popen(['bash', './run_FD_all'])
        
        try:
            print('Running in process', process_script.pid)
            process_script.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', process_script.pid)
            process_script.kill()
        print("\nscript done")
    
    elif(planner=="SYMBA"):
        plans_path="./jar/seq-opt-symba-2/plans"
        
        process_script=subprocess.Popen(['bash', './run_SYMBA_all'])
        #os.system("bash ./run_SYMBA_all")
        
        try:
            print('Running in process', process_script.pid)
            process_script.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', process_script.pid)
            process_script.kill()
        print("\nscript done")
        
    else:
        plans_path="./jar/fast-downward/src/plans"
        
        process_script=subprocess.Popen(['bash', './run_FD_all'])
        #os.system("bash ./run_FD_all")
        
        try:
            print('Running in process', process_script.pid)
            process_script.wait()
        except subprocess.TimeoutExpired:
            print('Timed out - killing', process_script.pid)
            process_script.kill()
        print("\nscript done")
       
    os.chdir(working_dir)

    return "done"

@app_conformance.route('/costFile', methods=['POST'])
def costFile():
    costHeader=request.headers.get('Contenuto')
    costJson = json.loads(costHeader)

    with open(cost_file_path, "w") as f:
        for singleCost in costJson:
            f.write(singleCost.lower().replace(" ", "")+" "+str(costJson[singleCost][0])+" "+str(costJson[singleCost][1]))    
            f.write('\n')
        f.close()

    return "done"

@app_conformance.route('/traceDetail', methods=['GET'])
def traceDetail():
    allTraceName=""

    working_dir=os.getcwd()
    print("Current working directory: {0}".format(os.getcwd()))

    global plans_path

    os.chdir(plans_path)
    for fileName in glob.glob("*.txt"):
        # print(fileName)
        traceIndex=(fileName.replace("out","").replace(".txt",""))
        allTraceName=allTraceName+traceIndex+"#"
        
    allTraceName = allTraceName[:-1]
    os.chdir(working_dir)
    
    return allTraceName

@app_conformance.route('/updateTraceDetail', methods=['GET'])
def updateTraceDetail():
    nameTrace = str(request.args.get('nameTrace'))
    global plans_path
  
    file_name = os.path.basename(plans_path+'/'+nameTrace)
    with open(plans_path+'/'+nameTrace) as f:
        trace = f.readlines()

    start_index=len(trace)
    start_string="Actual search time:"

    end_index=len(trace)
    end_string="Plan length:"

    plan_cost_string="Plan cost:"
    
    plan_cost=""
    plan_length=""
    search_time=""

    alignment=""
    i=0

    while(i<len(trace)-1):
        if(start_string in trace[i].strip()):
            start_index=i
            search_time=trace[i].strip().split(" [",1)[1].replace("t=","").replace("]","")

        if(end_string in trace[i+1].strip()):
            end_index=i+1
            plan_length=trace[i+1].strip().split("Plan length: ",1)[1].replace(".","").strip()

        
        if(plan_cost_string in trace[i+1].strip()):
            plan_cost=trace[i+1].strip().split("Plan cost:",1)[1].strip()
        
        if(i>start_index and i<end_index):
            alignment=alignment+trace[i].strip()+"\n"
        
        i=i+1
    
    return alignment+"$"+search_time+"$"+plan_length+"$"+plan_cost

@app_conformance.route('/generalTraceInfo', methods=['GET'])
def generalTraceInfo():
    dict_event = {}
    dict_trace = {} 
    temp_array = [] 
  
    dict_skip_ins ={} 

    working_dir=os.getcwd()

    # plans_path="./jar/fast-downward/src/plans" 
    # plans_path="./jar/seq-opt-symba-2/plans"
    global plans_path
    os.chdir(plans_path)
    for fileName in glob.glob("*.txt"):
        # print(fileName)
        traceIndex=(fileName.replace("out","").replace(".txt",""))
        # print(traceIndex)

        with open('./'+str(fileName)) as f:
            trace = f.readlines()

        start_index=len(trace)
        start_string="Actual search time:"

        end_index=len(trace)
        end_string="Plan length:"

        alignment=""
        i=0

        while(i<len(trace)-1):
            # print(trace[i].strip())
            trace_to_add=trace[i].strip()
            if(start_string in trace_to_add):
                # print("\ninizio: ")
                start_index=i

            if(end_string in trace[i+1].strip()):
                # print("fine: \n")
                end_index=i+1
            
            if(i>start_index and i<end_index):
                alignment=alignment+trace_to_add+"\n"

            trace_to_analyse=trace_to_add.split("#")

            if(trace_to_analyse[0]=="movesync"):
                if trace_to_analyse[1].split("  ")[0].strip() not in dict_event:
                    dict_event[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if (trace_to_analyse[1].split("  ")[0].strip()) not in dict_trace:
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if  trace_to_analyse[1].split("  ")[0].strip() not in temp_array:
                    # temp_array.append(trace_to_analyse[1].split("  ")[0].strip())
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()][0]= dict_trace[trace_to_analyse[1].split("  ")[0].strip()][0]+1

                
                dict_event[trace_to_analyse[1].split("  ")[0].strip()][0]= dict_event[trace_to_analyse[1].split("  ")[0].strip()][0]+1
   

            elif(trace_to_analyse[0]=="moveinthemodel"):
                if trace_to_analyse[1].split("  ")[0].strip() not in dict_event:
                    dict_event[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if (trace_to_analyse[1].split("  ")[0].strip()) not in dict_trace:
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    # print("this will execute")

                if  trace_to_analyse[1].split("  ")[0].strip() not in temp_array:
                    temp_array.append(trace_to_analyse[1].split("  ")[0].strip())
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()][1]= dict_trace[trace_to_analyse[1].split("  ")[0].strip()][1]+1
                
            
                dict_event[trace_to_analyse[1].split("  ")[0].strip()][1]= dict_event[trace_to_analyse[1].split("  ")[0].strip()][1]+1
                #print("mossa nel modello: "+trace_to_analyse[1].split("  ")[0].strip() )
                #print(trace_to_analyse[1].split("  ")[0].strip())

                if (trace_to_analyse[1].split("  ")[0].strip() not in dict_skip_ins):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()]=[[],[]] 
                
                if(('Trace '+str(traceIndex)) not in dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][1]):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][1].append('Trace '+str(traceIndex))

            
            elif(trace_to_analyse[0]=="moveinthelog"):
                if trace_to_analyse[1].split("  ")[0].strip() not in dict_event:
                    dict_event[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    

                if (trace_to_analyse[1].split("  ")[0].strip()) not in dict_trace:
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()]=[0,0,0]  
                    

                if  trace_to_analyse[1].split("  ")[0].strip() not in temp_array:
                    temp_array.append(trace_to_analyse[1].split("  ")[0].strip())
                    dict_trace[trace_to_analyse[1].split("  ")[0].strip()][2]= dict_trace[trace_to_analyse[1].split("  ")[0].strip()][2]+1

                #print(trace_to_analyse[1].split("  ")[0].strip())
                dict_event[trace_to_analyse[1].split("  ")[0].strip()][2]= dict_event[trace_to_analyse[1].split("  ")[0].strip()][2]+1


                if (trace_to_analyse[1].split("  ")[0].strip() not in dict_skip_ins):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()]=[[],[]] 
                
                if(('Trace '+str(traceIndex)) not in dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][0]):
                    dict_skip_ins[trace_to_analyse[1].split("  ")[0].strip()][0].append('Trace '+str(traceIndex))
            
            
            i=i+1
        temp_array=[]
    
    os.chdir(working_dir)

    return str(dict_event)+"#"+str(dict_trace)+"#"+str(dict_skip_ins)


@app_conformance.route('/mapPnml', methods=['POST'])
def mapPnml():
    costHeader=request.headers.get('Replace_content')
    
    replace_array=costHeader.split("#")

    x=replace_array[0] 
    y=replace_array[1] 

    with open('./net/petri_final.pnml') as f:
        trace = f.readlines()

    response=""

    for line in trace:
        if "<text>"+x+"</text>" in line:
            substi=line.replace(x,y)
        else:
            substi=line
           
        response=response+substi

    f = open("./net/petri_final_remap.pnml", "w")
    f.write(response)
    f.close()
    
    return "prova"

@app_conformance.route('/getPnmlExistence', methods=['GET'])
def getPnmlExistence():
    file_exists = exists("./net/petri_final_remap.pnml")
    return str(file_exists)

@app_conformance.route('/createRemap', methods=['POST'])
def createRemap():

    with open('./net/petri_final.pnml') as f:
        trace = f.readlines()

    response=""

    for line in trace:
        substi=line
        
        response=response+substi

    f = open("./net/petri_final_remap.pnml", "w")
    f.write(response)
    f.close()

    return "prova"
    
@app_conformance.route('/deleteRemap', methods=['POST'])
def deleteRemap():
    file_exists = exists("./net/petri_final_remap.pnml")

    if(file_exists):
        os.remove("./net/petri_final_remap.pnml")    

    return "prova"

