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

app_durFreq= Blueprint('app_durFreq',__name__)

############################################################
#__________________DURATION FREQUENCY API__________________#
############################################################

@app_durFreq.route('/allduration', methods=['GET'])
def allduration():
 
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    #global activity_list
    session["activity_list"] = []
    for i in range(0, len(log)):
        for j in range(0, len(log[i])):
            if(log[i][j]['concept:name'] not in session["activity_list"]):
                session["activity_list"].append(log[i][j]['concept:name'])

    # global activity_dictionary
    activity_dictionary=dict.fromkeys(session["activity_list"])
    
    for a in session["activity_list"]:
        activity_dictionary[a]=[]
    
    for i in range(0, len(log)):
        for p in range(0, len(log[i])):
            activity_dictionary[log[i][p]['concept:name']].append(log[i][p]['@@duration'])


    mean_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        #mean_dizionario[j]=statistics.mean(activity_dictionary[j])
        try:
            mean_dizionario[j]=statistics.mean(activity_dictionary[j])
        except TypeError as e:
            test_activity_dictionary = [float(x) for x in activity_dictionary[j]]
            mean_dizionario[j]=statistics.mean(test_activity_dictionary)
        except Exception as e:
            print(f"An error of type {type(e).__name__} occurred: {e}")
            print(activity_dictionary[j])

        
    total_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        #total_dizionario[j]=sum(activity_dictionary[j])
        try:
            total_dizionario[j]=sum(activity_dictionary[j])
        except TypeError as e:
            test_activity_dictionary = [float(x) for x in activity_dictionary[j]]
            total_dizionario[j]=sum(test_activity_dictionary)
        except Exception as e:
            print(f"An error of type {type(e).__name__} occurred: {e}")
            print(activity_dictionary[j])

    median_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        median_dizionario[j]=activity_dictionary[j][int(len(activity_dictionary[j])/2)]
        
    max_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        max_dizionario[j]=max(activity_dictionary[j])
        
        
    min_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        min_dizionario[j]=min(activity_dictionary[j])

    return str(mean_dizionario)+"*"+str(total_dizionario)+"*"+str(median_dizionario)+"*"+str(max_dizionario)+"*"+str(min_dizionario)

@app_durFreq.route('/alledgeduration', methods=['GET'])
def alledgeduration():
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})
    #global activity_list
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    follower_dictionary={}
    follower_list=[]
    follower_used_list=[]
    import datetime
    
    already_sum=[]

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

    return str(mean_edge_dizionario)+"*"+str(total_edge_dizionario)+"*"+str(median_edge_dizionario)+"*"+str(max_edge_dizionario)+"*"+str(min_edge_dizionario)

@app_durFreq.route('/allfrequency', methods=['GET'])
def allfrequency():
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
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

@app_durFreq.route('/alledgefrequency', methods=['GET'])
def alledgefrequency():
    # global log
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    # log = xes_importer.apply(log_path)
    # log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                         constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                         constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #absolute frequency
    follower_dictionary_absolute={}
    

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
