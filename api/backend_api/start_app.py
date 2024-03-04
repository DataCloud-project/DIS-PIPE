############################################################
#____________________ALL IMPORT SECTION____________________#
############################################################
from flask import Blueprint, render_template, session,abort
import flask
import os
import sys
import requests
import json
import simplejson
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
import xml.etree.ElementTree as ET
import re
from datetime import datetime, timedelta

############################################################
#______________________START API______________________#
############################################################

def process_string(input_string):
    split_string = input_string.split("api/", 1)
    if len(split_string) > 1:
        result = split_string[1]
        return result
    else:
        if(input_string.startswith("/storage")):
            return input_string[1:]
        else:
            return input_string
            


def translate_format(input_format):
    # Define a mapping of format characters to their corresponding regular expression patterns
    format_mapping = {
        '%Y': r'\d{4}',
        '%m': r'\d{2}',
        '%d': r'\d{2}',
        '%T': r'T',
        '%H': r'\d{2}',
        '%M': r'\d{2}',
        '%S': r'\d{2}',
        '%f': r'\d{1,6}',
        '%z': r'\+\d{2}:\d{2}',
    }
    
    # Replace each format character with its corresponding regular expression pattern
    for format_char, pattern in format_mapping.items():
        input_format = input_format.replace(format_char, pattern)
    
    # Add anchors to make sure the pattern matches the entire string
    output_pattern = f'^{input_format}$'
    
    return output_pattern



def validate_date_format(input_date):
    timestamp_formats = {
        '%Y-%m-%dT%H:%M:%S.%f%z': 'timestamp_format1',
        '%Y-%m-%dT%H:%M:%S%z': 'timestamp_format2',
        '%Y-%m-%dT%H:%M:%S.%f': 'timestamp_format3',
        '%Y-%m-%dT%H:%M:%S': 'timestamp_format4'
    }

    for format_str, format_name in timestamp_formats.items():
        try:
            datetime.strptime(input_date, format_str)
            return True, format_str
        except ValueError:
            pass
    return False, None


def add_leading_zeros(s):
    if len(s) == 1:
        return "0" + s
    elif "1" in s or "7" in s or "17" in s:
        return s.zfill(2)
    else:
        return s


def is_valid_datetime_string(pattern1, input_str):
    return re.match(pattern1, input_str) is not None


def pad_string(input_str, reference_str):
    if len(input_str) < len(reference_str):
        padding = reference_str[len(input_str):]
        padded_input_str = input_str + padding
        return padded_input_str, True
    elif len(input_str) > len(reference_str):
        return reference_str, True
    else:
        return input_str, False


def timeParser(filepath):
    tree = ET.parse(filepath)
    root = tree.getroot()

    change = False
    pattern = ""

    dict_tempo_temp={}
    temp_i=0
    InizioString=""
    timereset="1970-01-01T01:00:00.000"
    pattern1=""

    preStringList=['{http://www.xes-standard.org/}','','{http://www.xes-standard.org}','{https://www.xes-standard.org/}','{https://www.xes-standard.org}','{www.xes-standard.org/}','{www.xes-standard.org}']
    while(len(dict_tempo_temp)==0):
        for preString in preStringList:
            temp_i=0    
            for item in root.findall(preString+'trace'):
                for child in item.findall(preString+'event'):
                    for child2 in child.findall(preString+'date'):
                        try:
                            dataTempo = child2.get("value")

                            risultato, timestamp_f = validate_date_format(dataTempo)
                            
                            InizioString=preString
                           
                            new_date = "1970-01-01T01:00:00"
                            micsecond=""
                            try:
                                micsecond = (dataTempo.split(".")[1]).split("+")[0]
                                micsecond = '7' * len(micsecond)
                                micsecond = "."+micsecond
                            except Exception as e:
                                micsecond=""

                            timezone=""
                            try:
                                timezone = dataTempo.split("+")[1]
                                timezone = "+"+timezone
                            except Exception as e:
                                timezone=""

                            timereset=new_date+micsecond+timezone
                            pattern1=translate_format(timestamp_f)
                            temp_i=temp_i+1 
                            dict_tempo_temp[str(temp_i)]=child
                        except Exception as e:
                            print("error at the beginning")
                        if(len(dict_tempo_temp)>0):
                            break
                    if(len(dict_tempo_temp)>0):
                        break
                if(len(dict_tempo_temp)>0):
                    break
            
    change = False
    for item in root.findall(InizioString+'trace'):
        old_time = timereset
        for child in item.findall(InizioString+'event'):
            for child2 in child.findall(InizioString+'date'):
                
                
                temp_nuovo, is_padded =pad_string(child2.get("value"), old_time)
                if(is_padded):
                    change = True
                child2.set("value",temp_nuovo)
                date_obj=datetime.fromisoformat(child2.get("value"))
                
                if((re.match(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}.\d{2}:\d{2}$", child2.get("value")) is not None) or (re.match(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}$",child2.get("value"))) is not None):
                    print("first if timeParser()")
                    dataTempo_less = (date_obj.isoformat().split("+")[0]).split(".")[0]

                    micsecond=""
                    try:
                        micsecond = (old_time.split(".")[1]).split("+")[0]
                        micsecond = '7' * len(micsecond)
                        micsecond = "."+micsecond
                    except Exception as e:
                        micsecond=""

                    timezone=""
                    try:
                        timezone = child2.get("value").split("+")[1]
                        timezone = "+"+timezone
                    except Exception as e:
                        timezone=""

                    dataTempo=dataTempo_less+micsecond+timezone
                else:    
                    dataTempo = child2.get("value")


                #print(pattern1)
                #print(dataTempo)


                if is_valid_datetime_string(pattern1,dataTempo):
                    old_time = child2.get("value")
                else:
                    
                    original_time = datetime.fromisoformat(old_time)
                    new_time = original_time + timedelta(seconds=1)
                    new_time = (new_time.isoformat().split("+")[0]).split(".")[0]

                    micsecond=""
                    try:
                        micsecond = (old_time.split(".")[1]).split("+")[0]
                        micsecond = '7' * len(micsecond)
                        micsecond = "."+micsecond
                    except Exception as e:
                        micsecond=""

                    timezone=""
                    try:
                        timezone = old_time.split("+")[1]
                        timezone = "+"+timezone
                    except Exception as e:
                        timezone=""

                    timenew=new_time+micsecond+timezone

                    child2.set("value", timenew)
                    old_time = timenew
                    change = True
                    

    #print("IL CHANGE è in timeparser")
    #print(change)
    # Remove the namespace prefix from the tags
    if(change):
        print("save new xes")
        for elem in root.iter():
            if '}' in elem.tag:
                elem.tag = elem.tag.split('}', 1)[1]

        # Save the modified XML to a new file without namespace prefixes
        tree.write(filepath, default_namespace="")

    #InizioString, timereset
    checkTimePrec(filepath, InizioString, timereset )



def checkTimePrec(filepath, InizioString, timereset):
    tree = ET.parse(filepath)
    root = tree.getroot()
    #print(type(root))

    change = False
    for item in root.findall(InizioString+'trace'):
        old_time = timereset
        for child in item.findall(InizioString+'event'):
            for child2 in child.findall(InizioString+'date'):
                dataTempo = child2.get("value")

                #print(old_time)
                #print(dataTempo)
                
                if datetime.fromisoformat(old_time) > datetime.fromisoformat(dataTempo):
                    original_time = datetime.fromisoformat(old_time)
                    new_time = original_time + timedelta(seconds=1)
                    new_time = (new_time.isoformat().split("+")[0]).split(".")[0]

                    micsecond=""
                    try:
                        micsecond = (old_time.split(".")[1]).split("+")[0]
                        micsecond = '7' * len(micsecond)
                        micsecond = "."+micsecond
                    except Exception as e:
                        micsecond=""

                    timezone=""
                    try:
                        timezone = old_time.split("+")[1]
                        timezone = "+"+timezone
                    except Exception as e:
                        timezone=""
                        print(timezone)

                    timeprec=new_time+micsecond+timezone

                    child2.set("value", timeprec)
                    old_time = timeprec
                    change = True
                else:
                    old_time= dataTempo

                #print("#################################################")

    #print("IL CHANGE è in time precedence")
    #print(change)
    # Remove the namespace prefix from the tags
    if(change):
        for elem in root.iter():
            if '}' in elem.tag:
                elem.tag = elem.tag.split('}', 1)[1]

        # Save the modified XML to a new file without namespace prefixes
        tree.write(filepath, default_namespace="")

app_start = Blueprint('app_start',__name__)


@app_start.route('/initialAction', methods=['GET', 'POST'])
def initialAction():

    if(session["log_name"].endswith(".csv")):
        try:
            dataframe1 = pd.read_csv(session["log_path"], sep=',')
            dataframe1 = pm4py.format_dataframe(dataframe1, case_id='case:concept:name', activity_key='concept:name', timestamp_key='time:timestamp')
            event_log = pm4py.convert_to_event_log(dataframe1)
            new_log_path=session["log_path"][:-3]+"xes"
            pm4py.write_xes(event_log, new_log_path)
            session["log_path"]=new_log_path
            session["log_name"]=session["log_name"][:-3]+"xes"
        except:
            print("Invalid value provided.")

    
    try:
        timeParser(process_string(session["log_path"]))
    except Exception as e:
        print(f"An error occurred during time parser: {str(e)}")
    
    

    '''
    try:    
        checkTimePrec(process_string(session["log_path"]),InizioString, timeReset)
    except Exception as e:
        print(f"An error occurred during time precedence: {str(e)}")
    '''

    log_clone = xes_importer.apply(session["log_path"])

    log_clone = interval_lifecycle.assign_lead_cycle_time(log_clone, parameters={
                                                            constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
                                                            constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dataframe = pm4py.convert_to_dataframe(log_clone).replace(np.nan, 'None')

    session["log"]= (dataframe.to_dict())
    session["log_duplicate"] = (dataframe.to_dict())
    
    

    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    
    #print(log_clone==log)
    

    dfg=session["dfg"]
    

    dfg, start_activities, end_activities = pm4py.discover_dfg(log)
    session["dfg"]=dfg
    parameters = dfg_visualization.Variants.FREQUENCY.value.Parameters
    
    gviz_freq = dfg_visualization.apply(dfg, log=log, variant=dfg_visualization.Variants.FREQUENCY,
                                            parameters={parameters.FORMAT: "svg", parameters.START_ACTIVITIES: start_activities,
                                                parameters.END_ACTIVITIES: end_activities})

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
        try:
            mean_dizionario[j]=statistics.mean(activity_dictionary[j])
        except TypeError as e:
            test_activity_dictionary = [float(x) for x in activity_dictionary[j]]
            mean_dizionario[j]=statistics.mean(test_activity_dictionary)
        except Exception as e:
            print(f"initialAction() An error of type {type(e).__name__} occurred: {e}")
            print(activity_dictionary[j])

    total_dizionario=dict.fromkeys(session["activity_list"])
    for j in session["activity_list"]:
        try:
            total_dizionario[j]=sum(activity_dictionary[j])
        except TypeError as e:
            test_activity_dictionary = [float(x) for x in activity_dictionary[j]]
            total_dizionario[j]=sum(test_activity_dictionary)
        except Exception as e:
            print(f"initialAction() An error of type {type(e).__name__} occurred: {e}")
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

    activity_durata=(str(mean_dizionario)+"*"+str(total_dizionario)+"*"+str(median_dizionario)+"*"+str(max_dizionario)+"*"+str(min_dizionario))
    #fine all duration_______________________________________________________________________________________________________________________

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

    durata_edge=(str(mean_edge_dizionario)+"*"+str(total_edge_dizionario)+"*"+str(median_edge_dizionario)+"*"+str(max_edge_dizionario)+"*"+str(min_edge_dizionario))
    

    #___________________________________________________________________________



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


    activity_frequency= str(dictionary_absolute)+"*"+str(dictionary_case)+"*"+str(dictionary_max)

    #___________________________________________________________________________



    
    dfg, start_activities, end_activities = pm4py.discover_dfg(log)

    #ABSOLUTE FREQUENCY
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
    
    alternative_variants_array=[]
    
   
    variantsDict=jsonify({"variants": str(variants)})  
    varianti=(variantsDict)
    #print(varianti)

    #variants fine_______________________________________________________________________________________________________________________




    session["dfg"]=dfg
    #___________________________________________________________________________

    #return jsonify({"grafo_frequency": grafo_frequency, "grafo_performance":grafo_performance, "variabili_usare":variabili_usare, "activity_durata":activity_durata,  "alternative_variants_array":str(alternative_variants_array)})
    return grafo_frequency+"£"+grafo_performance+"£"+variabili_usare+"£"+activity_durata+"£"+durata_edge+"£"+activity_frequency+"£"+frequency_edge



@app_start.route('/initialVariantAction', methods=['GET', 'POST'])
def initialVariantAction():
    #global log
    #global log_duplicate
    
    dataframe1=pd.DataFrame(session["log"])
    log = pm4py.convert_to_event_log(dataframe1)
    #log = interval_lifecycle.assign_lead_cycle_time(log, parameters={
    #                                                        constants.PARAMETER_CONSTANT_START_TIMESTAMP_KEY: "start_timestamp",
    #                                                        constants.PARAMETER_CONSTANT_TIMESTAMP_KEY: "time:timestamp"})

    dataframe_duplicate=pd.DataFrame(session["log_duplicate"])
    log_duplicate= pm4py.convert_to_event_log(dataframe_duplicate)
    
    variants = variants_filter.get_variants(log)
    
    alternative_variants_array=[]
    
    response_string = json.dumps(variants, cls=MyJSONEncoder)
    response_json=json.loads(response_string, object_hook=object_hook)

    return response_json