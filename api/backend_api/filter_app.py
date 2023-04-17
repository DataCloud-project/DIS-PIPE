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
#_______________________FILTER API_________________________#
############################################################

app_filter = Blueprint('app_filter',__name__)


@app_filter.route('/variants', methods=['GET', 'POST'])
def variants():
    if(1):
        #global log_path
        log = xes_importer.apply(session["log_path"])
        log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
        variants = variants_filter.get_variants(log)
    
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

@app_filter.route('/filterScan', methods=['GET'])
def filterScan():
    variant_list= request.args.get('variantList') #
    variant_list_array=json.loads(variant_list)
    # global log
    
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    #global boolean_case
    session["boolean_case"]=False
    #variants = variants_filter.get_variants(log)
    filtered_log = pm4py.filter_variants(log,variant_list_array)
    log = filtered_log
    
    dataframe = pm4py.convert_to_dataframe(filtered_log)
    session["log"]= dataframe.to_dict()
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    session["boolean_case"]=True

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
    


    alternative_variants_array=[]

    result = str(f)+"|||"+str(p)

    return result 
 
@app_filter.route('/filter', methods=['GET', 'POST'])
def filter():
    if request.method == 'GET':
        __min_sec = request.args.get('min')
        __max_sec = request.args.get('max')

        __min_event = request.args.get('minevent')
        __max_event = request.args.get('maxevent')

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
        plusMode= request.args.get('plusmode')

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
    
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)



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

    #global boolean_case
    session["boolean_case"]=False

    # log = xes_importer.apply(log_path)

    if filterTime == "true":
        if timeframe == 'contained':
            # print("timeframe contained")
            
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.filter_traces_contained(log, start, end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            
    
        elif timeframe == 'intersecting':
            # print("timeframe intersecting")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif timeframe == 'started':
            # print("timeframe started")
            # log = xes_importer.apply(log_path)
            
            # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
            #                                                 constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
            #                                                 constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
            session["boolean_case"]=True
            variants = variants_filter.get_variants(log)
            filtered_log= pm4py.filter_log(lambda x: str(x[0]['start_timestamp'])>=(start) and str(x[0]['start_timestamp'])<=(end), log)
            # filtered_log = timestamp_filter.filter_traces_intersecting(log, start, end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif timeframe == 'completed':
            # print("timeframe completed")
            # log = xes_importer.apply(log_path)
            
            # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
            #                                                 constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
            #                                                 constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
            session["boolean_case"]=True
            variants = variants_filter.get_variants(log)
            # print(x[len(x)-1]['time:timestamp'])
            # print(end)
            # print(parse(end))
            filtered_log= pm4py.filter_log(lambda x: str(x[len(x)-1]['time:timestamp'])<=(end) and str(x[len(x)-1]['time:timestamp'])>=(start), log)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif timeframe == 'trim':
            # print("timeframe started")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log)
            filtered_log = timestamp_filter.apply_events(log, start, end)
            # print(start)
            # print(end)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            
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
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

        elif perfFrame == 'eventsNumber':
            # __min_event
            # print("performance eventsNumber")
            # log = xes_importer.apply(session["log_path"])
            variants = variants_filter.get_variants(log)
            
    
            filtered_log = pm4py.filter_log(lambda x: len(x) >=min_event and len(x) <=max_event , log)
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            

    # else:
        # print("else")
        # log = xes_importer.apply(log_path)
        # variants = variants_filter.get_variants(log)
        # filtered_log = log


    activity_list=[]
    resource_list=[]
    cost_list=[]
    cost_list_2=[]
    
    caseId_list=[]
    resource_list=list_attr.split(",")
   
    variant_list=[]

    if filterAttr == "true":

        if attrFrame == "activity":
            # print("attr activity")
            activity_list=list_attr.split(",")
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, activity_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, activity_list ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, activity_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            
        elif attrFrame == 'resource':
            # print("attr resource")
            
            if attrFrameFilt=="selected":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply_events(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                
                # print(log)

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, resource_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "org:resource", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                


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
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="mandatory":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list_2 ,parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: True})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

            elif attrFrameFilt=="forbidden":
                # log = xes_importer.apply(log_path)
                variants = variants_filter.get_variants(log)
                filtered_log = attributes_filter.apply(log, cost_list_2, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "resourceCost", attributes_filter.Parameters.POSITIVE: False})
                log = filtered_log
                
                dataframe = pm4py.convert_to_dataframe(filtered_log)
                session["log"]= dataframe.to_dict()
                dataframe1=pd.DataFrame(session["log"])
                log = pm4py.convert_to_event_log(dataframe1)
                

        elif attrFrame == 'variants':
            # print("attr variants")
            
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log) 
            #print(plusMode)
            if(plusMode=="1"):
                #print("plusmode")
                variant_list_array=json.loads(list_attr)
                filtered_log = pm4py.filter_variants(log,variant_list_array)
                #filtered_log = attributes_filter.apply_trace_attribute(log, variant_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
            else:
                variant_list=list_attr.split(",")
                filtered_log = attributes_filter.apply_trace_attribute(log, variant_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "variant", attributes_filter.Parameters.POSITIVE: True})
            log = filtered_log
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)
            


        elif attrFrame == 'caseID':
            # print("attr caseID")
            caseId_list=list_attr.split(",")
            # log = xes_importer.apply(log_path)
            variants = variants_filter.get_variants(log) 
            filtered_log = attributes_filter.apply_trace_attribute(log, caseId_list, parameters={attributes_filter.Parameters.ATTRIBUTE_KEY: "concept:name", attributes_filter.Parameters.POSITIVE: True})
            log = filtered_log  
            
            dataframe = pm4py.convert_to_dataframe(filtered_log)
            session["log"]= dataframe.to_dict()
            dataframe1=pd.DataFrame(session["log"])
            log = pm4py.convert_to_event_log(dataframe1)     
                 

    

    variants = variants_filter.get_variants(log)

    session["boolean_case"]=True

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
    
    
    
    alternative_variants_array=[]
    
    #result = str(f)+"|||"+str(p)+"|||"+str(variantsDict)
    result = str(f)+"|||"+str(p)
    
    return result   

    # ******************************************************************************
    # ******************************************************************************

@app_filter.route('/swipeRemoveAction', methods=['GET', 'POST'])
def swipeRemoveAction():
    # global log
    # global log_duplicate

    # log=log_duplicate

    # dataframe1=pd.DataFrame(session["log"])
    # log = pm4py.convert_to_event_log(dataframe1)

    dataframe_duplicate=pd.DataFrame(session["log_duplicate"])
    log_duplicate= pm4py.convert_to_event_log(dataframe_duplicate)

    log=log_duplicate
    
    dataframe = pm4py.convert_to_dataframe(log)
    session["log"]= dataframe.to_dict()
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)

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

    #return grafo_frequency+"£"+grafo_performance+"£"+varianti+"£"+str(alternative_variants_array)
    return grafo_frequency+"£"+grafo_performance

